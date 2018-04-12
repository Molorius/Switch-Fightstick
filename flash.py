#!/usr/bin/env python3

'''
This is my own personal flashing program, and is kept in case
somebody finds it useful. Mileage may vary. This is used
to upload to a ProMicro board with the Sparkfun Catalina bootloader.
'''

MCU = 'atmega32u4'
PORT = '/dev/ttyACM0'
BAUD = "57600"
AVRDUDE = "avrdude"
PROGRAMMER = "avr109"

from serial import Serial
from subprocess import call
from time import time, sleep
import sys
from pathlib import Path

def reset(port):
    try:
        com = Serial(port, 1200)
        com.dtr = False
        com.close()
        return True
    except:
        return False

def flash(f,timeout):
    fin = "flash:w:"
    tic = time()
    while (time()-tic) < timeout:
        output = call([AVRDUDE, "-v", "-p", MCU, "-c", PROGRAMMER, "-P", PORT, "-b", BAUD, "-U", "flash:w:"+f])
        if output == 0:
            print("successfully flashed")
            return True
        print("Could not flash, try resetting board")
        sleep(1)
    print("Could not connect")
    return False

def exists(f):
    return Path(f).is_file()

def main():
    n = len(sys.argv)

    if n != 2:
        print("Invalid arguments, it must be: %s \"file\"" % sys.argv[0])
        return False

    if not exists(sys.argv[1]):
        print("Not a valid file: %s" % sys.argv[1])
        return False

    print("reseting board")
    reset(PORT)
    print("flashing")
    flash(sys.argv[1], 10)
    return True

if __name__ == "__main__":
    main()
