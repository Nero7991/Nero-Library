/*
 * NRF24L01.cpp
 *
 * Created: 7/25/2017 4:53:19 PM
 *  Author: orencollaco
 */ 

#include "NRF24L01.h"

NRF24L01::NRF24L01(uint8_t Bufferlength_MasterTX, uint8_t Bufferlength_MasterRX, bool mode){
	_delay_ms(100);
	Mode = mode;
	Enable_DPL = 1;
	BufferLength_MasterTX = Bufferlength_MasterTX;
	BufferLength_MasterRX = Bufferlength_MasterRX;
	initialize();
}

NRF24L01::NRF24L01(uint8_t Bufferlength_MasterTX, bool mode){
	Mode = mode;
	BufferLength_MasterTX = Bufferlength_MasterTX;
	initialize();
}

void NRF24L01::powerON(bool set){
	temp = readSPIRegister(NRF_CONFIG,1);
	temp &= ~(1 << PWR_UP);
	temp |= (set << PWR_UP);
	writeSPIRegister(NRF_CONFIG, temp, 1);
}

void NRF24L01::initialize(){
	CSN_D = 1;
	CE_D = 1;
	CE = 1;
	LINK_LED_D = 1;
	rcount = 0;
	#ifdef STATS
	Notify(PSTR("Master TX Payload length set to : "));
	printNumber(BufferLength_MasterTX);
	Notify(PSTR("Master RX Payload length set to : "));
	printNumber(BufferLength_MasterRX);
	#endif
	setTransmitAddress(0xBABABABAB5);
	setReceive0Address(0xBABABABAB5);
	//if(Mode)
	//setReceivePayloadLength(0,BufferLength_MasterTX);
	writeSPIRegister(SETUP_RETR, 0, 1);
	setRFDataRate(1);
	setMode(Mode);
	if(Enable_DPL){
		toggleActivate();
		enableDynamicPayloadLength(true);
		enableDPLForRXPipe(0, true);
		enableACKWithPayload(true);
	}
	if(!Enable_DPL){
		//enableAutoACK(true);
		//enableReceiveAddress(0, 1);
		//writeSPIRegister(0x11,BufferLength_MasterTX,1);
	}
	powerON(true);
}

void NRF24L01::setMode(bool set){
	temp = readSPIRegister(NRF_CONFIG,1);
	printHexNumber(temp, 1);
	temp &= ~(1 << PRIM_RX);
	temp |= (set << PRIM_RX);
	writeSPIRegister(NRF_CONFIG, temp, 1);
	
	if(set){
		#ifdef STATS
		Notify(PSTR("Starting nRF24L01 in Receive Mode..."));
		#endif
	}
	else{
		#ifdef STATS
		Notify(PSTR("Starting nRF24L01 in Transmit Mode..."));
		#endif
	}
	if(readSPIRegister(NRF_CONFIG,1) != temp){
		#ifdef STATS
		Notify(PSTR("Initialization failed for nRF24L01. Check connections"));
		#endif
	}
	else{
		#ifdef STATS
		Notify(PSTR("Initializing successful."));
		#endif
	}
}

void NRF24L01::setReceivePayloadLength(uint8_t pipe, uint8_t length){
	writeSPIRegister(RX_PW_P0 + pipe, length, 1);
}

void NRF24L01::enableReceiveAddress(uint8_t pipe, bool set){
	temp = readSPIRegister(EN_RXADDR,1);
	temp &= ~(1 << pipe);
	temp |= (set << pipe);
	writeSPIRegister(EN_RXADDR, temp, 1);
}

void NRF24L01::enableAutoACK(bool set){
	temp = readSPIRegister(EN_AA,1);
	temp &= ~(1 << 0);
	temp |= (set << 0);
	writeSPIRegister(EN_AA, temp, 1);
}

void NRF24L01::enableACKWithPayload(bool set){
	temp = readSPIRegister(FEATURE,1);
	temp &= ~(1 << EN_ACK_PAY);
	temp |= (set << EN_ACK_PAY);
	writeSPIRegister(FEATURE, temp, 1);
}

void NRF24L01::enableDynamicPayloadLength(bool set){
	temp = readSPIRegister(FEATURE,1);
	temp &= ~(1 << EN_DPL);
	temp |= (set << EN_DPL);
	writeSPIRegister(FEATURE, temp, 1);
}

void NRF24L01::enableDPLForRXPipe(uint8_t pipe, bool set){
	if(pipe < 6){
		temp = readSPIRegister(DYNPD,1);
		temp &= ~(1 << pipe);
		temp |= (set << pipe);
		writeSPIRegister(DYNPD, temp, 1);
	}
}

