/*
 * Batt.cpp
 *
 * Created: 8/19/2017 7:39:50 PM
 *  Author: orencollaco
 */ 


#include "Batt.h"

Batt::Batt(uint16_t ratedVoltage, uint16_t maxVoltage, uint16_t cutoffVoltage, uint8_t ADC_channel){
	RatedVoltage = ratedVoltage;
	MaxVoltage = maxVoltage;
	CutoffVoltage = cutoffVoltage + 100;
	ADC_Channel = ADC_channel;
	Hysteresis = 100;
    BattLow = CutoffVoltage + 500 - Hysteresis;
	BattCritical = CutoffVoltage - Hysteresis;
	BattOver = MaxVoltage + Hysteresis;
	BattL = 0, BattC = 0, BattO = 0, PowerDownReady = 0;
	count = 0, countA = 0, t = 0;
	ADC_Init(ADMUX_VCC_NOADLAR,ADCSRA_FCPUBY32_NOINTERRUPT_SINGLE);
	ADC_Start();
	Batt_State = NORMAL;
	Set_State_Count = 20;
	Voltage = CutoffVoltage;
	avg = 0;
}

Batt::Batt(){
	
}

void Batt::initialize(){
	Hysteresis = 100;
	BattLow = CutoffVoltage + 500 - Hysteresis;
	BattCritical = CutoffVoltage - Hysteresis;
	BattOver = MaxVoltage + Hysteresis;
	BattL = 0, BattC = 0, BattO = 0, PowerDownReady = 0;
	count = 0, countA = 0, t = 0;
	ADC_Init(ADMUX_VCC_NOADLAR,ADCSRA_FCPUBY32_NOINTERRUPT_SINGLE);
	ADC_ChangeChannelTo(ADC_Channel);
	ADC_Start();
	Batt_State = NORMAL;
	Set_State_Count = 20;
	Voltage = CutoffVoltage;
	avg = 0;
}

void Batt::setParam(uint16_t ratedVoltage, uint16_t maxVoltage, uint16_t cutoffVoltage, uint8_t ADC_channel){
	RatedVoltage = ratedVoltage;
	MaxVoltage = maxVoltage;
	CutoffVoltage = cutoffVoltage + 100;
	ADC_Channel = ADC_channel;
}

uint16_t Batt::getCutoffVoltage(){
	return CutoffVoltage;
}

uint16_t Batt::getMaxVoltage(){
	return MaxVoltage;
}

uint16_t Batt::getRatedVoltage(){
	return RatedVoltage;
}

uint16_t Batt::getVoltage(){
	return Voltage;	
}

uint8_t Batt::getBattState(){
	return Batt_State;
}

void Batt::updateVoltage(){
	//ADC_ChangeChannelTo(ADC_Channel);
	ADC_Start();
	ADC_Read();
	ADC_Start();
	ADC_Read();
	avg = 0;
	for(uint16_t i = 0; i < 2000; i++){
		ADC_Start();
		avg += ADC_Read();
	}
	avg /= 2000;
	#ifndef NO_VOLTAGE_DIVIDER
	Voltage =  avg * (VREF/ADC_RES) * (R1 + R2)/R1 * CALI * 1000;
	#endif
	#ifdef NO_VOLTAGE_DIVIDER
	//ADC_Start();
	Voltage =  VREF * (ADC_RES / avg) * CALI * 1000;
	#endif
}

