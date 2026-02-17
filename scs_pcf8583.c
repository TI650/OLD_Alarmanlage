//***************************************************************************
//* 
//* Projekt Name		: SCS_PCF8583 
//* Projekt Type		: Library 
//* Main Titel	    :    
//* Sub Titel	      : 
//* File Name	  	  : scs_pcf8583.h
//* Date            : 09.04.2007
//* Version         : 0.0.1000
//* Support Tel 	  : +49 43 07 82 46 20
//* Support Fax		  : +49 43 07 82 46 21
//* Support Mobil	  : +49 17 34 26 69 77
//* Support eMail	  : m.kraus@suark.com
//* 
//* -------------------------------------------------------------------------
//* 
//* IDE             : Programmers Notepad 2.0.6.1
//* Compiler        : AVR-GCC 3.4.6
//* Target MCU		  :  
//* 
//* -------------------------------------------------------------------------
//*
//* NOTES:
//*    
//*
//* -------------------------------------------------------------------------
//*
//* Decription
//* -----------
//*
//*
//* 
//* Usage
//* ------
//* 
//* 
//*
//***************************************************************************
//
//

#include "scs_pcf8583.h"
#include "i2cmaster.h"

//
//  constants and macros
//

#define RTC_REG_CTL      0x00             // RTC-PCF8583 address of , Control/Status-Reg
                                          //  Bit 0   =   Timer-Flag
                                          //  Bit 1   =   Alarm-Flag
                                          //  Bit 2   =   Alarm-Enable-Bit
                                          //  Bit 3   =   Mask-Flag
                                          //  Bit 4-5 =   Function-Mode
                                          //  Bit 6   =   Hold-Last-Count-Flag
                                          //  Bit 7   =   Stop-Counting-Flag
#define RTC_REG_HSEC     0x01             // RTC-PCF8583 address of , Hundreds of Seconds (BCD-Code)
                                          //  Bit 0-3 =   1/100 s
                                          //  Bit 4-7 =   1/10  s
#define RTC_REG_SEC      0x02             // RTC-PCF8583 address of , Seconds (BCD-Code)
                                          //  Bit 0-3 =    1 s
                                          //  Bit 4-7 =   10 s
#define RTC_REG_MIN      0x03             // RTC-PCF8583 address of , Minutes (BCD-Code)
                                          //  Bit 0-3 =    1 min
                                          //  Bit 4-7 =   10 min
#define RTC_REG_HOUR     0x04             // RTC-PCF8583 address of , Hours (BCD-Code)
                                          //  Bit 0-3 =    1 hour
                                          //  Bit 4-5 =   10 hour
                                          //  Bit 6   =   AM/PM-Flag
                                          //  Bit 7   =   Format-Flag
#define RTC_REG_YEARDAY  0x05             // RTC-PCF8583 address of , Year/Day (BCD-Code)
                                          //  Bit 0-3 =    1 day
                                          //  Bit 4-5 =   10 day
                                          //  Bit 6-7 =   Year (binary 0 to 3)
#define RTC_REG_WDMONTH  0x06             // RTC-PCF8583 address of , Weekday/Month (BCD-Code)
                                          //  Bit 0-3 =    1 month
                                          //  Bit 4   =   10 month
                                          //  Bit 5-7 =   Weekday (binary 0 to 6)
#define RTC_REG_TIM      0x07             // RTC-PCF8583 address of , Timer (BCD-Code)
                                          //  Bit 0-3 =    1 day
                                          //  Bit 4-7 =   10 day
#define RTC_REG_ALMCTL   0x08             // RTC-PCF8583 address of , Alarm/Control-Reg
                                          //  Bit 0-2 =   Timer-Function
                                          //  Bit 3   =   Timer-Interrupt-Enable
                                          //  Bit 4-5 =   Clock-Alarm-Function
                                          //  Bit 6   =   Timer-Alarm-Enable
                                          //  Bit 7   =   Alarm-Interrupt-Enable
#define RTC_REG_EXTYEAR          0x11     // RTC-PCF8583 address of , extended Years        
#define RTC_REG_EXTYEARCENT      0x12     // RTC-PCF8583 address of , extended Years (Centuries)        
#define RTC_REG_BUF_YEAR_CHANGE  0x13     // RTC-PCF8583 address of , buffer of Years        
#define RTC_REG_RAMSTART         0x20     // RTC-PCF8583 address of , Start Free RAM



