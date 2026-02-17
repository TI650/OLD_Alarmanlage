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
//#define NO_I2C
//#define NO_RTC
//#define NO_I2C_INPORT
//#define NO_I2C_SLAVE



  //Hardware Design
#define BOARD_AC128
//#define BOARD_AVRCTRL

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

#define UART_BAUD_RATE      19200         // 9600 baud 
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
#include "AC_STRINGS.c"

#include "uart.h"
#ifndef NO_I2C
	#include "i2cmaster.h"
	#include "scs_pcf8574.h"
#endif
#ifndef NO_RTC
	#include "scs_pcf8583.h"
#endif
#ifndef NO_I2C_SLAVE	
	#include "scs_m8slave.h"
#endif
#include "AC_DEFINES.c"

#include "AC_CONFIG.c"
#include "cmd_Buffer.c"

//#include "AC_MSG.c"
//#include "AC_MENUE.c"
#include "AC_SCREEN.c"


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







#ifndef NO_I2C_SLAVE	
 unsigned char	SlaveAddress = 62;
#endif
unsigned char	Buzz1_Cnt = 0; // = 255;
unsigned char	Buzz1_Cnt_Time = CMD_AC_Buzz1_Cnt_Time_ValStd; //= 100; // = 1;
unsigned char	Int_Siren_Cnt = 0;
unsigned char	Int_Siren_Cnt_Time ;
//#define Buzz1_Cnt_Time 3


// **************************************************************************
// **************************************************************************
// **************************************************************************
// --------------------------------------------------------------------------
// SUB-Routines and Functions
// --------------------------------------------------------------------------
//

//
// --------------------------------------------------------------------------
//
void s_BuzzerStart_PRE(void){
	BuzzerTast  = eeprom_read_byte(&eePreAlarm_BuzzerTast);
	/*	
	BuzzerLevelMain  = eeprom_read_byte(&eePreAlarm_BuzzerLevelMain);
	BuzzerLevelMin  = eeprom_read_byte(&eePreAlarm_BuzzerLevelMin)-BuzzerLevelMain;
	BuzzerLevelMax	=	eeprom_read_byte(&eePreAlarm_BuzzerLevelMax)-BuzzerLevelMain;		
	BuzzerIntervall = eeprom_read_byte(&eePreAlarm_BuzzerIntervall);
	PWM_DDR  |= (1<<PWM_PIN);
	PWM_PORT |= (1<<PWM_PIN); // = 0xFF;                  // set output low -> turn all LEDs on
	TCNT1 = 65535 - (F_CPU/64/BuzzerTast);
	TIMSK |= (1 << TOIE1);
	*/
	Int_Buzz1_PORT |= (1 << Int_Buzz1_PIN); // Int-Buzzer No1 On  
	Int_FLASH_PORT |= (1 << Int_FLASH_PIN); // Int-Flash On  
}
//
// --------------------------------------------------------------------------
//
void s_BuzzerReStart_PRE(void){
	/*
	PWM_DDR  |= (1<<PWM_PIN);
	PWM_PORT |= (1<<PWM_PIN); // = 0xFF;                  // set output low -> turn all LEDs on
	TCNT1 = 65535 - (F_CPU/64/BuzzerTast);
	TIMSK |= (1 << TOIE1);
	*/
	Int_Buzz1_PORT |= (1 << Int_Buzz1_PIN); // Int-Buzzer No1 On  
	Int_FLASH_PORT |= (1 << Int_FLASH_PIN); // Int-Flash On  
}
//
// --------------------------------------------------------------------------
//
void s_BuzzerStart_ALM(void){
	BuzzerTast  = eeprom_read_byte(&eeFullAlarm_BuzzerTast);
	/*
	BuzzerLevelMain  = eeprom_read_byte(&eeFullAlarm_BuzzerLevelMain);
	BuzzerLevelMin  = eeprom_read_byte(&eeFullAlarm_BuzzerLevelMin)-BuzzerLevelMain;
	BuzzerLevelMax	=	eeprom_read_byte(&eeFullAlarm_BuzzerLevelMax)-BuzzerLevelMain;		
	BuzzerIntervall = eeprom_read_byte(&eeFullAlarm_BuzzerIntervall);
	PWM_DDR  |= (1<<PWM_PIN);
	PWM_PORT |= (1<<PWM_PIN); // = 0xFF;                  // set output low -> turn all LEDs on
	TCNT1 = 65535 - (F_CPU/64/BuzzerTast);
	TIMSK |= (1 << TOIE1);
	*/
	Int_Siren_Cnt = 0;
	Int_Buzz1_PORT &=~(1 << Int_Buzz1_PIN);     // Int-Buzzer No1 Off  
	if (regCtrlMode.bInternal == 1) 
	{
		Int_FLASH_PORT &=~(1 << Int_FLASH_PIN);   // Int-Flash Off
		Int_SIREN_PORT |= (1 << Int_SIREN_PIN);   // Int-Siren On  
	}
	else
	{
		Int_Buzz1_PORT |= (1 << Int_Buzz1_PIN);   // Int-Buzzer No1 On  
		if (regCtrlMode.bIntNoise == 1) 
		{
			Int_FLASH_PORT |= (1 << Int_FLASH_PIN); // Int-Flash On
		}      
	}
	
	if (regCtrlMode.bExternal == 1) 
	{
		Ext_PWR_FLASH_PORT |= (1 << Ext_PWR_FLASH_PIN);   // Ext-Flash On
		Ext_AlmSig_PORT    |= (1 << Ext_AlmSig_PIN);      // Ext-Alarm-Signal On  
	} 
	else
	{
		if (regCtrlMode.bExtNoise == 1) 
		{
			Ext_PWR_FLASH_PORT |= (1 << Ext_PWR_FLASH_PIN); // Ext-Flash On
		}
	}
	
	if (regCtrlMode.bByGSM == 1) 
	{}     
	
//	if ((regCtrlMode.bInternal == 0) && (regCtrlMode.bExternal == 0))
//	{
//		Int_Buzz1_PORT |= (1 << Int_Buzz1_PIN); // Int-Buzzer No1 On  
//	}

//	Int_FLASH_PORT |= (1 << Int_FLASH_PIN); // Int-Flash On  
}
//
// --------------------------------------------------------------------------
//
void s_BuzzerStop(void){
  /*
	TIMSK &=~  (1<<TOIE1);
	PWM_PORT &=~ (1<<PWM_PIN);                  // set output low -> turn all LEDs on
	PWM_DDR &=~ (1<<PWM_PIN);        // Pin PA3 input      
  */

	Int_FLASH_PORT     &=~(1 << Int_FLASH_PIN);     // Int-Flash Off  
	Int_SIREN_PORT     &=~(1 << Int_SIREN_PIN);     // Int-Siren Off  
	Ext_AlmSig_PORT    &=~(1 << Ext_AlmSig_PIN);    // Ext-Alarm-Signal Off  
	Ext_PWR_SIREN_PORT &=~(1 << Ext_PWR_SIREN_PIN); // Ext-Siren PWR Off  
	Ext_PWR_FLASH_PORT &=~(1 << Ext_PWR_FLASH_PIN); // Ext-Flash PWR Off  
	Ext_PWR_PORT       &=~(1 << Ext_PWR_PIN);       // Ext-Power Off  
	Int_Buzz1_PORT     &=~(1 << Int_Buzz1_PIN);     // Int-Buzzer No1 Off  
	Int_Buzz2_PORT     &=~(1 << Int_Buzz2_PIN);     // Int-Buzzer No2 Off  

}
//
// --------------------------------------------------------------------------
//
void s_SendIntegerToUart(int value){
  char buffer[6];
	
  itoa(value, buffer, 10); 
	uart_puts(buffer); 

}
//
// --------------------------------------------------------------------------
//
void s_SendIntegerToLCD(int value){
  char buffer[6];
	
  itoa(value, buffer, 10); 
	lcd_puts(buffer); 

}
//
// --------------------------------------------------------------------------
//
void s_SendMessage(unsigned int MsgNumber){
	uart_puts("$ACMSG");              // Protokollkennung
	                            uart_putc(DataSeperator); 
	uart_puts("AC ");                 // Controller ID
  uart_puts(AC_MSG_CODE[MsgNumber]);
	                            uart_putc(DataSeperator);
	uart_puts("*EA\r");                // Checksum
}
//
// --------------------------------------------------------------------------
//
unsigned char f_SendDateTime(void) {
//	unsigned char     i;
	unsigned char   crc;
	
  IntToStringformat(sDTG.Day, 2, 0);
	uart_puts(CharBuffer); uart_putc('.');
  IntToStringformat(sDTG.Month, 2, 0);
	uart_puts(CharBuffer); uart_putc('.');
  IntToStringformat(sDTG.Year, 2, 0);
	uart_puts(CharBuffer); uart_putc(' ');

  IntToStringformat(sDTG.Hour, 2, 0);
	uart_puts(CharBuffer); uart_putc(':');
  IntToStringformat(sDTG.Min, 2, 0);
	uart_puts(CharBuffer); uart_putc(':');
	IntToStringformat(sDTG.Sec,2,0);
 	uart_puts(CharBuffer); uart_putc(DataSeperator);  // Datum / Uhrzeit

	crc = 128;

	return crc;
}
//
// --------------------------------------------------------------------------
//
void s_SendStatusMain(void) {
	unsigned char   i;
	unsigned char   crc;
	
	uart_puts("$ACSTA"); uart_putc(DataSeperator);             // Protokollkennung
 
	crc =+ f_SendDateTime();

	s_SendIntegerToUart(regAlarmStatus.bAlarmArmed); uart_putc(DataSeperator);
	s_SendIntegerToUart(regAlarmStatus.bPreAlarm); uart_putc(DataSeperator);
	s_SendIntegerToUart(regAlarmStatus.bAlarmExecute); uart_putc(DataSeperator);
	s_SendIntegerToUart(regCtrlMode.bRemote); uart_putc(DataSeperator);
	s_SendIntegerToUart(regCtrlMode.bInternal); uart_putc(DataSeperator);
	s_SendIntegerToUart(regCtrlMode.bExternal); uart_putc(DataSeperator);
	s_SendIntegerToUart(regCtrlMode.bIntNoise); uart_putc(DataSeperator);
	s_SendIntegerToUart(regCtrlMode.bExtNoise); uart_putc(DataSeperator);
	s_SendIntegerToUart(regCtrlMode.bByGSM); uart_putc(DataSeperator);

	uart_putc('0'); uart_putc(DataSeperator); // SPARE

//	regPortActual[4] = pcf8574_get_port(InPortAdr[0]);     // 0x40);     // PINA;      //  0xFF;     // 
//	regPortActual[5] = pcf8574_get_port(InPortAdr[1]);     // 0x42);     // PINB;      //  
//	regPortActual[6] = pcf8574_get_port(InPortAdr[2]);     // 0x44);     // 0xFF;     // PINC;      //  
//	regPortActual[7] = pcf8574_get_port(InPortAdr[3]);     // 0x46);     // 0xFF;     // PIND;      //  

  
  for (i=0;i<8;i++)
	{
	  s_SendIntegerToUart(regPortActual[i]); uart_putc(DataSeperator); // Port 1 - Türen
	}                            
  
  uart_puts("*EA\r");                      // Checksum

}//
// --------------------------------------------------------------------------
//
void s_AlarmAbort(void){ // Deaktiviert die Sequenz zum Scharfschalten
	if (regAlarmStatus.bAlarmArmed == 0) 
	{ 
	  regAlarmStatus.bCountdown = 0;  // Stoppen des Countdowns
	  vCountdown = 255; 
		s_SendMessage(AC_MSGNO_AKTIVIERUNG_ABGEBROCHEN);
	}
}
//
// --------------------------------------------------------------------------
//
void s_AlarmArmed(void){ // Aktiviert die Sequenz zum Scharfschalten
  regAlarmStatus.bCountdown  = 1; // Aktiviert den Countdowns
	s_SendMessage(AC_MSGNO_AKTIVIERUNG_GESTARTET);
}
//
// --------------------------------------------------------------------------
//
void s_AlarmInert(void){ // Deaktiviert die Scharfschaltung
	regAlarmStatus.bCountdown      = 0;  // Stoppen des Countdowns
	regAlarmStatus.bAlarmArmed     = 0;  // Deaktivieren der Alarmanlage
	regAlarmStatus.bPreAlarm       = 0;  // Voralarm zurücksetzen
	regAlarmStatus.bAlarmExecute   = 0;  // Alarm Auslösung zurücksetzen
  vCountdown = 255; 
	s_BuzzerStop();
//	Ext_PWR_SIREN_PORT &=~(1 << Ext_PWR_SIREN_PIN); // Ext-Siren PWR Off  
	s_SendMessage(AC_MSGNO_DEAKTIVIERUNG);
}

