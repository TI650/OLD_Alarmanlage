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
// --------------------------------------------------------------------------
// System-Const
// --------------------------------------------------------------------------
//
//#define NO_LCD

  //Hardware Design
//#define BOARD_AC128
#define BOARD_AVRCTRL

  //LCD Variante
//#define LCD_0416 
#define LCD_0216 

//
// --------------------------------------------------------------------------
// HW-Const
// --------------------------------------------------------------------------
//

  // define CPU frequency in Mhz here if not defined in Makefile  
#ifndef F_CPU
  #define F_CPU 16000000  
#endif

#define UART_BAUD_RATE      9600          // 9600 baud 
#define MSECOND             F_CPU / 1000  // Definition Millisekunde  

//
// --------------------------------------------------------------------------
// INCLUDES
// --------------------------------------------------------------------------
//

#include <stdlib.h>
#include <string.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <avr/eeprom.h>
#include <util/delay.h>
#include "uart.h"

#include "P_Config.c"
//#ifndef NO_LCD
//  #ifdef BOARD_AC128
//    #include "lcd_AC128.h"
//  #endif
//  #ifdef BOARD_AVRCTRL
//    #include "lcd_AVRCTRL.h"
//  #endif
//#endif

// --------------------------------------------------------------------------
// DEBUG-DEFINES
// --------------------------------------------------------------------------

#define DEBUG_ONx

#ifdef DEBUG_ON

  #define DEBUG_STEPPING
	
	#include "DEBUG/Debug.h"

#endif

// --------------------------------------------------------------------------
// DEFINES
// --------------------------------------------------------------------------

#define NICHTS   0
#define MENU     1
#define UP       3
#define DOWN     4
#define OKAY     5
#define TASTE1   6
#define TASTE2   7
#define TASTE3   8 
#define TASTE4   9 
#define TASTE5  10 


#define DEFAULT                  255
#define Index_Menue_Auswahl01      1
#define Index_Menue_DateTime       2
#define Index_Menue_Overview     DEFAULT

#define Index_Menue_INSTALL       200


#define DataSeperator ','




// --------------------------------------------------------------------------
// SW-Const
// --------------------------------------------------------------------------

volatile uint8_t   menu_actual   = 0;
volatile uint8_t   menu_befor01  = 0;
volatile uint8_t   menu_befor02  = 0;

unsigned char MenuCount=0;
unsigned char Taste = 0;

unsigned char MenuMainIndex = DEFAULT;

char CharBuffer[22];
int  IntBuffer;

unsigned char vCountdown = 255;
#define cCountdown 10



char *HMenues[]= {"Datum/Uhrzeit","H-MENUE-01","H-MENUE-02","H-MENUE-03","H-MENUE-04","H-MENUE-05","H-MENUE-06","H-MENUE-07"};

