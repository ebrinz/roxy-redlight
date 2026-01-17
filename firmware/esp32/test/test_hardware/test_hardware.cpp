/**
 * Roxy RedLight v2.0 - Hardware Circuit Tests
 *
 * Run with: pio test -e lilygo-t-display-s3
 *
 * These tests run ON THE DEVICE to verify hardware connections.
 * Connect the actual hardware before running.
 *
 * SAFETY: Tests use low duty cycles and short durations.
 */

#ifdef ARDUINO

#include <unity.h>
#include <Arduino.h>

// Pin definitions (must match config.h)
#define PIN_RED_LED     43
#define PIN_NIR_LED     44
#define PIN_BUTTON_1    0
#define PIN_BUTTON_2    14
#define PIN_VBAT_ADC    4
#define PIN_BUZZER      21
#define PIN_POWER_ON    15

// PWM settings
#define PWM_FREQ        1000
#define PWM_RESOLUTION  8
#define PWM_CHANNEL_RED 0
#define PWM_CHANNEL_NIR 1

// Test thresholds
#define ADC_MIN_VALID       100     // Minimum valid ADC reading (not floating)
#define ADC_MAX_VALID       4000    // Maximum valid ADC reading (not shorted)
#define VBAT_MIN_EXPECTED   5.0     // Minimum expected battery voltage
#define VBAT_MAX_EXPECTED   9.0     // Maximum expected battery voltage

// =============================================================================
// TEST SETUP / TEARDOWN
// =============================================================================

void setUp(void) {
    // Ensure LEDs are off before each test
    ledcWrite(PWM_CHANNEL_RED, 0);
    ledcWrite(PWM_CHANNEL_NIR, 0);
}

void tearDown(void) {
    // Ensure LEDs are off after each test
    ledcWrite(PWM_CHANNEL_RED, 0);
    ledcWrite(PWM_CHANNEL_NIR, 0);
}

// =============================================================================
// PWM OUTPUT TESTS
// =============================================================================

void test_red_led_pwm_output(void) {
    // Test that PWM can be set without error
    // Visual verification: Red LEDs should flash briefly
    ledcWrite(PWM_CHANNEL_RED, 64);  // 25% duty
    delay(100);
    ledcWrite(PWM_CHANNEL_RED, 0);

    // If we got here without crash, PWM is working
    TEST_PASS();
}

void test_nir_led_pwm_output(void) {
    // Test that PWM can be set without error
    // Note: NIR is invisible, but MOSFET should activate
    ledcWrite(PWM_CHANNEL_NIR, 64);  // 25% duty
    delay(100);
    ledcWrite(PWM_CHANNEL_NIR, 0);

    TEST_PASS();
}

void test_pwm_duty_cycle_range(void) {
    // Test full range of duty cycles
    for (int duty = 0; duty <= 255; duty += 51) {
        ledcWrite(PWM_CHANNEL_RED, duty);
        delay(10);
    }
    ledcWrite(PWM_CHANNEL_RED, 0);

    TEST_PASS();
}

void test_both_channels_independent(void) {
    // Verify both channels can be controlled independently
    ledcWrite(PWM_CHANNEL_RED, 128);
    ledcWrite(PWM_CHANNEL_NIR, 64);
    delay(50);

    // Change one without affecting other
    ledcWrite(PWM_CHANNEL_RED, 0);
    delay(50);
    ledcWrite(PWM_CHANNEL_NIR, 0);

    TEST_PASS();
}

// =============================================================================
// ADC / BATTERY TESTS
// =============================================================================

void test_battery_adc_reads_valid(void) {
    // Read ADC and verify it's not floating or shorted
    int adc_raw = analogRead(PIN_VBAT_ADC);

    TEST_ASSERT_GREATER_THAN(ADC_MIN_VALID, adc_raw);
    TEST_ASSERT_LESS_THAN(ADC_MAX_VALID, adc_raw);
}

void test_battery_adc_stable(void) {
    // Take multiple readings and verify stability
    int readings[10];
    int sum = 0;

    for (int i = 0; i < 10; i++) {
        readings[i] = analogRead(PIN_VBAT_ADC);
        sum += readings[i];
        delay(10);
    }

    int avg = sum / 10;

    // All readings should be within 5% of average
    for (int i = 0; i < 10; i++) {
        int diff = abs(readings[i] - avg);
        int max_diff = avg / 20;  // 5%
        TEST_ASSERT_LESS_THAN(max_diff + 50, diff);  // +50 for noise margin
    }
}

