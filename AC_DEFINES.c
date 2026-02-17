

// --------------------------------------------------------------------------
// DEFINES
// --------------------------------------------------------------------------

#define NICHTS   0
#define MENU     1
#define UP       3
#define DOWN     4
#define RETURN   5
#define TASTE1   6
#define TASTE2   7
#define TASTE3   8 
#define TASTE4   9 
#define TASTE5  10 


#define DEFAULT                  255
#define Index_Menue_Auswahl01      1
#define Index_Menue_DateTime       2
#define Index_Menue_Overview     DEFAULT

#define Index_Menue_INSTALL       200


#define DataSeperator ','
#define cPortsMax     8



// --------------------------------------------------------------------------
// SW-Const
// --------------------------------------------------------------------------

//volatile uint8_t   menu_actual   = 0;
//volatile uint8_t   menu_befor01  = 0;
//volatile uint8_t   menu_befor02  = 0;

unsigned char TASTE     = 0;
unsigned char MenuCount = 255;

//unsigned char MenuMainIndex = DEFAULT;

char CharBuffer[22];
int  IntBuffer;

unsigned char vCountdown = 255;
//#define cCountdown 10


char *Day[] = {"Mo","Di","Mi","Do","Fr","Sa","So"};
char *OnOff[] = {"AUS","EIN"};
//char *Mode[] = {"FERNBETRIEB","HANDBETRIEB"};
//char *ModeSign[] = {"F","H"};


///char *HMenues[]= {"Datum/Uhrzeit","H-MENUE-01","H-MENUE-02","H-MENUE-03","H-MENUE-04","H-MENUE-05","H-MENUE-06","H-MENUE-07"};

// --------------------------------------------------------------------------
// SW-Variables
// --------------------------------------------------------------------------

  struct {
    unsigned char b01000ms    :1;    // 1 Bit für  1000 millisekunden
    unsigned char b05000ms    :1;    // 1 Bit für  5000 millisekunden  
    unsigned char b00500ms    :1;    // 1 Bit für   200 millisekunden 
    unsigned char b00100ms    :1;    // 1 Bit für   100 millisekunden      
    unsigned char bunused4    :1;    // 1 Bit für      
    unsigned char bunused5    :1;    // 1 Bit für      
    unsigned char bunused6    :1;    // 1 Bit für      
    unsigned char bunused7    :1;    // 1 Bit für      
    //unsigned char bNochNBit:1; // Und hier noch mal ein Bit
    //unsigned char b2Bits:2; // Dieses Feld ist 2 Bits breit
    // All das hat in einer einzigen Byte-Variable Platz.
    // die 3 verbleibenden Bits bleiben ungenutzt
  } regPollingTime;

//  struct {
//    unsigned char bunused0   :1;     
//    unsigned char bunused1   :1;     
//    unsigned char bunused2   :1;     
//    unsigned char bTast1     :1;     
//    unsigned char bTast2     :1;     
//    unsigned char bTast3     :1;     
//    unsigned char bTast4     :1;     
//    unsigned char bTast5     :1;     
//  } Taster;


  // Register für Steueraktivitäten des Nutzers
  struct {
    unsigned char bSwitchRemote  :1;    // HW Steuerung Fern = 1 / Hand = 0    
    unsigned char bSwitchArmed   :1;    // HW Alarmanlage scharf = 1 / unscharf = 0      
    //  unsigned char bunused2   :1;     
    //  unsigned char bunused3   :1;     
    //  unsigned char bTast1     :1;     
    //  unsigned char bTast2     :1;     
    //  unsigned char bTast3     :1;     
    //  unsigned char bTast4     :1;     
  } regUserControl;

  struct {
    //  unsigned char bunused0 :1;     
    unsigned char bRefreshDTG     :1;     
    //  unsigned char bRefreshMenu    :1;     
    unsigned char bRefreshDisplay  :1;     
    unsigned char bStopClock      :1;     
    //  unsigned char bTRight          :1;     
    //  unsigned char bWheelUp         :1;     
    unsigned char bMenuPolling     :1;     
  } regMenueCtrl; //BStatus;

  struct {
    unsigned char Weekday;     
    unsigned char Day;     
    unsigned char Month;     
    unsigned char Year;     
    unsigned char Hour;     
    unsigned char Min;     
    unsigned char Sec;     
  } sDTG;

  // Register zum akivieren des Sendens von verschiedenen Meldungen
  struct {  
    //  unsigned char bunused0       :1;    //  
    //  unsigned char bunused1       :1;    //  
    unsigned char bSendStatus    :1;    // Gesamtstatus
    unsigned char bSendPortDef   :1;     
    //  unsigned char bunused4       :1;     
    //  unsigned char bunused5       :1;     
    //  unsigned char bunused6       :1;     
    unsigned char bSendStatKL    :1;     
  } regSendData;

  // Register für Statusinformationen zum Alarmmode
  struct {  
    unsigned char bAlarmArmed     :1;    // Alarmanlage scharf = 1 / unscharf = 0 
    unsigned char bPreAlarm       :1;    // Voralarm ausgelöst = 1 / 
    unsigned char bAlarmExecute   :1;    // Alarm ausgelöst    = 1 /  
    //  unsigned char bunused3        :1;     
    //  unsigned char bunused4        :1;     
    //  unsigned char bunused5        :1;     
    unsigned char bCountdown      :1;    // Countdown läuft    = 1 / gestoppt = 0
  } regAlarmStatus;

  // Register für Controlmodes
  struct {  
    unsigned char bRemote         :1;    // Remotebetrieb         Fern = 1 / Hand = 0 
    unsigned char bInternal       :1;    // Alarmierung Intern    Ein  = 1 / Aus  = 0 
    unsigned char bExternal       :1;    // Alarmierung Extern    Ein  = 1 / Aus  = 0 
    unsigned char bIntNoise       :1;    // Alarmierung Akustisch Ein  = 1 / Aus  = 0      
    unsigned char bExtNoise       :1;    // Alarmierung Akustisch Ein  = 1 / Aus  = 0      
    unsigned char bByGSM          :1;    // Alarmierung über GSM  Ein  = 1 / Aus  = 0      
    //  unsigned char bunused2        :1;     
    //  unsigned char bunused3        :1;     
    //  unsigned char bunused4        :1;     
    //  unsigned char bunused5        :1;     
    //  unsigned char bunused6        :1;     
    //  unsigned char bunused7        :1;     
  } regCtrlMode;

