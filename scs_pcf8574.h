#ifndef SCS_PCF8574_H
#define SCS_PCF8574_H
//***************************************************************************
//* 
//* Projekt Name		: SCS_PCF8574 
//* Projekt Type		: Library, 8 Bit Port-Expander for I2C-Bus
//* Main Titel	    :    
//* Sub Titel	      : 
//* File Name	  	  : scs_pcf8574.h
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
//  @author Manfred Kraus  m.kraus@suark.com
//
 
/**@{*/


#if (__GNUC__ * 100 + __GNUC_MINOR__) < 304
#error "This library requires AVR-GCC 3.4 or later, update to newer AVR-GCC compiler !"
#endif


//*************************************************************************
// Function:  pcf8574_set_port(addr, value)
// Purpose :  set a value in the port specified by I2C device adress 
// para    :  addr  = address of I2C device 
//            value = value of port 
// Returns :  none
//*************************************************************************
extern void pcf8583_set_port(unsigned char addr, unsigned char value);
//
//
//
//*************************************************************************
// Function:  pcf8574_get_port(addr)
// Purpose :  get the value of the port specified by I2C device adress 
// para    :  addr  = address of I2C device 
// Returns :  value of port
//*************************************************************************
extern unsigned char pcf8574_get_port(unsigned char addr);
//
//


/**@}*/


#endif  //SCS_PCF8574_H
