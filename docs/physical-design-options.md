# Physical Design Options

Exploring options for the LED array mounting structure and ESP32 enclosure.

## Design Requirements

1. **Full scalp coverage** - LEDs must reach crown, temples, and hairline
2. **Consistent LED-to-scalp distance** - ~10-15mm for optimal irradiance
3. **Comfortable for 20-minute sessions** - lightweight, breathable
4. **Adjustable fit** - accommodate different head sizes
5. **Secure wiring** - protect connections from movement
6. **Easy to build** - minimal custom fabrication

---

## Option A: Welding Cap + 3D Printed LED Strips

**Concept:** Use a soft welding beanie cap as the comfortable base, with rigid 3D-printed LED strip holders attached to the fabric.

### Components
| Item | Description | Est. Cost |
|------|-------------|-----------|
| Welding beanie cap | Cotton, adjustable elastic | $8-12 |
| 3D printed LED strips (x6) | PETG, snap-fit LED holders | ~$3 filament |
| Velcro adhesive strips | Attach strips to cap | $5 |

### Pros
- Very comfortable and breathable
- Adjustable to any head size
- Washable cap (remove electronics first)
- LED strips can follow head contour

### Cons
- LED positioning may shift during use
- Requires sewing or strong adhesive
- More assembly complexity

### LED Strip Design
```
┌─────────────────────────────────────┐
│  3D Printed LED Strip (x6)          │
│  ○──○──○──○──○──○──○──○──○──○       │ 10 LEDs per strip
│  ↑                                  │
│  Snap-fit 5mm LED holders           │
│  Flexible hinge between segments    │
└─────────────────────────────────────┘
```

---

## Option B: Hard Hat Suspension + Dome Shell

**Concept:** Use an adjustable hard hat suspension system inside a 3D-printed or purchased dome shell. LEDs mount to the rigid shell.

### Components
| Item | Description | Est. Cost |
|------|-------------|-----------|
| Hard hat suspension | 4-point ratchet adjust | $8-12 |
| Plastic dome/bowl | ~12" diameter | $10-15 |
| 3D printed LED mounts | Internal brackets | ~$5 filament |

### Pros
- Professional, rigid structure
- Consistent LED-to-scalp distance
- Easy to wire (fixed positions)
- Adjustable fit via ratchet knob

### Cons
- Heavier than fabric options
- Less breathable
- Industrial appearance
- Requires finding suitable dome

### Sources
- [Universal Hard Hat Suspension](https://www.amazon.com/Universal-Hard-Suspension-Replacement-Liner/dp/B0FPCTZC9T) - ~$10

---

## Option C: Salon Bonnet Hood (Recommended)

**Concept:** Repurpose a soft bonnet hair dryer hood. These are already designed to sit on the head and have large interior space for our electronics.

### Components
| Item | Description | Est. Cost |
|------|-------------|-----------|
| Soft bonnet hood | Hair dryer attachment style | $10-15 |
| 3D printed LED ring | Mounts inside bonnet | ~$4 filament |
| Elastic headband | Secures hood to head | included |

### Pros
- **Already designed for head coverage**
- Large interior space for wiring
- Soft, comfortable material
- Adjustable drawstring fit
- Looks less "DIY" than other options
- Blocks ambient light (better treatment)

### Cons
- May trap heat (add ventilation holes)
- Need to cut hole for wires to ESP32

### Sources
- [Winvin Soft Bonnet Hood](https://www.amazon.com/Portable-Soft-Bonnet-Dryer-Attachment/dp/B08423SW9S) - ~$10
- [TEPENAR Bonnet Hood](https://www.amazon.com/Bonnet-Hood-Hair-Dryer-Attachment/dp/B0B2J5FN4T) - ~$12

### LED Layout Inside Bonnet
```
        ┌─────────────────┐
       /                   \
      /    ○ ○ ○ ○ ○ ○      \    Crown ring (18 LEDs)
     │                       │
     │  ○               ○    │
     │    ○           ○      │   Side arrays
     │      ○       ○        │   (12 LEDs each side)
     │        ○   ○          │
     │                       │
      \    ○ ○ ○ ○ ○ ○      /    Hairline ring (18 LEDs)
       \                   /
        └───────┬─────────┘
                │
            Wire exit to ESP32
```

---

## Option D: Bicycle Helmet Modification

**Concept:** Gut a cheap bicycle helmet, use its retention system, mount LEDs to interior.

### Components
| Item | Description | Est. Cost |
|------|-------------|-----------|
| Cheap bicycle helmet | Basic model | $15-20 |
| Remove foam padding | Keep shell + retention | $0 |
| 3D printed LED brackets | Mount to vents | ~$3 filament |

### Pros
- Built-in adjustment dial
- Lightweight rigid structure
- Vents provide natural cooling
- Already head-shaped

### Cons
- Vents create gaps in LED coverage
- May look odd without padding
- Limited interior space
- Hard to mount LEDs cleanly

---

## Option E: 3D Printed Full Helmet

**Concept:** Design and print a complete custom helmet optimized for LED placement.

### Components
| Item | Description | Est. Cost |
|------|-------------|-----------|
| 3D printed helmet (PETG) | ~300g filament | $8-10 |
| Foam padding strips | Comfort liner | $5 |
| Elastic headband | Retention | $3 |

### Pros
- Perfect LED placement
- Custom ventilation
- Integrated ESP32 mount
- Fully optimized design

### Cons
- **Large print - requires 250mm+ bed**
- Long print time (12+ hours)
- Complex multi-part assembly
- Requires CAD skills to design

---

## Recommendation: Option C (Salon Bonnet)

The salon bonnet hood is the best balance of:
- **Ease of build** - minimal custom parts needed
- **Comfort** - soft, adjustable, designed for head wear
- **Coverage** - full scalp access
- **Cost** - ~$12 for the hood
- **Appearance** - less "prototype" looking

### Implementation Plan

1. Purchase soft bonnet hood
2. Design 3D-printed LED ring that fits inside (~8" diameter)
3. Cut small wire exit hole at back
4. Mount ESP32 enclosure to outside of bonnet
5. Route flat ribbon cable through hole

---

## ESP32 Enclosure (All Options)

Regardless of helmet choice, we need a small enclosure for the T-Display S3 and BMS.

### Enclosure Requirements
- Fits T-Display S3 (68 x 28 x 11mm)
- Display window visible
- Access to USB-C port
- Button access (GPIO0, GPIO14)
- Wire exit for LED connections
- Battery holder space (2x 18650)
- Mounting method (clip, velcro, strap)

### Enclosure Design
```
┌─────────────────────────────────────────┐
│  ┌─────────────────┐                    │
│  │   LCD WINDOW    │  ← Display visible │
│  └─────────────────┘                    │
│  [BTN1]      [BTN2]   ← Button access   │
│                                         │
│  ┌─────────────────────────────────┐    │
│  │        BATTERY HOLDER           │    │
│  │      [ 18650 ]  [ 18650 ]       │    │
│  └─────────────────────────────────┘    │
│                                         │
│  ═══════════════════  ← Wire exit       │
│         [USB-C]       ← Charging port   │
└─────────────────────────────────────────┘
   Approx: 100mm x 70mm x 30mm
```

---

## Decision Needed

Which option should we proceed with?

- **Option A:** Welding Cap + LED Strips (most flexible, DIY feel)
- **Option B:** Hard Hat Suspension + Dome (rigid, industrial)
- **Option C:** Salon Bonnet Hood (recommended - comfortable, easy)
- **Option D:** Bicycle Helmet Mod (lightweight but gaps)
- **Option E:** Full 3D Print (optimal but complex)
