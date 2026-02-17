//***************************************************************************
//* 
//* Projekt Name	: AlarmControl 
//*
//* Modul Bez.      : String Konstanten
//*
//* Haupt-Titel	    : 
//* Unter-Titel	    :  
//* Datei Name		: AC_StringConst.c 
//* Datum           : 20.06.2010
//* Version         : 0.0.1000
//* Support Tel 	: +49 43 07 82 46 20
//* Support Fax		: +49 43 07 82 46 21
//* Support Mobil	: +49 17 34 26 69 77
//* Support eMail	: m.kraus@suark.com
//* 
//* -------------------------------------------------------------------------
//* 
//* IDE             : Programmers Notepad 2
//* Compiler        : AVR-GCC 3.4
//* Target MCU		: AtMega128
//*           		     oder
//*                   AtMega16 zu Testzwecken
//* 
//* -------------------------------------------------------------------------
//*
//*
//* Beschreibung
//* -------------
//*
//* Aufnahme aller String Konstanten des Projektes in dieser Datei.
//*
//* 
//* 
//*
//***************************************************************************
//
// --------------------------------------------------------------------------
// BEGIN String-Const
// --------------------------------------------------------------------------
//

#define AC_IntroUART_ProgNameVersion  "Alarm Control Vers: AC022b\r"
#define AC_IntroLCD_ProgNameVersion   "AlarmCtrl AC022b"
#define AC_IntroLCD_COPYRIGHT         "SUARK.COM,2010"

#define AC_MCU                        "ATMEGA128"
#define AC_FCPU                       "14745600"

#define AC_HwVers                     "HW02.00" // Hardwareversion
#define AC_SwVers                     "SW02.2b" // Softwareversion
#define AC_PrVers                     "Pr00.1a" // Protokollversion
#define AC_CompileDate                "201007031300"



//
// --------------------------------------------------------------------------
// BEGIN String-Const-System-Messages
// --------------------------------------------------------------------------
//


#define AC_MSGNO_UNBEKANNTER_FEHLER         0
#define AC_MSGNO_SCHARFSCHALTUNG_ERFOLGT    1
#define AC_MSGNO_VORALARM_AUSGELOEST        2
#define AC_MSGNO_ALARMIERUNG_AUSGELOEST     3
#define AC_MSGNO_AKTIVIERUNG_ABGEBROCHEN    4
#define AC_MSGNO_AKTIVIERUNG_GESTARTET      5
#define AC_MSGNO_DEAKTIVIERUNG              6
#define AC_MSGNO_SYSTEM_GESTARTET           7
#define AC_MSGNO_EEPROM_INITIALISIERT       8
#define AC_MSGNO_REMOTE_EIN                 9
#define AC_MSGNO_REMOTE_AUS                10

									
									
									



char *AC_MSG_CODE[]= {
												"UNBEKANNTER FEHLER",       // 000
												"SCHARFSCHALTUNG ERFOLGT",  // 001
												"VORALARM AUSGELOEST",      // 002
												"ALARMIERUNG AUSGELOEST",   // 003
												"AKTIVIERUNG ABGEBROCHEN",  // 004
												"AKTIVIERUNG GESTARTET",    // 005
												"DEAKTIVIERUNG",            // 006
												"SYSTEM GESTARTET",         // 007
												"EEPROM INITIALISIERT",     // 008
												"REMOTEBETRIEB EIN",        // 009
												"REMOTEBETRIEB AUS"         // 010
							 
											};



//
// --------------------------------------------------------------------------
// END OF MODUL
// --------------------------------------------------------------------------
