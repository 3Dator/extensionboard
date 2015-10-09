#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <math.h>
#include "usart.c"
#include "twislave.c"

void fanA(uint8_t* arg){
	usart_write_str("command: fanA ");
	usart_write_str((char*)arg);
	usart_write_str("\n");
}

void fanB(uint8_t* arg){
	usart_write_str("command: fanB ");
	usart_write_str((char*)arg);
	usart_write_str("\n");
}

void led(uint8_t* arg){
	usart_write_str("command: led ");
	usart_write_str((char*)arg);
	usart_write_str("\n");
}

void animation(uint8_t* arg){
	usart_write_str("command: animation ");
	usart_write_str((char*)arg);
	usart_write_str("\n");
}


int main(){
	usart_init(9600);
	_delay_ms(100);
	usart_write_str("LED and Fan Control Board (C) 3Dator GmbH 2015\n");
	usart_write_str("Firmware version 0.1\n");
	usart_write_str("Compiled at "__DATE__" "__TIME__"\n");
	
	init_twi_slave(0x12);
	
	twi_register_callback(0x10,&led,3);
	twi_register_callback(0x11,&animation,2);
	
	while(1){
	}
	
}
