/**
 * Roxy RedLight v2.0 - Dual Wavelength PBM Helmet
 *
 * ESP32-C3 firmware for photobiomodulation hair therapy device
 *
 * Features:
 * - Dual-channel PWM (650nm red + 850nm NIR)
 * - Multiple treatment modes
 * - Battery monitoring with low-voltage protection
 * - Session timing with auto-shutoff
 * - Persistent session counter
 *
 * Hardware:
 * - ESP32-C3 (Seeed XIAO or similar)
 * - 60x 650nm red LEDs via MOSFET on GPIO3
 * - 30x 850nm NIR LEDs via MOSFET on GPIO5
 * - 2S 18650 battery pack (6.0-8.4V)
 * - Tactile button on GPIO4
 */

#include <Arduino.h>
#include <Preferences.h>
#include <TFT_eSPI.h>
#include "config.h"
#include "display.h"

// =============================================================================
// GLOBAL STATE
// =============================================================================

Preferences prefs;

// Current state
volatile bool sessionActive = false;
volatile TreatmentMode currentMode = DEFAULT_MODE;
volatile uint8_t brightness = 255;  // 0-255

// Session tracking
unsigned long sessionStartTime = 0;
unsigned long totalSessionSeconds = 0;
uint32_t lifetimeSessions = 0;
uint32_t lifetimeMinutes = 0;

// Button state (two buttons on T-Display S3)
volatile bool button1Pressed = false;
volatile bool button2Pressed = false;
unsigned long button1PressTime = 0;
unsigned long button2PressTime = 0;
bool button1Handled = false;
bool button2Handled = false;

// Menu state
int menuSelectedIndex = 0;
unsigned long lastDisplayUpdate = 0;
#define DISPLAY_UPDATE_INTERVAL 100  // ms

// Battery
float batteryVoltage = 0.0;
bool lowBatteryWarning = false;
bool overVoltageError = false;

// Thermal (optional)
float temperature = 0.0;
bool thermalWarning = false;

// Safety tracking
uint8_t dailySessionCount = 0;
unsigned long lastSessionEndTime = 0;
unsigned long dayStartTime = 0;

// Alternating mode state
unsigned long lastAlternateTime = 0;
bool alternatePhase = false;  // false = red, true = NIR

// =============================================================================
// FUNCTION PROTOTYPES
// =============================================================================

void setupPWM();
void setupButton();
void setupBattery();
void loadPreferences();
void savePreferences();

void setLEDs(uint8_t red, uint8_t nir);
void applyMode(TreatmentMode mode);
void updateAlternating();

void startSession();
void stopSession();
void cycleMode();

float readBatteryVoltage();
void checkBattery();
float readTemperature();
void checkThermal();
bool checkSafetyLimits();
void emergencyShutdown(const char* reason);

void blinkStatus(int count, int onTime, int offTime);
void playTone(int freq, int duration);

void updateDisplay();
void handleButtons();
void IRAM_ATTR button1ISR();
void IRAM_ATTR button2ISR();

// =============================================================================
// SETUP
// =============================================================================

void setup() {
    Serial.begin(115200);
    delay(100);

    Serial.println();
    Serial.println("=================================");
    Serial.println("  Roxy RedLight v2.0");
    Serial.println("  Dual Wavelength PBM Helmet");
    Serial.println("  LilyGO T-Display S3");
    Serial.println("=================================");
    Serial.println();

    // Keep power on (T-Display S3 specific)
    pinMode(PIN_POWER_ON, OUTPUT);
    digitalWrite(PIN_POWER_ON, HIGH);

    // Initialize display first
    display.begin();
    display.showAlert("FOLICULATOR", "Initializing...", COLOR_GREEN);
    delay(500);

    // Initialize hardware
    setupPWM();
    setupButton();
    setupBattery();

    // Buzzer (optional)
    pinMode(PIN_BUZZER, OUTPUT);
    digitalWrite(PIN_BUZZER, LOW);

    // Load saved data
    loadPreferences();

    // Initial battery check
    batteryVoltage = readBatteryVoltage();
    Serial.printf("Battery: %.2fV\n", batteryVoltage);
    Serial.printf("Lifetime sessions: %lu\n", lifetimeSessions);
    Serial.printf("Lifetime minutes: %lu\n", lifetimeMinutes);
    Serial.printf("Current mode: %d\n", currentMode);

    // Startup indication
    playTone(TONE_START, 100);

    // Ensure LEDs are off
    setLEDs(0, 0);

    // Show home screen
    display.setScreen(SCREEN_HOME);
    dayStartTime = millis();  // Initialize daily counter

    Serial.println("Ready. Press button to start session.");
    Serial.println();
}

