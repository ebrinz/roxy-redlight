# Roxy RedLight Firmware

ESP32-S3 firmware for the dual-wavelength photobiomodulation helmet with LCD display.

## Features

- **Dual-channel PWM** - Independent control of 650nm red and 850nm NIR LEDs
- **Multiple treatment modes** - Red-only, NIR-only, Dual, Alternating
- **1.9" LCD display** - Real-time session progress, battery status, statistics
- **6 UI screens** - Home, Session, Stats, Settings, Battery, Safety
- **Two-button navigation** - Context-sensitive controls
- **Session timer** - 20-minute default with auto-shutoff
- **Battery monitoring** - Low voltage warning and emergency cutoff
- **Persistent storage** - Tracks lifetime sessions and minutes
- **Comprehensive safety** - Voltage, thermal, and session limit protections
- **Unit tested** - 23 tests for all safety-critical functions

## Hardware Requirements

**Board:** LilyGO T-Display S3 (ESP32-S3 with 1.9" ST7789 LCD)

| Component | Pin | Function |
|-----------|-----|----------|
| Red LED MOSFET | GPIO43 | PWM control for 650nm array |
| NIR LED MOSFET | GPIO44 | PWM control for 850nm array |
| Button 1 | GPIO0 | Boot button - Start/stop, navigate |
| Button 2 | GPIO14 | Mode select, screen navigation |
| Battery ADC | GPIO4 | Voltage monitoring via divider |
| Buzzer | GPIO21 | Audio feedback (optional) |
| Temp sensor | GPIO7 | NTC thermistor (optional) |
| LCD | Built-in | ST7789 170x320 display |

### Battery Voltage Divider

```
VBAT (6.0-8.4V)
     │
    [100kΩ]
     ├────── GPIO1 (ADC)
    [33kΩ]
     │
    GND
```

## Building & Flashing

### Prerequisites

1. Install [PlatformIO](https://platformio.org/install)
2. Connect T-Display S3 via USB-C

### Commands

```bash
cd firmware/esp32

# Compile for T-Display S3
pio run -e lilygo-t-display-s3

# Upload to device
pio run -e lilygo-t-display-s3 --target upload

# Serial monitor (115200 baud)
pio device monitor

# Clean build
pio run -e lilygo-t-display-s3 --target clean
```

### First Upload

The LilyGO T-Display S3 may require holding the BOOT button (GPIO0) while connecting USB for first flash.

## Usage

### Button Controls

The T-Display S3 has two built-in buttons for navigation:

| Button | Home Screen | Session Screen | Settings Screen |
|--------|-------------|----------------|-----------------|
| **Button 1** | Start session | Stop session | Select option |
| **Button 2** | Next screen | Next screen | Next option |

**Navigation Flow:** Home → Session → Stats → Settings → Battery → Safety → Home

### Treatment Modes

| Mode | Red (650nm) | NIR (850nm) | Description |
|------|-------------|-------------|-------------|
| `RED_ONLY` | 100% | OFF | Surface follicles, maintenance |
| `NIR_ONLY` | OFF | 100% | Deep follicles, advanced thinning |
| `DUAL` | 100% | 100% | Comprehensive treatment (default) |
| `ALTERNATING` | 30s on/off | 30s on/off | Experimental pulsed protocol |

Mode is indicated by blink count (1=RED, 2=NIR, 3=DUAL, 4=ALT) and saved to flash.

### Session Behavior

1. Press button to start
2. Status LED blinks slowly during session
3. Progress printed to Serial every 30 seconds
4. Auto-stops at 20 minutes with completion tone
5. Safety cutoff at 30 minutes max

## Safety Features

The firmware includes multiple layers of protection:

### Voltage Protection

| Voltage | State | Action |
|---------|-------|--------|
| **>8.6V** | **OVER-VOLTAGE** | **Emergency shutdown, alarm** |
| 8.4V | Full | Normal operation |
| 7.4V | Nominal | Normal operation |
| 6.8V | Low | Warning beep, continues |
| 6.2V | Critical | Emergency shutdown |
| <6.0V | Cutoff | Won't operate |

### Thermal Protection (Optional)

If NTC thermistor installed on GPIO7:

| Temp | State | Action |
|------|-------|--------|
| <40°C | Normal | Full power |
| 40°C | Warning | Power reduced to 50% |
| 45°C | Critical | Emergency shutdown |

Enable in `config.h`: `#define TEMP_ENABLED true`

### Session Limits

| Limit | Default | Purpose |
|-------|---------|---------|
| Max session time | 30 min | Prevents accidental overexposure |
| Daily session limit | 3 | Prevents overuse |
| Session gap | 60 min | Ensures rest between treatments |

### Power Limits (Design Reference)

Based on clinical research, safe therapeutic range:
- **Target:** 5 mW/cm² (optimal for hair growth)
- **Safe range:** 1-50 mW/cm²
- **Danger zone:** >100 mW/cm² (thermal damage risk)

The LED array is designed to deliver ~5 mW/cm² at scalp level.

### Emergency Shutdown

Triggers immediate LED shutoff + alarm for:
- Over-voltage detected
- Under-voltage (battery depleted)
- Thermal cutoff (if sensor enabled)

```
!!! EMERGENCY SHUTDOWN !!!
OVER-VOLTAGE DETECTED!
DANGER: Battery voltage 9.12V exceeds safe limit!
Check charger and BMS immediately.
```

## Serial Output

Connect at 115200 baud to see:

```
=================================
  Roxy RedLight v2.0
  Dual Wavelength PBM Helmet
  LilyGO T-Display S3
=================================

Display initialized
Battery: 7.82V (78%)
Lifetime sessions: 42
Lifetime minutes: 840
Current mode: DUAL
Ready. Use buttons to navigate screens.

Session started - Mode: DUAL, Duration: 20 min
Session: 0:30 elapsed, 19:30 remaining
Session: 1:00 elapsed, 19:00 remaining
...
Session complete!
Session stopped. Duration: 20:00
Lifetime: 43 sessions, 860 minutes
```

## Configuration

Edit `include/config.h` to customize:

```c
// Pin definitions (T-Display S3)
#define PIN_RED_LED     43    // GPIO43 - Red MOSFET
#define PIN_NIR_LED     44    // GPIO44 - NIR MOSFET
#define PIN_BUTTON_1    0     // Boot button
#define PIN_BUTTON_2    14    // Second button
#define PIN_VBAT_ADC    4     // Battery ADC

// Session duration (minutes)
#define DEFAULT_SESSION_MINUTES     20
#define MAX_SESSION_MINUTES         30    // Safety hard limit

// Alternating mode period (seconds)
#define ALTERNATE_PERIOD_SEC        30

// Battery thresholds (volts)
#define VBAT_OVERVOLTAGE            8.6   // Emergency shutoff
#define VBAT_LOW                    6.8   // Warning
#define VBAT_CUTOFF                 6.2   // Emergency shutoff

// Thermal protection (optional)
#define TEMP_ENABLED                false // Set true if thermistor installed
#define TEMP_WARNING_C              40    // Reduce power at this temp
#define TEMP_CUTOFF_C               45    // Emergency shutoff

// Session limits (overuse protection)
#define MAX_DAILY_SESSIONS          3     // Per 24-hour period
#define MIN_SESSION_GAP_MIN         60    // Minutes between sessions

// PWM frequency (Hz)
#define PWM_FREQ                    1000
```

## Pin Mapping

```
┌─────────────────────────────────────┐
│      LilyGO T-Display S3            │
│       (ESP32-S3 + 1.9" LCD)         │
├─────────────────────────────────────┤
│                                     │
│  3V3  ◄──────── Regulated 3.3V      │
│  GND  ◄──────── Ground              │
│                                     │
│  GPIO0  ──────► Button 1 (Boot)     │
│  GPIO4  ──────► ADC: Battery voltage│
│  GPIO7  ──────► Temp sensor (opt)   │
│  GPIO14 ──────► Button 2            │
│  GPIO15 ──────► Power hold          │
│  GPIO21 ──────► Buzzer (optional)   │
│  GPIO43 ──────► RED MOSFET (PWM)    │
│  GPIO44 ──────► NIR MOSFET (PWM)    │
│                                     │
│  LCD pins managed by TFT_eSPI       │
│  USB-C ◄──────► Programming/power   │
│                                     │
└─────────────────────────────────────┘
```

## Persistent Data

Stored in ESP32 flash (Preferences library):

| Key | Type | Description |
|-----|------|-------------|
| `sessions` | uint32 | Lifetime session count |
| `minutes` | uint32 | Lifetime treatment minutes |
| `mode` | uint8 | Last used treatment mode |

Data persists across power cycles and firmware updates.

## Troubleshooting

### LEDs don't turn on
- Check MOSFET gate connections to GPIO43/GPIO44
- Verify MOSFETs are logic-level (Vgs(th) < 3.3V)
- Test with `ledcWrite(0, 255)` in setup()

### Buttons not responding
- GPIO0 and GPIO14 are the built-in buttons
- Check debounce timing in config.h
- Test with `Serial.println(digitalRead(PIN_BUTTON_1))`

### Display not working
- T-Display S3 LCD uses pins 5,6,7,11,12,38 - don't use these for other purposes
- Check TFT_eSPI configuration in platformio.ini build_flags

### Battery reading incorrect
- Verify voltage divider resistor values (100k/33k)
- Calibrate `VBAT_DIVIDER_RATIO` in config.h if needed
- Check ADC connected to GPIO4

### Can't upload firmware
- Hold BOOT button (GPIO0) while connecting USB
- Try different USB-C cable (data, not charge-only)
- Check COM port in platformio.ini

## Testing

The firmware includes comprehensive unit tests for safety-critical functions and UI logic, plus on-device hardware tests.

### Running Tests

```bash
cd firmware/esp32

# Run all unit tests on host machine (no hardware needed)
pio test -e native

# Run specific test suites
pio test -e native -f test_safety    # Safety module tests
pio test -e native -f test_ui        # UI/navigation tests

# Run hardware tests ON DEVICE (requires T-Display S3 connected)
pio test -e hardware
```

### Unit Test Coverage (Native)

| Category | Tests | Purpose |
|----------|-------|---------|
| **Voltage** | 5 | Over/under voltage detection, edge cases |
| **Thermal** | 5 | Temperature limits, derating, sensor disabled |
| **Session** | 4 | Daily limits, gap enforcement, duration |
| **Power** | 2 | Irradiance limits |
| **Comprehensive** | 5 | Full safety check integration |
| **Error Messages** | 2 | All errors have valid messages |
| **Navigation** | 7 | Screen cycling, goto, change flags |
| **Mode Cycling** | 2 | Treatment mode next/prev |
| **Button Handling** | 10 | Per-screen button behavior |
| **Session State** | 2 | Active/inactive transitions |

### Hardware Test Coverage (On-Device)

| Category | Tests | Purpose |
|----------|-------|---------|
| **PWM Output** | 4 | LED MOSFET control, duty cycle range |
| **ADC/Battery** | 3 | Voltage reading valid, stable, in range |
| **Buttons** | 2 | Pullup state verification |
| **Buzzer** | 1 | Audio output |
| **Power Hold** | 1 | GPIO15 power control |
| **Integration** | 1 | Visual LED sequence test |

### Testable Modules

```
lib/safety/          # Safety checks (voltage, thermal, session limits)
├── safety.h
└── safety.cpp

lib/ui/              # UI state machine (navigation, buttons, modes)
├── ui.h
└── ui.cpp

test/test_safety/    # Native safety tests (23 tests)
test/test_ui/        # Native UI tests (28 tests)
test/test_hardware/  # On-device hardware tests (12 tests)
```

### Test Output Example

```
test/test_safety/test_safety.cpp:42:test_voltage_normal_range       [PASSED]
test/test_safety/test_safety.cpp:49:test_voltage_overvoltage_detection [PASSED]
...
test/test_ui/test_ui.cpp:25:test_init_defaults                      [PASSED]
test/test_ui/test_ui.cpp:35:test_next_screen_cycles                 [PASSED]
...
-----------------------
51 Tests 0 Failures 0 Ignored
OK
```

## Future Enhancements

- [ ] Pulsed mode (10Hz) based on emerging research
- [x] Temperature monitoring with thermal cutoff
- [x] LCD display with real-time UI
- [x] Comprehensive safety testing
- [ ] OTA firmware updates

## License

MIT License - See project root LICENSE file