//
// --------------------------------------------------------------------------
//
void s_AlarmCheckPortStatus(void) {
	static unsigned int Status;
	static unsigned char DebounceCnt = 0;
	unsigned char i;
	unsigned char DebounceCntBuf;
	unsigned int StatusBuf;

	if (regAlarmStatus.bAlarmArmed == 1)
	{
		if ((regAlarmStatus.bPreAlarm == 0) && (regAlarmStatus.bAlarmExecute == 0))
		{
			DebounceCntBuf = 0;	
			for (i=1;i<8;i++)
			{
				if (regPortActual[i] != regPortStored[i])
				{
					DebounceCntBuf++;
					if (DebounceCnt == 10)
					{
						regAlarmStatus.bCountdown    = 0;  // Stoppen des Countdowns
						vCountdown = 255; 
						regAlarmStatus.bPreAlarm     = 0;  // Voralarm zurücksetzen
						regAlarmStatus.bAlarmExecute = 1;  // Alarm Auslösung aktivieren
						s_BuzzerStart_ALM();
						s_SendMessage(AC_MSGNO_ALARMIERUNG_AUSGELOEST);
						s_SendStatusMain(); 
					}
				}
			}
		
			if (regPortActual[0] != regPortStored[0])
			{
				DebounceCntBuf++;
				if (DebounceCnt == 10)
				{
					regAlarmStatus.bPreAlarm = 1;
					regAlarmStatus.bCountdown  = 1; // Aktiviert den Countdowns
					s_BuzzerStart_PRE();
					s_SendMessage(AC_MSGNO_VORALARM_AUSGELOEST);
					s_SendStatusMain(); 
				}
				
			} 
			
			if (DebounceCntBuf != 0)
			{
				DebounceCnt++;
			}
			else
			{
				DebounceCnt = 0;			
			}
			
			
		} 	
	}
	else
	{
		StatusBuf = 0;
		for (i=0;i<8;i++)
		{
			StatusBuf += regPortActual[i];
		}
		if (StatusBuf != Status)
		{	
		  Int_Buzz1_PORT |= (1 << Int_Buzz1_PIN); // Int-Buzzer No1 On  
		  //Buzz1_Cnt = 0;
		  Buzz1_Cnt = Buzz1_Cnt_Time;
		 
		}
		Status = StatusBuf;
	}		
}
//
// --------------------------------------------------------------------------
//
void s_Uart0_Read(void) {
	unsigned int zeichen;

	zeichen = uart_getc();
	if ( zeichen & UART_NO_DATA )
	{
		// no data available from UART 
	}
	else
	{

		switch (zeichen)
		{ 
		  case 's':   s_AlarmArmed(); break;  // Alarmanlage scharfschalten  

		  case 'i':   s_AlarmInert(); break;  // Deaktiviert die Scharfschaltung

		  case 'a':   s_AlarmAbort(); break;  // Deaktiviert die Sequenz zum Scharfschalten

		  case 'd':   regSendData.bSendPortDef=1; break;

  // nur zu Testzwecken		  
		  case 'h':   regSendData.bSendStatus=1; break;  // 


		  case 'W':   Int_FLASH_PORT |= (1 << Int_FLASH_PIN); break;  // Int-Flash On  
		  case 'w':   Int_FLASH_PORT &=~(1 << Int_FLASH_PIN); break;  // Int-Flash Off  

		  case 'Q':   Int_SIREN_PORT |= (1 << Int_SIREN_PIN); break;  // Int-Siren On  
		  case 'q':   Int_SIREN_PORT &=~(1 << Int_SIREN_PIN); break;  // Int-Siren Off  

		  case 'Z':   Ext_AlmSig_PORT |= (1 << Ext_AlmSig_PIN); break;  // Ext-Alarm-Signal On  
		  case 'z':   Ext_AlmSig_PORT &=~(1 << Ext_AlmSig_PIN); break;  // Ext-Alarm-Signal Off  

		  case 'T':   Ext_PWR_SIREN_PORT |= (1 << Ext_PWR_SIREN_PIN); break;  // Ext-Siren PWR On  
		  case 't':   Ext_PWR_SIREN_PORT &=~(1 << Ext_PWR_SIREN_PIN); break;  // Ext-Siren PWR Off  

		  case 'E':   Ext_PWR_FLASH_PORT |= (1 << Ext_PWR_FLASH_PIN); break;  // Ext-Flash PWR On  
		  case 'e':   Ext_PWR_FLASH_PORT &=~(1 << Ext_PWR_FLASH_PIN); break;  // Ext-Flash PWR Off  

		  case 'R':   Ext_PWR_PORT |= (1 << Ext_PWR_PIN); break;  // Ext-Power On  
		  case 'r':   Ext_PWR_PORT &=~(1 << Ext_PWR_PIN); break;  // Ext-Power Off  

		  case 'U':   Int_Buzz1_PORT |= (1 << Int_Buzz1_PIN); break;  // Int-Buzzer No1 On  
		  case 'u':   Int_Buzz1_PORT &=~(1 << Int_Buzz1_PIN); break;  // Int-Buzzer No1 Off  

		  case 'O':   Int_Buzz2_PORT |= (1 << Int_Buzz2_PIN); break;  // Int-Buzzer No2 On  
		  case 'o':   Int_Buzz2_PORT &=~(1 << Int_Buzz2_PIN); break;  // Int-Buzzer No2 Off  

		  case 'F':   regCtrlMode.bInternal = 1; break;  
		  case 'f':   regCtrlMode.bInternal = 0; break;  

		  case 'G':   regCtrlMode.bExternal = 1; break;  
		  case 'g':   regCtrlMode.bExternal = 0; break;  

		  case 'V':   regCtrlMode.bIntNoise = 1; break;  
		  case 'v':   regCtrlMode.bIntNoise = 0; break;  

		  case 'J':   regCtrlMode.bExtNoise = 1; break;  
		  case 'j':   regCtrlMode.bExtNoise = 0; break;  

		  case 'K':   regCtrlMode.bByGSM = 1; break;  
		  case 'k':   regCtrlMode.bByGSM = 0; break;  
 
 
/*			case 'P':   s_BuzzerStart_PRE(); 
									regAlarmStatus.bPreAlarm     = 1;    // Voralarm ausgelöst = 1 / 
									lcd_gotoxy(0,0);
									lcd_puts("P=1");
									Data2LCD();
									break;  

			case 'A':   s_BuzzerStart_ALM(); 
									regAlarmStatus.bAlarmExecute = 1;    // Alarm ausgelöst    = 1 /  
									lcd_gotoxy(0,0);
									lcd_puts("A=1");
									Data2LCD();
									break;  // 

			case 'S':   s_BuzzerStop(); 
									regAlarmStatus.bPreAlarm     = 0;    // Voralarm ausgelöst = 1 / 
									regAlarmStatus.bAlarmExecute = 0;    // Alarm ausgelöst    = 1 /  
									lcd_gotoxy(0,0);
									lcd_puts("S=0 ");
									Data2LCD();
									break;  // 
*/
			case '-':   
									BuzzerLevelMain++;
									BuzzerLevelMin--;  // -= BuzzerLevelMain;
									BuzzerLevelMax--;  // -= BuzzerLevelMain;		
//									Data2LCD();
									break;  // 

			case '+':   
									BuzzerLevelMain--;
									BuzzerLevelMin++;
									BuzzerLevelMax++;		
//									Data2LCD();
									break;  // 

		} 

			s_Cmd_PutChar2Buffer(zeichen);


//		uart_putc( (unsigned char)zeichen );

	}

}
//
// --------------------------------------------------------------------------
//
void ShowIntroUart0 (void) {
	uart_puts(AC_IntroUART_ProgNameVersion);
	s_SendMessage(AC_MSGNO_SYSTEM_GESTARTET);
}
//
// --------------------------------------------------------------------------
//
void ShowIntroLCD (void) {
#ifndef NO_LCD
  lcd_clrscr();                 // clear display and home cursor  
	lcd_puts(AC_IntroLCD_ProgNameVersion); // put string to display  
	lcd_gotoxy(1,1);              // move cursor to position 1 on line 2  
	lcd_puts(AC_IntroLCD_COPYRIGHT);   // put string to display
#endif  
}
//
// --------------------------------------------------------------------------
//
//void ShowMenueMain (void) {
//	ClearScreen();                     
//  GLCD_DrawFrameMain();
//}
//
// --------------------------------------------------------------------------
//
/*void View2LCD_DTG(void) {
//  char buffer[2];

#ifndef NO_RTC
  unsigned char data;
  pcf8583_get_Date(RTC_Adress, &sDTG.Weekday, &sDTG.Day, &sDTG.Month, &sDTG.Year);
  lcd_clrscr();
  lcd_gotoxy(1,0);
  lcd_puts (Day[sDTG.Weekday]);
  lcd_putc (' ');
#endif  
  
#ifndef NO_RTC
  data =  pcf8583_get_weekday(RTC_Adress);  
#endif

#ifndef NO_LCD
//  lcd_gotoxy(1,0);
//  lcd_puts (Day[data]);
//  lcd_putc (' ');

  IntToStringformat(sDTG.Day, 2, 0);
  lcd_puts(CharBuffer);
  lcd_putc ('.');

  IntToStringformat(sDTG.Month, 2, 0);
  lcd_puts(CharBuffer);
  lcd_puts (".20");


  IntToStringformat(sDTG.Year, 2, 0);
  lcd_puts(CharBuffer);
//  lcd_putc ('.');
#endif  


//  lcd_gotoxy(4,1);
//  lcd_puts ("        ");
#ifndef NO_RTC
  pcf8583_get_Time(RTC_Adress, &sDTG.Hour, &sDTG.Min, &sDTG.Sec);
#endif
  
#ifndef NO_LCD
  lcd_gotoxy(4,1);
  IntToStringformat(sDTG.Hour, 2, 0);
  lcd_puts(CharBuffer);
  lcd_putc (':');
	
  IntToStringformat(sDTG.Min, 2, 0);
  lcd_puts(CharBuffer);
  lcd_putc (':');
	
  IntToStringformat(sDTG.Sec, 2, 0);
  lcd_puts(CharBuffer);
//  lcd_putc (',');
#endif  
	
//  data =  pcf8583_get_hsec(0xa0); 
//  IntToStringformat(data, 2, 0);
//  lcd_puts(CharBuffer);
	
}
*/
//
/// --------------------------------------------------------------------------
//
void Refresh_DTG (void) {
#ifdef NO_RTC
	sDTG.Sec += 1;         
	if(sDTG.Sec == 60)
	{
		sDTG.Sec  = 0;
		sDTG.Min += 1;
		if(sDTG.Min == 60)
		{	
			sDTG.Min   = 0;
			sDTG.Hour += 1;
		}	
		if(sDTG.Hour == 24)
		{	
			sDTG.Hour  = 0;
			sDTG.Day  += 1;
			sDTG.Weekday += 1;
			if(sDTG.Weekday == 7)
			{	
				sDTG.Weekday  = 0;
			}	
		}	
		if(sDTG.Day == 32)
		{	
			sDTG.Day     = 1;
			sDTG.Month  += 1;
		}	
		if(sDTG.Month == 13)
		{	
			sDTG.Month  = 1;
			sDTG.Year  += 1;
		}	
	}
#endif
#ifndef NO_RTC
  pcf8583_get_Date(RTC_Adress, &sDTG.Weekday, &sDTG.Day, &sDTG.Month, &sDTG.Year);
  pcf8583_get_Time(RTC_Adress, &sDTG.Hour, &sDTG.Min, &sDTG.Sec);
#endif
}
//
// --------------------------------------------------------------------------
//
//
// --------------------------------------------------------------------------
//
void s_GetInterfacePorts(void) {

#ifdef BOARD_AVRCTRL
	#warning "Set: BOARD_AVRCTRL - Interfacedefinition"

	#ifdef BOARD_AVRCTRL_TEST
		InPortVal[0] = (PINA & 0b11110000);     //PINA;      //  
		InPortVal[1] = (PINB & 0b00000000);     //PINB;      //  
		InPortVal[2] = (PINC & 0b00000000);     //PINC;      //  
		InPortVal[3] = (PIND & 0b00000000);     // PIND;      //  

		InPortVal[4] =  0x00; // 
		InPortVal[5] =  0x00; // 
		InPortVal[6] =  0x00; // 
		InPortVal[7] =  0x00; //
	#else
		InPortVal[0] = (PINA & 0b11110000);     //PINA;      //  
		InPortVal[1] = (PINB & 0b00000000);     //PINB;      //  
		InPortVal[2] = (PINC & 0b00000000);     //PINC;      //  
		InPortVal[3] = (PIND & 0b00000000);     // PIND;      //  

		InPortVal[4] =  0x00; // 
		InPortVal[5] =  0x00; // 
		InPortVal[6] =  0x00; // 
		InPortVal[7] =  0x00; //
	#endif




#endif


#ifdef BOARD_AC128
	#warning "Set: BOARD_AC128 - Interfacedefinition"

#ifndef NO_I2C_INPORT
	InPortVal[0] = pcf8574_get_port(InPortAdr[0]);     // 0x40);     // PINA;      //  0xFF;     // 
	InPortVal[1] = pcf8574_get_port(InPortAdr[1]);     // 0x42);     // PINB;      //  
	InPortVal[2] = pcf8574_get_port(InPortAdr[2]);     // 0x44);     // 0xFF;     // PINC;      //  
	InPortVal[3] = pcf8574_get_port(InPortAdr[3]);     // 0x46);     // 0xFF;     // PIND;      //  
#else
	InPortVal[0] =  0x00; //40;//0xAA;      //  
	InPortVal[1] =  0x00;//0x22;      //  
	InPortVal[2] =  0x00;//0xCC;      //  
	InPortVal[3] =  0x00;//0xEE;      //  
#endif

	InPortVal[4] =  0x00; //40;//0xAA;      //  
	InPortVal[5] =  0x00;//0x22;      //  
	InPortVal[6] =  0x00;//0xCC;      //  
	InPortVal[7] =  0x00;//0xEE;      //  

#endif


}
//
// --------------------------------------------------------------------------
//


