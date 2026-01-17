/**
 * Roxy RedLight v2.0 - UI State Machine Unit Tests
 *
 * Run with: pio test -e native -f test_ui
 *
 * Tests navigation, button handling, and mode selection logic
 */

#include <unity.h>
#include "ui.h"

// =============================================================================
// TEST FIXTURES
// =============================================================================

static UIState state;

void setUp(void) {
    ui_init(&state);
}

void tearDown(void) {
    // Nothing to clean up
}

// =============================================================================
// INITIALIZATION TESTS
// =============================================================================

void test_init_defaults(void) {
    UIState s;
    ui_init(&s);

    TEST_ASSERT_EQUAL(SCREEN_HOME, s.current_screen);
    TEST_ASSERT_EQUAL(UI_MODE_DUAL, s.current_mode);
    TEST_ASSERT_FALSE(s.session_active);
    TEST_ASSERT_TRUE(s.screen_changed);  // Force initial draw
}

// =============================================================================
// NAVIGATION TESTS
// =============================================================================

void test_next_screen_cycles(void) {
    TEST_ASSERT_EQUAL(SCREEN_HOME, state.current_screen);

    ui_next_screen(&state);
    TEST_ASSERT_EQUAL(SCREEN_SESSION, state.current_screen);

    ui_next_screen(&state);
    TEST_ASSERT_EQUAL(SCREEN_STATS, state.current_screen);

    ui_next_screen(&state);
    TEST_ASSERT_EQUAL(SCREEN_SETTINGS, state.current_screen);

    ui_next_screen(&state);
    TEST_ASSERT_EQUAL(SCREEN_BATTERY, state.current_screen);

    ui_next_screen(&state);
    TEST_ASSERT_EQUAL(SCREEN_SAFETY, state.current_screen);

    // Should wrap to home
    ui_next_screen(&state);
    TEST_ASSERT_EQUAL(SCREEN_HOME, state.current_screen);
}

void test_prev_screen_cycles(void) {
    TEST_ASSERT_EQUAL(SCREEN_HOME, state.current_screen);

    // Should wrap to last screen
    ui_prev_screen(&state);
    TEST_ASSERT_EQUAL(SCREEN_SAFETY, state.current_screen);

    ui_prev_screen(&state);
    TEST_ASSERT_EQUAL(SCREEN_BATTERY, state.current_screen);
}

void test_goto_screen_direct(void) {
    ui_goto_screen(&state, SCREEN_SETTINGS);
    TEST_ASSERT_EQUAL(SCREEN_SETTINGS, state.current_screen);
    TEST_ASSERT_EQUAL(SCREEN_HOME, state.previous_screen);
}

void test_goto_screen_invalid(void) {
    ui_goto_screen(&state, (Screen)99);  // Invalid
    TEST_ASSERT_EQUAL(SCREEN_HOME, state.current_screen);  // Unchanged
}

void test_navigation_sets_changed_flag(void) {
    ui_screen_changed(&state);  // Clear flag
    TEST_ASSERT_FALSE(state.screen_changed);

    ui_next_screen(&state);
    TEST_ASSERT_TRUE(state.screen_changed);
}

void test_screen_changed_clears_flag(void) {
    state.screen_changed = true;
    TEST_ASSERT_TRUE(ui_screen_changed(&state));
    TEST_ASSERT_FALSE(ui_screen_changed(&state));  // Second call returns false
}

void test_previous_screen_tracked(void) {
    ui_goto_screen(&state, SCREEN_STATS);
    ui_goto_screen(&state, SCREEN_BATTERY);

    TEST_ASSERT_EQUAL(SCREEN_BATTERY, state.current_screen);
    TEST_ASSERT_EQUAL(SCREEN_STATS, state.previous_screen);
}

// =============================================================================
// MODE CYCLING TESTS
// =============================================================================

void test_next_mode_cycles(void) {
    UITreatmentMode mode = UI_MODE_RED_ONLY;

    mode = ui_next_mode(mode);
    TEST_ASSERT_EQUAL(UI_MODE_NIR_ONLY, mode);

    mode = ui_next_mode(mode);
    TEST_ASSERT_EQUAL(UI_MODE_DUAL, mode);

    mode = ui_next_mode(mode);
    TEST_ASSERT_EQUAL(UI_MODE_ALTERNATING, mode);

    // Should wrap to RED (skipping OFF)
    mode = ui_next_mode(mode);
    TEST_ASSERT_EQUAL(UI_MODE_RED_ONLY, mode);
}

