  
/*	Author: josiahlee
 *  Partner(s) Name: Shiyou Wang
 *	Lab Section:
 *	Assignment: Lab #  Exercise #
 *	Exercise Description: [optional - include for your own benefit]
 *
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 */
#include <avr/io.h>
#include <avr/interrupt.h>
#ifdef _SIMULATE_
#include "simAVRHeader.h"
#endif

volatile unsigned char TimerFlag = 0;
unsigned long _avr_timer_M = 1;
unsigned long _avr_timer_cntcurr = 0;

enum States { start, none_pressed, A0_pressed, A1_pressed, both_pressed } state;

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
	if (_avr_timer_cntcurr == 0){
		TimerISR();
		_avr_timer_cntcurr = _avr_timer_M;
	}
}

void TimerSet(unsigned long M) {
	_avr_timer_M = M;
	_avr_timer_cntcurr = _avr_timer_M;
}

unsigned char cnt;

void tick() {
	unsigned char a = (~PINA & 0x03);
	switch ( state ){
		case start: 
			state = none_pressed; 
			PORTC = 0x07;
			cnt = 0;
			break;
		case none_pressed: 	
			cnt = 0;
			if ( a == 0x01 ) {
				state = A0_pressed;
				if ( PORTC < 9 )
					PORTC = PORTC + 1;
			}
			else if (a == 0x02) {
				state = A1_pressed;
				if ( PORTC > 0 )
					PORTC = PORTC - 1;
			}
			else if (a == 0x03) {
				state = both_pressed;
			}
			break;
		case A0_pressed: 	
			if ( a == 0x00 ) state = none_pressed;
			else if (a == 0x02) state = A1_pressed;
			else if (a == 0x03) state = both_pressed;
			if (cnt > 10){
				PORTC = PORTC + 1;
				cnt = 0;
			}else {
				cnt++;
			}
			break;
		case A1_pressed: 
			if (a == 0x00) state = none_pressed;
			else if ( a == 0x01 ) state = A0_pressed;
			else if (a == 0x03) state = both_pressed;
			if (cnt > 10){
				PORTC = PORTC - 1;
				cnt = 0;
			}else {
				cnt++;
			}
			break;
		case both_pressed: 	
			if (a == 0x00) state = none_pressed;
			else if ( a == 0x01 ) state = A0_pressed;
			else if (a == 0x02) state = A1_pressed;
			break;
		default: state = start; 
			break;
	}
	switch ( state ){
		case none_pressed: 	
			break;
		case A0_pressed: 	
			break;
		case A1_pressed: 
			break;
		case both_pressed: 	
			PORTC = 0x00;
			break;
		default: state = start; 
			break;
	}
}

void main(void) {
    
    DDRA = 0x00; PORTA = 0xFF;
    DDRC = 0xFF; PORTC = 0x00;

    state = start;
    cnt = 0;

    while (1) {
    	tick();
    }
}