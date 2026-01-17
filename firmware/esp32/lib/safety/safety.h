/**
 * Roxy RedLight v2.0 - Safety Module
 *
 * Testable safety logic separated from hardware dependencies
 */

#ifndef SAFETY_H
#define SAFETY_H

#include <stdint.h>
#include <stdbool.h>

// =============================================================================
// SAFETY THRESHOLDS (must match config.h)
// =============================================================================

#define SAFETY_VBAT_OVERVOLTAGE    8.6f    // Over-voltage limit
#define SAFETY_VBAT_FULL           8.4f    // Fully charged
#define SAFETY_VBAT_NOMINAL        7.4f    // Nominal voltage
#define SAFETY_VBAT_LOW            6.8f    // Low battery warning
#define SAFETY_VBAT_CUTOFF         6.2f    // Under-voltage cutoff

#define SAFETY_TEMP_WARNING        40.0f   // Temperature warning
#define SAFETY_TEMP_CUTOFF         45.0f   // Thermal cutoff

#define SAFETY_MAX_SESSION_SEC     (30 * 60)   // 30 minutes max
#define SAFETY_MAX_DAILY_SESSIONS  3
#define SAFETY_MIN_SESSION_GAP_SEC (60 * 60)   // 60 minutes gap

#define SAFETY_MAX_POWER_MW_CM2    50.0f   // Max safe irradiance
#define SAFETY_TARGET_POWER_MW_CM2 5.0f    // Target therapeutic dose

// =============================================================================
// SAFETY CHECK RESULTS
// =============================================================================

typedef enum {
    SAFETY_OK = 0,
    SAFETY_ERR_OVERVOLTAGE,
    SAFETY_ERR_UNDERVOLTAGE,
    SAFETY_ERR_THERMAL,
    SAFETY_ERR_DAILY_LIMIT,
    SAFETY_ERR_SESSION_GAP,
    SAFETY_ERR_SESSION_TOO_LONG,
    SAFETY_ERR_POWER_TOO_HIGH
} SafetyResult;

typedef struct {
    bool voltage_ok;
    bool thermal_ok;
    bool session_ok;
    bool power_ok;
    SafetyResult error;
    const char* error_message;
} SafetyStatus;

// =============================================================================
// SAFETY CHECK FUNCTIONS
// =============================================================================

/**
 * Check if voltage is within safe operating range
 * @param voltage Battery voltage in volts
 * @return SAFETY_OK if safe, error code otherwise
 */
SafetyResult safety_check_voltage(float voltage);

/**
 * Check if temperature is within safe operating range
 * @param temp_c Temperature in Celsius
 * @return SAFETY_OK if safe, error code otherwise
 */
SafetyResult safety_check_temperature(float temp_c);

/**
 * Check if starting a new session is allowed
 * @param daily_sessions Number of sessions today
 * @param seconds_since_last Seconds since last session ended
 * @return SAFETY_OK if allowed, error code otherwise
 */
SafetyResult safety_check_session_start(uint8_t daily_sessions,
                                         uint32_t seconds_since_last);

/**
 * Check if session duration is within limits
 * @param elapsed_seconds Current session duration
 * @return SAFETY_OK if within limit, error code otherwise
 */
SafetyResult safety_check_session_duration(uint32_t elapsed_seconds);

/**
 * Check if power/irradiance is within safe limits
 * @param power_mw_cm2 Power density in mW/cm²
 * @return SAFETY_OK if safe, error code otherwise
 */
SafetyResult safety_check_power(float power_mw_cm2);

/**
 * Run all safety checks
 * @param voltage Battery voltage
 * @param temp_c Temperature (or -999 if sensor disabled)
 * @param daily_sessions Sessions today
 * @param seconds_since_last Time since last session
 * @param session_active Whether session is currently running
 * @param elapsed_seconds Current session duration (if active)
 * @return SafetyStatus with all check results
 */
SafetyStatus safety_check_all(float voltage, float temp_c,
                               uint8_t daily_sessions,
                               uint32_t seconds_since_last,
                               bool session_active,
                               uint32_t elapsed_seconds);

/**
 * Calculate battery percentage from voltage
 * @param voltage Battery voltage
 * @return Percentage 0-100
 */
uint8_t safety_calc_battery_percent(float voltage);

/**
 * Calculate recommended brightness reduction for thermal protection
 * @param temp_c Current temperature
 * @return Brightness multiplier 0.0-1.0
 */
float safety_calc_thermal_derating(float temp_c);

/**
 * Get human-readable error message
 * @param result Safety check result
 * @return Error message string
 */
const char* safety_get_error_message(SafetyResult result);

#endif // SAFETY_H