// --------------------------------------------------------------------------
// SW-Variables
// --------------------------------------------------------------------------

  struct {
    unsigned char b01000ms    :1;    // 1 Bit für  1000millisekunden
    unsigned char bunused1    :1;    // 1 Bit für  
    unsigned char bunused2    :1;    // 1 Bit für      
    unsigned char bunused3    :1;    // 1 Bit für      
    unsigned char bunused4    :1;    // 1 Bit für      
    unsigned char bunused5    :1;    // 1 Bit für      
    unsigned char bunused6    :1;    // 1 Bit für      
    unsigned char bunused7    :1;    // 1 Bit für      
    //unsigned char bNochNBit:1; // Und hier noch mal ein Bit
    //unsigned char b2Bits:2; // Dieses Feld ist 2 Bits breit
    // All das hat in einer einzigen Byte-Variable Platz.
    // die 3 verbleibenden Bits bleiben ungenutzt
  } fPollingTime;

  struct {
    unsigned char bunused0   :1;     
    unsigned char bunused1   :1;     
    unsigned char bunused2   :1;     
    unsigned char bTast1     :1;     
    unsigned char bTast2     :1;     
    unsigned char bTast3     :1;     
    unsigned char bTast4     :1;     
    unsigned char bTast5     :1;     
  } Taster;

  struct {
    unsigned char bRefreshDisplay :1;     
    unsigned char bRefreshDTG     :1;     
    unsigned char bRefreshMenu    :1;     
    unsigned char bStopClock  :1;     
    unsigned char bTRight   :1;     
    unsigned char bWheelUp :1;     
    unsigned char bWheelDwn :1;     
    unsigned char bunused7:1;     
  } BStatus;

  struct {
    unsigned char Weekday;     
    unsigned char Day;     
    unsigned char Month;     
    unsigned char Year;     
    unsigned char Hour;     
    unsigned char Min;     
    unsigned char Sec;     
  } sDTG;

  // Akiviert das Senden von verschiedenen Meldungen
  struct {  
    unsigned char bunused0       :1;    //  
    unsigned char bunused1       :1;    //  
    unsigned char SendStatus     :1;    // Gesamtstatus
    unsigned char bunused3       :1;     
    unsigned char bunused4       :1;     
    unsigned char bunused5       :1;     
    unsigned char bunused6       :1;     
    unsigned char bunused7       :1;     
  } fSendData;

  // Statusinformationen zum Alarmmode
  struct {  
    unsigned char AlarmAktiv     :1;    //  
    unsigned char bunused1       :1;    //  
    unsigned char Countdown      :1;    // Aktivität des Countdowns
    unsigned char bunused3       :1;     
    unsigned char bunused4       :1;     
    unsigned char bunused5       :1;     
    unsigned char bunused6       :1;     
    unsigned char bunused7       :1;     
  } fAlarmMode;



/*  struct {
    unsigned char bMenuMain      :1;     
    unsigned char bDispBackLight    :1;     
    unsigned char bunused2  :1;     
    unsigned char bunused3   :1;     
    unsigned char bunused4 :1;     
    unsigned char bunused5 :1;     
    unsigned char bunused6:1;     
    unsigned char bunused7:1;     
  } TimeOut;
	
  char *DayStr[]= {"Mo","Di","Mi","Do","Fr","Sa","So"};
*/
 
// --------------------------------------------------------------------------
// EEPROM - Definitions, Const and Variables
// --------------------------------------------------------------------------

#define EEPROM_DEFAULT                0xFF
#define EEPROM_DEFAULT_CHECKBYTEVALUE 0xAA
#define EEPROM_DEFAULT_COUNTDOWN        10
#define EEPROM_DEFAULT_INPORTCNT         8
#define EEPROM_DEFAULT_INPORTADR0     0xAA
#define EEPROM_DEFAULT_INPORTADR1     0xAA
#define EEPROM_DEFAULT_INPORTADR2     0xAA
#define EEPROM_DEFAULT_INPORTADR3     0xAA
#define EEPROM_DEFAULT_INPORTADR4     0xAA
#define EEPROM_DEFAULT_INPORTADR5     0xAA
#define EEPROM_DEFAULT_INPORTADR6     0xAA
#define EEPROM_DEFAULT_INPORTADR7     0xAA


// EEMEM wird bei aktuellen Versionen der avr-lib in eeprom.h definiert
// hier: definiere falls noch nicht bekannt ("alte" avr-libc)
#ifndef EEMEM
// alle Textstellen EEMEM im Quellcode durch __attribute__ ... ersetzen
#define EEMEM  __attribute__ ((section (".eeprom")))
#endif
//#define EEPROM_SECTION  __attribute__ ((section (".eeprom")))
//this global variables are stored in EEPROM
  uint16_t  eedummy          EEMEM  = 0 ;       // avoid using lowest addresses
  uint8_t   eeCheckByte      EEMEM  = EEPROM_DEFAULT_CHECKBYTEVALUE;       // 
  uint8_t   eeCountdown      EEMEM  = 10;       // Dauer des Countdowns
  uint8_t   eeInPortCnt      EEMEM  =  8;       // Anzahl der Ports (Anz Meldeschleifen = Ports mal 7)
  uint8_t   eeInPortAdr[8]   EEMEM ;// = {7,6,5,4,3,2,1,0};       // Portadress PCF8xxx

