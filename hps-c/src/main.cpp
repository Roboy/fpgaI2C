#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include "hwlib.h"
#include "socal/socal.h"
#include "socal/hps.h"
#include "socal/alt_gpio.h"
#include "hps_0.h"
#include "i2c.hpp"
#include <limits.h>

#define HW_REGS_BASE ( ALT_STM_OFST )
#define HW_REGS_SPAN ( 0x04000000 )
#define HW_REGS_MASK ( HW_REGS_SPAN - 1 )

int main(int argc, char *argv[]) {

	void *virtual_base;
	int fd;
	void *h2p_lw_i2c_addr;

	// map the address space for the LED registers into user space so we can interact with them.
	// we'll actually map in the entire CSR span of the HPS since we want to access various registers within that span

	if( ( fd = open( "/dev/mem", ( O_RDWR | O_SYNC ) ) ) == -1 ) {
		printf( "ERROR: could not open \"/dev/mem\"...\n" );
		return( 1 );
	}

	virtual_base = mmap( NULL, HW_REGS_SPAN, ( PROT_READ | PROT_WRITE ), MAP_SHARED, fd, HW_REGS_BASE );

	if( virtual_base == MAP_FAILED ) {
		printf( "ERROR: mmap() failed...\n" );
		close( fd );
		return( 1 );
	}
	
	h2p_lw_i2c_addr = virtual_base + ( (unsigned long) ( ALT_LWFPGASLVS_OFST + I2C_AVALON_BRIDGE_0_BASE ) & (unsigned long) ( HW_REGS_MASK ) );


	///////////////
	/*
	 * WRITE
	 * 0: addr <= writedata;
	 * 1: data_wr <= writedata;
	 * 2: rw <= writedata;
	 * 3: ena <= writedata;
	 */

	/*
	 * READ
	 * 	((address == 0))? addr :
	 * 	((address == 1))? data_rd :
	 * 	((address == 2))? rw :
	 * 	((address == 3))? ena :
	 * 	((address == 4))? busy :
	 * 	((address == 5))? ack_error :
	 */

	I2C* i2c = new I2C(h2p_lw_i2c_addr);
	uint i=0;
	while(i<10){
		uint32_t reg33 = i2c->readSensor(0x00, 33);
		bool dataOK = (reg33 >> 7) & 0b1;
		int absPos = reg33;

		uint32_t reg34 = i2c->readSensor(0x00, 34);
		bool tooFar = (reg34 >> 6) & 0b1;
		bool tooClose = (reg34 >> 5) & 0b1;

		printf("reg 33: %X       reg 34: %X\n",reg33,reg34);
		printf("dataOK:   %d\n", dataOK);
		printf("tooFar:   %d\n", tooFar);
		printf("tooClose: %d\n", tooClose);
		printf("absPos:   %d\n", absPos, absPos);

		usleep(10000);
		i++;
	}
//	i2c->i2cRead(0x01,2);
//	i2c->readSensor(0x00);


	// write
//	for (int i = 0; i < 10; i++) {
//		i2c->i2cWrite(i, i);
//		printf("wait\n");
//	}

	// read
	//char read = i2c->i2cRead(3);
	//printf("%X\n", read);

	delete i2c;



//	uint8_t reg;
//	write_control_register(h2p_lw_i2c_addr, 0b00000000);
//	IOWR(h2p_lw_i2c_addr, PRERlo, 0x0063);
//	read_prescaler_register(h2p_lw_i2c_addr);
//	write_control_register(h2p_lw_i2c_addr, 0b10000000);
//	read_control_register(h2p_lw_i2c_addr);
//	write_transmit_register(h2p_lw_i2c_addr, 0b11000001);
//	write_command_register(h2p_lw_i2c_addr, 0b01010000);
//	read_control_register(h2p_lw_i2c_addr);
//	read_status_register(h2p_lw_i2c_addr);

	// clean up our memory mapping and exit
	if( munmap( virtual_base, HW_REGS_SPAN ) != 0 ) {
		printf( "ERROR: munmap() failed...\n" );
		close( fd );
		return( 1 );
	}

	close( fd );

	return( 0 );
}
