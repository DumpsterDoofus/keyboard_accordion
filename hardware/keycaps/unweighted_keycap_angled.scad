// This creates angled keycaps that can be used on a board whose PCB has been angled so that the back rows are higher than the front rows. The keycap angle counteracts the PCB angle, so that the keycap surfaces are flat similar to the table the instrument sits on. In a separate OpenSCAD file I experimented with keycaps of various heights (for use on a flat PCB), but found stem wobble made the taller keycaps feel annoying.
$fn = 150;

slantDegrees = 8;

lPad = 19;
wPad = 16 * cos(slantDegrees);
hPad = 1.6;
cut = 2.5;

rStem = 2.9;
hStem = 3.8 + 2 * tan(slantDegrees) * rStem;


module CapRound() {
    rotate(a=slantDegrees, v=[1, 0, 0]) translate([0, 0, 0*hPad/2]) scale([18.5/2, 18.5*cos(slantDegrees)/2, hPad]) cylinder(h=1, r1=1, r2=0.9, center=true);
}

module CapRectangular() {
    rotate(a=slantDegrees, v=[1, 0, 0]) translate([0, 0, -hPad/2]) linear_extrude(height = hPad, scale = 0.88) polygon(points=[
            [-lPad/2, -wPad/2 + cut],
            [-lPad/2, wPad/2 - cut],
            [-lPad/2 + cut, wPad/2],
            [lPad/2 - cut, wPad/2],
            [lPad/2, wPad/2 - cut],
            [lPad/2, -wPad/2 + cut],
            [lPad/2 - cut, -wPad/2],
            [-lPad/2 + cut, -wPad/2],
        ]);
}

module Stem() {
    translate([0, 0, -hStem - tan(slantDegrees) * rStem + 0.002]) difference() {
        translate([0, 0, (hStem)/2]) cylinder(h=hStem, r1=2.8, r2=2.8, center=true);
        translate([0, 0, hStem/2 - 0.001]) union() {
            cube([1.25, 4.15, hStem], center=true);
            cube([4.15, 1.45, hStem], center=true);
        }
    }
}

CapRound();
//CapRectangular();
Stem();
