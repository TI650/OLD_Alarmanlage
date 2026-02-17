









#define CmdBufferTxMax   16
#define CmdBufferRxMax   16

//## SYSTEMBEFEHLE ##//
#define CMD_AC_ResetSystem                 1
#define CMD_AC_ReloadEepromData           10
#define CMD_AC_StartIntBuzzer            201
#define CMD_AC_NOP                       254






#define CMD_AC_CountdownTime             101

//*** CMD_AC_Buzz1_Cnt_Time
//* Signaldauer fuer den internen Buzzer beim Oeffnen
//* und Schliessen der Fenster und Türen.
//* Angabe in 1/10 sec.
//*   0 = Aus =   0 Sec (Min-Wert)
//*  10 = Ein =   1 Sec  
//* 250 = Ein =  25 Sec (Max-Wert)
#define CMD_AC_Buzz1_Cnt_Time            102
  #define CMD_AC_Buzz1_Cnt_Time_ValStd            5
  #define CMD_AC_Buzz1_Cnt_Time_ValMin            0
  #define CMD_AC_Buzz1_Cnt_Time_ValMax            250


#define CMD_AC_Int_Siren_Cnt_Time        103

#define CMD_AC_InPortAdr_Min             200
#define CMD_AC_InPortAdr_Max             207
#define CMD_AC_InPortCnt                 210
#define CMD_AC_SlaveAddress              220

#define CMD_AC_InPortRootingTable_Min    300
#define CMD_AC_InPortRootingTable_Max    363


#define CMD_RTC_SetWeekDay               501
#define CMD_RTC_SetDay                   502
#define CMD_RTC_SetMonth                 503
#define CMD_RTC_SetYear                  504
#define CMD_RTC_SetHour                  505
#define CMD_RTC_SetMinute                506
#define CMD_RTC_SetSecond                507

#define CMD_PreAlarm_BuzzerLevelMin      611
#define CMD_PreAlarm_BuzzerLevelMax      612
#define CMD_PreAlarm_BuzzerIntervall     613

#define CMD_FullAlarm_BuzzerLevelMin     621
#define CMD_FullAlarm_BuzzerLevelMax     622
#define CMD_FullAlarm_BuzzerIntervall    623


//#define CMD_SensorKorrWert_TA           1121
//#define CMD_SensorKorrWert_TI           1122

#define CMD_SensorKorrWert_Min          1110
#define CMD_SensorKorrWert_Max          1113



/*
 *  module global variables
 */
static volatile unsigned char CMD_TxBuf[CmdBufferTxMax];
static volatile unsigned char CMD_RxBuf[CmdBufferRxMax];
static volatile unsigned char CMD_TxPtr = 0;
static volatile unsigned char CMD_RxPtr = 0;
static volatile unsigned char CMD_Buffer[CmdBufferRxMax];
static volatile unsigned char CMD_Pointer = 0;

unsigned char CMD_Memory = 0;

