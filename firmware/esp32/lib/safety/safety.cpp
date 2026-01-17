/**
 * Roxy RedLight v2.0 - Safety Module Implementation
 */

#include "safety.h"

// =============================================================================
// VOLTAGE CHECKS
// =============================================================================

SafetyResult safety_check_voltage(float voltage) {
    if (voltage > SAFETY_VBAT_OVERVOLTAGE) {
        return SAFETY_ERR_OVERVOLTAGE;
    }
    if (voltage < SAFETY_VBAT_CUTOFF) {
        return SAFETY_ERR_UNDERVOLTAGE;
    }
    return SAFETY_OK;
}

uint8_t safety_calc_battery_percent(float voltage) {
    if (voltage >= SAFETY_VBAT_FULL) return 100;
    if (voltage <= SAFETY_VBAT_CUTOFF) return 0;

    float range = SAFETY_VBAT_FULL - SAFETY_VBAT_CUTOFF;
    float level = voltage - SAFETY_VBAT_CUTOFF;
    float percent = (level / range) * 100.0f;

    if (percent > 100.0f) percent = 100.0f;
    if (percent < 0.0f) percent = 0.0f;

    return (uint8_t)percent;
}

// =============================================================================
// THERMAL CHECKS
// =============================================================================

SafetyResult safety_check_temperature(float temp_c) {
    // Skip check if sensor not available (indicated by very low value)
    if (temp_c < -100.0f) {
        return SAFETY_OK;
    }

    if (temp_c >= SAFETY_TEMP_CUTOFF) {
        return SAFETY_ERR_THERMAL;
    }
    return SAFETY_OK;
}

float safety_calc_thermal_derating(float temp_c) {
    // Skip if sensor not available
    if (temp_c < -100.0f) {
        return 1.0f;
    }

    if (temp_c < SAFETY_TEMP_WARNING) {
        return 1.0f;  // Full power
    }

    if (temp_c >= SAFETY_TEMP_CUTOFF) {
        return 0.0f;  // Shutdown
    }

    // Linear derating between warning and cutoff
    float range = SAFETY_TEMP_CUTOFF - SAFETY_TEMP_WARNING;
    float over = temp_c - SAFETY_TEMP_WARNING;
    float derating = 1.0f - (over / range) * 0.5f;  // Max 50% reduction

    if (derating < 0.5f) derating = 0.5f;
    if (derating > 1.0f) derating = 1.0f;

    return derating;
}

// =============================================================================
// SESSION CHECKS
// =============================================================================

SafetyResult safety_check_session_start(uint8_t daily_sessions,
                                         uint32_t seconds_since_last) {
    // Check daily limit
    if (daily_sessions >= SAFETY_MAX_DAILY_SESSIONS) {
        return SAFETY_ERR_DAILY_LIMIT;
    }

    // Check minimum gap (only if there was a previous session)
    if (daily_sessions > 0 && seconds_since_last < SAFETY_MIN_SESSION_GAP_SEC) {
        return SAFETY_ERR_SESSION_GAP;
    }

    return SAFETY_OK;
}

SafetyResult safety_check_session_duration(uint32_t elapsed_seconds) {
    if (elapsed_seconds >= SAFETY_MAX_SESSION_SEC) {
        return SAFETY_ERR_SESSION_TOO_LONG;
    }
    return SAFETY_OK;
}

// =============================================================================
// POWER CHECKS
// =============================================================================

SafetyResult safety_check_power(float power_mw_cm2) {
    if (power_mw_cm2 > SAFETY_MAX_POWER_MW_CM2) {
        return SAFETY_ERR_POWER_TOO_HIGH;
    }
    return SAFETY_OK;
}

// =============================================================================
// COMPREHENSIVE CHECK
// =============================================================================

SafetyStatus safety_check_all(float voltage, float temp_c,
                               uint8_t daily_sessions,
                               uint32_t seconds_since_last,
                               bool session_active,
                               uint32_t elapsed_seconds) {
    SafetyStatus status;
    status.voltage_ok = true;
    status.thermal_ok = true;
    status.session_ok = true;
    status.power_ok = true;
    status.error = SAFETY_OK;
    status.error_message = "All systems OK";

    // Voltage check
    SafetyResult voltage_result = safety_check_voltage(voltage);
    if (voltage_result != SAFETY_OK) {
        status.voltage_ok = false;
        status.error = voltage_result;
        status.error_message = safety_get_error_message(voltage_result);
        return status;  // Critical - return immediately
    }

    // Thermal check
    SafetyResult thermal_result = safety_check_temperature(temp_c);
    if (thermal_result != SAFETY_OK) {
        status.thermal_ok = false;
        status.error = thermal_result;
        status.error_message = safety_get_error_message(thermal_result);
        return status;  // Critical - return immediately
    }

    // Session checks
    if (!session_active) {
        // Check if we can start a new session
        SafetyResult session_result = safety_check_session_start(
            daily_sessions, seconds_since_last);
        if (session_result != SAFETY_OK) {
            status.session_ok = false;
            status.error = session_result;
            status.error_message = safety_get_error_message(session_result);
            // Not critical - can continue but can't start session
        }
    } else {
        // Check if current session has exceeded limits
        SafetyResult duration_result = safety_check_session_duration(elapsed_seconds);
        if (duration_result != SAFETY_OK) {
            status.session_ok = false;
            status.error = duration_result;
            status.error_message = safety_get_error_message(duration_result);
            // Will trigger session stop
        }
    }

    return status;
}

// =============================================================================
// ERROR MESSAGES
// =============================================================================

const char* safety_get_error_message(SafetyResult result) {
    switch (result) {
        case SAFETY_OK:
            return "All systems OK";
        case SAFETY_ERR_OVERVOLTAGE:
            return "DANGER: Battery over-voltage!";
        case SAFETY_ERR_UNDERVOLTAGE:
            return "Battery critically low";
        case SAFETY_ERR_THERMAL:
            return "DANGER: Overheating!";
        case SAFETY_ERR_DAILY_LIMIT:
            return "Daily session limit reached";
        case SAFETY_ERR_SESSION_GAP:
            return "Wait between sessions";
        case SAFETY_ERR_SESSION_TOO_LONG:
            return "Max session time exceeded";
        case SAFETY_ERR_POWER_TOO_HIGH:
            return "Power output too high";
        default:
            return "Unknown error";
    }
}
