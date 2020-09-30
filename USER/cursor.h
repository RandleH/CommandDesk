#include "headfile.h"
#ifndef _cursor_H
#define _cursor_H

typedef struct
{
	char*  Shape;				//�α���״  ���磺"<-" "->" "*" "<<"
	uint8_t 			ShapeSize;		//��״��С
	uint8_t 			PerPage_Num;	//ÿһҳ��Ҫ��ʾ�Ĳ˵�����
	uint8_t 			Total_Num;		//�ܹ��ж��ٲ˵�
	uint8_t*			Line;					//�ļ�����ʾ�˵��� ��Ӧ������һ
	uint8_t				Pos;					//�α�λ��	���� ���ң�
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

