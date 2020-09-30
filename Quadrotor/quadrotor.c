#include "quadrotor.h"
#include "NRF24L01.h"

#include <stdint.h>
#include <stdio.h>
#include <math.h>
#include "info.h"
#include "utility.h"

QuadrotorPrj_t QuadrotorPrj;
static uint8_t txBuffer[32] = {0};
static uint8_t rxBuffer[32] = {0};
static bool    feedbackFlag = false;
static PID_t   PID[5] = {{.index = 0},{.index = 1},{.index = 2},{.index = 3}};
//                         Thrust       Pitch        Roll         Yaw    
static bool    PID_outputSpeedFlags[4];

static MOTOR_t       Motor;
static SystemFlags_t SystemFlags;

static bool Quad_RadioOperation(void){
	//size_t cnt = 0;
	while(NRF_TX_Send(txBuffer) != TX_OK){
		//cnt++;
		//if(cnt > 2000){
		//	QuadrotorPrj.connected = false;
		//	isDone = true;
		//	break;
		//
	}
	if(feedbackFlag == true){
		feedbackFlag = false;
		NRF_RX_Mode();
		//cnt = 0;
		delay_ms(1);
		while(NRF_RX_Receive(rxBuffer) != 0){
			//cnt++;
			//if(cnt > 2000){
			//	QuadrotorPrj.connected = false;
			//	isDone = true;
			//	break;
			//
		}
	}
	NRF_TX_Mode();
	return true;
}

static void Quad_Connect(void* param){
	bool isDone = false;
	memset(txBuffer,'\0',32);
	memset(rxBuffer,'\0',32);
	txBuffer[0]    = QUADROTOR_PACKAGE_HEAD;
	txBuffer[1]    = QUADROTOR_CMD_CONNECT;
	txBuffer[2]    = 'Q';
	txBuffer[3]    = 'U';
	txBuffer[4]    = 'A';
	txBuffer[5]    = 'D';
	txBuffer[6]    = 'R';
	txBuffer[7]    = 'O';
	txBuffer[8]    = 'T';
	txBuffer[9]    = 'O';
	txBuffer[10]   = 'R';
	feedbackFlag   = true;
	QuadrotorPrj.connected = true;

	if(Quad_RadioOperation() == false){
		QuadrotorPrj.connected = false;
		isDone = true;
	}
	
	if((rxBuffer[0] != QUADROTOR_PACKAGE_HEAD||rxBuffer[1] != QUADROTOR_CMD_CONNECT) && isDone == false){
		QuadrotorPrj.connected = false;
		isDone = true;
	}

	if(isDone == false){
		for(uint8_t i=2;i<=10;i++){
			if(rxBuffer[i] != txBuffer[10+2-i]){
				QuadrotorPrj.connected = false;
				break;
			}
		}
	}
	

}

static void Quad_Disconnect(void* param){
	size_t cnt = 0;
	memset(txBuffer,'\0',32);
	memset(rxBuffer,'\0',32);
	txBuffer[0]    = QUADROTOR_PACKAGE_HEAD;
	txBuffer[1]	   = QUADROTOR_CMD_DISCONNECT;
	QuadrotorPrj.connected = false;
	while(NRF_TX_Send(txBuffer) != TX_OK){
		cnt++;
		if(cnt > 2000){
			QuadrotorPrj.connected = true;
		}
	}

}

