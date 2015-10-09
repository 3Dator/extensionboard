#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <math.h>
#include "usart.c"
#include "i2cmaster.c"

int main(){
	usart_init(9600);
	i2c_init();
	
	usart_write_str("I2C Master Test\n");
	char buffer[3]={'a','b',0};
	while(1){
		
		i2c_start_wait(0x12);
		i2c_write(0x00);
		i2c_rep_start(0x13);
		itoa(i2c_readNak(),buffer,16);
		i2c_stop();
		usart_write_str("new Byte:");
		usart_write_str(buffer);
		usart_write_str("\n");
		_delay_ms(1000);
	}
	
}
