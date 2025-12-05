# Software

* `firmware`: This is the main software. It runs on the Teensy microcontroller. It makes it act like a USB MIDI device, allowing users to connect it to a PC and use it as a MIDI instrument.
* `cli`: This runs on a PC connected via USB to the MIDI controller. It can be used to change settings or log data from the MIDI controller.
* `sensor_visualizer`: A diagnostic tool used to visualize the raw Hall effect sensor readings collected by the Teensy. This is helpful for investigating noise, magnetic cross-talk, dynamic range, or other sensor problems.