uint8_t Batt::updateBatt(){
	if(Batt_State != WILL_SHUTDOWN){
		tVoltage = Voltage;
		updateVoltage();
		Voltage = (tVoltage + Voltage)/2;
		count += 1;
		#ifdef STATS
		printStringCRNL("Voltage = ");
		printNumberWithPoint(Voltage,3);
		#endif
	}
	if((Voltage > BattOver) && BattO){
		if(flag == BATTOVER_NO){
			countA += 1;
		}
		else{
			flag = BATTOVER_NO;
			countA = 0;
		}
		if(countA < Set_State_Count)
		return 1;
		else
		countA = 0;
		BattOver = MaxVoltage - Hysteresis;
		#ifdef DEBUG_BATT
		printStringCRNL("Powering Down... See you after voltage returns to Normal.");
		printNumberWithPoint(Voltage,3);
		#endif
		//shutdown();
		Batt_State = WAKE_UP;
		return Batt_State;
	}
	if((Voltage > BattOver) && !BattO){
		if(flag == BATTOVER_NO){
			countA += 1;
		}
		else{
			flag = BATTOVER_NO;
			countA = 0;
		}
		if(countA < Set_State_Count)
			return 1;
		else
			countA = 0;
		BattO = 1;
		BattOver = MaxVoltage - Hysteresis;
		#ifdef DEBUG_BATT
		printStringCRNL("OverVoltage. Going any further can damage the 5v regulator. Voltage = ");
		printNumberWithPoint(Voltage,3);
		#endif
		Batt_State = WILL_SHUTDOWN;
		return Batt_State;
	}
	if((Voltage < BattOver) && BattO){
		if(flag == BATTNORMAL_O){
			countA += 1;
		}
		else{
			flag = BATTNORMAL_O;
			countA = 0;
		}
		if(countA < Set_State_Count)
			return 1;
		else
			countA = 0;
		BattO = 0;
		BattOver = MaxVoltage + Hysteresis;
		#ifdef DEBUG_BATT
		printStringCRNL("Normal Voltage. Voltage = ");
		printNumberWithPoint(Voltage,3);
		#endif
		Batt_State = NORMAL;
		return Batt_State;
	}
	
	if((Voltage < BattOver) && (Voltage > BattLow) && !BattL && !BattC && !BattO){
		if(flag == BATTNORMAL_NL_NC_NO){
			countA += 1;
		}
		else{
			flag = BATTNORMAL_NL_NC_NO;
			countA = 0;
		}
		if(countA < Set_State_Count)
			return 1;
		else
			countA = 0;
		#ifdef DEBUG_BATT
			printStringCRNL("Voltage = ");
			printNumberWithPoint(Voltage,3);
		#endif
		Batt_State = NORMAL;
		return Batt_State;
	}
	if((Voltage < BattLow) && !BattL && !BattC){
		if(flag == BATTLOW_NL_NC){
			countA += 1;
		}
		else{
			flag = BATTLOW_NL_NC;
			countA = 0;
		}
		if(countA < Set_State_Count)
			return 1;
		else
			countA = 0;
		BattL = 1;
		BattLow = CutoffVoltage + 500 + Hysteresis;
		#ifdef DEBUG_BATT
		printStringCRNL("Low Battery. Voltage = ");
		printNumberWithPoint(Voltage,3);
		#endif
		indicateBattLow();
		Batt_State = LOW;
		return Batt_State;
	}
	if((Voltage > BattLow) && BattL && !BattC){
		if(flag == BATTNORMAL_L_NC){
			countA += 1;
		}
		else{
			flag = BATTNORMAL_L_NC;
			countA = 0;
		}
		if(countA < Set_State_Count)
			return 1;
		else
			countA = 0;
		BattL = 0;
		BattLow = CutoffVoltage + 500 - Hysteresis;
		#ifdef DEBUG_BATT
		printStringCRNL("Battery Normal. Voltage = ");
		printNumberWithPoint(Voltage,3);
		#endif
		stopLowBattIndicator();
		Batt_State = NORMAL;
		return Batt_State;
	}
	
	if(BattL && !BattC && (Voltage < BattCritical)){
		if(flag == BATTCRITICAL_L_NC){
			countA += 1;
		}
		else{
			flag = BATTCRITICAL_L_NC;
			countA = 0;
		}
		if(countA < 10)
			return 1;
		else
			countA = 0;
		BattCritical = CutoffVoltage + Hysteresis;
		BattC = 1;
		#ifdef DEBUG_BATT
		printStringCRNL("Battery Critical. Voltage = ");
		printNumberWithPoint(Voltage,3);
		#endif
		Batt_State = WILL_SHUTDOWN;
		return Batt_State;
	}
	
	if(BattL && BattC && (Voltage < BattCritical)){
		if(flag == BATTCRITICAL_NL_NC){
			countA += 1;
		}
		else{
			flag = BATTCRITICAL_NL_NC;
			countA = 0;
		}
		if(countA < 5)
			return 1;
		else
			countA = 0;
		BattCritical = CutoffVoltage + Hysteresis;
		#ifdef DEBUG_BATT
		printStringCRNL("Powering down... See you after a charge :)");
		#endif
		shutdown();
		Batt_State = WAKE_UP;
		return Batt_State;
	}
	
	if(BattL && BattC && (Voltage > BattCritical)){
		if(flag == BATTLOW_L_C){
			countA += 1;
		}
		else{
			flag = BATTLOW_L_C;
			countA = 0;
		}
		if(countA < Set_State_Count)
			return 1;
		else
			countA = 0;
		BattCritical = CutoffVoltage - Hysteresis;
		BattC = 0;
		#ifdef DEBUG_BATT
		printStringCRNL("Battery Low. Voltage = ");
		printNumberWithPoint(Voltage,3);
		#endif
		indicateBattLow();
		Batt_State = LOW;
		return Batt_State;
	}
	return 1;
}

void Batt::indicateBattLow(){
	setLEDIndicatorCount(10);
}

void Batt::stopLowBattIndicator(){
	setLEDIndicatorCount(60);	
}

void Batt::shutdown(){
	stopLEDIndicator();			//Stop Timer
	Voltage = 0;
	while(Voltage < CutoffVoltage){// Voltage > MaxVoltage){
		ADCSRA &= ~(1 << ADEN);		//Stop ADC
		PRR = 0xFF;
		INDICATE_LED = 0;
		powerDownFor(0x06);
		INDICATE_LED = 1;
		powerDownFor(0x02);
		INDICATE_LED = 0;
		powerDownFor(0x06);
		WDT_off();
		PRR &= ~(1 << 0);
		ADCSRA |= (1 << ADEN);		
		ADC_Start();				//Start ADC
		ADC_Read();
		updateVoltage();
	}
	PRR = 0;
	startLEDIndicator();		//Start Timer
}