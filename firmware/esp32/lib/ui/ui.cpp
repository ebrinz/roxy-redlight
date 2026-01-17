/**
 * Roxy RedLight v2.0 - UI State Machine Implementation
 */

#include "ui.h"

// =============================================================================
// INITIALIZATION
// =============================================================================

void ui_init(UIState* state) {
    state->current_screen = SCREEN_HOME;
    state->previous_screen = SCREEN_HOME;
    state->session_active = false;
    state->current_mode = UI_MODE_DUAL;
    state->selected_mode = UI_MODE_DUAL;
    state->settings_index = 0;
    state->screen_changed = true;  // Force initial draw
}

// =============================================================================
// NAVIGATION
// =============================================================================

void ui_next_screen(UIState* state) {
    state->previous_screen = state->current_screen;
    state->current_screen = (Screen)((state->current_screen + 1) % SCREEN_COUNT);
    state->screen_changed = true;
}

void ui_prev_screen(UIState* state) {
    state->previous_screen = state->current_screen;
    if (state->current_screen == 0) {
        state->current_screen = (Screen)(SCREEN_COUNT - 1);
    } else {
        state->current_screen = (Screen)(state->current_screen - 1);
    }
    state->screen_changed = true;
}

void ui_goto_screen(UIState* state, Screen screen) {
    if (screen < SCREEN_COUNT) {
        state->previous_screen = state->current_screen;
        state->current_screen = screen;
        state->screen_changed = true;
    }
}

bool ui_screen_changed(UIState* state) {
    bool changed = state->screen_changed;
    state->screen_changed = false;
    return changed;
}

// =============================================================================
// SESSION STATE
// =============================================================================

void ui_set_session_active(UIState* state, bool active) {
    state->session_active = active;
    if (active) {
        // Auto-navigate to session screen when starting
        ui_goto_screen(state, SCREEN_SESSION);
    }
}

// =============================================================================
// MODE CYCLING
// =============================================================================

UITreatmentMode ui_next_mode(UITreatmentMode current) {
    // Skip MODE_OFF in cycling (1 through MODE_COUNT-1)
    UITreatmentMode next = (UITreatmentMode)(current + 1);
    if (next >= UI_MODE_COUNT) {
        next = UI_MODE_RED_ONLY;  // Wrap to first active mode
    }
    return next;
}

UITreatmentMode ui_prev_mode(UITreatmentMode current) {
    if (current <= UI_MODE_RED_ONLY) {
        return (UITreatmentMode)(UI_MODE_COUNT - 1);  // Wrap to last mode
    }
    return (UITreatmentMode)(current - 1);
}

// =============================================================================
// BUTTON HANDLING
// =============================================================================

UIAction ui_handle_button(UIState* state, ButtonEvent event) {
    if (event == BUTTON_NONE) {
        return ACTION_NONE;
    }

    // Button 2 always navigates (except in settings)
    if (event == BUTTON_2_SHORT) {
        if (state->current_screen == SCREEN_SETTINGS) {
            // In settings, Button 2 cycles through options
            state->selected_mode = ui_next_mode(state->selected_mode);
            state->screen_changed = true;
            return ACTION_SELECT_SETTING;
        } else {
            ui_next_screen(state);
            return ACTION_NAVIGATE_NEXT;
        }
    }

    // Button 1 behavior depends on screen and session state
    if (event == BUTTON_1_SHORT) {
        switch (state->current_screen) {
            case SCREEN_HOME:
                if (!state->session_active) {
                    return ACTION_START_SESSION;
                }
                break;

            case SCREEN_SESSION:
                if (state->session_active) {
                    return ACTION_STOP_SESSION;
                } else {
                    return ACTION_START_SESSION;
                }
                break;

            case SCREEN_SETTINGS:
                // Confirm mode selection
                state->current_mode = state->selected_mode;
                state->screen_changed = true;
                return ACTION_CONFIRM_SETTING;

            case SCREEN_STATS:
            case SCREEN_BATTERY:
            case SCREEN_SAFETY:
                // No primary action on info screens
                break;

            default:
                break;
        }
    }

    // Long press Button 1 - quick mode change from home
    if (event == BUTTON_1_LONG) {
        if (state->current_screen == SCREEN_HOME && !state->session_active) {
            state->current_mode = ui_next_mode(state->current_mode);
            state->selected_mode = state->current_mode;
            state->screen_changed = true;
            return ACTION_CHANGE_MODE;
        }
    }

    return ACTION_NONE;
}

// =============================================================================
// STRING HELPERS
// =============================================================================

const char* ui_get_mode_name(UITreatmentMode mode) {
    switch (mode) {
        case UI_MODE_OFF:         return "OFF";
        case UI_MODE_RED_ONLY:    return "RED";
        case UI_MODE_NIR_ONLY:    return "NIR";
        case UI_MODE_DUAL:        return "DUAL";
        case UI_MODE_ALTERNATING: return "ALT";
        default:                  return "???";
    }
}

const char* ui_get_screen_name(Screen screen) {
    switch (screen) {
        case SCREEN_HOME:     return "Home";
        case SCREEN_SESSION:  return "Session";
        case SCREEN_STATS:    return "Stats";
        case SCREEN_SETTINGS: return "Settings";
        case SCREEN_BATTERY:  return "Battery";
        case SCREEN_SAFETY:   return "Safety";
        default:              return "Unknown";
    }
}
