/*********************************************
Project : 
Version : 
Date    : 03.12.2003
Author  : Holger Buss & Ingo Busker
Comments: 

Chip type           : AT90S8535
Clock frequency     : 8.0000000 MHz
Memory model        : Small
Internal SRAM size  : 512
External SRAM size  : 0
Data Stack size     : 128
*********************************************/
#include "Errata.c"  // Versionshistorie und Errata-Sheet                       
                 //1234567890123456

#define PROGNAME  "MHG-Adaptiv 0.1c"
#define COPYRIGHT "M.KRAUS - 2006"
#define SYS_ID    "HC32"     // Systemidentifikation
#define VERS_DATE "Date: 20061225" // Hardwareversion
#define VERS_HW   "HW02.008" // Hardwareversion
#define VERS_SW   "SW02.01c" // Softwareversion
#define VERS_PF   "PW02.10c" // Protokollversion
//#define PROGNAME  "I. Busker V1.1" 
//#define COPYRIGHT "H. Buss" 
 
//#define LCD_0416 
#define LCD_0216 

#define ExternTast 
       
#define ADC_TEST 


//#define NO_RTC 

#define EXTRA_MENU 
//#define INCR_GEBER 
//#define NO_LCD
//#define NO_RTC 
#define LOG_EXTENDED

#include <mega32.h>
//#include <mega16.h>
#include <delay.h>
#include <stdio.h>

#ifdef NO_LCD
#include "lcd_remote.h"                         
#include "lcd_remote.c"                         

#else
// Alphanumeric LCD Module functions
#asm
   .equ __lcd_port=0x15
#endasm
#include <lcd.h>                         
#endif

#ifdef NO_RTC 
#else
// Real time Clock PCF8583 functions 
#asm
   .equ __i2c_port=0x12
   .equ __sda_bit=6
   .equ __scl_bit=5
#endasm
#include <pcf8583.h>                         
#endif



//-----------
typedef eeprom struct
{
  unsigned int Zeit;
  unsigned char Mode;
} struct_daten;

eeprom struct_daten Timer[65];
eeprom unsigned char SchichtOffset;
unsigned int AktuellZeit = 0;
bit Polling100ms = 1;
#define WERKTAG       0x4000
#define ZEIT(s,m)     (s * 60 + m)
#define ZEIT_MASKE    7*1440
flash char Tag[7,3]= {"Mo","Di","Mi","Do","Fr","Sa","So"};
flash char txt_OnOff[2,4]= {"Ein","Aus"};
flash char txt_Modes[3,6]= {"Tag","Nacht","Abw."};
flash char txt_SenorNameShort[8,3]= {"TL","TA","TK","TW","P1","P2","P3","AG"};
eeprom unsigned char eeValYear;
eeprom unsigned char eeBufRtcYear;
#define ON  0
#define OFF 1

#ifdef NO_RTC 
#else
unsigned char date;
unsigned char month;
unsigned char year;
unsigned char hour;
unsigned char minute;
unsigned char second;
unsigned char hundredsecond;
unsigned char weekday;
#endif

#define NACHT         0
#define TAG           1
#define ABWESEND      2
#define DEAKTIV       3 

unsigned char BetriebsMode = TAG;
unsigned int CountHeizzeit = 0,CountZeit = 0, CountStartBRE = 0;

////////////////////   flash unsigned char Leds[3] = { 0x04, 0x08, 0x10 };
//-----------
unsigned char Taste = 0, MenuCount=0;
bit MenuPolling = 1;
bit RefreshDisplay = 0;
bit RefreshDispDateTime = 1;
bit PollingMinute = 0;                           
bit PollingTag = 0;
bit SendeDaten = 0;
bit Output = 0;
bit SendeDisplay = 0;
bit SendeLogDat = 0;
bit SendeLogDatMOD = 0;
bit SendeDataSet = 0;
bit SendeSysID = 0;
//bit SendeVersHW = 0;
bit SendeVersSW = 0;
bit SendeVersPF = 0;

#define NICHTS 0
#define MENU   1
#define UP     3
#define DOWN   4
#define OKAY   5
#define MODE   6

#define SEN230V  7
         
#ifdef ExternTast
  #define TASTE1       PIND.7        // Version mit externer Tastatur
  #define TASTE2       PIND.4
  #define TASTE3       PIND.3
  #define TASTE4       PIND.2        // Version mit externer Tastatur
  #define TASTE5       PINC.3
#else
  //#define TASTE1       PINA.7
  #define TASTE2       PINA.6
  #define TASTE3       PINA.5
  #define TASTE4       PINA.4
  #define TASTE5       PINA.3
#endif

//#define TASTE10      PIND.4

#ifdef ExternTast
  #define ADC_TL       0     // Sensor TempLeit                Pos:  Tauchhuelse Pufferspeicher Oben
  #define ADC_TA       1     // Sensor Temp-Aussen             Pos:  Aussen am Kellerniedergang
  #define ADC_TW       2     // Sensor Temp-Warmwasser         Pos:  Tauchhuelse Warmwasserspeicher
  #define ADC_TK       3     // Sensor Temp-Kessel OeHz        Pos:  Tauchhuelse OeHz-Kessel
  #define ADC_P1       4     // Sensor Temp-Ruecklauf02 OeHz   Pos:  Austritt OeHz Waermetauscher
  #define ADC_P2       5     // Sensor Temp-Ruecklauf01 OeHz   Pos:  Eintritt OeHz Waermetauscher
  #define ADC_P3       6     // Sensor Temp-Vorlauf OeHz       Pos:  Austritt OeHz-Kessel
  #define ADC_AG       7     // Sensor Temp-Abgas OeHz         Pos:  Abgasrohr Austritt Waermetauscher
#else
  #define ADC_TW       0     // Sensor Temp-Warmwasser         Pos:  Tauchhuelse Warmwasserspeicher
  #define ADC_TA       1     // Sensor Temp-Aussen             Pos:  Aussen am Kellerniedergang
  #define ADC_TL       2     // Sensor TempLeit                Pos:  Tauchhuelse Pufferspeicher Oben
  #define ADC_TK       7     // Sensor Temp-Kessel OeHz        Pos:  Tauchhuelse OeHz-Kessel
#endif

//-----------
#define ANZAHL_KENNLINIEN 3


signed int  IstTempAussen;      // Sensor Temp-Aussen             Pos:  Aussen am Kellerniedergang
signed int  IstTempLeit;        // Sensor TempLeit                Pos:  Tauchhuelse Pufferspeicher Oben
signed int  IstTempWWasser;     // Sensor Temp-Warmwasser         Pos:  Tauchhuelse Warmwasserspeicher
signed int  IstTempKessel_OHz;  // Sensor Temp-Kessel OeHz        Pos:  Tauchhuelse OeHz-Kessel
#ifdef ExternTast
signed int  TempValSen_P1;      // Sensor Temp-Ruecklauf02 OeHz   Pos:  Austritt OeHz Waermetauscher
signed int  TempValSen_P2;      // Sensor Temp-Ruecklauf01 OeHz   Pos:  Eintritt OeHz Waermetauscher
signed int  TempValSen_P3;      // Sensor Temp-Vorlauf OeHz       Pos:  Austritt OeHz-Kessel
signed int  TempValSen_AG;      // Sensor Temp-Abgas OeHz         Pos:  Abgasrohr Austritt Waermetauscher
#else
#endif
signed long DurchschnittsTemperatur = 0;        
#define ANZ_LOGGING 130
eeprom unsigned char RingSpeicher_Verbrauch[ANZ_LOGGING];
eeprom unsigned char RingSpeicher_TempAussen[ANZ_LOGGING];
eeprom unsigned char RingSpeicher_CounterStartBRE[ANZ_LOGGING];
eeprom unsigned char ZeigerRingspeicher;

#define LED_NACHT                      PORTB.0
#define LED_ABWESEND                   PORTB.1
#define RELAIS_ERZEUGERSPERRE_OELHZ    PORTB.2
#define RELAIS_KPUMPE                  PORTB.3
#define LED_TAG                        PORTB.4
#define LED_SERVICE                    PORTB.5
#define RELAIS_WPUMPE                  PORTB.7

#define LED_TEST                       PORTB.6




#define HYSTERESE_TEMPLEIT_MIN       20
#define HYSTERESE_TEMPLEIT_MAX      200
#define HYSTERESE_WARMWASSER_MIN     10
#define HYSTERESE_WARMWASSER_MAX   1200
#define TEMPERATUR_LEIT_MAX         800
#define TEMPERATUR_LEIT_MIN          50
eeprom unsigned int   HystereseTempLeit;
eeprom unsigned int   HystereseWarmWasser;
eeprom unsigned int   TempLeitMax_TL;
eeprom unsigned long  CounterStartBRE_Gesamt;



unsigned int ServiceMode = 1;
//unsigned int ServiceModeMaxTemp_TL = 800;

#define            LEIT_REST_TEMPERATUR  400                   // solange bleibt die Pumpe an

signed int         SollTempLeit = 0; 
signed int         SollTempWWasser = 0; 
eeprom signed int  MaxLeitTemperatur[ANZAHL_KENNLINIEN];
eeprom signed int  MinLeitTemperatur[ANZAHL_KENNLINIEN];
eeprom signed int  KennlinieTemperaturOben[ANZAHL_KENNLINIEN];
eeprom signed int  KennlinieTemperaturUnten[ANZAHL_KENNLINIEN];
eeprom signed int  PumpeEinTemperatur[ANZAHL_KENNLINIEN];
eeprom signed int  WarmWasserTemperatur[ANZAHL_KENNLINIEN];
eeprom signed int  SensorKorrWert_TA;
eeprom signed int  SensorKorrWert_TL;
eeprom signed int  SensorKorrWert_TW;
eeprom signed int  SensorKorrWert_TK;
eeprom signed int  SensorKorrWert_P1;
eeprom signed int  SensorKorrWert_P2;
eeprom signed int  SensorKorrWert_P3;
eeprom signed int  SensorKorrWert_AG;


eeprom unsigned char CheckEEpromData;
#define EEPROM_KOMPATIBILITAET  1
//-----------
#define GRAD_ZEICHEN  0xDF

#ifndef NO_LCD
flash char lcd_char1[8]=       // Tag
 {
  0b0000100,
  0b0010101,
  0b0001110,
  0b0010001,
  0b0010001,
  0b0001110,
  0b0010101,
  0b0000100
  };
flash char lcd_char0[8]=       // Nacht
 {
  0b0000000,
  0b0000000,
  0b0001110,
  0b0011111,
  0b0011111,
  0b0001110,
  0b0000000,
  0b0000000
  };
flash char lcd_char2[8]=       // Abwesend
 {
  0b0000000,
  0b0000000,
  0b0001110,
  0b0010001,
  0b0010001,
  0b0001110,
  0b0000000,
  0b0000000
  };

flash char lcd_char3[8]=       // Arrow Up
 {
  0b0000000,
  0b0000100,
  0b0001110,
  0b0010101,
  0b0000100,
  0b0000100,
  0b0000100,
  0b0000000
  };

flash char lcd_char4[8]=       // Arrow Down
 {
  0b0000000,
  0b0000100,
  0b0000100,
  0b0000100,
  0b0010101,
  0b0001110,
  0b0000100,
  0b0000000
  };
 
flash char lcd_char5[8]=       // Sonne
 {
  0b0000100,
  0b0010001,
  0b0001110,
  0b0011111,
  0b0011111,
  0b0001110,
  0b0010001,
  0b0000100
  };

/*
flash char lcd_char5[8]=       // Sonne
 {
  0b0011111,
  0b0010101,
  0b0001010,
  0b0010001,
  0b0001010,
  0b0010101,
  0b0011111,
  0b0000100
  };
*/

flash char lcd_char6[8]=       // Schneeflocke
 {
  0b0000000,
  0b0001010,
  0b0010101,
  0b0001110,
  0b0010101,
  0b0001010,
  0b0000000,
  0b0000000
  };

// ---------------------------------------------------------------------------------------
void LCD_UserChar(char flash *pc,char char_code)
{
  char i,a;
  a=(char_code<<3) | 0x40;   
  for (i=0; i<8; i++) lcd_write_byte(a++,*pc++);
}

#endif


// ---------------------------------------------------------------------------------------
void LCDInit(void)
{
// LCD module initialization
  lcd_init(16);
#ifndef NO_LCD
  LCD_UserChar(lcd_char0,1);
  LCD_UserChar(lcd_char1,2);
  LCD_UserChar(lcd_char2,3);
  LCD_UserChar(lcd_char3,4);
  LCD_UserChar(lcd_char4,5);
  LCD_UserChar(lcd_char5,6);
  LCD_UserChar(lcd_char6,7);
#endif
}
// ---------------------------------------------------------------------------------------
#ifdef NO_RTC 
#else
void rtc_set_weekday(unsigned char chip, unsigned char weekday)
  {  /* write Weekday to RTC */

    unsigned int ByteBuf;
    ByteBuf = rtc_read(chip,6);
    ByteBuf &= 0b00011111 ;
    ByteBuf += (weekday *  32);
    rtc_write(chip, 6, ByteBuf);

  }
