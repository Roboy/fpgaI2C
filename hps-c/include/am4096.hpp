/* AM4096 - 12 bit angular magnetic encoder IC */
#pragma once

#include "i2c.hpp"
#include <bitset>
#include <iostream>

class AM4096 {
private:
	void * h2p_lw_i2c_addr;

public:
	AM4096(void * baseAddr);
	~AM4096();
	bool readAbsAngle(uint8_t i2cAddr, uint32_t &data);
	bool readRelAngle(uint8_t i2cAddr, uint32_t &data);
	void readMagnetStatus(uint8_t i2cAddr, bool &magnetTooFar, bool &magnetTooClose);
	void readAgcGain(uint8_t i2cAddr, uint8_t &agcGain);
	bool readTacho(uint8_t i2cAddr, uint32_t &tacho);
private:
	I2C *i2c;
};
