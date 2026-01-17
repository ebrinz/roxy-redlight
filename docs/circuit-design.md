# Roxy RedLight Circuit Design

## Overview

This document details the electrical design for the Roxy RedLight LED therapy helmet, including power management, LED driving, and microcontroller integration.

## Design Goals

1. Drive 60-100 red LEDs at ~20mA each (1.2-2A total)
2. 2+ hours runtime on battery
3. USB-C rechargeable
4. Timer-controlled sessions
5. Battery monitoring and protection
6. Simple, reproducible design

---

## System Block Diagram

```
┌─────────────────────────────────────────────────────────────────────┐
│                        ROXY REDLIGHT v1.0                           │
├─────────────────────────────────────────────────────────────────────┤
│                                                                     │
│  ┌──────────┐    ┌──────────┐    ┌──────────┐    ┌──────────────┐  │
│  │  USB-C   │───▶│  TP4056  │───▶│  2x18650 │───▶│   MT3608     │  │
│  │  Input   │    │  Charger │    │  Battery │    │  Boost 5V    │  │
│  └──────────┘    └──────────┘    │  Pack    │    └──────┬───────┘  │
│                                  └────┬─────┘           │          │
│                                       │                 │          │
│                                       │ VBAT            │ 5V       │
│                                       ▼                 ▼          │
│                               ┌──────────────────────────┐         │
│                               │        ESP32-C3          │         │
│                               │                          │         │
│                               │  GPIO ──▶ ADC (VBAT)     │         │
│                               │  GPIO ──▶ LED Driver     │         │
│                               │  GPIO ──▶ Status LED     │         │
│                               │  GPIO ──▶ Button         │         │
│                               └────────────┬─────────────┘         │
│                                            │                       │
│                                            ▼                       │
│                               ┌──────────────────────────┐         │
│                               │    MOSFET LED Driver     │         │
│                               │    (Logic-level N-CH)    │         │
│                               └────────────┬─────────────┘         │
│                                            │                       │
│                                            ▼                       │
│                               ┌──────────────────────────┐         │
│                               │      LED Array           │         │
│                               │   10 parallel strings    │         │
│                               │   6-10 LEDs per string   │         │
│                               │   + current limit R      │         │
│                               └──────────────────────────┘         │
│                                                                     │
└─────────────────────────────────────────────────────────────────────┘
```

---

## Power System

### Battery Pack

**Configuration:** 2S1P (2x 18650 in series)

| Parameter | Value |
|-----------|-------|
| Nominal Voltage | 7.4V (2 x 3.7V) |
| Fully Charged | 8.4V (2 x 4.2V) |
| Cutoff Voltage | 6.0V (2 x 3.0V) |
| Capacity | 2600-3500 mAh |
| Max Discharge | 5A continuous |

**Reasoning:** Series configuration provides higher voltage, reducing current through boost converter and improving efficiency. A 2600mAh pack can supply 2A for ~1.3 hours, or with typical 1.5A LED load, ~1.7 hours runtime.

### Charging Circuit

**Module:** TP4056 with DW01A protection (USB-C variant)

```
                    ┌─────────────────┐
    USB-C ─────────▶│     TP4056      │
    5V input        │                 │
                    │  CHRG ──▶ LED   │ (Charging indicator)
                    │  STDBY ──▶ LED  │ (Charge complete)
                    │                 │
                    │  B+ ────────────┼──▶ Battery +
                    │  B- ────────────┼──▶ Battery -
                    │                 │
                    │  OUT+ ──────────┼──▶ Load +
                    │  OUT- ──────────┼──▶ Load -
                    └─────────────────┘
```

**Note:** Standard TP4056 is designed for 1S LiPo. For 2S configuration, use a **2S BMS board** instead:

**Recommended:** 2S 7.4V 8A BMS Protection Board with USB-C charging input

### Voltage Regulation

**Module:** MT3608 DC-DC Boost Converter (or buck if using higher voltage)

For 2S battery (6-8.4V input), we can either:
1. **Boost to 12V** - to drive longer LED strings
2. **Use direct battery voltage** - simpler, fewer components

