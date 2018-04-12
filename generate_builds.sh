#!/usr/bin/env bash

# This is my script to generate builds for common avr boards used in the community.
# Yes, most of the builds are identical. It is to avoid confusion/complaints.

DIR=builds # directory to save this all in
ADDR=0x51 # i2c address
PREFIX=Switch-I2C
FLAGS=-j4 # make flags
PULLUPS=1 # 1 for enabled

clean() {
  rm -f -r $DIR # delete directory and all contents
  mkdir -p $DIR # generate it back
}

final_clean() {
  rm -f -r obj/
}

generate() {
  # $1 = TARGET
  # $2 = MCU
  # $3 = F_CPU
  # $4 = I2C_ADDRESS
  # $5 = I2C_PULLUPS (1 for enabled, 0 for not)
  make $FLAGS TARGET=$1 MCU=$2 F_CPU=$3 I2C_ADDRESS=$4 I2C_PULLUPS=$5 # generate file
  mv $1.hex $DIR # move completed hex file to build directory
  make clean $FLAGS TARGET=$1 MCU=$2 F_CPU=$3 I2C_ADDRESS=$4 I2C_PULLUPS=$5 # clean up the build
}

main() {
  clean
  generate ${PREFIX}_Teensy++2.0_$ADDR at90usb1286 16000000 $ADDR $PULLUPS
  generate ${PREFIX}_Teensy2.0_$ADDR   atmega32u4  16000000 $ADDR $PULLUPS
  generate ${PREFIX}_ProMicro5_$ADDR   atmega32u4  16000000 $ADDR $PULLUPS
  generate ${PREFIX}_ProMicro3.3_$ADDR atmega32u4  8000000  $ADDR $PULLUPS
  generate ${PREFIX}_Leonardo_$ADDR    atmega32u4  16000000 $ADDR $PULLUPS
  final_clean
}

main
