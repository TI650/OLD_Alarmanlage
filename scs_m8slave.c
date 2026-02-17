//***************************************************************************
//* 
//* Projekt Name		: SCS_M8SLAVE 
//* Projekt Type		: Library, AtMega8 as I2C-Slave
//* Main Titel	    :    
//* Sub Titel	      : 
//* File Name	  	  : scs_m8slave.c
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

#include "scs_m8slave.h"
#include "i2cmaster.h"

#define adc_on                          1
#define adc_off                         0

#define pin_as_output                   1
#define pin_as_input                    0

#define pin_pullup_on                   1
#define pin_pullup_off                  0

#define pin_high                        1
#define pin_low                         0



#define cmd_twi_slave_get_adc         100
#define cmd_twi_slave_set_adc         110
#define cmd_twi_slave_eeprom_read     180
#define cmd_twi_slave_eeprom_write    181

#define cmd_twi_slave_set_pinb_ddr    120
#define cmd_twi_slave_set_pinb_pullup 121
#define cmd_twi_slave_set_pinb        122
#define cmd_twi_slave_get_pinb        123

#define cmd_twi_slave_set_pinc_ddr    130
#define cmd_twi_slave_set_pinc_pullup 131
#define cmd_twi_slave_set_pinc        132
#define cmd_twi_slave_get_pinc        133

#define cmd_twi_slave_set_pind_ddr    140
#define cmd_twi_slave_set_pind_pullup 141
#define cmd_twi_slave_set_pind        142
#define cmd_twi_slave_get_pind        143






//
//  constants and macros
//

	uint8_t		Data_Cmd[4];
	uint8_t		Data_Ret[4];


//
//
void m8slave_write_cmd(unsigned char AdressDev)
{
	uint8_t		i;

  i2c_start_wait(AdressDev+I2C_WRITE);     // set device address and write mode
	for (i=0;i<4;i++)
	{
		i2c_write(Data_Cmd[i]);            // write value 
	}
  i2c_stop();                              // set stop conditon = release bus
}
//
//

//
//
void m8slave_read_val(unsigned char AdressDev)
{
	uint8_t		i;

  i2c_start_wait(AdressDev+I2C_READ);     // set device address and write mode
	for (i=0;i<3;i++)
	{
		Data_Ret[i] = i2c_readAck();            // read value 
	}
 	Data_Ret[3] = i2c_readNak();
  i2c_stop();                              // set stop conditon = release bus
//  i2c_stop();                              // set stop conditon = release bus
	
}
//
//
//*************************************************************************
// Function:  m8slave_ 
// Purpose :    
// para    :    
// Returns :   
//*************************************************************************
void m8slave_set_pin_ddr(unsigned char addr, unsigned char cmd_ddr, unsigned char pin, unsigned char value)
{   
  Data_Cmd[0]= cmd_ddr ;
//  Data_Cmd[1]= 0 ;
//  Data_Cmd[2]= 0 ;
  Data_Cmd[1]= pin ;
//  Data_Cmd[4]= 0 ;
  Data_Cmd[2]= value ;
  Data_Cmd[3]= 0 ;
	m8slave_write_cmd(addr);
}
//
//
//*************************************************************************
// Function:  m8slave_ 
// Purpose :    
// para    :    
// Returns :   
//*************************************************************************
void m8slave_set_pin_pullup(unsigned char addr, unsigned char cmd_pullup, unsigned char pin, unsigned char value)
{   
  Data_Cmd[0]= cmd_pullup ;
//  Data_Cmd[1]= 0 ;
//  Data_Cmd[2]= 0 ;
  Data_Cmd[1]= pin ;
//  Data_Cmd[4]= 0 ;
  Data_Cmd[2]= value ;
  Data_Cmd[3]= 0 ;
	m8slave_write_cmd(addr);
}
//
//
//*************************************************************************
// Function:  m8slave_ 
// Purpose :    
// para    :    
// Returns :   
//*************************************************************************
void m8slave_set_pin(unsigned char addr, unsigned char cmd_set_pin, unsigned char pin, unsigned char value)
{   
  Data_Cmd[0]= cmd_set_pin ;
//  Data_Cmd[1]= 0 ;
//  Data_Cmd[2]= 0 ;
  Data_Cmd[1]= pin ;
//  Data_Cmd[4]= 0 ;
  Data_Cmd[2]= value ;
  Data_Cmd[3]= 0 ;
	m8slave_write_cmd(addr);
}
//
//
//*************************************************************************
// Function:  m8slave_ 
// Purpose :    
// para    :    
// Returns :   
//*************************************************************************
unsigned char m8slave_get_pin(unsigned char addr, unsigned char cmd_get_pin, unsigned char pin)
{   
  signed int data;
  Data_Cmd[0]= cmd_get_pin ;
//  Data_Cmd[1]= 0 ;
//  Data_Cmd[2]= 0 ;
  Data_Cmd[1]= pin ;
//  Data_Cmd[4]= 0 ;
//  Data_Cmd[5]= 0 ;
  Data_Cmd[2]= 0 ;
  Data_Cmd[3]= 0 ;
	m8slave_write_cmd(addr);
  m8slave_read_val(addr);	
	data = Data_Ret[1];
	return data; // 0x0F; //
}






