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
)

for lcsc_id in "${lcsc_ids[@]}"; do
    # TODO: See note about 3D model incorrect offsets.
    easyeda2kicad --full --output kicad/library/jlcpcb --lcsc_id="$lcsc_id"
    # JLC2KiCadLib -dir kicad/library/jlcpcb -symbol_lib symbols $lcsc_id
done