// ---------------------------------------------------------------------------------------
char rtc_get_weekday(unsigned char chip)
  {  /* read Weekday from RTC */
    unsigned int ByteVal;
     rtc_write(chip, 0, 0);
     ByteVal = rtc_read(chip,6);
     ByteVal &= 0b11100000 ;
     ByteVal /=  32;
     rtc_write(chip, 0, 8);
     return(ByteVal);
  }
// ---------------------------------------------------------------------------------------
void rtc_set_year(unsigned char chip, unsigned char year)
  {  /* write Year to RTC */

    unsigned int ByteBuf;
    ByteBuf = rtc_read(chip,5);
    ByteBuf &= 0b00111111 ;
    ByteBuf += (year *  64);
    rtc_write(chip, 5, ByteBuf);

  }
// ---------------------------------------------------------------------------------------
char rtc_get_year(unsigned char chip)
  {  /* read Year from RTC */
    unsigned int ByteVal;
     rtc_write(chip, 0, 0);
     ByteVal = rtc_read(chip,5);
     ByteVal &= 0b11000000 ;
     ByteVal /=  64;
     rtc_write(chip, 0, 8);
     return(ByteVal);
  }
// ---------------------------------------------------------------------------------------
#endif
#define LCD      0
#define SERIELL  1
#define lcd_int(w, a, p)     Raw_INT(LCD, w, a, p)
#define Seriell_int(w, a, p) Raw_INT(SERIELL, w, a, p)
#define _ALTERNATE_PUTCHAR_
void putchar(char c)
{
  if (Output == LCD) lcd_putchar(c);
   else 
   {
    #asm
     sbis usr,udre
     rjmp _putchar
     ld   r30,y
     out  udr,r30
#endasm
   }
}                                              

flash unsigned int disp_wert[] = {0,1,10,100,1000,10000,0xFF};
#define LEADING_ZERO  -1 
// ---------------------------------------------------------------------------------------
void Raw_INT(char output,signed int wert , signed char anzahl , signed char punkt_pos)
 {
  signed char i;
  unsigned char pos, null;  
   
  Output = output;
  
  if(anzahl < 0)                         
   {
     anzahl = -anzahl;
     if((signed int)wert < 0) { wert = -wert; putchar('-');  }
     else  putchar('+');                    
   }

//  if((punkt_pos > anzahl) || (anzahl > 3))
  if((punkt_pos > anzahl) || (anzahl > 5))
   {
    for (i=0;i<anzahl;i++) putchar('?'); return;                            
   }

  if(wert >= disp_wert[anzahl + 1])     
   {
    for (i=0;i<anzahl;i++) putchar('?');  return;                            
   }

  if(punkt_pos < 0 ) null = 1; else null = 0;
  
  for(i = anzahl ; i > 0 ; i--)
   {
    if(i == punkt_pos) {  putchar('.');  null = 1; } 

    pos = (wert / disp_wert[i]) + 0x30;

    if((pos == '0') && !null && (i != 1))    
      if (i== punkt_pos+1) putchar('0');
        else putchar(' ');
    else
     {
      null = 1;                        
      putchar(pos);
      wert %= disp_wert[i];            
     }
   }   
    
 }
// ---------------------------------------------------------------------------------------
unsigned int read_adc(unsigned char adc_input)
{
 ADMUX=adc_input;
 ADCSRA.6=1;
 while (ADCSRA.4==0);
 ADCSRA.4=1;
 return ((ADCW*25)/4)-2250 ;
}

// ---------------------------------------------------------------------------------------
void GetTemperatur(unsigned char Start)
{
 signed int tmp_int;
 unsigned int RTC_YEAR; 
 
 tmp_int = read_adc(ADC_TA) + SensorKorrWert_TA;
 
 if(Start) IstTempAussen = tmp_int;
 if(PollingMinute) 
  {
    if (IstTempAussen > tmp_int)      IstTempAussen--;
    else if (IstTempAussen < tmp_int) IstTempAussen++;

    RTC_YEAR = rtc_get_year(0);
    if (eeBufRtcYear == RTC_YEAR)
    {
      //putsf("NewMin\r");
    } 
    else
    {
      eeValYear ++; 
      eeBufRtcYear = RTC_YEAR;  
      //putsf("NewYear\r");
    } 

    PollingMinute = 0;
  }

 tmp_int = read_adc(ADC_TA) + SensorKorrWert_TA;
 if(Start) IstTempAussen = tmp_int;
 
 tmp_int = read_adc(ADC_TL) + SensorKorrWert_TL;
 if (IstTempLeit > tmp_int)      IstTempLeit--;
  else if (IstTempLeit < tmp_int) IstTempLeit++;
 if(Start) IstTempLeit = tmp_int;

 tmp_int = read_adc(ADC_TW) + SensorKorrWert_TW;
 if (IstTempWWasser > tmp_int)      IstTempWWasser--;
  else if (IstTempWWasser < tmp_int) IstTempWWasser++;
 if(Start) IstTempWWasser = tmp_int;

 tmp_int = read_adc(ADC_TK) + SensorKorrWert_TK;
 if (IstTempKessel_OHz > tmp_int)      IstTempKessel_OHz--;
  else if (IstTempKessel_OHz < tmp_int) IstTempKessel_OHz++;
 if(Start) IstTempKessel_OHz = tmp_int;

#ifdef ExternTast
 tmp_int = read_adc(ADC_P1) + SensorKorrWert_P1;
 if (TempValSen_P1 > tmp_int)      TempValSen_P1--;
  else if (TempValSen_P1 < tmp_int) TempValSen_P1++;
 if(Start) TempValSen_P1 = tmp_int;

 tmp_int = read_adc(ADC_P2) + SensorKorrWert_P2;
 if (TempValSen_P2 > tmp_int)      TempValSen_P2--;
  else if (TempValSen_P2 < tmp_int) TempValSen_P2++;
 if(Start) TempValSen_P2 = tmp_int;

 tmp_int = read_adc(ADC_P3) + SensorKorrWert_P3;
 if (TempValSen_P3 > tmp_int)      TempValSen_P3--;
  else if (TempValSen_P3 < tmp_int) TempValSen_P3++;
 if(Start) TempValSen_P3 = tmp_int;

 tmp_int = read_adc(ADC_AG) + SensorKorrWert_AG;
 if (TempValSen_AG > tmp_int)      TempValSen_AG--;
  else if (TempValSen_AG < tmp_int) TempValSen_AG++;
 if(Start) TempValSen_AG = tmp_int;
#endif

#ifdef ADC_TEST 
  IstTempAussen     = 070  + SensorKorrWert_TA;
  IstTempLeit       = 530  + SensorKorrWert_TL;
  IstTempWWasser    = 420  + SensorKorrWert_TW;
  IstTempKessel_OHz = 630  + SensorKorrWert_TK;
  TempValSen_P1     = 380  + SensorKorrWert_P1;
  TempValSen_P2     = 260  + SensorKorrWert_P2;
  TempValSen_P3     = 280  + SensorKorrWert_P3;
  TempValSen_AG     = 360  + SensorKorrWert_AG;
#endif



}

// ---------------------------------------------------------------------------------------
// 1ms - Interrupt
interrupt [TIM2_COMP] void timer2_comp_isr(void)
{
  static unsigned int clock= 0,tim = 0;
  
  if (++clock == 60000) 
   { 
     #ifdef NO_RTC
     AktuellZeit++; 
     #else
     #endif
     clock = 0; 
     PollingMinute = 1;
     DurchschnittsTemperatur += IstTempAussen / 10;

     if (AktuellZeit % 1440 == 0) 
      { 
           PollingTag = 1;
      }

     #ifdef NO_RTC
     if (AktuellZeit >= 1440*7) 
      { 
       AktuellZeit = 0;
      }
     #else
     #endif

   
   } 
  if(++tim == 2000)
   {    
    if(RELAIS_ERZEUGERSPERRE_OELHZ == 0) CountHeizzeit++;
    CountZeit++;
    tim = 0;
   }
  if ((clock % 350) == 0) RefreshDisplay = 1;
  if ((clock % 10 ) == 0) Polling100ms = 1;
}
// ---------------------------------------------------------------------------------------
interrupt [USART_RXC] void uart_rx_isr(void)
{
  unsigned char zeichen;
  zeichen = UDR;
  
  switch (zeichen)
  { case 'T':  SendeDaten = 1; break;
    case '2':  Taste = UP; SendeDisplay = 1;break;
    case '1':  Taste = DOWN; SendeDisplay = 1; break;
    case '3':  Taste = MODE; break;
    case ' ':  SendeDisplay = 1; break;
    case '\r': Taste = OKAY; SendeDisplay = 1; break; 
    case 'L':  SendeLogDat = 1; break;
    case 'M':  SendeLogDatMOD = 1; break;
    case 'D':  SendeDataSet = 1; break;
    case 'I':  SendeSysID = 1; break;
//    case 'H':  SendeVersHW = 1; break;
    case 'S':  SendeVersSW = 1; break;
    case 'P':  SendeVersPF = 1; break;
  } 
}

// ---------------------------------------------------------------------------------------
// External Interrupt 0 service routine
#ifdef INCR_GEBER
interrupt [EXT_INT1] void ext_int1_isr(void)
{    
  unsigned char Taste2;
  
  Taste2 = TASTE2;
  
  if (MCUCR==0x08) // falling edge 
  {
    if (!Taste2) Taste = DOWN; else Taste = UP;
    MCUCR=0x0C;
  }
  else 
  {  MCUCR=0x08;  // raising edge
     if (!Taste2) Taste = UP; else Taste = DOWN;
  }
  GIFR |= 0x80;
}                      
#endif
// ---------------------------------------------------------------------------------------
#define CNT_TASTE 7

void Tastatur(void)
{

 static char taste1 = 0,taste2 = 0,taste3 = 0,taste4 = 0,taste5 = 0;  //,taste10 = 0;
 Taste = NICHTS;  
 
#ifdef ExternTast
  #ifdef INCR_GEBER

  #else
    if(TASTE1) { if(taste1++ == 0 || taste1 == CNT_TASTE) Taste = DOWN;    if(taste1 == CNT_TASTE) taste1 = 3;} else taste1 = 0;
    if(TASTE2) { if(taste2++ == 0 || taste2 == CNT_TASTE) Taste = UP;  if(taste2 == CNT_TASTE) taste2 = 3;} else taste2 = 0;
  #endif
    if(TASTE3) { if(taste3++ == 0) Taste = OKAY;} else taste3 = 0;
    if(TASTE4) { if(taste4++ == 0) Taste = MODE;} else taste4 = 0;
#else
  #ifdef INCR_GEBER

  #else
    if(TASTE2) { if(taste2++ == 0 || taste2 == CNT_TASTE) Taste = DOWN;    if(taste2 == CNT_TASTE) taste2 = 3;} else taste2 = 0;
    if(TASTE3) { if(taste3++ == 0 || taste3 == CNT_TASTE) Taste = UP;  if(taste3 == CNT_TASTE) taste3 = 3;} else taste3 = 0;
  #endif
    if(TASTE4) { if(taste4++ == 0) Taste = OKAY;} else taste4 = 0;
    if(TASTE5) { if(taste5++ == 0) Taste = MODE;} else taste5 = 0;
#endif


// if(TASTE10) { if(taste10++ == 0) Taste = SEN230V;} else taste10 = 0;
// if(TASTE10) {  Taste = SEN230V;}


//  if (Taste == SEN230V)
//  {LED_TEST = 1;
//     lcd_gotoxy(9,1); lcd_putchar('X'); }
//  else
//  {LED_TEST = 0;}
//  { lcd_gotoxy(9,1); lcd_putchar(' '); }
     

}                   

// ---------------------------------------------------------------------------------------
unsigned char ErmittleBetriebsMode(unsigned int zeit)
{
   unsigned char rueck = DEAKTIV, i;
   
   for (i = 0; i < 32; i++) 
   {  
      if (Timer[i+ SchichtOffset].Zeit & WERKTAG)                         // Sonderfall Werktags
      {  if (zeit  < (6*1440))                                          // akt. Tag ist Mo - Fr
         if (((Timer[i+ SchichtOffset].Zeit & ~WERKTAG) % 1440)== (zeit % 1440)) { rueck = Timer[i+ SchichtOffset].Mode; break;} // testen  
      }
      else 
      if (Timer[i+ SchichtOffset].Zeit == zeit) { rueck = Timer[i+ SchichtOffset].Mode; break; } 
   }
   return(rueck);
}

