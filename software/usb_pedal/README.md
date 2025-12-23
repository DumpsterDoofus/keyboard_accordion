# USB sustain pedal

This program converts a [cheap $20 USB foot pedal](https://www.amazon.com/dp/B08X4H4FW9?ref=ppx_yo2ov_dt_b_fed_asin_title) into a MIDI sustain pedal.

I made this because USB MIDI sustain pedals are oddly expensive. For example, a [regular pedal-to-USB converter](https://www.tecontrol.se/products/usb-midi-pedal-controller) costs 70 Euros, and it doesn't even include the pedal, which has to be bought separately (typically $20-30).

## How to use

On Linux, need to enable permissions to the foot pedal:

```sh
sudo cp 99-footpedal.rules /etc/udev/rules.d/
```

Then run the application:

```sh
RUST_LOG=debug cargo run --release
```

When the pedal is pressed/released, it should print messages like `Pedal pressed` and `Pedal released`. MIDI applications like PianoTeq should show the sustain pedal being pressed/released.
