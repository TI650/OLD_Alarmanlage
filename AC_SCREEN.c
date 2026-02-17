
//
// --------------------------------------------------------------------------
//


#define EXTRA_MENU 




// --------------------------------------------------------------------------
// SUB-Routines and Functions
// --------------------------------------------------------------------------
//
void IntToStringformat(int value, char zero, char sign) {
    char buffer[6];
		char i;
		
		CharBuffer[0] = '\0';
		
		itoa( value, buffer, 10);   // convert interger into string (decimal format)         
		if ((value > 0) && (sign > 0)) strcat(CharBuffer,"+");  

		if (zero > 0) 
		{
		
		  for(i = strlen(buffer); i <= zero-1; i++)
      {
        strcat(CharBuffer,"0");
      }
		
		}
    strcat(CharBuffer,buffer);
}
//
// --------------------------------------------------------------------------
//
void s_Menu_MainData(void)
{                             //1234567890123456
//  static unsigned char index = 0, blink = 0, count = 0;
  static unsigned char index = 0;
  static unsigned char blink = 0;
//  static unsigned int BufUInt = 0;
 

  if (regMenueCtrl.bRefreshDisplay)
  { 
		LED_ModeHand_PORT &= ~(1 << LED_ModeHand_PNO);              /* Pin low     */		
		LED_Armed_PORT &= ~(1 << LED_Armed_PNO);              /* Pin low     */		
		LED03x_PORT &= ~(1 << LED03x_PNO);              /* Pin low     */		

	
		lcd_clrscr();
		lcd_gotoxy(0,0);
		lcd_puts (Day[sDTG.Weekday]);
		lcd_putc (' ');

		IntToStringformat(sDTG.Day, 2, 0);
		lcd_puts(CharBuffer);
		lcd_putc ('.');

		IntToStringformat(sDTG.Month, 2, 0);
		lcd_puts(CharBuffer);
		lcd_putc (' ');    

		lcd_putc (' ');    
		//lcd_puts (ModeSign[regUserControl.bSwitchRemote]);

    lcd_putc (' ');

		IntToStringformat(sDTG.Hour, 2, 0);
		lcd_puts(CharBuffer);
		lcd_putc (':');
		
		IntToStringformat(sDTG.Min, 2, 0);
		lcd_puts(CharBuffer);

		lcd_gotoxy(0,1);
		lcd_puts ("MODE: ");
		if (regAlarmStatus.bAlarmArmed)
		{
  		lcd_puts ("SCHARF");
			LED_Armed_PORT |=  (1 << LED_Armed_PNO);        /* Pin high    */
		}
		else
		{
			if (regAlarmStatus.bCountdown) 			{ lcd_puts ("SCHAERFEN"); }
			else                                {	lcd_puts ("UNSCHARF"); 	}
		}
		
		lcd_gotoxy(6,1);
		if (regAlarmStatus.bPreAlarm)         {	lcd_puts ("VORALARM  ");}
		
		if (regAlarmStatus.bAlarmExecute)
		{
  		lcd_puts ("AUSGELOEST");
			if (blink) LED_Armed_PORT |=  (1 << LED_Armed_PNO);        /* Pin high    */
			else LED_Armed_PORT &= ~(1 << LED_Armed_PNO);              /* Pin low     */ 
		}
    
		if (regCtrlMode.bRemote)
		{
			lcd_gotoxy(0,2);
			lcd_puts ("Bedienung: FERN");
			LED_ModeHand_PORT |=  (1 << LED_ModeHand_PNO);        /* Pin high    */
		}
		else
		{
			lcd_gotoxy(0,2);
			lcd_puts ("Bedienung: HAND");
			LED_ModeHand_PORT &= ~(1 << LED_ModeHand_PNO);              /* Pin low     */ 
		}
    
		if (regAlarmStatus.bCountdown)
		{
			lcd_gotoxy(0,3);
			lcd_puts ("COUNTDOWN: ");
			IntToStringformat(vCountdown, 3, 0);
			lcd_puts(CharBuffer);
			lcd_puts (" sec");
			if (blink) LED_Armed_PORT |=  (1 << LED_Armed_PNO);        /* Pin high    */
		}

    lcd_gotoxy(13 ,0);  
    if (blink) 
    { blink = 0; 
      lcd_putc(':'); 
    } 
    else 
    { blink = 1; 
      lcd_putc(' '); 
    } 













    regMenueCtrl.bRefreshDisplay = 0; 
  } 

  if (index == 0 && (TASTE == UP || TASTE == DOWN)) { MenuCount = 255; regMenueCtrl.bMenuPolling = 1;}



}
//
// --------------------------------------------------------------------------
//

//
// --------------------------------------------------------------------------
//

//
// --------------------------------------------------------------------------
//

//
// --------------------------------------------------------------------------
//

//
// --------------------------------------------------------------------------
//

//
// --------------------------------------------------------------------------
//
void s_Menu(void)
{                   
 // if (Taste == MENU) { MenuCount++; regMenueCtrl.bMenuPolling = 1; }
  s_Menu_MainData();  
  
  /*
	switch (MenuCount)
  {
    case 0:    s_Menu_MainData();
      break;
  
    case 1:    s_Menu_One();//MenuTimer();
      break;

    case 2:    s_Menu_Two();//MenuSchicht();
      break;

    case 3:    s_Menu_Three();//MenuTemperatur();
      break;
       
    default:
      #ifdef EXTRA_MENU   
      s_MenuAuswahl();
      #else
      MenuCount = 0;
      #endif     
      break;
  }
	*/
  
}
//
// --------------------------------------------------------------------------
//