**Option A: Direct Drive (Recommended for v1)**

With 7.4V nominal battery, we can drive:
- 3x red LEDs in series (3 × 2.0V = 6.0V forward drop)
- Leaves ~1.4V headroom for current-limiting resistor

```
LED String (3 LEDs + resistor):

    VBAT (7.4V)
        │
        ▼
    ┌───────┐
    │  LED  │ Vf = 2.0V
    └───┬───┘
        │
    ┌───────┐
    │  LED  │ Vf = 2.0V
    └───┬───┘
        │
    ┌───────┐
    │  LED  │ Vf = 2.0V
    └───┬───┘
        │
    ┌───────┐
    │  68Ω  │  R = (7.4 - 6.0) / 0.020 = 70Ω → 68Ω standard
    └───┬───┘
        │
        ▼
    MOSFET Drain
```

**Option B: Boost to 12V**

Allows 5 LEDs per string, fewer parallel strings needed.

---

## LED Array Design

### LED Specifications

| Parameter | Value |
|-----------|-------|
| Type | 5mm through-hole, clear lens |
| Wavelength | 650nm (red) |
| Forward Voltage | 1.8-2.2V (typ. 2.0V) |
| Forward Current | 20mA nominal |
| Viewing Angle | 20-30° (narrow) for scalp focus |
| Power per LED | ~40mW optical output |

### Array Configuration (Direct Drive @ 7.4V)

```
┌────────────────────────────────────────────────────────────────┐
│                     LED Array Layout                           │
├────────────────────────────────────────────────────────────────┤
│                                                                │
│   String 1    String 2    String 3   ...   String 20          │
│      │           │           │                  │              │
│   ┌──┴──┐     ┌──┴──┐     ┌──┴──┐          ┌──┴──┐           │
│   │ LED │     │ LED │     │ LED │          │ LED │           │
│   └──┬──┘     └──┬──┘     └──┬──┘          └──┬──┘           │
│   ┌──┴──┐     ┌──┴──┐     ┌──┴──┐          ┌──┴──┐           │
│   │ LED │     │ LED │     │ LED │          │ LED │           │
│   └──┬──┘     └──┬──┘     └──┬──┘          └──┬──┘           │
│   ┌──┴──┐     ┌──┴──┐     ┌──┴──┐          ┌──┴──┐           │
│   │ LED │     │ LED │     │ LED │          │ LED │           │
│   └──┬──┘     └──┬──┘     └──┬──┘          └──┬──┘           │
│   ┌──┴──┐     ┌──┴──┐     ┌──┴──┐          ┌──┴──┐           │
│   │ 68Ω │     │ 68Ω │     │ 68Ω │          │ 68Ω │           │
│   └──┬──┘     └──┬──┘     └──┬──┘          └──┬──┘           │
│      │           │           │                  │              │
│      └───────────┴───────────┴────────┬─────────┘              │
│                                       │                        │
│                                       ▼                        │
│                               MOSFET Drain                     │
│                                                                │
└────────────────────────────────────────────────────────────────┘

Configuration: 20 strings × 3 LEDs = 60 LEDs total
Current: 20 strings × 20mA = 400mA total
```

For 90 LEDs: 30 strings × 3 LEDs = 600mA total

### Current Limiting Resistor Calculation

```
R = (Vbat - (n × Vf)) / I

Where:
  Vbat = 7.4V (nominal) to 6.0V (cutoff)
  n = 3 (LEDs per string)
  Vf = 2.0V (forward voltage per LED)
  I = 20mA (target current)

At 7.4V: R = (7.4 - 6.0) / 0.020 = 70Ω → use 68Ω
At 6.0V: I = (6.0 - 6.0) / 68 = 0mA ← Problem!
```

**Issue:** At low battery, forward voltage equals supply voltage. Need either:
1. Use only 2 LEDs per string (headroom at all voltages)
2. Use boost converter to maintain voltage
3. Accept reduced brightness at low battery

**Revised: 2 LEDs per string**

```
At 7.4V: R = (7.4 - 4.0) / 0.020 = 170Ω → use 150Ω (22mA)
At 6.0V: I = (6.0 - 4.0) / 150 = 13mA (still functional)
```

