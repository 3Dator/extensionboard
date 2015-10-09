#include <util/twi.h> 	    //enthält z.B. die Bezeichnungen für die Statuscodes in TWSR
#include <avr/interrupt.h>  //dient zur Behandlung der Interrupts
#include <stdint.h>         //definiert den Datentyp uint8_t       
#include "twislave.h"

/*
	register callback
*/
void init_twi_slave(uint8_t adr){
    TWAR= adr; //addresse setzen
	TWCR &= ~(1<<TWSTA)|(1<<TWSTO);
	TWCR|= (1<<TWEA) | (1<<TWEN)|(1<<TWIE); 	
	
	sei();
}

//void (*write_callback) (uint8_t);
//uint8_t (*read_callback) ();

//define here the register and the subregister count
#define REGISTERS 4
#define SUBREGISTERS 3
//There is no MMU, so use fixed array length
uint8_t addresses[REGISTERS];
void (*functions[REGISTERS])(uint8_t*);
uint8_t subRegisterLength[REGISTERS];
uint8_t subRegister[SUBREGISTERS];
uint8_t registerCount=0;

void twi_register_callback(uint8_t address,void (*function)(uint8_t*), uint8_t length){
	
	addresses[registerCount]=address;
	functions[registerCount]=function;
	subRegisterLength[registerCount]=length;
	
	registerCount++;
}

void writeToSubRegister(uint8_t address, uint8_t value, uint8_t subAddress){
	if(subAddress < SUBREGISTERS){
		for(int i=0;i<registerCount;i++){
			if(addresses[i]==address){
				subRegister[subAddress]=value;
			}		
		}
	}else{
		usart_write_P("ERROR: two many arguments for command %x \n",address);
	}
}

void executeCallback(uint8_t address){
	for(int i=0;i<registerCount;i++){
		if(addresses[i]==address){
			functions[i](subRegister);
		}
	}
}


//Je nach Statuscode in TWSR müssen verschiedene Bitmuster in TWCR geschreiben werden(siehe Tabellen im Datenblatt!). 
//Makros für die verwendeten Bitmuster:

//ACK nach empfangenen Daten senden/ ACK nach gesendeten Daten erwarten
#define TWCR_ACK TWCR = (1<<TWEN)|(1<<TWIE)|(1<<TWINT)|(1<<TWEA)|(0<<TWSTA)|(0<<TWSTO)|(0<<TWWC);  

//NACK nach empfangenen Daten senden/ NACK nach gesendeten Daten erwarten     
#define TWCR_NACK TWCR = (1<<TWEN)|(1<<TWIE)|(1<<TWINT)|(0<<TWEA)|(0<<TWSTA)|(0<<TWSTO)|(0<<TWWC);

//switch to the non addressed slave mode...
#define TWCR_RESET TWCR = (1<<TWEN)|(1<<TWIE)|(1<<TWINT)|(1<<TWEA)|(0<<TWSTA)|(1<<TWSTO)|(0<<TWWC);  


uint8_t state=0;//0: wait for main register, else: the next byte 
uint8_t currentRegister=0x00;

/*ISR, die bei einem Ereignis auf dem Bus ausgelöst wird. Im Register TWSR befindet sich dann 
ein Statuscode, anhand dessen die Situation festgestellt werden kann.
*/
ISR (TWI_vect){

	switch (TW_STATUS){
		// Slave Receiver 

		case TW_SR_SLA_ACK: // 0x60 Slave Receiver, Slave wurde addressiert	
			TWCR_ACK; // nächstes Datenbyte empfangen, ACK danach senden
		break;
	
		case TW_SR_DATA_ACK: // 0x80 Slave Receiver, ein Datenbyte wurde empfangen
			if(!state){
				currentRegister=TWDR;
				state=1;
			}else{
				writeToSubRegister(currentRegister,TWDR,state-1);
				state++;
			}
			TWCR_ACK;
		break;


		//Slave transmitter

		case TW_ST_SLA_ACK: //0xA8 Slave wurde im Lesemodus addressiert und hat ein ACK zurückgegeben.
			//Hier steht kein break! Es wird also der folgende Code ebenfalls ausgeführt!
	
		case TW_ST_DATA_ACK: //0xB8 Slave Transmitter, Daten wurden angefordert
			//read_callback()
//			TWDR = read_callback();
			TWCR_ACK;
		break;
		case TW_SR_STOP:
			executeCallback(currentRegister);
			state=0;
			TWCR_ACK;
		break;
		case TW_ST_DATA_NACK: // 0xC0 Keine Daten mehr gefordert 
		case TW_SR_DATA_NACK: // 0x88 
		case TW_ST_LAST_DATA: // 0xC8  Last data byte in TWDR has been transmitted (TWEA = “0”); ACK has been received
		default: 	
			
			TWCR_RESET;
		break;
	
	}
}

////Ende von twislave.c////
