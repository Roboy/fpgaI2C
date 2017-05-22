#include "am4096.hpp"

AM4096::AM4096(void * baseAddr){
	i2c = new I2C(baseAddr);
}

AM4096::~AM4096(){
	delete i2c;
}


bool AM4096::readAbsAngle(uint8_t i2cAddr, uint32_t &data){
	uint32_t reg = i2c->read(i2cAddr, 33, 2);
	// reg = DataOK + 3x"0" + 12x bit abs angle + 16x"0"

	bool dataOK = (reg >> 31) & 0b1;

	//std::bitset<32> bits(reg);
	//std::cout << "readAbsAngle " << bits << std::endl;
	
	data = (reg >> 16) & 0xFFF;
	return dataOK;
}

bool AM4096::readRelAngle(uint8_t i2cAddr, uint32_t &data){
	uint32_t reg = i2c->read(i2cAddr, 32, 2);	
	// reg = DataOK + 3x"0" + 12x bit rel angle + 16x"0"

	bool dataOK = (reg >> 31) & 0b1;

	data = (reg >> 16) & 0xFFF;
	return dataOK;
}

void AM4096::readMagnetStatus(uint8_t i2cAddr, bool &magnetTooFar, bool &magnetTooClose){
	uint32_t reg = i2c->read(i2cAddr, 34, 2);

	// reg = "?" + Weh + Wel + 29x"?" 
	// Weh Magnet too far
	// Wel Magnet too close

	magnetTooFar = (reg >> 30) & 0x1;
	magnetTooClose = (reg >> 29) & 0x1;
}

/**
 * Reads AGC gain (magnet "power")
 */
void AM4096::readAgcGain(uint8_t i2cAddr, uint8_t &agcGain) {
	uint32_t reg = i2c->read(i2cAddr, 35, 2);
	// reg = 4x agcGain bits + "?" + Thof + 10x Tho bits + 16x "?"
	agcGain = (reg >> 28) & 0xF;
}

/**
 * Reads tacho, returns tacho overflow.
 */
bool AM4096::readTacho(uint8_t i2cAddr, uint32_t &tacho) {
	uint32_t reg = i2c->read(i2cAddr, 35, 2);
	// reg = 4x agcGain bits + "?" + Thof + 10x Tho bits + 16x "?"
	tacho = (reg >> 16) & 0x3FF;
	bool overflow = (reg >> 26) & 0x1; 
	return overflow;
}