**Updated Configuration:**
- 30 strings × 2 LEDs = 60 LEDs
- 45 strings × 2 LEDs = 90 LEDs
- Current: 30 × 22mA = 660mA (60 LEDs)
- Current: 45 × 22mA = 990mA (90 LEDs)

---

## MOSFET Switch

### Component Selection

**MOSFET:** IRLZ44N or AO3400 (logic-level N-channel)

| Parameter | IRLZ44N | AO3400 |
|-----------|---------|--------|
| Package | TO-220 | SOT-23 |
| Vds max | 55V | 30V |
| Id max | 47A | 5.8A |
| Vgs(th) | 1-2V | 0.65-1V |
| Rds(on) | 22mΩ | 32mΩ |

For 1A load, power dissipation: P = I²R = 1² × 0.022 = 22mW (negligible heat)

### MOSFET Driver Circuit

```
                      VCC (3.3V from ESP32)
                           │
                           │
    ESP32 GPIO ───┬───────┤
                  │        │
                 ┌┴┐      ┌┴┐
                 │ │10kΩ  │ │10kΩ (pull-down)
                 │ │      │ │
                 └┬┘      └┬┘
                  │        │
                  │        │
                  └────────┼───────┐
                           │       │
                           │    ┌──┴──┐
                           │    │GATE │
                           │    │     │
                           │    │IRLZ │──▶ LED Array (Drain)
                           │    │44N  │
                           │    │     │
                           │    └──┬──┘
                           │       │
                           │      GND (Source)
                           │
                          GND
```

**Note:** For PWM dimming, ESP32 LEDC peripheral can drive the gate directly at up to 40MHz. Use 1kHz-10kHz for flicker-free operation.

---

## ESP32-C3 Connections

### Pinout

```
┌─────────────────────────────────────┐
│           ESP32-C3 Mini             │
├─────────────────────────────────────┤
│                                     │
│  3V3 ◄──────── Regulated 3.3V       │
│  GND ◄──────── Ground               │
│                                     │
│  GPIO0 ──────► Boot button          │
│  GPIO1 ──────► ADC: Battery voltage │
│  GPIO2 ──────► Status LED (onboard) │
│  GPIO3 ──────► MOSFET gate (PWM)    │
│  GPIO4 ──────► Start/Stop button    │
│  GPIO5 ──────► Buzzer (optional)    │
│                                     │
│  USB  ◄──────► Programming/debug    │
│                                     │
└─────────────────────────────────────┘
```

### Battery Voltage Monitoring

ESP32 ADC is 0-3.3V. Need voltage divider for 8.4V max battery:

```
    VBAT (6.0-8.4V)
         │
        ┌┴┐
        │ │ 100kΩ
        │ │
        └┬┘
         ├──────────► GPIO1 (ADC)
        ┌┴┐
        │ │ 33kΩ
        │ │
        └┬┘
         │
        GND

Vout = Vbat × (33k / (100k + 33k))
     = Vbat × 0.248

At 8.4V: Vout = 2.08V (safe for ADC)
At 6.0V: Vout = 1.49V
```

---

## Complete Schematic

