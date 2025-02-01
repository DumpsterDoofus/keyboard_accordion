# Keyboard Accordion

This is a prototype keyboard MIDI controller. It uses a 6-row B-system [chromatic button accordion layout](https://en.wikipedia.org/wiki/Chromatic_button_accordion), also known as a "dugmetara". It's inspired by the Midihex (see [here](https://gullsonix.co.uk/) and [here](https://midi.org/innovation-award/midihex)), which instead uses a [Harmonic Table layout](https://en.wikipedia.org/wiki/Harmonic_table_note_layout).

Virtually nothing is done yet. Status:

* Schematic: Exists but likely broken.
* PCB layout: TODO.
* Firmware: TODO.
* Build/test: TODO.
* Expand from 2 octaves to 8 octaves: TODO.

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