//
//
//*************************************************************************
// Function:  m8slave_ 
// Purpose :    
// para    :    
// Returns :   
//*************************************************************************
extern void m8slave_set_adc(unsigned char addr, unsigned char channel, unsigned char on_off)
{   
  Data_Cmd[0]= cmd_twi_slave_set_adc ;
//  Data_Cmd[1]= 0 ;
//  Data_Cmd[2]= 0 ;
  Data_Cmd[1]= channel ;
//  Data_Cmd[4]= 0 ;
  Data_Cmd[2]= on_off ;
  Data_Cmd[3]= 0 ;
	m8slave_write_cmd(addr);
}
//
//
//*************************************************************************
// Function:  m8slave_ 
// Purpose :    
// para    :    
// Returns :   
//*************************************************************************
extern signed int m8slave_get_adc(unsigned char addr, unsigned char channel)
{   
  signed int data;
  Data_Cmd[0]= cmd_twi_slave_get_adc ;
//  Data_Cmd[1]= 0 ;
//  Data_Cmd[2]= 0 ;
  Data_Cmd[1]= channel ;
  Data_Cmd[2]= 0 ;
  Data_Cmd[3]= 0 ;
//  Data_Cmd[4]= 0 ;
//  Data_Cmd[5]= 0 ;
	m8slave_write_cmd(addr);
  m8slave_read_val(addr);	
	data = (Data_Ret[1]<<8)+Data_Ret[2];
	return data; // 0x0F; //
}
// m8slave_get_port  
//
//
//*************************************************************************
// Function:  m8slave_ 
// Purpose :    
// para    :    
// Returns :   
//*************************************************************************
extern void m8slave_write_eeprom(unsigned char addr, unsigned char eeaddr, unsigned char value)
{   
  Data_Cmd[0]= cmd_twi_slave_eeprom_write ;
//  Data_Cmd[1]= 0 ;
//  Data_Cmd[2]= 0 ;
  Data_Cmd[1]= eeaddr ;
//  Data_Cmd[4]= 0 ;
  Data_Cmd[2]= value ;
  Data_Cmd[3]= 0 ;
	m8slave_write_cmd(addr);
}
// m8slave_get_port  
//
//
//*************************************************************************
// Function:  m8slave_ 
// Purpose :    
// para    :    
// Returns :   
//*************************************************************************
extern unsigned char m8slave_read_eeprom(unsigned char addr, unsigned char eeaddr)
{   
  signed int data;
  Data_Cmd[0]= cmd_twi_slave_eeprom_read ;
//  Data_Cmd[1]= 0 ;
//  Data_Cmd[2]= 0 ;
  Data_Cmd[1]= eeaddr ;
//  Data_Cmd[4]= 0 ;
//  Data_Cmd[5]= 0 ;
  Data_Cmd[2]= 0 ;
  Data_Cmd[3]= 0 ;
	m8slave_write_cmd(addr);
  m8slave_read_val(addr);	
	data = Data_Ret[1];
	return data; // 0x0F; //
}