//
// --------------------------------------------------------------------------
//
unsigned int Read_ADC_Channel(unsigned char channel) {
  ADMUX=channel;
 
	ADCSRA |= (1<<ADEN);  // ADC aktivieren (2)

	ADCSRA |= (1<<ADSC); // eine ADC-Wandlung
	while ( ADCSRA & (1<<ADSC) ) 
	{
	  ; // auf Abschluss der Konvertierung warten
	}

	ADCSRA |= (1<<ADSC); // eine Wandlung "single conversion"
	while ( ADCSRA & (1<<ADSC) ) 
	{
		; // auf Abschluss der Konvertierung warten
	}

	ADCSRA &= ~(1<<ADEN); // ADC deaktivieren (2)

// ADCSRA.6=1;
// while (ADCSRA.4==0);
// ADCSRA.4=1;

 return ((ADCW*25)/4)-2250 ;

}
//
// --------------------------------------------------------------------------
//

void GetTemperatur(unsigned char Start) {
#ifndef NO_I2C_SLAVE	
  signed int     tmp_int; 
  signed int     tmp_buf; 
//	int16_t	      	value;
	unsigned char   i;
	
  for (i=0;i<4;i++)
	{
		tmp_int = m8slave_get_adc(SlaveAddress, i);
		tmp_buf = ((tmp_int*25)/4)-2250;
	
		tmp_int = tmp_buf + SensorKorrWert[i];
		tmp_buf = SensorIstWert[i];
		if (SensorIstWert[i] > tmp_int)
		{
			SensorIstWert[i]--;
			}  
		else 
		{
			if (SensorIstWert[i] < tmp_int) SensorIstWert[i]++;
		}
		if(Start) SensorIstWert[i] = tmp_int;
//		SensorIstWert[i] += tmp_buf;
	
	
//	  value = m8slave_get_adc(SlaveAddress, i);
//    SensorIstWert[i] = ((value*25)/4)-2250;    
//    SensorIstWert[i] += SensorKorrWert[i];    
	}                            

#else
  signed int     tmp_int; 
  signed int     tmp_buf; 
	unsigned char   i;
 
  for (i=0;i<4;i++)
	{
		tmp_buf = SensorIstWert[i];
		tmp_int = Read_ADC_Channel(i) + SensorKorrWert[i];
		if (SensorIstWert[i] > tmp_int)
		{
			SensorIstWert[i]--;
			}  
		else 
		{
			if (SensorIstWert[i] < tmp_int) SensorIstWert[i]++;
		}
		if(Start) SensorIstWert[i] = tmp_int;
//		SensorIstWert[i] += tmp_buf;
	}                            
	
/*
	tmp_buf = IstTempAussen;
	tmp_int = Read_ADC_Channel(ADC_TA) + SensorKorrWert_TA;
	if (IstTempAussen > tmp_int)
	{
		IstTempAussen--;
		}  
	else 
	{
		if (IstTempAussen < tmp_int) IstTempAussen++;
	}
	if(Start) IstTempAussen = tmp_int;
	IstTempAussen += tmp_buf;

	tmp_buf = IstTempInnen;
	tmp_int = Read_ADC_Channel(ADC_TI) + SensorKorrWert_TI;
	if (IstTempInnen > tmp_int) 
	{     
		IstTempInnen--;
	}
	else 
	{
		if (IstTempInnen < tmp_int) IstTempInnen++;
	}
	if(Start) IstTempInnen = tmp_int;
	IstTempInnen += tmp_buf;
*/

#endif
}
//
// --------------------------------------------------------------------------
//
void s_GetTastaturt(void) {
//	if ( TASTE_UP_PIN & (1<<TASTE_UP_PNO) ) 
//	{
//		LED03_PORT |=  (1 << LED03_PNO);        /* Pin high    */
//	}
//	else
//	{
//		LED03_PORT &= ~(1 << LED03_PNO);        /* Pin high    */
//	}

//	if ( TASTE_DWN_PIN & (1<<TASTE_DWN_PNO) ) 
//	{
//		LED03_PORT |=  (1 << LED03_PNO);        /* Pin high    */
//	}
//	else
//	{
//		LED03_PORT &= ~(1 << LED03_PNO);        /* Pin high    */
//	}

//	if ( TASTE_RET_PIN & (1<<TASTE_RET_PNO) ) 
//	{
//		LED03_PORT |=  (1 << LED03_PNO);        /* Pin high    */
//	}
//	else
//	{
//		LED03_PORT &= ~(1 << LED03_PNO);        /* Pin high    */
//	}

	if ( SWITCH_ALARM_PIN & (1<<SWITCH_ALARM_PNO) ) 
	{
	  regUserControl.bSwitchArmed = 1;
		LED_Armed_PORT |=  (1 << LED_Armed_PNO);        /* Pin high    */
	}
	else
	{
	  regUserControl.bSwitchArmed = 0;
		LED_Armed_PORT &= ~(1 << LED_Armed_PNO);        /* Pin high    */
	}

	if ( SWITCH_HAND_PIN & (1<<SWITCH_HAND_PNO) ) 
	{
		regUserControl.bSwitchRemote = 1;
		LED_ModeHand_PORT |=  (1 << LED_ModeHand_PNO);        /* Pin high    */
	}
	else
	{
		regUserControl.bSwitchRemote = 0;
		LED_ModeHand_PORT &= ~(1 << LED_ModeHand_PNO);        /* Pin high    */
	}

}
// ---------------------------------------------------------------------------------------
#define CNT_TASTE 7

