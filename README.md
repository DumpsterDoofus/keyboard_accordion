# Keyboard Accordion

This is a prototype keyboard MIDI controller. It uses a 6-row B-system [chromatic button accordion layout](https://en.wikipedia.org/wiki/Chromatic_button_accordion), also known as a "dugmetara". It's inspired by the Midihex (see [here](https://gullsonix.co.uk/) and [here](https://midi.org/innovation-award/midihex)), which instead uses a [Harmonic Table layout](https://en.wikipedia.org/wiki/Harmonic_table_note_layout).

Virtually nothing is done yet. Status:

* 3-key prototype
    * Done: Schematic, PCB layout/manufacturing, firmware and electrical testing: Done.
    * TODO: Key plate, case.
* 192-key final version: TODO.

## Developers

### To view schematic/PCB

Clone the repo:

```sh
git clone --recurse-submodules https://github.com/DumpsterDoofus/keyboard_accordion.git
```

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

Note: The this script uses `easyeda2kicad`, which seems to have a bug (see [here](https://github.com/uPesy/easyeda2kicad.py/issues/142) and [here](https://github.com/uPesy/easyeda2kicad.py/issues/91)) where the 3D models are sometimes offset in space by a large amount. The offset seems to vary by part number. As a workaround, in KiCad open Footprint Editor, open the footprint, and in File > Footprint Properties, look at the 3D view of the part. If the part doesn't line up with the solder pads or is so far away that it isn't even visible, edit the "Offset" values (try starting with `[0, 0, 0]`). TODO: Try using https://github.com/TousstNicolas/JLC2KiCad_lib instead?
