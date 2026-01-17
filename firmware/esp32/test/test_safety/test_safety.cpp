/**
 * Roxy RedLight v2.0 - Safety Module Unit Tests
 *
 * Run with: pio test -e native
 *
 * These tests verify all safety-critical functionality:
 * - Voltage limits (over/under voltage protection)
 * - Thermal limits (temperature protection)
 * - Session limits (overuse protection)
 * - Power limits (irradiance protection)
 */

#include <unity.h>
#include "safety.h"

// =============================================================================
// TEST SETUP / TEARDOWN
// =============================================================================

void setUp(void) {
    // Run before each test
}

void tearDown(void) {
    // Run after each test
}

// =============================================================================
// VOLTAGE SAFETY TESTS
// =============================================================================

void test_voltage_normal_range(void) {
    // Normal operating voltages should pass
    TEST_ASSERT_EQUAL(SAFETY_OK, safety_check_voltage(7.4f));  // Nominal
    TEST_ASSERT_EQUAL(SAFETY_OK, safety_check_voltage(8.4f));  // Full
    TEST_ASSERT_EQUAL(SAFETY_OK, safety_check_voltage(6.8f));  // Low but OK
    TEST_ASSERT_EQUAL(SAFETY_OK, safety_check_voltage(6.2f));  // At cutoff (edge)
}

void test_voltage_overvoltage_detection(void) {
    // Over-voltage must be detected
    TEST_ASSERT_EQUAL(SAFETY_ERR_OVERVOLTAGE, safety_check_voltage(8.7f));
    TEST_ASSERT_EQUAL(SAFETY_ERR_OVERVOLTAGE, safety_check_voltage(9.0f));
    TEST_ASSERT_EQUAL(SAFETY_ERR_OVERVOLTAGE, safety_check_voltage(12.0f));

    // Edge case: just at limit should still pass
    TEST_ASSERT_EQUAL(SAFETY_OK, safety_check_voltage(8.6f));
}

void test_voltage_undervoltage_detection(void) {
    // Under-voltage must be detected
    TEST_ASSERT_EQUAL(SAFETY_ERR_UNDERVOLTAGE, safety_check_voltage(6.1f));
    TEST_ASSERT_EQUAL(SAFETY_ERR_UNDERVOLTAGE, safety_check_voltage(6.0f));
    TEST_ASSERT_EQUAL(SAFETY_ERR_UNDERVOLTAGE, safety_check_voltage(5.0f));
    TEST_ASSERT_EQUAL(SAFETY_ERR_UNDERVOLTAGE, safety_check_voltage(0.0f));

    // Edge case: just at cutoff should pass
    TEST_ASSERT_EQUAL(SAFETY_OK, safety_check_voltage(6.2f));
}

void test_voltage_extreme_values(void) {
    // Extreme values must be handled
    TEST_ASSERT_EQUAL(SAFETY_ERR_UNDERVOLTAGE, safety_check_voltage(-1.0f));
    TEST_ASSERT_EQUAL(SAFETY_ERR_OVERVOLTAGE, safety_check_voltage(100.0f));
}

void test_battery_percent_calculation(void) {
    // Full battery
    TEST_ASSERT_EQUAL_UINT8(100, safety_calc_battery_percent(8.4f));
    TEST_ASSERT_EQUAL_UINT8(100, safety_calc_battery_percent(8.5f));  // Clamp high

    // Empty battery
    TEST_ASSERT_EQUAL_UINT8(0, safety_calc_battery_percent(6.2f));
    TEST_ASSERT_EQUAL_UINT8(0, safety_calc_battery_percent(6.0f));   // Clamp low
    TEST_ASSERT_EQUAL_UINT8(0, safety_calc_battery_percent(5.0f));   // Below cutoff

    // Mid-range (7.4V is ~54% of range from 6.2 to 8.4)
    uint8_t mid = safety_calc_battery_percent(7.4f);
    TEST_ASSERT_TRUE(mid >= 50 && mid <= 60);

    // Low battery warning threshold (6.8V is ~27%)
    uint8_t low = safety_calc_battery_percent(6.8f);
    TEST_ASSERT_TRUE(low >= 20 && low <= 35);
}

