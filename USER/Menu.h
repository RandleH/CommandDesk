#include "headfile.h"
#ifndef _menu_H
#define _menu_H

extern uint32_t 	Menu_Index;
extern uint8_t	 	Menu_Temp;
extern uint8_t  	Menu_Level;
extern bool		 		Menu_Need_Refresh;
extern bool   	 	Menu_Display;
extern uint8_t  	Menu_Start_Line;
void Set_Menu(uint8_t row,const char* text);
void Dsp_Menu(uint8_t PerPage_Num,uint8_t* Position,int8_t Text_Start,uint8_t Text[][31]);
#endif



