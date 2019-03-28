/*
 * Kernel.h
 *
 * Created: 8/31/2017 1:45:15 PM
 *  Author: orencollaco
 */ 


#ifndef KERNEL_H_
#define KERNEL_H_

#define MODE_MORMAL			00
#define MODE_SLIMIT			01

#define JOY_CENTER			00
#define JOY_UP				01
#define JOY_DOWN			02

#define SLIMIT_STOP			00
#define SLIMIT_FORWARD		01
#define SLIMIT_REVERSE		02

#include "MP6050.h"
#include "NRF24L01.h"
#include "Batt.h"
#include "Motor.h"
#include "PIDControl.h"

class Kernel{
	
private:

	Batt *B;
	NRF24L01 *N;
	Motor *M;
	MP6050 *S;
	PIDControl *P, *Angle;
	uint8_t Batt_State, Switch_State, Old_Switch_State, JoyV, Old_JoyV, Gain_Select, PayLoad_Count;
	uint8_t Joy_State, SpeedLimit, Slimit_State;
	uint16_t Speed;
	uint8_t PayLoad[5], InfoPayLoad[5], Mode;
	uint16_t noPayloadTime;
	bool NewPayload, DualWheel, AngleHold, Link_Lost, FirstRun, DebugMode;
	
public:

	Kernel(Batt* b, NRF24L01 *n, Motor *m, MP6050 *s, PIDControl *p);
	
	void runKernel();
	
	void handleDualWheel();
	
	void handleAngle();
	
	void updatePayload();
	
	void handlePayload();
	
	void userIDecode();
	
	void joyDown();
	
	void joyUp();
	
	void switchCenterToLeft();
	
	void switchCenterToRight();
	
	void switchLeftLong();
	
	void switchRightLong();
	
	void handleSLIMIT();
	
	void printPayload();
	
};





#endif /* KERNEL_H_ */