// =============================================================================
// MAIN LOOP
// =============================================================================

void loop() {
    // Handle button presses
    handleButtons();

    // Session active logic
    if (sessionActive) {
        // Update alternating mode
        if (currentMode == MODE_ALTERNATING) {
            updateAlternating();
        }

        // Check session duration
        unsigned long elapsed = (millis() - sessionStartTime) / 1000;
        unsigned long targetSeconds = DEFAULT_SESSION_MINUTES * 60;

        if (elapsed >= targetSeconds) {
            // Session complete
            Serial.println("Session complete!");
            playTone(TONE_COMPLETE, 500);
            delay(200);
            playTone(TONE_COMPLETE, 500);
            stopSession();
            display.showAlert("COMPLETE", "Session finished!", COLOR_GREEN);
            delay(2000);
        }

        // Safety: max session limit
        if (elapsed >= MAX_SESSION_MINUTES * 60) {
            Serial.println("Max session time reached - safety shutoff");
            stopSession();
        }

        // Print progress every 30 seconds
        static unsigned long lastProgress = 0;
        if (millis() - lastProgress > 30000) {
            unsigned long remaining = targetSeconds - elapsed;
            Serial.printf("Session: %lu:%02lu elapsed, %lu:%02lu remaining\n",
                         elapsed / 60, elapsed % 60,
                         remaining / 60, remaining % 60);
            lastProgress = millis();
        }
    }

    // Update display periodically
    if (millis() - lastDisplayUpdate > DISPLAY_UPDATE_INTERVAL) {
        updateDisplay();
        lastDisplayUpdate = millis();
    }

    // Battery monitoring (every 5 seconds)
    static unsigned long lastBatteryCheck = 0;
    if (millis() - lastBatteryCheck > 5000) {
        checkBattery();
        lastBatteryCheck = millis();
    }

    // Thermal monitoring (every 2 seconds, if enabled)
    #if TEMP_ENABLED
    static unsigned long lastThermalCheck = 0;
    if (millis() - lastThermalCheck > 2000) {
        checkThermal();
        lastThermalCheck = millis();
    }
    #endif

    delay(10);  // Small delay to prevent tight loop
}

// =============================================================================
// DISPLAY UPDATE
// =============================================================================

void updateDisplay() {
    uint8_t battPercent = (uint8_t)constrain(
        (batteryVoltage - VBAT_CUTOFF) / (VBAT_FULL - VBAT_CUTOFF) * 100, 0, 100);

    Screen screen = display.getScreen();

    if (sessionActive) {
        // Always show session screen when active
        unsigned long elapsed = (millis() - sessionStartTime) / 1000;
        unsigned long total = DEFAULT_SESSION_MINUTES * 60;
        bool redOn = (currentMode == MODE_RED_ONLY || currentMode == MODE_DUAL ||
                     (currentMode == MODE_ALTERNATING && !alternatePhase));
        bool nirOn = (currentMode == MODE_NIR_ONLY || currentMode == MODE_DUAL ||
                     (currentMode == MODE_ALTERNATING && alternatePhase));
        display.showSession(elapsed, total, currentMode, redOn, nirOn);
        return;
    }

    // Idle screens
    switch (screen) {
        case SCREEN_HOME:
            display.showHome(batteryVoltage, battPercent, currentMode);
            break;

        case SCREEN_STATS:
            display.showStats(lifetimeSessions, lifetimeMinutes, dailySessionCount);
            break;

        case SCREEN_SETTINGS:
            display.showSettings(currentMode, menuSelectedIndex);
            break;

        case SCREEN_BATTERY:
            display.showBattery(batteryVoltage, battPercent, false);
            break;

        case SCREEN_SAFETY:
            display.showSafety(batteryVoltage, temperature,
                              overVoltageError, batteryVoltage < VBAT_CUTOFF, thermalWarning);
            break;

        default:
            display.showHome(batteryVoltage, battPercent, currentMode);
            break;
    }
}

