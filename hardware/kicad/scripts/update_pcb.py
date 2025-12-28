# TODO: This file must be run using the system Python environment, not the .venv virtual environment. This is because "pcbnew" is installed systemwide by the KiCad 8 installer, and cannot be installed by pip, so we can't add it to the virtual environment. When KiCad 9 is released, switch from "pcbnew" to "kicad-python" (which can be installed by pip) to fix this annoyance.

from typing import Dict, List
import pcbnew
import csv

# button_positions = [
#     [6000 + 400, 3000 - 692.82],
#     [6000 + 800, 3000],
#     [6000, 3000],
# ]
# with open('temp/button_positions.csv', newline='') as f:
#     button_positions = [
#         row
#         for row in csv.reader(f)
#     ]

pcb_path = '96_key/96_key.kicad_pcb'
board: pcbnew.BOARD = pcbnew.LoadBoard(pcb_path)

i = 28
while i >= 0:
    board.FindFootprintByReference(f'H{i}').SetReference(f'H{i+1}')
    i -= 1

# index = 0
# for footprint in board.GetFootprints():
#     if footprint.GetReference() == 'REFn':
#         footprint.SetReference(f'KEY{index}')
#         index += 1

# found: Dict[str, list] = {}
# for footprint in board.GetFootprints():
#     reference = footprint.GetReference()
#     array = found.get(reference)
#     if array is None:
#         found[reference] = [footprint]
#     else:
#         array.append(footprint)
# for reference, footprints in found.items():
#     if len(footprints) > 1:
#         print(f'{reference} has {len(footprints)} copies')
#         for footprint in footprints[1:]:
#             board.Delete(footprint)

# for index, position in enumerate(button_positions):
#     # if index <= 15:
#     #     continue
#     # if index >= 48:
#     #     break
#     switch: pcbnew.FOOTPRINT = board.FindFootprintByReference(f'KEY{index}')
#     sensor: pcbnew.FOOTPRINT = board.FindFootprintByReference(f'S{index}')
#     capacitor: pcbnew.FOOTPRINT = board.FindFootprintByReference(f'C{index}')
#     # switch.SetReference(f'KEY{index}')
#     # sensor.SetReference(f'S{index}')
#     # capacitor.SetReference(f'C{index}')
#     flip = index % 16 <= 7
#     rotation = 180 if flip else 0
#     sensor.SetOrientationDegrees(rotation)
#     capacitor.SetOrientationDegrees(rotation)
#     switch.SetPos(pcbnew.VECTOR2I_Mils(float(position[0]), float(position[1])))
#     sensor.SetPos(pcbnew.VECTOR2I_Mils(float(position[0]), float(position[1])))
#     capacitor.SetPos(pcbnew.VECTOR2I_Mils(float(position[0]), float(position[1]) + 110*(-1 if flip else 1)))

# for i in range(12):
#     mux: pcbnew.FOOTPRINT = board.FindFootprintByReference(f'AM{i}')
#     pads: List[pcbnew.PAD] = mux.Pads()
#     for pad in pads:
#         net: pcbnew.NETINFO_ITEM = pad.GetNet()
#         name: str = net.GetNetname()
#         if name.startswith('AM'):
#             fixed_name = name.replace(name[0:3], f'AM{i}')
#             fixed_net: pcbnew.NETINFO_ITEM = board.FindNet(fixed_name)
#             if fixed_net is None:
#                 # raise ValueError()
#                 fixed_net = pcbnew.NETINFO_ITEM(board, fixed_name)
#                 board.Add(fixed_net)

#             pad.SetNet(fixed_net)

pcbnew.SaveBoard(pcb_path, board)
