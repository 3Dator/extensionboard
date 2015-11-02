#include <avr/io.h>
#include <avr/interrupt.h>
#include "fancontroller.h"
uint8_t speed;

void setSpeed(uint8_t value){
	speed=value;
}

void fanInit(){
	EIMSK |= 1<<INT0 || 1 << INT1;
	EICRA |= (1<< ISC11);
	EICRA |= (1<< ISC01);
	DDRD=0x00;
	sei();
	
	DDRD|= (1<< PD5);
	
	//PORTD|= (1<< PD5);
	
	TCCR0A |= (1<< WGM01);
 	TCCR0B |= (1<< CS11);
    TIMSK0 |= (1<<OCIE0A);  
    
    speed=1;
    OCR0A = 127;
}



ISR( INT0_vect ){
	
}

uint8_t pwmCounter=0;
ISR( TIMER0_COMPA_vect ) {
	
	pwmCounter++;
	
	if(speed > pwmCounter){
		PORTD|= (1 << PD5);
	}else{
		PORTD &= ~(1<< PD5);
	}
}
