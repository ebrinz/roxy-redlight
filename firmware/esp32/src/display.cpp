/**
 * Roxy RedLight v2.0 - Display Implementation
 *
 * UI rendering for LilyGO T-Display S3
 */

#include "display.h"

// Global instance
Display display;

// =============================================================================
// CONSTRUCTOR & INIT
// =============================================================================

Display::Display() : sprite(&tft) {
    currentScreen = SCREEN_HOME;
    needsRedraw = true;
    lastUpdate = 0;
}

void Display::begin() {
    tft.init();
    tft.setRotation(TFT_ROTATION);
    tft.fillScreen(COLOR_BG);

    // Create sprite for flicker-free rendering
    sprite.createSprite(TFT_WIDTH, TFT_HEIGHT);
    sprite.setTextDatum(MC_DATUM);

    // Enable backlight
    pinMode(PIN_TFT_BL, OUTPUT);
    digitalWrite(PIN_TFT_BL, HIGH);

    Serial.println("Display initialized");
}

void Display::clear() {
    sprite.fillSprite(COLOR_BG);
}

void Display::setBrightness(uint8_t level) {
    analogWrite(PIN_TFT_BL, level);
}

// =============================================================================
// SCREEN NAVIGATION
// =============================================================================

void Display::setScreen(Screen screen) {
    if (screen != currentScreen) {
        currentScreen = screen;
        needsRedraw = true;
    }
}

Screen Display::getScreen() {
    return currentScreen;
}

void Display::nextScreen() {
    currentScreen = (Screen)((currentScreen + 1) % SCREEN_COUNT);
    needsRedraw = true;
}

void Display::prevScreen() {
    if (currentScreen == 0) {
        currentScreen = (Screen)(SCREEN_COUNT - 1);
    } else {
        currentScreen = (Screen)(currentScreen - 1);
    }
    needsRedraw = true;
}

void Display::update() {
    sprite.pushSprite(0, 0);
}

// =============================================================================
// SCREEN: HOME (Idle)
// =============================================================================

void Display::showHome(float voltage, uint8_t battPercent, TreatmentMode mode) {
    clear();

    drawHeader("FOLICULATOR");

    // Battery icon and percentage
    drawBatteryIcon(TFT_WIDTH - 45, 8, battPercent, false);

    // Large mode display in center
    sprite.setTextSize(1);
    sprite.setTextFont(4);
    sprite.setTextColor(COLOR_TEXT, COLOR_BG);
    sprite.setTextDatum(MC_DATUM);

    const char* modeNames[] = {"OFF", "RED", "NIR", "DUAL", "ALT"};
    sprite.drawString(modeNames[mode], TFT_WIDTH/2, 100);

    // Mode description
    sprite.setTextFont(2);
    const char* modeDesc[] = {
        "Disabled",
        "650nm Surface",
        "850nm Deep",
        "Full Spectrum",
        "Alternating"
    };
    sprite.drawString(modeDesc[mode], TFT_WIDTH/2, 130);

    // LED indicator
    bool redOn = (mode == MODE_RED_ONLY || mode == MODE_DUAL || mode == MODE_ALTERNATING);
    bool nirOn = (mode == MODE_NIR_ONLY || mode == MODE_DUAL || mode == MODE_ALTERNATING);
    drawLEDIndicator(TFT_WIDTH/2, 170, redOn, nirOn);

    // Ready text
    sprite.setTextFont(2);
    sprite.setTextColor(COLOR_GREEN, COLOR_BG);
    sprite.drawString("READY", TFT_WIDTH/2, 220);

    sprite.setTextColor(COLOR_TEXT, COLOR_BG);
    sprite.drawString("Press to Start", TFT_WIDTH/2, 245);

    // Footer with battery voltage
    char voltStr[16];
    snprintf(voltStr, sizeof(voltStr), "%.2fV", voltage);
    drawFooter("Menu", voltStr);

    update();
}

// =============================================================================
// SCREEN: SESSION (Active Treatment)
// =============================================================================