```
                                    USB-C
                                      │
                                      ▼
┌─────────────────────────────────────────────────────────────────────────┐
│                                                                         │
│     ┌────────────┐         ┌────────────┐         ┌───────────────┐    │
│     │   2S BMS   │◄───────▶│  2×18650   │         │   ESP32-C3    │    │
│     │  Charger   │         │  Battery   │         │               │    │
│     └─────┬──────┘         └─────┬──────┘         │  GPIO1◄─┬─ADC │    │
│           │                      │                │         │     │    │
│           │                      │ VBAT           │  GPIO3──┼─PWM │    │
│           │                      │ (6-8.4V)       │         │     │    │
│           │                      │                │  GPIO4◄─┼─BTN │    │
│           └──────────────────────┼────────────────┤         │     │    │
│                                  │                │  3V3────┼─REG │    │
│                                  │                └─────────┼─────┘    │
│                                  │                          │          │
│                        ┌─────────┴─────────┐               │          │
│                        │    Voltage        │               │          │
│                        │    Divider        ├───────────────┘          │
│                        │   100k/33k        │                          │
│                        └─────────┬─────────┘                          │
│                                  │                                     │
│                                  ▼                                     │
│     ┌────────────────────────────────────────────────────────────┐    │
│     │                                                            │    │
│     │  ┌─────┐ ┌─────┐ ┌─────┐ ┌─────┐ ┌─────┐     ┌─────┐     │    │
│     │  │ LED │ │ LED │ │ LED │ │ LED │ │ LED │ ... │ LED │     │    │
│     │  │Str 1│ │Str 2│ │Str 3│ │Str 4│ │Str 5│     │Str N│     │    │
│     │  └──┬──┘ └──┬──┘ └──┬──┘ └──┬──┘ └──┬──┘     └──┬──┘     │    │
│     │     │       │       │       │       │           │        │    │
│     │     └───────┴───────┴───────┴───────┴─────┬─────┘        │    │
│     │                                           │               │    │
│     │                                      ┌────┴────┐          │    │
│     │                                      │ MOSFET  │◄─────────┘    │
│     │                                      │ IRLZ44N │  (GPIO3)      │
│     │                                      └────┬────┘               │
│     │                                           │                    │
│     │                                          GND                   │
│     │                        LED ARRAY                               │
│     └────────────────────────────────────────────────────────────────┘
│                                                                         │
└─────────────────────────────────────────────────────────────────────────┘
```

---

## Thermal Considerations

### Power Dissipation

| Component | Power | Notes |
|-----------|-------|-------|
| LEDs (60x) | 60 × 40mW = 2.4W | Distributed across helmet |
| LEDs (90x) | 90 × 40mW = 3.6W | May need ventilation |
| Resistors | 30 × 0.022² × 150 = 0.02W each | Negligible |
| MOSFET | 1² × 0.022 = 22mW | Negligible |
| ESP32 | ~100mW | Standard operation |

**Total:** ~2.5-4W, distributed. No heatsink needed.

### Thermal Cutoff (Optional)

Add NTC thermistor near LED cluster, monitored by ESP32 ADC. Shut down if temperature exceeds 45°C.

---

## Parts List (Full BOM)

| Qty | Component | Value/Part | Notes |
|-----|-----------|------------|-------|
| 1 | ESP32-C3 Mini | Seeed XIAO or similar | USB-C, compact |
| 60-90 | Red LEDs | 650nm, 5mm, 20mA | Clear lens, 20-30° |
| 30-45 | Resistors | 150Ω, 1/4W | Current limiting |
| 1 | MOSFET | IRLZ44N or AO3400 | Logic-level N-ch |
| 2 | 18650 cells | 2600mAh+ protected | Samsung, LG, Sony |
| 1 | 2S BMS | 7.4V 8A with USB-C | Charging + protection |
| 1 | 18650 holder | 2-cell series | Or spot-weld tabs |
| 2 | Resistors | 100kΩ, 33kΩ | Voltage divider |
| 1 | Resistor | 10kΩ | MOSFET pull-down |
| 1 | Tactile switch | 6mm | Start/stop button |
| 1 | LED | 3mm green | Status indicator |
| - | Wire | 22-24 AWG silicone | Flexible, heat-resistant |
| - | Connectors | JST-XH 2-pin | Battery, LED array |

---

## Assembly Notes

1. **Test LEDs first** - Verify all LEDs light up before soldering into array
2. **Match LED Vf** - Batch LEDs by forward voltage if possible
3. **Use silicone wire** - Flexibility for helmet mounting
4. **Conformal coat** - Protect ESP32 from sweat/humidity
5. **Strain relief** - At all wire entry points

---

## Firmware Pseudocode