// =============================================================================
// THERMAL SAFETY TESTS
// =============================================================================

void test_thermal_normal_range(void) {
    // Normal temperatures should pass
    TEST_ASSERT_EQUAL(SAFETY_OK, safety_check_temperature(25.0f));
    TEST_ASSERT_EQUAL(SAFETY_OK, safety_check_temperature(30.0f));
    TEST_ASSERT_EQUAL(SAFETY_OK, safety_check_temperature(39.9f));  // Just under warning
}

void test_thermal_warning_range(void) {
    // Warning range: 40-44°C should still pass (but trigger derating)
    TEST_ASSERT_EQUAL(SAFETY_OK, safety_check_temperature(40.0f));
    TEST_ASSERT_EQUAL(SAFETY_OK, safety_check_temperature(42.0f));
    TEST_ASSERT_EQUAL(SAFETY_OK, safety_check_temperature(44.9f));  // Just under cutoff
}

void test_thermal_cutoff_detection(void) {
    // Thermal cutoff must be detected
    TEST_ASSERT_EQUAL(SAFETY_ERR_THERMAL, safety_check_temperature(45.0f));
    TEST_ASSERT_EQUAL(SAFETY_ERR_THERMAL, safety_check_temperature(50.0f));
    TEST_ASSERT_EQUAL(SAFETY_ERR_THERMAL, safety_check_temperature(100.0f));
}

void test_thermal_sensor_disabled(void) {
    // Very low values indicate sensor not present - should pass
    TEST_ASSERT_EQUAL(SAFETY_OK, safety_check_temperature(-999.0f));
    TEST_ASSERT_EQUAL(SAFETY_OK, safety_check_temperature(-273.0f));
}

void test_thermal_derating_calculation(void) {
    // Below warning: full power
    TEST_ASSERT_FLOAT_WITHIN(0.01f, 1.0f, safety_calc_thermal_derating(25.0f));
    TEST_ASSERT_FLOAT_WITHIN(0.01f, 1.0f, safety_calc_thermal_derating(39.9f));

    // At warning: start derating
    float at_warning = safety_calc_thermal_derating(40.0f);
    TEST_ASSERT_TRUE(at_warning >= 0.9f && at_warning <= 1.0f);

    // At cutoff: maximum derating (50% or shutdown)
    float at_cutoff = safety_calc_thermal_derating(45.0f);
    TEST_ASSERT_TRUE(at_cutoff <= 0.5f);

    // Sensor disabled: full power
    TEST_ASSERT_FLOAT_WITHIN(0.01f, 1.0f, safety_calc_thermal_derating(-999.0f));
}

// =============================================================================
// SESSION LIMIT TESTS
// =============================================================================

void test_session_start_allowed(void) {
    // First session of day, no previous session
    TEST_ASSERT_EQUAL(SAFETY_OK, safety_check_session_start(0, 0));

    // Second session after proper gap (1+ hours)
    TEST_ASSERT_EQUAL(SAFETY_OK, safety_check_session_start(1, 3600));
    TEST_ASSERT_EQUAL(SAFETY_OK, safety_check_session_start(1, 7200));

    // Third session (at limit) after proper gap
    TEST_ASSERT_EQUAL(SAFETY_OK, safety_check_session_start(2, 3600));
}

void test_session_daily_limit_enforced(void) {
    // At limit: should fail
    TEST_ASSERT_EQUAL(SAFETY_ERR_DAILY_LIMIT, safety_check_session_start(3, 7200));

    // Over limit: should fail
    TEST_ASSERT_EQUAL(SAFETY_ERR_DAILY_LIMIT, safety_check_session_start(5, 7200));
    TEST_ASSERT_EQUAL(SAFETY_ERR_DAILY_LIMIT, safety_check_session_start(10, 7200));
}

void test_session_gap_enforced(void) {
    // Too soon after previous session (less than 1 hour)
    TEST_ASSERT_EQUAL(SAFETY_ERR_SESSION_GAP, safety_check_session_start(1, 0));
    TEST_ASSERT_EQUAL(SAFETY_ERR_SESSION_GAP, safety_check_session_start(1, 1800));  // 30 min
    TEST_ASSERT_EQUAL(SAFETY_ERR_SESSION_GAP, safety_check_session_start(1, 3599));  // 59:59

    // Edge case: exactly at gap limit should pass
    TEST_ASSERT_EQUAL(SAFETY_OK, safety_check_session_start(1, 3600));
}

