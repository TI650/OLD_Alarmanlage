#ifndef SCS_M8SLAVE_H
#define SCS_M8SLAVE_H
//***************************************************************************
//* 
//* Projekt Name		: SCS_M8SLAVE 
//* Projekt Type		: Library, AtMega8 as I2C-Slave
//* Main Titel	    :    
//* Sub Titel	      : 
//* File Name	  	  : scs_m8slave.h
//* Date            : 15.02.2008
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


#define adc_on                          1
#define adc_off                         0

#define pin_as_output                   1
#define pin_as_input                    0

#define pin_pullup_on                   1
#define pin_pullup_off                  0

#define pin_high                        1
#define pin_low                         0




//
//

//*************************************************************************
// Function:  m8slave_ 
// Purpose :    
// para    :   
// Returns :   
//*************************************************************************
extern void m8slave_set_adc(unsigned char addr, unsigned char channel, unsigned char on_off);

//*************************************************************************
// Function:  m8slave_ 
// Purpose :    
// para    :   
// Returns :   
//*************************************************************************
extern signed int m8slave_get_adc(unsigned char addr, unsigned char channel);

//*************************************************************************
// Function:  m8slave_ 
// Purpose :    
// para    :    
// Returns :   
//*************************************************************************
extern void m8slave_write_eeprom(unsigned char addr, unsigned char eeaddr, unsigned char value);

//*************************************************************************
// Function:  m8slave_ 
// Purpose :    
// para    :    
// Returns :   
//*************************************************************************
extern unsigned char m8slave_read_eeprom(unsigned char addr, unsigned char eeaddr);

//*************************************************************************
// Function:  m8slave_ 
// Purpose :    
// para    :    
// Returns :   
//*************************************************************************
extern void m8slave_set_pinb_ddr(unsigned char addr, unsigned char pin, unsigned char value);

//*************************************************************************
// Function:  m8slave_ 
// Purpose :    
// para    :    
// Returns :   
//*************************************************************************
extern void m8slave_set_pinc_ddr(unsigned char addr, unsigned char pin, unsigned char value);

//*************************************************************************
// Function:  m8slave_ 
// Purpose :    
// para    :    
// Returns :   
//*************************************************************************
extern void m8slave_set_pind_ddr(unsigned char addr, unsigned char pin, unsigned char value);

//*************************************************************************
// Function:  m8slave_ 
// Purpose :    
// para    :    
// Returns :   
//*************************************************************************
extern void m8slave_set_pinb_pullup(unsigned char addr, unsigned char pin, unsigned char value);

//*************************************************************************
// Function:  m8slave_ 
// Purpose :    
// para    :    
// Returns :   
//*************************************************************************
extern void m8slave_set_pinc_pullup(unsigned char addr, unsigned char pin, unsigned char value);

//*************************************************************************
// Function:  m8slave_ 
// Purpose :    
// para    :    
// Returns :   
//*************************************************************************
extern void m8slave_set_pind_pullup(unsigned char addr, unsigned char pin, unsigned char value);

//*************************************************************************
// Function:  m8slave_ 
// Purpose :    
// para    :    
// Returns :   
//*************************************************************************
extern void m8slave_set_pinb(unsigned char addr, unsigned char pin, unsigned char value);

//*************************************************************************
// Function:  m8slave_ 
// Purpose :    
// para    :    
// Returns :   
//*************************************************************************
extern void m8slave_set_pinc(unsigned char addr, unsigned char pin, unsigned char value);

//*************************************************************************
// Function:  m8slave_ 
// Purpose :    
// para    :    
// Returns :   
//*************************************************************************
extern void m8slave_set_pind(unsigned char addr, unsigned char pin, unsigned char value);

//*************************************************************************
// Function:  m8slave_ 
// Purpose :    
// para    :    
// Returns :   
//*************************************************************************
extern unsigned char m8slave_get_pinb(unsigned char addr, unsigned char pin);

//*************************************************************************
// Function:  m8slave_ 
// Purpose :    
// para    :    
// Returns :   
//*************************************************************************
extern unsigned char m8slave_get_pinc(unsigned char addr, unsigned char pin);

//*************************************************************************
// Function:  m8slave_ 
// Purpose :    
// para    :    
// Returns :   
//*************************************************************************
extern unsigned char m8slave_get_pind(unsigned char addr, unsigned char pin);


/**@}*/


#endif  //SCS_M8SLAVE_H