#include "info.h"
static void Quad_TestMotor(void* param){

	static uint16_t value[5] = {0};
	const  uint8_t step[]   = {1,5,10,20,50,100};
	static uint8_t step_selection = 0;
	// Record value of each motor.
	// value[0] --- ALL
	// value[1] --- M1
	// value[2] --- M2
	// value[3] --- M3
	// value[4] --- M4

	static const uint16_t value_max = 4096;
	static const uint16_t value_min = 0;
	static uint32_t last_cmd_state = 0;

	uint32_t* cmd_state = (uint32_t*)param;

	uint8_t motor_index = (uint8_t)((((*cmd_state)>>16)&0xf)%5);// 0x311 [1~4] 0000 --> motor_index = 1~4;
	                                                    		// 0x311 [5]   0000 --> motor_index = 0;

	memset(txBuffer,'\0',32);
	MK_PKGHEAD(txBuffer);
	MK_PKGACK(txBuffer); 
	MK_FUNC(txBuffer,QUADROTOR_CMD_TESTMOTOR);

	if( last_cmd_state != *cmd_state ){
		MK_SHAKEHAND(txBuffer);
		while(Quad_RadioOperation() != true);
		last_cmd_state = *cmd_state;
		return;
	}
	RM_SHAKEHAND(txBuffer);
	txBuffer[2] = motor_index;
	txBuffer[5] = (uint8_t)(value_min&0xff);
	txBuffer[6] = (uint8_t)(value_min>>8);
	txBuffer[7] = (uint8_t)(value_max&0xff);
	txBuffer[8] = (uint8_t)(value_max>>8);

	if(JoyStick_Data[X]>4000){
		switch(CMD_Temp){
		case 0://Speed
			if(value[motor_index]  < 4096)
				value[motor_index] += step[step_selection];
			break;

		case 3://STEP
			if(step_selection < (sizeof(step)/sizeof(*step)-1)) 
				step_selection++;
			break;
		}
		delay_ms(50);
	}
	if(JoyStick_Data[X]<100){
		switch(CMD_Temp){
		case 0://Speed
			if(value[motor_index]  >= step[step_selection])
				value[motor_index] -= step[step_selection];
			break;
		case 3://STEP
			if(step_selection > 0) 
				step_selection--;
			break;
		}
		delay_ms(50);
	}
	
	if(Info.userConfirm == true){
		txBuffer[3] = (uint8_t)(value[motor_index]&0xff);
		txBuffer[4] = (uint8_t)(value[motor_index]>>8);
		feedbackFlag = false;
		while(Quad_RadioOperation() != true);
		Info.userConfirm = false;
	}
	
	Info.VAL_Buf[0] = value[motor_index];
	Info.VAL_Buf[3] = step[step_selection];
	
}

static void Quad_MotorGetParam(void* param){
	memset(txBuffer,'\0',32);
	MK_PKGHEAD(txBuffer);
	MK_PKGACK(txBuffer); 
	MK_FUNC(txBuffer,QUADROTOR_CMD_MOTORQUERY);

 	feedbackFlag = true;
	if(Quad_RadioOperation() == false){
		QuadrotorPrj.connected = false;
		return;
	}

	Motor.max_speedLimit = Info.VAL_Buf[0] = (uint32_t)((rxBuffer[3]<<8)|rxBuffer[2]);//Max Limit
	Info.VAL_Buf[1] = 9;//Min Limit
	Info.VAL_Buf[2] = 9;//Speed Range
	Info.VAL_Buf[3] = 9;//PWM Duty
	Info.VAL_Buf[4] = 9;//PWM CNT_16Bit

}

static void Quad_MotorSetParam(void* param){
	memset(txBuffer,'\0',32);
	MK_PKGHEAD(txBuffer);
	MK_PKGACK(txBuffer); 
	MK_FUNC(txBuffer,QUADROTOR_CMD_MOTORSETTING);

	const uint8_t step[] = {1,5,10,20};
	static size_t step_selection = 0;

	if(JoyStick_Data[X]>4000){
		switch(CMD_Temp){
		case 0: Motor.max_speedLimit += step[step_selection];
				break;
		case 1: Motor.min_speedLimit += step[step_selection];
				break;
		case 2: break;
		case 3: break;
		case 4: break;
		case 5: if(step_selection < (sizeof(step)/sizeof(*step)-1)) 
					step_selection++;
				break;
		}
		delay_ms(50);
	}

	if(JoyStick_Data[X]<100){
		switch(CMD_Temp){
		case 0: if(Motor.max_speedLimit > step[step_selection])
					Motor.max_speedLimit -= step[step_selection];
				break;
		case 1: if(Motor.min_speedLimit > step[step_selection])
					Motor.min_speedLimit -= step[step_selection];
				break;
		case 2: break;
		case 3: break;
		case 4: break;
		case 5: if(step_selection > 0) 
					step_selection--;
				break;
		}
		delay_ms(50);
	}

	txBuffer[2] = (uint8_t)Motor.max_speedLimit;
	txBuffer[3] = (uint8_t)(Motor.max_speedLimit >> 8);

	txBuffer[4] = (uint8_t)Motor.min_speedLimit;
	txBuffer[5] = (uint8_t)(Motor.min_speedLimit >> 8);

	Info.VAL_Buf[0] = Motor.max_speedLimit;
	Info.VAL_Buf[1] = Motor.min_speedLimit;
	// Info.VAL_Buf[2]
	// Info.VAL_Buf[3]
	// Info.VAL_Buf[4]
	Info.VAL_Buf[5] = step[step_selection];

	if(Info.userConfirm == true){
		while(Quad_RadioOperation() != true);
		Info.userConfirm = false;
	}
	
}

