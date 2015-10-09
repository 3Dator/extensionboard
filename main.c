#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <math.h>
#include "usart.c"
#include "twislave.c"

enum registerState{
	START,
	FAN_A=0x00,
	FAN_B=0x01,
	LED=0x02,
	ANIMATION=0x03
};
typedef enum registerState registerState_;
registerState_ state = START;


uint8_t read(){
	return 0x43;
}

void write(uint8_t value){
	if(state==START){
		state=value;
	}else{
		
	}
}

void fanA(*uint8_t arg){

}

void fanB(*uint8_t arg){
	
}

void led(*uint8_t arg){

}

void


int main(){
	usart_init(9600);
	_delay_ms(100);
	usart_write_str("LED and Fan Control Board (C) 3Dator GmbH 2015\n");
	
	init_twi_slave(0x12,&write,&read);
	
	while(1){
		_delay_ms(1000);
		usart_write_str("TEST2\n");
	}
	
}
