#include "headfile.h"


void EXTI9_5_IRQHandler(void)
{
	if(EXTI_GetITStatus(EXTI_Line7)!=RESET)
	{
		switch(CMD_State)
		{
/*======================================================================*/	
case 0x00000000://Command Desk shows welcome
{
	//In this case, user is going to select a project.
	CMD_LastState = CMD_State;
	CMD_BOX.PRJ_Index = (PRJ_Type)CMD_Temp;
	CMD_State |= (++CMD_Temp)<<(7*4);
	break;
}
case 0x10000000://Porject: H SmartCar
case 0x20000000://Project: C SmartCar
case 0x30000000://Project: CHIP Dector
{
	//In this case, user is going to select the COM Device.
	CMD_LastState = CMD_State;
	if(CMD_Temp!=Info.BackIndex)
	{
		if(CMD_Temp == 0)	{CMD_COM_Device.NRF = Enable;
											 CMD_COM_Device.BLT = Disable;}
		if(CMD_Temp == 1)	{CMD_COM_Device.NRF = Disable;
											 CMD_COM_Device.BLT = Enable;}
		CMD_State &= ~(1<<6*4);
		CMD_State |= (++CMD_Temp)<<(6*4);
	}
	else
		CMD_State = 0x00000000;
	break;
}
/*==========================¶þ¼¶×´Ì¬===========================*/
case 0x21000000:// C_SmartCar -> NRF		
case 0x22000000:// C_SmartCar -> BLT
{
	//In this case,user is going to select a function.
	CMD_LastState = CMD_State;
	if(CMD_Temp!=4)
	{
		//CMD_BOX.Project_C_SmartCar.Mode = (C_SmartCar_ModeType)CMD_Temp;
		CMD_State &= ~(1<<(5*4));
		CMD_State |= (++CMD_Temp)<<(5*4);
	}
	else
		CMD_State &= 0xF0FFFFFF;
	break;
}
case 0x31000000:// QuadRotor  -> NRF
case 0x32000000:// QuadRotor  -> BLT
{
	//In this case,the user is going to test modules.
	CMD_LastState = CMD_State;
	if(CMD_Temp != Info.BackIndex){
		CMD_State &= ~(1<<(5*4));
		CMD_State |= (++CMD_Temp)<<(5*4);
	}else
		CMD_State &= 0xf0ffffff;
	break;
}
/*==========================Èý¼¶×´Ì¬===========================*/
case 0x21100000:// C_SmartCar -> NRF -> TEST MODULE
case 0x22100000:// C_SmartCar -> BLT -> TEST MODULE	
{
	//In this case, user is going to select a module to test.
	CMD_LastState = CMD_State;
	if(CMD_Temp!=Module_Num)
	{
		CMD_BOX.Project_C_SmartCar.CMD_Buf[1] = (MODULE_ADDR)CMD_Temp;
		CMD_State &= ~(1<<(4*4));
		CMD_State |= (++CMD_Temp)<<(4*4);
	}
	else
		CMD_State &= 0xFF0FFFFF; 
	break;
}

case 0x21300000:// C_SmartCar -> NRF -> MANAUL_MODE
case 0x22300000:// C_SmartCar -> BLT -> MANAUL_MODE
{
	CMD_LastState = CMD_State;
	if(CMD_Temp == Info.TEXT_Num-1)
	{
		CMD_State &= 0xFF0FFFFF;
	}
	else
	{
		CMD_State |= (++CMD_Temp)<<(4*4);
	}
	break;
}
case 0x21400000:// C_SmartCar -> NRF -> ALGORITHM
case 0x22400000:// C_SmartCar -> BLT -> ALGORITHM
{
	//In this case, user is going to select a module to test.
	CMD_LastState = CMD_State;
	if(CMD_Temp==1)//back
	{
		CMD_State &= 0xFF0FFFFF;
	}
	else 
	{
		CMD_State &= ~(1<<(4*4));
		CMD_State |= (++CMD_Temp)<<(4*4); 
	}
	break;
}
case 0x31100000:// QuadRotor  -> NRF -> Motor
case 0x32100000:// QuadRotor  -> BLT -> Motor
{
	//In this case, the user is going to test one of motors
	CMD_LastState = CMD_State;
	if(CMD_Temp==Info.BackIndex){
		CMD_State &= 0xFF0FFFFF;
	}
	else{
		CMD_State &= ~(1<<(4*4));
		CMD_State |= (++CMD_Temp)<<(4*4); 
	}
	break;
}
case 0x31400000:// QuadRotor  -> NRF -> PID
case 0x32400000:// QuadRotor  -> BLT -> PID
{
	//In this case, the user is going to set one of PID modules
	CMD_LastState = CMD_State;
	if(CMD_Temp==Info.BackIndex){
		CMD_State &= 0xFF0FFFFF;
	}
	else{
		CMD_State &= ~(1<<(4*4));
		CMD_State |= (++CMD_Temp)<<(4*4); 
	}
	break;
}

case 0x31500000:// QuadRotor  -> NRF -> System
case 0x32500000:// QuadRotor  -> BLT -> System
{
	CMD_LastState = CMD_State;
	if(CMD_Temp != Info.BackIndex)
		Info.userConfirm     = true;
	else
		Info.userRequireBack = true;
	break;
}
/*==========================ËÄ¼¶×´Ì¬===========================*/
case 0x21110000:// C_SmartCar -> NRF -> TEST_MODE -> MOTOR
case 0x22110000:// C_SmartCar -> BLT -> TEST_MODE -> MOTOR
{
	//In this case, the user is testing Motor and ready to return.
	CMD_LastState = CMD_State;
	CMD_State &= 0xFFF0FFFF;
	break;
}

case 0x21120000:// C_SmartCar -> NRF -> TEST_MODE -> SERVO
case 0x22120000:// C_SmartCar -> BLT -> TEST_MODE -> SERVO
{
	if(CMD_Temp==Info.TEXT_Num-1)
	{
		CMD_LastState = CMD_State;
		CMD_State &= 0xFFF0FFFF;
		/*CMD_BOX.Project_C_SmartCar.CMD_Buf[0] = 0x39;
		CMD_BOX.Project_C_SmartCar.CMD_Buf[1] = 0x04;
		CMD_BOX.Project_C_SmartCar.CMD_Buf[2] = 3;
		while(NRF_TX_Send(CMD_BOX.Project_C_SmartCar.CMD_Buf)!=TX_OK);*/
	}
	break;
}

case 0x21130000:// C_SmartCar -> NRF -> TEST_MODE -> CAMERA
case 0x22130000:// C_SmartCar -> BLT -> TEST_MODE -> CAMERA
{
	CMD_LastState = CMD_State;
	if(CMD_Temp==8)
	{
		CMD_State &= 0xFFF0FFFF;
		CMD_BOX.Project_C_SmartCar.CMD_Buf[0] = 0x39;
		CMD_BOX.Project_C_SmartCar.CMD_Buf[1] = 0x02;
		CMD_BOX.Project_C_SmartCar.CMD_Buf[2] = 8;
		while(NRF_TX_Send(CMD_BOX.Project_C_SmartCar.CMD_Buf)!=TX_OK);
	}
	break;
}

case 0x21320000:// C_SmartCar -> NRF -> MANAUL_MODE -> Drive Setting
case 0x22320000:// C_SmartCar -> BLT -> MANAUL_MODE -> Drive Setting
{
	if(CMD_Temp == Info.TEXT_Num-1)
		CMD_State &= 0xFFF0FFFF;
	break;
}
case 0x21310000:// C_SmartCar -> NRF -> MANAUL_MODE -> Start Driving
case 0x22310000:// C_SmartCar -> BLT -> MANAUL_MODE -> Start Driving
{
	CMD_Temp = 0xff;
	break;
}
case 0x21410000:// C_SmartCar -> NRF -> ALGORITHM   -> Image Processor
case 0x22410000:// C_SmartCar -> BLT -> ALGORITHM   -> Image Processor
{
	CMD_LastState = CMD_State;
	if(CMD_Temp != Info.TEXT_Num-1)
	{
		CMD_State &= 0xFFFF0FFF;
		CMD_State |= (++CMD_Temp)<<(3*4);
	}
	else
		CMD_State &=  0xFFF0FFFF;
	break;
}
case 0x31110000:// QuadRotor  -> NRF -> Motor       -> Motor 1
case 0x32110000:// QuadRotor  -> BLT -> Motor       -> Motor 1
case 0x31120000:// QuadRotor  -> NRF -> Motor       -> Motor 2
case 0x32120000:// QuadRotor  -> BLT -> Motor       -> Motor 2
case 0x31130000:// QuadRotor  -> NRF -> Motor       -> Motor 3
case 0x32130000:// QuadRotor  -> BLT -> Motor       -> Motor 3
case 0x31140000:// QuadRotor  -> NRF -> Motor       -> Motor 4
case 0x32140000:// QuadRotor  -> BLT -> Motor       -> Motor 4
case 0x31150000:// QuadRotor  -> NRF -> Motor       -> Motor ALL
case 0x32150000:// QuadRotor  -> BLT -> Motor       -> Motor ALL
{
	//In this case, the user is going to quit
	CMD_LastState = CMD_State;
	
	switch(CMD_Temp){
		case 0:
		Info.userConfirm = true;
		break;

		case 1:
		Info.userRequireBack = true;
		Info.userWantToSave  = false;
		break;

		case 2:
		Info.userRequireBack = true;
		Info.userWantToSave  = true;
		break;
	}
	break;
}
case 0x31160000:// QuadRotor  -> NRF -> Motor       -> Parameter...
case 0x32160000:// QuadRotor  -> BLT -> Motor       -> Parameter...
{
	//In this case, the user is going to quit
	CMD_LastState = CMD_State;
	if(CMD_Temp != Info.BackIndex)
		Info.userConfirm     = true;
	else
		Info.userRequireBack = true;
	break;
}

case 0x31410000:// QuadRotor  -> NRF -> PID         -> Pitch
case 0x32410000:// QuadRotor  -> BLT -> PID         -> Pitch
case 0x31420000:// QuadRotor  -> NRF -> PID         -> Roll
case 0x32420000:// QuadRotor  -> BLT -> PID         -> Roll
case 0x31430000:// QuadRotor  -> NRF -> PID         -> Yaw
case 0x32430000:// QuadRotor  -> BLT -> PID         -> Yaw
{
//	In this case, the user is going to back.
	// if(CMD_Temp == Info.BackIndex){
	// 	CMD_LastState = CMD_State;
	// 	Info.userRequireBack = true;
	// }
	if(CMD_Temp != Info.BackIndex)
		Info.userConfirm     = true;
	else
		Info.userRequireBack = true;
	break;
}

case 0x31450000:// QuadRotor  -> NRF -> PID         -> Throttle
case 0x32450000:// QuadRotor  -> BLT -> PID         -> Throttle
{
//  In this case, the user is going either to send throttle or back.

	if(CMD_Temp != Info.BackIndex)
		Info.userConfirm     = true;
	else
		Info.userRequireBack = true;
	break;	
}
/*==========================Îå¼¶×´Ì¬===========================*/
case 0x21411000:// C_SmartCar -> NRF -> ALGORITHM -> Image Processor -> Gussian Blur
case 0x22411000:// C_SmartCar -> BLT -> ALGORITHM -> Image Processor -> Gussian Blur
{
//	In this case, the user is going to save and send config data.
	CMD_LastState = CMD_State;
	if(CMD_Temp == Info.TEXT_Num-1)
	{
		//CMD_BOX.Project_C_SmartCar.CMD_Buf[0]=0x39;
		//CMD_BOX.Project_C_SmartCar.CMD_Buf[1]=0x32;
		//CMD_BOX.Project_C_SmartCar.CMD_Buf[2]=2;
		//while(NRF_TX_Send(CMD_BOX.Project_C_SmartCar.CMD_Buf)!=TX_OK);
		CMD_State &=  0xFFFF0FFF;
	}
	break;
}
case 0x21412000:// C_SmartCar -> NRF -> ALGORITHM -> Image Processor -> Binary
case 0x22412000:// C_SmartCar -> BLT -> ALGORITHM -> Image Processor -> Binary
{
//	In this case, the user is going to save and send config data
	CMD_LastState = CMD_State;
	if(CMD_Temp == Info.TEXT_Num-1)
	{
		//CMD_BOX.Project_C_SmartCar.CMD_Buf[0]=0x39;
		//CMD_BOX.Project_C_SmartCar.CMD_Buf[1]=0x34;
		//CMD_BOX.Project_C_SmartCar.CMD_Buf[2]=2;
		//while(NRF_TX_Send(CMD_BOX.Project_C_SmartCar.CMD_Buf)!=TX_OK);
		CMD_State &=  0xFFFF0FFF;
	}
	break;
}
case 0x21413000:// C_SmartCar -> NRF -> ALGORITHM -> Image Processor -> Canny Edge
case 0x22413000:// C_SmartCar -> BLT -> ALGORITHM -> Image Processor -> Canny Edge		
{
//	In this case, the user is going to save and send config data
	CMD_LastState = CMD_State;
	if(CMD_Temp == Info.TEXT_Num-1)
	{
		//CMD_BOX.Project_C_SmartCar.CMD_Buf[0]=0x39;
		//CMD_BOX.Project_C_SmartCar.CMD_Buf[1]=0x36;
		//CMD_BOX.Project_C_SmartCar.CMD_Buf[2]=6;
		//while(NRF_TX_Send(CMD_BOX.Project_C_SmartCar.CMD_Buf)!=TX_OK);
		CMD_State &=  0xFFFF0FFF;
	}
	break;
}
case 0x21414000:// C_SmartCar -> NRF -> ALGORITHM -> Image Processor -> Avg Filter
case 0x22414000:// C_SmartCar -> BLT -> ALGORITHM -> Image Processor -> Avg Filter	
{
//	In this case, the user is going to save and send conig data
	CMD_LastState = CMD_State;
	if(CMD_Temp == Info.TEXT_Num-1)
	{
		CMD_BOX.Project_C_SmartCar.CMD_Buf[0]=0x39;
		CMD_BOX.Project_C_SmartCar.CMD_Buf[1]=Set_Image_AvgFilter;
		CMD_BOX.Project_C_SmartCar.CMD_Buf[2]=1;
		while(NRF_TX_Send(CMD_BOX.Project_C_SmartCar.CMD_Buf)!=TX_OK);
		CMD_State &=  0xFFFF0FFF;
	}
	break;
}
case 0x21415000:// C_SmartCar -> NRF -> ALGORITHM -> Image Processor -> Mid Filter
case 0x22415000:// C_SmartCar -> BLT -> ALGORITHM -> Image Processor -> Mid Filter	
{
//	In this case, the user is going to save and send conig data
	CMD_LastState = CMD_State;
	if(CMD_Temp == Info.TEXT_Num-1)
	{
		CMD_BOX.Project_C_SmartCar.CMD_Buf[0]=0x39;
		CMD_BOX.Project_C_SmartCar.CMD_Buf[1]=Set_Image_MidFilter;
		CMD_BOX.Project_C_SmartCar.CMD_Buf[2]=1;
		while(NRF_TX_Send(CMD_BOX.Project_C_SmartCar.CMD_Buf)!=TX_OK);
		CMD_State &=  0xFFFF0FFF;
	}
	break;
}

/*======================================================================*/		
		}
	}
	delay_ms(150);
	EXTI_ClearITPendingBit(EXTI_Line7);
}