static void Quad_GetPIDParam(void* param){
	uint32_t* cmd_state = (uint32_t*)param;
	bool isDone = false;

	struct Note xxxx_10E;

	memset(txBuffer,'\0',32);
	memset(rxBuffer,'\0',32);
	MK_PKGHEAD(txBuffer);
	MK_PKGACK(txBuffer); 
	MK_FUNC(txBuffer,QUADROTOR_CMD_PIDQUERY);

	switch(*cmd_state){
		case 0x31410000:txBuffer[2] = 1;break;	// Pitch
		case 0x31420000:txBuffer[2] = 2;break;	// Roll 
		case 0x31430000:txBuffer[2] = 3;break;	// Yaw
		case 0x31440000:txBuffer[2] = 0;break;	// Thrust
	}
	
	feedbackFlag   = true;
	QuadrotorPrj.connected = true;

	if(Quad_RadioOperation() == false){
		QuadrotorPrj.connected = false;
		isDone = true;
	}

	if(isDone == false){

		xxxx_10E.value = (double)((rxBuffer[5]<<8)|rxBuffer[4]);
		xxxx_10E.E     = (signed char)rxBuffer[3];
		Info.VAL_FLOAT_Buf[0] = PID[txBuffer[2]].kp = DE_NOTATION(&xxxx_10E);

		xxxx_10E.value = (double)((rxBuffer[8]<<8)|rxBuffer[7]);
		xxxx_10E.E     = (signed char)rxBuffer[6];
		Info.VAL_FLOAT_Buf[1] = PID[txBuffer[2]].ti = DE_NOTATION(&xxxx_10E);

		xxxx_10E.value = (double)((rxBuffer[11]<<8)|rxBuffer[10]);
		xxxx_10E.E     = (signed char)rxBuffer[9];
		Info.VAL_FLOAT_Buf[2] = PID[txBuffer[2]].td = DE_NOTATION(&xxxx_10E);

		Info.VAL_FLOAT_Buf[4] = PID[txBuffer[2]].limit.p = ((rxBuffer[13]<<8)|rxBuffer[12]);
		Info.VAL_FLOAT_Buf[5] = PID[txBuffer[2]].limit.i = ((rxBuffer[15]<<8)|rxBuffer[14]);
		Info.VAL_FLOAT_Buf[6] = PID[txBuffer[2]].limit.d = ((rxBuffer[17]<<8)|rxBuffer[16]);

		PID[txBuffer[2]].outputFlag           = (bool)(rxBuffer[24]);  
		PID[txBuffer[2]].outputSpeedFlags[M1] = (bool)(rxBuffer[25]);
		PID[txBuffer[2]].outputSpeedFlags[M2] = (bool)(rxBuffer[26]);
		PID[txBuffer[2]].outputSpeedFlags[M3] = (bool)(rxBuffer[27]);
		PID[txBuffer[2]].outputSpeedFlags[M4] = (bool)(rxBuffer[28]);
	}
}

