from typing import List
import math
import numpy
import csv

def unit_vector(
        theta: float,
    ) -> numpy.ndarray:

    return numpy.array([
        math.cos(theta),
        math.sin(theta)
    ])

def get_button_positions(
        top_left_button_position: List[int],
        button_spacing: float,
    ) -> numpy.ndarray:

    # Note: KiCAD uses a left-handed coordinate system, where increasing X moves to the right but increasing Y points down (instead of the usual up). These position vectors are expressed in KiCAD's coordinate system.
    minor_third = unit_vector(0)
    half_step = unit_vector(2 * math.pi / 6)
    duplicate = 3 * half_step - minor_third

    # These are the positions for 2 octaves of buttons.
    raw_positions = [
        top_left_button_position + button_spacing * (half_steps * half_step + duplicates * duplicate + minor_thirds * minor_third)
        for duplicates in range(2)
        for half_steps in range(3)
        for minor_thirds in range(8)
    ]

    # The above positions are perfectly usable. However, I added the below reordering step them to better match the topology of the pins on the multiplexers so that vias aren't needed.
    reordered_positions = []
    indices = set()
    for i in range(6):
        flip = i % 2 == 0
        offset = 7 if flip else 0
        multiplier = -1 if flip else 1
        for j in range(8):
            index = 8 * i + multiplier * j + offset
            indices.add(index)
            reordered_positions.append(raw_positions[index])
    if len(indices) != len(raw_positions):
        raise ValueError('Each index should occur exactly once in a reordering, but this was not the case. This is a bug.')

    # Finally, duplicate the 2 octaves 4 times, to give the full 8 octave layout.
    all_positions = []
    for i in range(4):
        for position in reordered_positions:
            all_positions.append(position + 8 * i * minor_third * button_spacing)

    return numpy.array(all_positions)

# Units are in mils.
button_positions = get_button_positions(
    # Edit if necessary to avoid collision with other parts on PCB.
    top_left_button_position=[0, 0],

    # 800 mils is 20.32 mm, which is a reasonable (but slightly larger than traditional) distance between accordion keys.
    button_spacing=800)

with open('temp/button_positions.csv', 'w', newline='') as file:
    writer = csv.writer(file)
    writer.writerows(button_positions)
