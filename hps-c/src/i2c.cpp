#include "i2c.hpp"

I2C::I2C(void * baseAddr) {
	h2p_lw_i2c_addr = baseAddr;
}

void I2C::readSensor(uint8_t i2cAddr){
	// Set slave address
	IOWR(h2p_lw_i2c_addr, reg_addr, i2cAddr);
	// Set data to write
	IOWR(h2p_lw_i2c_addr, reg_data, 0x21);
	// Set operation mode: write
	IOWR(h2p_lw_i2c_addr, reg_rw, opt_write);
	// Start operation (enable = 1)
	IOWR(h2p_lw_i2c_addr, reg_ena, 1);

	// Set operation mode: read
	IOWR(h2p_lw_i2c_addr, reg_rw, opt_read);
	// Start operation (enable = 1)
	IOWR(h2p_lw_i2c_addr, reg_ena, 1);


}

void I2C::i2cWrite(uint8_t i2cAddr, uint8_t data) {
	while( IORD(h2p_lw_i2c_addr, reg_busy) ) {
		printf("busy waiting\b");
		usleep(1000000);
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
		usleep(1000000);
	}

}


uint8_t I2C::i2cRead(uint8_t i2cAddr, uint8_t reg) {

	while( IORD(h2p_lw_i2c_addr, reg_busy) ) {
		printf("busy waiting\b");
	}

	i2cWrite(i2cAddr,1);

	// Set slave address
	IOWR(h2p_lw_i2c_addr, reg_addr, i2cAddr);
	// Set operation mode: read
	IOWR(h2p_lw_i2c_addr, reg_rw, opt_read);
	// Start operation (enable = 1)
	IOWR(h2p_lw_i2c_addr, reg_ena, 1);

	while( IORD(h2p_lw_i2c_addr, reg_busy) ) {
		printf("busy waiting\b");
		usleep(1000000);
	}

	// read and return data
	return IORD(h2p_lw_i2c_addr, reg_data);
}