// ---------------------------------------------------------------------------------------
void MenuIstData(void)
{                             //1234567890123456
  static unsigned char index = 0, blink = 0, count = 0;
  static unsigned int BufUInt = 0;
  
  if (MenuPolling)
  {  lcd_gotoxy(2,0); lcd_putsf("             %");
     MenuPolling = 0;
   }                  

//#define LCD_XY(x,y) {_lcd_write_data(0x80 + x + (y * (0xc0-0x80)));}
   
  if (RefreshDisplay)
  { 
    #ifdef NO_RTC
    #else
    weekday = rtc_get_weekday(0);
    rtc_get_date(0,&date,&month,&year); 
    rtc_get_time(0,&hour,&minute,&second,&hundredsecond);           
    BufUInt = hour; 
    BufUInt *= 60; 
    AktuellZeit = BufUInt + weekday * 1440 + minute;
    #endif
    lcd_gotoxy(0,0); lcd_putsf(&Tag[AktuellZeit / 1440] [0]);
      
    lcd_gotoxy(3 ,0); lcd_int((AktuellZeit % 1440) / 60, 2 , LEADING_ZERO); 
    if (blink) 
    { blink = 0; 
      lcd_putchar(':'); 
    } 
    else 
    { blink = 1; 
      lcd_putchar(' '); 
    } 
    lcd_int((AktuellZeit % 1440) % 60, 2 ,LEADING_ZERO);
         
    #ifdef LCD_0416
     lcd_gotoxy(0,1);
     lcd_putsf("TL=");  lcd_int(IstTempLeit/10, -2, 0); lcd_putchar(GRAD_ZEICHEN); lcd_putchar(' ');
     lcd_putsf("Tk=");  lcd_int(IstTempKessel_OHz/10, -2, 0); lcd_putchar(GRAD_ZEICHEN);  lcd_putchar('C'); 
     lcd_gotoxy(0,2);
     lcd_putsf("Ta=");  lcd_int(IstTempAussen/10, -2, 0); lcd_putchar(GRAD_ZEICHEN); lcd_putchar(' ');
     lcd_putsf("Tw=");  lcd_int(IstTempWWasser/10, -2, 0); lcd_putchar(GRAD_ZEICHEN); lcd_putchar('C');

// //     lcd_gotoxy(0,1); 
// //     lcd_putsf("Ta="); lcd_int(IstTempAussen/10, -2, 0); lcd_putchar(GRAD_ZEICHEN); lcd_putchar(' '); 
// //     lcd_putsf("Tw="); lcd_int(IstTempWWasser/10, -2, 0); lcd_putchar(GRAD_ZEICHEN); lcd_putchar('C');

//     lcd_gotoxy(0,1); 
//     lcd_putsf("Ta="); lcd_int(IstTempAussen/10, -2, 0); lcd_putchar(GRAD_ZEICHEN); lcd_putchar(' '); 
//     lcd_putsf("Tk="); lcd_int(TemperaturVal_Sensor01/10, -2, 0); lcd_putchar(GRAD_ZEICHEN); lcd_putchar('C');

// //     lcd_gotoxy(0,2); 
// //     lcd_putsf("Ts="); lcd_int(SollTempLeit/10, -2, 0); lcd_putchar(GRAD_ZEICHEN); lcd_putchar(' ');
// //     lcd_putsf("TL="); lcd_int(IstTempLeit/10, -2, 0); lcd_putchar(GRAD_ZEICHEN); lcd_putchar('C');

//     if (count++ < 32) 
//     { lcd_putsf("Ts=");  lcd_int(SollTempWWasser/10, -2, 0); 
//     }                                                         
//     else          
//     { lcd_putsf("Ts=");  lcd_int(SollTempLeit/10, -2, 0); 
//     }                                                         
//   
//     lcd_putchar(GRAD_ZEICHEN); lcd_putchar(' ');
//     if (count < 32) 
//     { lcd_putsf("Tw=");  lcd_int(IstTempWWasser/10, -2, 0); lcd_putchar(GRAD_ZEICHEN); lcd_putchar('C');
//     }                                                         
//     else
//     { lcd_putsf("TL=");  lcd_int(IstTempLeit/10, -2, 0); lcd_putchar(GRAD_ZEICHEN); lcd_putchar('C');
//     }                                                         


//     lcd_gotoxy(0,3);
//     lcd_putsf("BRE.St:");lcd_int(CountStartBRE, 3, 0); 
//     lcd_putsf("/");lcd_int(CounterStartBRE_Gesamt, 5, 0); 

    #else
       
#ifdef ExternTast
     lcd_gotoxy(0,1); 
     count++;
     if (count < 8) 
     { 
       lcd_putsf("Ta=");  lcd_int(IstTempAussen/10, -2, 0); lcd_putchar(GRAD_ZEICHEN); lcd_putchar(' ');
       lcd_putsf("Tk=");  lcd_int(IstTempKessel_OHz/10, -2, 0); lcd_putchar(GRAD_ZEICHEN);  lcd_putchar('C'); 
     }                                                         
     if ((count > 8) && (count < 16))
     { 
       lcd_putsf("Tw=");  lcd_int(IstTempWWasser/10, -2, 0); lcd_putchar(GRAD_ZEICHEN); lcd_putchar(' ');
       lcd_putsf("TL=");  lcd_int(IstTempLeit/10, -2, 0); lcd_putchar(GRAD_ZEICHEN); lcd_putchar('C');
     }                                                         
     if ((count > 16) && (count < 24))
     { 
       lcd_putsf("P1=");  lcd_int(TempValSen_P1/10, -2, 0); lcd_putchar(GRAD_ZEICHEN); lcd_putchar(' ');
       lcd_putsf("P2=");  lcd_int(TempValSen_P2/10, -2, 0); lcd_putchar(GRAD_ZEICHEN); lcd_putchar('C');
     }                                                         
     if ((count > 24) && (count < 32))
     { 
       lcd_putsf("P3=");  lcd_int(TempValSen_P3/10, -2, 0); lcd_putchar(GRAD_ZEICHEN); lcd_putchar(' ');
       lcd_putsf("AG=");  lcd_int(TempValSen_AG/10, -2, 0); lcd_putchar(GRAD_ZEICHEN); lcd_putchar('C');
     }                                                         
#else
     lcd_gotoxy(0,1); 
     count++;
     if (count < 8) 
     { 
       lcd_putsf("TL=");  lcd_int(IstTempLeit/10, -2, 0); lcd_putchar(GRAD_ZEICHEN); lcd_putchar(' ');
       lcd_putsf("Tk=");  lcd_int(IstTempKessel_OHz/10, -2, 0); lcd_putchar(GRAD_ZEICHEN);  lcd_putchar('C'); 
     }                                                         
     if ((count > 8) && (count < 16))
     { 
       lcd_putsf("Ta=");  lcd_int(IstTempAussen/10, -2, 0); lcd_putchar(GRAD_ZEICHEN); lcd_putchar(' ');
       lcd_putsf("Tw=");  lcd_int(IstTempWWasser/10, -2, 0); lcd_putchar(GRAD_ZEICHEN); lcd_putchar('C');
     }                                                         

#endif  
     

    #endif

    
       

//////////   lcd_putsf("TL=");  lcd_int(IstTempLeit/10, -2, 0); lcd_putchar(GRAD_ZEICHEN); lcd_putchar('C');
 
   lcd_gotoxy(12,0); lcd_int(((long) CountHeizzeit * 100) / CountZeit, 3, 0);
 
 
   lcd_gotoxy(10,0);
   switch (BetriebsMode)
     {
       case TAG:   
                       lcd_putchar(BetriebsMode+1);
          break;
       case NACHT:  
                       lcd_putchar(BetriebsMode+1);
          break;
       case ABWESEND:
                       lcd_putchar(BetriebsMode+1);
          break;
       case DEAKTIV: 
                       lcd_putchar(' ');
          break;
     }
   RefreshDisplay = 0; 
  } 

//   lcd_gotoxy(8,0); lcd_putchar(6); lcd_putchar(7);

                       

#ifdef EXTRA_MENU  
    if (index == 0 && (Taste == UP || Taste == DOWN)) { MenuCount = 255; MenuPolling =1;}
#endif
  
    #ifdef NO_RTC
    switch(index)
     { case 0: break;
     
       case 1:        
                lcd_gotoxy(2,0); lcd_putchar('\x7e');  
                if (Taste == UP) AktuellZeit += 60; 
                 else if (Taste == DOWN) if (AktuellZeit > 60) AktuellZeit -= 60; 
            break;
       case 2: 
               lcd_gotoxy(2,0); lcd_putchar(' ');  
               lcd_gotoxy(8,0); lcd_putchar('\x7f');
               if (Taste == UP) AktuellZeit ++; 
                else if (Taste == DOWN) if (AktuellZeit > 0) AktuellZeit --;
            break;
       case 3: 
                 lcd_gotoxy(8,0); lcd_putchar(' ');
                 lcd_gotoxy(2,0); lcd_putchar('\x7f');
                 if (Taste == UP) if (AktuellZeit < 6*1440)  AktuellZeit += 1440; else AktuellZeit %= 1440;
                   else if (Taste == DOWN) if (AktuellZeit > 1440) AktuellZeit -=1440; else AktuellZeit += 6*1440;
            break;
     }
     
     if (Taste == OKAY) 
     {
        if (index < 3) { index++; }
        else 
        { 
          index = 0;      
          MenuPolling = 1;
        }
     }                     
    #endif
#ifdef ExternTast
   if (count > 32)  count = 0;
#else
   if (count > 16)  count = 0;
#endif 
   
  


}
// ---------------------------------------------------------------------------------------
unsigned char EditTimer(unsigned char pos)
{
  static unsigned int tmp_wert, zeit, tagZeit;
  static unsigned char tmp_mode, index = 0;
  
  pos += SchichtOffset;
  
 if(MenuPolling)
 {
    MenuPolling = 0;
    lcd_gotoxy(2,0); lcd_putsf("              ");

   lcd_gotoxy(0,0);
   if (Timer[pos].Zeit & WERKTAG) lcd_putsf("WT");
     else lcd_putsf(&Tag[Timer[pos].Zeit % (ZEIT_MASKE) / 1440] [0]);
    
  
   lcd_gotoxy(0,1); lcd_putsf("Mode   ");
   zeit = Timer[pos].Zeit & ~(0x4000+0x8000);
   tmp_mode = Timer[pos].Mode;
   tmp_wert = zeit;                            
   tagZeit = (zeit / 1440)*1440; 
   if (Timer[pos].Zeit & WERKTAG) tagZeit = WERKTAG;
  }
   
     lcd_gotoxy(7 ,0); lcd_int((tmp_wert % 1440) / 60, 2, LEADING_ZERO); lcd_putchar(':');
                       lcd_int((tmp_wert % 1440) % 60, 2, LEADING_ZERO);

     lcd_gotoxy(7,1);
     switch(tmp_mode)
     {
       case TAG:      lcd_putsf("Tag      "); 
                       lcd_gotoxy(14,0);
                       lcd_putchar(tmp_mode+1);
          break;
       case NACHT:    lcd_putsf("Nacht    "); 
                       lcd_gotoxy(14,0);
                       lcd_putchar(tmp_mode+1);
          break;
       case ABWESEND: lcd_putsf("Abwesend "); 
                       lcd_gotoxy(14,0);
                       lcd_putchar(tmp_mode+1);
          break;
       case DEAKTIV: lcd_putsf("DEAKTIV  "); 
                       lcd_gotoxy(14,0);
                       lcd_putchar('-');
          break;
     }

     switch(index)
     { case 0:
                lcd_gotoxy(6,0); lcd_putchar('\x7e');  
                if (Taste == UP) tmp_wert += 60; 
                 else if (Taste == DOWN) tmp_wert -= 60;
            break;
       case 1: 
               lcd_gotoxy(6,0); lcd_putchar(' ');  
               lcd_gotoxy(12,0); lcd_putchar('\x7f');
               if (Taste == UP) tmp_wert ++; 
                else if (Taste == DOWN) tmp_wert --;
            break;
       case 2: 
                 lcd_gotoxy(12,0); lcd_putchar(' ');  
                 lcd_gotoxy(6,1); lcd_putchar('\x7e');
                 if (Taste == UP) if (tmp_mode < 3) tmp_mode++; else tmp_mode = 0;
                else if (Taste == DOWN) if (tmp_mode > 0) tmp_mode--; else tmp_mode = 3;
            break;
     }
     
     if (Taste == OKAY) 
     {
        if (index < 2) { index++;  }
        else 
        { 
          Timer[pos].Mode = tmp_mode;
          Timer[pos].Zeit = (tmp_wert % 1440) + tagZeit;
          index = 0;      
          MenuPolling = 1;
          return(1);
        }
     }                     
   return(0);
}