void test_session_duration_limits(void) {
    // Within limits
    TEST_ASSERT_EQUAL(SAFETY_OK, safety_check_session_duration(0));
    TEST_ASSERT_EQUAL(SAFETY_OK, safety_check_session_duration(60));       // 1 min
    TEST_ASSERT_EQUAL(SAFETY_OK, safety_check_session_duration(1200));     // 20 min
    TEST_ASSERT_EQUAL(SAFETY_OK, safety_check_session_duration(1799));     // 29:59

    // At/over limit (30 min = 1800 sec)
    TEST_ASSERT_EQUAL(SAFETY_ERR_SESSION_TOO_LONG, safety_check_session_duration(1800));
    TEST_ASSERT_EQUAL(SAFETY_ERR_SESSION_TOO_LONG, safety_check_session_duration(3600));
}

// =============================================================================
// POWER LIMIT TESTS
// =============================================================================

void test_power_normal_range(void) {
    // Therapeutic range
    TEST_ASSERT_EQUAL(SAFETY_OK, safety_check_power(5.0f));    // Target
    TEST_ASSERT_EQUAL(SAFETY_OK, safety_check_power(1.0f));    // Low end
    TEST_ASSERT_EQUAL(SAFETY_OK, safety_check_power(10.0f));   // Moderate
    TEST_ASSERT_EQUAL(SAFETY_OK, safety_check_power(50.0f));   // At max limit
}

void test_power_excessive_detection(void) {
    // Over max safe irradiance
    TEST_ASSERT_EQUAL(SAFETY_ERR_POWER_TOO_HIGH, safety_check_power(51.0f));
    TEST_ASSERT_EQUAL(SAFETY_ERR_POWER_TOO_HIGH, safety_check_power(100.0f));
    TEST_ASSERT_EQUAL(SAFETY_ERR_POWER_TOO_HIGH, safety_check_power(500.0f));
}

// =============================================================================
// COMPREHENSIVE SAFETY CHECK TESTS
// =============================================================================

void test_comprehensive_all_ok(void) {
    SafetyStatus status = safety_check_all(
        7.4f,    // Normal voltage
        25.0f,   // Normal temp
        0,       // First session
        0,       // N/A for first
        false,   // Not in session
        0        // N/A
    );

    TEST_ASSERT_TRUE(status.voltage_ok);
    TEST_ASSERT_TRUE(status.thermal_ok);
    TEST_ASSERT_TRUE(status.session_ok);
    TEST_ASSERT_EQUAL(SAFETY_OK, status.error);
}

void test_comprehensive_overvoltage_stops_everything(void) {
    SafetyStatus status = safety_check_all(
        9.0f,    // OVER-VOLTAGE
        25.0f,   // Normal temp
        0,       // First session
        0,       // N/A
        false,   // Not in session
        0        // N/A
    );

    TEST_ASSERT_FALSE(status.voltage_ok);
    TEST_ASSERT_EQUAL(SAFETY_ERR_OVERVOLTAGE, status.error);
}

void test_comprehensive_thermal_stops_everything(void) {
    SafetyStatus status = safety_check_all(
        7.4f,    // Normal voltage
        50.0f,   // OVERHEATING
        0,       // First session
        0,       // N/A
        false,   // Not in session
        0        // N/A
    );

    TEST_ASSERT_FALSE(status.thermal_ok);
    TEST_ASSERT_EQUAL(SAFETY_ERR_THERMAL, status.error);
}

void test_comprehensive_session_limit_blocks_start(void) {
    SafetyStatus status = safety_check_all(
        7.4f,    // Normal voltage
        25.0f,   // Normal temp
        3,       // DAILY LIMIT REACHED
        7200,    // 2 hours since last
        false,   // Not in session (trying to start)
        0        // N/A
    );

    TEST_ASSERT_TRUE(status.voltage_ok);    // These are OK
    TEST_ASSERT_TRUE(status.thermal_ok);
    TEST_ASSERT_FALSE(status.session_ok);   // But can't start session
    TEST_ASSERT_EQUAL(SAFETY_ERR_DAILY_LIMIT, status.error);
}

