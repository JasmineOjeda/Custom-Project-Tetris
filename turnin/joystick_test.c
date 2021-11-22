  /*	Author: Jasmine Ojeda jojed016@ucr.edu
 *	Lab Section: 022
 *	Assignment: Custom Lab Project
 *	Exercise Description: [optional - include for your own benefit]
 *
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 *
 *	Demo Link:
 */
#include <avr/io.h>
#ifdef _SIMULATE_
#include "simAVRHeader.h"
#endif

void ADC_init() {
    ADCSRA |= (1 << ADEN) | (1 << ADSC) | (1 << ADATE);
}

int main(void) {
    /* Insert DDR and PORT initializations */
    DDRB = 0xFF; PORTB = 0x00;
    DDRD = 0xFF; PORTD = 0x00;

    DDRC = 0xFC; PORTC = 0x03;

    unsigned char tmp_B = 0x00;
    unsigned char tmp_D = 0x00;    
    unsigned short x;
    unsigned char tmp_C = 0x00;
    //unsigned char s = 0x00;
    ADMUX = 0x00;

    ADC_init();
    /* Insert your solution below */
    while (1) {
        tmp_C = (~PINC) & 0x03;

	if (tmp_C == 0x01) {
            ADMUX = 0x00;
	}
	else if (tmp_C == 0x02) {
            ADMUX = 0x01;
	}
        /*
	if (ADMUX == 0x01) {
           s = 0x04;
	}
	else {
           s = 0x00;
	}

	PORTC = PORTC | s;*/
        x = ADC;

        tmp_B = (char)x;
	tmp_D = (char)((x & 0x0300) >> 8);

	PORTB = tmp_B;
	PORTD = tmp_D;
    }
    return 1;
}
