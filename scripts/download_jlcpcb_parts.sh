#!/bin/bash

# This script was used to download the symbols, footprints, and 3D models for all JLCPCB parts to kicad/library/jlcpcb*. This data is committed to source control so you don't need to run this script after cloning the repo. However, if you modify the parts list, you'll want to rerun this to update the library.

# These "LCSC IDs" show up on JLCPCB's website as "JLCPCB Part #". On LSCS (another website run by the same company), they show up as "LCSC Part #".
lcsc_ids=(
    # DC barrel jack
    "C720558"

    # Linear regulator
    "C45908"

    # 10 uF capacitor
    "C1691"

    # 24 pin header
    "C2883741"

    # 1k resistor
    "C4410"

    # Multiplexer
    "C98457"

    # Hall sensor
    "C266230"

    # 100 nF capacitor
    "C155422"
)

for lcsc_id in "${lcsc_ids[@]}"; do
    easyeda2kicad --full --output kicad/library/jlcpcb --lcsc_id="$lcsc_id"
done