void Tastatur(void) {

 static char taste_up = 0,taste_dwn = 0,taste_ret = 0;  //,taste10 = 0;

 TASTE = NICHTS;
  
	if (TASTE_UP_PIN & (1<<TASTE_UP_PNO)) 
	{
//		LED03_PORT |=  (1 << LED03_PNO);        /* Pin high    */
    if(taste_up++ == 0 || taste_up == CNT_TASTE) TASTE = UP;  
    if(taste_up == CNT_TASTE) taste_up = 3;
  } 
  else taste_up = 0;

  
	if (TASTE_DWN_PIN & (1<<TASTE_DWN_PNO)) 
	{
//		LED03_PORT |=  (1 << LED03_PNO);        /* Pin high    */
    if(taste_dwn++ == 0 || taste_dwn == CNT_TASTE) TASTE = DOWN;  
    if(taste_dwn == CNT_TASTE) taste_dwn = 3;
  } 
  else taste_dwn = 0;

  
	if (TASTE_RET_PIN & (1<<TASTE_RET_PNO)) 
	{
//		LED03_PORT |=  (1 << LED03_PNO);        /* Pin high    */
    if(taste_ret++ == 0 || taste_ret == CNT_TASTE) TASTE = RETURN;  
    if(taste_ret == CNT_TASTE) taste_ret = 3;
  } 
  else taste_ret = 0;

// 
//    if(TASTE1) 
//    { 
//      if(taste1++ == 0 || taste1 == CNT_TASTE) Taste = DOWN;    
//      if(taste1 == CNT_TASTE) taste1 = 3;
//    } else taste1 = 0;
//    
//    if(TASTE2) 
//    { 
//      if(taste2++ == 0 || taste2 == CNT_TASTE) Taste = UP;  
//      if(taste2 == CNT_TASTE) taste2 = 3;
//    } else taste2 = 0;
//    
//    if(TASTE3) { if(taste3++ == 0) Taste = OKAY;} else taste3 = 0;
//    if(TASTE4) { if(taste4++ == 0) Taste = MODE;} else taste4 = 0;


	if ( SWITCH_HAND_PIN & (1<<SWITCH_HAND_PNO) ) 
	{
		regUserControl.bSwitchRemote = 1;
//		LED02_PORT |=  (1 << LED02_PNO);        /* Pin high    */
	}
	else
	{
		regUserControl.bSwitchRemote = 0;
//		LED02_PORT &= ~(1 << LED02_PNO);        /* Pin high    */
	}

	if ( SWITCH_ALARM_PIN & (1<<SWITCH_ALARM_PNO) ) 
	{
	  regUserControl.bSwitchArmed = 1;
		//regUserControl.bSwitchRemote = 1;
//		LED01_PORT |=  (1 << LED01_PNO);        /* Pin high    */
	}
	else
	{
	  regUserControl.bSwitchArmed = 0;
		//regUserControl.bSwitchRemote = 0;
//		LED01_PORT &= ~(1 << LED01_PNO);        /* Pin high    */
	}



}                   
//
// --------------------------------------------------------------------------
//

void s_TestRemoteMode(void){
	if ( regUserControl.bSwitchRemote == 1 ) 
	{
		if ( regUserControl.bSwitchArmed == 1 ) 
		{
			if ( regCtrlMode.bRemote == 0 ) 
			{
				regCtrlMode.bRemote = 1;
				Int_Buzz1_PORT &=~(1 << Int_Buzz1_PIN);  // Int-Buzzer No1 Off  
				s_SendMessage(AC_MSGNO_REMOTE_EIN);
			}
		}
		else 
		{
			regCtrlMode.bRemote = 0;
		  Int_Buzz1_PORT |= (1 << Int_Buzz1_PIN);  // Int-Buzzer No1 On  
			lcd_gotoxy(0,3);
			lcd_puts ("Err: Remotebetr.");
		}
	}
	else
	{ 
		if ( regCtrlMode.bRemote == 1 ) 
		{
			regCtrlMode.bRemote = 0;
			s_SendMessage(AC_MSGNO_REMOTE_AUS);
			Int_Buzz1_PORT &=~(1 << Int_Buzz1_PIN);  // Int-Buzzer No1 Off  
		}
	}




}                   

