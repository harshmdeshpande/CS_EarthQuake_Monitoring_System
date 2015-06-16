/*
 * adc.h
 *
 *  Created on: Jun 28, 2015
 *      Author: harsh
 */

#ifndef ADC_H_
#define ADC_H_

void init_ADC();
uint16_t read_ADC(uint8_t ch);

void init_ADC() {
	ADCSRA = (1<<ADEN) | (1<<ADPS2); // Enable ADC hardware i. e. ADEN=1 and
	// set ADC Prescaler to 16 -->
	// 1Mhz CPU clock/16 = 62.5kHz, i. e. ADPS2=1
	// (ADPS0 and ADPS1 default to 0)
	ADMUX = ADMUX | (0<<REFS1) | (1<<REFS0); // Reference voltage = AVCC = 5V
	ADMUX = ADMUX & ~(1 << ADLAR); // Right adjust the result registers
}

uint16_t read_ADC(uint8_t ch) {
	uint16_t x; // variable to store the ADC result

	// Select channel, e.g. pin 3 (which is the 4th pin at port A)
	ADMUX = ADMUX & 0b11111000; // initialize channel bits (0-2)
	ADMUX = ADMUX | ch; // set channel 3
	PORTA = PORTA & ~(1 << 3);// switch off pull up resistor 3

	// Start a 10 bit AD conversion and store the digital value in a 16 bit variable
	ADCSRA = ADCSRA | (1<<ADSC); // start ADC
	loop_until_bit_is_clear(ADCSRA,ADSC); // wait for completion of ADC

	x = ADCW; // store result in 16bit-variable
	return x;
}

void read_ACCEL_XY(uint16_t *acc) {
	acc[0] = read_ADC(0);
	acc[1] = read_ADC(1);
}

#endif /* ADC_H_ */
