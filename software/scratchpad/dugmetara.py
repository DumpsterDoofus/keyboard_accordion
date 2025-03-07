'''
This dumps Teensy USB Serial output to a file. I've tested it writing up to 26MB/second. It's more performant than VS Code + PlatformIO Serial Monitor, which seems to be bottlenecked by terminal performance.
'''
import serial
import time
import argparse

def main():
    parser = argparse.ArgumentParser(
        description='The Dugmetara Keyboard MIDI controller CLI interface.',)

    subparsers = parser.add_subparsers()

    log_parser = subparsers.add_parser(
        name='log',
        help="Logs the serial output to a file. This can be used for debugging.")
    log_parser.add_argument(
        '-f',
        '--file_path',
        type=str,
        help='Where the logs will be written.',
        default='serial.log')
    log_parser.set_defaults(func=lambda args: log_serial_output(file_path=args.file_path))

    begin_calibrating_parser = subparsers.add_parser(
        name='begin_calibrating',
        help='Begins sensor calibration mode. Press each key down and up fully, and then either save the results via complete_calibrating, or discard the results with cancel_calibrating. Typically this only needs to be done once.')
    begin_calibrating_parser.set_defaults(func=lambda args: serial_write(data=b'\x00'))

    complete_calibrating_parser = subparsers.add_parser(
        name='complete_calibrating',
        help='Saves sensor calibration results to the SD card.')
    complete_calibrating_parser.set_defaults(func=lambda args: serial_write(data=b'\x01'))

    cancel_calibrating_parser = subparsers.add_parser(
        name='cancel_calibrating',
        help='Discards sensor calibration results and restores the old calibration results.')
    cancel_calibrating_parser.set_defaults(func=lambda args: serial_write(data=b'\x02'))

    enable_velocity_sensing_parser = subparsers.add_parser(
        name='enable_velocity_sensing',
        help='Switches to playing in velocity-sensitive mode (like a piano). Virtually all MIDI instruments support this.')
    enable_velocity_sensing_parser.set_defaults(func=lambda args: serial_write(data=b'\x03'))

    enable_pressure_sensing_parser = subparsers.add_parser(
        name='enable_pressure_sensing',
        help='Switches to playing in polytonic-aftertouch mode (press distance of each key controls its volume). Very few MIDI instruments support this.')
    enable_pressure_sensing_parser.set_defaults(func=lambda args: serial_write(data=b'\x03'))

    args = parser.parse_args()
    if hasattr(args, 'func'):
        args.func(args)
    else:
        parser.print_help()

def log_serial_output(
        file_path: str):

    # Read/write in blocks of at most 1MB.
    block_size = 2**20

    with (
        create_serial_port() as serial_port,
        open(
            file=file_path,
            mode='wb') as log_file
    ):
        print(f'Writing serial output to log file at {log_file.name}')
        while True:
            elapsed_seconds = time.perf_counter()
            bytes_written = log_file.write(serial_port.read(block_size))
            elapsed_seconds = time.perf_counter() - elapsed_seconds

            bytes_per_second = bytes_written/elapsed_seconds
            print(f'Bytes/second: {bytes_per_second}')

def serial_write(data: bytes):
    with create_serial_port() as serial_port:
        serial_port.write(data)

def create_serial_port() -> serial.Serial:
    return serial.Serial(
        # The Teensy connects here (found by looking at PlatformIO Serial Monitor output). 
        port='/dev/ttyACM0',

        # This ensures read(block_size) won't take forever if the Teensy is logging very slowly.
        timeout=1)

if __name__=="__main__":
    main()
