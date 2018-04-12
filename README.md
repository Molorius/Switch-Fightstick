
Switch-I2C-Controller
=====================

This allows an Atmel AVR microcontroller to be a bridge between a Nintendo Switch and an I2C master, such as an Arduino or Odroid. After programming, the AVR will take I2C commands that in turn will control send controller commands via USB to a Switch. This has all of the standard buttons available, but does not support motion.

Uses the LUFA library and reverse-engineering of the Pokken Tournament Pro Pad for the Wii U.

This also uses an I2C slave driver that was modified from [here](https://github.com/thegouger/avr-i2c-slave).

The default I2C address is 0x51, but this can be modified in the [makefile](makefile).

It should go without saying, but make sure the voltages match between master and slave or use level shifters!

Tested on a Pro Micro 3.3V (cheap clone of the Sparkfun Pro Micro).

I2C Commands
============

### Writing

After the address frame, the next byte should have the first register you wish to write to. The following byte will write to that register. Any subsequent bytes sent (until the STOP command) will write to the next register.

For example, the command [02h,00h,FFh] says to start on the HAT register. The next byte, 00h, will be written to the HAT register. The last byte, FFh, will be written to the LX register.

### Reading

We can also read the current controller button presses. First a single byte must be written to the device to tell it which register we want to start with. If we wanted to read the RX register, this byte would be 05h. Then the read command is sent, during which the device will return each register in order.

For example, say we wanted to read every register. We would start by writing 00h, which tells it to start at register BTN_0. We would then read 7 bytes. The device will return the contents of each register, starting from register 00h and ending on register 06h.

I2C Register Map
================

Note that each register is 1 byte (8 bits).

 -------------------------------------------------------------------------------
| Address  |  Name  | Description                               | Initial Value |
|----------|--------|-------------------------------------------|---------------|
|   00h    |  BTN_0 | Buttons such as Minus, Home, and Capture. |      00h      |
|   01h    |  BTN_1 | Buttons such as A, B, and X.              |      00h      |
|   02h    |   HAT  | Directional pad.                          |      08h      |
|   03h    |   LX   | Left joystick x-axis.                     |      80h      |
|   04h    |   LY   | Left joystick y-axis.                     |      80h      |
|   05h    |   RX   | Right joystick x-axis.                    |      80h      |
|   06h    |   RY   | Right joystick y-axis.                    |      80h      |
 -------------------------------------------------------------------------------

### 00h - BTN_0

Presses or releases the button. 0 is release, 1 is press. All buttons are released by default.

 ------------------
| Bit | Button     |
|-----|------------|
| 7:6 | [reserved] |
|  5  | CAPTURE    |
|  4  | HOME       |
|  3  | RCLICK     |
|  2  | LCLICK     |
|  1  | PLUS       |
|  0  | MINUS      |
 ------------------

### 01h - BTN_1

Presses or releases the button. 0 is release, 1 is press. All buttons are released by default.

 --------------
| Bit | Button |
|-----|--------|
|  7  | ZR     |
|  6  | ZL     |
|  5  | R      |
|  4  | L      |
|  3  | X      |
|  2  | A      |
|  1  | B      |
|  0  | Y      |
 --------------

### 02h - HAT

Control the Directional Pad (d-pad). This is in neutral position by default.

 --------------------
| Value | Direction  |
|-------|------------|
|  00h  | Up         |
|  01h  | Up-Right   |
|  02h  | Right      |
|  03h  | Down-Right |
|  04h  | Down       |
|  05h  | Down-Left  |
|  06h  | Left       |
|  07h  | Up-Left    |
|  08h  | Neutral    |
 --------------------

### 02h:026

Registers 03h through 06h control the joysticks. 00h corresponds to completely left/down, 80h is neutral, and FFh is completely right/up. They are all set to neutral by default.

Flashing
========

Before compiling, edit the [makefile](makefile) for the desired microcontroller, clock speed, and I2C address. There is plenty of documentation out there on how to download gcc-avr and avr-libc for various operating systems.

If you have trouble compiling, the [build](build/) directory has several pre-built hex files for several common boards such as the Sparkfun Pro Micro and the Teensy 2.0. Note that these all have internal I2C pullup resistors enabled.

Teensy users can use [Teensy Loader](https://github.com/bertrandom/snowball-thrower/pull/1) to upload the final hex file.

Arduino users can copy and paste the upload command from the Arduino IDE (turn on verbose upload) to upload the final hex file. Just edit the command to point to the hex file location.

Additionally, I have included my [quick flasher program](flash.py) that I use. This is just for my own ease, it is not necessary. Mileage my vary.