// ---------------------------------------------------------------------------------------
void ZeigTimer(char x, char y, unsigned char pos)
{
  char sym[] = "\1\2\3";
  unsigned int zeit;  
  
  pos += SchichtOffset;
  
  zeit = Timer[pos].Zeit & ~(0x4000+0x8000);
  
  lcd_gotoxy(x,y);
     
  if (Timer[pos].Mode == DEAKTIV) lcd_putsf(" --:--"); 
   else 
   {  lcd_putchar(sym[Timer[pos].Mode]);
      lcd_int((zeit % 1440) / 60,2, LEADING_ZERO); lcd_putchar(':'); lcd_int((zeit % 1440) % 60, 2, LEADING_ZERO); 
   }
}
// ---------------------------------------------------------------------------------------
void MenuTimer(void)
{                 
  static unsigned char pos=0, index=0, edit = 0,clearlcd = 1;

  if (MenuPolling) 
   { //pos=0; index=0; 
     edit = 0; MenuPolling = 0; clearlcd = 1;
   };
  
  if (edit == 2)  // EDIT TIMER
  {
    if(EditTimer(pos+index)) edit = 0;   
  
  }
  else       // Select Timer
  {
  if (clearlcd)
  {  
     lcd_gotoxy(0,0);
     if (Timer[pos].Zeit & WERKTAG) lcd_putsf("WT");                           
      else lcd_putsf(&Tag[Timer[pos].Zeit % (ZEIT_MASKE) / 1440] [0]);
      
     lcd_gotoxy(0,1); lcd_int((SchichtOffset/32)+1, 1, 0);
     
     ZeigTimer(3 ,0, pos);
     ZeigTimer(10,0, pos+1);
     ZeigTimer(3 ,1, pos+2);
     ZeigTimer(10,1, pos+3);
     clearlcd = 0;   
     lcd_gotoxy(2,0); lcd_putchar(' ');   lcd_gotoxy(9,0); lcd_putchar(' '); 
     lcd_gotoxy(1,1); lcd_putchar(' ');   lcd_putchar(' ');  lcd_gotoxy(9,1); lcd_putchar(' '); 
  }                  
  
  clearlcd = 1; 
  
  if (edit == 0)
  { if(Taste == UP) if (pos < 28) pos += 4; 
     else
        { pos = 0; 
          #ifdef EXTRA_MENU
          MenuCount = 255; 
          #endif
          }
    else if(Taste == DOWN) if (pos > 0) pos -= 4; else pos = 28;
    else clearlcd = 0;    
  }
  else
  {
    if (Taste==UP) if(index < 3) index++; else index = 0;
    else if (Taste==DOWN) if (index > 0) index--; else index = 3; 
    else clearlcd = 0;
    switch(index)
    {
      case 0: lcd_gotoxy(2,0); break;
      case 1: lcd_gotoxy(9,0); break;
      case 2: lcd_gotoxy(2,1); break;
      case 3: lcd_gotoxy(9,1); break;
      
    }
    lcd_putchar('\x7e'); 
  }
  
  if (Taste == OKAY) 
   { if (edit == 0) edit = 1;
      else if (edit == 1) 
      {  edit = 2;               
         MenuPolling = 1;
         Taste = NICHTS;
         EditTimer(pos+index);
      }   
      clearlcd = 1;        
       
   }
 }   
}

// ---------------------------------------------------------------------------------------
void MenuSchicht(void)
{                          //1234567890123456
  if (MenuPolling)
  { // lcd_gotoxy(0,0); lcd_putsf("Timer - Gruppe   ");
     lcd_gotoxy(0,1); lcd_putsf("setzen");
     MenuPolling = 0;
  }                  
                                            
  lcd_gotoxy(10,1); lcd_int(SchichtOffset / 32 +1, 1, 0);
  
  if ((Taste == UP) || (Taste == DOWN)) 
  {  if (!SchichtOffset) SchichtOffset = 32; else SchichtOffset = 0;
  }
  
#ifdef EXTRA_MENU
   if (Taste == OKAY) { MenuCount = 255; };
 #endif    
}

// ---------------------------------------------------------------------------------------
void MenuDateTime(void)
{                          //1234567890123456
    static unsigned char index = 0;
    unsigned int ByteVal, ByteBuf, RTC_YEAR;
    char lcd_buffer[33];
  if (MenuPolling)
  {
     MenuPolling = 0;
  }                  

  if (RefreshDispDateTime)
  {
    weekday = rtc_get_weekday(0); 
    RTC_YEAR = rtc_get_year(0);
    rtc_get_date(0,&date,&month,&year); 
    lcd_clear();
    lcd_gotoxy(1,0); lcd_putsf(&Tag[weekday] [0]);
    lcd_gotoxy(4,0); lcd_int(date, 2 , LEADING_ZERO); lcd_putsf(".");
                     lcd_int(month, 2 , LEADING_ZERO); lcd_putsf(".20");
//                     lcd_int(year, 2 , LEADING_ZERO);
                     lcd_int(eeValYear, 2 , LEADING_ZERO);
       
    rtc_get_time(0,&hour,&minute,&second,&hundredsecond);           
    lcd_gotoxy(4,1); lcd_int(hour, 2 , LEADING_ZERO); lcd_putsf(":");
                     lcd_int(minute, 2 , LEADING_ZERO); lcd_putsf(":");
                     lcd_int(second, 2 , LEADING_ZERO);
                 
//    lcd_gotoxy(1,1); lcd_int(RTC_YEAR, 2 , LEADING_ZERO);
 
    RefreshDispDateTime = 0; 
   } 
  
 #ifdef EXTRA_MENU
    if (index == 0 && (Taste == UP || Taste == DOWN)) { MenuCount = 255; MenuPolling =1;}
 #endif    
      
   switch(index)
   { case 0: RefreshDispDateTime = 1; 
             break;
     
     case 1: lcd_gotoxy(6,0); lcd_putchar('\x7f');  
             if (Taste == UP)   date += 1; 
             if (Taste == DOWN) date -= 1; 
             if (date == 32)    date = 1; 
             if (date == 0)     date = 31; 
             rtc_set_date(0,date,month,year);
             //rtc_set_weekday(0, weekday);
             RefreshDispDateTime = 1; 
             break;
     case 2: 
             lcd_gotoxy(6,0); lcd_putchar('.');  
             lcd_gotoxy(9,0); lcd_putchar('\x7f');
             if (Taste == UP) month += 1; 
             if (Taste == DOWN)  month -= 1; 
             if (month > 12) month = 1; 
             if (month < 1)  month = 12; 
             rtc_set_date(0,date,month,year);
             //rtc_set_weekday(0, weekday);
             RefreshDispDateTime = 1; 
             break;
     case 3: 
             lcd_gotoxy(9,0); lcd_putchar('.');
             lcd_gotoxy(14,0); lcd_putchar('\x7f');
             if (Taste == UP) 
             {
               eeValYear += 1;
 
             //  
             //  lcd_gotoxy(0,1); lcd_int(year, 2 , LEADING_ZERO);
             //  if (eeValYear > 99) eeValYear = 0; 
             } ;
             //else
             
             //{
             if (Taste == DOWN)
             {
               eeValYear -= 1;  
               
             //     
             //    lcd_gotoxy(14,1); lcd_int(year, 2 , LEADING_ZERO);
             //    if (year > 99 )  year = 0; 
             //  } ;
             };
             if (eeValYear > 99) eeValYear = 0; 
             rtc_set_year(0, eeValYear);

//             rtc_set_date(0,date,month,year);
             //rtc_set_weekday(0, weekday);
             RefreshDispDateTime = 1; 
             break;
     case 4: 
             lcd_gotoxy(14,0); lcd_putchar(' ');
             lcd_gotoxy(3,0); lcd_putchar('\x7f');
             if (Taste == UP) 
             { 
               weekday += 1;
             }
             else
             {
               if (Taste == DOWN)  weekday -= 1; 
             };
             if (weekday > 6 ) weekday = 0; 
             rtc_set_weekday(0, weekday);
             RefreshDispDateTime = 1; 
             break;
     case 5: 
             lcd_gotoxy(3,0); lcd_putchar(' ');
             lcd_gotoxy(6,1); lcd_putchar('\x7f');
             if (Taste == UP) 
             { 
               hour += 1;
               if (hour > 23) hour = 0; 
             }
             else
             {
               if (Taste == DOWN)  hour -= 1; 
               if (hour == 255)  hour = 23; 
             };
             rtc_set_time(0,hour,minute,second,0);
             RefreshDispDateTime = 1; 
             break;
     case 6: 
             lcd_gotoxy(6,1); lcd_putchar(':');
             lcd_gotoxy(9,1); lcd_putchar('\x7f');
             if (Taste == UP) 
             { 
               minute += 1;
               if (minute > 59) minute = 0; 
             }
             else
             {
               if (Taste == DOWN)  minute -= 1; 
               if (minute == 255)  minute = 59; 
             };
             rtc_set_time(0,hour,minute,second,0);
             RefreshDispDateTime = 1; 
             break;
     case 7: 
             lcd_gotoxy(9,1); lcd_putchar(':');
             lcd_gotoxy(12,1); lcd_putchar('\x7f');
             if (Taste == UP) 
             { 
               second += 1;
               if (second > 59) second = 0; 
             }
             else
             {
               if (Taste == DOWN)  second -= 1; 
               if (second == 255)  second = 59; 
             };
             rtc_set_time(0,hour,minute,second,0);
             RefreshDispDateTime = 1; 
             break;
   }
     
   if (Taste == OKAY) 
   {
     if (index < 7) { index++; }
     else 
     { 
       index = 0;      
       MenuPolling = 1;
       RefreshDispDateTime = 1; 
     }
   }                     
  
}

// ---------------------------------------------------------------------------------------
void MenuTemperatur(void)
{                          //1234567890123456
  static unsigned char kennlinie = 0, index =0;
  
  if (MenuPolling)
  {                            //1234567890123456
     lcd_gotoxy(0,0); lcd_putsf("  \x4   \xdf bei    \xdf");
     lcd_gotoxy(0,1); lcd_putsf("  \x5   \xdf bei    \xdf");
     MenuPolling = 0;
  }                  
  
  lcd_gotoxy(0,0); lcd_putchar(kennlinie+1);
  
  lcd_gotoxy(4,0);  lcd_int( MaxLeitTemperatur[kennlinie]/10, 2, 0); 
  lcd_gotoxy(4,1);  lcd_int( MinLeitTemperatur[kennlinie]/10, 2, 0); 
  lcd_gotoxy(12,0); lcd_int( KennlinieTemperaturOben[kennlinie]/10, -2, 0); 
  lcd_gotoxy(12,1); lcd_int( KennlinieTemperaturUnten[kennlinie]/10, -2, 0); 
 
  switch (index)
  {
    case 0:
            lcd_gotoxy(11,1); lcd_putchar(' ');
            if (Taste == UP)   if (kennlinie < 2) kennlinie++; else 
            { kennlinie=0;
              #ifdef EXTRA_MENU
                MenuCount = 255; 
              #endif    
            }
            
            if (Taste == DOWN) if (kennlinie > 0) kennlinie--; else kennlinie=2;
        break;
  
    case 1:
               lcd_gotoxy(7,0); lcd_putchar('\x7f');
            if (Taste == UP)   MaxLeitTemperatur[kennlinie]+=10;
            if (Taste == DOWN) MaxLeitTemperatur[kennlinie]-=10;
        break;
    case 2:
               lcd_gotoxy(7,0); lcd_putchar(' ');  
               lcd_gotoxy(11,0); lcd_putchar('\x7e');
            if (Taste == UP)   KennlinieTemperaturOben[kennlinie]+=10;
            if (Taste == DOWN) KennlinieTemperaturOben[kennlinie]-=10;
        break;
    case 3:
               lcd_gotoxy(11,0); lcd_putchar(' ');  
               lcd_gotoxy(7,1); lcd_putchar('\x7f');
            if (Taste == UP)   MinLeitTemperatur[kennlinie]+=10;
            if (Taste == DOWN) MinLeitTemperatur[kennlinie]-=10;
        break;

    case 4:
               lcd_gotoxy(7,1); lcd_putchar(' ');  
               lcd_gotoxy(11,1); lcd_putchar('\x7e');
            if (Taste == UP)   KennlinieTemperaturUnten[kennlinie]+=10;
            if (Taste == DOWN) KennlinieTemperaturUnten[kennlinie]-=10;
        break;
  }
  
  if (Taste == OKAY) if (index <4) index++; else index = 0;

}
// ---------------------------------------------------------------------------------------
void MenuRaumTemperatur(void)
{                          
  static unsigned char tmp_mode=0;
  
  if (MenuPolling)                          
  {  
     MenuPolling = 0;
     
//   lcd_gotoxy(0,0); lcd_putsf("Pumpe aus       ");
     lcd_gotoxy(0,0); lcd_putsf("Temp. Pilotraum ");
     lcd_gotoxy(0,1); lcd_putsf("      T1 =    \xdfC");
     
     switch(tmp_mode)
     { case NACHT:                       //1234567890123456
//                lcd_gotoxy(12,0); lcd_putchar(tmp_mode+1);
                lcd_gotoxy(0,1); lcd_putsf("Nacht");
        break; 
     
       case TAG:
//                lcd_gotoxy(12,0); lcd_putchar(tmp_mode+1);
                lcd_gotoxy(0,1); lcd_putsf("Tag");
        break; 
     
      case ABWESEND:
//                lcd_gotoxy(12,0); lcd_putchar(tmp_mode+1);
                lcd_gotoxy(0,1); lcd_putsf("Abw.");
        break; 
     }
  }                  
                                            
  lcd_gotoxy(11,1);
  lcd_int( PumpeEinTemperatur[tmp_mode]/10, -2, 0);
 
  switch (tmp_mode)
  {
    case 0:
    case 1:
    case 2:
            if (Taste == UP)   PumpeEinTemperatur[tmp_mode]+=10;
            if (Taste == DOWN) PumpeEinTemperatur[tmp_mode]-=10;
        break;
  
   }

  if (Taste == OKAY) 
   {  if (tmp_mode < 2) tmp_mode++; else 
      { tmp_mode = 0;
        #ifdef EXTRA_MENU
        MenuCount = 255; 
        #endif    
      }
      MenuPolling = 1;
   }   
}