void Display::showSession(unsigned long elapsedSec, unsigned long totalSec,
                          TreatmentMode mode, bool redOn, bool nirOn) {
    clear();

    // Calculate remaining time
    unsigned long remainingSec = (totalSec > elapsedSec) ? totalSec - elapsedSec : 0;
    float progress = (float)elapsedSec / totalSec;

    // Header with mode
    const char* modeNames[] = {"OFF", "RED", "NIR", "DUAL", "ALT"};
    char header[32];
    snprintf(header, sizeof(header), "SESSION - %s", modeNames[mode]);
    drawHeader(header);

    // Large countdown timer
    sprite.setTextFont(7);
    sprite.setTextColor(COLOR_GREEN, COLOR_BG);
    sprite.setTextDatum(MC_DATUM);

    char timeStr[16];
    snprintf(timeStr, sizeof(timeStr), "%lu:%02lu",
             remainingSec / 60, remainingSec % 60);
    sprite.drawString(timeStr, TFT_WIDTH/2, 100);

    // "remaining" label
    sprite.setTextFont(2);
    sprite.setTextColor(COLOR_TEXT, COLOR_BG);
    sprite.drawString("remaining", TFT_WIDTH/2, 140);

    // Progress bar
    drawProgressBar(MARGIN, 165, TFT_WIDTH - 2*MARGIN, 20, progress, COLOR_GREEN);

    // Elapsed time
    sprite.setTextFont(2);
    char elapsedStr[32];
    snprintf(elapsedStr, sizeof(elapsedStr), "Elapsed: %lu:%02lu",
             elapsedSec / 60, elapsedSec % 60);
    sprite.drawString(elapsedStr, TFT_WIDTH/2, 200);

    // LED status indicator
    drawLEDIndicator(TFT_WIDTH/2, 240, redOn, nirOn);

    // Footer
    drawFooter("Stop", "");

    update();
}

// =============================================================================
// SCREEN: STATS
// =============================================================================

void Display::showStats(uint32_t sessions, uint32_t minutes, uint8_t dailySessions) {
    clear();

    drawHeader("STATISTICS");

    sprite.setTextFont(2);
    sprite.setTextColor(COLOR_TEXT, COLOR_BG);
    sprite.setTextDatum(TL_DATUM);

    int y = HEADER_HEIGHT + 20;
    int x = MARGIN;

    sprite.drawString("Lifetime Sessions:", x, y);
    sprite.setTextColor(COLOR_GREEN, COLOR_BG);
    char buf[32];
    snprintf(buf, sizeof(buf), "%lu", sessions);
    sprite.drawString(buf, TFT_WIDTH - MARGIN - sprite.textWidth(buf), y);
    y += 30;

    sprite.setTextColor(COLOR_TEXT, COLOR_BG);
    sprite.drawString("Total Minutes:", x, y);
    sprite.setTextColor(COLOR_GREEN, COLOR_BG);
    snprintf(buf, sizeof(buf), "%lu", minutes);
    sprite.drawString(buf, TFT_WIDTH - MARGIN - sprite.textWidth(buf), y);
    y += 30;

    sprite.setTextColor(COLOR_TEXT, COLOR_BG);
    sprite.drawString("Total Hours:", x, y);
    sprite.setTextColor(COLOR_GREEN, COLOR_BG);
    snprintf(buf, sizeof(buf), "%.1f", minutes / 60.0);
    sprite.drawString(buf, TFT_WIDTH - MARGIN - sprite.textWidth(buf), y);
    y += 30;

    sprite.setTextColor(COLOR_TEXT, COLOR_BG);
    sprite.drawString("Today's Sessions:", x, y);
    uint16_t color = (dailySessions >= MAX_DAILY_SESSIONS) ? COLOR_ORANGE : COLOR_GREEN;
    sprite.setTextColor(color, COLOR_BG);
    snprintf(buf, sizeof(buf), "%d/%d", dailySessions, MAX_DAILY_SESSIONS);
    sprite.drawString(buf, TFT_WIDTH - MARGIN - sprite.textWidth(buf), y);
    y += 30;

    // Estimated dose
    sprite.setTextColor(COLOR_TEXT, COLOR_BG);
    sprite.drawString("Est. Total Dose:", x, y);
    sprite.setTextColor(COLOR_GREEN, COLOR_BG);
    float joules = minutes * 60 * 0.005;  // 5mW/cm² * seconds
    snprintf(buf, sizeof(buf), "%.0f J/cm2", joules);
    sprite.drawString(buf, TFT_WIDTH - MARGIN - sprite.textWidth(buf), y);

    drawFooter("<", ">");

    update();
}

