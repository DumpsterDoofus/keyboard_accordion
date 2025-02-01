# Keyboard Accordion

This is a prototype keyboard MIDI controller. It uses a 6-row B-system [chromatic button accordion layout](https://en.wikipedia.org/wiki/Chromatic_button_accordion), also known as a "dugmetara". It's inspired by the Midihex (see [here](https://gullsonix.co.uk/) and [here](https://midi.org/innovation-award/midihex)), which instead uses a [Harmonic Table layout](https://en.wikipedia.org/wiki/Harmonic_table_note_layout).

Virtually nothing is done yet. Status:

* Schematic: Exists but likely broken.
* PCB layout: TODO.
* Firmware: TODO.
* Build/test: TODO.
* Expand from 2 octaves to 8 octaves: TODO.

## Developers

To clone the repo:

```sh
git clone --recurse-submodules https://github.com/DumpsterDoofus/keyboard_accordion.git
```

You can review the KiCad project and view the schematic/PCB in `kicad/3_key`.

If you also want to download the KiCad libraries:

```
python3 -m venv .venv
source .venv/bin/activate
pip install --requirement requirements.txt
```