// ---------------------------------------------------------------------------------------
void MenuPumpen(void)
{                          
  static unsigned char tmp_mode=0;
  
  if (MenuPolling)                          
  {  
     MenuPolling = 0;
     
     lcd_gotoxy(0,0); lcd_putsf("Pumpe aus       ");
     lcd_gotoxy(0,1); lcd_putsf("      TL <    \xdfC");
     
     switch(tmp_mode)
     { case NACHT:                       //1234567890123456
                lcd_gotoxy(12,0); lcd_putchar(tmp_mode+1);
                lcd_gotoxy(0,1); lcd_putsf("Nacht");
        break; 
     
       case TAG:
                lcd_gotoxy(12,0); lcd_putchar(tmp_mode+1);
                lcd_gotoxy(0,1); lcd_putsf("Tag");
        break; 
     
      case ABWESEND:
                lcd_gotoxy(12,0); lcd_putchar(tmp_mode+1);
                lcd_gotoxy(0,1); lcd_putsf("Abw.");
        break; 
     }
  }                  
                                            
  lcd_gotoxy(11,1);
  lcd_int( PumpeEinTemperatur[tmp_mode]/10, -2, 0);
 
  switch (tmp_mode)
  {
    case 0:
    case 1:
    case 2:
            if (Taste == UP)   PumpeEinTemperatur[tmp_mode]+=10;
            if (Taste == DOWN) PumpeEinTemperatur[tmp_mode]-=10;
        break;
  
   }

  if (Taste == OKAY) 
   {  if (tmp_mode < 2) tmp_mode++; else 
      { tmp_mode = 0;
        #ifdef EXTRA_MENU
        MenuCount = 255; 
        #endif    
      }
      MenuPolling = 1;
   }   
}

// ---------------------------------------------------------------------------------------
void MenuWarmWasser(void)
{                          
  static unsigned char tmp_mode=0;
  
  if (MenuPolling)                          
  {  
     MenuPolling = 0;
     
     lcd_gotoxy(0,0); lcd_putsf("Warmwasser bei:  ");
//     lcd_gotoxy(0,1); lcd_putsf("      TL <    \xdfC");
     lcd_gotoxy(0,1); lcd_putsf("      Tw <    \xdfC");
     
     switch(tmp_mode)
     { case NACHT:                       //1234567890123456
//                lcd_gotoxy(12,0); lcd_putchar(tmp_mode+1);
                lcd_gotoxy(0,1); lcd_putsf("Nacht");
                break; 
     
       case TAG:
//                lcd_gotoxy(12,0); lcd_putchar(tmp_mode+1);
                lcd_gotoxy(0,1); lcd_putsf("Tag");
                break; 
     
      case ABWESEND:
//                lcd_gotoxy(12,0); lcd_putchar(tmp_mode+1);
                lcd_gotoxy(0,1); lcd_putsf("Abw.");
                break; 
     }
  }                  
                                            
  lcd_gotoxy(11,1);
  lcd_int( WarmWasserTemperatur[tmp_mode]/10, -2, 0);
 
  switch (tmp_mode)
  {
    case 0:
    case 1:
    case 2:
            if (Taste == UP)   WarmWasserTemperatur[tmp_mode]+=10;
            if (Taste == DOWN) WarmWasserTemperatur[tmp_mode]-=10;
        break;
  
   }

  if (Taste == OKAY) 
   {  if (tmp_mode < 2) tmp_mode++; else 
      { tmp_mode = 0;
        #ifdef EXTRA_MENU
        MenuCount = 255; 
        #endif    
      }
      MenuPolling = 1;
   }   
}
// ---------------------------------------------------------------------------------------
void MenuTempSensorKorr(void)
{                          
  static unsigned char tmp_mode=0;
  
  if (MenuPolling)                          
  {  
     MenuPolling = 0;
     
     lcd_gotoxy(0,0); lcd_putsf("Korrekturwert   ");
     lcd_gotoxy(0,1); lcd_putsf("f\xf5r:         \xdfC");
//     lcd_gotoxy(0,1); lcd_putsf("      Tw <    \xdfC");
     
     lcd_gotoxy(5,1); 
     switch(tmp_mode)
     { case 0:                       //1234567890123456
                lcd_putsf("Ta = ");
                lcd_int(SensorKorrWert_TA/10, -2, 0);
                break; 
       case 1:
                lcd_putsf("TL = ");
                lcd_int(SensorKorrWert_TL/10, -2, 0);
                break; 
       case 2:
                lcd_putsf("Tw = ");
                lcd_int(SensorKorrWert_TW/10, -2, 0);
                break; 
       case 3:
                lcd_putsf("Tk = ");
                lcd_int(SensorKorrWert_TK/10, -2, 0);
                break; 
       case 4:
                lcd_putsf("P1 = ");
                lcd_int(SensorKorrWert_P1/10, -2, 0);
                break; 
       case 5:
                lcd_putsf("P2 = ");
                lcd_int(SensorKorrWert_P2/10, -2, 0);
                break; 
       case 6:
                lcd_putsf("P3 = ");
                lcd_int(SensorKorrWert_P3/10, -2, 0);
                break; 
       case 7:
                lcd_putsf("AG = ");
                lcd_int(SensorKorrWert_AG/10, -2, 0);
                break; 
     }
  }                  
                                            
  lcd_gotoxy(10,1);
 
  switch (tmp_mode)
  {
    case 0:
             if (Taste == UP)   SensorKorrWert_TA += 10;
             if (Taste == DOWN) SensorKorrWert_TA -= 10;
             lcd_int(SensorKorrWert_TA/10, -2, 0);
             break;
    case 1:
             if (Taste == UP)   SensorKorrWert_TL += 10;
             if (Taste == DOWN) SensorKorrWert_TL -= 10;
             lcd_int(SensorKorrWert_TL/10, -2, 0);
             break;
    case 2:
             if (Taste == UP)   SensorKorrWert_TW += 10;
             if (Taste == DOWN) SensorKorrWert_TW -= 10;
             lcd_int(SensorKorrWert_TW/10, -2, 0);
             break;
    case 3:
             if (Taste == UP)   SensorKorrWert_TK += 10;
             if (Taste == DOWN) SensorKorrWert_TK -= 10;
             lcd_int(SensorKorrWert_TK/10, -2, 0);
             break;
    case 4:
             if (Taste == UP)   SensorKorrWert_P1 += 10;
             if (Taste == DOWN) SensorKorrWert_P1 -= 10;
             lcd_int(SensorKorrWert_P1/10, -2, 0);
             break;
    case 5:
             if (Taste == UP)   SensorKorrWert_P2 += 10;
             if (Taste == DOWN) SensorKorrWert_P2 -= 10;
             lcd_int(SensorKorrWert_P2/10, -2, 0);
             break;
    case 6:
             if (Taste == UP)   SensorKorrWert_P3 += 10;
             if (Taste == DOWN) SensorKorrWert_P3 -= 10;
             lcd_int(SensorKorrWert_P3/10, -2, 0);
             break;
    case 7:
             if (Taste == UP)   SensorKorrWert_AG += 10;
             if (Taste == DOWN) SensorKorrWert_AG -= 10;
             lcd_int(SensorKorrWert_AG/10, -2, 0);
             break;
  
   }

  if (Taste == OKAY) 
   {  if (tmp_mode < 7) tmp_mode++; else 
      { tmp_mode = 0;
        #ifdef EXTRA_MENU
        MenuCount = 255; 
        #endif    
      }
      MenuPolling = 1;
   }   
}

// ---------------------------------------------------------------------------------------
void MenuMainSettings(void)
{                          
  static unsigned char tmp_mode=0;
  
  if (MenuPolling)                          
  {  
     MenuPolling = 0;
     
     lcd_gotoxy(0,0); lcd_putsf("Einst. Hysterese");
     lcd_gotoxy(0,1); lcd_putsf("          \xdfC");
     
     lcd_gotoxy(1,1); 
     switch(tmp_mode)
     { case 0:
                lcd_putsf("HTL = ");
                lcd_int(HystereseTempLeit/10, -2, 0);
                break; 
     
       case 1:
                lcd_putsf("HTW = ");
                lcd_int(HystereseWarmWasser/10, -2, 0);
                break; 

       case 2:
                lcd_gotoxy(0,0); lcd_putsf("Einst. MAX-Temp.");
                lcd_gotoxy(0,1); lcd_putsf("             \xdfC");
                lcd_gotoxy(1,1); 
                lcd_putsf("Leit = ");
                lcd_int(HystereseWarmWasser/10, -2, 0);
                break; 
     }
  }                  

                                            
  lcd_gotoxy(7,1);
 
  switch (tmp_mode)
  {
    case 0:
             if (Taste == UP)   HystereseTempLeit += 10;
             if (HystereseTempLeit > HYSTERESE_TEMPLEIT_MAX) HystereseTempLeit -= 10;
             if (Taste == DOWN) HystereseTempLeit -= 10;
             if (HystereseTempLeit < HYSTERESE_TEMPLEIT_MIN) HystereseTempLeit += 10;
             lcd_int(HystereseTempLeit/10, -2, 0);
             break;

    case 1:
             if (Taste == UP)   HystereseWarmWasser += 10;
             if (HystereseWarmWasser > HYSTERESE_WARMWASSER_MAX) HystereseWarmWasser -= 10;
             if (Taste == DOWN) HystereseWarmWasser -= 10;
             if (HystereseWarmWasser < HYSTERESE_WARMWASSER_MIN) HystereseWarmWasser += 10;
             lcd_int(HystereseWarmWasser/10, -2, 0);
             break;  
    case 2:
             lcd_gotoxy(10,1);
             if (Taste == UP)   TempLeitMax_TL += 10;
             if (TempLeitMax_TL > TEMPERATUR_LEIT_MAX) TempLeitMax_TL -= 10;
             if (Taste == DOWN) TempLeitMax_TL -= 10;
             if (TempLeitMax_TL < TEMPERATUR_LEIT_MIN) TempLeitMax_TL += 10;
             lcd_int(TempLeitMax_TL/10, -2, 0);
             break;  
   }

  if (Taste == OKAY) 
   {  if (tmp_mode < 2) tmp_mode++; else 
      { tmp_mode = 0;
        #ifdef EXTRA_MENU
        MenuCount = 255; 
        #endif    
      }
      MenuPolling = 1;
   }   
}
// ---------------------------------------------------------------------------------------
void MenuService(void)
{                          
  static unsigned char tmp_mode=0;
  
  if (MenuPolling)                          
  {  
     MenuPolling = 0;
     
     lcd_gotoxy(0,0); lcd_putsf("Servicemode     ");
     lcd_gotoxy(12,0); lcd_putsf(txt_OnOff[ServiceMode]);

//     if (ServiceMode == 0) lcd_putsf("AUS");
//     if (ServiceMode == 1) lcd_putsf("EIN");
//     lcd_gotoxy(0,1); lcd_putsf("fuer:         \xdfC");
//     lcd_gotoxy(0,1); lcd_putsf("      Tw <    \xdfC");
     
//     lcd_gotoxy(6,1); 
     lcd_gotoxy(0,1); 
     switch(tmp_mode)
     { case 0:                       //1234567890123456
                lcd_gotoxy(0,0); lcd_putsf("Servicemode     ");
                lcd_gotoxy(12,0); lcd_putsf(txt_OnOff[ServiceMode]);
                break; 
     
       case 1:
                lcd_putsf("Erz.Spe.OHz ");
                lcd_gotoxy(12,1); lcd_putsf(txt_OnOff[RELAIS_ERZEUGERSPERRE_OELHZ]);
//                if (RELAIS_LEIT == 1) lcd_putsf("AUS");
//                if (RELAIS_LEIT == 0) lcd_putsf("EIN");
                break; 
     
      case 2:
                lcd_putsf("KWP ");
                lcd_gotoxy(12,1); lcd_putsf(txt_OnOff[RELAIS_KPUMPE]);
//                if (RELAIS_KPUMPE == 1) lcd_putsf("AUS");
//                if (RELAIS_KPUMPE == 0) lcd_putsf("EIN");
                break; 
      case 3:
                lcd_putsf("WWP ");
                lcd_gotoxy(12,1); lcd_putsf(txt_OnOff[RELAIS_WPUMPE]);
//                if (RELAIS_WPUMPE == 1) lcd_putsf("AUS");
//                if (RELAIS_WPUMPE == 0) lcd_putsf("EIN");
                break; 
     }
  }                  

  lcd_gotoxy(11,1);
 
  switch (tmp_mode)
  {
    case 0:
             if ((Taste == UP) || (Taste == DOWN)) 
             {  if (ServiceMode == ON) ServiceMode = OFF; else ServiceMode = ON;
             }
             lcd_gotoxy(12,0); lcd_putsf(txt_OnOff[ServiceMode]);
             break;
    case 1:
             if (ServiceMode == ON)
             { if ((Taste == UP) || (Taste == DOWN)) 
               {  if (RELAIS_ERZEUGERSPERRE_OELHZ == ON) RELAIS_ERZEUGERSPERRE_OELHZ = OFF; else RELAIS_ERZEUGERSPERRE_OELHZ = ON;
               }
               lcd_gotoxy(12,1); lcd_putsf(txt_OnOff[RELAIS_ERZEUGERSPERRE_OELHZ]);
             }
             break;
    case 2:
             if (ServiceMode == ON)
             { if ((Taste == UP) || (Taste == DOWN)) 
               {  if (RELAIS_KPUMPE == ON) RELAIS_KPUMPE = OFF; else RELAIS_KPUMPE = ON;
               }
               lcd_gotoxy(12,1); lcd_putsf(txt_OnOff[RELAIS_KPUMPE]);
             }
             break;
    case 3:
             if (ServiceMode == ON)
             {  if ((Taste == UP) || (Taste == DOWN)) 
               {  if (RELAIS_WPUMPE == ON) RELAIS_WPUMPE = OFF; else RELAIS_WPUMPE = ON;
               }
               lcd_gotoxy(12,1); lcd_putsf(txt_OnOff[RELAIS_WPUMPE]);
             }
             break;
  }
  
   if (Taste == OKAY) 
   {  if (tmp_mode < 3) tmp_mode++; else 
      { tmp_mode = 0;
        #ifdef EXTRA_MENU
        MenuCount = 255; 
        #endif    
      }
      MenuPolling = 1;
   }   

   
//  if (ServiceMode == 0) 
//  { 

//  }

}
// ---------------------------------------------------------------------------------------
void MenuHistory(void)
{                          
  static unsigned char index=0, tmp_verb[5], tmp_temp[5];
  unsigned char i; 
  
  if (MenuPolling)                          
  {  
     MenuPolling = 0;
     index = ZeigerRingspeicher-1;          
    
     for (i = 0; i < 5; i++)
     {
       if (index >= ANZ_LOGGING) index = ANZ_LOGGING-1;
       tmp_verb[i] = RingSpeicher_Verbrauch[index];
       tmp_temp[i] = RingSpeicher_TempAussen[index];
       index--;     
     }  
  }
  if (RefreshDisplay)
  {
     lcd_gotoxy(0,0);
     for (i=0;i<5;i++) if(tmp_verb[i]<=100) lcd_int( tmp_verb[i], 3, 0);  else lcd_putsf(" - ");      
     lcd_putchar('%');
     lcd_gotoxy(0,1);                                         
     for (i=0;i<5;i++) if(tmp_temp[i]<=100) lcd_int( tmp_temp[i], -2, 0); else lcd_putsf(" - ");    
     lcd_putchar('\xdf');
  }
  
  if ((Taste == UP) ||  (Taste == DOWN) ||  (Taste == OKAY)) 
   {  
     MenuCount = 255; 
     MenuPolling = 1;   
   }   
}