// =============================================================================
// BUTTON HANDLING (Two buttons on T-Display S3)
// =============================================================================

void handleButtons() {
    Screen screen = display.getScreen();

    // Button 1 (GPIO0) - Primary action / Navigate left
    if (button1Pressed && !button1Handled) {
        if (digitalRead(PIN_BUTTON_1) == HIGH) {
            // Button released
            unsigned long pressDuration = millis() - button1PressTime;

            if (sessionActive) {
                // During session: stop
                stopSession();
            } else if (screen == SCREEN_HOME) {
                // Home screen: start session
                if (pressDuration < BUTTON_LONG_PRESS_MS) {
                    startSession();
                    display.setScreen(SCREEN_SESSION);
                }
            } else if (screen == SCREEN_SETTINGS) {
                // Settings: navigate up or back
                if (menuSelectedIndex > 0) {
                    menuSelectedIndex--;
                } else {
                    display.setScreen(SCREEN_HOME);
                }
            } else {
                // Other screens: go back/previous
                display.prevScreen();
            }

            button1Pressed = false;
            button1Handled = true;
            playTone(1000, 50);
        }
    }

    // Button 2 (GPIO14) - Secondary action / Navigate right
    if (button2Pressed && !button2Handled) {
        if (digitalRead(PIN_BUTTON_2) == HIGH) {
            // Button released

            if (sessionActive) {
                // During session: no action (or could toggle mode)
            } else if (screen == SCREEN_HOME) {
                // Home screen: go to menu
                display.setScreen(SCREEN_STATS);
            } else if (screen == SCREEN_SETTINGS) {
                // Settings: navigate down or select
                if (menuSelectedIndex < 3) {
                    menuSelectedIndex++;
                } else {
                    // Select current mode
                    currentMode = (TreatmentMode)(menuSelectedIndex + 1);
                    savePreferences();
                    display.setScreen(SCREEN_HOME);
                    Serial.printf("Mode changed to: %d\n", currentMode);
                }
            } else {
                // Other screens: go to next
                display.nextScreen();
            }

            button2Pressed = false;
            button2Handled = true;
            playTone(1200, 50);
        }
    }

    // Reset handled state when released
    if (digitalRead(PIN_BUTTON_1) == HIGH) button1Handled = false;
    if (digitalRead(PIN_BUTTON_2) == HIGH) button2Handled = false;
}

// =============================================================================
// PWM SETUP AND CONTROL
// =============================================================================

void setupPWM() {
    // Configure LEDC for red channel
    ledcSetup(PWM_CHANNEL_RED, PWM_FREQ, PWM_RESOLUTION);
    ledcAttachPin(PIN_RED_LED, PWM_CHANNEL_RED);
    ledcWrite(PWM_CHANNEL_RED, 0);

    // Configure LEDC for NIR channel
    ledcSetup(PWM_CHANNEL_NIR, PWM_FREQ, PWM_RESOLUTION);
    ledcAttachPin(PIN_NIR_LED, PWM_CHANNEL_NIR);
    ledcWrite(PWM_CHANNEL_NIR, 0);

    Serial.println("PWM initialized (dual channel)");
}

void setLEDs(uint8_t red, uint8_t nir) {
    ledcWrite(PWM_CHANNEL_RED, red);
    ledcWrite(PWM_CHANNEL_NIR, nir);
}

void applyMode(TreatmentMode mode) {
    switch (mode) {
        case MODE_OFF:
            setLEDs(0, 0);
            break;
        case MODE_RED_ONLY:
            setLEDs(brightness, 0);
            break;
        case MODE_NIR_ONLY:
            setLEDs(0, brightness);
            break;
        case MODE_DUAL:
            setLEDs(brightness, brightness);
            break;
        case MODE_ALTERNATING:
            // Handled in updateAlternating()
            if (alternatePhase) {
                setLEDs(0, brightness);
            } else {
                setLEDs(brightness, 0);
            }
            break;
        default:
            setLEDs(0, 0);
            break;
    }
}

