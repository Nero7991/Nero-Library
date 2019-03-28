/*
 * NRF24L01.h
 *
 * Created: 7/25/2017 4:53:57 PM
 *  Author: orencollaco
 */ 


#ifndef NRF24L01_H_
#define NRF24L01_H_
#include "Essential.h"

//Register bits
#define PWR_UP				1
#define PRIM_RX				0
#define EN_DYN_ACK			0
#define EN_ACK_PAY			1
#define EN_DPL				2 
#define RF_DR				3
#define S_TX_FULL			0
#define S_MAX_RT			4
#define S_TX_DS				5
#define S_RX_DR				6
#define F_RX_EMPTY			0
#define F_RX_FULL			1
#define F_TX_EMPTY			4
#define F_TX_FULL			5


//Registers
#define NRF_CONFIG				0x00
#define STATUS					0x07
#define TX_ADDR					0x10
#define RX_PW_P0				0x11
#define FIFO_STATUS				0x17
#define RF_SETUP				0x06
#define FEATURE					0x1D
#define EN_RXADDR				0x02
#define RF_CHANNEL				0x05
#define DYNPD					0x1C
#define RX_ADDR_P0				0x0A

//Commands				
#define ACTIVATE				0x50
#define ACTIVATE_FOLL			0x73
#define W_ACK_PAYLOAD			0xA8
#define R_RX_PL_WID				0x60
#define FLUSH_TX				0xE1
#define FLUSH_RX				0xE2

class NRF24L01{
private:
uint8_t BufferLength_MasterTX, BufferLength_MasterRX, rcount, temp;

uint16_t Speed;

bool NRF_OK, Mode, Enable_DPL;


public:

NRF24L01(uint8_t BufferLength_MasterTX, bool mode);

NRF24L01(uint8_t BufferLength_MasterTX, uint8_t BufferLength_MasterRX, bool mode);

uint8_t isTXFull();

uint8_t isTXEmpty();

uint8_t isRXFull();

uint8_t isRXEmpty();

uint8_t isTX_DS_Set();

void readFIFO(volatile uint8_t *data);

uint16_t transmitPayload(volatile uint8_t *data);

void writeToFIFO(volatile uint8_t *data);

uint8_t sendSPICommand(uint8_t cmd);

uint8_t writeSPIRegister(uint8_t addr, uint64_t data, uint8_t length);

uint64_t readSPIRegister(uint8_t addr, uint8_t length);

uint8_t readFIFObyte();

void writeFIFObyte(volatile uint8_t data);

void printInfo();

void fastTransferPayload(volatile uint8_t data);

void initialize();

void powerON(bool set);

void setMode(bool set);

void enableACKWithPayload(bool set);

void enableDynamicPayloadLength(bool set);

void enableReceiveAddress(uint8_t pipe, bool set);

void enableDPLForRXPipe(uint8_t pipe, bool set);

void setAddressWidth(uint8_t width);

void setRFChannel(uint16_t channel);

void setRFDataRate(bool rate);

void toggleActivate();

void writeACKPayload(volatile uint8_t *data);

void readACKPayload(volatile uint8_t *data);

void setTransmitAddress(uint64_t address);

void setReceive0Address(uint64_t address);

void flushTX();

void flushRX();

};

#endif /* NRF24L01_H_ */