void NRF24L01::toggleActivate(){
	CSN = 0;
	SPI_MasterTransmit(ACTIVATE);
	SPI_MasterTransmit(ACTIVATE_FOLL);
	CSN = 1;
}

void NRF24L01::printInfo(){
	printStringCRNL(" Config: ");
	printHexNumber(readSPIRegister(NRF_CONFIG,1),1);
	printString(" FIFO Status: ");
	printHexNumber(readSPIRegister(0x17,1),1);
	printString(" Status: ");
	printHexNumber(sendSPICommand(0xFF),1);
	printString(" TX Observe: ");
	printNumber(readSPIRegister(0x08,1));
	printString(" Feature: ");
	printNumber(readSPIRegister(FEATURE,1));
	printString(" Millis: ");
	printNumber(millis());
}

uint8_t NRF24L01::isTXFull(){
	return ((uint8_t)readSPIRegister(0x17,1)& 0x20);
}

uint8_t NRF24L01::isRXFull(){
	return ((uint8_t)readSPIRegister(0x17,1)& 0x02);
}


uint8_t NRF24L01::isTXEmpty(){
	return ((uint8_t)readSPIRegister(0x17,1) & 0x10);
}

uint8_t NRF24L01::isRXEmpty(){
	return ((uint8_t)readSPIRegister(0x17,1) & 0x01);
}

uint8_t NRF24L01::isTX_DS_Set(){
	return ((uint8_t)readSPIRegister(0x07,1) & 0x20);
}

uint8_t NRF24L01::isRT_Max_Set(){
	return ((uint8_t)readSPIRegister(0x07,1) & 0x10);
}

void NRF24L01::clearTX_DS(){
	writeSPIRegister(0x07,0x20,1);
}

void NRF24L01::clearRT_Max(){
	writeSPIRegister(0x07,0x10,1);
}


void NRF24L01::writeACKPayload(volatile uint8_t *data){
	static uint8_t i;
	writeSPIRegister(STATUS, (1 << 5), 1);
	if(isTXFull()){
	 	flushTX();
	}
	CSN = 0;
	SPI_MasterTransmit(W_ACK_PAYLOAD);
	for(i = 0; i < BufferLength_MasterRX; i++){
		SPI_MasterTransmit(*data);
		data += 1;
	}
	CSN = 1;
}

void NRF24L01::readACKPayload(volatile uint8_t *data){
	writeSPIRegister(STATUS, (1 << 6), 1);
	CSN = 0;
	SPI_MasterTransmit(0x61);
	for(uint8_t i = 0; i < BufferLength_MasterRX; i++){
		*data =  SPI_MasterTransmit(0x00);
		data += 1;
	}
	CSN = 1;
}

void NRF24L01::setRFDataRate(bool rate){
	temp = readSPIRegister(RF_SETUP,1);
	temp &= ~(1 << RF_DR);
	temp |= (rate << RF_DR);
	writeSPIRegister(RF_SETUP, temp, 1);
}

uint16_t NRF24L01::transmitPayload(volatile uint8_t *data){
	static uint8_t status, fstatus, recount = 0;
	static uint16_t pcount = 0, dcount = 0;
	static bool linklost = 1, packetInFIFO = 0;
	#ifdef STATS
	static uint16_t t;
	if((uint16_t)millis() - t > 1000){
		t = millis();
		Speed = pcount * BufferLength_MasterTX;
		printStringCRNL("Speed = ");
		printNumber(Speed);
		printString("B/s ");
		printString(" Drops = ");
		printNumber(dcount);
		printString("Packets ");
		dcount = 0;
		pcount = 0;
	}
	#endif
	if(linklost){
		status = sendSPICommand(0xFF);
		fstatus = readSPIRegister(FIFO_STATUS,1);
		if(!(fstatus & 0x20)){
			#ifdef DEBUG_UART
			Notify(PSTR("Establishing link..."));
			#endif
			writeToFIFO(data);
		}
		if(status & (1 << S_TX_DS)){
			Notify(PSTR("Link established! Connected."));
			LINK_LED = 1;
			writeSPIRegister(0x07,0x20,1);
			linklost = 0;
		}
		if(status & (1 << S_TX_FULL)){
			sendSPICommand(FLUSH_TX);
		}
		if(fstatus & (1 << F_RX_FULL)){
			sendSPICommand(FLUSH_RX);
		}
		if(status & 0x10)
			writeSPIRegister(0x07,0x10,1);
	}
	if(!linklost){
		status = sendSPICommand(0xFF);
		if((status & 0x20) && packetInFIFO){
			#ifdef DEBUG_UART
			Notify(PSTR("Packet Transmitted"));
			#endif
			pcount += 1;
			LINK_LED = 1;
			writeSPIRegister(0x07,0x20,1);
			packetInFIFO = 0;
		}
		if(!(status & 0x01) && !packetInFIFO){
			#ifdef DEBUG_UART
			Notify(PSTR("Transmitting packet..."));
			#endif
			writeToFIFO(data);
			packetInFIFO = 1;
			return Speed;
		}
		if(!(status & 0x11) && packetInFIFO){
			rcount += 1;
			if(rcount > 20){
				rcount = 0;
				#ifdef DEBUG_UART
				Notify(PSTR("No response. Link Lost"));
				#endif
				packetInFIFO = 0;
				linklost = 1;
				LINK_LED = 0;
			}
		}
		if(status & 0x10){
			#ifdef DEBUG_UART
			Notify(PSTR("Packet Transmission failed"));
			#endif
			dcount += 1;	
			LINK_LED = 0;
			if(recount < 20){
				#ifdef DEBUG_UART
				Notify(PSTR("Retransmitting..."));
				#endif
				writeSPIRegister(0x07,0x10,1);
				recount += 1;
			}
			else{
				Notify(PSTR("Maximum retransmit reached. TX FIFO Flushed. Link lost"));
				recount = 0;
				sendSPICommand(0xE1);
				linklost = 1;
			}
		}
		if(status & 0x01){
			#ifdef DEBUG_UART
			Notify(PSTR("TX Buffer Full, Waiting..."));
			#endif
		}
		return Speed;
	}
	return Speed;
}

