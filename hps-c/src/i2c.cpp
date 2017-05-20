#include "i2c.hpp"

I2C::I2C(void * baseAddr) {
	h2p_lw_i2c_addr = baseAddr;
}

uint32_t I2C::readSensor(uint8_t i2cAddr, uint8_t reg){
	return i2cRead(0x00,reg,2);
}

void I2C::i2cWrite(uint8_t i2cAddr, uint32_t data, uint8_t number_of_bytes) {
	// Set slave address
	IOWR(h2p_lw_i2c_addr, reg_addr, i2cAddr);
	// Set data to write
	IOWR(h2p_lw_i2c_addr, reg_data, data);
	// Set the number of bytes to be written
	IOWR(h2p_lw_i2c_addr, reg_number_of_bytes, number_of_bytes);
	// Set operation mode: write
	IOWR(h2p_lw_i2c_addr, reg_rw, opt_write);
	// Start operation (enable = 1)
	IOWR(h2p_lw_i2c_addr, reg_ena, 1);
}


uint32_t I2C::i2cRead(uint8_t i2cAddr, uint8_t reg, uint8_t number_of_bytes) {
	// Set slave address
	IOWR(h2p_lw_i2c_addr, reg_addr, i2cAddr);
	// Set operation mode: read
	IOWR(h2p_lw_i2c_addr, reg_rw, opt_read);
	// Set data to write
	IOWR(h2p_lw_i2c_addr, reg_data, reg);
	// Set the number of bytes to be read
	IOWR(h2p_lw_i2c_addr, reg_number_of_bytes, number_of_bytes+1);
	// Start operation (enable = 1)
	IOWR(h2p_lw_i2c_addr, reg_ena, 1);
	// read and return data
	return IORD(h2p_lw_i2c_addr, reg_data);
}

