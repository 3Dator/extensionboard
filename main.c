#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <math.h>
#include "usart.c"
#include "twislave.c"
#include "fancontroller.c"

#define miniDator 0 
#define anlogLED 1

#include "ledcontroller.c"



void fanA(uint8_t* arg){
	setSpeed(arg[0]);

}

void fanB(uint8_t* arg){
}

void led(uint8_t* arg){
	#ifdef anlogLED
		setColorAnalog(arg[0],arg[1],arg[2]);
	#else
		setColor(arg[0],arg[1],arg[2]);
	#endif
}

void animation(uint8_t* arg){
	setAnimation(arg[0],arg[1]);
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
	
	fanInit();
	ledInit();
	uint8_t i=0;
	while(1){
		/*
		setColorAnalog(i,i,i);
		i++;
		setSpeed(255);
		_delay_ms(20);*/
		/*PORTB = 0;
		PORTB = (1<< PB1);
		_delay_ms(1000);
		PORTB = 0;
		PORTB = (1<< PB2);
		_delay_ms(1000);
		PORTB = 0;
		PORTB = (1<< PB3);
		
		_delay_ms(1000);*/
	}
	
}