static void Quad_AdjustPID(void* param){
	uint32_t* cmd_state = (uint32_t*)param;
	
	

	static size_t step_selection = 1;// This is an index of step[].Default step is 0.01 which is equal to step[1]
	const float   step[] = {0.001,0.01,0.1,1.0,10.0,100.0};
	
	memset(txBuffer,'\0',32);
	MK_PKGHEAD(txBuffer);
	MK_PKGACK(txBuffer); 
	MK_FUNC(txBuffer,QUADROTOR_CMD_PIDADJUSTER);

	switch(*cmd_state){
		case 0x31410000:txBuffer[2] = 1;break;	// Pitch
		case 0x31420000:txBuffer[2] = 2;break;	// Roll 
		case 0x31430000:txBuffer[2] = 3;break;	// Yaw
		case 0x31440000:txBuffer[2] = 0;break;	// Thrust
	}
	if(JoyStick_Data[X]>4000){
		switch(CMD_Temp){
			case 0: PID[txBuffer[2]].kp += step[step_selection];break;
			case 1: PID[txBuffer[2]].ti += step[step_selection];break;
			case 2: PID[txBuffer[2]].td += step[step_selection];break; 
			case 3: if(step_selection < (sizeof(step)/sizeof(*step)-1)) step_selection++;break;
			case 4: PID[txBuffer[2]].limit.p += step[step_selection];break;
			case 5: PID[txBuffer[2]].limit.i += step[step_selection];break;
			case 6: PID[txBuffer[2]].limit.d += step[step_selection];break;
			case 7: PID[txBuffer[2]].outputFlag = !PID[txBuffer[2]].outputFlag;delay_ms(50);break;
			case 8: PID[txBuffer[2]].outputSpeedFlags[M1] = !PID[txBuffer[2]].outputSpeedFlags[M1];delay_ms(50);break;
			case 9: PID[txBuffer[2]].outputSpeedFlags[M2] = !PID[txBuffer[2]].outputSpeedFlags[M2];delay_ms(50);break;
			case 10:PID[txBuffer[2]].outputSpeedFlags[M3] = !PID[txBuffer[2]].outputSpeedFlags[M3];delay_ms(50);break;
			case 11:PID[txBuffer[2]].outputSpeedFlags[M4] = !PID[txBuffer[2]].outputSpeedFlags[M4];delay_ms(50);break;
		}
		delay_ms(50);
	}
	if(JoyStick_Data[X]<100){
		switch(CMD_Temp){
			case 0:	if(PID[txBuffer[2]].kp>step[step_selection]) 
						PID[txBuffer[2]].kp -= step[step_selection];
					else 
						PID[txBuffer[2]].kp = 0.0;break;

			case 1:	if(PID[txBuffer[2]].ti>step[step_selection]) 
						PID[txBuffer[2]].ti -= step[step_selection];
					else 
						PID[txBuffer[2]].ti = 0.0;break;

			case 2:	if(PID[txBuffer[2]].td>step[step_selection]) 
						PID[txBuffer[2]].td -= step[step_selection];
					else 
						PID[txBuffer[2]].td = 0.0;break;

			case 3:	if(step_selection != 0) 
						step_selection--;break;

			case 4: if( PID[txBuffer[2]].limit.p > step[step_selection]) 
						PID[txBuffer[2]].limit.p -= step[step_selection];
					else 
						PID[txBuffer[2]].limit.p = 0.0;break;

			case 5: if( PID[txBuffer[2]].limit.i > step[step_selection]) 
						PID[txBuffer[2]].limit.i -= step[step_selection];
					else 
						PID[txBuffer[2]].limit.i = 0.0;break;

			case 6: if( PID[txBuffer[2]].limit.d > step[step_selection]) 
						PID[txBuffer[2]].limit.d -= step[step_selection];
					else 
						PID[txBuffer[2]].limit.d = 0.0;break;

			case 7: PID[txBuffer[2]].outputFlag = !PID[txBuffer[2]].outputFlag;delay_ms(50);break;
			case 8: PID[txBuffer[2]].outputSpeedFlags[M1] = !PID[txBuffer[2]].outputSpeedFlags[M1];delay_ms(50);break;
			case 9: PID[txBuffer[2]].outputSpeedFlags[M2] = !PID[txBuffer[2]].outputSpeedFlags[M2];delay_ms(50);break;
			case 10:PID[txBuffer[2]].outputSpeedFlags[M3] = !PID[txBuffer[2]].outputSpeedFlags[M3];delay_ms(50);break;
			case 11:PID[txBuffer[2]].outputSpeedFlags[M4] = !PID[txBuffer[2]].outputSpeedFlags[M4];delay_ms(50);break;
		}
		delay_ms(50);
	}

	struct Note xxxx_10E;

	xxxx_10E = MK_NOTATION(PID[txBuffer[2]].kp,4);
	txBuffer[3] = (uint8_t)xxxx_10E.E;
	txBuffer[4] = (uint8_t)xxxx_10E.value;
	txBuffer[5] = ((uint16_t)(xxxx_10E.value))>>8;

	xxxx_10E = MK_NOTATION(PID[txBuffer[2]].ti,4);
	txBuffer[6] = (uint8_t)xxxx_10E.E;
	txBuffer[7] = (uint8_t)xxxx_10E.value;
	txBuffer[8] = ((uint16_t)(xxxx_10E.value))>>8;

	xxxx_10E = MK_NOTATION(PID[txBuffer[2]].td,4);
	txBuffer[9] = (uint8_t)xxxx_10E.E;
	txBuffer[10] = (uint8_t)xxxx_10E.value;
	txBuffer[11] = ((uint16_t)(xxxx_10E.value))>>8;

	txBuffer[12] = (uint8_t)PID[txBuffer[2]].limit.p;
	txBuffer[13] = ((uint16_t)(PID[txBuffer[2]].limit.p)>>8);

	txBuffer[14] = (uint8_t)PID[txBuffer[2]].limit.i;
	txBuffer[15] = ((uint16_t)(PID[txBuffer[2]].limit.i)>>8);

	txBuffer[16] = (uint8_t)PID[txBuffer[2]].limit.d;
	txBuffer[17] = ((uint16_t)(PID[txBuffer[2]].limit.d)>>8);

	txBuffer[24] = (uint8_t)PID[txBuffer[2]].outputFlag;
	txBuffer[25] = (uint8_t)PID[txBuffer[2]].outputSpeedFlags[M1];
	txBuffer[26] = (uint8_t)PID[txBuffer[2]].outputSpeedFlags[M2];
	txBuffer[27] = (uint8_t)PID[txBuffer[2]].outputSpeedFlags[M3];
	txBuffer[28] = (uint8_t)PID[txBuffer[2]].outputSpeedFlags[M4];

	Info.VAL_FLOAT_Buf[0]   = PID[txBuffer[2]].kp;
	Info.VAL_FLOAT_Buf[1]   = PID[txBuffer[2]].ti;
	Info.VAL_FLOAT_Buf[2]   = PID[txBuffer[2]].td;
	Info.VAL_FLOAT_Buf[3]   = step[step_selection];
	Info.VAL_FLOAT_Buf[4]   = PID[txBuffer[2]].limit.p;
	Info.VAL_FLOAT_Buf[5]   = PID[txBuffer[2]].limit.i;
	Info.VAL_FLOAT_Buf[6]   = PID[txBuffer[2]].limit.d;

	(PID[txBuffer[2]].outputFlag ==true)?(Info.TEXT_Buf[7] ="Module   ON ") : (Info.TEXT_Buf[7] ="Module   OFF");

	(PID[txBuffer[2]].outputSpeedFlags[M1]==true)?(Info.TEXT_Buf[8] ="Motor 1  ON "):(Info.TEXT_Buf[8] ="Motor 1  OFF");
	(PID[txBuffer[2]].outputSpeedFlags[M2]==true)?(Info.TEXT_Buf[9] ="Motor 2  ON "):(Info.TEXT_Buf[9] ="Motor 2  OFF");
	(PID[txBuffer[2]].outputSpeedFlags[M3]==true)?(Info.TEXT_Buf[10]="Motor 3  ON "):(Info.TEXT_Buf[10]="Motor 3  OFF");
	(PID[txBuffer[2]].outputSpeedFlags[M4]==true)?(Info.TEXT_Buf[11]="Motor 4  ON "):(Info.TEXT_Buf[11]="Motor 4  OFF");

	feedbackFlag = false;

	if(Info.userConfirm == true){
		Info.userConfirm = false;
		while(Quad_RadioOperation() != true);
	}
}

