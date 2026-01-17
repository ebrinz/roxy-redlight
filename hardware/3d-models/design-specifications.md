# 3D Model Design Specifications

Complete specifications for all 3D printed parts. Software-agnostic - use OpenSCAD, Fusion 360, FreeCAD, or any CAD tool.

---

## Part 1: LED Strip (x6)

Flexible strip with snap-fit LED holders and living hinges.

### Overview

```
┌─────────────────────────────────────────────────────────────────────────┐
│ [SEW TAB]  ○───○───○───○───○───○───○───○───○───○───○───○  [SEW TAB]     │
│            LED HOLDERS (connected by living hinges)                      │
└─────────────────────────────────────────────────────────────────────────┘
```

### Key Dimensions

| Parameter | Value | Notes |
|-----------|-------|-------|
| LED diameter | 5.0mm | Standard 5mm LED |
| LED holder ID | 5.1mm | Slight clearance for snap-fit |
| LED holder OD | 8.0mm | Wall thickness ~1.5mm |
| LED holder height | 6.0mm | Holds LED securely |
| LED spacing (center-to-center) | 15.0mm | Based on irradiance calcs |
| Hinge width | 5.0mm | Between holders |
| Hinge thickness | 0.4mm | 2 layers at 0.2mm = flexible |
| Strip width | 12.0mm | Overall width |
| Sew tab size | 10mm x 15mm | With 2mm holes for thread |
| Wire channel width | 3.0mm | Under strip for routing |
| Wire channel depth | 2.0mm | Recessed into bottom |

### LED Holder Detail

```
        TOP VIEW                      SIDE SECTION
    ┌─────────────┐              ┌─────────────────┐
    │  ┌───────┐  │              │    ┌─────┐      │
    │  │       │  │              │    │     │      │ 6.0mm
    │  │  LED  │  │              │    │ LED │      │
    │  │ 5.1mm │  │              │    │     │      │
    │  │       │  │              │────┴─────┴──────│
    │  └───────┘  │              │  SNAP RIM 0.3mm │
    └─────────────┘              └─────────────────┘
         8.0mm                         8.0mm

    SNAP-FIT DETAIL:
    - 0.3mm inward lip at top holds LED in place
    - LED pushes past lip and clicks in
    - Slight taper on entry (chamfer 0.5mm)
```

### Living Hinge Detail

```
    SIDE VIEW OF HINGE

    ┌────────┐         ┌────────┐
    │ HOLDER │─────────│ HOLDER │
    │        │  0.4mm  │        │
    │   ○    │ ═══════ │   ○    │
    │        │  HINGE  │        │
    └────────┘         └────────┘

    HINGE GEOMETRY:
    - Thickness: 0.4mm (2 print layers)
    - Width: full strip width (12mm)
    - Length: 5.0mm
    - Material: PETG required for flexibility
    - Print orientation: flat, hinges along X-axis
```

### Sew Tab Detail

```
    ┌─────────────┐
    │  ○       ○  │  Holes: 2.0mm diameter
    │             │  Hole spacing: 6mm
    │  ○       ○  │  Tab connects to first/last holder
    └──────┬──────┘
           │
    ───────┴─────── LED strip continues

    Tab size: 10mm wide x 15mm tall x 2mm thick
```

### Strip Configurations

| Strip ID | LED Count | Length (mm) | Position on Head |
|----------|-----------|-------------|------------------|
| Strip 1 | 15 | 225 | Hairline (front) |
| Strip 2 | 12 | 180 | Left temple |
| Strip 3 | 12 | 180 | Right temple |
| Strip 4 | 15 | 225 | Left crown |
| Strip 5 | 15 | 225 | Right crown |
| Strip 6 | 21 | 315 | Center crown |

**Formula:** Length = (LED_count - 1) × 15mm + 8mm + (2 × 15mm for tabs)

### Wire Channel

```
    BOTTOM VIEW

    ┌─────────────────────────────────────────┐
    │ ════════════════════════════════════════│  Wire channel
    │         3mm wide, 2mm deep              │  runs full length
    │ ════════════════════════════════════════│  centered on strip
    └─────────────────────────────────────────┘

    - Wires lay in channel, secured with tape or friction
    - Channel has entry/exit at sew tabs
```

### Print Settings

| Setting | Value |
|---------|-------|
| Material | PETG (required for living hinges) |
| Layer height | 0.2mm |
| Infill | 100% for hinges, 20% for holders |
| Orientation | Flat, hinges along X-axis |
| Supports | None needed |
| Walls | 3 perimeters |

---

## Part 2: ESP32 Enclosure

