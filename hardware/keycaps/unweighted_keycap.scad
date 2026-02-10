$fn = 100;

dPad = 19;
hPad = 2.5;
overhangFactor = 1.04;
intersection() {
    union() {
        rotate_extrude(convexity = 10)
            translate([dPad/2 - hPad*overhangFactor, 0, 0])
                circle(r = hPad*overhangFactor);
         cylinder(h=2*hPad*overhangFactor, r1=dPad/2 - hPad*overhangFactor, r2=dPad/2 - hPad*overhangFactor, center=true);
    }
    cylinder(h=hPad, r1=2*dPad, r2=2*dPad);
}

hStem = 3.9;
hRise = 10.0;
translate([0, 0, -(hStem + hRise) + 0.002]) difference() {
    translate([0, 0, (hStem + hRise)/2]) cube(size = [6.4, 4.8, hStem + hRise], center = true);
    translate([0, 0, hStem/2 - 0.001]) union() {
        cube([1.25, 4.15, hStem], center=true);
        cube([4.15, 1.45, hStem], center=true);
    }
}
