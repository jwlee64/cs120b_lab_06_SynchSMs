/*	Author: josiahlee
 *  Partner(s) Name: Shiyou Wang
 *	Lab Section:
 *	Assignment: Lab #6  Exercise 2
 *	Exercise Description: 
 *
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

enum States { start, B0, B1, B2, lock, lock_up } state;

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

void tick() {
	switch(state){
		case start: state = B0; break;
		case B0: if (~PINA & 0x01) state = lock; else state = B1; break;
		case B1: if (~PINA & 0x01) state = lock; else state = B2; break;
		case B2: if (~PINA & 0x01) state = lock; else state = B0; break;
		case lock: if (PINA & 0x01) state = lock_up; break;
		case lock_up: if (~PINA & 0x01) state = B0; break;
		default: state = start; break;
	}

	switch(state){
		case start: PORTC = 0x00; break;
		case B0: PORTC = 0x01; break;
		case B1: PORTC = 0x02; break;
		case B2: PORTC = 0x04; break;
		case lock: break;
		case lock_up: break;
		default: state = start; break;
	}

}

void main(void) {
    /* Insert DDR and PORT initializations */
	DDRA = 0x00; PORTA = 0xFF;
	DDRC = 0xFF; PORTC = 0x00;
	TimerSet(300);
	TimerOn();
	state = start;
    /* Insert your solution below */
    while (1) {
    	tick();
    	while(!TimerFlag);
    	TimerFlag = 0;
    }
}
