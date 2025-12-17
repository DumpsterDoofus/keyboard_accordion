with open('/home/peter/Documents/git/keyboard_accordion/hardware/kicad/96_key/96_key.kicad_sch') as schematic_file:
    schematic = schematic_file.read()

s = 'AM'
i = 5
j = 15
while i >= 0:
    while j >= 0:
        schematic = schematic.replace(f'"{s}{i}:{j}"', f'"{s}{i+1}:{j+1}"')
        j -= 1
    i -= 1
    j = 15

with open('/home/peter/Documents/git/keyboard_accordion/hardware/kicad/96_key/96_key.kicad_sch', 'w') as schematic_file:
    schematic_file.write(schematic)
