#include <util/twi.h>
#include <avr/interrupt.h>
//#include <util/delay.h>
#include "I2CSlave.h"

#if I2C_PULLUPS
#define SDA_SCL_INIT PORTD |= (1<<1) | (1<<0) // 2->pd1, 3->pd0
#endif

static void (*I2C_recv)(char*,uint8_t);
static void (*I2C_req)(void);

static volatile char buffer[I2C_BUFFER_SIZE];
static volatile uint8_t buf_pos = 0;

void I2C_setCallbacks(void (*recv)(char*,uint8_t), void (*req)(void))
{
  I2C_recv = recv;
  I2C_req = req;
}

void I2C_init(uint8_t address)
{
  cli();

  // put pullups in place, if set
  #if I2C_PULLUPS
  SDA_SCL_INIT;
  #endif

  // load address into TWI address register
  TWAR = address << 1;

  // set the TWCR to enable address matching and enable TWI, clear TWINT, enable TWI interrupt
  TWCR = (1<<TWIE) | (1<<TWEA) | (1<<TWINT) | (1<<TWEN);
  //TWCR = (1<<TWIE) | (1<<TWEA) | (1<<TWEN);
  sei();
}

void I2C_stop(void)
{
  // clear acknowledge and enable bits
  cli();
  TWCR = 0;
  TWAR = 0;
  sei();
}

ISR(TWI_vect)
{
  switch(TW_STATUS)
  {
    case TW_SR_DATA_ACK:
      // received data from master, save it to buffer
      buffer[buf_pos] = TWDR;
      if(buf_pos < I2C_BUFFER_SIZE) {
        buf_pos++;
      }
      TWCR = (1<<TWIE) | (1<<TWINT) | (1<<TWEA) | (1<<TWEN);
      break;
    case TW_ST_SLA_ACK:
      // master is requesting data, call the request callback
      I2C_req();
      TWCR = (1<<TWIE) | (1<<TWINT) | (1<<TWEA) | (1<<TWEN);
      break;
    case TW_ST_DATA_ACK:
      // master is requesting data, call the request callback
      I2C_req();
      TWCR = (1<<TWIE) | (1<<TWINT) | (1<<TWEA) | (1<<TWEN);
      break;
    case TW_SR_STOP:
      // master sent STOP command, call the receive callback
      I2C_recv((char*)buffer, buf_pos);
      buf_pos = 0;
      TWCR = (1<<TWIE) | (1<<TWINT) | (1<<TWEA) | (1<<TWEN);
      break;
    case TW_BUS_ERROR:
      // some sort of erroneous state, prepare TWI to be readdressed
      TWCR = 0;
      TWCR = (1<<TWIE) | (1<<TWINT) | (1<<TWEA) | (1<<TWEN);
      break;
    default:
      TWCR = (1<<TWIE) | (1<<TWINT) | (1<<TWEA) | (1<<TWEN);
      break;
  }
}