//
//
unsigned char PCF_Read_from_Ram(unsigned char AdressDev, unsigned char AdressRAM)
{
  unsigned char data;
  i2c_start_wait(AdressDev+I2C_WRITE);     // set device address and write mode

  i2c_write(AdressRAM);                        // write address = 5
  i2c_rep_start(AdressDev+I2C_READ);       // set device address and read mode

  data = i2c_readNak();                    // read one byte from EEPROM
  i2c_stop();
  
	return data;
}
//
//
void PCF_Write_to_Ram(unsigned char AdressDev, unsigned char AdressRAM, char Value)
{
  i2c_start_wait(AdressDev+I2C_WRITE);     // set device address and write mode
  i2c_write(AdressRAM);                    // write address = 5
  i2c_write(Value);                        // write value 0x75 to EEPROM
  i2c_stop();                              // set stop conditon = release bus
}
//
//
void PCF_Clock_Stop(unsigned char AdressDev)
{
  unsigned char data;
  data = PCF_Read_from_Ram(AdressDev, RTC_REG_CTL);
	data |= 0b10000000;
  PCF_Write_to_Ram(AdressDev, RTC_REG_CTL, data);
}
//
//
void PCF_Clock_Start(unsigned char AdressDev)
{
  unsigned char data;
  data = PCF_Read_from_Ram(AdressDev, RTC_REG_CTL);
	data &= 0b01111111;
  PCF_Write_to_Ram(AdressDev, RTC_REG_CTL, data);
}
//
//
unsigned char bcd2hex(unsigned char bcd)
{
  unsigned char buffer;
  
  buffer = (bcd / 16) * 10 +  (bcd % 16);

  return buffer ;
}         
//
//
unsigned char hex2bcd(unsigned char hex)
{
  unsigned char buffer;
  
  buffer = (hex / 10) * 16 +  (hex % 10);

  return buffer ;
}         
//
//

//
//