static void Quad_PIDGetThrottleParam(void* param){
	bool isDone = false;

	memset(txBuffer,'\0',32);
	MK_PKGHEAD(txBuffer);
	MK_PKGACK(txBuffer); 
	MK_FUNC(txBuffer,QUADROTOR_CMD_PIDQUERYTHROTTLE);

	feedbackFlag   = true;

	if(Quad_RadioOperation() == false){
		QuadrotorPrj.connected = false;
		isDone = true;
	}

	if(isDone != true){
		Info.VAL_Buf[0] = PID[0].averageSpeed  = (float)((rxBuffer[4]<<8)|rxBuffer[3]);
		Info.VAL_Buf[1] = (uint16_t)(PID[0].averageSpeed*100/4096);

		PID_outputSpeedFlags[M1] = (bool)rxBuffer[5];
		PID_outputSpeedFlags[M2] = (bool)rxBuffer[6];
		PID_outputSpeedFlags[M3] = (bool)rxBuffer[7];
		PID_outputSpeedFlags[M4] = (bool)rxBuffer[8];
		(PID_outputSpeedFlags[M1]==true)?(Info.TEXT_Buf[3] = "Motor 1      on "):(Info.TEXT_Buf[3] = "Motor 1      off");
		(PID_outputSpeedFlags[M2]==true)?(Info.TEXT_Buf[4] = "Motor 2      on "):(Info.TEXT_Buf[4] = "Motor 2      off");
		(PID_outputSpeedFlags[M3]==true)?(Info.TEXT_Buf[5] = "Motor 3      on "):(Info.TEXT_Buf[5] = "Motor 3      off");
		(PID_outputSpeedFlags[M4]==true)?(Info.TEXT_Buf[6] = "Motor 4      on "):(Info.TEXT_Buf[6] = "Motor 4      off");
	}

}

