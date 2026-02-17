#ifndef SCS_PCF8583_H
#define SCS_PCF8583_H
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
//  @author Manfred Kraus  m.kraus@suark.com
//
 
/**@{*/


#if (__GNUC__ * 100 + __GNUC_MINOR__) < 304
#error "This library requires AVR-GCC 3.4 or later, update to newer AVR-GCC compiler !"
#endif


//
//
//*************************************************************************
// Function:  pcf8583_init(addr)
// Purpose :  initialise the Control-Reg of the RTC I2C device   
// para    :  addr address of I2C device  
// Returns :  none
//*************************************************************************
extern void pcf8583_init(unsigned char addr);
//
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
extern void pcf8583_get_Time(unsigned char addr, unsigned char *hours,
                       unsigned char *minutes, unsigned char *seconds);
//
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
extern void pcf8583_set_Time(unsigned char addr, unsigned char *hours,
                       unsigned char *minutes, unsigned char *seconds);
//
//
//
//*************************************************************************
// Function:  pcf8583_get_Date(addr,weekday,day,month,year)
// Purpose :  returns the date of the RTC specified by I2C device adress 
// para    :  addr    address of I2C device  
// para    :  weekday   value as pointer
// para    :  day   value as pointer
// para    :  month value as pointer
// para    :  year value as pointer
// Returns :  return values will be store in the parameters
//*************************************************************************
extern void pcf8583_get_Date(unsigned char addr, unsigned char *weekday,
                       unsigned char *day, unsigned char *month, 
											 unsigned char *year);
//  
//
//
//*************************************************************************
// Function:  pcf8583_set_Date(addr,weekday,day,month,year)
// Purpose :  set the date of the RTC specified by I2C device adress 
// para    :  addr    address of I2C device  
// para    :  weekday   value as pointer
// para    :  day   value as pointer
// para    :  month value as pointer
// para    :  year value as pointer
// Returns :  none
//*************************************************************************
extern void pcf8583_set_Date(unsigned char addr, unsigned char *weekday,
                       unsigned char *day, unsigned char *month, 
											 unsigned char *year);
