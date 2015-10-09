#include <avr/io.h>
uint8_t volatile leds[162];
uint16_t ledcount=162;

static void updateWS2812(uint8_t *leds, uint16_t ledlength){
	uint8_t sreg = SREG;
	cli();
	asm volatile(
		//r17 stores the bitcount: 8 gots decremented each bit transfer, until it is 0
		//r16 stores the current transfer byte
			"RJMP Main_Loop" 		"\n\r" //start with the Main_Loop
		"Txd_bit_low:" 				"\n\t" //Send Low (Pin must be allready 1)
			"NOP"					"\n\t"
			"NOP"					"\n\t"
			"NOP"					"\n\t"
			"CBI %[PORT], 4"	"\n\t"
			"NOP"					"\n\t"
			"NOP"					"\n\t"
			"NOP"					"\n\t"
			"NOP"					"\n\t"
			"NOP"					"\n\t"
			"NOP"					"\n\t"
			"RJMP Bit_Loop_after" 	"\n\t"
		"Txd_bit_high:"				"\n\t" //Send high (Pin must be allready 1)
			"NOP"					"\n\t"
			"NOP"					"\n\t"
			"NOP"					"\n\t"
			"NOP"					"\n\t"
			"NOP"					"\n\t"
			"NOP"					"\n\t"
			"NOP"					"\n\t"
			"CBI %[PORT], 4"	"\n\t"
			"NOP"					"\n\t"
			"NOP"					"\n\t"
			"NOP"					"\n\t"
		"Bit_Loop_after:"			"\n\t"
			"DEC r17"				"\n\t"	 // Decrement the count of the current bit
			"BREQ Main_After"		"\n\t" //And if it is null, break the inner loop
		"Bit_Loop:"					"\n\t"
			"SBI %[PORT], 4"	"\n\t" //set the pin to high
			"LSL r16" 				"\n\t" //move the most significant bit in to carry
			"BRCC Txd_bit_low"		"\n\t" //check if carry is 0, then send low
			"RJMP Txd_bit_high"		"\n\t" //otherwise send high
		"Main_Loop:"				"\n\t" //Main loop, loads the bytes 
			"LD	r16, %a[LEDS]+"		"\n\t" //Load the current LED byte from the SRAM, and increment the pointer
			"LDI r17,8"				"\n\t" //Load 8 in the bitcount register
			"RJMP Bit_Loop" 		"\n\t" //start the Bit_Loop
		"Main_After:" 				"\n\t" 
			"SBIW %[LEDCOUNT],1" 	"\n\t" //After return decrement the byte count
			"BRNE Main_Loop"		"\n\t" //and go on, if it is not null
		:
		: [LEDS] "e" (leds),[LEDCOUNT] "w" (ledlength),[PORT] "I"  (_SFR_IO_ADDR(PORTD)),[PIN] "I" (7)
		: "r16", "r17");
	SREG = sreg;
}

void setColor(uint8_t r, uint8_t g, uint8_t b){
	for(int i=0;i<ledcount;i+=3){
		leds[i]=r;
		leds[i+1]=g;
		leds[i+2]=b;
	}
	updateWS2812(leds,ledcount);
}

void setAnimation(uint8_t animation, uint8_t option){
	
}