void NRF24L01::fastTransferPayload(volatile uint8_t data){
	static uint8_t status;
	status = sendSPICommand(0xFF);
	if(!(status & 0x01)){
		CSN = 0;
		SPI_MasterTransmit(0xA0);
		SPI_MasterTransmit(data);
		CSN = 1;
		return;
	}
	if(status & 0x10)
	writeSPIRegister(0x07,0x10,1);
}

void NRF24L01::writeToFIFO(volatile uint8_t *data){
	static uint8_t i;
	CSN = 0;
	SPI_MasterTransmit(0xA0);
	for(i = 0; i < BufferLength_MasterTX; i++){
		SPI_MasterTransmit(*data);
		data += 1;
	}
	CSN = 1;
}

void NRF24L01::readFIFO(volatile uint8_t *data){
	writeSPIRegister(STATUS, (1 << 6), 1);
	CSN = 0;
	SPI_MasterTransmit(0x61);
	for(uint8_t i = 0; i < BufferLength_MasterTX; i++){
		*data =  SPI_MasterTransmit(0x00);
		data += 1;
	}
	CSN = 1;
	if(isRXFull()){
		flushRX();
	}
}

uint8_t NRF24L01::readFIFObyte(){
	CSN = 0;
	SPI_MasterTransmit(0x61);
	return SPI_MasterTransmit(0x00);
	CSN = 1;
}

void NRF24L01::writeFIFObyte(volatile uint8_t data){
	CSN = 0;
	SPI_MasterTransmit(0xA0);
	SPI_MasterTransmit(data);
	CSN = 1;
}

uint8_t NRF24L01::sendSPICommand(uint8_t cmd){
	uint8_t data;
	CSN = 0;
	data = SPI_MasterTransmit(cmd);
	CSN = 1;
	return	data;
}

uint64_t NRF24L01::readSPIRegister(uint8_t addr, uint8_t length){
	uint64_t data = 0;
	CSN = 0;
	SPI_MasterTransmit(addr);
	for(uint8_t i = 0; i < length; i++){
		data |= SPI_MasterTransmit(0x00);
		data = data << 8;
	}
	CSN = 1;
	data >>= 8;
	return	data;
}

uint8_t NRF24L01::writeSPIRegister(uint8_t addr, uint64_t data, uint8_t length){
	uint8_t status;
	CSN = 0;
	addr |= 0x20;
	status = SPI_MasterTransmit(addr);
	for(uint8_t i = 0; i < length; i++){
		SPI_MasterTransmit(data);
		data = data >> 8;
	}
	CSN = 1;
	return status;
}

void NRF24L01::setTransmitAddress(uint64_t address){
	writeSPIRegister(TX_ADDR, address, 5);
}

void NRF24L01::setReceive0Address(uint64_t address){
	writeSPIRegister(RX_ADDR_P0, address, 5);
}

void NRF24L01::flushTX(){
	sendSPICommand(FLUSH_TX);
}

void NRF24L01::flushRX(){
	sendSPICommand(FLUSH_RX);
}



