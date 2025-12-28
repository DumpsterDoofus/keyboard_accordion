# Hardware

### To view schematic/PCB

You can open the KiCad project at `kicad/3_key` to view the schematic/PCB.

For the PCB, if you want 3D models of parts to render, in Preferences > Configure Paths, define a variable named `EASYEDA2KICAD` with value `${KIPRJMOD}/../library`.

> TODO: Is there a way to source control this variable?

### To modify schematic/PCB

Download Python dependencies:

```sh
python3 -m venv .venv
source .venv/bin/activate
pip install --requirement requirements.txt
```

To modify the JLCPCB parts list:

```sh
./scripts/download_jlcpcb_parts.sh
```

Note: This script uses `easyeda2kicad`, which seems to have a bug (see [here](https://github.com/uPesy/easyeda2kicad.py/issues/142) and [here](https://github.com/uPesy/easyeda2kicad.py/issues/91)) where the 3D models are sometimes offset in space by a large amount. The offset seems to vary by part number. As a workaround, in KiCad open Footprint Editor, open the footprint, and in File > Footprint Properties, look at the 3D view of the part. If the part doesn't line up with the solder pads or is so far away that it isn't even visible, edit the "Offset" values (try starting with `[0, 0, 0]`). I also tried using https://github.com/TousstNicolas/JLC2KiCad_lib but saw similar incorrect offsets.