// =============================================================================
// SCREEN: SETTINGS
// =============================================================================

void Display::showSettings(TreatmentMode mode, int selectedIndex) {
    clear();

    drawHeader("SELECT MODE");

    const char* modeNames[] = {"RED ONLY", "NIR ONLY", "DUAL", "ALTERNATING"};
    const char* modeDesc[] = {
        "650nm - Surface treatment",
        "850nm - Deep follicles",
        "Both - Comprehensive",
        "30s alternating cycle"
    };

    int y = HEADER_HEIGHT + 20;

    for (int i = 0; i < 4; i++) {
        TreatmentMode m = (TreatmentMode)(i + 1);  // Skip MODE_OFF

        // Highlight selected
        if (i == selectedIndex) {
            sprite.fillRoundRect(MARGIN - 5, y - 5, TFT_WIDTH - 2*MARGIN + 10, 50, 5, 0x2104);
        }

        // Checkmark for current mode
        if (m == mode) {
            sprite.setTextColor(COLOR_GREEN, (i == selectedIndex) ? 0x2104 : COLOR_BG);
            sprite.drawString("*", MARGIN, y + 10);
        }

        sprite.setTextFont(2);
        sprite.setTextColor(COLOR_TEXT, (i == selectedIndex) ? 0x2104 : COLOR_BG);
        sprite.setTextDatum(TL_DATUM);
        sprite.drawString(modeNames[i], MARGIN + 15, y);

        sprite.setTextFont(1);
        sprite.setTextColor(0x8410, (i == selectedIndex) ? 0x2104 : COLOR_BG);  // Gray
        sprite.drawString(modeDesc[i], MARGIN + 15, y + 22);

        y += 55;
    }

    drawFooter("Back", "Select");

    update();
}

// =============================================================================
// SCREEN: BATTERY
// =============================================================================

void Display::showBattery(float voltage, uint8_t percent, bool charging) {
    clear();

    drawHeader("BATTERY");

    // Large battery icon
    int iconX = TFT_WIDTH/2 - 30;
    int iconY = 70;
    int iconW = 60;
    int iconH = 100;

    // Battery outline
    sprite.drawRoundRect(iconX, iconY, iconW, iconH, 5, COLOR_TEXT);
    sprite.fillRect(iconX + 20, iconY - 8, 20, 10, COLOR_TEXT);  // Top nub

    // Fill level
    int fillH = (iconH - 10) * percent / 100;
    uint16_t fillColor = (percent > 50) ? COLOR_GREEN :
                         (percent > 20) ? COLOR_YELLOW : COLOR_DANGER;
    sprite.fillRect(iconX + 5, iconY + iconH - 5 - fillH, iconW - 10, fillH, fillColor);

    // Percentage
    sprite.setTextFont(4);
    sprite.setTextColor(COLOR_TEXT, COLOR_BG);
    sprite.setTextDatum(MC_DATUM);
    char buf[16];
    snprintf(buf, sizeof(buf), "%d%%", percent);
    sprite.drawString(buf, TFT_WIDTH/2, 200);

    // Voltage
    sprite.setTextFont(2);
    snprintf(buf, sizeof(buf), "%.2f V", voltage);
    sprite.drawString(buf, TFT_WIDTH/2, 230);

    // Status
    if (charging) {
        sprite.setTextColor(COLOR_GREEN, COLOR_BG);
        sprite.drawString("CHARGING", TFT_WIDTH/2, 260);
    } else if (percent < 20) {
        sprite.setTextColor(COLOR_DANGER, COLOR_BG);
        sprite.drawString("LOW BATTERY", TFT_WIDTH/2, 260);
    }

    drawFooter("<", ">");

    update();
}

