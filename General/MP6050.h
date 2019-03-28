/*
 * MP6050.h
 *
 * Created: 8/20/2017 11:45:44 AM
 *  Author: orencollaco
 */ 


#ifndef MP6050_H_
#define MP6050_H_

#define CONFIG				 0x1A
#define GYRO_CONFIG			 0x1B
#define ACCEL_CONFIG		 0x1C
#define ACCEL_XOUT_H		 0x3B
#define ACCEL_XOUT_L		 0x3C
#define ACCEL_YOUT_H		 0x3D
#define ACCEL_YOUT_L		 0x3E
#define ACCEL_ZOUT_H		 0x3F
#define ACCEL_ZOUT_L		 0x40
#define TEMP_OUT_H			 0x41
#define TEMP_OUT_L			 0x42
#define GYRO_XOUT_H			 0x43
#define GYRO_XOUT_L			 0x44
#define GYRO_YOUT_H			 0x45
#define GYRO_YOUT_L			 0x46
#define GYRO_ZOUT_H			 0x47
#define GYRO_ZOUT_L			 0x48
#define SIGNAL_PATH_RESET	 0x68
#define PWR_MGMT_1			 0x6B
#define PWR_MGMT_2			 0x6C
#define WHO_AM_I			 0x75
#define ACCEL_SCALE			 16384.0
#define GYRO_SCALE			 131.0

#include "Essential.h"

class MP6050{
	
private: 

uint8_t addr, temp, data;
int16_t value;

public:

MP6050();

uint8_t getWhoAmI();

void setClockSource(uint8_t src);

void powerOn();

void powerDown();

void resetSensor();

void setDigitalFilter(uint8_t bypass);

int16_t readGyroX();

int16_t readGyroY();

int16_t readGyroZ();

int16_t readAcceleroX();

int16_t readAcceleroY();

int16_t readAcceleroZ();

uint16_t readTemp();

void setGyroConfig(uint8_t config);

void setAcceleroConfig(uint8_t config);

void setConfig(uint8_t Config);

void resetSignalPath();
	
void printSensorOutput();
	
};




#endif /* MP6050_H_ */