//
//
//
//
//*************************************************************************
// Function:  pcf8583_init(addr)
// Purpose :  initialise the Control-Reg of the RTC I2C device   
// para    :  addr address of I2C device  
// Returns :  none
//*************************************************************************
void pcf8583_init(unsigned char addr)
{   
  unsigned char data;
  PCF_Write_to_Ram(addr, RTC_REG_CTL, 0);//, buffer);
  data = PCF_Read_from_Ram(addr, RTC_REG_YEARDAY) >> 6;
	data &= 0b00000011;
  PCF_Write_to_Ram(addr, RTC_REG_BUF_YEAR_CHANGE, data);
}
// pcf8583_init  
//
//
//*************************************************************************
// Function:  pcf8583_get_Time(addr,hours,minutes,seconds)
// Purpose :  returns the time of the RTC specified by I2C device adress 
// para    :  addr    address of I2C device  
// para    :  hours   value as pointer 
// para    :  minutes value as pointer 
// para    :  seconds value as pointer
// Returns :  return values will be store in the parameters
//*************************************************************************
void pcf8583_get_Time(unsigned char addr, unsigned char *hours,
                       unsigned char *minutes, unsigned char *seconds)
{   
  *hours   = pcf8583_get_hour(addr);
  *minutes = pcf8583_get_min(addr);
  *seconds = pcf8583_get_sec(addr);
}
// pcf8583_get_Time  
//
//
//*************************************************************************
// Function:  pcf8583_set_Time(addr,hours,minutes,seconds)
// Purpose :  set the time of the RTC specified by I2C device adress 
// para    :  addr    address of I2C device  
// para    :  hours   value as pointer 
// para    :  minutes value as pointer 
// para    :  seconds value as pointer
// Returns :  none
//*************************************************************************
void pcf8583_set_Time(unsigned char addr, unsigned char *hours,
                       unsigned char *minutes, unsigned char *seconds)
{   
  pcf8583_set_hour(addr, *hours);
  pcf8583_set_min(addr, *minutes);
  pcf8583_set_sec(addr, *seconds);
}
// pcf8583_set_Time  
//
//
//*************************************************************************
// Function:  pcf8583_get_Date(addr,weekday,day,month,year)
// Purpose :  returns the date of the RTC specified by I2C device adress 
// para    :  addr     address of I2C device  
// para    :  weekday  value as pointer
// para    :  day      value as pointer
// para    :  month    value as pointer
// para    :  year     value as pointer
// Returns :  return values will be store in the parameters
//*************************************************************************
void pcf8583_get_Date(unsigned char addr, unsigned char *weekday,
                       unsigned char *day, unsigned char *month, 
											 unsigned char *year)
{   
  *weekday = pcf8583_get_weekday(addr);
  *day     = pcf8583_get_day(addr);
  *month   = pcf8583_get_month(addr);
  *year    = pcf8583_get_year(addr);
}
// pcf8583_get_Date  
//
//
//*************************************************************************
// Function:  pcf8583_set_Date(addr,weekday,day,month,year)
// Purpose :  set the date of the RTC specified by I2C device adress 
// para    :  addr     address of I2C device  
// para    :  weekday  value as pointer
// para    :  day      value as pointer
// para    :  month    value as pointer
// para    :  year     value as pointer
// Returns :  none
//*************************************************************************
void pcf8583_set_Date(unsigned char addr, unsigned char *weekday,
                       unsigned char *day, unsigned char *month, 
											 unsigned char *year)
{   
  pcf8583_set_weekday(addr, *weekday);
  pcf8583_set_day(addr, *day);
  pcf8583_set_month(addr, *month);
  pcf8583_set_year(addr, *year);
}
// pcf8583_set_Date  
//
//
//*************************************************************************
// Function:  pcf8583_get_weekday(addr)
// Purpose :  returns the weekday of the RTC specified by I2C device adress 
// para    :  addr    address of I2C device  
// Returns :  weekday of RTC
//*************************************************************************
unsigned char pcf8583_get_weekday(unsigned char addr)
{   
  unsigned char data;
  data = PCF_Read_from_Ram(addr, RTC_REG_WDMONTH) >> 5;
	data &= 0b00000111;
  return data;
}
// pcf8583_get_weekday  
//
//
//*************************************************************************
// Function:  pcf8583_set_weekday(addr)
// Purpose :  set the weekdays of the RTC specified by I2C device adress 
// para    :  addr address of I2C device  
// Returns :  none
//*************************************************************************
void pcf8583_set_weekday(unsigned char addr, unsigned char value)
{   
  unsigned char data, buffer;
	data = value << 5;  
  buffer = PCF_Read_from_Ram(addr, RTC_REG_WDMONTH);
	buffer &= 0b00011111;
	buffer |= data;
  PCF_Clock_Stop(addr);
  PCF_Write_to_Ram(addr, RTC_REG_WDMONTH, buffer);  
  PCF_Clock_Start(addr);
}
// pcf8583_set_weekday  
//
//
//*************************************************************************
// Function:  pcf8583_get_day(addr)
// Purpose :  returns the days of the RTC specified by I2C device adress 
// para    :  addr address of I2C device  
// Returns :  days of RTC
//*************************************************************************
unsigned char pcf8583_get_day(unsigned char addr)
{   
  unsigned char data;
  data = PCF_Read_from_Ram(addr, RTC_REG_YEARDAY);
	data &= 0b00111111;
  return bcd2hex(data);
}
// pcf8583_get_day  
//
//
//*************************************************************************
// Function:  pcf8583_set_day(addr)
// Purpose :  set the days of the RTC specified by I2C device adress 
// para    :  addr address of I2C device  
// Returns :  none
//*************************************************************************
void pcf8583_set_day(unsigned char addr, unsigned char value)
{   
  unsigned char buffer;
  buffer = PCF_Read_from_Ram(addr, RTC_REG_YEARDAY);
	buffer &= 0b11000000;
	buffer |= hex2bcd(value);
  PCF_Clock_Stop(addr);
  PCF_Write_to_Ram(addr, RTC_REG_YEARDAY, buffer);  
  PCF_Clock_Start(addr);
}
// pcf8583_set_day  
//
//
//*************************************************************************
// Function:  pcf8583_get_month(addr)
// Purpose :  returns the month of the RTC specified by I2C device adress 
// para    :  addr address of I2C device  
// Returns :  month of RTC
//*************************************************************************
unsigned char pcf8583_get_month(unsigned char addr)
{   
  unsigned char data;
  data = PCF_Read_from_Ram(addr, RTC_REG_WDMONTH);
	data &= 0b00011111;
  return bcd2hex(data);
}
// pcf8583_get_month  
//
//
//*************************************************************************
// Function:  pcf8583_set_month(addr)
// Purpose :  set the months of the RTC specified by I2C device adress 
// para    :  addr address of I2C device  
// Returns :  none
//*************************************************************************
void pcf8583_set_month(unsigned char addr, unsigned char value)
{   
  unsigned char buffer;
  buffer = PCF_Read_from_Ram(addr, RTC_REG_WDMONTH);
	buffer &= 0b11100000;
	buffer |= hex2bcd(value);
  PCF_Clock_Stop(addr);
  PCF_Write_to_Ram(addr, RTC_REG_WDMONTH, buffer);  
  PCF_Clock_Start(addr);
}
// pcf8583_set_month  
//
//
//*************************************************************************
// Function:  pcf8583_get_year(addr)
// Purpose :  returns the years of the RTC specified by I2C device adress 
// para    :  addr address of I2C device  
// Returns :  years of RTC
//*************************************************************************
unsigned char pcf8583_get_year(unsigned char addr)
{   
  unsigned char data, buffer;
  buffer = PCF_Read_from_Ram(addr, RTC_REG_YEARDAY) >> 6;
	buffer &= 0b00000011;
  data = PCF_Read_from_Ram(addr, RTC_REG_YEARDAY) >> 6;
	data &= 0b00000011;

	if (buffer != data)
	{
    PCF_Write_to_Ram(addr, RTC_REG_BUF_YEAR_CHANGE, data);
    buffer = PCF_Read_from_Ram(addr, RTC_REG_EXTYEAR);
    buffer += 1;
    PCF_Write_to_Ram(addr, RTC_REG_EXTYEAR, buffer);
	}

  data += PCF_Read_from_Ram(addr, RTC_REG_EXTYEAR);
  return data;
}
// pcf8583_get_year  
//
//
//*************************************************************************
// Function:  pcf8583_set_year(addr)
// Purpose :  set the years of the RTC specified by I2C device adress 
// para    :  addr address of I2C device  
// Returns :  none
//*************************************************************************
void pcf8583_set_year(unsigned char addr, unsigned char value)
{   
  unsigned char data, buffer;
	data = value % 4;
  PCF_Write_to_Ram(addr, RTC_REG_EXTYEAR, value - data);
	buffer = data << 6;  
	data = buffer;	
  PCF_Write_to_Ram(addr, RTC_REG_BUF_YEAR_CHANGE, data);
  buffer = PCF_Read_from_Ram(addr, RTC_REG_YEARDAY);
	buffer &= 0b00111111;
	buffer |= data;
  PCF_Clock_Stop(addr);
  PCF_Write_to_Ram(addr, RTC_REG_YEARDAY, buffer);  
  PCF_Clock_Start(addr);
}
// pcf8583_set_year  
//
//
//*************************************************************************
// Function:  pcf8583_get_hour(addr)
// Purpose :  returns the hours of the RTC specified by I2C device adress 
// para    :  addr address of I2C device  
// Returns :  hours of RTC
//*************************************************************************
unsigned char pcf8583_get_hour(unsigned char addr)
{   
  unsigned char data;
  data = PCF_Read_from_Ram(addr, RTC_REG_HOUR);
	data &= 0b00111111;
  return bcd2hex(data);
}
// pcf8583_get_hour  
//
//
//*************************************************************************
// Function:  pcf8583_set_hour(addr)
// Purpose :  set the hours of the RTC specified by I2C device adress 
// para    :  addr address of I2C device  
// Returns :  none
//*************************************************************************
void pcf8583_set_hour(unsigned char addr, unsigned char value)
{   
  PCF_Clock_Stop(addr);
  PCF_Write_to_Ram(addr, RTC_REG_HOUR, hex2bcd(value));  
  PCF_Clock_Start(addr);
}
// pcf8583_set_hour  
//
//
//*************************************************************************
// Function:  pcf8583_get_min(addr)
// Purpose :  returns the minutes of the RTC specified by I2C device adress 
// para    :  addr address of I2C device  
// Returns :  minutes of RTC
//*************************************************************************
unsigned char pcf8583_get_min(unsigned char addr)
{    
  return bcd2hex(PCF_Read_from_Ram(addr, RTC_REG_MIN));
}
// pcf8583_get_min 
//
//
//*************************************************************************
// Function:  pcf8583_set_min(addr)
// Purpose :  set the minutes of the RTC specified by I2C device adress 
// para    :  addr address of I2C device  
// Returns :  none
//*************************************************************************
void pcf8583_set_min(unsigned char addr, unsigned char value)
{   
  PCF_Clock_Stop(addr);
  PCF_Write_to_Ram(addr, RTC_REG_MIN, hex2bcd(value));  
  PCF_Clock_Start(addr);
}
// pcf8583_set_min  
//
//
//*************************************************************************
// Function:  pcf8583_get_sec(addr)
// Purpose :  returns the seconds of the RTC specified by I2C device adress 
// para    :  addr address of I2C device  
// Returns :  seconds of RTC
//*************************************************************************
unsigned char pcf8583_get_sec(unsigned char addr)
{    
  return bcd2hex(PCF_Read_from_Ram(addr, RTC_REG_SEC));
}
// pcf8583_get_sec 
//
//
//*************************************************************************
// Function:  pcf8583_set_sec(addr)
// Purpose :  set the seconds of the RTC specified by I2C device adress 
// para    :  addr address of I2C device  
// Returns :  none
//*************************************************************************
void pcf8583_set_sec(unsigned char addr, unsigned char value)
{   
  PCF_Clock_Stop(addr);
  PCF_Write_to_Ram(addr, RTC_REG_SEC, hex2bcd(value));  
  PCF_Clock_Start(addr);
}
// pcf8583_set_sec  
//
//
//*************************************************************************
// Function:  pcf8583_get_hsec(addr)
// Purpose :  returns the hundsec of the RTC specified by I2C device adress 
// para    :  addr address of I2C device  
// Returns :  seconds of RTC
//*************************************************************************
unsigned char pcf8583_get_hsec(unsigned char addr)
{    
  return bcd2hex(PCF_Read_from_Ram(addr, RTC_REG_HSEC));
}
// pcf8583_get_hsec 
//
//
//*************************************************************************
// Function:  pcf8583_set_hsec(addr)
// Purpose :  set the hundsec of the RTC specified by I2C device adress 
// para    :  addr address of I2C device  
// Returns :  seconds none
//*************************************************************************
void pcf8583_set_hsec(unsigned char addr, unsigned char value)
{    
  PCF_Clock_Stop(addr);
  PCF_Write_to_Ram(addr, RTC_REG_HSEC, hex2bcd(value));  
  PCF_Clock_Start(addr);
}
// pcf8583_set_hsec 
//
//
//*************************************************************************
// Function:  pcf8583_ram_read(addr,memaddr)
// Purpose :  returns the value of the RTC specified by I2C device adress 
// para    :  addr    address of I2C device  
// para    :  memaddr memoryaddress of I2C device  
// Returns :  value from adress of specified RTC
//*************************************************************************
unsigned char pcf8583_ram_read(unsigned char addr, unsigned char memaddr)
{    
  return PCF_Read_from_Ram(addr, memaddr);
}
// pcf8583_ram_read 
//
//
//*************************************************************************
// Function:  pcf8583_ram_write(addr,memaddr,value)
// Purpose :  set the value of the RTC specified by I2C device adress 
// para    :  addr    address of I2C device  
// para    :  memaddr memoryaddress of I2C device  
// para    :  value      
// Returns :  none
//*************************************************************************
void pcf8583_ram_write(unsigned char addr, unsigned char memaddr, unsigned char value)
{    
  PCF_Write_to_Ram(addr, memaddr, value);  
}
// pcf8583_ram_write 
//
//
//
//