// =============================================================================
// SCREEN: SAFETY
// =============================================================================

void Display::showSafety(float voltage, float temp, bool overVoltage,
                         bool underVoltage, bool thermal) {
    clear();

    drawHeader("SAFETY STATUS");

    sprite.setTextFont(2);
    sprite.setTextDatum(TL_DATUM);

    int y = HEADER_HEIGHT + 20;
    int x = MARGIN;

    // Voltage status
    sprite.setTextColor(COLOR_TEXT, COLOR_BG);
    sprite.drawString("Voltage:", x, y);
    char buf[32];
    snprintf(buf, sizeof(buf), "%.2fV", voltage);
    uint16_t vColor = overVoltage ? COLOR_DANGER :
                      underVoltage ? COLOR_DANGER :
                      (voltage < VBAT_LOW) ? COLOR_YELLOW : COLOR_GREEN;
    sprite.setTextColor(vColor, COLOR_BG);
    sprite.drawString(buf, TFT_WIDTH - MARGIN - sprite.textWidth(buf), y);
    y += 25;

    // Voltage range
    sprite.setTextColor(0x8410, COLOR_BG);
    sprite.drawString("Safe: 6.2V - 8.6V", x, y);
    y += 35;

    // Temperature status
    sprite.setTextColor(COLOR_TEXT, COLOR_BG);
    sprite.drawString("Temperature:", x, y);
    #if TEMP_ENABLED
    snprintf(buf, sizeof(buf), "%.1fC", temp);
    uint16_t tColor = thermal ? COLOR_DANGER :
                      (temp > TEMP_WARNING_C) ? COLOR_YELLOW : COLOR_GREEN;
    #else
    snprintf(buf, sizeof(buf), "N/A");
    uint16_t tColor = 0x8410;
    #endif
    sprite.setTextColor(tColor, COLOR_BG);
    sprite.drawString(buf, TFT_WIDTH - MARGIN - sprite.textWidth(buf), y);
    y += 25;

    sprite.setTextColor(0x8410, COLOR_BG);
    sprite.drawString("Max: 45C", x, y);
    y += 35;

    // Overall status
    sprite.setTextFont(4);
    sprite.setTextDatum(MC_DATUM);
    if (overVoltage || underVoltage || thermal) {
        sprite.setTextColor(COLOR_DANGER, COLOR_BG);
        sprite.drawString("ERROR", TFT_WIDTH/2, 240);
    } else {
        sprite.setTextColor(COLOR_GREEN, COLOR_BG);
        sprite.drawString("ALL OK", TFT_WIDTH/2, 240);
    }

    drawFooter("<", ">");

    update();
}

// =============================================================================
// ALERTS
// =============================================================================

void Display::showAlert(const char* title, const char* message, uint16_t color) {
    clear();

    // Alert box
    int boxY = TFT_HEIGHT/2 - 60;
    sprite.fillRoundRect(10, boxY, TFT_WIDTH - 20, 120, 10, 0x2104);
    sprite.drawRoundRect(10, boxY, TFT_WIDTH - 20, 120, 10, color);

    // Title
    sprite.setTextFont(2);
    sprite.setTextColor(color, 0x2104);
    sprite.setTextDatum(MC_DATUM);
    sprite.drawString(title, TFT_WIDTH/2, boxY + 30);

    // Message
    sprite.setTextColor(COLOR_TEXT, 0x2104);
    sprite.drawString(message, TFT_WIDTH/2, boxY + 70);

    update();
}

