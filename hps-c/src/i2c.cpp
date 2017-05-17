#include "i2c.hpp"

I2C::I2C(void * baseAddr) {
	h2p_lw_i2c_addr = baseAddr;
}

void I2C::i2cWrite(char i2cAddr, char data) {
	while( IORD(h2p_lw_i2c_addr, reg_busy) ) {
		printf("busy waiting\b");
	}

	// Set slave address
	IOWR(h2p_lw_i2c_addr, reg_addr, i2cAddr);
	// Set data to write
	IOWR(h2p_lw_i2c_addr, reg_data, data);
	// Set operation mode: write
	IOWR(h2p_lw_i2c_addr, reg_rw, opt_write);
	// Start operation (enable = 1)
	IOWR(h2p_lw_i2c_addr, reg_ena, 1);

	while( IORD(h2p_lw_i2c_addr, reg_busy) ) {
		printf("busy waiting\b");
	}

}


char I2C::i2cRead(char i2cAddr) {

	while( IORD(h2p_lw_i2c_addr, reg_busy) ) {
		printf("busy waiting\b");
	}

	// Set slave address
	IOWR(h2p_lw_i2c_addr, reg_addr, i2cAddr);
	// Set operation mode: read
	IOWR(h2p_lw_i2c_addr, reg_rw, opt_read);
	// Start operation (enable = 1)
	IOWR(h2p_lw_i2c_addr, reg_ena, 1);

	while( IORD(h2p_lw_i2c_addr, reg_busy) ) {
		printf("busy waiting\b");
	}

	// read and return data
	return IORD(h2p_lw_i2c_addr, reg_data);
}
