/*
 * Batt.h
 *
 * Created: 8/19/2017 7:40:03 PM
 *  Author: orencollaco
 */ 


#ifndef BATT_H_
#define BATT_H_

#include "Essential.h"
#include "Power.h"
#include "Timer.h"

#define BATTLOW_NL_NC			01
#define BATTNORMAL_L_NC			02
#define BATTCRITICAL_L_NC		03
#define BATTLOW_L_C				04
#define BATTOVER_NO				05
#define BATTNORMAL_O			06
#define BATTNORMAL_NL_NC_NO		07
#define BATTCRITICAL_NL_NC		10


#define NORMAL				11
#define LOW					12
#define WILL_SHUTDOWN		13
#define WAKE_UP				14

class Batt{

private:
 uint16_t RatedVoltage;
 uint16_t MaxVoltage;
 uint16_t CutoffVoltage;
 uint16_t Voltage;
 uint16_t Set_State_Count;
 uint8_t ADC_Channel;
 uint32_t avg;
 uint16_t BattLow, BattCritical, BattOver, tVoltage;
 uint8_t Hysteresis, count, countA, flag, Batt_State;
 uint16_t t;
 bool BattL, BattC, BattO, PowerDownReady;
 
 public:
 Batt(uint16_t	ratedVoltage, uint16_t maxVoltage, uint16_t cutoffVoltage, uint8_t ADC_channel);
 
 Batt();
 
 void setParam(uint16_t ratedVoltage, uint16_t maxVoltage, uint16_t cutoffVoltage, uint8_t ADC_channel);
 
 void initialize();
 
 uint16_t getRatedVoltage();
 
 uint16_t getMaxVoltage();
 
 uint16_t getCutoffVoltage();
 
 uint16_t getVoltage();
 
 uint8_t getBattState();
 
 void updateVoltage();
 
 uint8_t checkChargeLevel();
 
 uint8_t updateBatt();
 
 void indicateBattLow();
 
 void stopLowBattIndicator();
 
 void shutdown();	
};


#endif /* BATT_H_ */