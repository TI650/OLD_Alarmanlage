//***************************************************************************
//* 
//* Projekt Name		: SCS_PCF8574 
//* Projekt Type		: Library, 8 Bit Port-Expander for I2C-Bus
//* Main Titel	    :    
//* Sub Titel	      : 
//* File Name	  	  : scs_pcf8574.c
//* Date            : 05.02.2008
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

#include "scs_pcf8574.h"
#include "i2cmaster.h"

//
//  constants and macros
//


//
//
unsigned char pcf8574_read_port(unsigned char AdressDev)
{
  unsigned char data;
//	data = 0x15;
  i2c_start_wait(AdressDev+I2C_WRITE);     // set device address and write mode
  i2c_rep_start(AdressDev+I2C_READ);       // set device address and read mode
  data = i2c_readNak();                    // read one byte from I2C-Device
  i2c_stop();
	return data;
}
//
//
void pcf8574_write_port(unsigned char AdressDev, char Value)
{
  i2c_start_wait(AdressDev+I2C_WRITE);     // set device address and write mode
  i2c_write(Value);                        // write value 0x75 to EEPROM
  i2c_stop();                              // set stop conditon = release bus
}
//
//



//*************************************************************************
// Function:  pcf8574_set_port(addr, value)
// Purpose :  set a value in the port specified by I2C device adress 
// para    :  addr  = address of I2C device 
//            value = value of port 
// Returns :  none
//*************************************************************************
extern void pcf8574_set_port(unsigned char addr, unsigned char value)
{   
	pcf8574_write_port(addr, value);
}
// pcf8574_set_port  
//
//
//*************************************************************************
// Function:  pcf8574_get_port(addr)
// Purpose :  get the value of the port specified by I2C device adress 
// para    :  addr  = address of I2C device 
// Returns :  value of port
//*************************************************************************
extern unsigned char pcf8574_get_port(unsigned char addr)
{   
  unsigned char data;
  data = pcf8574_read_port(addr);
	return data; // 0x0F; //
}
// pcf8574_get_port  
//
//
//
//
//


