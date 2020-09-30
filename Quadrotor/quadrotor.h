#ifndef QUADROTOR_H
#define QUADROTOR_H 



#define QUADROTOR_PACKAGE_HEAD 		        0x51
#define QUADROTOR_PACKAGE_ACK               0xac
#define PACKAGE_ACK_INDEX			        (this->bufferSize-1)
#define PACKAGE_FUNC_INDEX			        1

//Package Function:
#define QUADROTOR_CMD_CONNECT		        0x00
#define QUADROTOR_CMD_TESTMOTOR             0x01
#define QUADROTOR_CMD_MOTORQUERY            0x02
#define QUADROTOR_CMD_MOTORSETTING          0x03
#define QUADROTOR_CMD_PIDQUERY		        0x10
#define QUADROTOR_CMD_PIDADJUSTER           0x11
#define QUADROTOR_CMD_PIDTHROTTLE           0x12
#define QUADROTOR_CMD_PIDQUERYTHROTTLE      0x13 
#define QUADROTOR_CMD_SYSTEMFLAGQUERY       0x20                  
#define QUADROTOR_CMD_SYSTEMFLAG            0x21     
#define QUADROTOR_CMD_DISCONNECT	        0xf0
#define QUADROTOR_CMD_KILLMOTOR 	        0xf1

#define QUADROTOR_CMD_EXIT_TESTMOTOR 		0xd1
#define QUADROTOR_CMD_EXIT_MOTORSETTING     0xd2
#define QUADROTOR_CMD_EXIT_PIDADJUSTER		0xe1
#define QUADROTOR_CMD_EXIT_PIDTHROTTLE		0xe2
#define QUADROTOR_CMD_EXIT_SYSTEMFLAG       0xe3


#define MK_FUNC(pArray,index)		        pArray[1]  = index
#define MK_PKGHEAD(pArray)                  pArray[0]  = QUADROTOR_PACKAGE_HEAD
#define MK_PKGACK(pArray)                   pArray[31] = QUADROTOR_PACKAGE_ACK
#define MK_SHAKEHAND(pArray)		        pArray[30] = 0x00
#define RM_SHAKEHAND(pArray)		        pArray[30] = 0xff

#define M1     0U
#define M2     1U
#define M3     2U
#define M4     3U

#include <stdbool.h>
#include <stdint.h>

typedef struct {
 	void (*operation[0xff])(void*);
 	bool connected;
}QuadrotorPrj_t;

struct PID_Limit{
	float p;
	float i;
	float d;
};

typedef struct{
	uint8_t    index;
	float      kp;
	float      ti;
	float      td;
	struct PID_Limit limit;
	bool       outputFlag;
	bool       outputSpeedFlags[4];
	int32_t    averageSpeed;
}PID_t;

typedef struct{
	uint32_t   max_speedLimit;
	uint32_t   min_speedLimit;
}MOTOR_t;

typedef struct{
	bool      enablePIDFlag;
	bool      enableMotorFlag;
	bool      enableLEDFlag;
	bool      enableCameraFlag;
}SystemFlags_t;

extern QuadrotorPrj_t QuadrotorPrj;

//extern struct _quadrotor_project QuadrotorPrj;
void QuadrotorPrj_Init(void);

#endif


