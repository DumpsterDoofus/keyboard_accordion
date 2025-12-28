with open('/home/peter/Documents/git/keyboard_accordion/hardware/kicad/daughterboard/daughterboard.kicad_sch') as schematic_file:
    schematic = schematic_file.read()

s = 'CONTROL'
i = 3
while i >= 0:
    schematic = schematic.replace(f'"{s}{i}"', f'"{s}{i+1}"')
    i -= 1

with open('/home/peter/Documents/git/keyboard_accordion/hardware/kicad/daughterboard/daughterboard.kicad_sch', 'w') as schematic_file:
    schematic_file.write(schematic)
