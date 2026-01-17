# Hardware Assembly Guide

Step-by-step instructions for building the Roxy RedLight PBM helmet.

**Estimated Build Time:** 4-6 hours
**Skill Level:** Intermediate (basic soldering required)

---

## Table of Contents

1. [Tools & Materials](#1-tools--materials)
2. [Pre-Assembly Preparation](#2-pre-assembly-preparation)
3. [LED Strip Assembly](#3-led-strip-assembly)
4. [Wiring the LED Strips](#4-wiring-the-led-strips)
5. [ESP32 Enclosure Assembly](#5-esp32-enclosure-assembly)
6. [Power System Assembly](#6-power-system-assembly)
7. [Final Integration](#7-final-integration)
8. [Testing & Verification](#8-testing--verification)
9. [Troubleshooting](#9-troubleshooting)

---

## 1. Tools & Materials

### Required Tools

| Tool | Purpose |
|------|---------|
| Soldering iron (adjustable temp) | Wire connections |
| Solder (60/40 or lead-free) | Joining wires |
| Wire strippers | Prep 28 AWG wire |
| Flush cutters | Trim LED legs, wires |
| Multimeter | Testing continuity, voltage |
| Needle-nose pliers | Holding small parts |
| Heat gun or lighter | Heat shrink tubing |
| Small Phillips screwdriver | M2 screws |
| Scissors | Cutting tape, fabric |

### Optional But Helpful

| Tool | Purpose |
|------|---------|
| Helping hands / PCB holder | Holds work steady |
| Solder wick / desoldering pump | Fixing mistakes |
| Hot glue gun | Securing wires |
| Label maker or tape | Wire identification |

### Safety Equipment

- Safety glasses (soldering)
- Ventilation or fume extractor
- Heat-resistant work surface

---

## 2. Pre-Assembly Preparation

### 2.1 Inventory Check

Before starting, verify all parts from the [BOM](bom-with-links.md):

**Electronics:**
- [ ] LilyGO T-Display S3 (1)
- [ ] Red LEDs 5mm diffused (60 needed, 100 in pack)
- [ ] NIR LEDs 5mm 850nm (30 needed, 50 in pack)
- [ ] IRLZ44N MOSFETs (2 needed, 5 in pack)
- [ ] 18650 batteries (2)
- [ ] 2S BMS with USB-C (1)
- [ ] 18650 holder (1)
- [ ] Resistors 150Ω (12 needed)
- [ ] Resistors 100Ω (6 needed)

**Connectors & Wire:**
- [ ] JST-XH 3-pin connectors (6 pairs)
- [ ] 28 AWG silicone wire (red, black, orange, white)
- [ ] Heat shrink tubing assortment

**Mechanical:**
- [ ] 3D printed LED strips (6)
- [ ] 3D printed ESP32 enclosure (base + lid)
- [ ] 3D printed junction box (1)
- [ ] Snap buttons (12 pairs)
- [ ] Base cap/beanie (1)

### 2.2 Test Components

Before assembly, test key components:

**Test LEDs (sample 5-10 of each type):**
```
1. Set multimeter to diode mode
2. Touch probes to LED legs (longer leg = anode = +)
3. LED should glow dimly
4. Red LEDs: ~2.0V forward voltage
5. NIR LEDs: ~1.5V (invisible, but multimeter shows voltage)
```

**Test T-Display S3:**
```
1. Connect USB-C to computer
2. Display should show demo or blank screen
3. Upload test firmware (see firmware/README.md)
```

**Test Batteries:**
```
1. Check voltage with multimeter: should be 3.6-4.2V each
2. If below 3.0V, charge before use
```

### 2.3 Print 3D Parts

Print all parts per [design-specifications.md](../hardware/3d-models/design-specifications.md):

| Part | Qty | Material | Notes |
|------|-----|----------|-------|
| LED Strip 15 | 3 | PETG | Living hinges require PETG |
| LED Strip 12 | 2 | PETG | |
| LED Strip 21 | 1 | PETG | |
| Enclosure Base | 1 | PETG/PLA | |
| Enclosure Lid | 1 | PETG/PLA | |
| Junction Box | 1 | PETG/PLA | |

**Post-Print Cleanup:**
- Remove any support material
- Test LED fit in holders (should snap in)
- Flex living hinges gently to break in
- Clean any stringing

---

## 3. LED Strip Assembly

### 3.1 LED Placement Plan

Each strip has a specific red/NIR pattern (2:1 ratio):

```
Pattern: R-R-N-R-R-N-R-R-N... (repeat)

R = Red 620-630nm (diffused, 120°)
N = NIR 850nm (clear or with diffuser cap)
```

**Strip LED Counts:**

| Strip | Total | Red | NIR | Position |
|-------|-------|-----|-----|----------|
| 1 | 15 | 10 | 5 | Hairline |
| 2 | 12 | 8 | 4 | Left temple |
| 3 | 12 | 8 | 4 | Right temple |
| 4 | 15 | 10 | 5 | Left crown |
| 5 | 15 | 10 | 5 | Right crown |
| 6 | 21 | 14 | 7 | Center crown |

### 3.2 Insert LEDs

**LED Orientation - CRITICAL:**

```
    LONGER LEG = ANODE (+)
    SHORTER LEG = CATHODE (-)

         ANODE (+)
           │
    ┌──────┴──────┐
    │    ┌───┐    │
    │    │ ▲ │    │  ← Flat edge on cathode side
    │    └───┘    │
    └──────┬──────┘
           │
        CATHODE (-)
```

**For each strip:**

1. Lay strip flat, sew tabs at ends
2. All LEDs must face SAME DIRECTION (anodes toward one tab)
3. Insert first LED: push until it clicks past snap rim
4. Continue inserting LEDs in R-R-N pattern
5. Double-check orientation before soldering

```
CORRECT:
[TAB+] →(+)LED(-)→(+)LED(-)→(+)LED(-)→ [TAB-]
        All anodes pointing same direction

WRONG:
[TAB] →(+)LED(-)→(-)LED(+)→(+)LED(-)→ [TAB]
       ↑ This LED is backwards - won't work!
```

### 3.3 LED Insertion Tips

- If LED is too tight: file holder slightly or use less infill when printing
- If LED is too loose: add tiny drop of hot glue after testing
- NIR LEDs look identical to clear lens red - mark the bag!
- Test each LED as you insert (multimeter diode mode)

---

## 4. Wiring the LED Strips

### 4.1 Series Wiring Concept

LEDs are wired in series groups to match battery voltage:

```
SERIES GROUP (5 LEDs):

     V+ (7.4V from battery)
      │
      ▼
    ┌───┐
    │LED│ 2.0V drop
    └─┬─┘
      │
    ┌─▼─┐
    │LED│ 2.0V drop
    └─┬─┘
      │
    ┌─▼─┐
    │LED│ 2.0V drop
    └─┬─┘
      │
    ┌─▼─┐
    │LED│ 2.0V drop
    └─┬─┘
      │
    ┌─▼─┐
    │LED│ 2.0V drop
    └─┬─┘
      │
     ───
     150Ω resistor (drops remaining ~0.4V, limits current)
      │
      ▼
     GND (via MOSFET)

Total: 5 × 2.0V = 10.0V ... wait, that's more than 7.4V!
```

**Actually, for 7.4V battery:**
- Red LEDs (2.0V): 3 in series = 6.0V, leaves 1.4V for resistor
- NIR LEDs (1.5V): 4-5 in series = 6.0-7.5V

Let me recalculate:

### 4.2 Correct Series Configuration

**For 7.4V nominal (6.2V - 8.4V range):**

| LED Type | Vf | LEDs in Series | Total Vf | Resistor | Current |
|----------|-----|----------------|----------|----------|---------|
| Red 620nm | 2.0V | 3 | 6.0V | 68Ω | 20mA |
| NIR 850nm | 1.5V | 4 | 6.0V | 68Ω | 20mA |

**Resistor calculation:**
```
R = (Vbat - Vf_total) / I
R = (7.4V - 6.0V) / 0.020A
R = 1.4V / 0.020A = 70Ω

Use nearest standard value: 68Ω or 75Ω
At low battery (6.2V): I = (6.2 - 6.0) / 68 = 3mA (dim but works)
At full battery (8.4V): I = (8.4 - 6.0) / 68 = 35mA (slightly bright)
```

### 4.3 Strip Wiring Diagram

**Strip 1 (15 LEDs = 10 Red + 5 NIR):**

```
V+ ────┬──────────────┬──────────────┬──────────────┬──────────────┐
       │              │              │              │              │
    ┌──▼──┐        ┌──▼──┐        ┌──▼──┐        ┌──▼──┐        ┌──▼──┐
    │R1-R2│        │R4-R5│        │R7-R8│        │R10  │        │N1-N2│
    │ -R3 │        │ -R6 │        │ -R9 │        │     │        │-N3  │
    └──┬──┘        └──┬──┘        └──┬──┘        └──┬──┘        └──┬──┘
       │              │              │              │              │
      68Ω            68Ω            68Ω            68Ω            68Ω
       │              │              │              │              │
       └──────┬───────┴───────┬─────┴───────┬─────┴──────┬────────┘
              │               │             │            │
              ▼               ▼             ▼            ▼
           RED_GND         RED_GND       RED_GND      NIR_GND
        (to MOSFET)      (to MOSFET)   (to MOSFET)  (to MOSFET)
```

Wait, this is getting complex. Let me simplify:

### 4.4 Simplified Wiring (Recommended)

**All Red LEDs in parallel groups, all NIR in parallel groups:**

```
                        V+ (7.4V)
                           │
          ┌────────────────┼────────────────┐
          │                │                │
     ┌────┴────┐      ┌────┴────┐      ┌────┴────┐
     │ 3x RED  │      │ 3x RED  │      │ 4x NIR  │
     │ series  │      │ series  │      │ series  │
     └────┬────┘      └────┬────┘      └────┬────┘
          │                │                │
         68Ω              68Ω              47Ω
          │                │                │
          └────────┬───────┴────────┬───────┘
                   │                │
               RED_CTRL         NIR_CTRL
             (from MOSFET)    (from MOSFET)
```

### 4.5 Step-by-Step Wiring

**For each LED strip:**

1. **Prep wire lengths:**
   - Cut 28 AWG wire to strip length + 50mm extra
   - Red wire: V+ bus (runs full length)
   - Orange wire: Red LED return (to junction box)
   - White wire: NIR LED return (to junction box)

2. **Solder V+ bus:**
   ```
   - Run red wire along back of strip in wire channel
   - Solder to anode (+) of first LED in each series group
   - Use short jumper wires to reach each group's first LED
   ```

3. **Solder series connections:**
   ```
   - Connect cathode of LED 1 to anode of LED 2
   - Connect cathode of LED 2 to anode of LED 3
   - Repeat for each series group (3 Red or 4 NIR)
   ```

4. **Add current limiting resistors:**
   ```
   - Solder 68Ω resistor to cathode of last RED LED in each group
   - Solder 47Ω resistor to cathode of last NIR LED in each group
   - Connect all RED resistors together → Orange wire
   - Connect all NIR resistors together → White wire
   ```

5. **Add JST connector:**
   ```
   JST-XH 3-pin at end of strip:
   Pin 1: Red (V+)
   Pin 2: Orange (RED return)
   Pin 3: White (NIR return)
   ```

6. **Insulate:**
   - Heat shrink on all solder joints
   - Hot glue to secure wires in channel

### 4.6 Wire Routing

```
         FRONT OF HEAD
              │
    ┌─────────┴─────────┐
   ╱   Strip 1           ╲
  │   ═══════════════════ │   Wires route
  │                       │   down back
  │ Strip   ═══   Strip   │   of head
  │   2     ═══     3     │
  │         ═══           │
  │   Strip 4+5+6         │
   ╲  ═══════════════    ╱
    └────────┬───────────┘
             │
         All wires
         merge here
             │
             ▼
      ┌──────────────┐
      │ JUNCTION BOX │
      │ (at nape)    │
      └──────┬───────┘
             │
             ▼
      ┌──────────────┐
      │ ESP32        │
      │ ENCLOSURE    │
      └──────────────┘
```

---

## 5. ESP32 Enclosure Assembly

### 5.1 Prepare T-Display S3

1. **Solder headers (if not pre-soldered):**
   - Solder male headers to GPIO pins
   - Or solder wires directly for compact build

2. **Test display:**
   - Connect USB-C
   - Upload firmware (see firmware/README.md)
   - Verify LCD shows home screen

### 5.2 Wire MOSFET Driver Circuit

**Circuit for each channel (build 2x):**

```
                  DRAIN (to LED cathodes)
                     │
                     │
    ┌────────────────┴────────────────┐
    │                                 │
    │            IRLZ44N              │
    │          ┌─────────┐            │
    │    GATE──┤         ├──SOURCE    │
    │          └─────────┘     │      │
    │              │           │      │
    │              │          GND     │
    │              │                  │
    │         ┌────┴────┐             │
    │         │  1kΩ    │ (optional   │
    │         │ pulldown│  gate       │
    │         └────┬────┘  resistor)  │
    │              │                  │
    └──────────────┴──────────────────┘
                   │
                  GND

    GPIO43 (RED) ──────► GATE of MOSFET 1
    GPIO44 (NIR) ──────► GATE of MOSFET 2
```

**Wiring steps:**

1. Mount MOSFETs on small perfboard or solder directly
2. Connect GATE to ESP32 GPIO (43 for RED, 44 for NIR)
3. Connect SOURCE to GND
4. Connect DRAIN to LED return wires (via junction box)
5. Add 1kΩ resistor from GATE to GND (prevents floating)

### 5.3 Assemble Enclosure

1. **Install BMS module:**
   - Position BMS so USB-C port aligns with enclosure cutout
   - Hot glue or screw in place
   - Connect battery holder wires to BMS input

2. **Install battery holder:**
   - Place in battery compartment
   - Ensure spring contacts have clearance
   - Connect to BMS (observe polarity!)

3. **Mount T-Display S3:**
   - Use M2 standoffs or hot glue
   - Align LCD with window cutout
   - Route wires cleanly

4. **Install MOSFETs:**
   - Mount to sidewall or perfboard
   - Ensure heat dissipation (they run cool at 20mA)

5. **Wire connections:**

   | ESP32 Pin | Connection |
   |-----------|------------|
   | GPIO43 | MOSFET 1 Gate (RED) |
   | GPIO44 | MOSFET 2 Gate (NIR) |
   | GPIO4 | Battery ADC (via voltage divider) |
   | 3.3V | Voltage divider top |
   | GND | Common ground |
   | 5V/VBUS | BMS output (for charging indicator, optional) |

6. **Add voltage divider for battery monitoring:**
   ```
   VBAT (7.4V) ──┬── 100kΩ ──┬── GPIO4 (ADC)
                 │           │
                 └── 33kΩ ───┴── GND
   ```

### 5.4 Close Enclosure

1. Route wires through exit hole
2. Add strain relief (zip tie in notch)
3. Snap or screw lid in place
4. Test fit before final assembly

---

## 6. Power System Assembly

### 6.1 Battery Safety

**CRITICAL SAFETY RULES:**

1. Never short-circuit batteries
2. Never charge unattended
3. Use protected cells with built-in BMS
4. Store at 3.7V if not used for weeks
5. Dispose of damaged batteries properly

### 6.2 BMS Wiring

```
         ┌─────────────────────────────┐
         │      2S BMS MODULE          │
         │                             │
BATTERY  │  B+  B-  P+  P-  C+  C-    │
   │     │  │   │   │   │   │   │     │
   │     └──┼───┼───┼───┼───┼───┼─────┘
   │        │   │   │   │   │   │
   │        │   │   │   │   │   └── USB-C GND
   │        │   │   │   │   └────── USB-C 5V
   │        │   │   │   └────────── Pack negative (to circuit GND)
   │        │   │   └────────────── Pack positive (to circuit V+)
   │        │   └────────────────── Battery negative
   │        └────────────────────── Battery mid-point (balance)
   │
   └── Connect battery holder here
```

**Note:** BMS modules vary. Check your specific module's pinout!

### 6.3 Final Power Connections

```
BATTERY+ ──► BMS B+
BATTERY- ──► BMS B-
BMS P+ ─────► V+ bus (to junction box, ESP32)
BMS P- ─────► GND bus (common ground)
```

---

## 7. Final Integration

### 7.1 Junction Box Wiring

Connect all strip connectors to junction box:

```
Junction Box Internal Wiring:

STRIP 1 ─┐
STRIP 2 ─┤
STRIP 3 ─┼── All V+ (Pin 1) connected together ──► Red wire to ESP32 V+
STRIP 4 ─┤
STRIP 5 ─┤
STRIP 6 ─┘

STRIP 1 ─┐
STRIP 2 ─┤
STRIP 3 ─┼── All RED (Pin 2) connected together ──► MOSFET 1 DRAIN
STRIP 4 ─┤
STRIP 5 ─┤
STRIP 6 ─┘

STRIP 1 ─┐
STRIP 2 ─┤
STRIP 3 ─┼── All NIR (Pin 3) connected together ──► MOSFET 2 DRAIN
STRIP 4 ─┤
STRIP 5 ─┤
STRIP 6 ─┘
```

### 7.2 Attach Strips to Cap

1. **Position strips on cap:**
   - Wear cap, mark strip positions with tape
   - Strips should cover: hairline, temples, crown

2. **Attach snap buttons:**
   - Install snap bases on strip sew tabs
   - Install matching snaps on cap fabric
   - Test fit and adjust positions

3. **Route wires:**
   - Tuck wires under cap brim/band
   - Route to junction box at nape
   - Secure with fabric tape or stitching

### 7.3 Final Assembly Checklist

- [ ] All 6 strips attached to cap
- [ ] All strips connected to junction box
- [ ] Junction box connected to ESP32 enclosure
- [ ] Batteries installed
- [ ] ESP32 firmware loaded
- [ ] Enclosure closed and secured
- [ ] USB-C charging port accessible

---

## 8. Testing & Verification

### 8.1 Pre-Power Checks

**With batteries REMOVED:**

1. **Continuity test each strip:**
   ```
   Multimeter on continuity mode:
   - V+ to RED return: should beep (LEDs + resistors in path)
   - V+ to NIR return: should beep
   - RED to NIR: should NOT beep (separate circuits)
   ```

2. **Check for shorts:**
   ```
   - V+ to GND: should NOT beep
   - Any exposed wire to GND: should NOT beep
   ```

### 8.2 Bench Power Test

**Before using batteries, test with bench supply:**

1. Set supply to 7.4V, current limit 500mA
2. Connect to circuit V+ and GND
3. Power on ESP32, verify display works
4. Use firmware to turn on RED channel at 50%
5. Measure current: should be ~200-300mA
6. Turn on NIR at 50%, current increases
7. Check all LEDs visually (NIR needs phone camera)

### 8.3 Battery Power Test

1. Install fully charged batteries
2. Power on, verify display shows battery voltage
3. Run through all treatment modes
4. Verify auto-shutoff at 20 minutes
5. Check battery voltage after session

### 8.4 On-Device Hardware Tests

Run the firmware hardware tests:

```bash
cd firmware/esp32
pio test -e hardware
```

Expected results:
- PWM output: PASS
- ADC battery: PASS
- Button inputs: PASS
- All 12 hardware tests pass

### 8.5 Irradiance Verification (Optional)

If you have a light meter:
1. Power on RED channel at 100%
2. Measure irradiance at 10mm from LED
3. Target: 5-15 mW/cm² per LED
4. Repeat for NIR (need IR-capable meter)

---

## 9. Troubleshooting

### LEDs Don't Light

| Symptom | Possible Cause | Fix |
|---------|---------------|-----|
| No LEDs light | No power | Check battery, BMS, connections |
| No LEDs light | MOSFET not switching | Check GPIO connection, gate resistor |
| One strip dark | Loose connector | Reseat JST connector |
| One strip dark | Broken wire | Check continuity, resolder |
| Single LED dark | LED dead or reversed | Replace LED, check polarity |
| LEDs very dim | Low battery | Charge batteries |
| LEDs very dim | Wrong resistor | Check resistor value |

### Display Issues

| Symptom | Possible Cause | Fix |
|---------|---------------|-----|
| No display | No power to ESP32 | Check 3.3V rail |
| Garbled display | Loose connection | Check ribbon cable |
| Display flicker | Power issue | Add capacitor on 3.3V |

### Battery Issues

| Symptom | Possible Cause | Fix |
|---------|---------------|-----|
| Won't charge | BMS protection | Check for shorts, let BMS reset |
| Quick drain | Short circuit | Check wiring, measure idle current |
| Voltage reading wrong | Divider values | Recalculate, adjust in firmware |

### Button Issues

| Symptom | Possible Cause | Fix |
|---------|---------------|-----|
| No response | Wrong GPIO | Check pin numbers |
| Erratic | No pullup | Enable INPUT_PULLUP in code |
| Stuck | Button damaged | Replace button |

---

## Appendix A: Wire Color Code

| Color | Signal | Description |
|-------|--------|-------------|
| Red | V+ | Battery positive (7.4V) |
| Black | GND | Ground |
| Orange | RED_CTL | Red LED return (to MOSFET) |
| White | NIR_CTL | NIR LED return (to MOSFET) |
| Yellow | ADC | Battery voltage sense |
| Green | DATA | Reserved/optional |

---

## Appendix B: Connector Pinouts

### JST-XH 3-Pin (LED Strips)

```
┌─────────────┐
│ [1] [2] [3] │
└─────────────┘
  │   │   │
  │   │   └── NIR return (white)
  │   └────── RED return (orange)
  └────────── V+ (red)
```

### Main Cable (Junction Box to ESP32)

```
5-conductor cable:
1. Red: V+ (battery positive)
2. Black: GND
3. Orange: RED MOSFET drain
4. White: NIR MOSFET drain
5. Yellow: Battery ADC (optional, can sense at ESP32)
```

---

## Appendix C: Quick Reference Card

Print this and keep in your workspace:

```
╔══════════════════════════════════════════════════════════╗
║         ROXY REDLIGHT QUICK REFERENCE                    ║
╠══════════════════════════════════════════════════════════╣
║ LED Polarity:  LONG leg = + (anode)                      ║
║ LED Pattern:   R-R-N-R-R-N (2:1 red to NIR)              ║
║ Series Groups: 3 Red LEDs + 68Ω  |  4 NIR LEDs + 47Ω     ║
║ Battery:       2S Li-ion, 6.2V-8.4V range                ║
║ Target mA:     20mA per LED                              ║
╠══════════════════════════════════════════════════════════╣
║ ESP32 PINS:                                              ║
║   GPIO43 → RED MOSFET gate                               ║
║   GPIO44 → NIR MOSFET gate                               ║
║   GPIO4  → Battery ADC                                   ║
║   GPIO0  → Button 1                                      ║
║   GPIO14 → Button 2                                      ║
╠══════════════════════════════════════════════════════════╣
║ TEST COMMANDS:                                           ║
║   pio test -e native      (unit tests)                   ║
║   pio test -e hardware    (on-device tests)              ║
║   pio run -e lilygo-t-display-s3 --target upload         ║
╚══════════════════════════════════════════════════════════╝
```