/*  struct {
    unsigned char bMenuMain      :1;     
    unsigned char bDispBackLight    :1;     
    unsigned char bunused2  :1;     
    unsigned char bunused3   :1;     
    unsigned char bunused4 :1;     
    unsigned char bunused5 :1;     
    unsigned char bunused6:1;     
    unsigned char bunused7:1;     
  } TimeOut;
	
  char *DayStr[]= {"Mo","Di","Mi","Do","Fr","Sa","So"};
*/
 



  #define ADC_TA       1     // Sensor Temp-Aussen             Pos:  Aussen am Kellerniedergang
  #define ADC_TI       2     // Sensor Temp-Warmwasser         Pos:  Tauchhuelse Warmwasserspeicher



	volatile	unsigned char InPortVal[] = {0,0,0,0,0,0,0,0};     //  
	volatile	unsigned char InPortAdr[] = {0x40,0x41,0x42,0x43,0,0,0,0};     //  
	volatile	unsigned char regPortActual[] = {0,0,0,0,0,0,0,0};     //  
	volatile	unsigned char regPortStored[] = {0,0,0,0,0,0,0,0};     //  
	volatile	unsigned char InPortRootingTable[64];
	int16_t  SensorKorrWert[4];              // Sensor-Korrekturwerte
	int16_t  SensorIstWert[4];               // Sensor-Istwerte
	
//	int16_t  IstTempAussen;       // Sensor Temp-Aussen             Pos:  Aussen am Kellerniedergang
//  int16_t  IstTempInnen;        // Sensor Temp-Warmwasser         Pos:  Tauchhuelse Warmwasserspeicher
//	int16_t  IstTemp_CH02;        // Sensor Kanal 02
//  int16_t  IstTemp_CH03;        // Sensor Kanal 03
//	int16_t  SensorKorrWert_TA;   // Sensor Temp-Aussen             Pos:  Aussen am Kellerniedergang
//  int16_t  SensorKorrWert_TI;   // Sensor Temp-Warmwasser         Pos:  Tauchhuelse Warmwasserspeicher
//	int16_t  SensorKorrWert_CH02; // Sensor Kanal 02
//  int16_t  SensorKorrWert_CH03; // Sensor Kanal 03




// --------------------------------------------------------------------------
// EEPROM - Definitions, Const and Variables
// --------------------------------------------------------------------------

#define EEPROM_DEFAULT                0xFF
#define EEPROM_DEFAULT_CHECKBYTEVALUE 0xAA
#define EEPROM_DEFAULT_COUNTDOWN        10 // 60 // 
#define EEPROM_DEFAULT_INPORTCNT         8
#define EEPROM_DEFAULT_INPORTADR0     0x40
#define EEPROM_DEFAULT_INPORTADR1     0x42
#define EEPROM_DEFAULT_INPORTADR2     0x44
#define EEPROM_DEFAULT_INPORTADR3     0x46
#define EEPROM_DEFAULT_INPORTADR4     0x48
#define EEPROM_DEFAULT_INPORTADR5     0x4A
#define EEPROM_DEFAULT_INPORTADR6     0x4C
#define EEPROM_DEFAULT_INPORTADR7     0x4E


