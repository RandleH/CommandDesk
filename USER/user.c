#include "user.h"
#include "OLED_Image.h"



void System_Init(void)
{
	uint8_t event=10;//初始化函数的个数
	delay_init(72);
#if OLED_I2C||OLED_SPI
	OLED_Init();
#endif
	while(event--)
	{
		switch(event)//每个初始化函数单独放一个case
		{
#if JoyStick
			case 5: JoyStick_Init();			Dsp_StarDust();break;
#endif			
			
#if LCD1602				
			case 4:	LCD1602_Init();				Dsp_StarDust();break;
#endif	
			
#if TFT18_SPI
			case 3: TFT_Init();						Dsp_StarDust();break;
#endif
			
#if NRF24L01
			case 2:	NRF24L01_Init(NRF_TX_MODE);Dsp_StarDust();break;
#endif
			
#if LED_PC13
			case 1:	LED_Init();						Dsp_StarDust();break;
#endif						
							
	
			default:											Dsp_StarDust();break;
		}
	}
#if OLED_I2C||OLED_SPI
	OLED_Clean();
#endif
}

void Dsp_StarDust(void)
{
#if OLED_I2C||OLED_SPI
	static uint8_t j=0;
	if(!j)//首次进入将显示欢迎图片
	{
		OLED_DisplayImage(StarDust);
		OLED_Display6X8Str(5,0,"System initialization");
		OLED_Display6X8Str(6,20,"please wait");
		j++;
	}
	OLED_Display6X8Char(6,80+6*j,'.');
	delay_ms(20);
	j++;
	if(j>3)
	{
		OLED_Display6X8Str(6,86,"   ");
		j=1;
		delay_ms(80);
	}
#endif
}

float map(float val, float I_Min, float I_Max, float O_Min, float O_Max)
{
	if(	  I_Max<I_Min
      ||O_Max<O_Min
      ||val<I_Min
      ||val>I_Max	)
    return val;
  return(O_Min+(O_Max-O_Min)*(val-I_Min)/(I_Max-I_Min));
}

uint8_t CNT_Bits(uint32_t number)
{
	uint8_t CNT=0;
	while(number!=0)
	{
		number/=10;
		CNT++;
	}
	if(CNT==0)
		CNT=1;
	return CNT;
}







