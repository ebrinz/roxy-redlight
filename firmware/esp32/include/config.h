/**
 * Roxy RedLight v2.0 - Configuration
 *
 * Hardware pin definitions and treatment parameters
 */

#ifndef CONFIG_H
#define CONFIG_H

// =============================================================================
// PIN DEFINITIONS - LilyGO T-Display S3
// =============================================================================

// LED Channel Pins (directly drive MOSFETs)
#define PIN_RED_LED     43  // GPIO43 - Red 650nm MOSFET gate
#define PIN_NIR_LED     44  // GPIO44 - NIR 850nm MOSFET gate

// User Interface - T-Display S3 has two built-in buttons
#define PIN_BUTTON_1    0   // GPIO0 - Boot button (active LOW)
#define PIN_BUTTON_2    14  // GPIO14 - Second button (active LOW)
#define PIN_BUTTON      PIN_BUTTON_1  // Primary button alias
#define PIN_BUZZER      21  // GPIO21 - Piezo buzzer (optional)

// Display backlight (directly controlled in TFT_eSPI, but useful for sleep)
#define PIN_TFT_BL      38  // GPIO38 - TFT backlight

// Analog Input
#define PIN_VBAT_ADC    4   // GPIO4 - Battery voltage via divider

// Power control (T-Display S3 specific)
#define PIN_POWER_ON    15  // GPIO15 - Keep HIGH to maintain power

// =============================================================================
// DISPLAY SETTINGS
// =============================================================================

#define TFT_WIDTH       170
#define TFT_HEIGHT      320
#define TFT_ROTATION    0   // 0=Portrait, 1=Landscape

// Colors (RGB565)
#define COLOR_BG        0x0000  // Black
#define COLOR_TEXT      0xFFFF  // White
#define COLOR_RED       0xF800  // Red LED indicator
#define COLOR_NIR       0x7800  // Dark red for NIR (invisible to eye)
#define COLOR_GREEN     0x07E0  // Green for OK status
#define COLOR_YELLOW    0xFFE0  // Yellow for warnings
#define COLOR_ORANGE    0xFD20  // Orange for caution
#define COLOR_DANGER    0xF800  // Red for danger

// UI Layout
#define HEADER_HEIGHT   40
#define FOOTER_HEIGHT   30
#define MARGIN          10

// =============================================================================
// PWM CONFIGURATION
// =============================================================================

#define PWM_FREQ        1000    // 1kHz - flicker-free
#define PWM_RESOLUTION  8       // 8-bit (0-255)
#define PWM_CHANNEL_RED 0       // LEDC channel for red LEDs
#define PWM_CHANNEL_NIR 1       // LEDC channel for NIR LEDs

// =============================================================================
// BATTERY MONITORING
// =============================================================================

// Voltage divider: 100k / 33k
// Vout = Vbat * (33 / 133) = Vbat * 0.248
#define VBAT_DIVIDER_RATIO  0.248
#define VBAT_ADC_MAX        4095    // 12-bit ADC
#define VBAT_REF_VOLTAGE    3.3     // ADC reference

// Battery thresholds (2S Li-ion: 6.0V - 8.4V)
#define VBAT_OVERVOLTAGE    8.6     // Over-voltage protection (bad charger)
#define VBAT_FULL           8.4     // Fully charged
#define VBAT_NOMINAL        7.4     // Nominal voltage
#define VBAT_LOW            6.8     // Low battery warning
#define VBAT_CUTOFF         6.2     // Emergency shutoff (under-voltage)

// =============================================================================
// SAFETY LIMITS
// =============================================================================

// Power/irradiance limits based on clinical research
// Target: ~5 mW/cm² - safe therapeutic range is 1-50 mW/cm²
// Above 100 mW/cm² risks thermal damage
#define MAX_POWER_MW_CM2        50      // Max safe irradiance
#define THERAPEUTIC_MW_CM2      5       // Target therapeutic dose

// Thermal protection (if NTC thermistor connected)
#define PIN_TEMP_ADC            7       // GPIO7 for optional thermistor
#define TEMP_WARNING_C          40      // Warning threshold
#define TEMP_CUTOFF_C           45      // Emergency shutoff
#define TEMP_ENABLED            false   // Set true if thermistor installed

// Session safety
#define MAX_DAILY_SESSIONS      3       // Prevent overuse
#define MIN_SESSION_GAP_MIN     60      // Minimum gap between sessions

// =============================================================================
// TREATMENT PARAMETERS (Evidence-based)
// =============================================================================

// Session duration in minutes
#define DEFAULT_SESSION_MINUTES     20

// Auto-shutoff safety (minutes)
#define MAX_SESSION_MINUTES         30

// Fluence target: 4 J/cm² @ 5 mW/cm² = 800 seconds = 13.3 minutes
// We use 20 min for margin and user convenience

// =============================================================================
// TREATMENT MODES
// =============================================================================

typedef enum {
    MODE_OFF = 0,
    MODE_RED_ONLY,      // 650nm only - maintenance, early stage
    MODE_NIR_ONLY,      // 850nm only - deep follicle treatment
    MODE_DUAL,          // Both wavelengths - comprehensive
    MODE_ALTERNATING,   // Alternate every 30s - experimental
    MODE_COUNT          // Number of modes
} TreatmentMode;

// Default mode
#define DEFAULT_MODE    MODE_DUAL

// Alternating mode period (seconds)
#define ALTERNATE_PERIOD_SEC    30

// =============================================================================
// USER INTERFACE
// =============================================================================

// Button debounce time (ms)
#define BUTTON_DEBOUNCE_MS      50

// Long press threshold (ms) - for mode change
#define BUTTON_LONG_PRESS_MS    1000

// Status LED blink patterns (ms)
#define BLINK_FAST      100
#define BLINK_SLOW      500
#define BLINK_VERY_SLOW 1000

// Buzzer tones (Hz)
#define TONE_START      1000
#define TONE_STOP       500
#define TONE_LOW_BAT    2000
#define TONE_COMPLETE   1500

// =============================================================================
// PERSISTENCE
// =============================================================================

#define PREFS_NAMESPACE     "roxyredlight"
#define PREFS_KEY_SESSIONS  "sessions"
#define PREFS_KEY_MINUTES   "minutes"
#define PREFS_KEY_MODE      "mode"

#endif // CONFIG_H