//
// --------------------------------------------------------------------------
//
void s_SendPortDefRoot(void) {
	unsigned char Port;
	unsigned char Bit;
	unsigned char InPort;
	unsigned char InBit;
	unsigned char PtrToTable;
	
  for (Port=0;Port<8;Port++)
	{
		PtrToTable = Port * 8;
		for (Bit=0;Bit<8;Bit++)
		{
			if (InPortRootingTable[PtrToTable + Bit] != 0xFF);
			{
				InPort = InPortRootingTable[PtrToTable + Bit];
				InPort = (InPort && 0b11110000) / 16;
				InBit = InPortRootingTable[PtrToTable + Bit];
				InBit = InBit && 0b00001111;
				
				uart_puts("$ACROT"); uart_putc(DataSeperator);             // Protokollkennung
	      s_SendIntegerToUart(PtrToTable + Bit); uart_putc(DataSeperator); //  
	      s_SendIntegerToUart(Port); uart_putc(DataSeperator); // 
	      s_SendIntegerToUart(Bit); uart_putc(DataSeperator); // 
	      s_SendIntegerToUart(InPort); uart_putc(DataSeperator); // 
	      s_SendIntegerToUart(InBit); uart_putc(DataSeperator); // 
	      s_SendIntegerToUart(InPortRootingTable[PtrToTable + Bit]); uart_putc(DataSeperator); // 
				uart_puts("*EA\r");                      // Checksum
			}
		}                            
	}                            

}
//
// --------------------------------------------------------------------------
//
void s_SendStatusCountdown(void) {
//  char buffer[2];

	uart_puts("$ACCNT"); uart_putc(DataSeperator);  // Protokollkennung

	s_SendIntegerToUart(regAlarmStatus.bAlarmArmed); uart_putc(DataSeperator);
	s_SendIntegerToUart(regAlarmStatus.bPreAlarm); uart_putc(DataSeperator);
	s_SendIntegerToUart(regAlarmStatus.bAlarmExecute); uart_putc(DataSeperator);
	s_SendIntegerToUart(vCountdown); uart_putc(DataSeperator);

	uart_puts("*EA\r");              // Checksum

}
//
// --------------------------------------------------------------------------
//
void s_SendInfoHardware(void){
	uart_puts("$ACHW1;");              // Protokollkennung
	uart_puts(AC_MCU);           // Integrierter MC
	uart_putc(DataSeperator);
	uart_puts(AC_FCPU);            // MC-Frequenz in Hz
	uart_putc(DataSeperator);
	uart_puts(AC_HwVers);               // Boardversion
	uart_putc(DataSeperator);
	uart_puts(AC_SwVers);            // Diverse Daten
	uart_putc(DataSeperator);
	uart_puts(AC_CompileDate);            // Diverse Daten
	uart_putc(DataSeperator);
	uart_puts("*EA\r");                // Checksum
} //AC_FW_M128_SW02.1a_200806241200
//
// --------------------------------------------------------------------------
//
void s_KL_SendStatus(void){
	unsigned char   crc;
	unsigned char   i;

	uart_puts("$KLSTA"); uart_putc(DataSeperator);  // Protokollkennung
  
	crc =+ f_SendDateTime();

  for (i=0;i<4;i++)
	{
	  s_SendIntegerToUart(SensorIstWert[i]); uart_putc(DataSeperator);    
	}                            
//	s_SendIntegerToUart(IstTempAussen); uart_putc(DataSeperator);
//	s_SendIntegerToUart(IstTempInnen); uart_putc(DataSeperator);
//	s_SendIntegerToUart(IstTemp_CH02); uart_putc(DataSeperator);
//	s_SendIntegerToUart(IstTemp_CH03); uart_putc(DataSeperator);
	uart_puts("*EA\r");              // Checksum
	
}
//
// --------------------------------------------------------------------------
//
void s_KL_SendStatus_old(void){
	int16_t		      value;
	unsigned char   crc;

	uart_puts("$KLSTA"); uart_putc(DataSeperator);  // Protokollkennung
  
	crc =+ f_SendDateTime();

	value = m8slave_get_adc(SlaveAddress, 0); 
	s_SendIntegerToUart(((value*25)/4)-2250); uart_putc(DataSeperator);
	value = m8slave_get_adc(SlaveAddress, 1); 
	s_SendIntegerToUart(((value*25)/4)-2250); uart_putc(DataSeperator);
	value = m8slave_get_adc(SlaveAddress, 2); 
	s_SendIntegerToUart(((value*25)/4)-2250); uart_putc(DataSeperator);
	value = m8slave_get_adc(SlaveAddress, 3); 
	s_SendIntegerToUart(((value*25)/4)-2250); uart_putc(DataSeperator);
	uart_puts("*EA\r");              // Checksum

}
//
// --------------------------------------------------------------------------
//
/*
void s_SendStatusI2C_Slave(void){
	int16_t		value;
	uint8_t		data;

	value = m8slave_get_adc(SlaveAddress, 0);
	uart_puts("ADC CH0: ");
	s_SendIntegerToUart(((value*25)/4)-2250);
	uart_puts("\n\r");


	value = m8slave_get_adc(SlaveAddress, 1);
	uart_puts("ADC CH1: ");
	s_SendIntegerToUart(value);
	uart_puts("\n\r");

	value = m8slave_get_adc(SlaveAddress, 2);
	uart_puts("ADC CH2: ");
	s_SendIntegerToUart(value);
	uart_puts("\n\r");

	value = m8slave_get_adc(SlaveAddress, 3);
	uart_puts("ADC CH3: ");
	s_SendIntegerToUart(value);
	uart_puts("\n\r");

	data = m8slave_read_eeprom(SlaveAddress, 99);
	uart_puts("eeProm adr 99 : ");
	s_SendIntegerToUart(data);
	uart_puts("\n\r");

	data = m8slave_get_pind(SlaveAddress, 5);
	uart_puts("Port D5 : ");
	s_SendIntegerToUart(data);
	uart_puts("\n\r");

	data = m8slave_get_pind(SlaveAddress, 6);
	uart_puts("Port D6 : ");
	s_SendIntegerToUart(data);
	uart_puts("\n\r");

	data = m8slave_get_pind(SlaveAddress, 7);
	uart_puts("Port D7 : ");
	s_SendIntegerToUart(data);
	uart_puts("\n\r");

	uart_puts("\n\r");
 
						 //  Initialize ADC-Chanals on m8slave
////							m8slave_set_adc(SlaveAddress, 0, adc_on);
////							m8slave_set_adc(SlaveAddress, 1, adc_on);
////							m8slave_set_adc(SlaveAddress, 2, adc_off);
////							m8slave_set_adc(SlaveAddress, 3, adc_on);
////
}
*/
//
// --------------------------------------------------------------------------
//
void s_SendInfoSoftware(void){
	uart_puts("$ACSW1;");              // Protokollkennung
	uart_puts(AC_SwVers);               // Softwareversion
	uart_putc(DataSeperator);
	uart_puts(AC_PrVers);               // Protokollversion
	uart_putc(DataSeperator);
	uart_puts("xx;xx;xx;");            // Diverse Daten
	uart_puts("*EA\r");                // Checksum
}
//
// --------------------------------------------------------------------------
//
void s_WriteActivationDataToEeprom(void) {
	unsigned char   i;
	//unsigned char   buf = 0;
	
  for (i=0;i<8;i++)
	{
    regPortStored[i] = regPortActual[i] ;    
		eeprom_write_byte(&eeInPortVal[i],regPortActual[i]);	
	}                            

}
//
// --------------------------------------------------------------------------
//
void s_ReadStartDataFromEeprom(void) {
	unsigned char   i;
//	unsigned char   buf = 0;
	
  for (i=0;i<8;i++)
	{
		regPortActual[i] = eeprom_read_byte(&eeInPortVal[i]);	
		InPortAdr[i] = eeprom_read_byte(&eeInPortAdr[i]);	
	}                            

  for (i=0;i<64;i++)
	{
		InPortRootingTable[i] = eeprom_read_byte(&eeInPortRootingTable[i]);	
	}                            

  for (i=0;i<4;i++)
	{
		SensorKorrWert[i] = eeprom_read_word(&eeSensorKorrWert[i]);	
	}                            

	Buzz1_Cnt_Time     = eeprom_read_byte(&eeBuzz1_Cnt_Time);	
	Int_Siren_Cnt_Time = eeprom_read_byte(&eeInt_Siren_Cnt_Time);	
	SlaveAddress = eeprom_read_byte(&eeSlaveAddress);	

//	SensorKorrWert_TA = eeprom_read_word(&eeSensorKorrWert_TA);
//	SensorKorrWert_TI = eeprom_read_word(&eeSensorKorrWert_TI);
//	SensorKorrWert_CH02 = eeprom_read_word(&eeSensorKorrWert_CH02);
//	SensorKorrWert_CH03 = eeprom_read_word(&eeSensorKorrWert_CH03);

}
//
// --------------------------------------------------------------------------
//
void s_Countdown(void) {

	if(vCountdown == 255) { vCountdown = eeprom_read_byte(&eeCountdown); }

	s_SendStatusCountdown();

	vCountdown--;

	if (vCountdown == 255) 
	{ 
	  //vCountdown--;
		regAlarmStatus.bCountdown = 0;
		if (regAlarmStatus.bAlarmArmed == 0) 
		{ 
		  regAlarmStatus.bAlarmArmed   = 1;    // Aktiviert die Alarmanlage
		  regAlarmStatus.bPreAlarm     = 0;
		  regAlarmStatus.bAlarmExecute = 0;    // Alarm Auslösung zurücksetzen
			s_WriteActivationDataToEeprom();
			Ext_PWR_SIREN_PORT |= (1 << Ext_PWR_SIREN_PIN); // Ext-Siren PWR On  
			s_SendMessage(AC_MSGNO_SCHARFSCHALTUNG_ERFOLGT);
		}
		else 
		{ 
			if (regAlarmStatus.bPreAlarm == 1)   // wenn Voralarm bereits ausgelöst 
			{ 
        regAlarmStatus.bPreAlarm = 0;      // Voralarm zurücksetzen
				regAlarmStatus.bAlarmExecute = 1;  // Alarm Auslösen
				s_BuzzerStart_ALM();
				
			  s_SendMessage(AC_MSGNO_ALARMIERUNG_AUSGELOEST);
			}
		}   
	}

//	if (vCountdown == 250) 
//	{ 
//		regAlarmStatus.bCountdown = 0;
//	  vCountdown = 255;
//	}
}