//uint8_t   eeprom_var1      EEPROM_SECTION  = 1;       // EEPROM address 0002
//uint8_t   eeprom_var2      EEPROM_SECTION  = 2;       // EEPROM address 0003
//uint16_t  eeprom_var3      EEPROM_SECTION  = 1027;    // low byte = 0003, high = 0004  
//float     eeprom_var4      EEPROM_SECTION  = 1.3456;  // four byte float 
//char     *eeprom_str01     EEPROM_SECTION ="0123456789ABCDEF";
//char      eeprom_str01[17]     EEPROM_SECTION = "1234567890123456\0";



// **************************************************************************
// **************************************************************************
// **************************************************************************
// --------------------------------------------------------------------------
// SUB-Routines and Functions
// --------------------------------------------------------------------------
//
void IntToStringformat(int value, char zero, char sign) {
    char buffer[6];
		char i;
		
		CharBuffer[0] = '\0';
		
		itoa( value, buffer, 10);   // convert interger into string (decimal format)         
		if ((value > 0) && (sign > 0)) strcat(CharBuffer,"+");  

		if (zero > 0) 
		{
		
		  for(i = strlen(buffer); i <= zero-1; i++)
      {
        strcat(CharBuffer,"0");
      }
		
		}
    strcat(CharBuffer,buffer);
}
//
// --------------------------------------------------------------------------
//
void ShowIntroUart0 (void) {
  uart_puts("Alarm Control Vers: AC0001\r\n");
  uart_puts("DateTime, System started\r\n");
}
//
// --------------------------------------------------------------------------
//
void ShowIntroLCD (void) {
#ifndef NO_LCD
  lcd_clrscr();                 // clear display and home cursor  
	lcd_puts("AlarmCtrl AC0001"); // put string to display  
	lcd_gotoxy(1,1);              // move cursor to position 1 on line 2  
	lcd_puts("SUARK.COM,2007");   // put string to display
#endif  
}
//
// --------------------------------------------------------------------------
//
void ShowMenueMain (void) {
//	ClearScreen();                     
//  GLCD_DrawFrameMain();
}
//
// --------------------------------------------------------------------------
//
void getTastatur(void) {

 static char taste2 = 0,taste3 = 0,taste4 = 0,taste5 = 0;

//  static char enc_last = 0x01;
//  char i = 0;

//  Taste = NICHTS;
	Taster.bTast1     = 0;
	Taster.bTast2     = 0;
	Taster.bTast3     = 0;
	Taster.bTast4     = 0;
	Taster.bTast5     = 0;


	// Fuehre aus, wenn Bit Nr.3 in PINA gesetzt ist (Taste 4)
	if ( PINA & (1<<PINA3) ) 
  {
		/* Aktion */
    if(taste2++ == 0) Taster.bTast2 = 1;
  }
	else taste2 = 0;
	// Fuehre aus, wenn Bit Nr.2 in PINA gesetzt ist (Taste 3)
	if ( PINA & (1<<PINA2) ) 
  {
	  /* Aktion */
    if(taste3++ == 0) Taster.bTast3 = 1;
  }
	else taste3 = 0;
	// Fuehre aus, wenn Bit Nr.1 in PINA gesetzt ist (Taste 2)
	if ( PINA & (1<<PINA1) ) 
  {
		/* Aktion */
    if(taste4++ == 0) Taster.bTast4 = 1;
  }
	else taste4 = 0;
	// Fuehre aus, wenn Bit Nr.0 in PINA gesetzt ist (Taste 1)
	if ( PINA & (1<<PINA0) ) 
  {
		/* Aktion */
    if(taste5++ == 0) Taster.bTast5 = 1;
  }
	else taste5 = 0;
	
  if(Taster.bTast5     == 1)  Taste = TASTE5;
  if(Taster.bTast4     == 1)  Taste = TASTE4;
  if(Taster.bTast3     == 1)  Taste = TASTE3;
  if(Taster.bTast2     == 1)  Taste = TASTE2;
  if(Taster.bTast1     == 1)  Taste = TASTE1;

}
//
// --------------------------------------------------------------------------
//
void s_SendStatus(void){
  char buffer[2];

	uart_puts("$ACSTA;");              // Protokollkennung
	uart_puts("06.01.08 17:15:04;");   // Datum / Uhrzeit

  itoa(fAlarmMode.AlarmAktiv, buffer, 10);     // convert interger into string (decimal format)         
	uart_puts(buffer);                 // Diverse Daten
	uart_putc(';');                    // Datenseperator
	uart_puts("xx;xx;xx;");            // Diverse Daten
	uart_puts("*EA\r\n");                // Checksum
}
//
// --------------------------------------------------------------------------
//
void s_SendInfoHardware(void){
	uart_puts("$ACHW1;");              // Protokollkennung
	uart_puts("ATMEGA128;");           // Integrierter MC
	uart_puts("147xxxxx;");            // MC-Frequenz in Hz
	uart_puts("00.01;");               // Boardversion
	uart_puts("xx;xx;xx;");            // Diverse Daten
	uart_puts("*EA\r\n");                // Checksum
}
//
// --------------------------------------------------------------------------
//
void s_SendInfoSoftware(void){
	uart_puts("$ACSW1;");              // Protokollkennung
	uart_puts("00.01;");               // Softwareversion
	uart_puts("00.01;");               // Protokollversion
	uart_puts("xx;xx;xx;");            // Diverse Daten
	uart_puts("*EA\r\n");                // Checksum
}
//
// --------------------------------------------------------------------------
//
void s_Countdown(void){
  char buffer[2];

	if(vCountdown == 255) { vCountdown = eeprom_read_byte(&eeCountdown); }

	uart_puts("$ACCNT;");              // Protokollkennung
  itoa( vCountdown, buffer, 10);     // convert interger into string (decimal format)         
	uart_puts(buffer);                 // Diverse Daten
	uart_putc(DataSeperator);     
	uart_puts("*EA\r\n");              // Checksum
	vCountdown--;

	if (vCountdown == 0) 
	{ 
	  vCountdown--;
		fAlarmMode.Countdown = 0;
		if (fAlarmMode.AlarmAktiv == 0)
		{ fAlarmMode.AlarmAktiv = 1; }   // Aktiviert den Alarmmode
		else
		{ fAlarmMode.AlarmAktiv = 0; }   // Deaktiviert den Alarmmode
//		s_SendStatus();
	}
}
//
// --------------------------------------------------------------------------
//***************************************************************************
//* Name		        : EEProm_Init 
//* Datum / Version : 09.01.2008 / 0.0.1000
//* -------------------------------------------------------------------------
//* Start-Value:	 
//*   mode       unsigned char     >> Mode of Initialisation
//*                                   Mode 0 = Init if EEPROM is empty
//*                                   Mode 1 = Set Default Values to EEPROM
//* -------------------------------------------------------------------------
//* Return-Value:  -none- 
//* -------------------------------------------------------------------------
//* Description:
//*   Initialisation of the used EEProm space controled by a given Value.
//*   On mode = 0 the EEPROM space will only init if the space is empty.
//*   On mode = 1 the EEPROM space will be writen by default values.
//* 
//*
//***************************************************************************
#define EEProm_Init_IfEmpty     0
#define EEProm_Init_SetDefault  1
void s_EEProm_Init(unsigned char mode){
	unsigned char buffer = EEPROM_DEFAULT;
  
	if(mode == EEProm_Init_IfEmpty)
	{
    buffer = eeprom_read_byte(&eeCheckByte);
	}

//	if(mode == EEProm_Init_SetDefault)
//	{
//    buffer = EEPROM_DEFAULT;
//	}

  if(buffer != EEPROM_DEFAULT_CHECKBYTEVALUE)
	{
		eeprom_write_byte(&eeCheckByte, EEPROM_DEFAULT_CHECKBYTEVALUE);
		eeprom_write_byte(&eeCountdown, EEPROM_DEFAULT_COUNTDOWN);
		eeprom_write_byte(&eeInPortCnt, EEPROM_DEFAULT_INPORTCNT);
		eeprom_write_byte(&eeInPortAdr[0], EEPROM_DEFAULT_INPORTADR0);
		eeprom_write_byte(&eeInPortAdr[1], EEPROM_DEFAULT_INPORTADR1);
		eeprom_write_byte(&eeInPortAdr[2], EEPROM_DEFAULT_INPORTADR2);
		eeprom_write_byte(&eeInPortAdr[3], EEPROM_DEFAULT_INPORTADR3);
		eeprom_write_byte(&eeInPortAdr[4], EEPROM_DEFAULT_INPORTADR4);
		eeprom_write_byte(&eeInPortAdr[5], EEPROM_DEFAULT_INPORTADR5);
		eeprom_write_byte(&eeInPortAdr[6], EEPROM_DEFAULT_INPORTADR6);
		eeprom_write_byte(&eeInPortAdr[7], EEPROM_DEFAULT_INPORTADR7);
	}
}
//
// --------------------------------------------------------------------------
//
void s_AlarmAbort(void){

}
//
// --------------------------------------------------------------------------
//
void s_AlarmEnable(void){

}
//
// --------------------------------------------------------------------------
//
void s_AlarmDisable(void){

}
//
// --------------------------------------------------------------------------
// Hardware-Initialisation-Routines
// --------------------------------------------------------------------------
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
//void TimerCounter1_Init (void) {
//  TCCR1B = 1<<WGM12^1<<CS10;		// divide by 1
//					// clear on compare
//  OCR1A = XTAL / DEBOUNCE - 1;
//  TCNT1 = 0;
//  second = 0;
//  prescaler = (uchar)DEBOUNCE;
//  TIMSK = 1<<OCIE1A;
//}
//
// --------------------------------------------------------------------------
//
//void TimerCounter0_Init (void) {
//  TCCR0 = 1<<CS01;			//divide by 8 * 256
////  TIMSK = 1<<TOIE0;			//enable timer interrupt
//  TIMSK |= (1<<TOIE0);			//enable timer interrupt
//}
//
// --------------------------------------------------------------------------
//
void Ports_Init (void) {
  // Initialisation Port For Taste1 
  DDRA &=~ (1 << PA4);        /* Pin PA4 input               */
  PORTA |= (0 << PA4);        /* Pin PA4 pull-up disabled    */
  // Initialisation Port For Taste2
	DDRA &=~ (1 << PA3);        /* Pin PA3 input               */
  PORTA |= (0 << PA3);        /* Pin PA3 pull-up disabled    */
  // Initialisation Port For Taste3
  DDRA &=~ (1 << PA2);        /* Pin PA2 input               */
  PORTA |= (0 << PA2);        /* Pin PA2 pull-up disabled    */
  // Initialisation Port For Taste4
  DDRA &=~ (1 << PA1);        /* Pin PA1 input               */
  PORTA |= (0 << PA1);        /* Pin PA1 pull-up disabled    */
  // Initialisation Port For Taste5 
  DDRA &=~ (1 << PA0);        /* Pin PA0 input               */
  PORTA |= (0 << PA0);        /* Pin PA0 pull-up disabled    */
}
//
// --------------------------------------------------------------------------
//

