# Bill of Materials - Complete with Links

Roxy RedLight v2.0 - Dual Wavelength PBM Helmet

## Electronics

| Qty | Component | Specification | Est. Cost | Link |
|-----|-----------|---------------|-----------|------|
| 1 | **LilyGO T-Display S3** | ESP32-S3 + 1.9" LCD | $18 | [Amazon](https://www.amazon.com/LILYGO-T-Display-S3-ESP32-S3-Development-Soldered/dp/B0BF542H39) |
| 100 | **Red LEDs (diffused)** | 5mm, 620-630nm, 120° wide angle, 20mA | $8 | [Amazon - EDGELEC 100pcs](https://www.amazon.com/EDGELEC-Diffused-Resistors-Included-Emitting/dp/B077X95F7C) |
| 50 | **850nm NIR LEDs** | 5mm, 850nm IR, 20mA | $8 | [Amazon - Chanzon 100pcs](https://www.amazon.com/Infrared-Lighting-Electronics-Components-Emitting/dp/B01BVGIZIU) |
| 5 | **IRLZ44N MOSFETs** | Logic-level N-channel, 55V 47A | $7 | [Amazon - 5 pack](https://www.amazon.com/IRLZ44N-IRLZ44NPBF-Mosfet-N-Channel-0-02Ohm/dp/B09SV14RX7) |
| 2 | **18650 Li-ion Cells** | 3.7V, 2600mAh, protected | $12 | [Amazon - QOJH 2-pack](https://www.amazon.com/QOJH-18650-Rechargeable-Flashlight-Headlamp/dp/B0D7TG9CBY) |
| 1 | **2S BMS + USB-C Charger** | 7.4V/8.4V, 2A charge | $8 | [Amazon - Adeept](https://www.amazon.com/Adeept-18650-Charge-Charging-Board/dp/B0BJTW4QBR) |
| 1 | **18650 Holder** | 2-cell series | $3 | [Amazon](https://www.amazon.com/LAMPVPATH-Battery-Holder-Leads-Wires/dp/B07F7YZBJN) |
| 1 | **Resistor Kit** | 1/4W, includes 150Ω, 100Ω | $7 | [Amazon - 1280pcs kit](https://www.amazon.com/Resistor-Assorted-Resistors-Assortment-Experiments/dp/B07L851T3V) |
| 1 | **JST-XH Connector Kit** | 2/3/4 pin assortment | $8 | [Amazon](https://www.amazon.com/Connector-Housing-Adapter-Assortment-Kit/dp/B07CTH5NW4) |
| 1 | **Silicone Wire Kit** | 28 AWG, multiple colors | $9 | [Amazon](https://www.amazon.com/Fermerry-Silicone-Stranded-Copper-Electrical/dp/B089CQFLFC) |

**Electronics Subtotal: ~$88**

---

## Physical / Mechanical

| Qty | Component | Specification | Est. Cost | Link |
|-----|-----------|---------------|-----------|------|
| 1 | **Base Cap** | Cotton welding beanie or any cap | $10 | [Amazon - Welding Cap](https://www.amazon.com/Welding-Beanie-Cotton-Welders-Protection/dp/B0D4DLWLD3) |
| 1 | **Snap Button Kit** | 10mm, for attaching strips | $7 | [Amazon](https://www.amazon.com/Snaps-Fasteners-Leather-Buttons-Clothing/dp/B09LCDLD26) |
| 50g | **PETG Filament** | For LED strips (any color) | $3 | (from existing spool) |
| 1 | **Heat Shrink Tubing** | Assorted sizes | $6 | [Amazon](https://www.amazon.com/Ginsco-580-pcs-Assorted-Sleeving/dp/B01MFA3OFA) |
| 1 | **Velcro Strips** | Adhesive backed (optional) | $5 | [Amazon](https://www.amazon.com/VELCRO-Brand-Sticky-Fasteners-Perfect/dp/B000TGSPV6) |

**Physical Subtotal: ~$31**

---

## Optional / Recommended

| Qty | Component | Specification | Est. Cost | Link |
|-----|-----------|---------------|-----------|------|
| 50 | **LED Diffuser Caps** | 5mm, for NIR LEDs | $5 | [Amazon](https://www.amazon.com/Diffused-Holder-Emitting-Diodes-White/dp/B07GDRQ81L) |
| 1 | **Piezo Buzzer** | 5V, for audio feedback | $5 | [Amazon](https://www.amazon.com/Cylewet-Electronic-Sounder-Arduino-CLT1076/dp/B01NCOXB2Q) |
| 1 | **10K NTC Thermistor** | For thermal protection | $6 | [Amazon](https://www.amazon.com/uxcell-Thermistor-Temperature-Sensor-Probe/dp/B07MXXZRY9) |
| 1 | **Power Switch** | Rocker or slide, 3A | $5 | [Amazon](https://www.amazon.com/Position-Switch-Solder-Electronics-Projects/dp/B07QQ22DTB) |

**Optional Subtotal: ~$21**

---

## Tools Required (Not Included)

| Tool | Purpose | Est. Cost | Link |
|------|---------|-----------|------|
| Soldering Iron | Wire connections | $20-40 | [Amazon - Basic Kit](https://www.amazon.com/Soldering-Iron-Kit-Temperature-Desoldering/dp/B07GTGWBGN) |
| Wire Strippers | Prep wires | $8 | [Amazon](https://www.amazon.com/IRWIN-VISE-GRIP-Stripper-8-Inch-2078309/dp/B000JNNWQ2) |
| Multimeter | Testing | $15 | [Amazon](https://www.amazon.com/AstroAI-Digital-Multimeter-Voltage-Tester/dp/B01ISAMUA6) |
| 3D Printer | LED strips, enclosure | $200+ | (or use library/makerspace) |
| Sewing Kit | Attach strips to cap | $5 | [Amazon](https://www.amazon.com/ARTIKA-Supplies-Accessories-Emergency-Beginners/dp/B0714Q2NKD) |

---

## Cost Summary

| Category | Cost |
|----------|------|
| Electronics (required) | $88 |
| Physical (required) | $31 |
| **Minimum Build** | **$119** |
| Optional additions | +$21 |
| **Full Build** | **$140** |

---

## Notes

### LED Wavelength Clarification

- **Red LEDs:** The linked EDGELEC diffused LEDs are 620-630nm. True 650nm deep red LEDs with diffused wide-angle are rare. 620-630nm is still within therapeutic range and clinically proven effective.

- **NIR LEDs:** Standard 850nm LEDs are narrow angle (~20°). Add diffuser caps for better spread, or the hair itself will provide some diffusion.

### Quantity Buffers

- LEDs: Buying 100+50 gives spares for testing and mistakes
- Resistors: Kit includes many values for experiments
- Connectors: Kit includes extras

### Alternative Sources

For potentially lower prices, also check:
- AliExpress (longer shipping)
- Digi-Key / Mouser (for specific specs)
- eBay (mixed quality)

---

## Quick Reference - Key Specs

```
Red LEDs:   620-630nm, 5mm diffused, 120°, 2V @ 20mA
NIR LEDs:   850nm, 5mm clear+diffuser, 1.5V @ 20mA
Battery:    2S Li-ion, 7.4V nominal, 2600mAh
Charger:    USB-C, 2A charge rate
MCU:        ESP32-S3 with 1.9" LCD
MOSFETs:    IRLZ44N logic-level (Vgs < 3V)
```