Houses the T-Display S3, BMS, and battery holder.

### Overview

```
    TOP VIEW (lid removed)
    ┌────────────────────────────────────────────────┐
    │                                                │
    │  ┌──────────────────────────┐                  │
    │  │      LCD WINDOW          │  ← 42mm x 26mm   │
    │  │      (cutout)            │                  │
    │  └──────────────────────────┘                  │
    │                                                │
    │  ┌────────────────────────────────────────┐    │
    │  │                                        │    │
    │  │         T-Display S3 Board             │    │
    │  │           68mm x 28mm                  │    │
    │  │                                        │    │
    │  └────────────────────────────────────────┘    │
    │                                                │
    │  ┌────────────────────────────────────────┐    │
    │  │     [18650]          [18650]           │    │
    │  │      CELL 1           CELL 2           │    │
    │  └────────────────────────────────────────┘    │
    │                                                │
    │  ┌──────────┐                                  │
    │  │   BMS    │  ← 2S USB-C charge module        │
    │  └──────────┘                                  │
    │                                                │
    └────────────────────────────────────────────────┘
```

### Enclosure Dimensions

| Parameter | Value | Notes |
|-----------|-------|-------|
| **Overall** | | |
| Length | 110mm | Fits batteries + board |
| Width | 75mm | Side by side batteries |
| Height | 35mm | Stacked components |
| Wall thickness | 2.0mm | Sturdy, printable |
| Corner radius | 3.0mm | Comfortable edges |
| | | |
| **LCD Window** | | |
| Window width | 42mm | Visible display area |
| Window height | 26mm | Visible display area |
| Window position X | 10mm from left edge | |
| Window position Y | 5mm from top edge | |
| Window bezel | 1.0mm overlap | Holds screen in place |
| | | |
| **T-Display S3 Mount** | | |
| Board dimensions | 68mm x 28mm x 11mm | Including components |
| Mount standoffs | 4x M2 holes at corners | 2.2mm holes, 5mm tall |
| Board corner holes | 64mm x 24mm spacing | Standard mount pattern |
| | | |
| **Battery Compartment** | | |
| 18650 diameter | 18.5mm | With wrapper |
| 18650 length | 65mm | Standard unprotected |
| Compartment size | 75mm x 40mm x 20mm | Side-by-side, spring contacts |
| Spring contact clearance | 5mm each end | Compression space |
| | | |
| **BMS Module** | | |
| BMS size | 25mm x 18mm x 5mm | Typical 2S USB-C module |
| USB-C port position | Centered on bottom edge | External access |
| USB-C cutout | 10mm x 4mm | Fits USB-C plug |
| | | |
| **Wire Exit** | | |
| Exit hole size | 8mm x 5mm | Oval, for flat cable |
| Exit position | Bottom edge, opposite USB | To LED strips |
| Strain relief | Notch for zip tie | Secures cable |

### Button Access

```
    TOP EDGE DETAIL

    ────────┬────────────────────┬────────────
            │                    │
         [BTN1]              [BTN2]
          GPIO0               GPIO14
            │                    │
    ────────┴────────────────────┴────────────

    Button holes: 4mm diameter
    Button spacing: Match T-Display S3 layout (check board)
    Actuator clearance: 3mm above board surface
```

### Lid Design

```
    LID SECTION VIEW

    ┌──────────────────────────────────────────────┐
    │░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░│ 2mm thick
    └──────────────────────────────────────────────┘
          │                                    │
          └── 1.5mm lip fits inside box ───────┘

    Lid attachment: Friction fit OR M2 screws at corners
    LCD window: Matches base window, clear acrylic optional
```

### Mounting Options

```
    BACK OF ENCLOSURE

    ┌────────────────────────────────────────────────┐
    │                                                │
    │    ┌──────┐                      ┌──────┐      │
    │    │SLOT 1│                      │SLOT 2│      │  Belt clip slots
    │    └──────┘                      └──────┘      │  3mm x 15mm
    │                                                │
    │         ═══════════════════════                │  Velcro pad area
    │         ═══════════════════════                │  40mm x 20mm flat
    │                                                │
    └────────────────────────────────────────────────┘

    - Belt clip slots: 3mm wide, 15mm tall, 2mm from edge
    - Velcro area: Flat recessed pad for adhesive velcro
```

### Internal Layout (Side Section)

