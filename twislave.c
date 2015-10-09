#include <util/twi.h> 	    //enthält z.B. die Bezeichnungen für die Statuscodes in TWSR
#include <avr/interrupt.h>  //dient zur Behandlung der Interrupts
#include <stdint.h>         //definiert den Datentyp uint8_t       
#include "twislave.h"

void (*write_callback) (uint8_t);
uint8_t (*read_callback) ();

/*
	register callback
*/
void init_twi_slave(uint8_t adr,void (*write)(uint8_t),uint8_t (*read)()){
    TWAR= adr; //Adresse setzen
	TWCR &= ~(1<<TWSTA)|(1<<TWSTO);
	TWCR|= (1<<TWEA) | (1<<TWEN)|(1<<TWIE); 	
	
	write_callback = write;
	read_callback = read;
	
	sei();
}

void twi_write_callback(void (*func)(uint8_t)){
	write_callback =func;
}

void twi_read_callback(uint8_t (*func)()){
	read_callback =func;
}


//Je nach Statuscode in TWSR müssen verschiedene Bitmuster in TWCR geschreiben werden(siehe Tabellen im Datenblatt!). 
//Makros für die verwendeten Bitmuster:

//ACK nach empfangenen Daten senden/ ACK nach gesendeten Daten erwarten
#define TWCR_ACK TWCR = (1<<TWEN)|(1<<TWIE)|(1<<TWINT)|(1<<TWEA)|(0<<TWSTA)|(0<<TWSTO)|(0<<TWWC);  

//NACK nach empfangenen Daten senden/ NACK nach gesendeten Daten erwarten     
#define TWCR_NACK TWCR = (1<<TWEN)|(1<<TWIE)|(1<<TWINT)|(0<<TWEA)|(0<<TWSTA)|(0<<TWSTO)|(0<<TWWC);

//switch to the non adressed slave mode...
#define TWCR_RESET TWCR = (1<<TWEN)|(1<<TWIE)|(1<<TWINT)|(1<<TWEA)|(0<<TWSTA)|(1<<TWSTO)|(0<<TWWC);  


/*ISR, die bei einem Ereignis auf dem Bus ausgelöst wird. Im Register TWSR befindet sich dann 
ein Statuscode, anhand dessen die Situation festgestellt werden kann.
*/
ISR (TWI_vect){

	switch (TW_STATUS){
		// Slave Receiver 

		case TW_SR_SLA_ACK: // 0x60 Slave Receiver, Slave wurde adressiert	
			TWCR_ACK; // nächstes Datenbyte empfangen, ACK danach senden
		break;
	
		case TW_SR_DATA_ACK: // 0x80 Slave Receiver, ein Datenbyte wurde empfangen
			write_callback(TWDR); //Empfangene Daten auslesen
			TWCR_ACK;
		break;


		//Slave transmitter

		case TW_ST_SLA_ACK: //0xA8 Slave wurde im Lesemodus adressiert und hat ein ACK zurückgegeben.
			//Hier steht kein break! Es wird also der folgende Code ebenfalls ausgeführt!
	
		case TW_ST_DATA_ACK: //0xB8 Slave Transmitter, Daten wurden angefordert
			//read_callback()
			TWDR = read_callback();
			TWCR_ACK;
		break;
		case TW_SR_STOP:
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
