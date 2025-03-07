This is the first small prototype. It has 2 bugs:

* The EN pin of the multiplexer is connected to the Teensy 3.3V, but the HC4067 uses active low, which means it is always off when the Teensy is powered. As a workaround, I used a razor blade to cut the trace; it seems like it treats floating and low the same. In newer boards, I connect it to ground.
* The DC jack is 2.5mm x 6.3mm, which is a virtually nonexistent size. As a workaround, I connected a 2.1mm x 5.5mm AC adapter to a 2.5mm x 5.5mm barrel jack adapter, whose 5.5mm OD makes enough contact with the spring of the 6.3mm OD jack to get an electrical connection. In newer boards, I use a 2.1mm x 5.5mm DC jack.