// EEMEM wird bei aktuellen Versionen der avr-lib in eeprom.h definiert
// hier: definiere falls noch nicht bekannt ("alte" avr-libc)
#ifndef EEMEM
// alle Textstellen EEMEM im Quellcode durch __attribute__ ... ersetzen
#define EEMEM  __attribute__ ((section (".eeprom")))
#endif
//this global variables are stored in EEPROM
  uint16_t  eedummy          EEMEM  = 0 ;       // avoid using lowest addresses
  uint8_t   eeCheckByte      EEMEM  = EEPROM_DEFAULT_CHECKBYTEVALUE;       // 
  uint8_t   eeCountdown      EEMEM  = EEPROM_DEFAULT_COUNTDOWN;       // Dauer des Countdowns
  uint8_t   eeInPortCnt      EEMEM  = EEPROM_DEFAULT_INPORTCNT;       // Anzahl der Ports (Anz Meldeschleifen = Ports mal 7)
  uint8_t   eeInPortAdr[8]   EEMEM ;// = {7,6,5,4,3,2,1,0};       // Portadress PCF8xxx
  uint8_t   eeInPortVal[8]   EEMEM ;// = {7,6,5,4,3,2,1,0};       // Portadress PCF8xxx

  uint8_t   eeInPortRootingTable[64]   EEMEM ;//    

  uint16_t  eeSensorKorrWert[4]        EEMEM; //  
//  uint16_t  eeSensorKorrWert_TAx      EEMEM  = -30; //  
//  uint16_t  eeSensorKorrWert_TI      EEMEM  = -30; //   
//  uint16_t  eeSensorKorrWert_CH02    EEMEM  = -30; //  
//  uint16_t  eeSensorKorrWert_CH03    EEMEM  = -30; //   

  uint8_t   eePreAlarm_BuzzerTast       EEMEM  =   2;
  uint8_t   eePreAlarm_BuzzerLevelMain  EEMEM  = 100;
  uint8_t   eePreAlarm_BuzzerLevelMin   EEMEM  = 255;
  uint8_t   eePreAlarm_BuzzerLevelMax   EEMEM  = 255;
  uint8_t   eePreAlarm_BuzzerIntervall  EEMEM  = 100;

  uint8_t   eeFullAlarm_BuzzerTast      EEMEM  =   5;
  uint8_t   eeFullAlarm_BuzzerLevelMain EEMEM  =  10; 
  uint8_t   eeFullAlarm_BuzzerLevelMin  EEMEM  = 230; 
  uint8_t   eeFullAlarm_BuzzerLevelMax  EEMEM  = 255;
  uint8_t   eeFullAlarm_BuzzerIntervall EEMEM  = 100;

  uint8_t   eeBuzz1_Cnt_Time            EEMEM  =   2;
  uint8_t   eeInt_Siren_Cnt_Time        EEMEM  =  60;
  uint8_t   eeSlaveAddress              EEMEM  =  62;



  #define cErrMemoryMax     16
  uint8_t   eeErrMemCounter      EEMEM  =   0;//    
  uint8_t   eeErrMem_No[cErrMemoryMax]     EEMEM;//    
  uint8_t   eeErrMem_Day[cErrMemoryMax]    EEMEM;//    
  uint8_t   eeErrMem_Month[cErrMemoryMax]  EEMEM;//    
  uint8_t   eeErrMem_Hour[cErrMemoryMax]   EEMEM;//    
  uint8_t   eeErrMem_Min[cErrMemoryMax]    EEMEM;//    
  uint8_t   eeErrMem_Sec[cErrMemoryMax]    EEMEM;//    
  uint8_t   eeErrMem_Val1[cErrMemoryMax]   EEMEM;//    
  uint8_t   eeErrMem_Val2[cErrMemoryMax]   EEMEM;//    
  uint8_t   eeErrMem_Val3[cErrMemoryMax]   EEMEM;//    

 



//uint8_t   eeprom_var1      EEPROM_SECTION  = 1;       // EEPROM address 0002
//uint8_t   eeprom_var2      EEPROM_SECTION  = 2;       // EEPROM address 0003
//uint16_t  eeprom_var3      EEPROM_SECTION  = 1027;    // low byte = 0003, high = 0004  
//float     eeprom_var4      EEPROM_SECTION  = 1.3456;  // four byte float 
//char     *eeprom_str01     EEPROM_SECTION ="0123456789ABCDEF";
//char      eeprom_str01[17]     EEPROM_SECTION = "1234567890123456\0";



