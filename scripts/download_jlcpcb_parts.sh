#!/bin/bash

# This script was used to download the symbols, footprints, and 3D models for all JLCPCB parts to kicad/library/jlcpcb*. This data is committed to source control so you don't need to run this script after cloning the repo. However, if you modify the parts list, you'll want to rerun this to update the library.

# These "LCSC IDs" show up on JLCPCB's website as "JLCPCB Part #". On LSCS (another website run by the same company), they show up as "LCSC Part #".
lcsc_ids=(
    # DC barrel jack
    # TODO: Replace with C2880553?
    "C720558"

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

    # 24 pin header
    "C2883741"

    # 1 kΩ resistor (250mW 200V ±1% 1206)
    "C4410"

    # Multiplexer
    "C98457"

    # Hall sensor
    "C266230"

    # 100 nF capacitor (25V X7R ±10% 0603)
    "C1590"
)

for lcsc_id in "${lcsc_ids[@]}"; do
    easyeda2kicad --full --output kicad/library/jlcpcb --lcsc_id="$lcsc_id"
done