static void Quad_SystemGetFlag(void* param){
	memset(txBuffer,'\0',32);
	MK_PKGHEAD(txBuffer);
	MK_PKGACK(txBuffer); 
	MK_FUNC(txBuffer,QUADROTOR_CMD_SYSTEMFLAGQUERY);

	feedbackFlag   = true;
	QuadrotorPrj.connected = true;

	if(Quad_RadioOperation() == false){
		QuadrotorPrj.connected = false;
		return;
	}

	SystemFlags.enablePIDFlag    = (bool)rxBuffer[2];
	SystemFlags.enableMotorFlag  = (bool)rxBuffer[3];
	SystemFlags.enableLEDFlag    = (bool)rxBuffer[4];
	SystemFlags.enableCameraFlag = (bool)rxBuffer[5];

	(SystemFlags.enableMotorFlag  == true) ? (Info.TEXT_Buf[0] = "enMotor    ON "):(Info.TEXT_Buf[0] = "enMotor    OFF");
	(SystemFlags.enablePIDFlag    == true) ? (Info.TEXT_Buf[1] = "enPID      ON "):(Info.TEXT_Buf[1] = "enPID      OFF");
	(SystemFlags.enableLEDFlag    == true) ? (Info.TEXT_Buf[2] = "enLED      ON "):(Info.TEXT_Buf[2] = "enLED      OFF");
	(SystemFlags.enableCameraFlag == true) ? (Info.TEXT_Buf[3] = "enCamera   ON "):(Info.TEXT_Buf[3] = "enCamera   OFF");	

}

static void Quad_SystemSetFlag(void* param){
	memset(txBuffer,'\0',32);
	MK_PKGHEAD(txBuffer);
	MK_PKGACK(txBuffer); 
	MK_FUNC(txBuffer,QUADROTOR_CMD_SYSTEMFLAG);

	if(JoyStick_Data[X]>4000 || JoyStick_Data[X]<100){
		switch(CMD_Temp){
			case 0: SystemFlags.enableMotorFlag  = !SystemFlags.enableMotorFlag;
					break;
			case 1: SystemFlags.enablePIDFlag    = !SystemFlags.enablePIDFlag;
					break;
			case 2: SystemFlags.enableLEDFlag    = !SystemFlags.enableLEDFlag;
					break;
			case 3:	SystemFlags.enableCameraFlag = !SystemFlags.enableCameraFlag;
					break;
		}
		delay_ms(50);
	}

	txBuffer[2] = SystemFlags.enablePIDFlag;
	txBuffer[3] = SystemFlags.enableMotorFlag;
	txBuffer[4] = SystemFlags.enableLEDFlag;
	txBuffer[5] = SystemFlags.enableCameraFlag;

	(SystemFlags.enableMotorFlag  == true) ? (Info.TEXT_Buf[0] = "enMotor    ON "):(Info.TEXT_Buf[0] = "enMotor    OFF");
	(SystemFlags.enablePIDFlag    == true) ? (Info.TEXT_Buf[1] = "enPID      ON "):(Info.TEXT_Buf[1] = "enPID      OFF");
	(SystemFlags.enableLEDFlag    == true) ? (Info.TEXT_Buf[2] = "enLED      ON "):(Info.TEXT_Buf[2] = "enLED      OFF");
	(SystemFlags.enableCameraFlag == true) ? (Info.TEXT_Buf[3] = "enCamera   ON "):(Info.TEXT_Buf[3] = "enCamera   OFF");

	if(Info.userConfirm == true){
		while(Quad_RadioOperation() != true);
		Info.userConfirm = false;
	}
}