//
// --------------------------------------------------------------------------
//
void s_Cmd_Parse(void){
  unsigned char i;
  char bufferStr[5];
  unsigned int bufferIntCmd;
  unsigned int bufferIntVal;
	
	
	
//	for(i=0; i<CMD_Pointer; i++)
//	{
//		uart_putc(CMD_Buffer[i]);
//	}

	if (CMD_Buffer[1] == '#')
	{
	
//		uart_puts("CMD ACK\n\r");
		for(i=0; i<5; i++)
		{
			bufferStr[i] = CMD_Buffer[i+2];
		}
		
//		for(i=0; i<5; i++)
//		{
//			uart_putc(bufferStr[i]);
//		}
		
		bufferIntCmd = atoi(bufferStr);   // convert string into interger (decimal format)         
	
		
		for(i=0; i<5; i++)
		{
			bufferStr[i] = CMD_Buffer[i+7];
		}
		
		bufferIntVal = atoi(bufferStr);   // convert string into interger (decimal format)         

//		uart_puts("Value: ");   // convert interger into string (decimal format)         
//		for(i=0; i<5; i++)
//		{
//			uart_putc(bufferStr[i]);
//		}
//		uart_putc(' ');
//		uart_puts(itoa( bufferIntVal, bufferStr, 10));   // convert interger into string (decimal format)         
//		uart_puts("\r\n");   // convert interger into string (decimal format)         

		switch (bufferIntCmd)
		{
			//## SYSTEMBEFEHLE ##//
			case	CMD_AC_ResetSystem:
						CMD_Memory = CMD_AC_ResetSystem;
						break;

			case	CMD_AC_ReloadEepromData:
						CMD_Memory = CMD_AC_ReloadEepromData;
						break;

			case	CMD_AC_NOP:
						CMD_Memory = CMD_AC_NOP;
						break;

			case	CMD_AC_StartIntBuzzer:
						CMD_Memory = CMD_AC_StartIntBuzzer;
						break;

#ifndef NO_RTC
			case	CMD_RTC_SetWeekDay:
						pcf8583_set_weekday(RTC_Adress, bufferIntVal);
						break;
		
			case	CMD_RTC_SetDay:
						pcf8583_set_day(RTC_Adress, bufferIntVal);
						break;
		
			case	CMD_RTC_SetMonth:
						pcf8583_set_month(RTC_Adress, bufferIntVal);
						break;
		
			case	CMD_RTC_SetYear:
						pcf8583_set_year(RTC_Adress, bufferIntVal - 2000);
						break;
		
			case	CMD_RTC_SetHour:
						pcf8583_set_hour(RTC_Adress, bufferIntVal);
						break;
		
			case	CMD_RTC_SetMinute:
						pcf8583_set_min(RTC_Adress, bufferIntVal);
						break;
		
			case	CMD_RTC_SetSecond:
						pcf8583_set_sec(RTC_Adress, bufferIntVal);
						break;
#endif
		
			case	CMD_AC_CountdownTime:
						eeprom_write_byte(&eeCountdown, bufferIntVal);
						break;
		
			case	CMD_AC_InPortCnt:
						eeprom_write_byte(&eeInPortCnt, bufferIntVal);
						break;
		
			case	CMD_PreAlarm_BuzzerLevelMin:
						eeprom_write_byte(&eePreAlarm_BuzzerLevelMin, bufferIntVal);
						break;
		
			case	CMD_PreAlarm_BuzzerLevelMax:
						eeprom_write_byte(&eePreAlarm_BuzzerLevelMax, bufferIntVal);
						break;
		
			case	CMD_PreAlarm_BuzzerIntervall:
						eeprom_write_byte(&eePreAlarm_BuzzerIntervall, bufferIntVal);
						break;
		
			case	CMD_FullAlarm_BuzzerLevelMin:
						eeprom_write_byte(&eeFullAlarm_BuzzerLevelMin, bufferIntVal);
						break;
		
			case	CMD_FullAlarm_BuzzerLevelMax:
						eeprom_write_byte(&eeFullAlarm_BuzzerLevelMax, bufferIntVal);
						break;
		
			case	CMD_FullAlarm_BuzzerIntervall:
						eeprom_write_byte(&eeFullAlarm_BuzzerIntervall, bufferIntVal);
						break;

			case	CMD_AC_Buzz1_Cnt_Time:
						if (bufferIntVal > CMD_AC_Buzz1_Cnt_Time_ValMax)
						{
							bufferIntVal = CMD_AC_Buzz1_Cnt_Time_ValMax;
						}
						if (bufferIntVal < CMD_AC_Buzz1_Cnt_Time_ValMin)
						{
							bufferIntVal = CMD_AC_Buzz1_Cnt_Time_ValMin;
						}
						eeprom_write_byte(&eeBuzz1_Cnt_Time, bufferIntVal);
						break;

			case	CMD_AC_Int_Siren_Cnt_Time:
						eeprom_write_byte(&eeInt_Siren_Cnt_Time, bufferIntVal);
						break;

			case	CMD_AC_SlaveAddress:
						eeprom_write_byte(&eeSlaveAddress, bufferIntVal);
						break;


//			case	CMD_SensorKorrWert_TA:
//						eeprom_write_word(&eeSensorKorrWert_TA, bufferIntVal);
//						break;
		
//			case	CMD_SensorKorrWert_TI:
//						eeprom_write_word(&eeSensorKorrWert_TI, bufferIntVal);
//						break;



	    default :

						if ((bufferIntCmd >= CMD_AC_InPortAdr_Min) && (bufferIntCmd <= CMD_AC_InPortAdr_Max))
						{
							eeprom_write_byte(&eeInPortAdr[bufferIntCmd - CMD_AC_InPortAdr_Min], bufferIntVal);
							break;
						}

						if ((bufferIntCmd >= CMD_AC_InPortRootingTable_Min) && (bufferIntCmd <= CMD_AC_InPortRootingTable_Max))
						{
							eeprom_write_byte(&eeInPortRootingTable[bufferIntCmd - CMD_AC_InPortRootingTable_Min], bufferIntVal);
              InPortRootingTable[bufferIntCmd - CMD_AC_InPortRootingTable_Min] = bufferIntVal;
							break;
						}

						if ((bufferIntCmd >= CMD_SensorKorrWert_Min) && (bufferIntCmd <= CMD_SensorKorrWert_Max))
						{
							eeprom_write_word(&eeSensorKorrWert[bufferIntCmd - CMD_SensorKorrWert_Min], bufferIntVal);
              SensorKorrWert[bufferIntCmd - CMD_SensorKorrWert_Min] = bufferIntVal;
							break;
						}




						uart_puts("CMD ERROR\r\n");
						break;

		}





	}
	else
	{
	
		uart_puts("NO CMD\r\n");
	
	}


}
//
// --------------------------------------------------------------------------
//
void s_Cmd_PutChar2Buffer(char data){
  unsigned char i;
	
	if (data == '&'){ CMD_RxPtr = 0;	}

  CMD_RxBuf[CMD_RxPtr] = data;

	CMD_RxPtr++ ;

	if (CMD_RxPtr == CmdBufferRxMax){ CMD_RxPtr = 0;	}
	
	if (data == '\n')
	{
		for(i=0; i<CMD_RxPtr; i++)
		{
			CMD_Buffer[i] = CMD_RxBuf[i];
		}
		CMD_Pointer = CMD_RxPtr;
		CMD_RxPtr = 0;
		s_Cmd_Parse();
	}

}
//
// --------------------------------------------------------------------------
//