void updateAlternating() {
    if (millis() - lastAlternateTime >= ALTERNATE_PERIOD_SEC * 1000) {
        alternatePhase = !alternatePhase;
        lastAlternateTime = millis();

        if (alternatePhase) {
            setLEDs(0, brightness);
            Serial.println("Alternating: NIR phase");
        } else {
            setLEDs(brightness, 0);
            Serial.println("Alternating: RED phase");
        }
    }
}

// =============================================================================
// SESSION CONTROL
// =============================================================================

void startSession() {
    // Run comprehensive safety checks
    if (!checkSafetyLimits()) {
        return;  // Safety check failed, reason already logged
    }

    sessionActive = true;
    dailySessionCount++;
    sessionStartTime = millis();
    lastAlternateTime = millis();
    alternatePhase = false;

    applyMode(currentMode);

    lifetimeSessions++;
    savePreferences();

    const char* modeNames[] = {"OFF", "RED", "NIR", "DUAL", "ALT"};
    Serial.printf("Session started - Mode: %s, Duration: %d min\n",
                 modeNames[currentMode], DEFAULT_SESSION_MINUTES);

    playTone(TONE_START, 200);
    digitalWrite(PIN_STATUS_LED, HIGH);
}

void stopSession() {
    sessionActive = false;
    lastSessionEndTime = millis();  // Track for session gap enforcement

    // Calculate session duration
    unsigned long elapsed = (millis() - sessionStartTime) / 1000;
    totalSessionSeconds += elapsed;

    // Update lifetime minutes
    lifetimeMinutes += elapsed / 60;
    savePreferences();

    // Turn off LEDs
    setLEDs(0, 0);

    Serial.printf("Session stopped. Duration: %lu:%02lu\n",
                 elapsed / 60, elapsed % 60);
    Serial.printf("Lifetime: %lu sessions, %lu minutes\n",
                 lifetimeSessions, lifetimeMinutes);
    Serial.printf("Daily sessions: %d/%d\n", dailySessionCount, MAX_DAILY_SESSIONS);

    playTone(TONE_STOP, 200);
    digitalWrite(PIN_STATUS_LED, LOW);
}

void cycleMode() {
    currentMode = (TreatmentMode)((currentMode + 1) % MODE_COUNT);
    if (currentMode == MODE_OFF) {
        currentMode = MODE_RED_ONLY;  // Skip OFF mode in cycle
    }

    savePreferences();

    const char* modeNames[] = {"OFF", "RED", "NIR", "DUAL", "ALT"};
    Serial.printf("Mode changed to: %s\n", modeNames[currentMode]);

    // Feedback: blink count indicates mode
    blinkStatus(currentMode, 150, 150);
    playTone(1000 + currentMode * 200, 100);
}

// =============================================================================
// BUTTON HANDLING
// =============================================================================

void setupButton() {
    pinMode(PIN_BUTTON_1, INPUT_PULLUP);
    pinMode(PIN_BUTTON_2, INPUT_PULLUP);
    attachInterrupt(digitalPinToInterrupt(PIN_BUTTON_1), button1ISR, FALLING);
    attachInterrupt(digitalPinToInterrupt(PIN_BUTTON_2), button2ISR, FALLING);
    Serial.println("Buttons initialized (2x on T-Display S3)");
}

void IRAM_ATTR button1ISR() {
    static unsigned long lastInterrupt = 0;
    unsigned long now = millis();

    // Debounce
    if (now - lastInterrupt > BUTTON_DEBOUNCE_MS) {
        button1Pressed = true;
        button1PressTime = now;
        lastInterrupt = now;
    }
}

void IRAM_ATTR button2ISR() {
    static unsigned long lastInterrupt = 0;
    unsigned long now = millis();

    // Debounce
    if (now - lastInterrupt > BUTTON_DEBOUNCE_MS) {
        button2Pressed = true;
        button2PressTime = now;
        lastInterrupt = now;
    }
}

// =============================================================================
// BATTERY MONITORING
// =============================================================================

void setupBattery() {
    pinMode(PIN_VBAT_ADC, INPUT);
    analogReadResolution(12);
    analogSetAttenuation(ADC_11db);  // Full 0-3.3V range
    Serial.println("Battery ADC initialized");
}

