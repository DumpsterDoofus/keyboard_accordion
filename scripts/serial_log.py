"""
This dumps Teensy USB Serial output to a file. I've tested it writing up to 26MB/second. It's more performant than VS Code + PlatformIO Serial Monitor, which seems to be bottlenecked by terminal performance.
"""
import serial
import time

# Read/write in blocks of at most 1MB.
block_size = 2**20

with (
    serial.Serial(
        # The Teensy connects here (found by looking at PlatformIO Serial Monitor output). 
        port='/dev/ttyACM0',

        # This ensures read(block_size) won't take forever if the Teensy is logging very slowly.
        timeout=1) as serial_port,
    open(
        file='software/logs/serial.log',
        mode='wb') as log_file
):
    while True:
        elapsed_seconds = time.perf_counter()
        bytes_written = log_file.write(serial_port.read(block_size))
        elapsed_seconds = time.perf_counter() - elapsed_seconds

        bytes_per_second = bytes_written/elapsed_seconds
        print(f'Bytes/second: {bytes_per_second}')
