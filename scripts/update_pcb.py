# TODO: This file must be run using the system Python environment, not the .venv virtual environment. This is because "pcbnew" is installed systemwide by the KiCad 8 installer, and cannot be installed by pip, so we can't add it to the virtual environment. When KiCad 9 is released, switch from "pcbnew" to "kicad-python" (which can be installed by pip) to fix this annoyance.

import pcbnew

button_positions = [
    [6000, 3000],
    [6000 + 400, 3000 - 692.82],
    [6000 + 800, 3000],
]

button_positions.reverse()

pcb_path = 'kicad/3_key/3_key.kicad_pcb'
board = pcbnew.LoadBoard(pcb_path)
for index, position in enumerate(button_positions):
    index += 1
    switch = board.FindFootprintByReference(f'REF{index}')
    sensor = board.FindFootprintByReference(f'S{index}')
    capacitor = board.FindFootprintByReference(f'C{index}')
    switch.SetPos(pcbnew.VECTOR2I_Mils(float(position[0]), float(position[1])))
    sensor.SetPos(pcbnew.VECTOR2I_Mils(float(position[0]), float(position[1])))
    capacitor.SetPos(pcbnew.VECTOR2I_Mils(float(position[0]), float(position[1]) - 150))
#     switch.SetReference(f"REF{i}")

pcbnew.SaveBoard(pcb_path, board)