float readBatteryVoltage() {
    // Average multiple readings for stability
    long sum = 0;
    for (int i = 0; i < 10; i++) {
        sum += analogRead(PIN_VBAT_ADC);
        delay(1);
    }
    float adcValue = sum / 10.0;

    // Convert to voltage
    float vAdc = (adcValue / VBAT_ADC_MAX) * VBAT_REF_VOLTAGE;
    float vBat = vAdc / VBAT_DIVIDER_RATIO;

    return vBat;
}

void checkBattery() {
    batteryVoltage = readBatteryVoltage();

    // Calculate percentage (linear approximation)
    float percent = (batteryVoltage - VBAT_CUTOFF) / (VBAT_FULL - VBAT_CUTOFF) * 100.0;
    percent = constrain(percent, 0, 100);

    // CRITICAL: Over-voltage protection (wrong charger, damaged BMS)
    if (batteryVoltage > VBAT_OVERVOLTAGE) {
        overVoltageError = true;
        emergencyShutdown("OVER-VOLTAGE DETECTED!");
        Serial.printf("DANGER: Battery voltage %.2fV exceeds safe limit!\n", batteryVoltage);
        Serial.println("Check charger and BMS immediately.");
        return;
    } else {
        overVoltageError = false;
    }

    // CRITICAL: Under-voltage protection
    if (batteryVoltage < VBAT_CUTOFF) {
        emergencyShutdown("UNDER-VOLTAGE - Battery critically low!");
        return;
    }

    // Low battery warning
    if (batteryVoltage < VBAT_LOW && !lowBatteryWarning) {
        lowBatteryWarning = true;
        Serial.printf("WARNING: Low battery! %.2fV (%.0f%%)\n", batteryVoltage, percent);
        playTone(TONE_LOW_BAT, 100);
    } else if (batteryVoltage >= VBAT_LOW) {
        lowBatteryWarning = false;
    }
}

// =============================================================================
// THERMAL MONITORING (Optional - requires NTC thermistor)
// =============================================================================

float readTemperature() {
    #if TEMP_ENABLED
    // 10k NTC thermistor with 10k pullup
    // Using simplified Steinhart-Hart approximation
    int adcValue = analogRead(PIN_TEMP_ADC);
    float resistance = 10000.0 * adcValue / (4095 - adcValue);

    // Simplified B-parameter equation (B=3950 typical for 10k NTC)
    float tempK = 1.0 / (1.0/298.15 + (1.0/3950.0) * log(resistance/10000.0));
    float tempC = tempK - 273.15;

    return tempC;
    #else
    return 25.0;  // Default safe value if not enabled
    #endif
}

void checkThermal() {
    #if TEMP_ENABLED
    temperature = readTemperature();

    // CRITICAL: Thermal cutoff
    if (temperature >= TEMP_CUTOFF_C) {
        emergencyShutdown("THERMAL CUTOFF - Overheating!");
        Serial.printf("DANGER: Temperature %.1fC exceeds safe limit!\n", temperature);
        return;
    }

    // Thermal warning
    if (temperature >= TEMP_WARNING_C && !thermalWarning) {
        thermalWarning = true;
        Serial.printf("WARNING: High temperature! %.1fC\n", temperature);
        playTone(TONE_LOW_BAT, 100);

        // Reduce power to 50% as protective measure
        brightness = 128;
        if (sessionActive) {
            applyMode(currentMode);
            Serial.println("Power reduced to 50% due to temperature.");
        }
    } else if (temperature < TEMP_WARNING_C - 5) {  // 5C hysteresis
        if (thermalWarning) {
            thermalWarning = false;
            brightness = 255;  // Restore full power
            if (sessionActive) {
                applyMode(currentMode);
                Serial.println("Temperature normal - full power restored.");
            }
        }
    }
    #endif
}

// =============================================================================
// COMPREHENSIVE SAFETY CHECK
// =============================================================================

