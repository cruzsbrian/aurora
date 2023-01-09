import argparse
import serial

parser = argparse.ArgumentParser(description='Upload a pattern to Aurora.')
parser.add_argument('port', help='Serial port to use')
parser.add_argument('filename', help='Pattern file to upload')
parser.add_argument('--save', action='store_true', help='Save the pattern to the SD card')

args = parser.parse_args()

with serial.Serial(args.port, 115200) as ser:
    # Read the file and send it over serial.
    with open(args.filename) as f:
        data = f.read()
        if args.save:
            ser.write(b'SAVE\n' + data.encode('ascii') + b'\0')
        else:
            ser.write(b'LOAD\n' + data.encode('ascii') + b'\0')

    # Loop, printing anything received from the device.
    # Stop after we see an FPS message 5 times in a row.
    fps_count = 0
    while True:
        line = ser.readline().decode('ascii')
        if line.startswith('FPS:'):
            fps_count += 1
        else:
            fps_count = 0

        print(line, end='')

        if fps_count >= 5:
            break