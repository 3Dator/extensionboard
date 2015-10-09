#ifndef _TWISLAVE_H
#define _TWISLAVE_H

#include <util/twi.h>
#include <avr/interrupt.h>
#include <stdint.h>

void init_twi_slave(uint8_t adr);
void twi_register_callback(uint8_t address,void (*function)(uint8_t*), uint8_t length);
#endif //#ifdef _TWISLAVE_H