///
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
	unsigned char   i;
  
	if(mode == EEProm_Init_IfEmpty)
	{
    buffer = eeprom_read_byte(&eeCheckByte);
	}

  if(buffer != EEPROM_DEFAULT_CHECKBYTEVALUE)
	{
#ifndef NO_LCD
		lcd_clrscr();
		lcd_puts("EEPROM INIT");
#endif  
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
		
		eeprom_write_byte(&eePreAlarm_BuzzerTast,         2);
		eeprom_write_byte(&eePreAlarm_BuzzerLevelMain,  100);
		eeprom_write_byte(&eePreAlarm_BuzzerLevelMin,   255);
		eeprom_write_byte(&eePreAlarm_BuzzerLevelMax,   255);
		eeprom_write_byte(&eePreAlarm_BuzzerIntervall,  100);
		
		eeprom_write_byte(&eeFullAlarm_BuzzerTast,        5);
		eeprom_write_byte(&eeFullAlarm_BuzzerLevelMain,  10);
		eeprom_write_byte(&eeFullAlarm_BuzzerLevelMin,  230);
		eeprom_write_byte(&eeFullAlarm_BuzzerLevelMax,  255);
		eeprom_write_byte(&eeFullAlarm_BuzzerIntervall, 100);

    for (i=0;i<4;i++)
	  {
	    		eeprom_write_word(&eeSensorKorrWert[i], -50);   
	  }                            

		eeprom_write_byte(&eeBuzz1_Cnt_Time,       2);
    eeprom_write_byte(&eeInt_Siren_Cnt_Time, 180);
    eeprom_write_byte(&eeSlaveAddress,        62);



#ifdef BOARD_AVRCTRL
	#warning "Set: BOARD_AVRCTRL - Interfacerooting"
		// Port 0
		eeprom_write_byte(&eeInPortRootingTable[0],  0x04); // K Haustür
		eeprom_write_byte(&eeInPortRootingTable[1],  0x32); // !! K PSK-Küche
		eeprom_write_byte(&eeInPortRootingTable[2],  0xFF); // !! K Kellertür
		eeprom_write_byte(&eeInPortRootingTable[3],  0xFF);
		eeprom_write_byte(&eeInPortRootingTable[4],  0xFF);
		eeprom_write_byte(&eeInPortRootingTable[5],  0xFF);
		eeprom_write_byte(&eeInPortRootingTable[6],  0xFF);
		eeprom_write_byte(&eeInPortRootingTable[7],  0xFF);
		// Port 1
		eeprom_write_byte(&eeInPortRootingTable[8],  0x07); // S Haustür
		eeprom_write_byte(&eeInPortRootingTable[9],  0x33); // !! S PSK-Küche
		eeprom_write_byte(&eeInPortRootingTable[10], 0xFF); // !! S Kellertür
		eeprom_write_byte(&eeInPortRootingTable[11], 0xFF);
		eeprom_write_byte(&eeInPortRootingTable[12], 0xFF);
		eeprom_write_byte(&eeInPortRootingTable[13], 0xFF);
		eeprom_write_byte(&eeInPortRootingTable[14], 0xFF);
		eeprom_write_byte(&eeInPortRootingTable[15], 0xFF);

		// Port 2
		eeprom_write_byte(&eeInPortRootingTable[16], 0x05); // K Windfang II
		eeprom_write_byte(&eeInPortRootingTable[17], 0x04); // K Windfang III
		eeprom_write_byte(&eeInPortRootingTable[18], 0x10); // !! K Wohnzimmer I
		eeprom_write_byte(&eeInPortRootingTable[19], 0x12); // !! K Wohnzimmer II
		eeprom_write_byte(&eeInPortRootingTable[20], 0x14); // !! K Esszimmer I
		eeprom_write_byte(&eeInPortRootingTable[21], 0x16); // !! K Esszimmer II
		eeprom_write_byte(&eeInPortRootingTable[22], 0x34); // !! K Bad
		eeprom_write_byte(&eeInPortRootingTable[23], 0xFF);
		// Port 3
		eeprom_write_byte(&eeInPortRootingTable[24], 0x07); // S Windfang II
		eeprom_write_byte(&eeInPortRootingTable[25], 0x07); // S Windfang III
		eeprom_write_byte(&eeInPortRootingTable[26], 0x10); // !! S Wohnzimmer I
		eeprom_write_byte(&eeInPortRootingTable[27], 0x23); // !! S Wohnzimmer II
		eeprom_write_byte(&eeInPortRootingTable[28], 0x15); // !! S Esszimmer I
		eeprom_write_byte(&eeInPortRootingTable[29], 0xFF); // !! S Esszimmer II
		eeprom_write_byte(&eeInPortRootingTable[30], 0x37); // !! S Bad
		eeprom_write_byte(&eeInPortRootingTable[31], 0xFF);
		// Port 4
		eeprom_write_byte(&eeInPortRootingTable[32], 0xFF);
		eeprom_write_byte(&eeInPortRootingTable[33], 0xFF);
		eeprom_write_byte(&eeInPortRootingTable[34], 0xFF);
		eeprom_write_byte(&eeInPortRootingTable[35], 0xFF);
		eeprom_write_byte(&eeInPortRootingTable[36], 0xFF);
		eeprom_write_byte(&eeInPortRootingTable[37], 0xFF);
		eeprom_write_byte(&eeInPortRootingTable[38], 0xFF);
		eeprom_write_byte(&eeInPortRootingTable[39], 0xFF);
		// Port 5
		eeprom_write_byte(&eeInPortRootingTable[40], 0xFF);
		eeprom_write_byte(&eeInPortRootingTable[41], 0xFF);
		eeprom_write_byte(&eeInPortRootingTable[42], 0xFF);
		eeprom_write_byte(&eeInPortRootingTable[43], 0xFF);
		eeprom_write_byte(&eeInPortRootingTable[44], 0xFF);
		eeprom_write_byte(&eeInPortRootingTable[45], 0x17);
		eeprom_write_byte(&eeInPortRootingTable[46], 0xFF);
		eeprom_write_byte(&eeInPortRootingTable[47], 0xFF);
		// Port 6
		eeprom_write_byte(&eeInPortRootingTable[48], 0xFF);
		eeprom_write_byte(&eeInPortRootingTable[49], 0xFF);
		eeprom_write_byte(&eeInPortRootingTable[50], 0xFF);
		eeprom_write_byte(&eeInPortRootingTable[51], 0xFF);
		eeprom_write_byte(&eeInPortRootingTable[52], 0xFF);
		eeprom_write_byte(&eeInPortRootingTable[53], 0xFF);
		eeprom_write_byte(&eeInPortRootingTable[54], 0xFF);
		eeprom_write_byte(&eeInPortRootingTable[55], 0xFF);
		// Port 7
		eeprom_write_byte(&eeInPortRootingTable[56], 0xFF);
		eeprom_write_byte(&eeInPortRootingTable[57], 0xFF);
		eeprom_write_byte(&eeInPortRootingTable[58], 0xFF);
		eeprom_write_byte(&eeInPortRootingTable[59], 0xFF);
		eeprom_write_byte(&eeInPortRootingTable[60], 0xFF);
		eeprom_write_byte(&eeInPortRootingTable[61], 0xFF);
		eeprom_write_byte(&eeInPortRootingTable[62], 0xFF);
		eeprom_write_byte(&eeInPortRootingTable[63], 0xFF);
#endif


#ifdef BOARD_AC128
	#warning "Set: BOARD_AC128 Interfacerooting"
		// Port 0
		eeprom_write_byte(&eeInPortRootingTable[0],  0x03); // K Haustür
		eeprom_write_byte(&eeInPortRootingTable[1],  0x15); // K PSK-Küche
		eeprom_write_byte(&eeInPortRootingTable[2],  0xFF); // !! K Kellertür
		eeprom_write_byte(&eeInPortRootingTable[3],  0xFF);
		eeprom_write_byte(&eeInPortRootingTable[4],  0xFF);
		eeprom_write_byte(&eeInPortRootingTable[5],  0xFF);
		eeprom_write_byte(&eeInPortRootingTable[6],  0xFF);
		eeprom_write_byte(&eeInPortRootingTable[7],  0xFF);
		// Port 1
		eeprom_write_byte(&eeInPortRootingTable[8],  0x02); // S Haustür
		eeprom_write_byte(&eeInPortRootingTable[9],  0x14); // S PSK-Küche
		eeprom_write_byte(&eeInPortRootingTable[10], 0xFF); // !! S Kellertür
		eeprom_write_byte(&eeInPortRootingTable[11], 0xFF);
		eeprom_write_byte(&eeInPortRootingTable[12], 0xFF);
		eeprom_write_byte(&eeInPortRootingTable[13], 0xFF);
		eeprom_write_byte(&eeInPortRootingTable[14], 0xFF);
		eeprom_write_byte(&eeInPortRootingTable[15], 0xFF);

		// Port 2
		eeprom_write_byte(&eeInPortRootingTable[16], 0x01); // K Windfang II
		eeprom_write_byte(&eeInPortRootingTable[17], 0x00); // K Windfang III
		eeprom_write_byte(&eeInPortRootingTable[18], 0x07); // K Wohnzimmer I
		eeprom_write_byte(&eeInPortRootingTable[19], 0x05); // K Wohnzimmer II
		eeprom_write_byte(&eeInPortRootingTable[20], 0x13); // K Esszimmer I
		eeprom_write_byte(&eeInPortRootingTable[21], 0x11); // K Esszimmer II
		eeprom_write_byte(&eeInPortRootingTable[22], 0x17); // K Bad
		eeprom_write_byte(&eeInPortRootingTable[23], 0xFF);
		// Port 3
		eeprom_write_byte(&eeInPortRootingTable[24], 0x02); // S Windfang II
		eeprom_write_byte(&eeInPortRootingTable[25], 0x02); // S Windfang III
		eeprom_write_byte(&eeInPortRootingTable[26], 0x06); // S Wohnzimmer I
		eeprom_write_byte(&eeInPortRootingTable[27], 0x04); // S Wohnzimmer II
		eeprom_write_byte(&eeInPortRootingTable[28], 0x12); // S Esszimmer I
		eeprom_write_byte(&eeInPortRootingTable[29], 0x10); // S Esszimmer II
		eeprom_write_byte(&eeInPortRootingTable[30], 0x16); // S Bad
		eeprom_write_byte(&eeInPortRootingTable[31], 0xFF);
		// Port 4
		eeprom_write_byte(&eeInPortRootingTable[32], 0xFF);
		eeprom_write_byte(&eeInPortRootingTable[33], 0xFF);
		eeprom_write_byte(&eeInPortRootingTable[34], 0xFF);
		eeprom_write_byte(&eeInPortRootingTable[35], 0xFF);
		eeprom_write_byte(&eeInPortRootingTable[36], 0xFF);
		eeprom_write_byte(&eeInPortRootingTable[37], 0xFF);
		eeprom_write_byte(&eeInPortRootingTable[38], 0xFF);
		eeprom_write_byte(&eeInPortRootingTable[39], 0xFF);
		// Port 5
		eeprom_write_byte(&eeInPortRootingTable[40], 0xFF);
		eeprom_write_byte(&eeInPortRootingTable[41], 0xFF);
		eeprom_write_byte(&eeInPortRootingTable[42], 0xFF);
		eeprom_write_byte(&eeInPortRootingTable[43], 0xFF);
		eeprom_write_byte(&eeInPortRootingTable[44], 0xFF);
		eeprom_write_byte(&eeInPortRootingTable[45], 0xFF);
		eeprom_write_byte(&eeInPortRootingTable[46], 0xFF);
		eeprom_write_byte(&eeInPortRootingTable[47], 0xFF);
		// Port 6
		eeprom_write_byte(&eeInPortRootingTable[48], 0xFF);
		eeprom_write_byte(&eeInPortRootingTable[49], 0xFF);
		eeprom_write_byte(&eeInPortRootingTable[50], 0xFF);
		eeprom_write_byte(&eeInPortRootingTable[51], 0xFF);
		eeprom_write_byte(&eeInPortRootingTable[52], 0xFF);
		eeprom_write_byte(&eeInPortRootingTable[53], 0xFF);
		eeprom_write_byte(&eeInPortRootingTable[54], 0xFF);
		eeprom_write_byte(&eeInPortRootingTable[55], 0xFF);
		// Port 7
		eeprom_write_byte(&eeInPortRootingTable[56], 0xFF);
		eeprom_write_byte(&eeInPortRootingTable[57], 0xFF);
		eeprom_write_byte(&eeInPortRootingTable[58], 0xFF);
		eeprom_write_byte(&eeInPortRootingTable[59], 0xFF);
		eeprom_write_byte(&eeInPortRootingTable[60], 0xFF);
		eeprom_write_byte(&eeInPortRootingTable[61], 0xFF);
		eeprom_write_byte(&eeInPortRootingTable[62], 0xFF);
		eeprom_write_byte(&eeInPortRootingTable[63], 0xFF);
#endif

    s_SendMessage(AC_MSGNO_EEPROM_INITIALISIERT);



//	  lcd_clrscr();
	}
}
//
// --------------------------------------------------------------------------
// Hardware-Initialisation-Routines
// --------------------------------------------------------------------------
//
// --------------------------------------------------------------------------
//
void TimerCounter1_Init (void) {
	//Set TIMER1 
	//Prescaler 64
	TCCR1B = (1<<CS10 | 1<<CS11);
	//SYSCLK define in usart.h
//	TCNT1 = 65535 - (SYSCLK/64/8000);
//	TIMSK |= (1 << TOIE1);
}


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
SIGNAL (SIG_OUTPUT_COMPARE2) {
  static unsigned int timer00100ms = 0, timer00350ms = 0, timer00500ms = 0, timer01000ms = 0, timer05000ms = 0;

	if(++timer00100ms == 100)
  {    
    regPollingTime.b00100ms = 1;
    timer00100ms = 0;
  }

  if(++timer00350ms == 350)
  {    
    regMenueCtrl.bRefreshDisplay = 1;
    timer00350ms = 0;
  }

  if(++timer00500ms == 500)
  {    
    regPollingTime.b00500ms = 1;
    timer00500ms = 0;
  }

  if(++timer01000ms == 1000)
  {    
    regPollingTime.b01000ms = 1;
    timer01000ms = 0;
  }
  
	if(++timer05000ms == 5000)
  {    
    regPollingTime.b05000ms = 1;
    timer05000ms = 0;
  }
}
//
// --------------------------------------------------------------------------
//



