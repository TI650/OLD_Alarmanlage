//***************************************************************************
//* 
//* Projekt Name		:   
//* Haupt-Titel	    : 
//* Unter-Titel	    :  
//* Datei Name		  :  
//* Datum           : 07.01.2007
//* Version         : 0.0.1000
//* Support Tel 	  : +49 43 07 82 46 20
//* Support Fax		  : +49 43 07 82 46 21
//* Support Mobil	  : +49 17 34 26 69 77
//* Support eMail	  : m.kraus@suark.com
//* 
//* -------------------------------------------------------------------------
//* 
//* IDE             : Programmers Notepad 2
//* Compiler        : AVR-GCC 3.4
//* Target MCU		  : AtMega128
//*           		     oder
//*                   AtMega16 zu Testzwecken
//* 
//* -------------------------------------------------------------------------
//*
//*
//* Beschreibung
//* -------------
//*
//* 
//*
//* 
//* 
//*
//* 
//* 
//*
//***************************************************************************
//


volatile unsigned char  BuzzerLevelMin;
volatile unsigned char  BuzzerLevelMax;
volatile unsigned int   BuzzerIntervall;


//
// --------------------------------------------------------------------------
// System-Configuration für Hardware auf AVRCTRL-Basis
// Kennung: BOARD_AVRCTRL
// F_CPU  :  8000000Hz
// --------------------------------------------------------------------------
//
#ifdef BOARD_AVRCTRL
	#warning "Set: BOARD_AVRCTRL"
	#ifndef NO_LCD
		// lib: lcd
		#include "lcd_avrctrl.h"
	#endif
	
	#define PWM_PORT    PORTE
	#define PWM_DDR     DDRE
	#define PWM_PIN     PE5

	#define RTC_Adress  0xA0

//
// --------------------------------------------------------------------------
//
void TimerCounterPWM_Init (void) {
	//Set TIMER0 (PWM OC0 Pin)
//	DDRB |= (1<<3);
	TCCR0 |= (1<<WGM01|1<<WGM00|1<<COM01|1<<CS00);
//	OCR0 = 64;
}
//
// --------------------------------------------------------------------------
//
void TimerCounter2_Init (void) {
  TCCR2  = 0x0C;
//  OCR2   = 0x30; // 0x3F; // 0x7D;//0xFA; //  ca. 1ms on 14745600Hz        
  OCR2   = 0x7D;                          //  ca. 1ms on  8000000Hz        
  TIMSK |= (1<<OCIE2);
}
//
// --------------------------------------------------------------------------
//
// Timer1 Interrupt
volatile unsigned char bytecounter = 0;
ISR (TIMER1_OVF_vect) {
	TCNT1 = 65535 - (F_CPU/64/BuzzerIntervall);///500/100
	bytecounter++;
	OCR0 = bytecounter; // 255;  //128; //256;//pgm_read_byte(&sound[bytecounter]);
	if(bytecounter>BuzzerLevelMax) ///64)//
	{
		bytecounter = BuzzerLevelMin; //30;//
	}
}
//
// --------------------------------------------------------------------------
//
void Ports_Init (void) {
  // --- PORT A
  // Initialisation Port For Taste x 
  DDRA &=~ (1 << PA7);        /* Pin PA7 input               */
  PORTA |= (0 << PA7);        /* Pin PA7 pull-up disabled    */
  // Initialisation Port For Taste x
	DDRA &=~ (1 << PA6);        /* Pin PA6 input               */
  PORTA |= (0 << PA6);        /* Pin PA6 pull-up disabled    */
  // Initialisation Port For Taste x
	DDRA &=~ (1 << PA5);        /* Pin PA5 input               */
  PORTA |= (0 << PA5);        /* Pin PA5 pull-up disabled    */
  // Initialisation Port For Taste x 
  DDRA &=~ (1 << PA4);        /* Pin PA4 input               */
  PORTA |= (0 << PA4);        /* Pin PA4 pull-up disabled    */
  // Initialisation Port For Taste x
	DDRA &=~ (1 << PA3);        /* Pin PA3 input               */
  PORTA |= (0 << PA3);        /* Pin PA3 pull-up disabled    */

 // --- PORT B
  // Initialisation Port For  
  DDRB &=~ (1 << PB7);        /* Pin PB7 input               */
  PORTB |= (0 << PB7);        /* Pin PB7 pull-up disabled    */
  // Initialisation Port For  
	DDRB &=~ (1 << PB6);        /* Pin PB6 input               */
  PORTB |= (0 << PB6);        /* Pin PB6 pull-up disabled    */
  // Initialisation Port For  
	DDRB &=~ (1 << PB5);        /* Pin PB5 input               */
  PORTB |= (0 << PB5);        /* Pin PB5 pull-up disabled    */
  // Initialisation Port For  
  DDRB &=~ (1 << PB4);        /* Pin PB4 input               */
  PORTB |= (0 << PB4);        /* Pin PB4 pull-up disabled    */
  // Initialisation Port For  
//	DDRB &=~ (1 << PB3);        /* Pin PB3 input               */ PWM 
//  PORTB |= (0 << PB3);        /* Pin PB3 pull-up disabled    */ PWM 
  // Initialisation Port For  
	DDRB &=~ (1 << PB2);        /* Pin PB2 input               */
  PORTB |= (0 << PB2);        /* Pin PB2 pull-up disabled    */
  // Initialisation Port For  
  DDRB &=~ (1 << PB1);        /* Pin PB1 input               */
  PORTB |= (0 << PB1);        /* Pin PB1 pull-up disabled    */
  // Initialisation Port For  
	DDRB &=~ (1 << PB0);        /* Pin PB0 input               */
  PORTB |= (0 << PB0);        /* Pin PB0 pull-up disabled    */

 // --- PORT C
  // Initialisation Port For  
	DDRC &=~ (1 << PC3);        /* Pin PC3 input               */
  PORTC |= (0 << PC3);        /* Pin PC3 pull-up disabled    */

 // --- PORT D
  // Initialisation Port For  
  DDRD &=~ (1 << PD7);        /* Pin PD7 input               */
  PORTD |= (0 << PD7);        /* Pin PD7 pull-up disabled    */
  // Initialisation Port For  
  DDRD &=~ (1 << PD4);        /* Pin PD4 input               */
  PORTD |= (0 << PD4);        /* Pin PD4 pull-up disabled    */
  // Initialisation Port For  
	DDRD &=~ (1 << PD3);        /* Pin PD3 input               */
  PORTD |= (0 << PD3);        /* Pin PD3 pull-up disabled    */
  // Initialisation Port For  
	DDRD &=~ (1 << PD2);        /* Pin PD2 input               */
  PORTD |= (0 << PD2);        /* Pin PD2 pull-up disabled    */


}
//
// --------------------------------------------------------------------------
//