void Display::showEmergency(const char* reason) {
    clear();

    sprite.fillScreen(COLOR_DANGER);

    sprite.setTextFont(4);
    sprite.setTextColor(COLOR_TEXT, COLOR_DANGER);
    sprite.setTextDatum(MC_DATUM);
    sprite.drawString("EMERGENCY", TFT_WIDTH/2, 80);

    sprite.setTextFont(2);
    sprite.drawString("SHUTDOWN", TFT_WIDTH/2, 120);

    sprite.setTextFont(2);
    sprite.setTextColor(COLOR_BG, COLOR_DANGER);
    sprite.drawString(reason, TFT_WIDTH/2, 180);

    sprite.drawString("LEDs DISABLED", TFT_WIDTH/2, 240);

    update();
}

// =============================================================================
// HELPERS
// =============================================================================

void Display::drawHeader(const char* title) {
    sprite.fillRect(0, 0, TFT_WIDTH, HEADER_HEIGHT, 0x1082);  // Dark blue-gray
    sprite.setTextFont(2);
    sprite.setTextColor(COLOR_TEXT, 0x1082);
    sprite.setTextDatum(MC_DATUM);
    sprite.drawString(title, TFT_WIDTH/2, HEADER_HEIGHT/2);
}

void Display::drawFooter(const char* left, const char* right) {
    int y = TFT_HEIGHT - FOOTER_HEIGHT;
    sprite.fillRect(0, y, TFT_WIDTH, FOOTER_HEIGHT, 0x1082);

    sprite.setTextFont(2);
    sprite.setTextColor(COLOR_TEXT, 0x1082);

    sprite.setTextDatum(ML_DATUM);
    sprite.drawString(left, MARGIN, y + FOOTER_HEIGHT/2);

    sprite.setTextDatum(MR_DATUM);
    sprite.drawString(right, TFT_WIDTH - MARGIN, y + FOOTER_HEIGHT/2);
}

void Display::drawBatteryIcon(int x, int y, uint8_t percent, bool charging) {
    int w = 30, h = 14;

    // Outline
    sprite.drawRect(x, y, w, h, COLOR_TEXT);
    sprite.fillRect(x + w, y + 3, 3, h - 6, COLOR_TEXT);  // Nub

    // Fill
    int fillW = (w - 4) * percent / 100;
    uint16_t color = (percent > 50) ? COLOR_GREEN :
                     (percent > 20) ? COLOR_YELLOW : COLOR_DANGER;
    sprite.fillRect(x + 2, y + 2, fillW, h - 4, color);

    // Charging indicator
    if (charging) {
        sprite.setTextFont(1);
        sprite.setTextColor(COLOR_BG, color);
        sprite.setTextDatum(MC_DATUM);
        sprite.drawString("+", x + w/2, y + h/2);
    }
}

void Display::drawProgressBar(int x, int y, int w, int h,
                               float progress, uint16_t color) {
    // Background
    sprite.fillRoundRect(x, y, w, h, h/2, 0x2104);

    // Progress fill
    int fillW = (w - 4) * constrain(progress, 0.0f, 1.0f);
    if (fillW > 0) {
        sprite.fillRoundRect(x + 2, y + 2, fillW, h - 4, (h-4)/2, color);
    }

    // Border
    sprite.drawRoundRect(x, y, w, h, h/2, 0x4208);
}

void Display::drawLEDIndicator(int x, int y, bool redOn, bool nirOn) {
    int r = 15;
    int spacing = 40;

    // RED LED
    int redX = x - spacing/2;
    sprite.fillCircle(redX, y, r, redOn ? COLOR_RED : 0x4000);
    sprite.drawCircle(redX, y, r, redOn ? 0xFFFF : 0x8000);
    sprite.setTextFont(1);
    sprite.setTextColor(COLOR_TEXT, COLOR_BG);
    sprite.setTextDatum(MC_DATUM);
    sprite.drawString("RED", redX, y + r + 12);

    // NIR LED
    int nirX = x + spacing/2;
    sprite.fillCircle(nirX, y, r, nirOn ? COLOR_NIR : 0x2000);
    sprite.drawCircle(nirX, y, r, nirOn ? 0xC000 : 0x4000);
    sprite.drawString("NIR", nirX, y + r + 12);
}