static void Quad_ThrottlePID(void* param){
	const uint16_t step[] = {1,5,10,20};
	static size_t  step_selection = 0;// defualt:step[0]


	memset(txBuffer,'\0',32);
	MK_PKGHEAD(txBuffer);
	MK_PKGACK(txBuffer); 
	MK_FUNC(txBuffer,QUADROTOR_CMD_PIDTHROTTLE);

	if(param != NULL){
		MK_SHAKEHAND(txBuffer);
		while(Quad_RadioOperation() != true);
		return;
	}

	RM_SHAKEHAND(txBuffer);

	// The valid value of throttle should be between 500 and 2048.
	if(JoyStick_Data[X]>4000){
		

		switch(CMD_Temp){
			case 0: // Throttle setting
			case 1:	// Throttle setting
					if(PID[0].averageSpeed < 2048)
						PID[0].averageSpeed+=step[step_selection];
					break;
			case 2:	if(step_selection < (sizeof(step)/sizeof(*step)-1)) 
						step_selection++;
					break;
			case 3: PID_outputSpeedFlags[M1] = !PID_outputSpeedFlags[M1];
					break;
			case 4: PID_outputSpeedFlags[M2] = !PID_outputSpeedFlags[M2];
					break;
			case 5: PID_outputSpeedFlags[M3] = !PID_outputSpeedFlags[M3];
					break;
			case 6: PID_outputSpeedFlags[M4] = !PID_outputSpeedFlags[M4];
					break;
		}
		delay_ms(50);
	}

	if(JoyStick_Data[X]<100){
		switch(CMD_Temp){
			case 0: // Throttle setting
			case 1:	// Throttle setting
					if(PID[0].averageSpeed > 500)
						PID[0].averageSpeed-=step[step_selection];
					break;
			case 2: if(step_selection != 0) 
						step_selection--;
					break;
			case 3: PID_outputSpeedFlags[M1] = !PID_outputSpeedFlags[M1];
					break;
			case 4: PID_outputSpeedFlags[M2] = !PID_outputSpeedFlags[M2];
					break;
			case 5: PID_outputSpeedFlags[M3] = !PID_outputSpeedFlags[M3];
					break;
			case 6: PID_outputSpeedFlags[M4] = !PID_outputSpeedFlags[M4];
					break;
		}

		delay_ms(50);
	}

	txBuffer[1] = QUADROTOR_CMD_PIDTHROTTLE;
	txBuffer[2] = 0;
	txBuffer[3] = (uint8_t)(PID[0].averageSpeed);
	txBuffer[4] = (uint8_t)((uint16_t)(PID[0].averageSpeed)>>8);
	txBuffer[5] = (uint8_t)PID_outputSpeedFlags[M1];
	txBuffer[6] = (uint8_t)PID_outputSpeedFlags[M2];
	txBuffer[7] = (uint8_t)PID_outputSpeedFlags[M3];
	txBuffer[8] = (uint8_t)PID_outputSpeedFlags[M4];

	Info.VAL_Buf[0] = (uint16_t)PID[0].averageSpeed;
	Info.VAL_Buf[1] = (uint16_t)(PID[0].averageSpeed*100/4096);
	Info.VAL_Buf[2] = step[step_selection];

	(PID_outputSpeedFlags[M1]==true)?(Info.TEXT_Buf[3] = "Motor 1      on "):(Info.TEXT_Buf[3] = "Motor 1      off");
	(PID_outputSpeedFlags[M2]==true)?(Info.TEXT_Buf[4] = "Motor 2      on "):(Info.TEXT_Buf[4] = "Motor 2      off");
	(PID_outputSpeedFlags[M3]==true)?(Info.TEXT_Buf[5] = "Motor 3      on "):(Info.TEXT_Buf[5] = "Motor 3      off");
	(PID_outputSpeedFlags[M4]==true)?(Info.TEXT_Buf[6] = "Motor 4      on "):(Info.TEXT_Buf[6] = "Motor 4      off");

	feedbackFlag = false;

	if(Info.userConfirm == true){

		if(CMD_Temp == 8)//Kill Motor
			txBuffer[1]  = QUADROTOR_CMD_KILLMOTOR;
		while(Quad_RadioOperation() != true);
		Info.userConfirm = false;
	}
}