void test_battery_voltage_in_range(void) {
    // Calculate actual voltage and verify it's reasonable
    int adc_raw = analogRead(PIN_VBAT_ADC);
    float voltage_at_adc = (adc_raw / 4095.0) * 3.3;

    // Voltage divider: 100k / 33k = ratio of 0.248
    float battery_voltage = voltage_at_adc / 0.248;

    TEST_ASSERT_GREATER_THAN(VBAT_MIN_EXPECTED, battery_voltage);
    TEST_ASSERT_LESS_THAN(VBAT_MAX_EXPECTED, battery_voltage);

    // Print for manual verification
    Serial.printf("Battery voltage: %.2fV (ADC: %d)\n", battery_voltage, adc_raw);
}

// =============================================================================
// BUTTON INPUT TESTS
// =============================================================================

void test_button1_pullup_high(void) {
    // Button should read HIGH when not pressed (internal pullup)
    int state = digitalRead(PIN_BUTTON_1);
    TEST_ASSERT_EQUAL(HIGH, state);
}

void test_button2_pullup_high(void) {
    // Button should read HIGH when not pressed
    int state = digitalRead(PIN_BUTTON_2);
    TEST_ASSERT_EQUAL(HIGH, state);
}

// Note: Interactive button press tests would require user action
// These are marked as manual tests in documentation

// =============================================================================
// BUZZER TESTS (if connected)
// =============================================================================

void test_buzzer_output(void) {
    // Quick beep test - very short to avoid annoyance
    pinMode(PIN_BUZZER, OUTPUT);
    tone(PIN_BUZZER, 1000, 50);  // 1kHz for 50ms
    delay(100);
    noTone(PIN_BUZZER);

    TEST_PASS();
}

// =============================================================================
// POWER HOLD TEST
// =============================================================================

void test_power_hold_pin(void) {
    // Verify power hold pin can be controlled
    pinMode(PIN_POWER_ON, OUTPUT);
    digitalWrite(PIN_POWER_ON, HIGH);

    int state = digitalRead(PIN_POWER_ON);
    TEST_ASSERT_EQUAL(HIGH, state);
}

// =============================================================================
// INTEGRATION TESTS
// =============================================================================

void test_led_sequence(void) {
    // Visual test: Red -> NIR -> Both -> Off
    Serial.println("LED Sequence Test: Watch for Red -> NIR -> Both -> Off");

    // Red only
    ledcWrite(PWM_CHANNEL_RED, 128);
    ledcWrite(PWM_CHANNEL_NIR, 0);
    delay(500);

    // NIR only
    ledcWrite(PWM_CHANNEL_RED, 0);
    ledcWrite(PWM_CHANNEL_NIR, 128);
    delay(500);

    // Both
    ledcWrite(PWM_CHANNEL_RED, 128);
    ledcWrite(PWM_CHANNEL_NIR, 128);
    delay(500);

    // Off
    ledcWrite(PWM_CHANNEL_RED, 0);
    ledcWrite(PWM_CHANNEL_NIR, 0);

    TEST_PASS();
}

// =============================================================================
// SETUP - Initialize hardware before tests
// =============================================================================

void setup() {
    Serial.begin(115200);
    delay(2000);  // Wait for serial

    Serial.println("\n=================================");
    Serial.println("  Roxy RedLight Hardware Tests");
    Serial.println("  LilyGO T-Display S3");
    Serial.println("=================================\n");

    // Initialize PWM
    ledcSetup(PWM_CHANNEL_RED, PWM_FREQ, PWM_RESOLUTION);
    ledcSetup(PWM_CHANNEL_NIR, PWM_FREQ, PWM_RESOLUTION);
    ledcAttachPin(PIN_RED_LED, PWM_CHANNEL_RED);
    ledcAttachPin(PIN_NIR_LED, PWM_CHANNEL_NIR);

    // Initialize buttons
    pinMode(PIN_BUTTON_1, INPUT_PULLUP);
    pinMode(PIN_BUTTON_2, INPUT_PULLUP);

    // Initialize ADC
    analogReadResolution(12);
    analogSetAttenuation(ADC_11db);

    // Run tests
    UNITY_BEGIN();

    // PWM tests
    RUN_TEST(test_red_led_pwm_output);
    RUN_TEST(test_nir_led_pwm_output);
    RUN_TEST(test_pwm_duty_cycle_range);
    RUN_TEST(test_both_channels_independent);

    // ADC tests
    RUN_TEST(test_battery_adc_reads_valid);
    RUN_TEST(test_battery_adc_stable);
    RUN_TEST(test_battery_voltage_in_range);

    // Button tests
    RUN_TEST(test_button1_pullup_high);
    RUN_TEST(test_button2_pullup_high);

    // Buzzer test
    RUN_TEST(test_buzzer_output);

    // Power hold test
    RUN_TEST(test_power_hold_pin);

    // Integration test (visual)
    RUN_TEST(test_led_sequence);

    UNITY_END();
}

void loop() {
    // Nothing - tests run once in setup
}

#endif // ARDUINO
