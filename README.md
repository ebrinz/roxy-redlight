# Roxy RedLight

An open-source, DIY photobiomodulation (PBM) helmet for scalp LED therapy, designed to promote hair regrowth using evidence-based light therapy parameters.

## Overview

This project aims to create an affordable, effective LED therapy device for androgenetic alopecia (pattern hair loss) treatment based on peer-reviewed clinical research. The device uses an ESP32 microcontroller, a dual-wavelength array of 650nm red and 850nm near-infrared LEDs, 3D-printed housing, and rechargeable battery power.

## Scientific Basis

LED/laser therapy for hair regrowth (photobiomodulation) is FDA-cleared and backed by multiple randomized controlled trials. The mechanism involves stimulating mitochondrial cytochrome c oxidase in hair follicle stem cells, increasing ATP production and promoting the anagen (growth) phase.

### Evidence-Based Parameters

| Parameter | Target Value | Source |
|-----------|-------------|--------|
| Primary Wavelength | 650-660nm (red) | Multiple RCTs |
| Secondary Wavelength | 850nm (NIR) optional | Hamblin 2019 |
| Power Density | ~5 mW/cm² | Standard clinical protocol |
| Fluence per Session | 4 J/cm² | Hamblin 2019 |
| Treatment Duration | 15-20 minutes | Clinical trials |
| Frequency | 3x/week to daily | Various protocols |
| Treatment Course | 16-26 weeks minimum | RCT data |

### Key Research Findings

- **Efficacy:** Meta-analysis shows LLLT comparable to minoxidil for hair growth (Adil & Godwin, 2017)
- **Combination therapy:** LLLT + minoxidil shows synergistic effects (Esmat et al., 2017)
- **Mechanism:** 650nm light upregulates Wnt/β-catenin pathway, extends anagen phase (Yang et al., 2021)
- **Safety:** <10% incidence of mild, transient side effects across 50+ years of use

### Limitations

- Works for **thinning hair only** - cannot revive dead/scarred follicles
- Effect sizes are modest (~17-18 hairs/cm² increase vs sham)
- Requires consistent long-term use (months)
- Individual results vary

## Combination Therapies (Enhanced Protocol)

Research shows **multimodal therapy** significantly outperforms any single treatment:

| Combination | Efficacy Ranking | Evidence |
|-------------|-----------------|----------|
| Minoxidil 5% + Microneedling | #1 (SUCRA 95.8%) | Strong |
| Minoxidil 5% + PBM | Synergistic | Moderate |
| Dual-wavelength PBM (650nm + 850nm) | Enhanced depth | Moderate |

### Why Dual Wavelength?

- **650nm (red):** Targets superficial follicles, early thinning (1-2mm depth)
- **850nm (NIR):** Penetrates 3-4mm to reach miniaturized deep follicles

Both wavelengths activate cytochrome c oxidase but at different tissue depths.

### Recommended Protocol

| Day | Treatment |
|-----|-----------|
| Monday | Microneedling (1.0mm) - no topicals for 24h |
| Tue/Thu/Sat | PBM session (20 min) |
| Wed/Fri/Sun | Minoxidil or caffeine serum |

See [Combination Therapies Research](docs/research-combination-therapies.md) for full protocol details.

## Project Goals

1. **Build a functional PBM helmet** matching clinical trial parameters
2. **Keep costs under $150** (vs $200-600+ commercial devices)
3. **Open-source everything** - hardware, firmware, 3D models
4. **Document the science** - no snake oil claims

## Hardware Design

### Physical Design: Flexible LED Strips

The LED array uses **flexible 3D-printed strips** that attach to any cap or beanie:

- 6 flexible strips with snap-fit LED holders
- Attach via snap buttons (removable) or sew-on tabs
- LEDs rest directly against hair for optimal irradiance
- Wide-angle diffused LEDs (120°) prevent hot spots
- ESP32 enclosure clips to back of cap

See [Flexible LED Strip Design](docs/flexible-led-strip-design.md) for details.

### Bill of Materials - v2.0 (Dual Wavelength)