void test_prev_mode_cycles(void) {
    UITreatmentMode mode = UI_MODE_RED_ONLY;

    // Should wrap to last mode
    mode = ui_prev_mode(mode);
    TEST_ASSERT_EQUAL(UI_MODE_ALTERNATING, mode);

    mode = ui_prev_mode(mode);
    TEST_ASSERT_EQUAL(UI_MODE_DUAL, mode);
}

// =============================================================================
// BUTTON HANDLING - HOME SCREEN
// =============================================================================

void test_home_button1_starts_session(void) {
    state.current_screen = SCREEN_HOME;
    state.session_active = false;

    UIAction action = ui_handle_button(&state, BUTTON_1_SHORT);
    TEST_ASSERT_EQUAL(ACTION_START_SESSION, action);
}

void test_home_button1_no_action_when_active(void) {
    state.current_screen = SCREEN_HOME;
    state.session_active = true;

    UIAction action = ui_handle_button(&state, BUTTON_1_SHORT);
    TEST_ASSERT_EQUAL(ACTION_NONE, action);
}

void test_home_button2_navigates(void) {
    state.current_screen = SCREEN_HOME;

    UIAction action = ui_handle_button(&state, BUTTON_2_SHORT);
    TEST_ASSERT_EQUAL(ACTION_NAVIGATE_NEXT, action);
    TEST_ASSERT_EQUAL(SCREEN_SESSION, state.current_screen);
}

void test_home_long_press_changes_mode(void) {
    state.current_screen = SCREEN_HOME;
    state.session_active = false;
    state.current_mode = UI_MODE_DUAL;

    UIAction action = ui_handle_button(&state, BUTTON_1_LONG);
    TEST_ASSERT_EQUAL(ACTION_CHANGE_MODE, action);
    TEST_ASSERT_EQUAL(UI_MODE_ALTERNATING, state.current_mode);
}

void test_home_long_press_no_change_when_active(void) {
    state.current_screen = SCREEN_HOME;
    state.session_active = true;
    state.current_mode = UI_MODE_DUAL;

    UIAction action = ui_handle_button(&state, BUTTON_1_LONG);
    TEST_ASSERT_EQUAL(ACTION_NONE, action);
    TEST_ASSERT_EQUAL(UI_MODE_DUAL, state.current_mode);  // Unchanged
}

// =============================================================================
// BUTTON HANDLING - SESSION SCREEN
// =============================================================================

void test_session_button1_stops_when_active(void) {
    state.current_screen = SCREEN_SESSION;
    state.session_active = true;

    UIAction action = ui_handle_button(&state, BUTTON_1_SHORT);
    TEST_ASSERT_EQUAL(ACTION_STOP_SESSION, action);
}

void test_session_button1_starts_when_inactive(void) {
    state.current_screen = SCREEN_SESSION;
    state.session_active = false;

    UIAction action = ui_handle_button(&state, BUTTON_1_SHORT);
    TEST_ASSERT_EQUAL(ACTION_START_SESSION, action);
}

// =============================================================================
// BUTTON HANDLING - SETTINGS SCREEN
// =============================================================================

void test_settings_button2_cycles_modes(void) {
    state.current_screen = SCREEN_SETTINGS;
    state.selected_mode = UI_MODE_RED_ONLY;

    UIAction action = ui_handle_button(&state, BUTTON_2_SHORT);
    TEST_ASSERT_EQUAL(ACTION_SELECT_SETTING, action);
    TEST_ASSERT_EQUAL(UI_MODE_NIR_ONLY, state.selected_mode);
}

void test_settings_button1_confirms(void) {
    state.current_screen = SCREEN_SETTINGS;
    state.current_mode = UI_MODE_DUAL;
    state.selected_mode = UI_MODE_RED_ONLY;

    UIAction action = ui_handle_button(&state, BUTTON_1_SHORT);
    TEST_ASSERT_EQUAL(ACTION_CONFIRM_SETTING, action);
    TEST_ASSERT_EQUAL(UI_MODE_RED_ONLY, state.current_mode);  // Applied
}

// =============================================================================
// BUTTON HANDLING - INFO SCREENS
// =============================================================================

void test_info_screens_button1_no_action(void) {
    Screen info_screens[] = {SCREEN_STATS, SCREEN_BATTERY, SCREEN_SAFETY};

    for (int i = 0; i < 3; i++) {
        state.current_screen = info_screens[i];
        UIAction action = ui_handle_button(&state, BUTTON_1_SHORT);
        TEST_ASSERT_EQUAL(ACTION_NONE, action);
    }
}

