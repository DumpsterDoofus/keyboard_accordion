# Case

The case is programmatically generated from the PCB.

To regenerate the files, open [the PCB](../pcb/96_key/96_key.kicad_pcb) in KiCad, and run this:

```sh
# This updates the .scad files.
python3 pcb_to_scad.py

# This converts the .scad files to .3mf.
openscad bottom.scad -o bottom.3mf
openscad top.scad -o top.3mf
```
