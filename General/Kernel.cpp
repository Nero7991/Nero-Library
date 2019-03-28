/*
 * Kernel.cpp
 *
 * Created: 8/31/2017 1:44:21 PM
 *  Author: orencollaco
 */ 

#include "Kernel.h"

Kernel::Kernel(Batt* b, NRF24L01 *n, Motor *m, MP6050 *s, PIDControl *p){
	B = b;
	N = n;
	M = m;
	S = s;
	P = p;
	NewPayload = 0;
	noPayloadTime = 0;
	DualWheel = 0;
	AngleHold = 0;
	Gain_Select = 0;
	Link_Lost = 1;
	FirstRun = 1;
	DebugMode = 0;
	PayLoad_Count = 0;
	Mode = MODE_MORMAL;
	Slimit_State = SLIMIT_STOP;
	SpeedLimit = 60;
	INDICATE_LED_D = 1; 
	P->setTarget(9000);
	P->setGain(2.4,1.5,0.2);
	P->setMaxError(3500);
	setLEDIndicatorCount(60);
	S->setClockSource(1);
	_delay_ms(10);
	S->resetSignalPath();
	N->writeACKPayload(InfoPayLoad);
}

void Kernel::runKernel(){
	Batt_State = B->updateBatt();
	switch(Batt_State){
	case LOW:
	case NORMAL:
		handleDualWheel();
		handleAngle();
		updatePayload();
		handlePayload();
		userIDecode();
		if(FirstRun && !Link_Lost){
			FirstRun = 0;
			if(Switch_State){
				DebugMode = 1;
			}
		}
		if(DebugMode){
			N->printInfo();
			printPayload();
		}
	break;
	case WILL_SHUTDOWN:
		M->shutdown();
		if(isI2CGood()){
			S->setClockSource(0);
			_delay_ms(5);
			S->powerDown();
		}
	break;
	case WAKE_UP:
		M->wakeUp();
		if(isI2CGood()){
			S->setClockSource(1);
			S->powerOn();
		}
	break;
	}
}

void Kernel::handleDualWheel(){
	if(DualWheel){
		Speed = P->stayAtTarget(S->readAcceleroZ());
		M->setSpeedDirectionL(Speed);
		M->setSpeedDirectionR(Speed);
	}
}

void Kernel::handleAngle(){
	if(AngleHold){
		//Angle->stayAtTarget(S->readAcceleroX());
	}
}

void Kernel::updatePayload(){
	if(N->isTXFull()){
		N->flushTX();
	}
	if(!N->isRXEmpty()){
		N->readFIFO(PayLoad);
		N->writeACKPayload(InfoPayLoad);
		#ifdef EXTRA_DEBUG
		printStringCRNL("New PayLoad");
		printNumber(millis());
		#endif
		NewPayload = 1;
		noPayloadTime = millis();
		if(Link_Lost){
			PayLoad_Count += 1;
			if(PayLoad_Count > 10){
				#ifdef STATS
				printStringCRNL("Connected!");
				#endif
				Link_Lost = 0;
				PayLoad_Count = 0;
			}
		}
	}
	else{
		if(!Link_Lost){
			if((uint16_t)millis() - noPayloadTime > 200){
				#ifdef STATS
				printStringCRNL("Link Lost");
				#endif
				Link_Lost = 1;
				M->enableBreaksL();
				M->enableBreaksR();
				noPayloadTime = millis();
			}
		}
	}
}

void Kernel::handlePayload(){
	if(NewPayload){
		static uint16_t BattV;
		BattV = B->getVoltage();
		InfoPayLoad[0] = BattV;
		InfoPayLoad[1] = BattV >> 8;
		InfoPayLoad[2] = Mode & 0x03;
		if(!DualWheel && !Link_Lost){
			#ifdef EXTRA_DEBUG
			printString("HandlePayload ");
			#endif
			if(Mode == MODE_MORMAL){
				M->updateMotorSpeed_RearDrive((PayLoad[2] | PayLoad[3] << 8),(PayLoad[0] | PayLoad[1] << 8));
			}
			if(Mode == MODE_SLIMIT){
				M->setSpeedDirectionL(M->offsetForJoy(PayLoad[0] | PayLoad[1] << 8));
				handleSLIMIT();
			}
		}
		NewPayload = 0;
	}
}