#endif

//
// --------------------------------------------------------------------------
// System-Configuration für Hardware mit ATMEGA128 und Peripherie
// Kennung: BOARD_AC128
// F_CPU  : 14745600Hz
// --------------------------------------------------------------------------
//
#ifdef BOARD_AC128
	#warning "Set: BOARD_AC128"
	#ifndef NO_LCD
		// lib: lcd
		#include "lcd_ac128.h"
	#endif
	
	#define PWM_PORT    PORTB
	#define PWM_DDR     DDRB
	#define PWM_PIN     PB3

	#define RTC_Adress  0xA0

//
// --------------------------------------------------------------------------
//
void TimerCounterPWM_Init (void) {
	//Set TIMER0 (PWM OC0 Pin)
//	DDRB |= (1<<3);
	TCCR0 |= (1<<WGM01|1<<WGM00|1<<COM01|1<<CS00);
//	OCR0 = 64;
}
//
// --------------------------------------------------------------------------
//
void TimerCounter2_Init (void) {
  TCCR2  = 0x0C;
  OCR2   = 0x30; // 0x3F; // 0x7D;//0xFA; //  ca. 1ms on 14745600Hz        
//  OCR2   = 0x7D;                          //  ca. 1ms on  8000000Hz        
  TIMSK |= (1<<OCIE2);
}
//
// --------------------------------------------------------------------------
//
// Timer1 Interrupt
volatile unsigned char bytecounter = 0;
ISR (TIMER1_OVF_vect) {
	TCNT1 = 65535 - (F_CPU/64/BuzzerIntervall);///500/100
	bytecounter++;
	OCR0 = bytecounter; // 255;  //128; //256;//pgm_read_byte(&sound[bytecounter]);
	if(bytecounter>BuzzerLevelMax) ///64)//
	{
		bytecounter = BuzzerLevelMin; //30;//
	}
}
//
// --------------------------------------------------------------------------
//
void Ports_Init (void) {
  // --- PORT A


 // --- PORT B


 // --- PORT C


 // --- PORT D

}
//
// --------------------------------------------------------------------------
//


#endif



//
// --------------------------------------------------------------------------
// HW-Const
// --------------------------------------------------------------------------
//


//
// --------------------------------------------------------------------------
// INCLUDES
// --------------------------------------------------------------------------
//
