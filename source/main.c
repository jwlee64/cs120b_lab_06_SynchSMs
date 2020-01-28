/*	Author: Shiyou Wang swang324
 *  Partner(s) Name: Josiah Lee
 *	Lab Section: 
 *	Assignment: Lab #6  Exercise #3
 *	Exercise Description: [optional - include for your own benefit]
 *
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 */

#include <avr/io.h>
#include <avr/interrupt.h>


unsigned char curStage=0x00;
unsigned char btns = 0x00;
unsigned char temp=0x00;

enum _state {INIT,BEFORE_PLUS_ONE, PLUS_ONE, BEFORE_MINUS_ONE, MINUS_ONE,RESET,STANDBY, LONG_PRESSED} state, restoreState;


volatile unsigned char TimerFlag = 0;

unsigned long _avr_timer_M = 1; // Default 1 ms.
unsigned long _avr_timer_cntcurr = 0; // Ticks

void TimerOn(){
TCCR1B = 0x0B; // Clear timer and set basic unit.    bit3---clear  bit2bit1bit0---time unit options   clock /64  125,000 ticks/s	
OCR1A = 125;   // Set unit period.           num * ticks/s  = interupt time 
TIMSK1 = 0x02; // Enable timer	

TCNT1=0;  // Initial counter
_avr_timer_cntcurr = _avr_timer_M; // TimerISR period in milliseconds

SREG |= 0x80;  //Enable global interrupts
}

void TimerOff(){	
TCCR1B = 0x00; // bit3bit1bit0=000: timer off
}

void TimerISR(){
TimerFlag = 1;
}

// TCNT1 == OCR1
ISR(TIMER1_COMPA_vect){
_avr_timer_cntcurr--;
if (_avr_timer_cntcurr == 0){	
TimerISR();
_avr_timer_cntcurr = _avr_timer_M;
}
}

// Set TimerISR() to tick every M ms
void TimerSet(unsigned long M){
_avr_timer_M = M;
_avr_timer_cntcurr = _avr_timer_M;
}


void Tick(){

btns = PINA & 0x03;

switch(state){

case INIT:{
temp = 0x07;
if(btns==0x01)
state = BEFORE_PLUS_ONE;
else if(btns==0x02)
state = BEFORE_MINUS_ONE;
else if(btns==0x03)
state = RESET;
break;
}
case BEFORE_PLUS_ONE:	{
if(btns==0x00)
state = PLUS_ONE;
else{	
restoreState = PLUS_ONE;
state = LONG_PRESSED;
}	
break;
}
case PLUS_ONE:{

if(temp >= 0x09)
temp = 0x09;
else
temp = temp + 0x01;

state = STANDBY;

break;
}
case BEFORE_MINUS_ONE:	{
if(btns==0x00)
state = MINUS_ONE;
break;

case MINUS_ONE:
if(temp <= 0x00)
temp = 0x00;
else
temp = temp - 0x01;
state = STANDBY;
break;
}
case STANDBY:{
if(btns == 0x01)
state = BEFORE_PLUS_ONE;
else if(btns==0x02)
state = BEFORE_MINUS_ONE;
else if(btns == 0x03)
state = RESET;
break;
{
case RESET:{
if(btns == 0x03)
temp = 0x07;
else
state = INIT;
break;
}
case LONG_PRESSED:{

curStage++;
if(curStage==10){
curStage=0;
state = restoreState;
}
else  {
state  = LONG_PRESSED;
}

break;
}
default:
break;
}

PORTC = temp;

}
}
}

int main(void)
{
DDRA = 0x00;PORTA = 0xFF;
DDRC = 0xFF;PORTC = 0x00;
TimerSet(100); // 100ms
TimerOn();
state = INIT;
while(1) {
Tick();
while (!TimerFlag);
TimerFlag = 0;
}
}
