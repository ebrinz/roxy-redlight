# Irradiance Calculations

Calculating the power density (mW/cm²) at the scalp for our LED array design.

## LED Specifications

| Parameter | Red LEDs | NIR LEDs |
|-----------|----------|----------|
| Wavelength | 650nm | 850nm |
| Quantity | 60 | 30 |
| Forward current | 20mA | 20mA |
| Radiant power (typical) | ~20mW | ~30mW |
| Viewing angle | 20-30° | 20-30° |

**Note:** 5mm LEDs vary widely. "Radiant power" (actual light energy) differs from "luminous intensity" (mcd rating, weighted for human vision). IR LEDs are rated in mW/sr (radiant intensity).

---

## Single LED Irradiance vs Distance

For a single LED with narrow beam (20° viewing angle):

```
Irradiance at center of beam = Power / Illuminated Area
Area = π × (distance × tan(half-angle))²
```

### Narrow Beam LED (20° viewing angle, 10° half-angle)

| Distance | Beam Diameter | Area | Peak Irradiance (25mW LED) |
|----------|---------------|------|---------------------------|
| 0mm (surface) | ~3mm | 0.07 cm² | ~350 mW/cm² |
| 5mm | 1.8mm | 0.025 cm² | ~1000 mW/cm² ⚠️ |
| 10mm | 3.5mm | 0.10 cm² | ~250 mW/cm² |
| 20mm | 7.0mm | 0.39 cm² | ~65 mW/cm² |
| 30mm | 10.5mm | 0.87 cm² | ~29 mW/cm² |
| 50mm | 17.6mm | 2.4 cm² | ~10 mW/cm² ✓ |

### Wide Beam LED (60° viewing angle, 30° half-angle)

| Distance | Beam Diameter | Area | Peak Irradiance (25mW LED) |
|----------|---------------|------|---------------------------|
| 5mm | 5.8mm | 0.26 cm² | ~96 mW/cm² |
| 10mm | 11.5mm | 1.05 cm² | ~24 mW/cm² |
| 15mm | 17.3mm | 2.36 cm² | ~11 mW/cm² ✓ |
| 20mm | 23.1mm | 4.19 cm² | ~6 mW/cm² ✓ |

**Target zone: 5-10 mW/cm²** (shaded ✓)

---

## Key Insight: Beam Angle Matters More Than Distance

With **narrow beam (20°) LEDs at 5-10mm distance**, we get dangerously high irradiance at the beam center (250-1000 mW/cm²). This could cause:
- Hot spots
- Potential thermal damage
- Biphasic response (too much = inhibition, not stimulation)

With **wide beam (60°) LEDs at 10-20mm distance**, we get therapeutic levels (10-25 mW/cm²).

---

## Overlapping Beams: Array Analysis

When multiple LEDs are spaced across the scalp, their beams overlap, creating more uniform coverage.

### Our Array Configuration
- 90 total LEDs
- Scalp coverage area: ~600-700 cm²
- Average LED spacing: ~25-30mm

### Total Power Calculation

| Channel | Count | Power Each | Total Power |
|---------|-------|------------|-------------|
| Red (650nm) | 60 | 20mW | 1,200 mW |
| NIR (850nm) | 30 | 30mW | 900 mW |
| **Combined** | 90 | - | **2,100 mW** |

### Average Irradiance (Uniform Distribution)

```
Average Irradiance = Total Power / Treatment Area
                   = 2,100 mW / 650 cm²
                   = 3.2 mW/cm²
```

This is **below our 5 mW/cm² target**. However:
- Actual distribution is not uniform
- Peak irradiance under each LED is higher
- Gaps between LEDs have lower irradiance

---

## Achieving Target: Design Recommendations

### Option 1: Use Wide-Angle LEDs (Recommended)

Select LEDs with **60-120° viewing angle** instead of narrow 20° beam:

| LED Type | Viewing Angle | Distance for 5-10 mW/cm² |
|----------|---------------|--------------------------|
| Narrow beam | 20° | 40-50mm (too far!) |
| Medium beam | 45° | 20-30mm |
| Wide beam | 60° | 15-20mm ✓ |
| Very wide | 120° | 5-10mm ✓ |

**Search for:** "5mm LED 60 degree" or "wide angle LED 650nm"

### Option 2: Use Diffusers

Add frosted diffuser caps to narrow-beam LEDs:
- Spreads beam from 20° to ~60-90°
- Slight power loss (~10-20%)
- More uniform coverage

### Option 3: Increase LED Count

More LEDs at lower power = more uniform coverage:
- Currently: 90 LEDs × 25mW = 2,250mW total
- Alternative: 150 LEDs × 15mW = 2,250mW total
- Better overlap, fewer hot spots

### Option 4: Adjust Spacing

Optimal LED spacing for given beam angle and distance:

```
Spacing ≈ 2 × Distance × tan(half-angle) × 0.7
        (0.7 factor ensures ~30% overlap)
```

For 60° LEDs at 15mm distance:
```
Spacing = 2 × 15mm × tan(30°) × 0.7
        = 2 × 15 × 0.577 × 0.7
        = 12mm spacing
```

For 20° LEDs at 15mm distance:
```
Spacing = 2 × 15mm × tan(10°) × 0.7
        = 2 × 15 × 0.176 × 0.7
        = 3.7mm spacing (impractical!)
```

---

## Revised BOM Recommendation

| Original Spec | Revised Spec | Reason |
|---------------|--------------|--------|
| 5mm 650nm LED (any angle) | 5mm 650nm LED **60° wide angle** | Therapeutic irradiance at 15-20mm |
| 5mm 850nm LED (any angle) | 5mm 850nm LED **60° wide angle** | Even coverage |

### LED Spacing Guide

| Distance from Scalp | LED Beam Angle | Recommended Spacing |
|---------------------|----------------|---------------------|
| 5-10mm (contact) | 120° diffused | 15-20mm |
| 10-15mm (hair gap) | 60° wide | 12-15mm |
| 15-20mm (bonnet) | 60° wide | 15-20mm |
| 20-30mm (helmet) | 45° medium | 20-25mm |

---

## Final Design Parameters

Based on this analysis:

```
┌─────────────────────────────────────────────────────┐
│  RECOMMENDED DESIGN                                 │
├─────────────────────────────────────────────────────┤
│  LED type:        5mm, 60° viewing angle            │
│  Distance:        10-15mm from scalp                │
│  Spacing:         15mm between LEDs                 │
│  Target irradiance: 5-10 mW/cm²                     │
│  Session time:    20 min (→ 6-12 J/cm² fluence)     │
│  Total LEDs:      90 (60 red + 30 NIR)              │
└─────────────────────────────────────────────────────┘
```

### Fluence Check

At 7.5 mW/cm² for 20 minutes:
```
Fluence = Power Density × Time
        = 7.5 mW/cm² × 1200 seconds
        = 9,000 mJ/cm²
        = 9 J/cm²
```

This is within the **4-10 J/cm² therapeutic range** for hair growth.

---

## Safety Margin

Our design has built-in safety:

| Concern | Mitigation |
|---------|------------|
| Hot spots | Wide-angle LEDs distribute power |
| Overexposure | 20-min timer, 30-min hard limit |
| Thermal | LEDs at 20mA generate minimal heat (~0.5W total waste heat) |
| Eye safety | Red/NIR at these levels is Class 1 (safe) |

### Maximum Safe Exposure

Per research, thermal damage risk begins at **>100 mW/cm²**. Our worst-case peak (directly under LED at 10mm) is ~24 mW/cm² with wide-angle LEDs - well under the danger zone.