// ---------------------------------------------------------------------------------------
#ifdef EXTRA_MENU
void MenuAuswahl(void)
{
  static unsigned char index = 0;
  
   if (RefreshDisplay)
   {  
     lcd_gotoxy(0,1); lcd_putsf("                 ");
     lcd_gotoxy(0,0); 
     switch(index) 
     { case 0: 
                lcd_putsf("Status           ");      
              break;
       
       case 1: 
                lcd_putsf("Timer - Setup    ");
              break;
      
       case 2: 
                lcd_putsf("Schalt - Gruppe  ");
              break;
                
       case 3: 
                lcd_putsf("Kennlinien       ");
              break;
      
       case 4: 
//              lcd_putsf("Wasserpumpe      ");
                lcd_putsf("Pilotraumtemp.   ");
              break;
#ifdef NO_RTC     
       case 5: 
                lcd_putsf("Verbrauch \xf5ber   ");
                lcd_gotoxy(0,1); 
                lcd_putsf("5 Tage");
#else
       case 5: 
                lcd_putsf("Datum / Zeit     ");
              break;

       case 6: 
                lcd_putsf("Verbrauch \xf5ber   ");
                lcd_gotoxy(0,1); 
                lcd_putsf("5 Tage");
              break;

       case 7: 
                lcd_putsf("Warmwasser-Temp.");
              break;

       case 8: 
                lcd_putsf("Sensorabgleich  ");
              break;

       case 9: 
                lcd_putsf("Einstellungen   ");
              break;

       case 10: 
                lcd_putsf("Service-Funktion");
#endif

              break;
     }         
   }                                        
  
#ifdef NO_RTC      
  if (Taste == UP) if (index < 5) index++; else index = 0;
  else if (Taste == DOWN) if (index > 0) index--; else index = 5;
#else
  if (Taste == UP) if (index < 10) index++; else index = 0;
  else if (Taste == DOWN) if (index > 0) index--; else index = 10;
#endif
  
  if (Taste == OKAY)
  {
    MenuCount = index;
    MenuPolling = 1;
  } 
  
}
#endif

// ---------------------------------------------------------------------------------------
void GetRegelpunkt(void)
{  
 signed long soll,dK,dT;
 if(IstTempAussen < KennlinieTemperaturOben[BetriebsMode]) 
  {
   SollTempLeit = MaxLeitTemperatur[BetriebsMode];
  }
 else  
 if(IstTempAussen > KennlinieTemperaturUnten[BetriebsMode]) 
  {
   SollTempLeit = MinLeitTemperatur[BetriebsMode];
  }
 else
  {
   dK = MaxLeitTemperatur[BetriebsMode] - MinLeitTemperatur[BetriebsMode];
   dT = KennlinieTemperaturUnten[BetriebsMode] - KennlinieTemperaturOben[BetriebsMode];
   soll = MaxLeitTemperatur[BetriebsMode] - ((IstTempAussen - KennlinieTemperaturOben[BetriebsMode]) * dK) / dT;
   SollTempLeit = soll; 
  } 

 if (IstTempLeit > (WarmWasserTemperatur[BetriebsMode] + HystereseWarmWasser))
  {
   SollTempWWasser = WarmWasserTemperatur[BetriebsMode];
  }
 else
  {
   SollTempWWasser = IstTempLeit - HystereseWarmWasser - 10;
  }
  
  
}
#define AUS 1
#define EIN 0

// ---------------------------------------------------------------------------------------
void SteuerRelais(void)
{
 if (ServiceMode == OFF)
 { 
   if(IstTempLeit > SollTempLeit + HystereseTempLeit) RELAIS_ERZEUGERSPERRE_OELHZ = EIN; //  AUS; // 
   else // aus
   if((IstTempLeit) < SollTempLeit)
   {
//       if(RELAIS_ERZEUGERSPERRE_OELHZ == AUS) CountStartBRE ++;      // ein   
       if(RELAIS_ERZEUGERSPERRE_OELHZ == EIN) CountStartBRE ++;      // ein   
       RELAIS_ERZEUGERSPERRE_OELHZ = AUS; //EIN;      // ein   
   }
 
 
///   if(IstTempLeit > SollTempLeit + HystereseTempLeit) RELAIS_LEIT = AUS; 
///   else // aus
///   if((IstTempLeit < SollTempLeit) && ((IstTempLeit + 20) < IstTempWWasser))                      
///   {
///       if(RELAIS_LEIT == AUS) CountStartBRE ++;      // ein   
///       RELAIS_LEIT = EIN;      // ein   
///   }
///   else
///   {
///       RELAIS_LEIT = AUS;      // ein   
///   }


 

//   if ((IstTempLeit < PumpeEinTemperatur[BetriebsMode]) && 
//      (SollTempLeit < PumpeEinTemperatur[BetriebsMode]))           RELAIS_KPUMPE = AUS; // weniger als xx ° im Leit AUS                                                            
//   else 
//   if((SollTempLeit > PumpeEinTemperatur[BetriebsMode] + 10) && 
//       (IstTempLeit > PumpeEinTemperatur[BetriebsMode]))              RELAIS_KPUMPE = EIN;


/////   if ((TemperaturVal_Sensor01 < PumpeEinTemperatur[BetriebsMode]) || (RELAIS_LEIT == EIN))   RELAIS_KPUMPE = EIN; 
                                                                        // weniger als xx ° im Pilotraum oder Brenner = An ==> AUS                                                            
/////   if((TemperaturVal_Sensor01 > PumpeEinTemperatur[BetriebsMode] + 5) && (RELAIS_LEIT == AUS)) RELAIS_KPUMPE = AUS;


   if (IstTempWWasser > SollTempWWasser + HystereseWarmWasser)  RELAIS_WPUMPE = AUS;
   if (IstTempWWasser < SollTempWWasser)                        RELAIS_WPUMPE = EIN;

    
   if ((IstTempLeit < IstTempKessel_OHz - HystereseWarmWasser) && (IstTempKessel_OHz > 420))  RELAIS_KPUMPE = EIN;
   else     RELAIS_KPUMPE = AUS;

 }
 else
 { 
   if(IstTempLeit > TempLeitMax_TL) 
   { RELAIS_ERZEUGERSPERRE_OELHZ = EIN;  /////AUS;  // aus
     ServiceMode = OFF;
   }
 }
    
//   if ((IstTempWWasser < SollTempWWasser + HystereseWarmWasser) && 
//      ((IstTempWWasser + 20) < IstTempLeit))                        RELAIS_WPUMPE = EIN;
//   else     RELAIS_WPUMPE = AUS;


}

// ---------------------------------------------------------------------------------------
//void SteuerRelaisWW(void)
//{
// if (IstTempWWasser < SollTempWWasser + WarmWasserHysterese)
// {
//   if ((IstTempWWasser + 20) < IstTempLeit) RELAIS_WPUMPE = EIN;
//   else RELAIS_WPUMPE = AUS;
// } 
// else
// {     
//    RELAIS_WPUMPE = AUS;
// }
//}

// ---------------------------------------------------------------------------------------
void Menu(void)
{                   
 // if (Taste == MENU) { MenuCount++; MenuPolling = 1; }
  
  if (Taste == MODE) 
  { 
    if (BetriebsMode < 2) BetriebsMode++; else BetriebsMode = 0; 
    MenuPolling = 1;
    LCDInit();
  }
  
  switch (MenuCount)
  {
    case 0:    MenuIstData();
      break;
  
    case 1:    MenuTimer();
      break;

    case 2:    MenuSchicht();
      break;

    case 3:    MenuTemperatur();
      break;
       
//    case 4:  MenuPumpen(); 
    case 4:    MenuRaumTemperatur();
      break;             
      
#ifdef NO_RTC

    case 5:   MenuHistory();
      break; 
#else

    case 5:   MenuDateTime();
      break; 
                
    case 6:   MenuHistory();
      break; 
                
    case 7:   MenuWarmWasser();
      break; 
                
    case 8:   MenuTempSensorKorr();
      break; 
                
                
    case 9:   MenuMainSettings();
      break; 
                

    case 10:   MenuService();
      break; 
#endif        
    default:
    #ifdef EXTRA_MENU   
       MenuAuswahl();
    #else
      MenuCount = 0;
    #endif     
     break;
  
  }
  
}
// ---------------------------------------------------------------------------------------
void InitEEPROM(void)
{
  unsigned char i;
  
  if (CheckEEpromData != EEPROM_KOMPATIBILITAET)
  {
   lcd_clear();
   lcd_putsf("INIT EEPROM");
   lcd_gotoxy(0,1);
   for (i = 0; i < 64; i++)
   {
    delay_ms(15);
    switch (i % 4)
    { case 0:  Timer[i].Zeit = ZEIT(7,00);  Timer[i].Mode = TAG;      break;
      case 1:  Timer[i].Zeit = ZEIT(8,00);  Timer[i].Mode = ABWESEND; break;
      case 2:  Timer[i].Zeit = ZEIT(17,00); Timer[i].Mode = TAG;      break;
      case 3:  Timer[i].Zeit = ZEIT(22,00); Timer[i].Mode = NACHT;    lcd_putchar('.'); break;
         
    }   
    
    if ((i%32)<4) Timer[i].Zeit |= WERKTAG;
    else
    if ((i%32)<12) Timer[i].Zeit += (((i%32-4)/4)+5) * 1440;
    else     
     { Timer[i].Zeit += (((i%32-12)/4)) * 1440;
       Timer[i].Mode = DEAKTIV;
     }
   }

 
    MaxLeitTemperatur[TAG] = 420;
    MinLeitTemperatur[TAG] = 420;
    KennlinieTemperaturOben[TAG] = -100;
    KennlinieTemperaturUnten[TAG] = 200;
    PumpeEinTemperatur[TAG] = 350;
    WarmWasserTemperatur[TAG] = 500;
    
    MaxLeitTemperatur[NACHT] = 420;
    MinLeitTemperatur[NACHT] = 420;
    KennlinieTemperaturOben[NACHT] = -100;
    KennlinieTemperaturUnten[NACHT] = 200;
    PumpeEinTemperatur[NACHT] = 400;
    WarmWasserTemperatur[NACHT] = 450;
 
    MaxLeitTemperatur[ABWESEND] = 420;
    MinLeitTemperatur[ABWESEND] = 420;
    KennlinieTemperaturOben[ABWESEND] = -100;
    KennlinieTemperaturUnten[ABWESEND] = 200;
    PumpeEinTemperatur[ABWESEND] = 450;          
    WarmWasserTemperatur[ABWESEND] = 400;
    
    SchichtOffset = 0;   
     
   for(i = 0 ; i < ANZ_LOGGING ;i++)
   { RingSpeicher_Verbrauch[i] = 0xFF;   
     RingSpeicher_TempAussen[i] = 0xFF;
     RingSpeicher_CounterStartBRE[i] = 0x00;
   }
    ZeigerRingspeicher = 0;    
    SensorKorrWert_TA = -40;    
    SensorKorrWert_TL = -50;    
    SensorKorrWert_TW = -40;    
    SensorKorrWert_TK = -120;              
    SensorKorrWert_P1 = -40;    
    SensorKorrWert_P2 = -40;    
    SensorKorrWert_P3 = -40;    
    SensorKorrWert_AG = -50;              
    
    
    HystereseTempLeit = 120;
    HystereseWarmWasser = 20;
    TempLeitMax_TL = 800;
                         
    CounterStartBRE_Gesamt = 0;
    eeValYear = 6;
    
    CheckEEpromData = EEPROM_KOMPATIBILITAET;

  }
}

