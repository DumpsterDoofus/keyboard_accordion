# USB sustain pedal

This program converts a [cheap $20 USB foot pedal](https://www.amazon.com/dp/B08X4H4FW9?ref=ppx_yo2ov_dt_b_fed_asin_title) into a MIDI sustain pedal.

I made this because USB MIDI sustain pedals are oddly expensive. For example, a [regular pedal-to-USB converter](https://www.tecontrol.se/products/usb-midi-pedal-controller) costs 70 Euros, and it doesn't even include the pedal, which has to be bought separately (typically $20-30).

## Prerequisites

### Linux

Need to enable permissions to the foot pedal:

```sh
sudo cp 99-footpedal.rules /etc/udev/rules.d/
```

### Mac

In Audio MIDI Setup, double-click IAC Driver and add an entry with a unique name like "USB Pedal". When launching the app, when prompted to choose an output port, select the newly-created driver. Also may need to run with `sudo` (TODO can we avoid this?).

### Windows

TODO: test this?

## How to run

To run the application:

```sh
RUST_LOG=debug cargo run --release
```

When the pedal is pressed/released, it should print messages like `Pedal pressed` and `Pedal released`. MIDI applications like PianoTeq should show the sustain pedal being pressed/released.