//
//
//*************************************************************************
// Function:  m8slave_ 
// Purpose :    
// para    :    
// Returns :   
//*************************************************************************
extern void m8slave_set_pinb_ddr(unsigned char addr, unsigned char pin, unsigned char value)
{   
  m8slave_set_pin_ddr(addr, cmd_twi_slave_set_pinb_ddr, pin, value);
}
//
//
//*************************************************************************
// Function:  m8slave_ 
// Purpose :    
// para    :    
// Returns :   
//*************************************************************************
extern void m8slave_set_pinc_ddr(unsigned char addr, unsigned char pin, unsigned char value)
{   
  m8slave_set_pin_ddr(addr, cmd_twi_slave_set_pinc_ddr, pin, value);
}
//
//
//*************************************************************************
// Function:  m8slave_ 
// Purpose :    
// para    :    
// Returns :   
//*************************************************************************
extern void m8slave_set_pind_ddr(unsigned char addr, unsigned char pin, unsigned char value)
{   
  m8slave_set_pin_ddr(addr, cmd_twi_slave_set_pind_ddr, pin, value);
}
//
//
//*************************************************************************
// Function:  m8slave_ 
// Purpose :    
// para    :    
// Returns :   
//*************************************************************************
extern void m8slave_set_pinb_pullup(unsigned char addr, unsigned char pin, unsigned char value)
{   
  m8slave_set_pin_pullup(addr, cmd_twi_slave_set_pinb_pullup, pin, value);
}
//
//
//*************************************************************************
// Function:  m8slave_ 
// Purpose :    
// para    :    
// Returns :   
//*************************************************************************
extern void m8slave_set_pinc_pullup(unsigned char addr, unsigned char pin, unsigned char value)
{   
  m8slave_set_pin_pullup(addr, cmd_twi_slave_set_pinc_pullup, pin, value);
}
//
//
//*************************************************************************
// Function:  m8slave_ 
// Purpose :    
// para    :    
// Returns :   
//*************************************************************************
extern void m8slave_set_pind_pullup(unsigned char addr, unsigned char pin, unsigned char value)
{   
  m8slave_set_pin_pullup(addr, cmd_twi_slave_set_pind_pullup, pin, value);
}
//
//
//*************************************************************************
// Function:  m8slave_ 
// Purpose :    
// para    :    
// Returns :   
//*************************************************************************
extern void m8slave_set_pinb(unsigned char addr, unsigned char pin, unsigned char value)
{   
  m8slave_set_pin(addr, cmd_twi_slave_set_pinb, pin, value);
}
//
//
//*************************************************************************
// Function:  m8slave_ 
// Purpose :    
// para    :    
// Returns :   
//*************************************************************************
extern void m8slave_set_pinc(unsigned char addr, unsigned char pin, unsigned char value)
{   
  m8slave_set_pin(addr, cmd_twi_slave_set_pinc, pin, value);
}
//
//
//*************************************************************************
// Function:  m8slave_ 
// Purpose :    
// para    :    
// Returns :   
//*************************************************************************
extern void m8slave_set_pind(unsigned char addr, unsigned char pin, unsigned char value)
{   
  m8slave_set_pin(addr, cmd_twi_slave_set_pind, pin, value);
}
//
//
//*************************************************************************
// Function:  m8slave_ 
// Purpose :    
// para    :    
// Returns :   
//*************************************************************************
extern unsigned char m8slave_get_pinb(unsigned char addr, unsigned char pin)
{   
  unsigned int data;
	data = m8slave_get_pin(addr, cmd_twi_slave_get_pinb, pin);
	return data; 
}
//
//
//*************************************************************************
// Function:  m8slave_ 
// Purpose :    
// para    :    
// Returns :   
//*************************************************************************
extern unsigned char m8slave_get_pinc(unsigned char addr, unsigned char pin)
{   
  unsigned int data;
	data = m8slave_get_pin(addr, cmd_twi_slave_get_pinc, pin);
	return data; 
}
//
//
//*************************************************************************
// Function:  m8slave_ 
// Purpose :    
// para    :    
// Returns :   
//*************************************************************************
extern unsigned char m8slave_get_pind(unsigned char addr, unsigned char pin)
{   
  unsigned int data;
	data = m8slave_get_pin(addr, cmd_twi_slave_get_pind, pin);
	return data; 
}
//
//
//
//
//
//
//
//
