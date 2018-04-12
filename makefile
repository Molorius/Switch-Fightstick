#
#             LUFA Library
#     Copyright (C) Dean Camera, 2014.
#
#  dean [at] fourwalledcubicle [dot] com
#           www.lufa-lib.org
#
# --------------------------------------
#         LUFA Project Makefile.
# --------------------------------------

# Run "make help" for target help.

# Set the MCU and F_CPU according to your device
# at90usb1286 for Teensy 2.0++
# atmega32u4 for Teensy 2.0, Pro Micro
# F_CPU = 16000000 for most devices, 8000000 with a different clock (such as on 3.3V Pro Micro)
# I2C_ADDRESS is the address your device will be.
MCU          = atmega32u4
ARCH         = AVR8
F_CPU        = 8000000
F_USB        = $(F_CPU)
I2C_ADDRESS  = 0x51
I2C_PULLUPS  = 1 # set to 1 to enable internal pullup resistors
OPTIMIZATION = s
TARGET       = Switch-I2C
SRC          = Joystick.c I2CSlave.c Descriptors.c $(LUFA_SRC_USB)
LUFA_PATH    = ./lufa/LUFA
CC_FLAGS     = -DUSE_LUFA_CONFIG_HEADER -IConfig/ -DCONFIG_I2C_ADDRESS=$(I2C_ADDRESS) -DCONFIG_I2C_PULLUPS=$(I2C_PULLUPS)
LD_FLAGS     =

# Default target
all:

# Include LUFA build script makefiles
include $(LUFA_PATH)/Build/lufa_core.mk
include $(LUFA_PATH)/Build/lufa_sources.mk
include $(LUFA_PATH)/Build/lufa_build.mk
include $(LUFA_PATH)/Build/lufa_cppcheck.mk
include $(LUFA_PATH)/Build/lufa_doxygen.mk
include $(LUFA_PATH)/Build/lufa_dfu.mk
include $(LUFA_PATH)/Build/lufa_hid.mk
include $(LUFA_PATH)/Build/lufa_avrdude.mk
include $(LUFA_PATH)/Build/lufa_atprogram.mk
