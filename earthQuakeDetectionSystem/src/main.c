/*
 * main.c
 *
 *  Created on: Jun 25, 2015
 *      Author: harsh
 */

#define F_CPU 16000000UL

#include <avr/io.h>

#include "usart.h"
#include "adc.h"


int main(void){
	init_USART();
	init_ADC();

	init_wifi();
	connect_client("", "");

	DDRC = 0xFF;
	PORTC |= 0x01;

	uint16_t acc[2];
	char s[100];
	while(1){
		//PORTC ^= 0x01;
		//char id = wait_for_client();
		//_delay_ms(1000);

		//send_USART_data(id, acc);

		//wait_for_close(id);
		_delay_ms(2000);
		read_ACCEL_XY(acc);
		send_HTTP_request((float)acc[0]);

	}
}

