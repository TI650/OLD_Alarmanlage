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


volatile unsigned char  BuzzerTast;
volatile unsigned char  BuzzerLevelMain;
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
	
	#define BOARD_AVRCTRL_TEST
	
	#define PWM_PORT    PORTE
	#define PWM_DDR     DDRE
	#define PWM_PIN     PE5

	#ifdef BOARD_AVRCTRL_TEST
		#define LED01_PORT       PORTB
		#define LED01_DDR        DDRB
		#define LED01_PNO        PB0
		#define LED02_PORT       PORTB
		#define LED02_DDR        DDRB
		#define LED02_PNO        PB1
		#define LED03_PORT       PORTB
		#define LED03_DDR        DDRB
		#define LED03_PNO        PB2

		#define TASTE_UP_PORT    PORTA
		#define TASTE_UP_DDR     DDRA
		#define TASTE_UP_PIN     PINA
		#define TASTE_UP_PNO     PA0
		#define TASTE_DWN_PORT   PORTA
		#define TASTE_DWN_DDR    DDRA
		#define TASTE_DWN_PIN    PINA
		#define TASTE_DWN_PNO    PA1
		#define TASTE_RET_PORT   PORTA
		#define TASTE_RET_DDR    DDRA
		#define TASTE_RET_PIN    PINA
		#define TASTE_RET_PNO    PA2

		#define SWITCH_ALARM_PORT   PORTA
		#define SWITCH_ALARM_DDR    DDRA
		#define SWITCH_ALARM_PIN    PINA
		#define SWITCH_ALARM_PNO    PA4
		#define SWITCH_HAND_PORT    PORTA
		#define SWITCH_HAND_DDR     DDRA
		#define SWITCH_HAND_PIN     PINA
		#define SWITCH_HAND_PNO     PA3
	#else
	#endif

	#define TASTE_EEProm_Init_PORT   PORTA
	#define TASTE_EEProm_Init_DDR    DDRA
	#define TASTE_EEProm_Init_PIN    PINA
	#define TASTE_EEProm_Init_PNO    PA3

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
volatile unsigned char tastcounter = 0;
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
#ifdef BOARD_AVRCTRL_TEST
  // Initialisation Port For LED01 
  LED01_DDR  |= (1 << LED01_PNO);                // AS Output              
  LED01_PORT |= (1 << LED01_PNO);                // SetPort for Test
  // Initialisation Port For LED02 
  LED02_DDR  |= (1 << LED02_PNO);                // AS Output              
  LED02_PORT |= (1 << LED02_PNO);                // SetPort for Test
  // Initialisation Port For LED03  
  LED03_DDR  |= (1 << LED03_PNO);                // AS Output              
  LED03_PORT |= (1 << LED03_PNO);                // SetPort for Test

  // Initialisation Port For TASTE_UP 
  TASTE_UP_DDR  &=~ (1 << TASTE_UP_PNO);         // AS Input              
  TASTE_UP_PORT &=~ (1 << TASTE_UP_PNO);         // Pull-up disabled   
  // Initialisation Port For TASTE_DWN  
  TASTE_DWN_DDR  &=~ (1 << TASTE_DWN_PNO);       // AS Input              
  TASTE_DWN_PORT &=~ (1 << TASTE_DWN_PNO);       // Pull-up disabled   
  // Initialisation Port For TASTE_RET 
  TASTE_RET_DDR  &=~ (1 << TASTE_RET_PNO);       // AS Input              
  TASTE_RET_PORT &=~ (1 << TASTE_RET_PNO);       // Pull-up disabled   
  // Initialisation Port For SWITCH_ALARM 
  SWITCH_ALARM_DDR  &=~ (1 << SWITCH_ALARM_PNO); // AS Input              
  SWITCH_ALARM_PORT &=~ (1 << SWITCH_ALARM_PNO); // Pull-up disabled   
  // Initialisation Port For SWITCH_HAND 
  SWITCH_HAND_DDR  &=~ (1 << SWITCH_HAND_PNO);   // AS Input              
  SWITCH_HAND_PORT &=~ (1 << SWITCH_HAND_PNO);   // Pull-up disabled   
