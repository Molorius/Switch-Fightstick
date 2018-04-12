#ifndef I2C_SLAVE_H
#define I2C_SLAVE_H

#include <avr/interrupt.h>
#include <stdint.h>

#define I2C_PULLUPS CONFIG_I2C_PULLUPS // set to 0 to remove internal pullups
#define I2C_BUFFER_SIZE 100 // anything over 255 will have undefined behavior

void I2C_init(uint8_t address);
void I2C_stop(void);
void I2C_setCallbacks(void (*recv)(char*,uint8_t), void (*req)(void));

inline void __attribute__((always_inline)) I2C_transmitByte(uint8_t data)
{
  TWDR = data;
}

ISR(TWI_vect);

#endif
