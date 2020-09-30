#include "cursor.h"

Cursor_TypeDef Cursor_Structure;

uint8_t Cursor_Line[8]={0};
uint8_t Cursor_Page;

void Cursor_InitStruct(Cursor_TypeDef* Cursor_Structure)
{
	Cursor_Structure->Shape				=	"<-";
	Cursor_Structure->ShapeSize		= 2;
	Cursor_Structure->PerPage_Num	= Arrange_Cursor(0xA8);
	Cursor_Structure->Total_Num		= 6;
	Cursor_Structure->Line			= Cursor_Line;
	Cursor_Structure->Pos				= 80;
}

void Cursor_Scan(Cursor_TypeDef* Cursor_Structure)
{
	uint8_t Lastline,Firstline;
	CursorShift_Type Direction;
	
	Firstline	=	Find_Firstline_Cursor();
	Lastline	=	Find_Lastline_Cursor();
	
	/* �û������»���ҡ�� */
	if(JoyStick_Data[Y]<100)
	{
		Direction=lsh;
		if(Menu_Temp < Cursor_Structure->Total_Num)
		{
			Menu_Temp++;
			/* ��ǰ�α�δָ�����һ�в˵� �������α� */
			if(Cursor_Page<Lastline)
			{
				/* �����ǰ�α� */
				OLED_ClearPageArea(Cursor_Page,Cursor_Structure->Pos,Cursor_Structure->Pos+6*Cursor_Structure->ShapeSize);			
				/* Ѱ����һ�˵�λ�� ����:���� */
				Cursor_Page = Find_Next_Cursor_Line(Direction);
			}
			else
				Menu_Start_Line=Menu_Temp-Cursor_Structure->PerPage_Num;
		}
	}
	/* �û������ϻ���ҡ�� */
	else if(JoyStick_Data[Y]>4000)
	{
		Direction=hsh;
		/* ��ǰ�α�δָ���һ�в˵� */
		if(Menu_Temp > 1)
		{
			Menu_Temp--;
			if(Cursor_Page>Firstline)
			{
				/* �����ǰ�α� */
				OLED_ClearPageArea(Cursor_Page,Cursor_Structure->Pos,Cursor_Structure->Pos+6*Cursor_Structure->ShapeSize);			
				/* Ѱ����һ�˵�λ�� ����:���� */
				Cursor_Page = Find_Next_Cursor_Line(Direction);
			}
			else
				Menu_Start_Line=Menu_Temp-Cursor_Structure->PerPage_Num;
		}
	}
	if(Menu_Display)
	{
		/* ��ӡ�µ��α� */
		OLED_Print(Cursor_Page,Cursor_Structure->Pos,(uint8_t*)Cursor_Structure->Shape);

#if OLED_PRINT_MENU_TEMP	
		OLED_Print(0,0,"Menu_Temp=");
		OLED_Display6X8Number(0,6*strlen("Menu_Temp="),Menu_Temp);
#endif	
	
#if OLED_PRINT_MENU_INDEX
		OLED_Print(1,0,"Menu_Index=");
		OLED_Display6X8Number(1,6*strlen("Menu_Index="),Menu_Index);
#endif
		
#if OLED_PRINT_MENU_LEVEL
		OLED_Print(1,0,"Menu_Level=");
		OLED_Display6X8Number(1,6*strlen("Menu_Level="),Menu_Level);
#endif
		
		Dsp_Menu(Cursor_Structure->PerPage_Num,Cursor_Line,Menu_Start_Line,Menu_TextBuf);
		delay_ms(100);//��ָҡ��ҡ�˵�ʱ���Լ100ms������̫��
	}
}

static uint8_t Find_Lastline_Cursor(void)
{
	uint8_t i;
	uint8_t result=0;
	for(i=0;i<=7;i++)
	{
		if(Cursor_Line[i])
			result=i;
	}
	return result;
}

static uint8_t Find_Firstline_Cursor(void)
{
	uint8_t i;
	uint8_t result=0;
	for(i=8;i>0;i--)
	{
		if(Cursor_Line[i-1])
			result=i-1;
	}
	return result;
}

static uint8_t Find_Next_Cursor_Line(CursorShift_Type ddr)
{
	uint8_t i=0,Next_CursorLine;
	if(ddr == hsh)//����
	{
		do
		{
			i++;
		}while(!Cursor_Line[Cursor_Page-i]);//�ҵ���һ��������Ϊֹ
		Next_CursorLine = Cursor_Page-i;
	}
	else					//����
	{
		do
		{
			i++;
		}while(!Cursor_Line[Cursor_Page+i]);//�ҵ���һ��������Ϊֹ
		Next_CursorLine = Cursor_Page+i;
	}
	return Next_CursorLine;
}

uint8_t Arrange_Cursor(uint8_t MSB_Data)
{
	uint8_t i,j=0;
	for(i=8;i>0;i--)
	{
		Cursor_Line[i-1] = (MSB_Data&0x80)>>7;
		if(Cursor_Line[i-1])
			j++;
		MSB_Data<<=1;
	}
	return j;
}



