#include "Menu.h"

/*********************************************/
uint32_t Menu_Index;	//To Record the Menu Data
uint8_t	 Menu_Temp;		//To Record the Temp Data
uint8_t  Menu_Level;
uint8_t  Menu_TextBuf[3][31];
bool		 Menu_Need_Refresh	= true;
bool   	 Menu_Display				= true;
uint8_t  Menu_Start_Line  	= 0;
/*********************************************/


