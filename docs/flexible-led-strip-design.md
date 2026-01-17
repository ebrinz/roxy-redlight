# Flexible LED Strip Design

Universal LED strip system that attaches to any cap, beanie, or headwear.

## Design Concept

```
┌────────────────────────────────────────────────────────────────┐
│                                                                │
│   6 flexible LED strips attach to any cap via sew tabs         │
│   LEDs rest directly against hair (0-5mm contact)              │
│   Wide-angle diffused LEDs prevent hot spots                   │
│   ESP32 enclosure clips to back of cap                         │
│                                                                │
└────────────────────────────────────────────────────────────────┘
```

## Strip Layout on Head

```
            FRONT (forehead)
                  │
           ┌──────┴──────┐
          /   Strip 1     \        ← Hairline strip (15 LEDs)
         /    ○○○○○○○○○○   \
        │                   │
        │  Strip    Strip   │
        │    2        3     │      ← Temple strips (12 LEDs each)
        │   ○○○      ○○○    │
        │   ○○○      ○○○    │
        │   ○○○      ○○○    │
        │   ○○○      ○○○    │
        │                   │
         \  Strip 4 + 5    /       ← Crown strips (15 LEDs each)
          \ ○○○○○○○○○○○○○ /
           \  ○○○○○○○○○○ /
            └─────┬─────┘
                  │
            BACK (nape)
                  │
           [ESP32 Enclosure]
              clipped here
```

## LED Distribution

| Strip | Position | Red LEDs | NIR LEDs | Total |
|-------|----------|----------|----------|-------|
| 1 | Hairline | 10 | 5 | 15 |
| 2 | Left temple | 8 | 4 | 12 |
| 3 | Right temple | 8 | 4 | 12 |
| 4 | Left crown | 10 | 5 | 15 |
| 5 | Right crown | 10 | 5 | 15 |
| 6 | Center crown | 14 | 7 | 21 |
| **Total** | | **60** | **30** | **90** |

## 3D Printed Strip Design

Each strip is a flexible PCB-like structure with snap-fit LED holders.

### Single Strip (15 LEDs)

```
┌─────────────────────────────────────────────────────────────────────┐
│ [TAB]                                                         [TAB] │
│   │                                                             │   │
│   ○───○───○───○───○───○───○───○───○───○───○───○───○───○───○     │
│   R   R   N   R   R   N   R   R   N   R   R   N   R   R   N     │
│   │   │   │   │   │   │   │   │   │   │   │   │   │   │   │     │
│   └───┴───┴───┴───┴───┴───┴───┴───┴───┴───┴───┴───┴───┴───┘     │
│                         WIRING CHANNEL                           │
│                              ════                                │
│                         (to next strip)                          │
└─────────────────────────────────────────────────────────────────────┘

R = Red 650nm LED (diffused, 120°)
N = NIR 850nm LED (clear + diffuser cap)

LED spacing: 15mm center-to-center
Strip width: 12mm
Strip length: varies (150-300mm)
Material: PETG or TPU (flexible)
```

### Living Hinge Detail

```
     LED holder      Living hinge      LED holder
    ┌─────────┐     ┌───────────┐     ┌─────────┐
    │  ┌───┐  │     │  ╱╲╱╲╱╲   │     │  ┌───┐  │
    │  │ ○ │  │─────│  ╲╱╲╱╲╱   │─────│  │ ○ │  │
    │  └───┘  │     │           │     │  └───┘  │
    └─────────┘     └───────────┘     └─────────┘
        5mm            5mm               5mm

    Hinge allows strip to conform to head curvature
    Print in PETG at 0.2mm layer height for flexibility
```

### Sew Tab Detail

```
    ┌─────────┐
    │  ○   ○  │  ← Holes for needle/thread
    │         │
    │  ○   ○  │
    └────┬────┘
         │
    ─────┴───── LED strip continues

    Tab size: 10mm x 15mm
    Hole diameter: 2mm
    Can also use snap buttons or velcro
```

## Wiring Scheme

### Series-Parallel Configuration

Each strip has LEDs wired in series groups, with groups in parallel:

```
Strip 1 (15 LEDs):
├── Red group 1: 5 LEDs in series (10V @ 20mA)
├── Red group 2: 5 LEDs in series (10V @ 20mA)
├── NIR group:   5 LEDs in series (8V @ 20mA)
└── Total: 60mA per strip

All 6 strips in parallel:
Total current: 6 × 60mA = 360mA (from 7.4V battery via PWM)
```

### Connector System

```
                    ┌─────────────────────┐
                    │    ESP32 Enclosure  │
                    │                     │
                    │  [MOSFET] [MOSFET]  │
                    │    RED      NIR     │
                    └──────┬───────┬──────┘
                           │       │
              ┌────────────┴───────┴────────────┐
              │         Junction Box            │
              │  (hidden under cap at nape)     │
              │                                 │
              │   Strip connectors (JST-XH):    │
              │   [1] [2] [3] [4] [5] [6]       │
              └─────────────────────────────────┘
                    │   │   │   │   │   │
                    ↓   ↓   ↓   ↓   ↓   ↓
                 Individual LED strips
                 routed under cap fabric
```

### Wire Routing

- Main harness runs down back of head to nape
- Junction box sits at base of skull (hidden by cap/hair)
- Individual strip wires (28 AWG silicone) route under cap to each position
- Total wire length: ~2m

## Attachment Methods

### Method A: Sew-On (Permanent)
- Thread through tab holes
- Secure to cap fabric
- Most secure, washable (remove electronics first)

### Method B: Snap Buttons
- Attach snap bases to cap
- Snap studs on strip tabs
- Removable, transferable between caps

### Method C: Velcro
- Adhesive velcro on tabs
- Matching velcro sewn to cap
- Easy on/off, less secure

### Recommended: Snap Buttons
- Best balance of security and flexibility
- Can move strips between different caps
- Available at any fabric store

## Bill of Materials (Per Unit)

| Item | Qty | Notes |
|------|-----|-------|
| 3D printed strips | 6 | PETG, ~50g total |
| Snap buttons | 12 pairs | 10mm diameter |
| JST-XH connectors | 6 | 3-pin (V+, Red, NIR) |
| 28 AWG silicone wire | 3m | Red, black, white |
| Heat shrink tubing | 1m | 3mm diameter |
| Base cap (user supplied) | 1 | Any beanie/cap |

## Assembly Order

1. **Print strips** - 6 strips, check flexibility
2. **Insert LEDs** - Snap-fit into holders, observe polarity
3. **Solder wiring** - Series groups per strip
4. **Add connectors** - JST-XH at strip ends
5. **Attach snaps** - To strip tabs
6. **Prepare cap** - Attach matching snaps
7. **Route wires** - Under cap to junction box
8. **Connect to ESP32** - MOSFET outputs to junction box

## Comfort Considerations

- Total added weight: ~80g (strips + wiring)
- LEDs have rounded backs (no sharp edges)
- Silicone wire is soft and flexible
- No rigid parts pressing on scalp
- Cap fabric provides padding layer

## Thermal Analysis

At 20mA per LED, 90 LEDs total:
- Total LED current: 1.8A
- LED forward voltage drop: ~2V (red), ~1.5V (NIR)
- Power dissipated in LEDs: ~3W
- Heat per LED: 33mW (negligible)
- No active cooling required
