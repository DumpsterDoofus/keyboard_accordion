import kipy
from kipy.util.board_layer import layer_from_canonical_name
from kipy.board_types import BoardPolygon
from kipy.geometry import Vector2

# Note: Need to have 96_key PCB open in KiCad, otherwise API call will fail.
board = kipy.KiCad().get_board()
footprints = {
    footprint.reference_field.text.value: footprint
    for footprint in board.get_footprints()
}

def to_mm(point: Vector2) -> list[float, float]:
    # Convert from nanometers to mm and invert Y axis because KiCad interprets positive Y as down instead of up
    return [point.x / 1_000_000, -point.y / 1_000_000]

def get_board_edge_vertices() -> list[list[float, float]]:
    edge_cuts = [
        drawing
        for drawing in board.get_shapes()
        if drawing.layer == layer_from_canonical_name("Edge.Cuts")
    ]

    if len(edge_cuts) != 1:
        raise ValueError(f"Expected one Edge.Cuts drawing, found {len(edge_cuts)}")

    edge_cut: BoardPolygon = edge_cuts[0]
    polygon = [
        to_mm(node.point)
        for poly in edge_cut.polygons
        for node in poly.outline.nodes
    ]
    return polygon

def get_mounting_hole_positions() -> list[list[float, float]]:
    return [
        to_mm(footprints.get(f'H{i+1}').position)
        for i in range(29)
    ]

def get_key_positions() -> list[list[float, float]]:
    return [
        to_mm(footprints.get(f'KEY{i+1}').position)
        for i in range(96)
    ]

def write_bottom_plate():
    with open('bottom.scad', 'w') as f:
        f.write('$fn = 50;\n')
        f.write('epsilon = 0.0001;\n')

        for mounting_hole_position in get_mounting_hole_positions():
            f.write(f'translate([{mounting_hole_position[0]}, {mounting_hole_position[1]}, 1.6 - epsilon]) {{\n')
            # h difference must be at least 1.6 mm (PCB thickness).
            f.write('    difference() {\n')
            f.write('        cylinder(h=3.4, r=2.5, center=false);\n')
            f.write('        cylinder(h=3.4 + epsilon, r=1.25, center=false);\n')
            f.write('    }\n')
            f.write('}\n')

        f.write('linear_extrude(1.6) {\n')
        f.write(f'    polygon(points={get_board_edge_vertices()});\n')
        f.write('}\n')

def write_top_plate():
    with open('top.scad', 'w') as f:
        f.write('$fn = 50;\n')
        f.write('epsilon = 0.001;\n')
        f.write('wiggle_room = 0.05;\n')

        board_edge_vertices = get_board_edge_vertices()
        i = len(board_edge_vertices)

        f.write(f'a0 = {board_edge_vertices};\n')
        f.write(f'b0 = {list(range(i))};\n')

        key_positions = get_key_positions()
        for index, [x, y] in enumerate(key_positions):
            f.write(f'a{index+1} = {[[x+7,y+7], [x+7,y-7], [x-7,y-7], [x-7,y+7]]};\n')
            f.write(f'b{index+1} = {[i, i+1, i+2, i+3]};\n')
            i += 4

        a = [f'a{j}' for j in range(len(key_positions)+1)]
        b = [f'b{j}' for j in range(len(key_positions)+1)]

        f.write(f"a = concat ({', '.join(a)});\n")
        f.write(f"b = [{', '.join(b)}];\n")

        f.write('linear_extrude(1.6) {\n')
        f.write(f'    polygon(points=a, paths=b);\n')
        f.write('}\n')

        for mounting_hole_position in get_mounting_hole_positions():
            f.write(f'translate([{mounting_hole_position[0]}, {mounting_hole_position[1]}, -(8.3 - wiggle_room)]) {{\n')
            # h difference must be at least 1.6 mm (PCB thickness).
            f.write('    cylinder(h=8.3 - wiggle_room + epsilon, r=1.25 - wiggle_room, center=false);\n')
            f.write('}\n')

write_bottom_plate()
write_top_plate()