//   
//
//
//*************************************************************************
// Function:  pcf8583_get_weekday(addr)
// Purpose :  returns the weekday of the RTC specified by I2C device adress 
// para    :  addr address of I2C device  
// Returns :  weekday of RTC
//*************************************************************************
extern unsigned char pcf8583_get_weekday(unsigned char addr);
//
//
//
//*************************************************************************
// Function:  pcf8583_set_weekday(addr)
// Purpose :  set the weekdays of the RTC specified by I2C device adress 
// para    :  addr address of I2C device  
// Returns :  none
//*************************************************************************
extern void pcf8583_set_weekday(unsigned char addr, unsigned char value);
//
//
//
//*************************************************************************
// Function:  pcf8583_get_day(addr)
// Purpose :  returns the days of the RTC specified by I2C device adress 
// para    :  addr address of I2C device  
// Returns :  days of RTC
//*************************************************************************
extern unsigned char pcf8583_get_day(unsigned char addr);
//
//
//
//*************************************************************************
// Function:  pcf8583_set_day(addr)
// Purpose :  set the days of the RTC specified by I2C device adress 
// para    :  addr address of I2C device  
// Returns :  none
//*************************************************************************
extern void pcf8583_set_day(unsigned char addr, unsigned char value);
//
//
//
//*************************************************************************
// Function:  pcf8583_get_month(addr)
// Purpose :  returns the month of the RTC specified by I2C device adress 
// para    :  addr address of I2C device  
// Returns :  month of RTC
//*************************************************************************
extern unsigned char pcf8583_get_month(unsigned char addr);
//
//
//
//
//*************************************************************************
// Function:  pcf8583_set_month(addr)
// Purpose :  set the months of the RTC specified by I2C device adress 
// para    :  addr address of I2C device  
// Returns :  none
//*************************************************************************
extern void pcf8583_set_month(unsigned char addr, unsigned char value);
//
//
//
//*************************************************************************
// Function:  pcf8583_get_year(addr)
// Purpose :  returns the years of the RTC specified by I2C device adress 
// para    :  addr address of I2C device  
// Returns :  years of RTC
//*************************************************************************
extern unsigned char pcf8583_get_year(unsigned char addr);
//
//
//
//*************************************************************************
// Function:  pcf8583_set_year(addr)
// Purpose :  set the years of the RTC specified by I2C device adress 
// para    :  addr address of I2C device  
// Returns :  none
//*************************************************************************
extern void pcf8583_set_year(unsigned char addr,  unsigned char value);
//
//
//
//*************************************************************************
// Function:  pcf8583_get_hour(addr)
// Purpose :  returns the hours of the RTC specified by I2C device adress 
// para    :  addr address of I2C device  
// Returns :  hours of RTC
//*************************************************************************
extern unsigned char pcf8583_get_hour(unsigned char addr);
//
//
//
//*************************************************************************
// Function:  pcf8583_set_hour(addr)
// Purpose :  set the hours of the RTC specified by I2C device adress 
// para    :  addr address of I2C device  
// Returns :  none
//*************************************************************************
extern void pcf8583_set_hour(unsigned char addr, unsigned char value);
//
//
//
//*************************************************************************
// Function:  pcf8583_get_min(addr)
// Purpose :  returns the minutes of the RTC specified by I2C device adress 
// para    :  addr address of I2C device  
// Returns :  minutes of RTC
//*************************************************************************
extern unsigned char pcf8583_get_min(unsigned char addr);
//
//
//
//*************************************************************************
// Function:  pcf8583_set_min(addr)
// Purpose :  set the minutes of the RTC specified by I2C device adress 
// para    :  addr address of I2C device  
// Returns :  none
//*************************************************************************
extern void pcf8583_set_min(unsigned char addr, unsigned char value);
//
//
//
//*************************************************************************
// Function:  pcf8583_get_sec(addr)
// Purpose :  returns the seconds of the RTC specified by I2C device adress 
// para    :  addr address of I2C device  
// Returns :  seconds of RTC
//*************************************************************************
extern unsigned char pcf8583_get_sec(unsigned char addr);
//
//
//
//*************************************************************************
// Function:  pcf8583_set_sec(addr)
// Purpose :  set the seconds of the RTC specified by I2C device adress 
// para    :  addr address of I2C device  
// Returns :  none
//*************************************************************************
extern void pcf8583_set_sec(unsigned char addr, unsigned char value);
//
//
//
//*************************************************************************
// Function:  pcf8583_get_hsec(addr)
// Purpose :  returns the hundsec of the RTC specified by I2C device adress 
// para    :  addr address of I2C device  
// Returns :  seconds of RTC
//*************************************************************************
extern unsigned char pcf8583_get_hsec(unsigned char addr);
//
//
//
//*************************************************************************
// Function:  pcf8583_set_hsec(addr)
// Purpose :  set the hundsec of the RTC specified by I2C device adress 
// para    :  addr address of I2C device  
// Returns :  seconds none
//*************************************************************************
extern void pcf8583_set_hsec(unsigned char addr, unsigned char value);
//
//
//
//*************************************************************************
// Function:  pcf8583_ram_read(addr,memaddr)
// Purpose :  returns the value of the RTC specified by I2C device adress 
// para    :  addr    address of I2C device  
// para    :  memaddr memoryaddress of I2C device  
// Returns :  value from adress of specified RTC
//*************************************************************************
extern unsigned char pcf8583_ram_read(unsigned char addr, 
                                        unsigned char memaddr);
//
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
extern void pcf8583_ram_write(unsigned char addr, 
                               unsigned char memaddr, unsigned char value);
//
//
//


/**@}*/


#endif  //SCS_PCF8583_H
