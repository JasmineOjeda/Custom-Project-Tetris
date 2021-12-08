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
#define F_CPU 1000000UL

#include <stdio.h>
#include <avr/io.h>
#include <avr/interrupt.h>
//#include <util/delay.h>
#include "ledmatrix7219d88.h"
#include "nokia5110.h"
#include "blocks.h"
/*
#ifdef _SIMULATE_
#include "simAVRHeader.h"
#endif
*/

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

unsigned char right = 0;
unsigned char left = 0;
unsigned char sound_detected = 1;

enum JoystickStates {JOYSTICK_START, JOYSTICK_SET_DIRECTION} joystickState;

void JoystickSM() {
    unsigned short horizontal;

    ADMUX = 0x01;
    ADCSRA |= (1 << ADSC);
    while (ADCSRA & (1 << ADSC));
    horizontal = ADC;

    ADCSRA &= ~(1 << ADSC);

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
		right = 0;
		left = 0;
		break;
	case JOYSTICK_SET_DIRECTION:
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

enum MoveBlock {MOVE_START, MOVE_STILL, MOVE_MOVE} moveState;

void MoveSM() {
    switch(moveState) {
	case MOVE_START:
	    moveState = MOVE_STILL;
	    break;
	case MOVE_STILL:
	    if (right || left) {
                moveState = MOVE_MOVE;
            }
	    else {
                moveState = MOVE_STILL;
	    }
	    break;
	case MOVE_MOVE:
	    moveState = MOVE_STILL;
	    break;
	default: break;
    }

    switch(moveState) {
        case MOVE_START: break;
        case MOVE_STILL: break;
	case MOVE_MOVE:
	    moveBlock1(0, left, right);
	    break;
	default: break;
    }

}

enum DownStates {DOWN_START, DOWN_LOOP} downState;

void DownSM() {
    switch(downState) {
        case DOWN_START:
            downState = DOWN_LOOP;
	    break;
	case DOWN_LOOP:
	    downState = DOWN_LOOP;
	    break;
	default:
	   break;
    }

    switch(downState) {
        case DOWN_START: 
		block1.cur_col1 = 0;
                block1.cur_col2 = 0;
                block1.cur_row1 = 1;
                block1.cur_row2 = 2;
		break;
	case DOWN_LOOP: 
	        moveBlock1(1, 0, 0);
		break;
	default: break;
    }
}

enum SoundStates {SOUND_START, SOUND_LOOP} soundState;

void SoundSM() {
    unsigned char sound;

    switch(soundState) {
        case SOUND_START:
	    soundState = SOUND_LOOP;
	    break;
	case SOUND_LOOP:
	    soundState = SOUND_LOOP;
	    break;
	default: break;
    }

    switch(soundState) {
        case SOUND_START:
	    sound = 0;
	    sound_detected = 0;
	    break;
        case SOUND_LOOP:
            ADMUX = 0x02;
            ADCSRA |= (1 << ADSC);
            while (ADCSRA & (1 << ADSC));
            sound = ADC;

            ADCSRA &= ~(1 << ADSC);

	    if (sound >= 15) {
                sound_detected = (~sound_detected) & 0x01;
	    }
	    break;
	default: break;
    }
}

int main(void) {
    /* Insert DDR and PORT initializations */
    DDRD = 0xFF; PORTD = 0x00;

    ADC_init();
    nokia_lcd_init();

    ledmatrix7219d88_init();

    nokia_lcd_clear();
    nokia_lcd_display();

    joystickState = JOYSTICK_START;
    moveState = MOVE_START;
    soundState = SOUND_START;
    downState = DOWN_START;

    unsigned long move_i = 0;
    unsigned long sound_i = 0;
    
    TimerSet(10);
    TimerOn();
    /* Insert your solution below */
    while (1) {
        JoystickSM();

	if (move_i == 250) {
	    if (sound_detected) {
	        MoveSM();
	        DownSM();
	    }
	    move_i = 0;
	}

	if (sound_i == 70) {
            SoundSM();
	    sound_i = 0;
	}

	move_i += 10;
	sound_i += 10;

	while (!TimerFlag) { };
	TimerFlag = 0;
    }
    return 1;
}