#ifndef NO_RTC
void RTC_set(void)
{
  sDTG.Weekday =  3;
  sDTG.Day     = 06;
  sDTG.Month   =  2;
  sDTG.Year    =  8;
  pcf8583_set_Date(RTC_Adress, &sDTG.Weekday, &sDTG.Day, &sDTG.Month, &sDTG.Year);
  sDTG.Hour    = 12;
  sDTG.Min     = 38;
  sDTG.Sec     =  0;
  pcf8583_set_Time(RTC_Adress, &sDTG.Hour, &sDTG.Min, &sDTG.Sec);
}
#endif

//
// --------------------------------------------------------------------------
//
///
// --------------------------------------------------------------------------
//***************************************************************************
//* Name		    : s_AccessCmd 
//* Datum / Version : 03.07.2010 / 0.0.1000
//* -------------------------------------------------------------------------
//* Start-Value:	 
//*   command    unsigned char     >> Id for the next executablecommand
//*                                   
//* -------------------------------------------------------------------------
//* Return-Value:  -none- 
//* -------------------------------------------------------------------------
//* Description:
//*   Starts Systemprocedure commanded by the given Value.
//*   Value = 0        = no command
//*   value = 1 .. 253 = valid commands 
//*   value = 254      = NOP commands 
//*   value = 255      = unvalid command 
//*
//***************************************************************************

void s_AccessCmd(unsigned char command){
	switch (command)
	{
		//## SYSTEMBEFEHLE ##//
		case	CMD_AC_ResetSystem:
				    uart_puts("CMD SYSTEM RESET\r\n");
					// ToDo
					break;

		case	CMD_AC_ReloadEepromData:
					// ToDo  
					s_ReadStartDataFromEeprom();
				    uart_puts("CMD RELOAD EEPROM\r\n");
					break;

		case	CMD_AC_NOP:
					// ToDo  
					uart_puts("CMD NOP\r\n");
					break;

		case	CMD_AC_StartIntBuzzer:
					Buzz1_Cnt = Buzz1_Cnt_Time;
				    uart_puts("CMD START INT BUZZER\r\n");
					break;

		default :
				uart_puts(" ERROR\r\n");
				break;
	}
}