void test_comprehensive_active_session_timeout(void) {
    SafetyStatus status = safety_check_all(
        7.4f,    // Normal voltage
        25.0f,   // Normal temp
        1,       // One session done
        0,       // Currently in session
        true,    // SESSION ACTIVE
        1800     // 30 MIN - AT LIMIT
    );

    TEST_ASSERT_TRUE(status.voltage_ok);
    TEST_ASSERT_TRUE(status.thermal_ok);
    TEST_ASSERT_FALSE(status.session_ok);   // Must stop session
    TEST_ASSERT_EQUAL(SAFETY_ERR_SESSION_TOO_LONG, status.error);
}

// =============================================================================
// ERROR MESSAGE TESTS
// =============================================================================

void test_error_messages_not_null(void) {
    // All error codes should have messages
    TEST_ASSERT_NOT_NULL(safety_get_error_message(SAFETY_OK));
    TEST_ASSERT_NOT_NULL(safety_get_error_message(SAFETY_ERR_OVERVOLTAGE));
    TEST_ASSERT_NOT_NULL(safety_get_error_message(SAFETY_ERR_UNDERVOLTAGE));
    TEST_ASSERT_NOT_NULL(safety_get_error_message(SAFETY_ERR_THERMAL));
    TEST_ASSERT_NOT_NULL(safety_get_error_message(SAFETY_ERR_DAILY_LIMIT));
    TEST_ASSERT_NOT_NULL(safety_get_error_message(SAFETY_ERR_SESSION_GAP));
    TEST_ASSERT_NOT_NULL(safety_get_error_message(SAFETY_ERR_SESSION_TOO_LONG));
    TEST_ASSERT_NOT_NULL(safety_get_error_message(SAFETY_ERR_POWER_TOO_HIGH));
}

void test_error_messages_contain_relevant_text(void) {
    const char* msg;

    msg = safety_get_error_message(SAFETY_ERR_OVERVOLTAGE);
    TEST_ASSERT_NOT_NULL(strstr(msg, "over") || strstr(msg, "OVER") ||
                         strstr(msg, "voltage") || strstr(msg, "VOLTAGE"));

    msg = safety_get_error_message(SAFETY_ERR_THERMAL);
    TEST_ASSERT_NOT_NULL(strstr(msg, "heat") || strstr(msg, "HEAT") ||
                         strstr(msg, "temp") || strstr(msg, "TEMP") ||
                         strstr(msg, "thermal") || strstr(msg, "THERMAL"));
}

// =============================================================================
// TEST RUNNER
// =============================================================================

int main(int argc, char **argv) {
    UNITY_BEGIN();

    // Voltage tests
    RUN_TEST(test_voltage_normal_range);
    RUN_TEST(test_voltage_overvoltage_detection);
    RUN_TEST(test_voltage_undervoltage_detection);
    RUN_TEST(test_voltage_extreme_values);
    RUN_TEST(test_battery_percent_calculation);

    // Thermal tests
    RUN_TEST(test_thermal_normal_range);
    RUN_TEST(test_thermal_warning_range);
    RUN_TEST(test_thermal_cutoff_detection);
    RUN_TEST(test_thermal_sensor_disabled);
    RUN_TEST(test_thermal_derating_calculation);

    // Session tests
    RUN_TEST(test_session_start_allowed);
    RUN_TEST(test_session_daily_limit_enforced);
    RUN_TEST(test_session_gap_enforced);
    RUN_TEST(test_session_duration_limits);

    // Power tests
    RUN_TEST(test_power_normal_range);
    RUN_TEST(test_power_excessive_detection);

    // Comprehensive tests
    RUN_TEST(test_comprehensive_all_ok);
    RUN_TEST(test_comprehensive_overvoltage_stops_everything);
    RUN_TEST(test_comprehensive_thermal_stops_everything);
    RUN_TEST(test_comprehensive_session_limit_blocks_start);
    RUN_TEST(test_comprehensive_active_session_timeout);

    // Error message tests
    RUN_TEST(test_error_messages_not_null);
    RUN_TEST(test_error_messages_contain_relevant_text);

    return UNITY_END();
}
