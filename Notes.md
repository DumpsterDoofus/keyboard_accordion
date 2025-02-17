## Key spacing

For the Roland accordion:

* Horizontal spacing is 19.06 mm, which is almost the same spacing as the keys on my mechanical keyboard.
* Vertical spacing is about 15.5 mm (possibly harder to measure due to angling), which is less than the 19.06 mm * sin(2*pi/6) = 16.5 mm spacing of a true hexagonal lattice.

For the Orfey accordion:

* Horizontal spacing is 19.81 mm.
* Vertical spacing is 18.25 mm, which is more than the 19.81 mm * sin(2*pi/6) = 17.16 mm spacing of a true hexagonal lattice.

On the mechanical keyboard, there is a little space between the switches, so we could theoretically make the spacing denser than the Roland, but I don't think this would make sense.

Going to try 800 mil = 20.32 mm horizontal spacing and 800 mil * sin(2*pi/6) = 17.5976 mm vertical spacing.

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
    Appears to fit in a 14mm x 14mm mounting plate

https://github.com/sssata/fluxpad/tree/fluxpad-2/ECAD/FluxpadKicad
https://github.com/roxandtol/roxpad
    Uses Lekker switches, might be able to copy?


https://www.reddit.com/r/AskElectronics/comments/1igsc1m/is_a_pullup_resistor_needed_for_an_enable_pin/?
https://www.reddit.com/r/PrintedCircuitBoard/comments/1ifec3p/review_request_teensy_41_hall_effect_minikeyboard/
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
    https://www.aliexpress.us/item/3256806421387301.html?spm=a2g0o.productlist.main.11.7a173d1frfWwjT&algo_pvid=45d6adf5-7cdc-4983-bab7-b52151c5c3ea&algo_exp_id=45d6adf5-7cdc-4983-bab7-b52151c5c3ea-5&pdp_ext_f=%7B%22order%22%3A%227%22%2C%22eval%22%3A%221%22%7D&pdp_npi=4%40dis%21USD%218.73%215.73%21%21%218.73%215.73%21%402103146f17393252582881426ef216%2112000037806734082%21sea%21US%210%21ABX&curPageLogUid=xfbv3krZe5tU&utparam-url=scene%3Asearch%7Cquery_from%3A
        3.3V 3A, 5.5mm x 2.5mm
    https://www.powerstream.com/ac-0330.htm
        3.3V 3A, 5.5mm x 2.1mm

Defining ground and power planes on PCB: https://www.youtube.com/watch?v=RyZklHTfkQg

## Part Numbers

Can search for part numbers at https://www.snapeda.com.

100 nF ceramic capacitor:
GCM155R71C104KA55D
https://jlcpcb.com/partdetail/MurataElectronics-GCM155R71C104KA55D/C85857
https://www.snapeda.com/parts/GCM155R71C104KA55D/Murata/view-part/?ref=rel_components


10uF capacitor:
GRM21BC81A106KE18L
https://jlcpcb.com/partdetail/3515478-GRM21BC81A106KE18L/C3011661
https://www.snapeda.com/parts/GRM21BC81A106KE18L/Murata/view-part/

1000 ohm resistor:
RC0603JR-071KL
https://jlcpcb.com/partdetail/Yageo-RC0603JR071KL/C14676
https://www.snapeda.com/parts/RC0603JR-071KL/Yageo/view-part/

Multiplexer:
CD74HC4067SM
https://jlcpcb.com/partdetail/TexasInstruments-CD74HC4067SM96/C98457
Footprint/symbol in KiCad standard library

Hall sensor:
GH39FKSW
https://jlcpcb.com/partdetail/GoChip_Elec_Tech_Shanghai-GH39FKSW/C266230
Footprint/symbol in KiCad standard library

Barrel connector:
EJ508A
https://jlcpcb.com/partdetail/3567038-EJ508A/C3030986
https://www.snapeda.com/parts/EJ508A/MPD%20(Memory%20Protection%20Devices)/view-part/

Linear regulator:
https://jlcpcb.com/partdetail/Advanced_MonolithicSystems-AMS1117_33/C6186
AMS1117-3.3
Footprint/symbol in KiCad standard library
or (increases max current from 1A to 3A)
https://jlcpcb.com/partdetail/46911-AMS1085CM_33/C45908
AMS1085CM-3.3

Socket (based on https://forum.pjrc.com/index.php?threads/any-tips-suggestions-or-experience-having-solutions-manufactured-w-a-teensy.75913/post-354631)
    https://jlcpcb.com/partdetail/XkbConnection-X6511FV_24C85D32/C2883741
    600 mil separation between the two rows (see "Dimensions" section at https://www.pjrc.com/store/teensy41.html#tech), so use mil units on PCB

Teensy 4.1 with pins soldered
https://www.pjrc.com/store/teensy41_pins.html

## JLCPCB

https://www.youtube.com/watch?v=OQhkx3zeJaE

## Enclosure

Macrolev CAD: https://cad.onshape.com/documents/2af2025c576c4f084cb26625/w/9564bb1d9dfbd4b4af4294c8/e/e2406b8294380372b9116245
Check out Fluxpad CAD directory, it uses Lekker switches

## Key plate