| Qty | Component | Specification | Est. Cost |
|-----|-----------|--------------|-----------|
| 1 | [LilyGO T-Display S3](https://www.amazon.com/LILYGO-T-Display-S3-ESP32-S3-Development-Soldered/dp/B0BF542H39) | ESP32-S3 with 1.9" LCD | $18 |
| 100 | [Red LEDs (diffused)](https://www.amazon.com/EDGELEC-Diffused-Resistors-Included-Emitting/dp/B077X95F7C) | 5mm, 620-630nm, 120° wide | $8 |
| 50 | [850nm NIR LEDs](https://www.amazon.com/Infrared-Lighting-Electronics-Components-Emitting/dp/B01BVGIZIU) | 5mm, 850nm IR | $8 |
| 5 | [IRLZ44N MOSFETs](https://www.amazon.com/IRLZ44N-IRLZ44NPBF-Mosfet-N-Channel-0-02Ohm/dp/B09SV14RX7) | Logic-level N-ch, 55V 47A | $7 |
| 2 | [18650 Li-ion Cells](https://www.amazon.com/QOJH-18650-Rechargeable-Flashlight-Headlamp/dp/B0D7TG9CBY) | 3.7V, 2600mAh, protected | $12 |
| 1 | [2S BMS + USB-C](https://www.amazon.com/Adeept-18650-Charge-Charging-Board/dp/B0BJTW4QBR) | 7.4V/8.4V, 2A charge | $8 |
| 1 | 18650 Holder | 2-cell series | $3 |
| 1 | Resistor Kit | Includes 150Ω, 100Ω | $7 |
| - | Wire, connectors, misc | JST-XH, silicone wire | $17 |
| - | 3D printed strips | PETG, ~50g | $3 |
| 1 | [Base Cap](https://www.amazon.com/Welding-Beanie-Cotton-Welders-Protection/dp/B0D4DLWLD3) | Cotton welding beanie | $10 |
| | **Total** | | **~$119** |

Full BOM with all Amazon links: [docs/bom-with-links.md](docs/bom-with-links.md)

### Key Design Features

- **Flexible LED strips** attach to any cap/beanie
- **Wide-angle diffused LEDs** (120°) for safe, even coverage
- **1.9" LCD display** showing session progress, battery, stats
- **Two-button navigation** for mode selection and control
- **Timer-controlled sessions** with auto-shutoff
- **Battery monitoring** with low-voltage protection
- **USB-C charging** for convenience

## Firmware Features

- **Dual-channel PWM** for independent red/NIR control
- **Treatment modes:** Red-only, NIR-only, Dual, Alternating
- **LCD display UI** with 6 screens (Home, Session, Stats, Settings, Battery, Safety)
- **Two-button navigation** with context-sensitive actions
- **Session management** with 20-min default and auto-shutoff
- **Persistent storage** for lifetime sessions, minutes, and mode preference
- **Comprehensive safety system:**
  - Over-voltage protection (>8.6V emergency shutdown)
  - Under-voltage cutoff (6.2V)
  - Thermal protection (optional, 45°C cutoff)
  - Daily session limits (3 max)
  - Minimum session gap (60 min)
  - Maximum session duration (30 min hard limit)
- **Unit tested** - 23 tests covering all safety-critical functions

## Project Structure

```
roxy-redlight/
├── README.md                              # This file
├── docs/
│   ├── hardware-assembly.md               # Step-by-step build guide
│   ├── circuit-design.md                  # v2.0 dual-wavelength schematic
│   ├── bom-with-links.md                  # Full BOM with Amazon links
│   ├── flexible-led-strip-design.md       # Physical design for LED strips
│   ├── irradiance-calculations.md         # Power density analysis
│   ├── physical-design-options.md         # Design options evaluated
│   ├── research-lllt-androgenetic-alopecia.md
│   ├── research-photobiomodulation-mechanisms.md
│   ├── research-650nm-rna-sequencing.md
│   └── research-combination-therapies.md  # NIR, microneedling, serums
├── hardware/
│   ├── schematics/                        # KiCad project (TODO)
│   └── 3d-models/
│       └── design-specifications.md       # Full dimensions for all parts
├── firmware/
│   ├── README.md                          # Firmware documentation
│   └── esp32/                             # PlatformIO project
│       ├── platformio.ini                 # Build config (T-Display S3)
│       ├── include/
│       │   ├── config.h                   # Pin definitions, thresholds
│       │   └── display.h                  # Display class API
│       ├── src/
│       │   ├── main.cpp                   # Main application
│       │   └── display.cpp                # LCD UI implementation
│       ├── lib/
│       │   ├── safety/                    # Testable safety module
│       │   │   ├── safety.h
│       │   │   └── safety.cpp
│       │   └── ui/                        # Testable UI state machine
│       │       ├── ui.h
│       │       └── ui.cpp
│       └── test/
│           ├── test_safety/               # Safety unit tests (23 tests)
│           ├── test_ui/                   # UI unit tests (28 tests)
│           └── test_hardware/             # On-device hardware tests (12 tests)
```

## Safety Considerations

### Physical Safety
- **Eye protection:** LEDs should not be visible from wearing position
- **Heat management:** Optional thermistor for thermal cutoff at 45°C
- **Electrical safety:** 2S BMS with overcharge/overdischarge protection

### Firmware Safety (Built-in)
- **Voltage monitoring:** Over-voltage (>8.6V) and under-voltage (<6.2V) emergency shutdown
- **Session limits:** Max 3 sessions/day, 60-min gap between sessions, 30-min hard cutoff
- **Thermal derating:** Reduces power at 40°C, shuts down at 45°C (if sensor installed)
- **All safety code unit tested:** 23 tests covering edge cases

### Disclaimer
- **Not a medical device:** This is an experimental DIY project, not FDA-cleared

## Research Sources

See `/docs/` for full peer-reviewed papers:

1. [Role of LLLT in Androgenetic Alopecia](docs/research-lllt-androgenetic-alopecia.md) - Pillai & Mysore, 2021
2. [Photobiomodulation Mechanisms](docs/research-photobiomodulation-mechanisms.md) - Hamblin, 2019
3. [650nm RNA Sequencing Study](docs/research-650nm-rna-sequencing.md) - Yang et al., 2021
4. [Combination Therapies Research](docs/research-combination-therapies.md) - NIR, microneedling, serums

## Roadmap

- [x] Literature review and parameter research
- [x] Document scientific basis
- [x] Circuit design and schematic
- [x] ESP32 firmware development (LilyGO T-Display S3)
- [x] Display UI with 6 screens
- [x] Safety module with comprehensive protections
- [x] Unit tests for safety-critical code
- [x] Physical design specifications (flexible LED strips)
- [x] BOM with Amazon purchase links
- [x] Irradiance calculations and LED selection
- [ ] 3D model files (LED strips, enclosure)
- [ ] Prototype build and testing
- [ ] Optimization based on thermal/power measurements

## Contributing

This is an open research/maker project. Contributions welcome for:
- Circuit optimization
- 3D model improvements
- Firmware features
- Additional literature review
- Build documentation and photos

## Disclaimer

This project is for **educational and experimental purposes only**. It is not a medical device and makes no therapeutic claims. Consult a dermatologist for hair loss treatment. The authors are not responsible for any injury or adverse effects from building or using this device.

## License

MIT License - See LICENSE file

---

*"The best time to plant a tree was 20 years ago. The second best time is now."*