// ---------------------------------------------------------------------------------------
void SpeicherVerbrauch(void)
{

  RingSpeicher_Verbrauch[ZeigerRingspeicher] = ((long) CountHeizzeit * 100) / CountZeit;
  RingSpeicher_TempAussen[ZeigerRingspeicher] = DurchschnittsTemperatur / 1440;
  RingSpeicher_CounterStartBRE[ZeigerRingspeicher] = CountStartBRE;
  CounterStartBRE_Gesamt += CountStartBRE;

  
  DurchschnittsTemperatur = 0;
  CountHeizzeit = 0;
  CountZeit = 0;
  CountStartBRE = 0;
  
  
  
  if (ZeigerRingspeicher++ >= ANZ_LOGGING) ZeigerRingspeicher = 0; 
}

// ---------------------------------------------------------------------------------------
void SendValue2Uart(int value)
{
  char sign;
  
  sign='+';
  if (value<0)
  {
    sign='-';
    value=-value;
  };
  if (value < 100)
  {  
  // send the value via the RS232 serial communication
  //  printf("%c0%i.%u\n",sign,value/10,value%10);
    printf("%c0%i.%u",sign,value/10,value%10);
  }
  else
  {  
  // send the value via the RS232 serial communication
  //  printf("%c%i.%u\n",sign,value/10,value%10);
    printf("%c%i.%u",sign,value/10,value%10);
  };
  if (value > 999)
  {  
  // send the value via the RS232 serial communication
  //  printf("%c%i.%u\n",sign,value/10,value%10);
    printf("%c%i.%u",sign,value/10,value%10);
  };
}
// ---------------------------------------------------------------------------------------




