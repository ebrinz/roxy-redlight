/**
 * Roxy RedLight v2.0 - Display Module
 *
 * UI for LilyGO T-Display S3 (170x320 ST7789)
 */

#ifndef DISPLAY_H
#define DISPLAY_H

#include <TFT_eSPI.h>
#include "config.h"

// =============================================================================
// MENU SCREENS
// =============================================================================

typedef enum {
    SCREEN_HOME = 0,    // Main idle screen
    SCREEN_SESSION,     // Active session display
    SCREEN_STATS,       // Usage statistics
    SCREEN_SETTINGS,    // Mode selection
    SCREEN_BATTERY,     // Battery details
    SCREEN_SAFETY,      // Safety status
    SCREEN_COUNT
} Screen;

// =============================================================================
// DISPLAY CLASS
// =============================================================================

class Display {
public:
    Display();

    void begin();
    void update();
    void clear();
    void setBrightness(uint8_t level);  // 0-255

    // Screen navigation
    void setScreen(Screen screen);
    Screen getScreen();
    void nextScreen();
    void prevScreen();

    // Screen renderers
    void showHome(float voltage, uint8_t battPercent, TreatmentMode mode);
    void showSession(unsigned long elapsedSec, unsigned long totalSec,
                     TreatmentMode mode, bool redOn, bool nirOn);
    void showStats(uint32_t sessions, uint32_t minutes, uint8_t dailySessions);
    void showSettings(TreatmentMode mode, int selectedIndex);
    void showBattery(float voltage, uint8_t percent, bool charging);
    void showSafety(float voltage, float temp, bool overVoltage,
                    bool underVoltage, bool thermal);

    // Alerts
    void showAlert(const char* title, const char* message, uint16_t color);
    void showEmergency(const char* reason);

    // Helpers
    void drawHeader(const char* title);
    void drawFooter(const char* left, const char* right);
    void drawBatteryIcon(int x, int y, uint8_t percent, bool charging);
    void drawProgressBar(int x, int y, int w, int h,
                         float progress, uint16_t color);
    void drawLEDIndicator(int x, int y, bool redOn, bool nirOn);

private:
    TFT_eSPI tft;
    TFT_eSprite sprite;  // For flicker-free updates
    Screen currentScreen;
    bool needsRedraw;
    unsigned long lastUpdate;
};

// Global display instance
extern Display display;

#endif // DISPLAY_H