```c
#define LED_PIN 3
#define BUTTON_PIN 4
#define VBAT_PIN 1
#define SESSION_MINUTES 20

void setup() {
    pinMode(LED_PIN, OUTPUT);
    pinMode(BUTTON_PIN, INPUT_PULLUP);
    ledcSetup(0, 1000, 8);  // PWM channel 0, 1kHz, 8-bit
    ledcAttachPin(LED_PIN, 0);
}

void loop() {
    if (buttonPressed()) {
        runSession(SESSION_MINUTES);
    }

    float vbat = readBatteryVoltage();
    if (vbat < 6.2) {
        lowBatteryWarning();
    }
}

void runSession(int minutes) {
    ledcWrite(0, 255);  // Full brightness

    for (int i = 0; i < minutes * 60; i++) {
        delay(1000);
        if (buttonPressed()) break;  // Cancel
    }

    ledcWrite(0, 0);  // Off
    playCompleteTone();
}
```

---

---

## Dual Wavelength Design (v2.0) - 650nm + 850nm NIR

### Efficacy Comparison: Red vs NIR vs Combined

| Wavelength | Hair Density Change | Depth | Evidence |
|------------|---------------------|-------|----------|
| **650-660nm (Red)** | +17.2 hairs/cm² | 1-2mm | Strong (multiple RCTs) |
| **830nm (NIR)** | "Greater efficacy than 632, 670, 785nm" | 3-4mm | Moderate |
| **Combined (630-690 + 820-970nm)** | Significant increase (p<.01) | Full depth | Moderate |
| **nNIR (580-1040nm broad)** | 5.88x more follicles vs white | Deep | In vivo mouse |

**Key findings:**
- nNIR induced 341.9% anagen score vs 202.1% for white light (1.69x better)
- 850nm penetrates 3-4mm deeper, reaching miniaturized follicles
- 830nm specifically showed greater efficacy than other tested wavelengths
- Combined wavelengths: 91.84% showed hair condition improvement at 24 weeks

### 850nm NIR LED Specifications

| Parameter | Value |
|-----------|-------|
| Wavelength | 850nm (invisible to eye) |
| Forward Voltage | 1.4-1.6V (lower than red!) |
| Forward Current | 20mA |
| Viewing Angle | 60° (wider than red) |
| Package | 5mm through-hole, clear lens |
| Power Output | ~30-40mW radiant |

**Note:** NIR LEDs have ~0.5V lower Vf than red LEDs. This affects string design.

### Dual-Channel Circuit Design

```
┌──────────────────────────────────────────────────────────────────────────┐
│                     ROXY REDLIGHT v2.0 (Dual Wavelength)                 │
├──────────────────────────────────────────────────────────────────────────┤
│                                                                          │
│     ┌──────────┐    ┌──────────┐    ┌──────────┐                        │
│     │  USB-C   │───▶│  2S BMS  │───▶│ 2×18650  │                        │
│     │  Input   │    │  Charger │    │ Battery  │                        │
│     └──────────┘    └──────────┘    └────┬─────┘                        │
│                                          │ VBAT (6-8.4V)                │
│                                          │                              │
│                              ┌───────────┴───────────┐                  │
│                              │       ESP32-C3        │                  │
│                              │                       │                  │
│                              │  GPIO3 ──► RED_MOSFET │                  │
│                              │  GPIO5 ──► NIR_MOSFET │                  │
│                              │  GPIO1 ◄── VBAT_ADC   │                  │
│                              │  GPIO4 ◄── BUTTON     │                  │
│                              └───────────────────────┘                  │
│                                    │           │                        │
│                                    ▼           ▼                        │
│                              ┌─────────┐ ┌─────────┐                    │
│                              │ MOSFET  │ │ MOSFET  │                    │
│                              │  RED    │ │  NIR    │                    │
│                              └────┬────┘ └────┬────┘                    │
│                                   │           │                         │
│        ┌──────────────────────────┴───────────┴──────────────────────┐  │
│        │                    LED ARRAY (Mixed)                        │  │
│        │                                                             │  │
│        │   ┌─────┐ ┌─────┐ ┌─────┐ ┌─────┐ ┌─────┐ ┌─────┐        │  │
│        │   │ RED │ │ NIR │ │ RED │ │ NIR │ │ RED │ │ NIR │  ...   │  │
│        │   │650nm│ │850nm│ │650nm│ │850nm│ │650nm│ │850nm│        │  │
│        │   └──┬──┘ └──┬──┘ └──┬──┘ └──┬──┘ └──┬──┘ └──┬──┘        │  │
│        │      │       │       │       │       │       │           │  │
│        │      ▼       ▼       ▼       ▼       ▼       ▼           │  │
│        │   RED_GND  NIR_GND  RED_GND NIR_GND RED_GND NIR_GND      │  │
│        │                                                          │  │
│        └──────────────────────────────────────────────────────────┘  │
│                                                                       │
└───────────────────────────────────────────────────────────────────────┘
```