void test_info_screens_button2_navigates(void) {
    state.current_screen = SCREEN_STATS;

    UIAction action = ui_handle_button(&state, BUTTON_2_SHORT);
    TEST_ASSERT_EQUAL(ACTION_NAVIGATE_NEXT, action);
    TEST_ASSERT_EQUAL(SCREEN_SETTINGS, state.current_screen);
}

// =============================================================================
// SESSION STATE TESTS
// =============================================================================

void test_set_session_active_navigates_to_session(void) {
    state.current_screen = SCREEN_HOME;

    ui_set_session_active(&state, true);
    TEST_ASSERT_TRUE(state.session_active);
    TEST_ASSERT_EQUAL(SCREEN_SESSION, state.current_screen);
}

void test_set_session_inactive_stays_on_screen(void) {
    state.current_screen = SCREEN_SESSION;
    state.session_active = true;

    ui_set_session_active(&state, false);
    TEST_ASSERT_FALSE(state.session_active);
    TEST_ASSERT_EQUAL(SCREEN_SESSION, state.current_screen);  // Stays
}

// =============================================================================
// STRING HELPER TESTS
// =============================================================================

void test_mode_names(void) {
    TEST_ASSERT_EQUAL_STRING("OFF", ui_get_mode_name(UI_MODE_OFF));
    TEST_ASSERT_EQUAL_STRING("RED", ui_get_mode_name(UI_MODE_RED_ONLY));
    TEST_ASSERT_EQUAL_STRING("NIR", ui_get_mode_name(UI_MODE_NIR_ONLY));
    TEST_ASSERT_EQUAL_STRING("DUAL", ui_get_mode_name(UI_MODE_DUAL));
    TEST_ASSERT_EQUAL_STRING("ALT", ui_get_mode_name(UI_MODE_ALTERNATING));
}

void test_screen_names(void) {
    TEST_ASSERT_EQUAL_STRING("Home", ui_get_screen_name(SCREEN_HOME));
    TEST_ASSERT_EQUAL_STRING("Session", ui_get_screen_name(SCREEN_SESSION));
    TEST_ASSERT_EQUAL_STRING("Stats", ui_get_screen_name(SCREEN_STATS));
    TEST_ASSERT_EQUAL_STRING("Settings", ui_get_screen_name(SCREEN_SETTINGS));
    TEST_ASSERT_EQUAL_STRING("Battery", ui_get_screen_name(SCREEN_BATTERY));
    TEST_ASSERT_EQUAL_STRING("Safety", ui_get_screen_name(SCREEN_SAFETY));
}

// =============================================================================
// EDGE CASES
// =============================================================================

void test_button_none_returns_none(void) {
    UIAction action = ui_handle_button(&state, BUTTON_NONE);
    TEST_ASSERT_EQUAL(ACTION_NONE, action);
}

// =============================================================================
// TEST RUNNER
// =============================================================================

int main(int argc, char **argv) {
    UNITY_BEGIN();

    // Initialization
    RUN_TEST(test_init_defaults);

    // Navigation
    RUN_TEST(test_next_screen_cycles);
    RUN_TEST(test_prev_screen_cycles);
    RUN_TEST(test_goto_screen_direct);
    RUN_TEST(test_goto_screen_invalid);
    RUN_TEST(test_navigation_sets_changed_flag);
    RUN_TEST(test_screen_changed_clears_flag);
    RUN_TEST(test_previous_screen_tracked);

    // Mode cycling
    RUN_TEST(test_next_mode_cycles);
    RUN_TEST(test_prev_mode_cycles);

    // Home screen buttons
    RUN_TEST(test_home_button1_starts_session);
    RUN_TEST(test_home_button1_no_action_when_active);
    RUN_TEST(test_home_button2_navigates);
    RUN_TEST(test_home_long_press_changes_mode);
    RUN_TEST(test_home_long_press_no_change_when_active);

    // Session screen buttons
    RUN_TEST(test_session_button1_stops_when_active);
    RUN_TEST(test_session_button1_starts_when_inactive);

    // Settings screen buttons
    RUN_TEST(test_settings_button2_cycles_modes);
    RUN_TEST(test_settings_button1_confirms);

    // Info screens
    RUN_TEST(test_info_screens_button1_no_action);
    RUN_TEST(test_info_screens_button2_navigates);

    // Session state
    RUN_TEST(test_set_session_active_navigates_to_session);
    RUN_TEST(test_set_session_inactive_stays_on_screen);

    // String helpers
    RUN_TEST(test_mode_names);
    RUN_TEST(test_screen_names);

    // Edge cases
    RUN_TEST(test_button_none_returns_none);

    return UNITY_END();
}
