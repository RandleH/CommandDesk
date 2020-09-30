
#include "projectConfig.h"
#include "GPIOLIKE51.h"
#include "headfile.h"
#include "OLED_Image.h"


uint8_t RX_BUF[32]={0};

int main(void)
{
	InfoStrucure_Init(&Info);
	System_Init();
	NRF24L01_Init(NRF_TX_MODE);
	while(1)
	{
		switch(CMD_State)
		{
/*==========================Ò»¼¶×´Ì¬===========================*/
case 0x00000000://Command Desk shows welcome
{
	if(CMD_LastState!=0x00000000)
	{
		InfoStrucure_Init(&Info);
		Info.TEXT_Num = 3;
		Info.TEXT_Buf[0]  = "H SmartCar";
		Info.TEXT_Buf[1]  = "C SmartCar";
		Info.TEXT_Buf[2]  = "QuadRotor";
		Info.TITLE_Buf	  = "CMD Desk V1.0";
		CMD_LastState = 0x00000000;
	}
	break;
}
case 0x10000000://Porject: H SmartCar
case 0x20000000://Project: C SmartCar

case 0x30000000://Project: Quadrotor
{
	QuadrotorPrj_Init();

	if( 	 CMD_LastState!=0x10000000 
		  && CMD_LastState!=0x20000000 
		  && CMD_LastState!=0x30000000   )
	{
		OLED_Clean();
		InfoStrucure_Init(&Info);
		if(CMD_LastState == 0x31000000){
			QuadrotorPrj.operation[QUADROTOR_CMD_DISCONNECT](NULL);
		}
		if(QuadrotorPrj.connected == true){
			Info.TITLE_Buf	  = "Disconnect Failed";
			Info.TEXT_Num = 1;
			Info.TEXT_Buf[0]  = "Back";
			Info.BackIndex    = 0;
		}else{
			Info.TEXT_Num = 3;
			Info.TEXT_Buf[0]  = "NRF_24L01";
			Info.TEXT_Buf[1]  = "BTL 5.0";
			Info.TEXT_Buf[2]  = "Back";
			Info.TITLE_Buf	  = "Wireless Device:";	
			Info.BackIndex    = 2;
		}
		CMD_LastState = CMD_State;
	}
	break;
}

/*==========================¶þ¼¶×´Ì¬===========================*/
case 0x11000000:// H_SmartCar -> NRF
case 0x12000000:// H_SmartCar -> BLT
{
	if(CMD_LastState != 0x11000000 && CMD_LastState != 0x12000000)
	{
		OLED_Clean();
		InfoStrucure_Init(&Info);
		Info.TEXT_Num = 1;
		Info.TEXT_Buf[0]  = "void";
		Info.TITLE_Buf	  = "VOID";
		CMD_LastState = CMD_State;
	}
	break;
}
#ifdef PRJ_C_SMARTCAR
case 0x21000000:// C_SmartCar -> NRF		
case 0x22000000:// C_SmartCar -> BLT
{
	if(CMD_LastState != 0x21000000 && CMD_LastState != 0x22000000)
	{
		OLED_Clean();
		InfoStrucure_Init(&Info);
		Info.TEXT_Num = 4+1;
		Info.TEXT_Buf[0]  = "TEST MODULE";
		Info.TEXT_Buf[1]  = "NXP CONTEST";
		Info.TEXT_Buf[2]  = "MANUAL DRIVE";
		Info.TEXT_Buf[3]  = "ALGORITHM";
		Info.TEXT_Buf[4]  = "Back";
		Info.TITLE_Buf	  = "MODE:";
		CMD_LastState = CMD_State;
	}
	break;
}
#endif
case 0x31000000:// QuadRotor -> NRF
case 0x32000000:// QuadRotor -> BLT
{
	if(CMD_LastState != 0x31000000 && CMD_LastState != 0x32000000){
		OLED_Clean();
		
		if(QuadrotorPrj.connected == false){		//Maybe it is uninitialized.
			QuadrotorPrj.operation[QUADROTOR_CMD_CONNECT](NULL);//Initializing...
			if(QuadrotorPrj.connected == false){	//Initialization failed.
				InfoStrucure_Init(&Info);
				Info.TEXT_Num = 0+1;
				Info.TITLE_Buf	  = "Connect Failed";
				Info.TEXT_Buf[0]  = "Back";
				Info.BackIndex    = 0;
			}
		}
		if(QuadrotorPrj.connected == true){			//Initialization Succeed or already been initialized.
			InfoStrucure_Init(&Info);
			Info.TEXT_Num = 5+1;
			Info.TEXT_Buf[0]  = "Motor";
			Info.TEXT_Buf[1]  = "Camera";
			Info.TEXT_Buf[2]  = "Servo";
			Info.TEXT_Buf[3]  = "PID";
			Info.TEXT_Buf[4]  = "System";
			Info.TEXT_Buf[5]  = "Back";
			Info.BackIndex    = 5;
			Info.TITLE_Buf	  = "QUADROTOR";
		}
		CMD_LastState = CMD_State;
	}
	break;
}


/*==========================Èý¼¶×´Ì¬===========================*/
#ifdef PRJ_C_SMARTCAR
case 0x21100000:// C_SmartCar -> NRF -> TEST MODULE
case 0x22100000:// C_SmartCar -> BLT -> TEST MODULE
{
	if(CMD_LastState != 0x21100000 && CMD_LastState != 0x22100000)
	{
		OLED_Clean();
		InfoStrucure_Init(&Info);
		Info.TEXT_Num = 7;
		Info.TEXT_Buf[0]  = "MOTOR";
		Info.TEXT_Buf[1]  = "SERVO";
		Info.TEXT_Buf[2]  = "CAMERA";
		Info.TEXT_Buf[3]  = "CODER";
		Info.TEXT_Buf[4]  = "MEGNETICS";
		Info.TEXT_Buf[5]  = "BUZZER";
		Info.TEXT_Buf[6]  = "Back";
		Info.TITLE_Buf	  = "MODULE:";
		CMD_BOX.Project_C_SmartCar.CMD_Buf[0] = 0x39;
		CMD_BOX.Project_C_SmartCar.CMD_Buf[1] = 0x20;
		CMD_BOX.Project_C_SmartCar.CMD_Buf[2] = 0;
		NRF_TX_Send(CMD_BOX.Project_C_SmartCar.CMD_Buf);
		CMD_LastState = CMD_State;
	}
	break;
}
case 0x21200000:// C_SmartCar -> NRF -> NXP CONTEST
	break;
case 0x21300000:// C_SmartCar -> NRF -> MANAUL_MODE
case 0x22300000:// C_SmartCar -> BLT -> MANAUL_MODE
{
	if(CMD_LastState != 0x21300000 && CMD_LastState != 0x22300000)
	{
		OLED_Clean();
		InfoStrucure_Init(&Info);
		Info.TEXT_Num = 3;
		Info.TEXT_Buf[0]  = "Start Driving";
		Info.TEXT_Buf[1]  = "Param Setting";
		Info.TEXT_Buf[2]  = "Back";
		Info.TITLE_Buf	  = "MANUAL:";
		CMD_BOX.Project_C_SmartCar.CMD_Buf[0] = 0x39;
		CMD_BOX.Project_C_SmartCar.CMD_Buf[1] = Manual_Drive;
		CMD_BOX.Project_C_SmartCar.CMD_Buf[2] = 0;
		CMD_LastState = CMD_State;
		while(NRF_TX_Send(CMD_BOX.Project_C_SmartCar.CMD_Buf)!=TX_OK);
	}
	break;
	
	/*
	if(CMD_LastState != 0x21300000 && CMD_LastState != 0x22300000)
	{
		
	}
	CMD_BOX.Project_C_SmartCar.CMD_Buf[3]=JoyStick_Data[X];
	CMD_BOX.Project_C_SmartCar.CMD_Buf[4]=JoyStick_Data[Y];
	NRF_TX_Send(CMD_BOX.Project_C_SmartCar.CMD_Buf);
	break;*/
}
case 0x21400000:// C_SmartCar -> NRF -> ALGORITHM
case 0x22400000:// C_SmartCar -> BLT -> ALGORITHM
{
	if(CMD_LastState != 0x21400000 && CMD_LastState != 0x22400000)
	{
		OLED_Clean();
		InfoStrucure_Init(&Info);
		Info.TEXT_Num = 2;
		Info.TEXT_Buf[0]  = "IMG Filter";
		Info.TEXT_Buf[1]  = "Back";
		Info.TITLE_Buf	  = "ALGORITHM:";
		CMD_BOX.Project_C_SmartCar.CMD_Buf[0] = 0x39;
		CMD_BOX.Project_C_SmartCar.CMD_Buf[1] = 0x30;
		CMD_BOX.Project_C_SmartCar.CMD_Buf[2] = 1;
		//NRF_TX_Send(CMD_BOX.Project_C_SmartCar.CMD_Buf);
		CMD_LastState = CMD_State;
	}
	break;
}
#endif
case 0x31100000:// QuadRotor  -> NRF -> Motor
case 0x32100000:// QuadRotor  -> BLT -> Motor
{
	if(CMD_LastState != 0x31100000 && CMD_LastState != 0x32100000)
	{
		OLED_Clean();
		InfoStrucure_Init(&Info);
		Info.TEXT_Num = 6+1;
		Info.TEXT_Buf[0]  = "Motor 1";//
		Info.TEXT_Buf[1]  = "Motor 2";
		Info.TEXT_Buf[2]  = "Motor 3";
		Info.TEXT_Buf[3]  = "Motor 4";
		Info.TEXT_Buf[4]  = "Motor ALL";//
		Info.TEXT_Buf[5]  = "Parameter...";
		Info.TEXT_Buf[6]  = "Back";
		Info.BackIndex    = 6;
		Info.TITLE_Buf	  = "MOTOR TEST";
		CMD_LastState = CMD_State;
	}
	break;
}
case 0x31400000:// QuadRotor  -> NRF -> PID
case 0x32400000:// QuadRotor  -> BLT -> PID
{
	if(CMD_LastState != 0x31400000 && CMD_LastState != 0x32400000)
	{
		OLED_Clean();
		InfoStrucure_Init(&Info);
		Info.TEXT_Num = 5+1;
		Info.TEXT_Buf[0]  = "Pitch";
		Info.TEXT_Buf[1]  = "Roll";
		Info.TEXT_Buf[2]  = "Yaw #";
		Info.TEXT_Buf[3]  = "Thrust #";
		Info.TEXT_Buf[4]  = "Throttle";
		Info.TEXT_Buf[5]  = "Back";
		Info.BackIndex    = 5;
		Info.TITLE_Buf	  = "PID";
		CMD_LastState = CMD_State;
	}
	break;
}
case 0x31500000:// QuadRotor  -> NRF -> System
case 0x32500000:// QuadRotor  -> BLT -> System
{
	if(CMD_LastState != 0x31500000 && CMD_LastState != 0x32500000){
		OLED_Clean();
		InfoStrucure_Init(&Info);
		Info.TEXT_Num = 4+1;
		Info.TEXT_Buf[0]  = "enMotor    ON ";
		Info.TEXT_Buf[1]  = "enPID      ON ";
		Info.TEXT_Buf[2]  = "enLED      ON ";
		Info.TEXT_Buf[3]  = "enCamera   ON ";
		Info.TEXT_Buf[4]  = "Back";
		Info.BackIndex    = 4;
		Info.TITLE_Buf	  = "System";
		Info.userRequireBack = false;
		Info.userWantToSave  = false;
		Info.userConfirm     = false;
		QuadrotorPrj.operation[ QUADROTOR_CMD_SYSTEMFLAGQUERY  ](NULL);
		CMD_LastState = CMD_State;
	}
	Info.TextActive		  = true;
	Info.CursorActive	  = true;
	QuadrotorPrj.operation[ QUADROTOR_CMD_SYSTEMFLAG  ](NULL);
	if(Info.userRequireBack == true){
		uint8_t index = QUADROTOR_CMD_EXIT_SYSTEMFLAG;
		CMD_State &=  0xFF0FFFFF;
		Info.userRequireBack = false;
		QuadrotorPrj.operation[ QUADROTOR_CMD_EXIT_SYSTEMFLAG  ](&index);
	}
	break;
}

/*==========================ËÄ¼¶×´Ì¬===========================*/
#ifdef PRJ_C_SMARTCAR
case 0x21110000:// C_SmartCar -> NRF -> TEST_MODE -> MOTOR
case 0x22110000:// C_SmartCar -> BLT -> TEST_MODE -> MOTOR
{
	OLED_Clean();
	Info.Info_Cmd = Disable;
	while(CMD_State==0x21110000 || CMD_State==0x22110000)
	{
		C_SmartCar_TestMotor();
	}
	break;
}
case 0x21120000:// C_SmartCar -> NRF -> TEST_MODE -> SERVO
case 0x22120000:// C_SmartCar -> BLT -> TEST_MODE -> SERVO
{
	if(CMD_LastState != 0x21120000 && CMD_LastState != 0x22120000)		
	{
		OLED_Clean();
		InfoStrucure_Init(&Info);
		Info.TITLE_Buf 	 = "SERVO:";
		Info.TEXT_Num = 8;
		Info.TEXT_Buf[0] = "Freq      %dHz";
		Info.TEXT_Buf[1] = "Duty      %d%%";
		Info.TEXT_Buf[2] = "MaxDuty   %d%%";
		Info.TEXT_Buf[3] = "MinDuty   %d%%";
		Info.TEXT_Buf[4] = "Angle     %d";
		Info.TEXT_Buf[5] = "MaxAngle  %d";
		Info.TEXT_Buf[6] = "MinAngle  %d";
		Info.TEXT_Buf[7] = "Save & Back";
		
		if(C_SmartCar_Get(Get_Servo))
		{
			Info.VAL_Buf[0] |= CMD_BOX.Project_C_SmartCar.CMD_Buf[3+0]<<8;
			Info.VAL_Buf[0] |= CMD_BOX.Project_C_SmartCar.CMD_Buf[3+1];
			Info.VAL_Buf[1]  = CMD_BOX.Project_C_SmartCar.CMD_Buf[4+1];
			Info.VAL_Buf[2]  = CMD_BOX.Project_C_SmartCar.CMD_Buf[4+2];
			Info.VAL_Buf[3]  = CMD_BOX.Project_C_SmartCar.CMD_Buf[4+3];
			Info.VAL_Buf[4]  = (int8_t)CMD_BOX.Project_C_SmartCar.CMD_Buf[4+4];
			Info.VAL_Buf[5]  = (int8_t)CMD_BOX.Project_C_SmartCar.CMD_Buf[4+5];
			Info.VAL_Buf[6]  = (int8_t)CMD_BOX.Project_C_SmartCar.CMD_Buf[4+6];
		}
		else
		{
			for(int i=0;i<Info.TEXT_Num;i++)
				Info.VAL_Buf[i]=0;
		}
	}
	C_SmartCar_SetServo();
	Info.TextActive		= true;
	Info.CursorActive	= true;
	CMD_LastState = CMD_State;
	break;
}
	
case 0x21130000:// C_SmartCar -> NRF -> TEST_MODE -> CAMERA
case 0x22130000:// C_SmartCar -> BLT -> TEST_MODE -> CAMERA
{
	if(CMD_LastState != 0x21130000 && CMD_LastState != 0x22130000)		
	{
		OLED_Clean();
		InfoStrucure_Init(&Info);
		Info.TITLE_Buf	  = "MT9V032:";
		Info.TEXT_Num = 9;
		Info.TEXT_Buf[0]  = "AUTO_EXP     %d";
		Info.TEXT_Buf[1]  = "EXP_TIME     %d";
		Info.TEXT_Buf[2]  = "FPS          %d";
		Info.TEXT_Buf[3]  = "SET_COL      %d";
		Info.TEXT_Buf[4]  = "SET_ROW      %d";   
		Info.TEXT_Buf[5]  = "LR_OFFSET    %d";
		Info.TEXT_Buf[6]  = "UD_OFFSET    %d";
		Info.TEXT_Buf[7]  = "GAIN         %d";
		Info.TEXT_Buf[8]  = "Save & Back";
		Info.Cursor_Pos  = 100;
		Info.Cursor_Buf  = "<->";
		if(C_SmartCar_GetCamera())
		{
			Info.VAL_Buf[0]   = CMD_BOX.Project_C_SmartCar.CMD_Buf[0+3];
			Info.VAL_Buf[1]   = CMD_BOX.Project_C_SmartCar.CMD_Buf[1+3];
			Info.VAL_Buf[2]   = CMD_BOX.Project_C_SmartCar.CMD_Buf[2+3];
			Info.VAL_Buf[3]   = CMD_BOX.Project_C_SmartCar.CMD_Buf[3+3];
			Info.VAL_Buf[4]   = CMD_BOX.Project_C_SmartCar.CMD_Buf[4+3];
			Info.VAL_Buf[5]   = CMD_BOX.Project_C_SmartCar.CMD_Buf[5+3];
			Info.VAL_Buf[6]   = CMD_BOX.Project_C_SmartCar.CMD_Buf[6+3];
			Info.VAL_Buf[7]   = CMD_BOX.Project_C_SmartCar.CMD_Buf[7+3];
		}
		else// Connect failed!
		{
			for(int i=0;i<8;i++)
				Info.VAL_Buf[i] = 0;
		}
	}
	C_SmartCar_SetCamera();
	Info.TextActive		= true;
	Info.CursorActive	= true;
	CMD_LastState = CMD_State;
	break;
}
case 0x21140000:// C_SmartCar -> NRF -> TEST_MODE   -> CODER
case 0x22140000:// C_SmartCar -> BLT -> TEST_MODE   -> CODER
{
	break;
}
case 0x21150000:// C_SmartCar -> NRF -> TEST_MODE   -> MEGNETICS
case 0x22150000:// C_SmartCar -> BLT -> TEST_MODE   -> MEGNETICS
{
	break;
}
case 0x21160000:// C_SmartCar -> NRF -> TEST_MODE   -> BUZZER
case 0x22160000:// C_SmartCar -> BLT -> TEST_MODE   -> BUZZER 
{
	break;
}
case 0x21310000:// C_SmartCar -> NRF -> MANAUL_MODE -> Start Driving
case 0x22310000:// C_SmartCar -> BLT -> MANAUL_MODE -> Start Driving
{
	if(CMD_LastState != 0x21310000 && CMD_LastState != 0x22310000)
	{
		OLED_Clean();
		Info.Info_Cmd = Disable;
		OLED_Display_Triangle(2,(127-16)/2,'^');
		OLED_Display_Triangle(4,127/2-8-8-8,'<');
		OLED_Display_Triangle(4,127/2+8+8,'>');
		OLED_Display_Triangle(7,(127-16)/2,'v');
		CMD_Temp = 0;
		CMD_LastState = CMD_State;
	}
	if(CMD_Temp == 0)
		C_SmartCar_Manual(CTRL,NULL);
	if(CMD_Temp == 0xff)
	{
		C_SmartCar_Manual(QUIT,NULL);
		CMD_LastState = CMD_State;
		CMD_State &= 0xFFF0FFFF;
	}
	break;
}
case 0x21320000:// C_SmartCar -> NRF -> MANAUL_MODE -> Drive Setting
case 0x22320000:// C_SmartCar -> BLT -> MANAUL_MODE -> Drive Setting
{
	ManualDrive_Typedef Setting;
	if(CMD_LastState != 0x21320000 && CMD_LastState != 0x22320000)
	{
		OLED_Clean();
		InfoStrucure_Init(&Info);
		Info.TITLE_Buf 	 = "Setting:";
		Info.TEXT_Num = 2;
		Info.TEXT_Buf[0] = "Speed:       %d%%";
		Info.TEXT_Buf[1] = "Save & Back";
		CMD_LastState = CMD_State;
	}
	C_SmartCar_Manual(RECORD,&Setting);
	Info.VAL_Buf[0] = Setting.SpeedDuty;
	Info.TextActive		= true;
	Info.CursorActive	= true;
	
	break;
}

case 0x21410000:// C_SmartCar -> NRF -> ALGORITHM   -> Image Processor
case 0x22410000:// C_SmartCar -> BLT -> ALGORITHM   -> Image Processor
{
	if(CMD_LastState != 0x21410000 && CMD_LastState != 0x22410000)
	{
		OLED_Clean();
		InfoStrucure_Init(&Info);
		Info.TEXT_Num = 7;
		Info.TEXT_Buf[0]  = "Gussian Blur";
		Info.TEXT_Buf[1]  = "Binary";
		Info.TEXT_Buf[2]  = "Canny Edge";
		Info.TEXT_Buf[3]  = "Avg Filter";
		Info.TEXT_Buf[4]	= "Mid Filter";
		Info.TEXT_Buf[5]  = "Origin Img";
		Info.TEXT_Buf[6]  = "Back";
		Info.Cursor_Pos  = 100; 
		Info.Cursor_Buf  = "<-";
		Info.TITLE_Buf	  = "Image Processor:";
		CMD_LastState = CMD_State;
	}
	break;
}
#endif

case 0x31110000:// QuadRotor  -> NRF -> Motor       -> Motor 1
case 0x32110000:// QuadRotor  -> BLT -> Motor       -> Motor 1
{
	uint32_t temp_param[2] = {CMD_State,0};
	if(CMD_LastState != 0x31110000 && CMD_LastState != 0x32110000){
		OLED_Clean();
		InfoStrucure_Init(&Info);
		Info.TITLE_Buf	  = "Motor 1:";
		Info.TEXT_Num = 2+2;
		Info.TEXT_Buf[0]  = "SPEED     %d";
		Info.TEXT_Buf[1]  = "Turn Off & Leave";
		Info.TEXT_Buf[2]  = "Save & Leave";
		Info.TEXT_Buf[3]  = "STEP      %d";
		Info.BackIndex    = 2;// <!> Must modify "isr.c" simultaneously
		Info.Cursor_Pos   = 110;
		Info.Cursor_Buf   = "<->";
		Info.VAL_Buf[0]   = 0;
		Info.userRequireBack = false;
		
		CMD_LastState = CMD_State;
	}
	QuadrotorPrj.operation[QUADROTOR_CMD_TESTMOTOR](temp_param);//[ Cyclic Function ]
	temp_param[0] = CMD_State;
	temp_param[1] = 0;	// 1 = Stop Motor;
	Info.TextActive		= true;
	Info.CursorActive	= true;
	if(Info.userRequireBack == true){
		CMD_State &=  0xFFF0FFFF;
		if(Info.userWantToSave == false){
			temp_param[1] = 1;
			QuadrotorPrj.operation[QUADROTOR_CMD_EXIT_TESTMOTOR](temp_param);
		}
	}
	break;
}

case 0x31120000:// QuadRotor  -> NRF -> Motor       -> Motor 2
case 0x32120000:// QuadRotor  -> BLT -> Motor       -> Motor 2
{
	uint32_t temp_param[2] = {0};
	if(CMD_LastState != 0x31120000 && CMD_LastState != 0x32120000){
		OLED_Clean();
		InfoStrucure_Init(&Info);
		Info.TITLE_Buf	  = "Motor 2:";
		Info.TEXT_Num = 2+2;
		Info.TEXT_Buf[0]  = "SPEED     %d";
		Info.TEXT_Buf[1]  = "Turn Off & Leave";
		Info.TEXT_Buf[2]  = "Save & Leave";
		Info.TEXT_Buf[3]  = "STEP      %d";
		Info.BackIndex    = 2;// <!> Must modify "isr.c" simultaneously
		Info.Cursor_Pos   = 110;
		Info.Cursor_Buf   = "<->";
		Info.VAL_Buf[0]   = 0;

		Info.userRequireBack = false;
		CMD_LastState = CMD_State;
	}
	temp_param[0] = CMD_State;
	temp_param[1] = 0;	// 1 = Stop Motor;
	QuadrotorPrj.operation[QUADROTOR_CMD_TESTMOTOR](temp_param);
	Info.TextActive		= true;
	Info.CursorActive	= true;
	if(Info.userRequireBack == true){
		CMD_State &=  0xFFF0FFFF;
		if(Info.userWantToSave == false){
			temp_param[1] = 1;
			QuadrotorPrj.operation[QUADROTOR_CMD_EXIT_TESTMOTOR](temp_param);
		}
	}
	break;
}

case 0x31130000:// QuadRotor  -> NRF -> Motor       -> Motor 3
case 0x32130000:// QuadRotor  -> BLT -> Motor       -> Motor 3
{
	uint32_t temp_param[2] = {0};
	if(CMD_LastState != 0x31130000 && CMD_LastState != 0x32130000){
		OLED_Clean();
		InfoStrucure_Init(&Info);
		Info.TITLE_Buf	  = "Motor 3:";
		Info.TEXT_Num = 2+2;
		Info.TEXT_Buf[0]  = "SPEED     %d";
		Info.TEXT_Buf[1]  = "Turn Off & Leave";
		Info.TEXT_Buf[2]  = "Save & Leave";
		Info.TEXT_Buf[3]  = "STEP      %d";
		Info.BackIndex    = 2;// <!> Must modify "isr.c" simultaneously
		Info.Cursor_Pos   = 110;
		Info.Cursor_Buf   = "<->";
		Info.VAL_Buf[0]   = 0;
		Info.VAL_Buf[3]   = 0;

		Info.userRequireBack = false;
		CMD_LastState = CMD_State;
	}
	temp_param[0] = CMD_State;
	temp_param[1] = 0;	// 1 = Stop Motor;
	QuadrotorPrj.operation[QUADROTOR_CMD_TESTMOTOR](temp_param);
	Info.TextActive		= true;
	Info.CursorActive	= true;
	if(Info.userRequireBack == true){
		CMD_State &=  0xFFF0FFFF;
		if(Info.userWantToSave == false){
			temp_param[1] = 1;
			QuadrotorPrj.operation[QUADROTOR_CMD_EXIT_TESTMOTOR](temp_param);
		}
	}
	break;
}

case 0x31140000:// QuadRotor  -> NRF -> Motor       -> Motor 4
case 0x32140000:// QuadRotor  -> BLT -> Motor       -> Motor 4
{
	uint32_t temp_param[2] = {0};
	if(CMD_LastState != 0x31140000 && CMD_LastState != 0x32140000){
		OLED_Clean();
		InfoStrucure_Init(&Info);
		Info.TITLE_Buf	  = "Motor 4:";
		Info.TEXT_Num = 2+2;
		Info.TEXT_Buf[0]  = "SPEED     %d";
		Info.TEXT_Buf[1]  = "Turn Off & Leave";
		Info.TEXT_Buf[2]  = "Save & Leave";
		Info.TEXT_Buf[3]  = "STEP      %d";
		Info.BackIndex    = 2;// <!> Must modify "isr.c" simultaneously
		Info.Cursor_Pos   = 110;
		Info.Cursor_Buf   = "<->";
		Info.VAL_Buf[0]   = 0;

		Info.userRequireBack = false;
		CMD_LastState = CMD_State;
	}
	temp_param[0] = CMD_State;
	temp_param[1] = 0;	// 1 = Stop Motor;
	QuadrotorPrj.operation[QUADROTOR_CMD_TESTMOTOR](temp_param);
	Info.TextActive		= true;
	Info.CursorActive	= true;
	if(Info.userRequireBack == true){
		CMD_State &=  0xFFF0FFFF;
		if(Info.userWantToSave == false){
			temp_param[1] = 1;
			QuadrotorPrj.operation[QUADROTOR_CMD_EXIT_TESTMOTOR](temp_param);
		}
	}
	break;
}

case 0x31150000:// QuadRotor  -> NRF -> Motor       -> Motor ALL
case 0x32150000:// QuadRotor  -> BLT -> Motor       -> Motor ALL
{
	uint32_t temp_param[2] = {CMD_State,0};
	// [0]: CMD_State;
	// [1]: MotorStop;
	// [2]: ShakeHand Message;


	if(CMD_LastState != 0x31150000 && CMD_LastState != 0x32150000){
		OLED_Clean();
		InfoStrucure_Init(&Info);
		Info.TITLE_Buf	  = "Motor ALL:";
		Info.TEXT_Num = 3;
		Info.TEXT_Buf[0]  = "SPEED     %d";
		Info.TEXT_Buf[1]  = "Turn Off & Leave";
		Info.TEXT_Buf[2]  = "Save & Leave";
		Info.BackIndex    = 2;// <!> Must modify "isr.c" simultaneously
		Info.Cursor_Pos   = 110;
		Info.Cursor_Buf   = "<->";
		Info.VAL_Buf[0]   = 0;

		Info.userRequireBack = false;
		Info.userWantToSave  = false;
		Info.userConfirm     = false;
		CMD_LastState = CMD_State;
	}
	
	QuadrotorPrj.operation[QUADROTOR_CMD_TESTMOTOR](temp_param);
	Info.TextActive		= true;
	Info.CursorActive	= true;

	temp_param[0] = CMD_State;
	temp_param[1] = 0;	// 1 = Stop Motor;
	if(Info.userRequireBack == true){
		if(Info.userWantToSave == false){
			temp_param[1] = 1;
		}

		QuadrotorPrj.operation[ QUADROTOR_CMD_EXIT_TESTMOTOR ](temp_param);
		CMD_State &=  0xFFF0FFFF;
	}
	break;
}

case 0x31160000:// QuadRotor  -> NRF -> Motor       -> Parameter...
case 0x32160000:// QuadRotor  -> BLT -> Motor       -> Parameter...
{
	if(CMD_LastState != 0x31160000 && CMD_LastState != 0x32160000){
		OLED_Clean();
		InfoStrucure_Init(&Info);
		Info.TITLE_Buf	  = "MOTOR PARAM:";
		Info.TEXT_Num = 6+1;
		Info.TEXT_Buf[0]  = "Max Limit    %d";
		Info.TEXT_Buf[1]  = "Min Limit    %d";
		Info.TEXT_Buf[2]  = "Spd Range  0:%d";
		Info.TEXT_Buf[3]  = "PWM Duty     %d%%";
		Info.TEXT_Buf[4]  = "CNT 16Bit    %d";
		Info.TEXT_Buf[5]  = "STEP:        %d";

		Info.TEXT_Buf[6]  = "Back";
		Info.BackIndex    = 6;
		Info.Cursor_Pos   = 116;
		Info.userRequireBack = false;
		Info.userWantToSave  = false;
		Info.userConfirm     = false;
		QuadrotorPrj.operation[QUADROTOR_CMD_MOTORQUERY](NULL);
		CMD_LastState = CMD_State;
	}

	Info.TextActive		  = true;
	Info.CursorActive	  = true;

	QuadrotorPrj.operation[QUADROTOR_CMD_MOTORSETTING](NULL);
	
	if(Info.userRequireBack == true){
		uint8_t index = QUADROTOR_CMD_EXIT_MOTORSETTING;
		CMD_State &=  0xFFF0FFFF;
		Info.userRequireBack = false;
		QuadrotorPrj.operation[QUADROTOR_CMD_EXIT_MOTORSETTING](&index);
	}
	break;
}

case 0x31410000:// QuadRotor  -> NRF -> PID         -> Pitch
case 0x32410000:// QuadRotor  -> BLT -> PID         -> Pitch
{
	if(CMD_LastState != 0x31410000 && CMD_LastState != 0x32410000){
		OLED_Clean();
		InfoStrucure_Init(&Info);
		Info.TITLE_Buf	  = "Pitch:";
		Info.TEXT_Num     = 12+1;
		Info.floatDisplayFlag = true;
		Info.TEXT_Buf[0]   = "Kp     %.3f";
		Info.TEXT_Buf[1]   = "Ti     %.3f";
		Info.TEXT_Buf[2]   = "Td     %.3f";
		Info.TEXT_Buf[3]   = "STEP:  %.3f";
		Info.TEXT_Buf[4]   = "P-Lim  %.3f";
		Info.TEXT_Buf[5]   = "I-Lim  %.3f";
		Info.TEXT_Buf[6]   = "D-Lim  %.3f";
		Info.TEXT_Buf[7]   = "Module   ON ";
		Info.TEXT_Buf[8]   = "Motor 1  ON ";
		Info.TEXT_Buf[9]   = "Motor 2  ON ";
		Info.TEXT_Buf[10]  = "Motor 3  ON ";
		Info.TEXT_Buf[11]  = "Motor 4  ON ";
		Info.TEXT_Buf[12]  = "Back";
		Info.BackIndex    = 12;
		Info.Cursor_Pos   = 110;
		Info.Cursor_Buf   = "<->";
		Info.VAL_FLOAT_Buf[0]   = 0; // Kp
		Info.VAL_FLOAT_Buf[1]   = 0; // Ti
		Info.VAL_FLOAT_Buf[2]   = 0; // Td
		Info.VAL_FLOAT_Buf[3]   = 0; // STEP
		Info.VAL_FLOAT_Buf[4]   = 0; // P-Lim
		Info.VAL_FLOAT_Buf[5]   = 0; // I-Lim
		Info.VAL_FLOAT_Buf[6]   = 0; // D-Lim

		Info.userRequireBack = false;
		CMD_LastState = CMD_State;
		QuadrotorPrj.operation[QUADROTOR_CMD_PIDQUERY](&CMD_State);//[ Block Function ]

	}
	QuadrotorPrj.operation[QUADROTOR_CMD_PIDADJUSTER](&CMD_State);// [Cyclic Function ]
	Info.TextActive		  = true;
	Info.CursorActive	  = true;

	if(Info.userRequireBack == true){
		uint8_t index = QUADROTOR_CMD_EXIT_PIDADJUSTER;
		QuadrotorPrj.operation[index](&index);
		CMD_State &= 0xFFF0FFFF;
	}
	break;
}

case 0x31420000:// QuadRotor  -> NRF -> PID         -> Roll
case 0x32420000:// QuadRotor  -> BLT -> PID         -> Roll
{
	if(CMD_LastState != 0x31420000 && CMD_LastState != 0x32420000){
		OLED_Clean();
		InfoStrucure_Init(&Info);
		Info.TITLE_Buf	  = "Roll:";
		Info.TEXT_Num     = 4+1;
		Info.floatDisplayFlag = true;
		Info.TEXT_Buf[0]  = "Kp     %.3f";
		Info.TEXT_Buf[1]  = "Ti     %.3f";
		Info.TEXT_Buf[2]  = "Td     %.3f";
		Info.TEXT_Buf[3]  = "STEP:  %.3f";
		Info.TEXT_Buf[4]  = "Back";
		Info.BackIndex    = 4;
		Info.Cursor_Pos   = 110;
		Info.Cursor_Buf   = "<->";
		Info.VAL_FLOAT_Buf[0]   = 0;
		Info.VAL_FLOAT_Buf[1]   = 0;
		Info.VAL_FLOAT_Buf[2]   = 0;
		Info.VAL_FLOAT_Buf[3]   = 0;

		Info.userRequireBack = false;
		CMD_LastState = CMD_State;
		QuadrotorPrj.operation[QUADROTOR_CMD_PIDQUERY](&CMD_State);//[ Block Function ]

	}
	QuadrotorPrj.operation[QUADROTOR_CMD_PIDADJUSTER](&CMD_State);// [Cyclic Function ]
	Info.TextActive		  = true;
	Info.CursorActive	  = true;
	//Info.floatDisplayFlag = true;
	if(Info.userRequireBack == true){
		uint8_t index = QUADROTOR_CMD_EXIT_PIDADJUSTER;
		QuadrotorPrj.operation[index](&index);
		CMD_State &= 0xFFF0FFFF;
	}
	break;
}

case 0x31430000:// QuadRotor  -> NRF -> PID         -> Yaw
case 0x32430000:// QuadRotor  -> BLT -> PID         -> Yaw
{
	if(CMD_LastState != 0x31430000 && CMD_LastState != 0x32430000){
		OLED_Clean();
		InfoStrucure_Init(&Info);
		Info.TITLE_Buf	  = "Yaw:";
		Info.TEXT_Num     = 4+1;
		Info.floatDisplayFlag = true;
		Info.TEXT_Buf[0]  = "Kp     %.3f";
		Info.TEXT_Buf[1]  = "Ti     %.3f";
		Info.TEXT_Buf[2]  = "Td     %.3f";
		Info.TEXT_Buf[3]  = "STEP:  %.3f";
		Info.TEXT_Buf[4]  = "Back";
		Info.BackIndex    = 4;
		Info.Cursor_Pos   = 110;
		Info.Cursor_Buf   = "<->";
		Info.VAL_FLOAT_Buf[0]   = 0;
		Info.VAL_FLOAT_Buf[1]   = 0;
		Info.VAL_FLOAT_Buf[2]   = 0;
		Info.VAL_FLOAT_Buf[3]   = 0;

		Info.userRequireBack = false;
		CMD_LastState = CMD_State;
		QuadrotorPrj.operation[QUADROTOR_CMD_PIDQUERY](&CMD_State);//[ Block Function ]

	}
	QuadrotorPrj.operation[QUADROTOR_CMD_PIDADJUSTER](&CMD_State);// [Cyclic Function ]
	Info.TextActive		  = true;
	Info.CursorActive	  = true;
	//Info.floatDisplayFlag = true;
	if(Info.userRequireBack == true){
		uint8_t index = QUADROTOR_CMD_EXIT_PIDADJUSTER;
		QuadrotorPrj.operation[index](&index);
		CMD_State &= 0xFFF0FFFF;
	}
	break;
}

case 0x31440000:// QuadRotor  -> NRF -> PID         -> Thrust
case 0x32440000:// QuadRotor  -> BLT -> PID         -> Thrust
{}

case 0x31450000:// QuadRotor  -> NRF -> PID         -> Throttle
case 0x32450000:// QuadRotor  -> BLT -> PID         -> Throttle
{
	if(CMD_LastState != 0x31450000 && CMD_LastState != 0x32450000){
		OLED_Clean();
		InfoStrucure_Init(&Info);
		Info.TITLE_Buf	  = "Throttle:";
		Info.TEXT_Num     = 8+1;
		Info.floatDisplayFlag = false;
		Info.TEXT_Buf[0]  = "TROT:       %d";
		Info.TEXT_Buf[1]  = "Presentage: %d%%";
		Info.TEXT_Buf[2]  = "STEP:       %d  ";
		Info.TEXT_Buf[3]  = "Motor 1      on ";// Very Dangerous! Be sure to remove rotor before configuration!
		Info.TEXT_Buf[4]  = "Motor 2      on ";// Very Dangerous! Be sure to remove rotor before configuration!
		Info.TEXT_Buf[5]  = "Motor 3      on ";// Very Dangerous! Be sure to remove rotor before configuration!
		Info.TEXT_Buf[6]  = "Motor 4      on ";// Very Dangerous! Be sure to remove rotor before configuration!

		Info.TEXT_Buf[7]  = "Back";
		Info.TEXT_Buf[8]  = "Kill Motor";
		
		Info.BackIndex    = 7;
		Info.Cursor_Pos   = 110;
		Info.Cursor_Buf   = "<->";
		Info.VAL_Buf[0]   = 0;
		Info.VAL_Buf[1]   = 0;

		Info.userRequireBack = false;
		CMD_LastState = CMD_State;
		QuadrotorPrj.operation[ QUADROTOR_CMD_PIDQUERYTHROTTLE ](&CMD_State);// [ Block Function ]
	}
	QuadrotorPrj.operation[ QUADROTOR_CMD_PIDTHROTTLE ](NULL);// [ Cyclic Function ]
	Info.TextActive		  = true;
	Info.CursorActive	  = true;
	if(Info.userRequireBack == true){
		uint8_t index = QUADROTOR_CMD_EXIT_PIDTHROTTLE;
		CMD_State &= 0xFFF0FFFF;
		QuadrotorPrj.operation[ index](&index);// [ Block Function ]
	}
	break;
}
/*==========================Îå¼¶×´Ì¬===========================*/
#ifdef PRJ_C_SMARTCAR
case 0x21411000:// C_SmartCar -> NRF -> ALGORITHM -> Image Processor -> Gussian Blur
case 0x22411000:// C_SmartCar -> BLT -> ALGORITHM -> Image Processor -> Gussian Blur
{
	if(CMD_LastState != 0x21411000 && CMD_LastState != 0x22411000)
	{
		OLED_Clean();
		InfoStrucure_Init(&Info);
		Info.TEXT_Num = 3;
		Info.TEXT_Buf[0]  = "Delta    %de-2";
		Info.TEXT_Buf[1]  = "Order    %d";
		Info.TEXT_Buf[2]  = "Save & Back";
		Info.TITLE_Buf	  = "Gussian Blur:";
		Info.Cursor_Pos   = 100; 
		Info.Cursor_Buf   = "<->";
		if(C_SmartCar_GetGusCFG())
		{
			Info.VAL_Buf[0]   = CMD_BOX.Project_C_SmartCar.CMD_Buf[0+3];
			Info.VAL_Buf[1]   = CMD_BOX.Project_C_SmartCar.CMD_Buf[1+3];
		}
		CMD_LastState = CMD_State;
	}
	C_SmartCar_SetGusCFG();
	Info.TextActive		= true;
	Info.CursorActive	= true;
	CMD_LastState = CMD_State;
	break;
}

case 0x21412000:// C_SmartCar -> NRF -> ALGORITHM -> Image Processor -> Binary
case 0x22412000:// C_SmartCar -> BLT -> ALGORITHM -> Image Processor -> Binary
{
	if(CMD_LastState != 0x21412000 && CMD_LastState != 0x22412000)
	{
		OLED_Clean();
		InfoStrucure_Init(&Info);
		Info.TEXT_Num = 3;
		Info.TEXT_Buf[0]  = "Threshold    %d";
		Info.TEXT_Buf[1]  = "Luminance    %de-2";
		Info.TEXT_Buf[2]  = "Save & Back";
		Info.TITLE_Buf	  = "Binary Image:";
		Info.Cursor_Pos   = 100; 
		Info.Cursor_Buf   = "<->";
		if(C_SmartCar_GetBinCFG())
		{
			Info.VAL_Buf[0]   = CMD_BOX.Project_C_SmartCar.CMD_Buf[0+3];
			Info.VAL_Buf[1]   = CMD_BOX.Project_C_SmartCar.CMD_Buf[1+3];
		}
		CMD_LastState = CMD_State;
	}
	C_SmartCar_SetBinCFG();
	Info.TextActive		= true;
	Info.CursorActive	= true;
	CMD_LastState = CMD_State;
	break;
}



case 0x21413000:// C_SmartCar -> NRF -> ALGORITHM -> Image Processor -> Canny Edge
case 0x22413000:// C_SmartCar -> BLT -> ALGORITHM -> Image Processor -> Canny Edge		
{
	if(CMD_LastState != 0x21413000 && CMD_LastState != 0x22413000)
	{
		OLED_Clean();
		InfoStrucure_Init(&Info);
		Info.TEXT_Num = 7;
		Info.TEXT_Buf[0]  = "Gus Delta   %de-2";
		Info.TEXT_Buf[1]  = "Gus Order   %d";
		Info.TEXT_Buf[2]  = "Gus CMD     %d";
		Info.TEXT_Buf[3]  = "Kernal      %d";
		Info.TEXT_Buf[4]  = "H Threshold %d";
		Info.TEXT_Buf[5]  = "L Threshold %d";
		Info.TEXT_Buf[6]  = "Save & Back";
		Info.TITLE_Buf	  = "Canny Edge:";
		Info.Cursor_Pos   = 109; 
		Info.Cursor_Buf   = "<->";
		if(C_SmartCar_GetCannyCFG())
		{
			Info.VAL_Buf[0]   = CMD_BOX.Project_C_SmartCar.CMD_Buf[0+3];
			Info.VAL_Buf[1]   = CMD_BOX.Project_C_SmartCar.CMD_Buf[1+3];
			Info.VAL_Buf[2]   = CMD_BOX.Project_C_SmartCar.CMD_Buf[2+3];
			Info.VAL_Buf[3]   = CMD_BOX.Project_C_SmartCar.CMD_Buf[3+3];
			Info.VAL_Buf[4]   = CMD_BOX.Project_C_SmartCar.CMD_Buf[4+3];
			Info.VAL_Buf[5]   = CMD_BOX.Project_C_SmartCar.CMD_Buf[5+3];
		}
		CMD_LastState = CMD_State;
	}
	C_SmartCar_SetCannyCFG();
	Info.TextActive		= true;
	Info.CursorActive	= true;
	CMD_LastState = CMD_State;
	break;
}
case 0x21414000:// C_SmartCar -> NRF -> ALGORITHM -> Image Processor -> Avg Filter
case 0x22414000:// C_SmartCar -> BLT -> ALGORITHM -> Image Processor -> Avg Filter	
{
	if(CMD_LastState != 0x21414000 && CMD_LastState != 0x22414000)
	{
		OLED_Clean();
		InfoStrucure_Init(&Info);
		Info.TEXT_Num = 2;
		Info.TEXT_Buf[0]  = "Order   %d";
		Info.TEXT_Buf[1]  = "Save & Back";
		Info.TITLE_Buf	  = "Average Filter:";
		Info.Cursor_Pos   = 100; 
		Info.Cursor_Buf   = "<->";
		if(C_SmartCar_Get(Get_Image_AvgFilter))
		{
			Info.VAL_Buf[0]   = CMD_BOX.Project_C_SmartCar.CMD_Buf[0+3];
		}
		CMD_LastState = CMD_State;
	}
	C_SmartCar_SetFilterCFG();
	Info.TextActive		= true;
	Info.CursorActive	= true;
	break;
}
case 0x21415000:// C_SmartCar -> NRF -> ALGORITHM -> Image Processor -> Mid Filter
case 0x22415000:// C_SmartCar -> BLT -> ALGORITHM -> Image Processor -> Mid Filter	
{
	if(CMD_LastState != 0x21415000 && CMD_LastState != 0x22415000)
	{
		OLED_Clean();
		InfoStrucure_Init(&Info);
		Info.TEXT_Num = 2;
		Info.TEXT_Buf[0]  = "Order   %d";
		Info.TEXT_Buf[1]  = "Save & Back";
		Info.TITLE_Buf	  = "Median Filter:";
		Info.Cursor_Pos   = 100; 
		Info.Cursor_Buf   = "<->";
		if(C_SmartCar_Get(Get_Image_MidFilter))
		{
			Info.VAL_Buf[0]   = CMD_BOX.Project_C_SmartCar.CMD_Buf[0+3];
		}
		CMD_LastState = CMD_State;
	}
	C_SmartCar_SetFilterCFG();
	Info.TextActive		= true;
	Info.CursorActive	= true;
	break;
}

		}
		DSP_Info(&Info);
	}
#endif
}
















