### LED String Design (Dual Channel)

**Red LED Strings (650nm):**
```
VBAT (7.4V)
    │
┌───┴───┐
│  RED  │ Vf = 2.0V
└───┬───┘
┌───┴───┐
│  RED  │ Vf = 2.0V
└───┬───┘
┌───┴───┐
│ 150Ω  │ R = (7.4 - 4.0) / 0.022 = 155Ω
└───┬───┘
    │
RED_MOSFET_DRAIN
```
- 2 LEDs per string @ 22mA
- 30 strings = 60 red LEDs = 660mA

**NIR LED Strings (850nm):**
```
VBAT (7.4V)
    │
┌───┴───┐
│  NIR  │ Vf = 1.5V
└───┬───┘
┌───┴───┐
│  NIR  │ Vf = 1.5V
└───┬───┘
┌───┴───┐
│  NIR  │ Vf = 1.5V
└───┬───┘
┌───┴───┐
│ 100Ω  │ R = (7.4 - 4.5) / 0.020 = 145Ω → 100Ω (24mA)
└───┬───┘
    │
NIR_MOSFET_DRAIN
```
- 3 LEDs per string @ 24mA (lower Vf allows more LEDs!)
- 10 strings = 30 NIR LEDs = 240mA

**Total Current:** 660mA + 240mA = 900mA (both channels on)

### Updated ESP32 Pinout

```
┌─────────────────────────────────────┐
│           ESP32-C3 Mini             │
├─────────────────────────────────────┤
│                                     │
│  3V3 ◄──────── Regulated 3.3V       │
│  GND ◄──────── Ground               │
│                                     │
│  GPIO0 ──────► Boot button          │
│  GPIO1 ──────► ADC: Battery voltage │
│  GPIO2 ──────► Status LED (onboard) │
│  GPIO3 ──────► RED MOSFET (PWM CH0) │  ◄── NEW
│  GPIO4 ──────► Start/Stop button    │
│  GPIO5 ──────► NIR MOSFET (PWM CH1) │  ◄── NEW
│  GPIO6 ──────► Buzzer (optional)    │
│                                     │
│  USB  ◄──────► Programming/debug    │
│                                     │
└─────────────────────────────────────┘
```

### Treatment Modes (Firmware)

```c
enum TreatmentMode {
    MODE_RED_ONLY,      // 650nm only - maintenance, early stage
    MODE_NIR_ONLY,      // 850nm only - deep treatment
    MODE_DUAL,          // Both on simultaneously - comprehensive
    MODE_ALTERNATING,   // Alternate every 30s - experimental
    MODE_PULSED         // 10Hz pulsed - some evidence of benefit
};
```

---

## Updated BOM: Red-Only vs Dual-Wavelength

### Option A: Red-Only (v1.0 - Original)

| Qty | Component | Unit Cost | Total |
|-----|-----------|-----------|-------|
| 1 | ESP32-C3 Mini | $6.00 | $6.00 |
| 60 | 650nm Red LEDs | $0.15 | $9.00 |
| 30 | 150Ω Resistors | $0.02 | $0.60 |
| 1 | IRLZ44N MOSFET | $1.50 | $1.50 |
| 2 | 18650 Cells | $5.00 | $10.00 |
| 1 | 2S BMS USB-C | $4.00 | $4.00 |
| 1 | 18650 Holder | $2.00 | $2.00 |
| - | Misc (wire, resistors, button) | - | $5.00 |
| - | 3D Printed Frame | - | $8.00 |
| | | **TOTAL** | **$46.10** |

### Option B: Dual-Wavelength (v2.0 - Recommended)