//
// --------------------------------------------------------------------------
//

//
// --------------------------------------------------------------------------
//

//
// --------------------------------------------------------------------------
//

//************************************************************************
// --------------------------------------------------------------------------
// Interrupt-Service-Routines
// --------------------------------------------------------------------------
//
// --------------------------------------------------------------------------
//
  // io handler for Output Compare 2 overflow interrupt
SIGNAL (SIG_OUTPUT_COMPARE2){
  static unsigned int timer01000ms = 0;

  if(++timer01000ms == 1000)
  {    
    fPollingTime.b01000ms = 1;
//     uart_putc('1');

    timer01000ms = 0;
  }
}
//
// --------------------------------------------------------------------------
//











int main(void)
{
  // ------------------------
  // --> SW-Settings
  // ------------------------

    sDTG.Weekday   =  2;     
    sDTG.Day       = 31;     
    sDTG.Month     = 12;     
    sDTG.Year      =  7;     
    sDTG.Hour      = 23;     
    sDTG.Min       = 59;
    sDTG.Sec       = 47;     
    
    unsigned int c;
//    char buffer[7];
//    int  num=134;

  // ------------------------
  // --> HW-Settings
  // ------------------------

  // initialize Ports 
//  Ports_Init();

//#define EEProm_Init_IfEmpty     0
//#define EEProm_Init_SetDefault  1
  s_EEProm_Init(EEProm_Init_IfEmpty);

#ifndef NO_LCD
  // initialize display, cursor off
  lcd_init(LCD_DISP_ON);
#endif

  //  Initialize UART library
  uart_init( UART_BAUD_SELECT(UART_BAUD_RATE,F_CPU) ); 

  // now enable interrupt, since UART library is interrupt controlled
  sei();

  // initialize Timer/Counter 
	TimerCounter2_Init();	    // initialize as Counter
//  TimerCounter1_Init();
//  TimerCounter0_Init();


#ifdef DEBUG_STEPPING 
 wait_until_key_pressed(); 
#endif
	
 		
	BStatus.bRefreshMenu = 1;	
	BStatus.bStopClock = 0;
		
  ShowIntroUart0();
	ShowIntroLCD();
	

  fAlarmMode.AlarmAktiv = 0;    // Deaktiviert den Alarmmode
  fAlarmMode.Countdown  = 1;    // Aktiviert den Countdowns




    
    
#ifndef NO_LCD
    /* clear display and home cursor */
//    lcd_clrscr();
        
    /* put string to display (line 1) with linefeed */
//    lcd_puts("LCD Test Line 1");

        
    /* move cursor to position 8 on line 2 */
//    lcd_gotoxy(1,1);  

    /* cursor is now on second line, write second line */
//    lcd_puts("Line 2");

    /* move cursor to position 2 on line 3 */
//    lcd_gotoxy(1,2);  
    /* cursor is now on second line, write second line */
//    lcd_puts("Line 3");
        
    /* move cursor to position 2 on line 4 */
//    lcd_gotoxy(1,3);  
    /* cursor is now on second line, write second line */
//    lcd_puts("Line 4");
#endif
        





    /*
     *  Transmit string to UART
     *  The string is buffered by the uart library in a circular buffer
     *  and one character at a time is transmitted to the UART using interrupts.
     *  uart_puts() blocks if it can not write the whole string to the circular 
     *  buffer
     */
//    uart_puts("String stored in SRAM\r\n");
    
    /*
     * Transmit string from program memory to UART
     */
//    uart_puts_P("String stored in FLASH\r\n");
    
        
    /* 
     * Use standard avr-libc functions to convert numbers into string
     * before transmitting via UART
     */     
//    itoa( num, buffer, 10);   // convert interger into string (decimal format)         
//    uart_puts(buffer);        // and transmit string to UART

    
    /*
     * Transmit single character to UART
     */
//    uart_putc('\r');
    
    for(;;)
    {
      /*
       * Get received character from ringbuffer
       * uart_getc() returns in the lower byte the received character and 
       * in the higher byte (bitmask) the last receive error
       * UART_NO_DATA is returned when no data is available.
       *
       */
      c = uart_getc();
      if ( c & UART_NO_DATA )
      {
        /* 
         * no data available from UART 
         */
      }
      else
      {
        /*
         * new data available from UART
         * check for Frame or Overrun error
         */
        if ( c & UART_FRAME_ERROR )
        {
          /* Framing Error detected, i.e no stop bit detected */
          uart_puts_P("UART Frame Error: ");
        }
        if ( c & UART_OVERRUN_ERROR )
        {
          /* 
           * Overrun, a character already present in the UART UDR register was 
           * not read by the interrupt handler before the next character arrived,
           * one or more received characters have been dropped
           */
          uart_puts_P("UART Overrun Error: ");
        }
        if ( c & UART_BUFFER_OVERFLOW )
        {
          /* 
           * We are not reading the receive buffer fast enough,
           * one or more received character have been dropped 
           */
           uart_puts_P("Buffer overflow error: ");
        }
        /* 
         * send received character back
         */
        uart_putc( (unsigned char)c );
      }

			if (fPollingTime.b01000ms == 1)
			{
				fSendData.SendStatus=1;
				if (fAlarmMode.Countdown == 1) { s_Countdown(); }    // Aktiviert den Countdowns
			
				fPollingTime.b01000ms = 0;
			}

			if (fSendData.SendStatus == 1) 
			{
				s_SendStatus();

				fSendData.SendStatus=0;
			}

			
    }
    
}
// --------------------------------------------------------------------------
// --------------------------------------------------------------------------
// End of MAIN-Routine / End of Program
// --------------------------------------------------------------------------
// --------------------------------------------------------------------------
// **************************************************************************
// **************************************************************************
// **************************************************************************

