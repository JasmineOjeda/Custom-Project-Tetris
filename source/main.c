  /*	Author: Jasmine Ojeda jojed016@ucr.edu
 *	Lab Section: 022
 *	Assignment: Custom Lab Project
 *	Exercise Description: Configuring joystick to atmega1284 and LED matrix
 *
 *	I acknowledge all content contained herein, excluding template, example
 *	code, and the outside header/source files to implement the led matrix, is my own original work.
 *
 *	Demo Link: https://youtu.be/bbx0JGYLw6A
 */
#include <stdio.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include "ledmatrix7219d88.h"

#ifdef _SIMULATE_
#include "simAVRHeader.h"
#endif

void ADC_init() {
    ADMUX |= (1 << REFS0);
    ADCSRA |= (1 << ADEN) | (1 << ADPS0) | (ADPS1) | (1 << ADPS2);
}

volatile unsigned char TimerFlag = 0;

unsigned long _avr_timer_M = 1;
unsigned long _avr_timer_cntcurr = 0;

void TimerOn() {
    TCCR1B = 0x0B;

    OCR1A = 125;

    TIMSK1 = 0x02;

    TCNT1 = 0;

    _avr_timer_cntcurr = _avr_timer_M;

    SREG |= 0x80;
}

void TimerOff() {
    TCCR1B = 0x00;
}

void TimerISR() {
    TimerFlag = 1;
}

ISR(TIMER1_COMPA_vect) {
    _avr_timer_cntcurr--;
    if (_avr_timer_cntcurr == 0) {
        TimerISR();
	_avr_timer_cntcurr = _avr_timer_M;
    }
}

void TimerSet(unsigned long M) {
    _avr_timer_M = M;
    _avr_timer_cntcurr = _avr_timer_M;
}

unsigned char up = 0;
unsigned char down = 0;
unsigned char right = 0;
unsigned char left = 0;

uint8_t cur_col = 4;
uint8_t cur_row = 4;

uint8_t rows[8] = {
			0b10000000,
			0b01000000,
			0b00100000,
			0b00010000,
			0b00001000,
			0b00000100,
			0b00000010,
			0b00000001
	};

enum JoystickStates {JOYSTICK_START, JOYSTICK_SET_DIRECTION} joystickState;

void JoystickSM() {
    unsigned short vertical;
    unsigned short horizontal;

    
    ADMUX &= ~_BV(MUX0);;
    ADCSRA |= (1 << ADSC);
    while (ADCSRA & (1 << ADSC));
    vertical = ADC;
    
    ADCSRA &= ~(1 << ADSC);

    ADMUX = _BV(MUX0);;
    ADCSRA |= (1 << ADSC);
    while (ADCSRA & (1 << ADSC));
    horizontal = ADC;

    switch(joystickState) {
        case JOYSTICK_START:
		joystickState = JOYSTICK_SET_DIRECTION;
		break;
	case JOYSTICK_SET_DIRECTION:
		joystickState = JOYSTICK_SET_DIRECTION;
		break;
        default: break;
    }

    switch(joystickState) {
        case JOYSTICK_START:
		up = 0;
		down = 0;
		right = 0;
		left = 0;
		break;
	case JOYSTICK_SET_DIRECTION:
		if(vertical <= 8) {
                   up = 1;
		}
		else {
                   up = 0;
		}

		if(vertical >= 0x3F0) {
                    down = 1;
		}
		else {
                    down = 0;
		}

		if(horizontal <= 8) {
                   left = 1;
                }
                else {
                   left = 0;
                }

                if(horizontal >= 0x3F0) {
                    right = 1;
                }
                else {
                    right = 0;
                }
		break;
        default: break;
    }
}

enum MoveBlock {MOVE_START, MOVE_STILL, MOVE_UP, MOVE_DOWN, MOVE_RIGHT, MOVE_LEFT} moveState;

void MoveSM() {
    switch(moveState) {
	case MOVE_START:
	    moveState = MOVE_STILL;
	    break;
	case MOVE_STILL:
	    if (up) {
	        moveState = MOVE_UP;	    
            }
	    else if (down) {
                moveState = MOVE_DOWN;
	    }
	    else if (right) {
                moveState = MOVE_RIGHT;
            }
	    else if (left) {
                moveState = MOVE_LEFT;
	    }
	    else {
                moveState = MOVE_STILL;
	    }
	    break;
	case MOVE_UP:
	case MOVE_DOWN:
	case MOVE_RIGHT:
	case MOVE_LEFT:
	    moveState = MOVE_STILL;
	    break;
	default: break;
    }

    switch(moveState) {
        case MOVE_START: break;
        case MOVE_STILL: break;
        case MOVE_UP:
	    ledmatrix7219d88_setrow(0, cur_col, 0b00000000);
	    cur_col++;
	    break;
        case MOVE_DOWN:
	    ledmatrix7219d88_setrow(0, cur_col, 0b00000000);
	    cur_col--;
	    break;
        case MOVE_RIGHT:
	    ledmatrix7219d88_setrow(0, cur_col + 1, 0b00000000);
	    ledmatrix7219d88_setrow(0, cur_col + 1, 0b00000000);
	    ledmatrix7219d88_setrow(0, cur_col - 1, 0b00000000);
	    cur_row++;
	    break;
	case MOVE_LEFT:
	    ledmatrix7219d88_setrow(0, cur_col + 1, 0b00000000);
            ledmatrix7219d88_setrow(0, cur_col + 1, 0b00000000);
            ledmatrix7219d88_setrow(0, cur_col - 1, 0b00000000);
	    cur_row--;
	    break;
	default: break;
    }

    ledmatrix7219d88_setrow(0, cur_col, rows[cur_row]);
}

int main(void) {
    /* Insert DDR and PORT initializations */
    DDRD = 0xFF; PORTD = 0x00;

    ADC_init();

    TimerSet(10);
    TimerOn();
    
    ledmatrix7219d88_init();

    joystickState = JOYSTICK_START;
    moveState = MOVE_START;

    unsigned long i = 0;

    /* Insert your solution below */
    while (1) {
        JoystickSM();
	if (i == 250) {
	    MoveSM();
	    i = 0;
	}
	i += 10;

	while (!TimerFlag) { };
	TimerFlag = 0;
    }
    return 1;
}
