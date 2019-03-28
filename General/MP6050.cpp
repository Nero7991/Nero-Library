/*
 * MP6050.cpp
 *
 * Created: 8/20/2017 11:46:01 AM
 *  Author: orencollaco
 */ 

#include "MP6050.h"

MP6050::MP6050(){
	addr = 0x68;
	I2C_Init(1,2);
	setDigitalFilter(5);
	setGyroConfig(0);
	setAcceleroConfig(0);
	powerOn();
	if(I2C_Read(addr,WHO_AM_I) != 0x68){
		Notify(PSTR("Bad MP6050. Check connections or replace."));
	}
	else{
		Notify(PSTR("MP6050 fine. WHO_AM_I = "));
		printHexNumber(I2C_Read(addr, WHO_AM_I),1);
	}
}

uint8_t MP6050::getWhoAmI(){
	return I2C_Read(addr,WHO_AM_I);
}

void MP6050::printSensorOutput(){
	printStringCRNL("A XYZ: ");
	printSignedNumber(readAcceleroX());
	printSignedNumber(readAcceleroY());
	printSignedNumber(readAcceleroZ());
	printString("G XYZ: ");
	printSignedNumber(readGyroX());
	printSignedNumber(readGyroY());
	printSignedNumber(readGyroZ());
	printString("T: ");
	printNumber(readTemp());
}

void MP6050::setClockSource(uint8_t src){
	src &= 0x07;
	data = I2C_Read(addr,PWR_MGMT_1);
	data &= 0xF8;
	data |= src;
	I2C_Write(addr,PWR_MGMT_1,data);
}

void MP6050::resetSensor(){
	data = I2C_Read(addr,PWR_MGMT_1);
	data |= (1 << 6);
	I2C_Write(addr,PWR_MGMT_1,data);
}

void MP6050::resetSignalPath(){
	I2C_Write(addr, SIGNAL_PATH_RESET, 0x07);
}

void MP6050::setDigitalFilter(uint8_t bypass){
	data = I2C_Read(addr,CONFIG);
	bypass &= 0x07;
	data |= bypass;
	I2C_Write(addr,CONFIG,data);
}

void MP6050::powerDown(){
	data = I2C_Read(addr,PWR_MGMT_1);
	data |= (1 << 6);
	I2C_Write(addr,PWR_MGMT_1,data);
}

void MP6050::powerOn(){
	data = I2C_Read(addr,PWR_MGMT_1);
	data &= ~(1 << 6);
	I2C_Write(addr,PWR_MGMT_1,data);
}

void MP6050::setGyroConfig(uint8_t scale){
	scale &= 0x03;
	scale <<= 3;
	I2C_Write(addr,GYRO_CONFIG,scale);
}

void MP6050::setAcceleroConfig(uint8_t scale){
	scale &= 0x03;
	scale <<= 3;
	I2C_Write(addr, ACCEL_CONFIG, scale);
}

int16_t MP6050::readAcceleroX(){
	temp = I2C_Read(addr,ACCEL_XOUT_H);
	value = I2C_Read(addr,ACCEL_XOUT_L);
	value |= temp << 8;
	value -= 1;
	value ^= 0xFFFF;
	value = (value/ACCEL_SCALE) * 1000 * 9.8;
	return value;
}

int16_t MP6050::readAcceleroY(){
	temp = I2C_Read(addr,ACCEL_YOUT_H);
	value = I2C_Read(addr,ACCEL_YOUT_L);
	value |= temp << 8;
	value -= 1;
	value ^= 0xFFFF;
	value = (value/ACCEL_SCALE) * 1000 * 9.8;
	return value;
}

int16_t MP6050::readAcceleroZ(){
	temp = I2C_Read(addr,ACCEL_ZOUT_H);
	value = I2C_Read(addr,ACCEL_ZOUT_L);
	value |= temp << 8;
	value -= 1;
	value ^= 0xFFFF;
	value = (value/ACCEL_SCALE) * 1000 * 9.8;
	return value;
}

int16_t MP6050::readGyroX(){
	temp = I2C_Read(addr,GYRO_XOUT_H);
	value = I2C_Read(addr,GYRO_XOUT_L);
	value |= temp << 8;
	value -= 1;
	value ^= 0xFFFF;
	value = (value/GYRO_SCALE) * 1000;
	return value;
}

int16_t MP6050::readGyroY(){
	temp = I2C_Read(addr,GYRO_YOUT_H);
	value = I2C_Read(addr,GYRO_YOUT_L);
	value |= temp << 8;
	value -= 1;
	value ^= 0xFFFF;
	value = (value/GYRO_SCALE) * 1000;
	return value;
}

int16_t MP6050::readGyroZ(){
	temp = I2C_Read(addr,GYRO_ZOUT_H);
	value = I2C_Read(addr,GYRO_ZOUT_L);
	value |= temp << 8;
	value -= 1;
	value ^= 0xFFFF;
	value = (value/GYRO_SCALE) * 1000;
	return value;
}

uint16_t MP6050::readTemp(){
	temp = I2C_Read(addr, TEMP_OUT_H);
	value = I2C_Read(addr, TEMP_OUT_L);
	value |= temp << 8;
	return value;
}