// **************************************************************************
// **************************************************************************
// **************************************************************************
// --------------------------------------------------------------------------
// --------------------------------------------------------------------------
// Begin of MAIN-Routine  
// --------------------------------------------------------------------------
// --------------------------------------------------------------------------
int main(void) {
  // ------------------------
  // --> SW-Settings
  // ------------------------

	sDTG.Weekday   =  1;     
	sDTG.Day       = 31;     
	sDTG.Month     = 12;     
	sDTG.Year      =  7;     
	sDTG.Hour      = 23;     
	sDTG.Min       = 59;
	sDTG.Sec       = 47;     
    
//    char buffer[7];
//    int  num=134;
//	int16_t		value;
//	uint8_t		data;

  // ------------------------
  // --> HW-Settings
  // ------------------------

  // initialize Ports 
  Ports_Init();

#ifndef NO_LCD
  // initialize display, cursor off
  lcd_init(LCD_DISP_ON);
#endif

  //  Initialize UART library
  uart_init( UART_BAUD_SELECT(UART_BAUD_RATE,F_CPU) ); 

  // now enable interrupt, since UART library is interrupt controlled
  sei();

  //  Initialisation of EEPROM
  s_EEProm_Init(EEProm_Init_IfEmpty);
	if(TASTE_EEProm_Init_PIN & (1<<TASTE_EEProm_Init_PNO)) s_EEProm_Init(EEProm_Init_SetDefault);

	
#ifndef NO_I2C
  // initialize I2C library
  i2c_init();  
#endif
                     
  // ADC initialization
  // ADC Clock frequency: 500,000 kHz
  ADCSRA=0x84;


  // initialize Timer/Counter 
	TimerCounterPWM_Init();	    // initialize as PWM-Output
	TimerCounter2_Init();	    // initialize as Counter
  TimerCounter1_Init();
//  TimerCounter0_Init();


#ifdef DEBUG_STEPPING 
 wait_until_key_pressed(); 
#endif
	
 		
//	BStatus.bRefreshMenu = 1;	
//	BStatus.bStopClock = 0;
		
  ShowIntroUart0();
	ShowIntroLCD();
	

  regAlarmStatus.bAlarmArmed = 0;    // Deaktiviert den Alarmmode
  regCtrlMode.bRemote        = 1;    // Aktiviert den Remotebetrieb
  regCtrlMode.bInternal      = 1;    // Alarmierung Intern    Ein  = 1 / Aus  = 0 
  regCtrlMode.bExternal      = 1;    // Alarmierung Extern    Ein  = 1 / Aus  = 0 
  regCtrlMode.bIntNoise      = 1;    // Alarmierung Akustisch Ein  = 1 / Aus  = 0      
  regCtrlMode.bExtNoise      = 1;    // Alarmierung Akustisch Ein  = 1 / Aus  = 0      
  regCtrlMode.bByGSM         = 0;    // Alarmierung über GSM  Ein  = 1 / Aus  = 0      







//  fAlarmMode.Countdown  = 1;    // Aktiviert den Countdowns

	s_ReadStartDataFromEeprom();

//  RTC_set();
    
    
#ifndef NO_LCD
    /* clear display and home cursor */
//    lcd_clrscr();
#endif
        

  //  Initialize ADC-Chanals on m8slave
	m8slave_set_adc(SlaveAddress, 0, adc_on);
	m8slave_set_adc(SlaveAddress, 1, adc_on);
	m8slave_set_adc(SlaveAddress, 2, adc_on);
	m8slave_set_adc(SlaveAddress, 3, adc_on);

//  m8slave_write_eeprom(SlaveAddress, 99, 111);

	m8slave_set_pinb_ddr(SlaveAddress, 2, pin_as_output);
	m8slave_set_pinb(SlaveAddress, 2, pin_high);
	m8slave_set_pinb_ddr(SlaveAddress, 3, pin_as_output);
	m8slave_set_pinb(SlaveAddress, 3, pin_low);

//	m8slave_set_pind_ddr(SlaveAddress, 6, pin_as_input);
//	m8slave_set_pind_ddr(SlaveAddress, 6, pin_as_input);

 
 GetTemperatur(1); 

//###DEBUG_ON###
//  int16_t DebugCnt = 0;
//###DEBUG_OFF###

//  static unsigned char counter;

    
    for(;;)
    {
      s_Menu();


//			if (regAlarmStatus.bRemote == 1)
//			{
//				s_Uart0_Read();
//			}

			
//			s_GetTastatur();
			Tastatur();
//		  Int_Buzz1_PORT &=~(1 << Int_Buzz1_PIN);  // Int-Buzzer No1 Off  
			s_TestRemoteMode();
			s_GetInterfacePorts();
			s_RootPortsToRegs();
			s_AlarmCheckPortStatus();
			
			if (CMD_Memory > 0)
			{
				s_AccessCmd(CMD_Memory);
				CMD_Memory = 0;
			}

			if (regAlarmStatus.bAlarmExecute == 1)
			{
				if (regCtrlMode.bInternal == 1) 
				{
					Int_SIREN_PORT |= (1 << Int_SIREN_PIN);   // Int-Siren On  
				}
				else
				{
					Int_Buzz1_PORT |= (1 << Int_Buzz1_PIN);   // Int-Buzzer No1 On  
					if (regCtrlMode.bIntNoise == 1) 
					{
						Int_FLASH_PORT |= (1 << Int_FLASH_PIN); // Int-Flash On
					}      
				}
				
				if (regCtrlMode.bExternal == 1) 
				{
					Ext_PWR_FLASH_PORT |= (1 << Ext_PWR_FLASH_PIN);   // Ext-Flash On
					Ext_AlmSig_PORT    |= (1 << Ext_AlmSig_PIN);      // Ext-Alarm-Signal On  
				} 
				else
				{
					if (regCtrlMode.bExtNoise == 1) 
					{
						Ext_PWR_FLASH_PORT |= (1 << Ext_PWR_FLASH_PIN); // Ext-Flash On
					}
				}
			}


 
			if (regCtrlMode.bRemote == 1)
			{
				s_Uart0_Read();
			}
			else
			{
				if (regUserControl.bSwitchArmed) 
				{
					if (regAlarmStatus.bAlarmArmed == 0)
					{
						if (regAlarmStatus.bCountdown == 0) s_AlarmArmed();
					}
				}
				else 
				{
					if (regAlarmStatus.bAlarmArmed)
					{
						s_AlarmInert();
					}
					if (regAlarmStatus.bCountdown)
					{
						s_AlarmAbort();
					}
				}
			}
			
//		LED_NotArmed_PORT &= ~(1 << LED_NotArmed_PNO);        /* Pin low    */
//		LED_NotArmed_PORT |=  (1 << LED_NotArmed_PNO);        /* Pin high    */
			if (regAlarmStatus.bAlarmArmed)
			{
				LED_NotArmed_PORT &= ~(1 << LED_NotArmed_PNO);        /* Pin low    */
			}
			else
			{
				LED_NotArmed_PORT |=  (1 << LED_NotArmed_PNO);        /* Pin high    */
			}
 
 
 
//			if (regAlarmStatus.bAlarmArmed)
//			{
//				s_AlarmCheckPortStatus();
//			}
//			else
//			{
//				xxx;
//			}




			if(regPollingTime.b00100ms == 1)
			{
				if(regAlarmStatus.bPreAlarm == 1)
				{
					if (tastcounter == 0)	
					{
						s_BuzzerReStart_PRE();
					}		 
					if (tastcounter == BuzzerTast)
					{
						s_BuzzerStop();
					} 
					tastcounter++;
					if (tastcounter == 10) tastcounter = 0;
				}

//				if (Buzz1_Cnt <= Buzz1_Cnt_Time)      
//				{ 
//				  Buzz1_Cnt ++; 
//					if (Buzz1_Cnt >= Buzz1_Cnt_Time)     
//					{ 
//						Int_Buzz1_PORT &=~(1 << Int_Buzz1_PIN);  // Int-Buzzer No1 Off  
//					}
//				}


				if (Buzz1_Cnt > 0)      
				{ 
				  Buzz1_Cnt--; 
					if (Buzz1_Cnt == 0)     
					{ 
						Int_Buzz1_PORT &=~(1 << Int_Buzz1_PIN);  // Int-Buzzer No1 Off  
					}
				}

				regPollingTime.b00100ms = 0;
			}

/*
			if (regPollingTime.b00500ms == 1)	
			{
				
				if (TASTE == UP) 
				{
					counter++;
					if (LED03x_PORT & (1<<LED03x_PNO))
					{
						LED03x_PORT &= ~(1 << LED03x_PNO);        // Pin low     
					} 
					else
					{
						LED03x_PORT |=  (1 << LED03x_PNO);        // Pin high     
					}
				} 
				
				if (TASTE == DOWN) 
				{
					counter--;
					if (LED03x_PORT & (1<<LED03x_PNO))
					{
						LED03x_PORT &= ~(1 << LED03x_PNO);        // Pin low    
					} 
					else
					{
						LED03x_PORT |=  (1 << LED03x_PNO);        // Pin high     
					}
				} 
				
				if (TASTE == RETURN) 
				{
					counter = 0;
					if (LED03x_PORT & (1<<LED03x_PNO))
					{
						LED03x_PORT &= ~(1 << LED03x_PNO);        // Pin low    
					} 
					else
					{
						LED03x_PORT |=  (1 << LED03x_PNO);        // Pin high    
					}
				} 
				
				regPollingTime.b00500ms = 0;
			}
*/

			if (regPollingTime.b01000ms == 1)	
			{
 				regSendData.bSendStatus = 1;
				if (regAlarmStatus.bCountdown == 1)     // Aktiviert den Countdowns
				{ 
				  s_Countdown(); 
				}
				Refresh_DTG ();
//        View2LCD_DTG();

//				if (Buzz1_Cnt <= Buzz1_Cnt_Time)      
//				{ 
//				  Buzz1_Cnt ++; 
//					if (Buzz1_Cnt >= Buzz1_Cnt_Time)     
//					{ 
//						Int_Buzz1_PORT &=~(1 << Int_Buzz1_PIN);  // Int-Buzzer No1 Off  
//					}
//				}

				if (regAlarmStatus.bAlarmExecute == 1)
				{
					Int_Siren_Cnt ++; 
					if (Int_Siren_Cnt == Int_Siren_Cnt_Time)
					{
						Int_SIREN_PORT &=~(1 << Int_SIREN_PIN);     // Int-Siren Off  
						Int_Siren_Cnt = 0; 
					}
					//lcd_gotoxy(0,3);
					//lcd_puts ("TTD: ");
					//IntToStringformat(Int_Siren_Cnt, 5, 0);
					//lcd_puts(CharBuffer);
					//lcd_puts (" sec");
				
				
				}
				regPollingTime.b01000ms = 0;
			}

			if (regPollingTime.b05000ms == 1)	
			{
				GetTemperatur(0);
				regSendData.bSendStatKL = 1;
				regPollingTime.b05000ms = 0;
			}

			if (regSendData.bSendStatus == 1)
			{
				s_SendStatusMain(); 
				regSendData.bSendStatus = 0;
//###DEBUG_ON###
//  uart_puts("DEBUG:");                      // Checksum
//  s_SendIntegerToUart(DebugCnt);   
//  uart_puts("\r");                      // Checksum
//  DebugCnt = 0;
//###DEBUG_OFF###

			}

			if (regSendData.bSendStatKL == 1) 
			{
				s_KL_SendStatus();
        //s_SendStatusI2C_Slave();
				regSendData.bSendStatKL = 0;
			}

			if (regSendData.bSendPortDef == 1)
			{
				s_SendPortDefRoot(); 
				regSendData.bSendPortDef = 0;
			}

//###DEBUG_ON###
//  ++DebugCnt;
//###DEBUG_OFF###
    
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

