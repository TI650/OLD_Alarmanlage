
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

		lcd_clrscr();
		lcd_gotoxy(1,0);
		lcd_puts (Day[sDTG.Weekday]);
		lcd_putc (' ');

		IntToStringformat(sDTG.Day, 2, 0);
		lcd_puts(CharBuffer);
		lcd_putc ('.');

		IntToStringformat(sDTG.Month, 2, 0);
		lcd_puts(CharBuffer);
		lcd_puts (" ");    //20");

		if (regUserControl.bSwitchRemote == 0)  {lcd_puts("H");}
		else  {lcd_puts("F");}

//		IntToStringformat(sDTG.Year, 2, 0);
//		lcd_puts(CharBuffer);
		lcd_putc (' ');

//		lcd_gotoxy(4,1);
		IntToStringformat(sDTG.Hour, 2, 0);
		lcd_puts(CharBuffer);
		lcd_putc (':');
		
		IntToStringformat(sDTG.Min, 2, 0);
		lcd_puts(CharBuffer);
//		lcd_putc (':');
//		
//		IntToStringformat(sDTG.Sec, 2, 0);
//		lcd_puts(CharBuffer);

      


    lcd_gotoxy(12 ,0);  
    if (blink) 
    { blink = 0; 
      lcd_putc(':'); 
    } 
    else 
    { blink = 1; 
      lcd_putc(' '); 
    } 
    
		lcd_gotoxy(8 ,1);  
		lcd_puts("H"); 		
		IntToStringformat(regUserControl.bSwitchRemote, 2, 0);
		lcd_puts(CharBuffer);

		

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
void s_Menu_One(void)
{                             //1234567890123456
  static unsigned char index = 0;
  static unsigned char blink = 0;

  if (regMenueCtrl.bRefreshDisplay)
  { 

		lcd_clrscr();
		lcd_gotoxy(1,0);
		lcd_puts ("Menue Eins");

    lcd_gotoxy(12 ,1);  
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
void s_Menu_Two(void)
{                             //1234567890123456
  static unsigned char index = 0;
  static unsigned char blink = 0;

  if (regMenueCtrl.bRefreshDisplay)
  { 

		lcd_clrscr();
		lcd_gotoxy(1,0);
		lcd_puts ("Menue Zwei");

    lcd_gotoxy(12 ,2);  
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
void s_Menu_Three(void)
{                             //1234567890123456
  static unsigned char index = 0;
  static unsigned char blink = 0;

  if (regMenueCtrl.bRefreshDisplay)
  { 

		lcd_clrscr();
		lcd_gotoxy(1,0);
		lcd_puts ("Menue Drei");

    lcd_gotoxy(12 ,3);  
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
#ifdef EXTRA_MENU
void s_MenuAuswahl(void)
{
  static unsigned char index = 0;
  
   if (regMenueCtrl.bRefreshDisplay)
   {  
      lcd_gotoxy(0,1); lcd_puts("                 ");
      lcd_gotoxy(0,0); 
      switch(index) 
      { case 0: 
                lcd_puts("Status           ");      
              break;
       
        case 1: 
                lcd_puts("Timer - Setup    ");
              break;
      
        case 2: 
                lcd_puts("Schalt - Gruppe  ");
              break;
                
        case 3: 
                lcd_puts("Kennlinien       ");
              break;
      } 
      regMenueCtrl.bRefreshDisplay = 0;  
   }                                        
  

  
  
  if (TASTE == UP) 
	{
	  if (index < 3) index++; 
		else index = 0;
	}
  else 
	{
	  if (TASTE == DOWN) 
		{
		  if (index > 0) index--;
		} 
		else index = 3;
	}

  if (TASTE == RETURN)
  {
    MenuCount = index;
    regMenueCtrl.bMenuPolling = 1;
  } 
  
}
#endif
// ---------------------------------------------------------------------------------------


//
// --------------------------------------------------------------------------
//
void s_Menu(void)
{                   
 // if (Taste == MENU) { MenuCount++; regMenueCtrl.bMenuPolling = 1; }
  
  
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
  
}
//
// --------------------------------------------------------------------------
//