bool checkSafetyLimits() {
    // Check all safety conditions before starting session

    // 1. Battery voltage in safe range
    if (batteryVoltage < VBAT_CUTOFF) {
        Serial.println("BLOCKED: Battery too low");
        playTone(TONE_LOW_BAT, 200);
        return false;
    }

    if (batteryVoltage > VBAT_OVERVOLTAGE) {
        Serial.println("BLOCKED: Battery voltage too high - check charger!");
        playTone(TONE_LOW_BAT, 500);
        return false;
    }

    // 2. Thermal check
    #if TEMP_ENABLED
    if (temperature >= TEMP_CUTOFF_C) {
        Serial.println("BLOCKED: Temperature too high");
        playTone(TONE_LOW_BAT, 200);
        return false;
    }
    #endif

    // 3. Daily session limit (prevent overuse)
    // Reset counter if it's been >24 hours
    if (millis() - dayStartTime > 24UL * 60 * 60 * 1000) {
        dailySessionCount = 0;
        dayStartTime = millis();
    }

    if (dailySessionCount >= MAX_DAILY_SESSIONS) {
        Serial.printf("BLOCKED: Daily limit reached (%d sessions)\n", MAX_DAILY_SESSIONS);
        Serial.println("Rest recommended. Wait 24 hours or power cycle to reset.");
        playTone(TONE_LOW_BAT, 200);
        return false;
    }

    // 4. Minimum gap between sessions
    if (lastSessionEndTime > 0) {
        unsigned long gapMinutes = (millis() - lastSessionEndTime) / 60000;
        if (gapMinutes < MIN_SESSION_GAP_MIN) {
            Serial.printf("BLOCKED: Wait %lu more minutes between sessions\n",
                         MIN_SESSION_GAP_MIN - gapMinutes);
            playTone(TONE_LOW_BAT, 200);
            return false;
        }
    }

    return true;
}

void emergencyShutdown(const char* reason) {
    Serial.println();
    Serial.println("!!! EMERGENCY SHUTDOWN !!!");
    Serial.println(reason);
    Serial.println();

    // Immediately disable all LEDs
    setLEDs(0, 0);
    sessionActive = false;

    // Show emergency screen
    display.showEmergency(reason);

    // Alarm pattern
    for (int i = 0; i < 5; i++) {
        playTone(TONE_LOW_BAT, 200);
        delay(200);
    }
}

// =============================================================================
// PREFERENCES (FLASH STORAGE)
// =============================================================================

void loadPreferences() {
    prefs.begin(PREFS_NAMESPACE, true);  // Read-only

    lifetimeSessions = prefs.getULong(PREFS_KEY_SESSIONS, 0);
    lifetimeMinutes = prefs.getULong(PREFS_KEY_MINUTES, 0);
    currentMode = (TreatmentMode)prefs.getUChar(PREFS_KEY_MODE, DEFAULT_MODE);

    // Validate mode
    if (currentMode >= MODE_COUNT || currentMode == MODE_OFF) {
        currentMode = DEFAULT_MODE;
    }

    prefs.end();
    Serial.println("Preferences loaded");
}

void savePreferences() {
    prefs.begin(PREFS_NAMESPACE, false);  // Read-write

    prefs.putULong(PREFS_KEY_SESSIONS, lifetimeSessions);
    prefs.putULong(PREFS_KEY_MINUTES, lifetimeMinutes);
    prefs.putUChar(PREFS_KEY_MODE, currentMode);

    prefs.end();
}

// =============================================================================
// USER FEEDBACK
// =============================================================================

void blinkStatus(int count, int onTime, int offTime) {
    for (int i = 0; i < count; i++) {
        digitalWrite(PIN_STATUS_LED, HIGH);
        delay(onTime);
        digitalWrite(PIN_STATUS_LED, LOW);
        if (i < count - 1) delay(offTime);
    }
}

void playTone(int freq, int duration) {
    // Simple tone using digitalWrite toggle
    // For proper tone, use ledcWriteTone or external buzzer driver
    #ifdef PIN_BUZZER
    int period = 1000000 / freq;
    int halfPeriod = period / 2;
    unsigned long start = millis();

    while (millis() - start < (unsigned long)duration) {
        digitalWrite(PIN_BUZZER, HIGH);
        delayMicroseconds(halfPeriod);
        digitalWrite(PIN_BUZZER, LOW);
        delayMicroseconds(halfPeriod);
    }
    #endif
}