| Qty | Component | Unit Cost | Total |
|-----|-----------|-----------|-------|
| 1 | ESP32-C3 Mini | $6.00 | $6.00 |
| 60 | 650nm Red LEDs | $0.15 | $9.00 |
| **30** | **850nm NIR LEDs** | **$0.05** | **$1.50** |
| 30 | 150Ω Resistors (red) | $0.02 | $0.60 |
| **10** | **100Ω Resistors (NIR)** | **$0.02** | **$0.20** |
| **2** | **IRLZ44N MOSFETs** | **$1.50** | **$3.00** |
| 2 | 18650 Cells | $5.00 | $10.00 |
| 1 | 2S BMS USB-C | $4.00 | $4.00 |
| 1 | 18650 Holder | $2.00 | $2.00 |
| - | Misc (wire, resistors, button) | - | $5.00 |
| - | 3D Printed Frame | - | $8.00 |
| | | **TOTAL** | **$49.30** |

### Cost Increase for Dual-Wavelength

| Item | Cost Delta |
|------|------------|
| 30x 850nm NIR LEDs | +$1.50 |
| 10x 100Ω resistors | +$0.20 |
| 1x additional MOSFET | +$1.50 |
| **Total increase** | **+$3.20** |

**Cost increase: 6.9%** for dual-wavelength capability

---

## Efficacy Analysis: Cost per Hair/cm²

### Published Efficacy Data

| Configuration | Δ Hairs/cm² | Evidence Level |
|---------------|-------------|----------------|
| Red only (650-660nm) | +17.2 | Strong (RCT, n=40) |
| NIR only (830nm) | "Greater than other wavelengths" | Moderate |
| Combined (630-970nm) | Significant (p<.01) at 24 wks | Moderate |
| nNIR broad spectrum | 5.88x follicle count | In vivo (mouse) |

### Projected Dual-Wavelength Benefit

Based on the research:
- Red alone: +17 hairs/cm² (baseline)
- Combined wavelengths likely provide **15-30% additional benefit** based on:
  - Deeper penetration reaching miniaturized follicles
  - 830nm showing "greater efficacy" in comparative studies
  - nNIR showing 1.69x anagen induction vs white light

**Conservative estimate:** +20 hairs/cm² with dual wavelength (+18% over red-only)

### Value Analysis

| Config | Cost | Est. Δ Hairs/cm² | Cost per Hair/cm² |
|--------|------|------------------|-------------------|
| Red-only (v1.0) | $46.10 | +17 | $2.71 |
| Dual-wave (v2.0) | $49.30 | +20 | $2.47 |

**Dual-wavelength provides ~9% better value** (cost per hair improvement)

---

## Recommended Configuration

Based on cost/efficacy analysis:

### Roxy RedLight v2.0 Specification

| Parameter | Value |
|-----------|-------|
| Red LEDs (650nm) | 60 units |
| NIR LEDs (850nm) | 30 units |
| Total LEDs | 90 units |
| Red/NIR Ratio | 67% / 33% |
| Total Power | ~3.6W optical |
| Current Draw | ~900mA max |
| Runtime (2x 2600mAh) | ~2.5 hours |
| Cost | ~$50 |

### LED Layout Pattern

Intersperse NIR LEDs among red LEDs for even coverage:

```
  R   R   R   R   R   R   R   R   R   R
    N       N       N       N       N
  R   R   R   R   R   R   R   R   R   R
    N       N       N       N       N
  R   R   R   R   R   R   R   R   R   R
    N       N       N       N       N

R = 650nm Red LED
N = 850nm NIR LED
```

---

## Future Enhancements

1. **App control** - BLE interface for session tracking
2. **Pulsed mode** - Some research suggests pulsed light may be more effective
3. **Scalp contact sensors** - Verify proper helmet placement
4. **Treatment logging** - Store session data in flash
5. **Wavelength ratio adjustment** - User-selectable red/NIR balance

---

## References

- ESP32-C3 Datasheet: https://www.espressif.com/en/products/socs/esp32-c3
- IRLZ44N Datasheet: https://www.infineon.com/
- TP4056 Application Note: Various
- 650nm LED typical specifications from datasheet
