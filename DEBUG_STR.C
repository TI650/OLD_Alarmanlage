


//###DEBUG_ON###
  int16_t DebugCnt = 0;
//###DEBUG_OFF###

//###DEBUG_ON###
  uart_puts("DEBUG:");                      // Checksum
  s_SendIntegerToUart(DebugCnt);   
  uart_puts("\r");                      // Checksum
  DebugCnt = 0;
//###DEBUG_OFF###

//###DEBUG_ON###
  ++DebugCnt;
//###DEBUG_OFF###























//
// --------------------------------------------------------------------------
//
void s_Uart0_Read(void) {
	unsigned int zeichen;

	zeichen = uart_getc();
	if ( zeichen & UART_NO_DATA )
	{
		// no data available from UART 
	}
	else
	{

		switch (zeichen)
		{ 
		  case 's':   s_AlarmArmed(); break;  // Alarmanlage scharfschalten  

		  case 'i':   s_AlarmInert(); break;  // Deaktiviert die Scharfschaltung

		  case 'a':   s_AlarmAbort(); break;  // Deaktiviert die Sequenz zum Scharfschalten

		  case 'd':   regSendData.bSendPortDef=1; break;

  // nur zu Testzwecken		  
		  case 'h':   regSendData.bSendStatus=1; break;  // 


		  case 'W':   Int_FLASH_PORT |= (1 << Int_FLASH_PIN); break;  // Int-Flash On  
		  case 'w':   Int_FLASH_PORT &=~(1 << Int_FLASH_PIN); break;  // Int-Flash Off  

		  case 'Q':   Int_SIREN_PORT |= (1 << Int_SIREN_PIN); break;  // Int-Siren On  
		  case 'q':   Int_SIREN_PORT &=~(1 << Int_SIREN_PIN); break;  // Int-Siren Off  

		  case 'Z':   Ext_AlmSig_PORT |= (1 << Ext_AlmSig_PIN); break;  // Ext-Alarm-Signal On  
		  case 'z':   Ext_AlmSig_PORT &=~(1 << Ext_AlmSig_PIN); break;  // Ext-Alarm-Signal Off  

		  case 'T':   Ext_PWR_SIREN_PORT |= (1 << Ext_PWR_SIREN_PIN); break;  // Ext-Siren PWR On  
		  case 't':   Ext_PWR_SIREN_PORT &=~(1 << Ext_PWR_SIREN_PIN); break;  // Ext-Siren PWR Off  

		  case 'E':   Ext_PWR_FLASH_PORT |= (1 << Ext_PWR_FLASH_PIN); break;  // Ext-Flash PWR On  
		  case 'e':   Ext_PWR_FLASH_PORT &=~(1 << Ext_PWR_FLASH_PIN); break;  // Ext-Flash PWR Off  

		  case 'R':   Ext_PWR_PORT |= (1 << Ext_PWR_PIN); break;  // Ext-Power On  
		  case 'r':   Ext_PWR_PORT &=~(1 << Ext_PWR_PIN); break;  // Ext-Power Off  

		  case 'U':   Int_Buzz1_PORT |= (1 << Int_Buzz1_PIN); break;  // Int-Buzzer No1 On  
		  case 'u':   Int_Buzz1_PORT &=~(1 << Int_Buzz1_PIN); break;  // Int-Buzzer No1 Off  

		  case 'O':   Int_Buzz2_PORT |= (1 << Int_Buzz2_PIN); break;  // Int-Buzzer No2 On  
		  case 'o':   Int_Buzz2_PORT &=~(1 << Int_Buzz2_PIN); break;  // Int-Buzzer No2 Off  

		  case 'F':   regCtrlMode.bInternal = 1; break;  
		  case 'f':   regCtrlMode.bInternal = 0; break;  

		  case 'G':   regCtrlMode.bExternal = 1; break;  
		  case 'g':   regCtrlMode.bExternal = 0; break;  

		  case 'V':   regCtrlMode.bIntNoise = 1; break;  
		  case 'v':   regCtrlMode.bIntNoise = 0; break;  

		  case 'J':   regCtrlMode.bExtNoise = 1; break;  
		  case 'j':   regCtrlMode.bExtNoise = 0; break;  

		  case 'K':   regCtrlMode.bByGSM = 1; break;  
		  case 'k':   regCtrlMode.bByGSM = 0; break;  
 
 
/*			case 'P':   s_BuzzerStart_PRE(); 
									regAlarmStatus.bPreAlarm     = 1;    // Voralarm ausgelöst = 1 / 
									lcd_gotoxy(0,0);
									lcd_puts("P=1");
									Data2LCD();
									break;  

			case 'A':   s_BuzzerStart_ALM(); 
									regAlarmStatus.bAlarmExecute = 1;    // Alarm ausgelöst    = 1 /  
									lcd_gotoxy(0,0);
									lcd_puts("A=1");
									Data2LCD();
									break;  // 

			case 'S':   s_BuzzerStop(); 
									regAlarmStatus.bPreAlarm     = 0;    // Voralarm ausgelöst = 1 / 
									regAlarmStatus.bAlarmExecute = 0;    // Alarm ausgelöst    = 1 /  
									lcd_gotoxy(0,0);
									lcd_puts("S=0 ");
									Data2LCD();
									break;  // 
*/
			case '-':   
									BuzzerLevelMain++;
									BuzzerLevelMin--;  // -= BuzzerLevelMain;
									BuzzerLevelMax--;  // -= BuzzerLevelMain;		
//									Data2LCD();
									break;  // 

			case '+':   
									BuzzerLevelMain--;
									BuzzerLevelMin++;
									BuzzerLevelMax++;		
//									Data2LCD();
									break;  // 

		} 

			s_Cmd_PutChar2Buffer(zeichen);


//		uart_putc( (unsigned char)zeichen );

	}

}
//
// --------------------------------------------------------------------------
//