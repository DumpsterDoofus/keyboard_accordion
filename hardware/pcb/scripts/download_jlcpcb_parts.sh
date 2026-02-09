#!/bin/bash

# This script was used to download the symbols, footprints, and 3D models for all JLCPCB parts to kicad/library/jlcpcb*. This data is committed to source control so you don't need to run this script after cloning the repo. However, if you modify the parts list, you'll want to rerun this to update the library.

# These "LCSC IDs" show up on JLCPCB's website as "JLCPCB Part #". On LSCS (another website run by the same company), they show up as "LCSC Part #".
lcsc_ids=(
    # DC barrel jack (2.5mm 6.3mm)
    "C720558"

    # DC barrel jack (2.1mm 6.3mm)
    "C7498153"

    # Step down regulator
    "C479075"

    # 10 uF capacitor (16V X5R ±10% 0805)
    "C1713"

    # 22 uF capacitor (16V X5R ±10% 1206)
    "C90146"

    # 100 kΩ resistor (100mW 75V ±1% 0603)
    "C14675"

    # 22.1 kΩ resistor (100mW 75V ±1% 0603)
    "C137768"

    # 220 pF capacitor (50V X7R ±10% 0603)
    "C107081"

    # 4.7 uH inductor (4.3A 7A saturation ±20% 4.1x4.1mm)
    "C39676083"

    # 24 pin header (top-facing)
    "C2883741"

    # 14 pin header (side-facing)
    "C2897396"

    # 14 pin header (top-facing)
    "C18078135"

    # 1 kΩ resistor (250mW 200V ±1% 1206)
    "C4410"

    # Multiplexer
    "C98457"

    # Hall sensor
    "C266230"

    # 100 nF capacitor (25V X7R ±10% 0603)
    "C1590"



    # JLCBCP Basic Catalog

    # 33Ω 50V 62.5mW Thick Film Resistor ±1% ±100ppm/℃ 0402
    "C25105"

    # 10V 4.7uF X5R ±20% 0402 Multilayer Ceramic Capacitors
    "C23733"

    # 100nF 16V X7R ±10% 0402 Multilayer Ceramic Capacitors
    "C1525"

    # 15pF 50V C0G ±5% 0402 Multilayer Ceramic Capacitors
    "C1548"

    # 1kΩ 50V 62.5mW Thick Film Resistor ±1% ±100ppm/℃ 0402 Chip Resistor
    "C11702"

    # 5.1kΩ 50V 62.5mW Thick Film Resistor ±1% ±100ppm/℃ 0402 Chip Resistor
    "C25905"

    # 22kΩ 50V 62.5mW Thick Film Resistor ±1% ±100ppm/℃ 0402
    "C25768"

    # 1uF 25V X5R ±10% 0402 Multilayer Ceramic Capacitors
    "C52923"

    # Pushbutton switch
    "C720477"

    # 120Ω@100MHz 2A 50mΩ ±25% 0603 Ferrite Beads, note Bus Pirate 6 uses C139190 but it's in the extended catalog and seems worse
    "C14709"

    # JLCBCP Extended Catalog

    # 27Ω 50V 62.5mW Thick Film Resistor ±1% ±100ppm/℃ 0402
    "C25100"

    # RP2354A: Commented out because KiCad library already has a symbol and footprint for it, just here for reference.
    #"C41378174"

    # RP inductor, note not needed if we use the 1.1V LDO
    # "C42411119"

    # RP crystal
    "C20625731"

    # JST SWD debug connector
    "C160403"

    # 1.1V 300mA LDO regulator, see also C485148 which is smaller and recommended in https://www.youtube.com/watch?v=UWSLP_WywtY but more expensive and less stocked
    "C485147"

    # 3.3V 500 mA LDO
    "C168807"

    # ADC
    "C2866175"

    # 8-channel multiplexer
    "C3007952"

    # 1A LDO regulator
    "C507885"

    # USB C receptacle
    "C165948"
)

for lcsc_id in "${lcsc_ids[@]}"; do
    # TODO: See note about 3D model incorrect offsets.
    easyeda2kicad --full --output library/jlcpcb --lcsc_id="$lcsc_id"
    # JLC2KiCadLib -dir library/jlcpcb -symbol_lib symbols $lcsc_id
done
