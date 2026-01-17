/**
 * Roxy RedLight v2.0 - UI State Machine
 *
 * Testable UI logic separated from display rendering
 */

#ifndef UI_H
#define UI_H

#include <stdint.h>
#include <stdbool.h>

// =============================================================================
// SCREEN DEFINITIONS
// =============================================================================

typedef enum {
    SCREEN_HOME = 0,
    SCREEN_SESSION,
    SCREEN_STATS,
    SCREEN_SETTINGS,
    SCREEN_BATTERY,
    SCREEN_SAFETY,
    SCREEN_COUNT
} Screen;

// =============================================================================
// TREATMENT MODES
// =============================================================================

typedef enum {
    UI_MODE_OFF = 0,
    UI_MODE_RED_ONLY,
    UI_MODE_NIR_ONLY,
    UI_MODE_DUAL,
    UI_MODE_ALTERNATING,
    UI_MODE_COUNT
} UITreatmentMode;

// =============================================================================
// BUTTON EVENTS
// =============================================================================

typedef enum {
    BUTTON_NONE = 0,
    BUTTON_1_SHORT,     // Primary action
    BUTTON_1_LONG,      // Secondary action
    BUTTON_2_SHORT,     // Navigation
    BUTTON_2_LONG       // Reserved
} ButtonEvent;

// =============================================================================
// UI ACTIONS (output from state machine)
// =============================================================================

typedef enum {
    ACTION_NONE = 0,
    ACTION_START_SESSION,
    ACTION_STOP_SESSION,
    ACTION_CHANGE_MODE,
    ACTION_NAVIGATE_NEXT,
    ACTION_NAVIGATE_PREV,
    ACTION_SELECT_SETTING,
    ACTION_CONFIRM_SETTING
} UIAction;

// =============================================================================
// UI STATE
// =============================================================================

typedef struct {
    Screen current_screen;
    Screen previous_screen;
    bool session_active;
    UITreatmentMode current_mode;
    UITreatmentMode selected_mode;      // In settings screen
    uint8_t settings_index;             // Which setting is highlighted
    bool screen_changed;                // Flag for display update
} UIState;

// =============================================================================
// UI FUNCTIONS
// =============================================================================

/**
 * Initialize UI state to defaults
 * @param state Pointer to UI state structure
 */
void ui_init(UIState* state);

/**
 * Process a button event and return resulting action
 * @param state Pointer to UI state
 * @param event Button event to process
 * @return Action to perform (may be ACTION_NONE)
 */
UIAction ui_handle_button(UIState* state, ButtonEvent event);

/**
 * Navigate to next screen
 * @param state Pointer to UI state
 */
void ui_next_screen(UIState* state);

/**
 * Navigate to previous screen
 * @param state Pointer to UI state
 */
void ui_prev_screen(UIState* state);

/**
 * Go to specific screen
 * @param state Pointer to UI state
 * @param screen Target screen
 */
void ui_goto_screen(UIState* state, Screen screen);

/**
 * Set session active state (affects button behavior)
 * @param state Pointer to UI state
 * @param active Whether session is running
 */
void ui_set_session_active(UIState* state, bool active);

/**
 * Get next treatment mode (cycles through modes)
 * @param current Current mode
 * @return Next mode in cycle
 */
UITreatmentMode ui_next_mode(UITreatmentMode current);

/**
 * Get previous treatment mode
 * @param current Current mode
 * @return Previous mode in cycle
 */
UITreatmentMode ui_prev_mode(UITreatmentMode current);

/**
 * Get mode name as string
 * @param mode Treatment mode
 * @return Mode name string
 */
const char* ui_get_mode_name(UITreatmentMode mode);

/**
 * Get screen name as string
 * @param screen Screen enum
 * @return Screen name string
 */
const char* ui_get_screen_name(Screen screen);

/**
 * Check if screen changed since last check (clears flag)
 * @param state Pointer to UI state
 * @return true if screen changed
 */
bool ui_screen_changed(UIState* state);

#endif // UI_H
