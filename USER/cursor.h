#include "headfile.h"
#ifndef _cursor_H
#define _cursor_H

typedef struct
{
	char*  Shape;				//游标形状  比如："<-" "->" "*" "<<"
	uint8_t 			ShapeSize;		//形状大小
	uint8_t 			PerPage_Num;	//每一页需要显示的菜单数量
	uint8_t 			Total_Num;		//总共有多少菜单
	uint8_t*			Line;					//哪几行显示菜单？ 对应行数置一
	uint8_t				Pos;					//游标位置	靠左？ 靠右？
}Cursor_TypeDef;

typedef enum
{
	hsh,
	lsh
}CursorShift_Type;

extern Cursor_TypeDef Cursor_Structure;
extern uint8_t Cursor_Page;
extern uint8_t  Menu_TextBuf[3][31];
extern uint8_t Cursor_Line[8];
void Cursor_InitStruct(Cursor_TypeDef* Cursor_Structure);
void Cursor_Hsh(Cursor_TypeDef* Cursor_Structure);
void Cursor_Lsh(Cursor_TypeDef* Cursor_Structure);
void Cursor_Scan(Cursor_TypeDef* Cursor_Structure);
static uint8_t Find_Firstline_Cursor(void);
static uint8_t Find_Lastline_Cursor(void);
static uint8_t Find_Next_Cursor_Line(CursorShift_Type ddr);
void Button_Sound(void);
uint8_t Arrange_Cursor(uint8_t MSB_Data);

#endif