//
// --------------------------------------------------------------------------
//

//&#0050100005    --> Weekday = Sa
//&#0050300005    --> Month   = 05
//&#0050500021    --> Hour    = 21
//&#0050600003    --> Minute  = 03


// Script-Line-Zuordnung


//&#0030000003


//&#0030800002

//&#0031600001
//&#0031700000

//&#0032400002
//&#0032500002
// 2^16 =65535
//&#0111065400
//&#0111000000
//&#0111100050
//&#0111200050
//&#0111300050



//  ---------------------------------------------------------------------------
// | Wert  | User |         Funktion         |     Wertebereich      | Einheit |
// |  ID   | Zone |                          |  Std  |  Min  |  Max  |         | 
// |-------+------+--------------------------+-------+-------+-------+---------| 
// |       |      |                          |       |       |       |         |
// |       |      |                          |       |       |       |         |
// |       |      |                          |       |       |       |         |
// |       |      |                          |       |       |       |         |
// |       |      |                          |       |       |       |         |
// |-------+------+--------------------------+-------+-------+-------+---------| 
// | SYSTEMBEFEHLE                                                             |
// |-------+------+--------------------------+-------+-------+-------+---------| 
// | 00001 |      | SYSTEM-RESET             |   -   |   -   |   -   |    -    | 
// |       |      |                          |       |       |       |         |
// |       |      |                          |       |       |       |         |
// |       |      |                          |       |       |       |         |
// |       |      |                          |       |       |       |         |
// | 00010 |      | Reload EEPROM-Daten      |   -   |   -   |   -   |    -    |
// |       |      |                          |       |       |       |         |
// |       |      |                          |       |       |       |         |
// |       |      |                          |       |       |       |         |
// |       |      |                          |       |       |       |         |
// | 00201 |      | CMD_AC_StartIntBuzzer    |   -   |   -   |   -   |    -    |
// |       |      |                          |       |       |       |         |
// |       |      |                          |       |       |       |         |
// | 00254 |      | NOP                      |   -   |   -   |   -   |    -    |
// |       |      |                          |       |       |       |         |
// |-------+------+--------------------------+-------+-------+-------+---------| 
// | PARAMETER                                                                 |
// |-------+------+--------------------------+-------+-------+-------+---------| 
// | 00101 |      | Signaldauer des internen |     5 |     0 |   250 |  1/10 s | 
// |       |      | Buzzers beim Oeffnen und |       |       |       |         | 
// |       |      | Schliessen der Türen und |       |       |       |         |
// |       |      | Fenster.                 |       |       |       |         | 
// |       |      |                          |       |       |       |         |
// |       |      |                          |       |       |       |         |
// |       |      |                          |       |       |       |         |
// |       |      |                          |       |       |       |         |
// |       |      |                          |       |       |       |         |
// |       |      |                          |       |       |       |         |
// |       |      |                          |       |       |       |         |
// |       |      |                          |       |       |       |         |
// |       |      |                          |       |       |       |         |
// |       |      |                          |       |       |       |         |
// |       |      |                          |       |       |       |         |
// |       |      |                          |       |       |       |         |
// |       |      |                          |       |       |       |         |
// |       |      |                          |       |       |       |         |
// |-------+------+--------------------------+-------+-------+-------+---------| 
// |-------+------+--------------------------+-------+-------+-------+---------| 
// |-------+------+--------------------------+-------+-------+-------+---------| 
// |       |      |                          |       |       |       |         |
// |-------+------+--------------------------+-------+-------+-------+---------| 
// |-------+------+--------------------------+-------+-------+-------+---------| 
// |-------+------+--------------------------+-------+-------+-------+---------| 
// |-------+------+--------------------------+-------+-------+-------+---------| 
// |-------+------+--------------------------+-------+-------+-------+---------| 




//*** CMD_AC_Buzz1_Cnt_Time
//* Signaldauer fuer den internen Buzzer beim Oeffnen
//* und Schliessen der Fenster und Türen.
//* Angabe in 1/10 sec.
//*   0 = Aus =   0 Sec (Min-Wert)
//*  10 = Ein =   1 Sec  
//* 250 = Ein =  25 Sec (Max-Wert)
#define CMD_AC_Buzz1_Cnt_Time            102
  #define CMD_AC_Buzz1_Cnt_Time_ValMin            0
  #define CMD_AC_Buzz1_Cnt_Time_ValMax            250


//
// --------------------------------------------------------------------------
//