// ---------------------------------------------------------------------------------------
void main(void)
{
// Declare your local variables here
  static unsigned char tmp_mode;      
  unsigned char i, j; 
// Input/Output Ports initialization
// Port A
//delay_ms(250);
//PORTA=0x00;
//DDRA=0x00;

// Port B
PORTB=0x00;
//////////DDRB=0x1f;
DDRB=0xff;

// Port C
//PORTC=0x00;
//DDRC=0x00;

#ifdef ExternTast
  // Port D
  PORTD=0x00;
  DDRD=0x9c;     // Version mit externer Tastatur
#else
  // Port D
  //PORTD=0x00;
  // DDRD=0xff; 
#endif

// Timer/Counter 0 initialization
// Clock source: System Clock
// Clock value: Timer 0 Stopped
// Mode: Output Compare
// OC0 output: Disconnected
TCCR0=0x02;
TCNT0=0x64;

// Timer/Counter 1 initialization
// Clock source: System Clock
// Clock value: Timer 1 Stopped
// Mode: Output Compare
// OC1A output: Discon.
// OC1B output: Discon.
// Noise Canceler: Off
// Input Capture on Falling Edge
/*TCCR1A=0x00;
TCCR1B=0x00;
TCNT1H=0x00;
TCNT1L=0x00;
OCR1AH=0x00;
OCR1AL=0x00;
OCR1BH=0x00;
OCR1BL=0x00;
  */
// Timer/Counter 2 initialization
// Clock source: System Clock
// Clock value: Timer 2 125,000 kHz
// Mode: Output Compare
// OC2 output: Disconnected
// T2 is cleared oncompare  match
TCCR2=0x0C;
//ASSR=0x00;
//TCNT2=0x00;
OCR2=0x7D;         // 125 dez = 1ms         

// UART initialization
// Communication Parameters: 8 Data, 1 Stop, No Parity
// UART Receiver: On
// UART Transmitter: On
// RX-Int: On
// TX-Int: Off
#define xtal 8000000L
#define baud 9600
UCSRB = 0x98;        
UBRRL=xtal/16/baud-1;

// External Interrupt(s) initialization
// INT0: Off
// INT1: Off
GICR=0x00;
MCUCR=0x00;   
#ifdef INCR_GEBER
GIMSK = 0x80;
MCUCR=0x08;    // falling edge Int1
#endif

// Timer(s)/Counter(s) Interrupt(s) initialization
TIMSK=0x80;
                     
// ADC initialization
// ADC Clock frequency: 500,000 kHz
ADCSRA=0x84;

 LCDInit();
#ifdef NO_RTC
#else
/* initialize the I2C bus */
i2c_init();
/* initialize the RTC 0 */
rtc_init(0,0);
#endif
 //    CheckEEpromData = EEPROM_KOMPATIBILITAET;     //just for debug  
 if (TASTE4) CheckEEpromData = 0xFF; // EEPROM neu init, wenn OKAY-Taste gedrückt

         //1234567890123456
 lcd_gotoxy(0,0);
 lcd_putsf(PROGNAME); //////////  lcd_putsf("I. Busker V1.1");
 lcd_gotoxy(0,1);
 lcd_putsf(COPYRIGHT); //////////  lcd_putsf("H. Buss");
 
 delay_ms(1500);
 InitEEPROM();
 
 GetTemperatur(1); 

// Global enable interrupts
#asm("sei")

while (1)
      {    
      
      

  if (Taste == SEN230V)
  {LED_TEST = 1;
     lcd_gotoxy(9,1); lcd_putchar('X'); }
  else
  {LED_TEST = 0;}
//  { lcd_gotoxy(9,1); lcd_putchar(' '); }
     

           delay_ms(40);
           tmp_mode = ErmittleBetriebsMode(AktuellZeit);
           if (tmp_mode != DEAKTIV) BetriebsMode = tmp_mode;
           
           Menu();
           GetTemperatur(0);
           GetRegelpunkt();
           SteuerRelais();
//           SteuerRelaisWW();
           
           Tastatur();              

           if (BetriebsMode == TAG)      LED_TAG = 1;      else LED_TAG = 0;
           if (BetriebsMode == NACHT)    LED_NACHT = 1;    else LED_NACHT = 0;
           if (BetriebsMode == ABWESEND) LED_ABWESEND = 1; else LED_ABWESEND = 0;
           if (ServiceMode == ON)
           { 
             if (LED_SERVICE == 0) LED_SERVICE = 1; else LED_SERVICE = 0;
           }
           else LED_SERVICE = 0;
           


           if(PollingTag) 
            { SpeicherVerbrauch();
              PollingTag = 0;
            }
            
           if (SendeSysID)
            {
              SendeSysID = 0;
              Output = SERIELL;
              putsf(SYS_ID);
              putchar('\r');
            }                          
//           if (SendeVersHW)
//            {
//              SendeVersHW = 0;
//              Output = SERIELL;
//              putsf(VERS_HW);
//              putchar('\r');
//            }                          
           if (SendeVersSW)
            {
              SendeVersSW = 0;
              Output = SERIELL;
              putsf(VERS_SW);
              putchar('\r');
            }                          
           if (SendeVersPF)
            {
              SendeVersPF = 0;
              Output = SERIELL;
              putsf(VERS_PF);
              putchar('\r');
            }                          

           if (SendeDisplay)
            {                          
////              unsigned char i;
              
              Menu();
              SendeDisplay = 0;
              Output = SERIELL;
#ifndef NO_LCD
              lcd_write_byte(0xC0+16,(PINB & 0x1F) + 0x30);
              for (i = 0x80; i < 0x80+16; i++)  putchar(lcd_read_byte(i)); 
              putchar('\r');
              for (i = 0xC0; i < 0xC0+17; i++)  putchar(lcd_read_byte(i)); 
              putchar('\r');
#else
              for (i = 0;  i < 16; i++)  putchar(LCD_Puffer[i]); 
              LCD_Puffer[32] = (PINB & 0x1F) + 0x30;
              putchar('\r');
              for (i = 16; i < 33; i++)  putchar(LCD_Puffer[i]); 
              putchar('\r');
#endif
            }
            
           if (SendeDaten)
            {                          
              unsigned char tmp_speicher;
              
              tmp_speicher = ZeigerRingspeicher-1;
              
              SendeDaten = 0;
              
              Output = SERIELL;
              putsf("[%],[°C],[n]");
              while(tmp_speicher != ZeigerRingspeicher)
              {
                if (tmp_speicher >= ANZ_LOGGING) tmp_speicher = ANZ_LOGGING-1;
                if (tmp_speicher == ZeigerRingspeicher) break;
             
                if (RingSpeicher_Verbrauch[tmp_speicher]<=100) 
                {
                  Seriell_int(RingSpeicher_Verbrauch[tmp_speicher], 3, 0); putchar(',');
                  Seriell_int(RingSpeicher_TempAussen[tmp_speicher], -2 , 0); putchar(',');
                  Seriell_int(RingSpeicher_CounterStartBRE[tmp_speicher], 3 , 0);  putchar('\r');
                // Seriell_int(tmp_speicher % 100, 3, 0); putchar(',');
                 // Seriell_int((char) (100- tmp_speicher), -3 , 0); putchar('\r');
                }
                tmp_speicher--; 
              } 
              
            }
            
           if (SendeLogDat)
            {                          
              rtc_get_date(0,&date,&month,&year); 
              rtc_get_time(0,&hour,&minute,&second,&hundredsecond);
              weekday = rtc_get_weekday(0);
           
              SendeLogDat = 0;
              Output = SERIELL;
              putchar('D');
              putchar('L');
              putchar('*');
              
#ifdef LOG_EXTENDED
              switch (weekday)
              {
                case 0: 
                       putchar('M');
                       putchar('O');
                       break;
                case 1: 
                       putchar('D');
                       putchar('I');
                       break;
                case 2: 
                       putchar('M');
                       putchar('I');
                       break;
                case 3: 
                       putchar('D');
                       putchar('O');
                       break;
                case 4: 
                       putchar('F');
                       putchar('R');
                       break;
                case 5: 
                       putchar('S');
                       putchar('A');
                       break;
                case 6: 
                       putchar('S');
                       putchar('O');
                       break;
              } 
#else
              Seriell_int(weekday, 1, 0);
#endif
              putchar('*');   

              Seriell_int(date, 2, LEADING_ZERO); putchar('.');
              Seriell_int(month, 2, LEADING_ZERO); putchar('.');
              //Seriell_int(year, 2, LEADING_ZERO); putchar(' ');
              Seriell_int(eeValYear, 2, LEADING_ZERO); putchar(' ');
              Seriell_int(hour, 2, LEADING_ZERO); putchar(':');
              Seriell_int(minute, 2, LEADING_ZERO); putchar(':');
              Seriell_int(second, 2, LEADING_ZERO); putchar('*');
#ifdef LOG_EXTENDED
              switch (BetriebsMode)
              {
                case 0: 
                       putchar('N');
                       break;
                case 1: 
                       putchar('T');
                       break;
                case 2: 
                       putchar('A');
                       break;
              } 
#else
              Seriell_int(BetriebsMode, 1, 0);
#endif
              putchar('*');   
              

              SendValue2Uart(IstTempAussen); putchar('*');
              SendValue2Uart(IstTempLeit); putchar('*');
              SendValue2Uart(IstTempWWasser); putchar('*');
              SendValue2Uart(SollTempLeit); putchar('*');
              SendValue2Uart(SollTempWWasser); putchar('*');
/*              
              Seriell_int(IstTempAussen/10, -2, 0); putchar('*');
              Seriell_int(IstTempLeit/10, -2, 0); putchar('*');
              Seriell_int(IstTempWWasser/10, -2, 0); putchar('*');
              Seriell_int(SollTempLeit/10, -2, 0); putchar('*');
              Seriell_int(SollTempWWasser/10, -2, 0); putchar('*');
*/
              Seriell_int(RELAIS_ERZEUGERSPERRE_OELHZ, 1, 0); putchar('*');
              Seriell_int(RELAIS_KPUMPE, 1, 0); putchar('*');
              Seriell_int(RELAIS_WPUMPE, 1, 0); putchar('*');
              
              if (ServiceMode == OFF) putchar('B'); else putchar('S');
              putchar('*');  
              printf("%u",CountStartBRE); putchar('*');  
              SendValue2Uart(IstTempKessel_OHz); putchar('*');

              putchar(13);


            }

 
            
           if (SendeLogDatMOD)
            {                          
              rtc_get_date(0,&date,&month,&year); 
              rtc_get_time(0,&hour,&minute,&second,&hundredsecond);
              weekday = rtc_get_weekday(0);
           
              SendeLogDatMOD = 0;
              Output = SERIELL;
              putchar('D');
              putchar('M');
              putchar('*');
              
#ifdef LOG_EXTENDED
              switch (weekday)
              {
                case 0: 
                       putchar('M');
                       putchar('O');
                       break;
                case 1: 
                       putchar('D');
                       putchar('I');
                       break;
                case 2: 
                       putchar('M');
                       putchar('I');
                       break;
                case 3: 
                       putchar('D');
                       putchar('O');
                       break;
                case 4: 
                       putchar('F');
                       putchar('R');
                       break;
                case 5: 
                       putchar('S');
                       putchar('A');
                       break;
                case 6: 
                       putchar('S');
                       putchar('O');
                       break;
              } 
#else
              Seriell_int(weekday, 1, 0);
#endif
              putchar('*');   

              Seriell_int(date, 2, LEADING_ZERO); putchar('.');
              Seriell_int(month, 2, LEADING_ZERO); putchar('.');
              //Seriell_int(year, 2, LEADING_ZERO); putchar(' ');
              Seriell_int(eeValYear, 2, LEADING_ZERO); putchar(' ');
              Seriell_int(hour, 2, LEADING_ZERO); putchar(':');
              Seriell_int(minute, 2, LEADING_ZERO); putchar(':');
              Seriell_int(second, 2, LEADING_ZERO); putchar('*');
#ifdef LOG_EXTENDED
              switch (BetriebsMode)
              {
                case 0: 
                       putchar('N');
                       break;
                case 1: 
                       putchar('T');
                       break;
                case 2: 
                       putchar('A');
                       break;
              } 
#else
              Seriell_int(BetriebsMode, 1, 0);
#endif
              putchar('*');   

              SendValue2Uart(IstTempLeit); putchar('*');       // Sensor TempLeit                Pos:  Tauchhuelse Pufferspeicher Oben
              SendValue2Uart(IstTempAussen); putchar('*');     // Sensor Temp-Aussen             Pos:  Aussen am Kellerniedergang
              SendValue2Uart(IstTempWWasser); putchar('*');    // Sensor Temp-Warmwasser         Pos:  Tauchhuelse Warmwasserspeicher
              SendValue2Uart(IstTempKessel_OHz); putchar('*'); // Sensor Temp-Kessel OeHz        Pos:  Tauchhuelse OeHz-Kessel
              SendValue2Uart(TempValSen_P1); putchar('*');     // Sensor Temp-Ruecklauf02 OeHz   Pos:  Austritt OeHz Waermetauscher
              SendValue2Uart(TempValSen_P2); putchar('*');     // Sensor Temp-Ruecklauf01 OeHz   Pos:  Eintritt OeHz Waermetauscher
              SendValue2Uart(TempValSen_P3); putchar('*');     // Sensor Temp-Vorlauf OeHz       Pos:  Austritt OeHz-Kessel
              SendValue2Uart(TempValSen_AG); putchar('*');     // Sensor Temp-Abgas OeHz         Pos:  Abgasrohr Austritt Waermetauscher

              Seriell_int(RELAIS_ERZEUGERSPERRE_OELHZ, 1, 0); putchar('*');
              Seriell_int(RELAIS_KPUMPE, 1, 0); putchar('*');
              Seriell_int(RELAIS_WPUMPE, 1, 0); putchar('*');

              if (ServiceMode == OFF) putchar('B'); else putchar('S');
              putchar('*');  
              SendValue2Uart(SollTempLeit); putchar('*');
              SendValue2Uart(SollTempWWasser); putchar('*');

              putchar(13);

            }
 
 
           if (SendeDataSet)
            {                          
              unsigned int BufInt01, BufInt02;
              SendeDataSet = 0;
              Output = SERIELL;

              putchar('D'); putchar('S'); putchar('S');  putchar('\r');
              

              putchar('D'); putchar('T'); putchar(' ');
              Seriell_int(date, 2, LEADING_ZERO); putchar('.');
              Seriell_int(month, 2, LEADING_ZERO); putchar('.');
              //Seriell_int(year, 2, LEADING_ZERO); putchar(' ');
              Seriell_int(eeValYear, 2, LEADING_ZERO); putchar(' ');
              Seriell_int(hour, 2, LEADING_ZERO); putchar(':');
              Seriell_int(minute, 2, LEADING_ZERO); putchar(':');
              Seriell_int(second, 2, LEADING_ZERO);
              putchar('\r');
              
              BufInt01 = NACHT;
              while(BufInt01 != ABWESEND+1)
              {
                putchar('K'); putchar('L'); putchar(' '); // putchar('\r');
                switch (BufInt01)
                {
                  case TAG: 
                         putchar('T');
                         break;
                  case NACHT: 
                         putchar('N');
                         break;
                  case ABWESEND: 
                         putchar('A');
                         break;
                } 
//                Seriell_int(BufInt01, 1, 0); 
                putchar(' ');
                Seriell_int(MaxLeitTemperatur[BufInt01]/10, 2, 0); putchar(' ');
                Seriell_int(KennlinieTemperaturOben[BufInt01]/10, -2, 0); putchar(' ');
                Seriell_int(MinLeitTemperatur[BufInt01]/10, 2, 0); putchar(' ');
                Seriell_int(KennlinieTemperaturUnten[BufInt01]/10, -2, 0); putchar(' ');
//                Seriell_int(WarmWasserTemperatur[BufInt01]/10, 2, 0); putchar(' ');
                putchar('\r');
                BufInt01++;
              }
              putchar('S'); putchar('G'); putchar(' '); Seriell_int(SchichtOffset / 32 +1, 1, 0);  putchar('\r');

              for (i = 0; i < 64; i+=4) 
              {  
                putchar('T'); putchar('S'); putchar(' ');
                Seriell_int(i / 32 +1, 1, 0); putchar(' ');
                if (Timer[i].Zeit & WERKTAG)                         // Sonderfall Werktags
                {
                  putchar('W'); putchar('T'); putchar(' ');
                }
                else
                { 
                  switch (Timer[i].Zeit/1440)
                  {
                    case 0: 
                           putchar('M');
                           putchar('O');
                           break;
                    case 1: 
                           putchar('D');
                           putchar('I');
                           break;
                    case 2: 
                           putchar('M');
                           putchar('I');
                           break;
                    case 3: 
                           putchar('D');
                           putchar('O');
                           break;
                    case 4: 
                           putchar('F');
                           putchar('R');
                           break;
                    case 5: 
                           putchar('S');
                           putchar('A');
                           break;
                    case 6: 
                           putchar('S');
                           putchar('O');
                           break; 
                  
                  }     
//                    putsf(Tag[Timer[i].Zeit/1440]);
//       putchar('D'); Seriell_int(Timer[i].Zeit/1440, 1, 0); 
                  putchar(' ');
                }
              for (j = 0; j < 4; j++) 
              {  

                switch (Timer[i+j].Mode)
                {
                  case DEAKTIV: 
                         putchar('D');
                         putchar('-');
                         putchar('-');
                         putchar(':');
                         putchar('-');
                         putchar('-');
                         //putsf("D--:--"); 
                         break;
                  case TAG: 
                         putchar('T');
                         break;
                  case NACHT: 
                         putchar('N');
                         break;
                  case ABWESEND: 
                         putchar('A');
                         break;
                } 
                if (Timer[i+j].Mode != DEAKTIV) 
                {
                  if (Timer[i+j].Zeit & WERKTAG)                         // Sonderfall Werktags
                  {
		  Seriell_int(((Timer[i+j].Zeit - WERKTAG) % 1440) / 60, 2 , LEADING_ZERO);
                  putchar(':');
		  Seriell_int(((Timer[i+j].Zeit - WERKTAG) % 1440)  % 60, 2 , LEADING_ZERO);
                  }
                  else
                  {
		  Seriell_int((Timer[i+j].Zeit % 1440) / 60, 2 , LEADING_ZERO);
                  putchar(':');
		  Seriell_int((Timer[i+j].Zeit % 1440) % 60, 2 , LEADING_ZERO);
                  }
                } 
                putchar(' ');
               } 
                
                
                putchar('\r');

 /*     {  if (zeit  < (6*1440))                                          // akt. Tag ist Mo - Fr
         if (((Timer[i+ SchichtOffset].Zeit & ~WERKTAG) % 1440)== (zeit % 1440)) { rueck = Timer[i+ SchichtOffset].Mode; break;} // testen  
      }
      else 
      if (Timer[i+ SchichtOffset].Zeit == zeit) { rueck = Timer[i+ SchichtOffset].Mode; break; } 

*/
   }

 
 /*             BufInt01 = 0;
              while(BufInt01 != 1+1)
              {
              putchar('T'); putchar('S'); putchar(' ');
                Seriell_int(BufInt01, 1, 0); putchar(' ');
                BufInt02 = 0;
                while(BufInt02 != 1+1)
                {
                  Seriell_int(BufInt01, 1, 0); putchar(' ');
                  Seriell_int(Timer[BufInt01].Mode, 1, 0); putchar(' ');
 
 
                  BufInt02++;
                }
                putchar('\r');
                BufInt01++;
              }

*/
              BufInt01 = NACHT;
              while(BufInt01 != ABWESEND+1)
              {
                putchar('W'); putchar('P'); putchar(' ');
                switch (BufInt01)
                {
                  case TAG: 
                         putchar('T');
                         break;
                  case NACHT: 
                         putchar('N');
                         break;
                  case ABWESEND: 
                         putchar('A');
                         break;
                } 
//                Seriell_int(BufInt01, 1, 0); 
                putchar(' ');
                Seriell_int(PumpeEinTemperatur[BufInt01]/10, -2, 0); putchar(' ');
                putchar('\r');
                BufInt01++;
              }

              BufInt01 = NACHT;
              while(BufInt01 != ABWESEND+1)
              {
                putchar('W'); putchar('T'); putchar(' ');
                switch (BufInt01)
                {
                  case TAG: 
                         putchar('T');
                         break;
                  case NACHT: 
                         putchar('N');
                         break;
                  case ABWESEND: 
                         putchar('A');
                         break;
                } 
//                Seriell_int(BufInt01, 1, 0); 
                putchar(' ');
                Seriell_int(WarmWasserTemperatur[BufInt01]/10, -2, 0); putchar(' ');
                putchar('\r');
                BufInt01++;
              }
               putchar('S'); putchar('K'); putchar(' '); putchar('A'); putchar(' '); Seriell_int(SensorKorrWert_TA/10, -2, 0); 
               putchar('\r');
               putchar('S'); putchar('K'); putchar(' '); putchar('K'); putchar(' '); Seriell_int(SensorKorrWert_TL/10, -2, 0); 
               putchar('\r');
               putchar('S'); putchar('K'); putchar(' '); putchar('W'); putchar(' '); Seriell_int(SensorKorrWert_TW/10, -2, 0); 
               putchar('\r');
               putchar('S'); putchar('K'); putchar(' '); putchar('1'); putchar(' '); Seriell_int(SensorKorrWert_TK/10, -2, 0); 
               putchar('\r');

//               putsf("SK A "); Seriell_int(SensorKorrWert_TA/10, -2, 0); 
//               putchar('\r');
//               putsf("SK K "); Seriell_int(SensorKorrWert_TL/10, -2, 0); 
//               putchar('\r');
//               putsf("SK W "); Seriell_int(SensorKorrWert_TW/10, -2, 0); 
//               putchar('\r');

               putchar('H'); putchar('T'); putchar(' '); putchar('K'); putchar(' '); Seriell_int(HystereseTempLeit/10, -2, 0); 
//               putsf("HT K "); Seriell_int(HystereseTempLeit/10, -2, 0); 
               putchar('\r');
               putchar('H'); putchar('T'); putchar(' '); putchar('W'); putchar(' '); Seriell_int(HystereseWarmWasser/10, -2, 0); 
//               putsf("HT W "); Seriell_int(HystereseWarmWasser/10, -2, 0); 
               putchar('\r');

               putchar('B'); putchar('S'); putchar(' '); putchar('T'); putchar(' '); printf("%u\r",CountStartBRE); 
               putchar('B'); putchar('S'); putchar(' '); putchar('G'); putchar(' '); printf("%u\r",CounterStartBRE_Gesamt); 
//               putsf("BS T "); printf("%u\r",CountStartBRE); 
//               putsf("BS G "); printf("%u\r",CounterStartBRE_Gesamt); 
//               puts("TEST ");               putsf("TEST\r");


//               putchar('D'); putchar('S'); putchar('E');  putchar('\r');
//               putchar('\r');
               putsf("DSE\r");
             
            }



            
      };
}