```
    SIDE SECTION

    ┌─────────────────────────────────────────────┐
    │▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓│ ← LID (2mm)
    ├─────────────────────────────────────────────┤
    │ T-Display S3 (11mm)                         │
    │ ┌─────────────────────────────────────────┐ │
    ├─┴─────────────────────────────────────────┴─┤
    │ ┌─────────────────────────────────────────┐ │
    │ │       18650 BATTERIES (18.5mm)          │ │
    │ └─────────────────────────────────────────┘ │
    ├─────────────────────────────────────────────┤
    │ BMS (5mm)                                   │
    └─────────────────────────────────────────────┘

    Total internal height: 11 + 18.5 + 5 = 34.5mm
    Add 2mm lid + 2mm base = 38.5mm total
    (Round to 35mm internal, 39mm external)
```

### Print Settings

| Setting | Value |
|---------|-------|
| Material | PETG or PLA+ |
| Layer height | 0.2mm |
| Infill | 20% |
| Walls | 3 perimeters |
| Supports | For USB-C cutout only |
| Orientation | Base down, lid separate |

---

## Part 3: Wire Junction Box

Small box where all strip wires meet before connecting to ESP32.

### Overview

```
    TOP VIEW
    ┌────────────────────────────────────┐
    │  [1] [2] [3] [4] [5] [6]           │  ← Strip connectors
    │                                    │     (JST-XH 3-pin)
    │         SOLDER/CRIMP               │
    │          TERMINALS                 │
    │                                    │
    │  ════════════════════════          │  ← Main cable exit
    │         (to ESP32)                 │     (5-wire: V+, GND, RED, NIR, DATA)
    └────────────────────────────────────┘
```

### Dimensions

| Parameter | Value | Notes |
|-----------|-------|-------|
| Length | 50mm | Fits 6 connectors |
| Width | 25mm | Wiring space |
| Height | 15mm | Low profile |
| Wall thickness | 1.5mm | |
| Connector holes | 8mm x 3mm | JST-XH footprint |
| Connector spacing | 7mm center-to-center | |
| Main cable exit | 10mm x 5mm | For 5-conductor cable |

### Print Settings

| Setting | Value |
|---------|-------|
| Material | PETG or PLA |
| Layer height | 0.2mm |
| Infill | 20% |

---

## Part 4: Diffuser Caps (Optional)

Snap-on caps for narrow-angle IR LEDs to spread beam.

### Dimensions

| Parameter | Value |
|-----------|-------|
| Inner diameter | 5.1mm |
| Outer diameter | 7.0mm |
| Height | 3.0mm |
| Dome radius | 3.5mm (hemisphere) |
| Material | Natural/white PETG (translucent) |

```
    SECTION VIEW

       ╭─────────╮
      ╱           ╲      ← Dome top (diffuses light)
     │             │
     │    ┌───┐    │     ← LED fits inside
     └────┤LED├────┘
          └───┘
```

---

## Assembly Reference

### Component Stack (ESP32 Enclosure)

```
Layer 0 (bottom):  Base shell (2mm)
Layer 1:           BMS module (5mm)
Layer 2:           18650 batteries (18.5mm)
Layer 3:           T-Display S3 board (11mm)
Layer 4 (top):     Lid (2mm)
```

### Wire Color Code

| Wire | Color | Function |
|------|-------|----------|
| V+ | Red | Battery positive (7.4V) |
| GND | Black | Ground |
| RED_CTL | Orange | Red LED PWM signal |
| NIR_CTL | White | NIR LED PWM signal |
| (optional) | Yellow | Data/sensor |

### Connector Pinout (JST-XH 3-pin)

```
┌─────────────┐
│ [1] [2] [3] │
└─────────────┘
  │   │   │
  │   │   └── NIR cathodes (common)
  │   └────── RED cathodes (common)
  └────────── V+ (anode common)
```

---

## Design Files Checklist

When creating 3D models, produce these files:

| Part | Filename | Format |
|------|----------|--------|
| LED Strip (15 LEDs) | `led_strip_15.stl` | STL |
| LED Strip (12 LEDs) | `led_strip_12.stl` | STL |
| LED Strip (21 LEDs) | `led_strip_21.stl` | STL |
| Enclosure Base | `enclosure_base.stl` | STL |
| Enclosure Lid | `enclosure_lid.stl` | STL |
| Junction Box | `junction_box.stl` | STL |
| Diffuser Cap | `diffuser_cap.stl` | STL |
| Source (if OpenSCAD) | `roxy_redlight.scad` | SCAD |
| Source (if Fusion) | `roxy_redlight.f3d` | F3D |
| STEP (for portability) | `*.step` | STEP |

---

## Revision History

| Version | Date | Changes |
|---------|------|---------|
| 1.0 | 2024-01 | Initial specification |
