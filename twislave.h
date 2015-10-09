#ifndef _TWISLAVE_H
#define _TWISLAVE_H

#include <util/twi.h>
#include <avr/interrupt.h>
#include <stdint.h>

void init_twi_slave(uint8_t adr,void (*write)(uint8_t),uint8_t (*read)());
void twi_write_callback(void (*func)(uint8_t));
void twi_read_callback(uint8_t (*func)());

#endif //#ifdef _TWISLAVE_H
