## Key spacing

For the Roland accordion:

* Horizontal spacing is 19.06 mm, which is almost the same spacing as the keys on my mechanical keyboard.
* Vertical spacing is about 15.5 mm (possibly harder to measure due to angling), which is less than the 19.06 mm * sin(2*pi/6) = 16.5 mm spacing of a true hexagonal lattice.

For the Orfey accordion:

* Horizontal spacing is 19.81 mm.
* Vertical spacing is 18.25 mm, which is more than the 19.81 mm * sin(2*pi/6) = 17.16 mm spacing of a true hexagonal lattice.

On the mechanical keyboard, there is a little space between the switches, so we could theoretically make the spacing denser than the Roland, but I don't think this would make sense.

Going to try 20 mm horizontal spacing and 20 mm * sin(2*pi/6) = 17.32 mm vertical spacing.

https://en.wikipedia.org/wiki/Scientific_pitch_notation
    88 keys on normal keyboard (MIDI number 21-108 in table), we could do 96 (MIDI Number 17-112, F0-E8, 8 octaves).
https://remcycles.net/blog/cba_keyboard.html
    Uses mechanical switches instead of Hall effect switches, no links to PCB designs, although could maybe contact him
https://wooting.io/product/lekker-switch-linear60
    There is also a v2 that is twice the cost but supposedly less wobbly.

https://github.com/rana-sylvatica/circle-keycaps
    Round keycaps that can be 3D printed by JLCPCB.

https://gateron.com/u_file/2312/26/file/GATERONMagneticOrangeSwitch-KS-20TO10B050NW-Y64.pdf
    Schematic for switch

https://github.com/sssata/fluxpad/tree/fluxpad-2/ECAD/FluxpadKicad
https://github.com/roxandtol/roxpad
    Uses Lekker switches, might be able to copy?



https://www.kicad.org/download/linux/

https://github.com/heiso/macrolev
https://github.com/geoffder/dometyl-keyboard

https://www.youtube.com/watch?v=TfKz_FbZWLQ
https://www.youtube.com/watch?v=sFR6E_Ejot0
    GH39FKSW Hall effect sensor https://jlcpcb.com/partdetail/GoChip_Elec_Tech_Shanghai-GH39FKSW/C266230
    CD74HC4067SM(96?) multiplexer https://jlcpcb.com/partdetail/TexasInstruments-CD74HC4067SM96/C98457
    STM32 Black Pill microcontroller https://www.adafruit.com/product/4877
        Has 10(?) analog inputs
    https://github.com/riskable/void_switch_65_pct
        Keyboard layout
    https://github.com/riskable/kicad_blackpill
        Black Pill symbol and footprint
    https://github.com/riskable/void_switch_kicad
        Custom magnetic switch symbol and footprint

https://www.youtube.com/watch?v=qXeD4k4cafE
https://www.youtube.com/watch?v=ezk02GJ9iMs

https://www.youtube.com/watch?v=09DSHwn-Brg
    At 40:25 he says (192 keys) / (16 keys/multiplexer) = 12 is more than the number of analog inputs on a Teensy, but according to docs Teensy 4.1 has 18 analog inputs?
    GullSonix Midihex, uses a different layout
    OH49E-S Hall effect sensors https://jlcpcb.com/partdetail/Nanjing_OuzhuoTech-OH49ES/C85573
    HC4067 multiplexer https://jlcpcb.com/partdetail/TexasInstruments-CD74HC4067M96/C496123
        Each can read up to 16 Hall effect sensors, so might have each PCB board have 2 octaves (48 keys) and 3 multiplexers.
    Teensy 4.1 microcontroller https://www.pjrc.com/store/teensy41.html
        Has 18 analog inputs (attach 1 to each multiplexer?)

https://github.com/TeensyUser/doc/wiki/ECAD
    Apparently you can create your own Teensy integrated into a larger PCB system, and I suspect that's what the GullSonix guy did.

https://github.com/XenGi/teensy_library
https://github.com/XenGi/teensy.pretty
    Teensy KiCad symbol and footprint
https://forum.pjrc.com/index.php?threads/kicad-symbols-and-footprints-for-teensy-boards.75374/
    Import instructions

https://github.com/pedvide/ADC
https://forum.pjrc.com/index.php?threads/high-speed-sampling-of-all-analog-channels-on-teensy-4-1.70646/
    Evidently the 18 analog pins are connected to 2 ADC units, but each ADC only connects to a subset of the 18 pins. For best sample rate, will need to connect multiplexers to pins in such a way that we can divide reads across the 2 ADCs and run both in parallel.
    ADC0: A0-A11, A16-A17
    ADC1: A0-A9, A12-A17
    Maybe use ADC1 A0-A5 and ADC0 A6-A11 for the 12 multiplexer inputs?

https://www.facebook.com/groups/7662621407172676
    Discussion group

https://forum.pjrc.com/index.php?threads/16-channel-mux-or-switch-register-for-reading-keyswitches.59127/
    MCP23017 might be alternative to HC4067

https://forum.pjrc.com/index.php?threads/velocity-sensitive-contactless-midi-keyboard-with-polyphonic-aftertouch-for-teensy-synth.75078/
    Piano layout but similar idea

https://www.pjrc.com/teensy/card11a_rev4_web.pdf
    According to https://forum.pjrc.com/index.php?threads/t-4-1-really-useable-pins.73044/#post-327502, any grey pin can be used for digital output (like multiplexer controls?).

https://www.modartt.com/pianoteq
    Has Bosendorfer and other sound samples, seems like it can be driven by MIDI input

## Power

The Teensy has a 3.3V output, but a commenter suggested to avoid using it for large current draw (https://forum.pjrc.com/index.php?threads/beginner-schematic-review-hall-effect-keyboard.76407/post-354362). So instead am going to power Teensy with USB, and other components with 5V DC power adapter dropped to 3.3V using a linear regulator.

Hall sensors will likely consume 4.2 mA * 192 keys = 800 mA. Multiplexers supposedly draw less than 1 mA (https://www.lcsc.com/datasheet/lcsc_datasheet_1810010611_Texas-Instruments-CD74HC4067SM96_C98457.pdf search for "quiescent").

Barrel connector: two most common sizes are 5.5mm/2.5mm and 5.5mm/2.1mm.
    https://jlcpcb.com/partdetail/XkbConnection-DC_018_2_5A_25/C2880553
        2.5mm 5.5mm 2.5A

Power adapters:
    https://www.sparkfun.com/power-supply-5v-4a.html
        5V 4A, center-positive 5.5x2.1mm barrel connector
    https://www.amazon.com/SUICINGU-100-240V-Converter-Transformer-5-5x2-5mm/dp/B0B6Z4XBVX?source=ps-sl-shoppingads-lpcontext&ref_=fplfs&psc=1&smid=A3G522G8JZO3PH&gQT=1
        5V 2A, 5.5mm x 2.5mm

Defining ground and power planes on PCB: https://www.youtube.com/watch?v=RyZklHTfkQg
