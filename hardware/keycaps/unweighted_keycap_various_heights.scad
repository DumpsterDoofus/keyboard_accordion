// This creates keycaps of 6 different heights (1st row is shortest, 2nd row is 2 mm taller, 3rd row is 4 mm taller, etc.) to achieve row height stepping on the instrument. I found they don't feel nice in practice because of stem wobble. The wobble is not a problem for the shortest key, but becomes increasingly visible and annoying as the keys get taller. For this reason, there is another OpenSCAD document that instead opts for angled keycaps, for use on an angled PCB.
$fn = 100;

dPad1 = 19;
dPad2 = 17;
hPad = 1.6;
hStem = 3.8;
hRise = 10.0;

mirror([0, 0, 1]) {
    for (hRise = [0 : 2 : 10]) {
        translate([hRise * 10.5, 0, 0]) {
            translate([0, 0, hPad/2]) cylinder(h=hPad, r1=dPad1/2, r2=dPad2/2, center=true);

            translate([0, 0, -(hStem + hRise) + 0.002]) difference() {
                translate([0, 0, (hStem + hRise)/2]) union() {
                    cube(size = [6.4, 4.8, hStem + hRise], center = true);
                    cylinder(h=hStem + hRise, r1=2.8, r2=2.8, center=true);
                }
                translate([0, 0, hStem/2 - 0.001]) union() {
                    cube([1.25, 4.15, hStem], center=true);
                    cube([4.15, 1.45, hStem], center=true);
                }
            }
        }
    }
}