void Kernel::userIDecode(){
	Old_Switch_State = Switch_State;
	Switch_State = PayLoad[4];
	if(Switch_State == 2){
		M->enableBreaksR();
	}
	if(!Old_Switch_State && Switch_State == 1){
		switchCenterToLeft();
	}
	if(!Old_Switch_State && Switch_State == 2){
		switchCenterToRight();
	}
	Old_JoyV = JoyV;
	JoyV = M->joyToSwitch((PayLoad[2] | PayLoad[3] << 8));
	switch(JoyV){
		case 0:
		Joy_State = JOY_CENTER;
		break;
		case 1:
		Joy_State = JOY_DOWN;
		break;
		case 2:
		Joy_State = JOY_UP;
		break;
	}	
	if(!Old_JoyV && JoyV == 1){
		joyDown();
	}
	if(!Old_JoyV && JoyV == 2){
		joyUp();
	}
	#ifdef DEBUG_UART
	printString("JoyState :");
	printNumber(Joy_State);
	#endif
}

void Kernel::switchCenterToLeft(){
	Mode ^= 1;
	M->disableBreaksR();
	Slimit_State = SLIMIT_STOP;
}

void Kernel::switchCenterToRight(){
	
}

void Kernel::handleSLIMIT(){
	static int16_t temp;
	static uint8_t time;
	temp = M->getSpeedR();
	#ifdef DEBUG_UART
	printString("SpeedR :");
	printSignedNumber(temp);
	#endif
	
	if(Slimit_State == SLIMIT_STOP){
		if(temp != 0){
			if((uint16_t)millis() - time > 10){
				time = (uint8_t)millis();
				if(temp > 0){
					M->setSpeedDirectionR(temp - 1);
				}
				if(temp < 0){
					M->setSpeedDirectionR(temp + 1);
				}
				return;
			}
		}
	}
	if(Slimit_State == SLIMIT_FORWARD){
		if(temp != SpeedLimit){
			if((uint16_t)millis() - time > 10){
				time = (uint8_t)millis();
				if(temp > SpeedLimit){
					M->setSpeedDirectionR(temp - 1);
				}
				if(temp < SpeedLimit){
					M->setSpeedDirectionR(temp + 1);
				}
				return;
			}
		}
	}
	if(Slimit_State == SLIMIT_REVERSE){
		if(temp != -SpeedLimit){
			if((uint16_t)millis() - time > 10){
				time = (uint8_t)millis();
				if(temp > -SpeedLimit){
					M->setSpeedDirectionR(temp - 1);
				}
				if(temp < -SpeedLimit){
					M->setSpeedDirectionR(temp + 1);
				}
				return;
			}
		}
	}
	
}

void Kernel::joyUp(){
	if(Mode == MODE_SLIMIT){
		if(Slimit_State == SLIMIT_STOP){
			Slimit_State = SLIMIT_FORWARD;
			return;
		}
		if(Slimit_State == SLIMIT_REVERSE){
			Slimit_State = SLIMIT_STOP;
			return;
		}
	}
}

void Kernel::joyDown(){
	if(Mode == MODE_SLIMIT){
		if(Slimit_State == SLIMIT_STOP){
			Slimit_State = SLIMIT_REVERSE;
			return;
		}
		if(Slimit_State == SLIMIT_FORWARD){
			Slimit_State = SLIMIT_STOP;
			return;
		}
	}
}

void Kernel::printPayload(){
		printStringCRNL("ADC = ");
		printNumber(PayLoad[0] | (PayLoad[1] << 8));
		printNumber(PayLoad[2] | (PayLoad[3] << 8));
		printString(" Data = ");
		printNumber(PayLoad[4]);
}