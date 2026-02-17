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
      
            delay_ms(40);
           tmp_mode = ErmittleBetriebsMode(AktuellZeit);
           if (tmp_mode != DEAKTIV) BetriebsMode = tmp_mode;
           
           Menu();
           GetTemperatur(0);
           GetRegelpunkt();
           SteuerRelais();

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
            { 
            }
            
           if (SendeSysID)
            {
            }                          

           if (SendeVersSW)
            {
            } 

           if (SendeVersPF)
            {
            }                          

           if (SendeDisplay)
            {                          
            }
            
           if (SendeDaten)
            {                          
              
            }
            
           if (SendeLogDat)
            {                          
            }

           if (SendeLogDatMOD)
            {                          
            }
 
           if (SendeDataSet)
            {                          

            }
   
      };
}