static void Quad_ExitSetting(void* param){
	memset(txBuffer,'\0',32);
	MK_PKGHEAD(txBuffer);
	MK_PKGACK(txBuffer); 
	MK_FUNC( txBuffer, *(uint8_t*)param );
	while(Quad_RadioOperation() != true);
}

static void Quad_ExitMotorTest(void* param){
	memset(txBuffer,'\0',32);
	MK_PKGHEAD(txBuffer);
	MK_PKGACK(txBuffer); 
	MK_FUNC(txBuffer,QUADROTOR_CMD_EXIT_TESTMOTOR );

	uint32_t* cmd_state = (uint32_t*)param;
	txBuffer[2] = (uint8_t)((((*cmd_state)>>16)&0xf)%5);// 0x311 [1~4] 0000 --> motor_index = 1~4;
	                                                    // 0x311 [5]   0000 --> motor_index = 0;
	txBuffer[3] = *((uint32_t*)param+1);                // 0: Don't do any thing; 1: Turn off sleceted motor;
	while(Quad_RadioOperation() != true);
}

void QuadrotorPrj_Init(void){
	QuadrotorPrj.operation[ QUADROTOR_CMD_CONNECT          ]   = Quad_Connect; 
	QuadrotorPrj.operation[ QUADROTOR_CMD_TESTMOTOR        ]   = Quad_TestMotor; 
	QuadrotorPrj.operation[ QUADROTOR_CMD_MOTORQUERY       ]   = Quad_MotorGetParam;
	QuadrotorPrj.operation[ QUADROTOR_CMD_MOTORSETTING     ]   = Quad_MotorSetParam;
	QuadrotorPrj.operation[ QUADROTOR_CMD_DISCONNECT       ]   = Quad_Disconnect; 
	QuadrotorPrj.operation[ QUADROTOR_CMD_PIDQUERY         ]   = Quad_GetPIDParam; 
	QuadrotorPrj.operation[ QUADROTOR_CMD_PIDADJUSTER      ]   = Quad_AdjustPID; 
	QuadrotorPrj.operation[ QUADROTOR_CMD_PIDTHROTTLE      ]   = Quad_ThrottlePID; 
	QuadrotorPrj.operation[ QUADROTOR_CMD_PIDQUERYTHROTTLE ]   = Quad_PIDGetThrottleParam; 
	QuadrotorPrj.operation[ QUADROTOR_CMD_SYSTEMFLAGQUERY  ]   = Quad_SystemGetFlag;
	QuadrotorPrj.operation[ QUADROTOR_CMD_SYSTEMFLAG       ]   = Quad_SystemSetFlag;

	QuadrotorPrj.operation[ QUADROTOR_CMD_EXIT_PIDTHROTTLE ]   = Quad_ExitSetting; 
	QuadrotorPrj.operation[ QUADROTOR_CMD_EXIT_PIDADJUSTER ]   = Quad_ExitSetting; 
	QuadrotorPrj.operation[ QUADROTOR_CMD_EXIT_MOTORSETTING]   = Quad_ExitSetting;
	QuadrotorPrj.operation[ QUADROTOR_CMD_EXIT_SYSTEMFLAG  ]   = Quad_ExitSetting;	
	QuadrotorPrj.operation[ QUADROTOR_CMD_EXIT_TESTMOTOR   ]   = Quad_ExitMotorTest;
}