#else
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

  // Initialisation Port For  
	DDRB &=~ (1 << PB2);        /* Pin PB2 input               */
  PORTB |= (0 << PB2);        /* Pin PB2 pull-up disabled    */
  // Initialisation Port For  
  DDRB &=~ (1 << PB1);        /* Pin PB1 input               */
  PORTB |= (0 << PB1);        /* Pin PB1 pull-up disabled    */
  // Initialisation Port For  
	DDRB &=~ (1 << PB0);        /* Pin PB0 input               */
  PORTB |= (0 << PB0);        /* Pin PB0 pull-up disabled    */
#endif

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
//	DDRB &=~ (1 << PB3);        /* Pin PB3 input              PWM */
//  PORTB |= (0 << PB3);        /* Pin PB3 pull-up disabled   PWM */

 // --- PORT C
  // Initialisation Port For 
	DDRC &=~ (1 << PC3);        /* Pin PD3 input               */
  PORTC |= (0 << PC3);        /* Pin PD3 pull-up disabled    */
 
 // --- PORT D
  // Initialisation Port For  
  DDRD &=~ (1 << PD7);        /* Pin PD7 input               */
  PORTD |= (0 << PD7);        /* Pin PD7 pull-up disabled    */
  // Initialisation Port For  
  DDRD &=~ (1 << PD4);        /* Pin PD4 input               */
  PORTD |= (0 << PD4);        /* Pin PD4 pull-up disabled    */
  // Initialisation Port For  
	DDRD &=~ (1 << PD3);        /* Pin PD3 input                */
  PORTD |= (0 << PD3);        /* Pin PD3 pull-up disabled     */
  // Initialisation Port For  
	DDRD &=~ (1 << PD2);        /* Pin PD2 input               */
  PORTD |= (0 << PD2);        /* Pin PD2 pull-up disabled    */
}
//
// --------------------------------------------------------------------------
//
void s_RootPortsToRegs(void) {
	unsigned char i;
	
  for (i=0;i<8;i++)
	{
    regPortActual[i] = 0;    
	}                            

			if( PINA & (1<<PINA3) )  
			{
			  // FREE
			}


			if( PINA & (1<<PINA4) )  
			{
				regPortActual[2] |= (1<<PA1);    // SETZEN
			}

			if( PINA & (1<<PINA5) )  
			{
				regPortActual[2] |= (1<<PA0);    // SETZEN
			}

			if( PINA & (1<<PINA6) )  
			{
				regPortActual[1] |= (1<<PA0);    // SETZEN
				regPortActual[3] |= (1<<PA0);    // SETZEN
				regPortActual[3] |= (1<<PA1);    // SETZEN
			}

			if( PINA & (1<<PINA7) )  
			{
				regPortActual[0] |= (1<<PA0);
			}


			if( PINB & (1<<PINB0) )  
			{
				regPortActual[2] |= (1<<PB2);
			}

			if( PINB & (1<<PINB1) )  
			{
				regPortActual[3] |= (1<<PB2);
			}

			if( PINB & (1<<PINB2) )  
			{
				regPortActual[2] |= (1<<PB3);
			}

//			if( PINB & (1<<PINB3) )                // PWM
//			{
//				regPortActual[3] |= (1<<PB3);
//			}

			if( PINB & (1<<PINB4) )  
			{
				regPortActual[2] |= (1<<PB4);
			}

			if( PINB & (1<<PINB5) )  
			{
				regPortActual[3] |= (1<<PB4);
			}

			if( PINB & (1<<PINB6) )  
			{
				regPortActual[2] |= (1<<PB5);
			}

			if( PINB & (1<<PINB7) )  
			{
				regPortActual[3] |= (1<<PB5);
			}


			if( PINC & (1<<PINC3) )      // Placed for PWM
			{
				regPortActual[3] |= (1<<PC3);
			}


			if( PIND & (1<<PIND2) )  
			{
				regPortActual[0] |= (1<<PD1);
			}

			if( PIND & (1<<PIND3) )             
			{
  			regPortActual[1] |= (1<<PD1);
			}

			if( PIND & (1<<PIND4) )  
			{
				regPortActual[2] |= (1<<PD6);
			}

			if( PIND & (1<<PIND7) )  
			{
				regPortActual[3] |= (1<<PD6);
			}

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
	
	#define PWM_PORT              PORTB
	#define PWM_DDR               DDRB
	#define PWM_PIN               PB3


	#define Int_Buzz1_PORT        PORTE
	#define Int_Buzz1_DDR         DDRE
	#define Int_Buzz1_PIN         PE5

	#define Int_Buzz2_PORT        PORTB
	#define Int_Buzz2_DDR         DDRB
	#define Int_Buzz2_PIN         PB3

	#define Int_SIREN_PORT        PORTE
	#define Int_SIREN_DDR         DDRE
	#define Int_SIREN_PIN         PE7

	#define Int_FLASH_PORT        PORTE
	#define Int_FLASH_DDR         DDRE
	#define Int_FLASH_PIN         PE6

	#define Ext_PWR_SIREN_PORT    PORTB
	#define Ext_PWR_SIREN_DDR     DDRB
	#define Ext_PWR_SIREN_PIN     PB6

	#define Ext_PWR_FLASH_PORT    PORTB
	#define Ext_PWR_FLASH_DDR     DDRB
	#define Ext_PWR_FLASH_PIN     PB5
	
	#define Ext_AlmSig_PORT       PORTB
	#define Ext_AlmSig_DDR        DDRB
	#define Ext_AlmSig_PIN        PB7

	#define Ext_PWR_PORT          PORTE
	#define Ext_PWR_DDR           DDRE
	#define Ext_PWR_PIN           PE4

	#define LED_ModeHand_PORT     PORTA
	#define LED_ModeHand_DDR      DDRA
	#define LED_ModeHand_PNO      PA5
	#define LED_Armed_PORT        PORTA
	#define LED_Armed_DDR         DDRA
	#define LED_Armed_PNO         PA6
	#define LED_NotArmed_PORT     PORTD
	#define LED_NotArmed_DDR      DDRD
	#define LED_NotArmed_PNO      PA3
	#define LED03x_PORT           PORTA
	#define LED03x_DDR            DDRA
	#define LED03x_PNO            PA7
	
	#define TASTE_UP_PORT         PORTG
	#define TASTE_UP_DDR          DDRG
	#define TASTE_UP_PIN          PING
	#define TASTE_UP_PNO          PG0
	#define TASTE_DWN_PORT        PORTA
	#define TASTE_DWN_DDR         DDRA
	#define TASTE_DWN_PIN         PINA
	#define TASTE_DWN_PNO         PA1
	#define TASTE_RET_PORT        PORTA
	#define TASTE_RET_DDR         DDRA
	#define TASTE_RET_PIN         PINA
	#define TASTE_RET_PNO         PA2

	#define SWITCH_ALARM_PORT     PORTA
	#define SWITCH_ALARM_DDR      DDRA
	#define SWITCH_ALARM_PIN      PINA
	#define SWITCH_ALARM_PNO      PA4
	#define SWITCH_HAND_PORT      PORTA
	#define SWITCH_HAND_DDR       DDRA
	#define SWITCH_HAND_PIN       PINA
	#define SWITCH_HAND_PNO       PA3

	#define TASTE_EEProm_Init_PORT   TASTE_RET_PORT
	#define TASTE_EEProm_Init_DDR    TASTE_RET_DDR
	#define TASTE_EEProm_Init_PIN    TASTE_RET_PIN
	#define TASTE_EEProm_Init_PNO    TASTE_RET_PNO

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
volatile unsigned char tastcounter = 0;
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

  // Initialisation Port For TASTE_UP 
  TASTE_UP_DDR  &=~ (1 << TASTE_UP_PNO);          // AS Input              
  TASTE_UP_PORT &=~ (1 << TASTE_UP_PNO);          // Pull-up disabled   
  // Initialisation Port For TASTE_DWN   
  TASTE_DWN_DDR  &=~ (1 << TASTE_DWN_PNO);        // AS Input              
  TASTE_DWN_PORT &=~ (1 << TASTE_DWN_PNO);        // Pull-up disabled   
  // Initialisation Port For TASTE_RET 
  TASTE_RET_DDR  &=~ (1 << TASTE_RET_PNO);        // AS Input              
  TASTE_RET_PORT &=~ (1 << TASTE_RET_PNO);        // Pull-up disabled   
  // Initialisation Port For SWITCH_ALARM 
  SWITCH_ALARM_DDR  &=~ (1 << SWITCH_ALARM_PNO);  // AS Input              
  SWITCH_ALARM_PORT &=~ (1 << SWITCH_ALARM_PNO);  // Pull-up disabled   
  // Initialisation Port For SWITCH_HAND 
  SWITCH_HAND_DDR  &=~ (1 << SWITCH_HAND_PNO);    // AS Input              
  SWITCH_HAND_PORT &=~ (1 << SWITCH_HAND_PNO);    // Pull-up disabled   

  // Initialisation Port For LED_ModeHand 
  LED_ModeHand_DDR  |= (1 << LED_ModeHand_PNO);                 // AS Output              
  LED_ModeHand_PORT |= (1 << LED_ModeHand_PNO);                 // SetPort for Test
  // Initialisation Port For LED02 
  LED_Armed_DDR  |= (1 << LED_Armed_PNO);                 // AS Output              
  LED_Armed_PORT |= (1 << LED_Armed_PNO);                 // SetPort for Test
  // Initialisation Port For LED03  
  LED03x_DDR  |= (1 << LED03x_PNO);                 // AS Output              
  LED03x_PORT |= (1 << LED03x_PNO);                 // SetPort for Test
  // Initialisation Port For LED04 
  LED_NotArmed_DDR  |= (1 << LED_NotArmed_PNO);                 // AS Output              
  LED_NotArmed_PORT |= (1 << LED_NotArmed_PNO);                 // SetPort for Test

  // Initialisation Port For Internal Buzzer No1 
  Int_Buzz1_DDR  |= (1 << Int_Buzz1_PIN);         // AS Output              
  Int_Buzz1_PORT &=~(1 << Int_Buzz1_PIN);         // Int-Buzzer No1 Off

  // Initialisation Port For Internal Buzzer No2
  Int_Buzz2_DDR  |= (1 << Int_Buzz2_PIN);         // AS Output              
  Int_Buzz2_PORT &=~(1 << Int_Buzz2_PIN);         // Int-Buzzer No2 Off

  // Initialisation Port For Internal Alarm-Siren 
  Int_SIREN_DDR  |= (1 << Int_SIREN_PIN);         // AS Output              
  Int_SIREN_PORT &=~(1 << Int_SIREN_PIN);         // Int-Siren Off

  // Initialisation Port For Internal Alarm-Flash 
  Int_FLASH_DDR  |= (1 << Int_FLASH_PIN);         // AS Output              
  Int_FLASH_PORT &=~(1 << Int_FLASH_PIN);         // Int-Flash Off

  // Initialisation Port For External Alarm-Siren 
  Ext_PWR_SIREN_DDR  |= (1 << Ext_PWR_SIREN_PIN); // AS Output              
  Ext_PWR_SIREN_PORT &=~(1 << Ext_PWR_SIREN_PIN); // Ext-Siren PWR Off

  // Initialisation Port For External Alarm-Flash 
  Ext_PWR_FLASH_DDR  |= (1 << Ext_PWR_FLASH_PIN); // AS Output              
  Ext_PWR_FLASH_PORT &=~(1 << Ext_PWR_FLASH_PIN); // Ext-Flash PWR Off

  // Initialisation Port For External Alarm-Signal 
  Ext_AlmSig_DDR  |= (1 << Ext_AlmSig_PIN);       // AS Output              
  Ext_AlmSig_PORT &=~(1 << Ext_AlmSig_PIN);       // Ext-Alarm-Signal Off

  // Initialisation Port For External Power 
  Ext_PWR_DDR  |= (1 << Ext_PWR_PIN);             // AS Output              
  Ext_PWR_PORT &=~(1 << Ext_PWR_PIN);             // Ext-Power Off
	

}
//
// --------------------------------------------------------------------------
//
void s_RootPortsToRegs(void) {
	unsigned char Port;
	unsigned char Bit;
	unsigned char InPort;
	unsigned char InBit;
	unsigned char PtrToTable;
	
  for (Port=0;Port<8;Port++)
	{
    regPortActual[Port] = 0;    
	}                            

  for (Port=0;Port<8;Port++)
	{
		PtrToTable = Port * 8;
		for (Bit=0;Bit<8;Bit++)
		{
			if (InPortRootingTable[PtrToTable + Bit] != 0xFF);
			{
				InPort = InPortRootingTable[PtrToTable + Bit];
				InPort = (InPort & 0b11110000) / 16;
				InBit = InPortRootingTable[PtrToTable + Bit];
				InBit = InBit & 0b00001111;
				
				if(InPortVal[InPort] & (1<<InBit))  
				{
					regPortActual[Port] |= (1<<Bit);
				}
			}
		}                            
	}                            

/*	
  for (Port=0;Port<8;Port++)
	{
		regPortActual[Port] = IntfPortVal[Port];

	}                            
*/



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
