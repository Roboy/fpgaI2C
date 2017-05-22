#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include "hwlib.h"
#include "socal/socal.h"
#include "socal/hps.h"
#include "socal/alt_gpio.h"
#include "hps_0.h"
#include "am4096.hpp"
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


	AM4096 jointAngle(h2p_lw_i2c_addr);
	while(true){
		uint32_t absAngle, relAngle, tacho;
		uint8_t agcGain;
		bool dataOKAbs, dataOKRel, tooFar, tooClose, tachoOverflow;
		dataOKAbs = jointAngle.readAbsAngle(0x00, absAngle);
		dataOKRel = jointAngle.readRelAngle(0x00, relAngle);
		jointAngle.readMagnetStatus(0x00, tooFar, tooClose);
		tachoOverflow = jointAngle.readTacho(0x00, tacho);
		jointAngle.readAgcGain(0x00, agcGain);
		printf("magnet   %s\n", (tooFar ? "too far" : (tooClose ? "too close" : "ok")));
		printf("agc gain %d\n", agcGain);
		printf("absPos   %d, %s\n", absAngle, (dataOKAbs     ? "not ok" : "ok"));
		printf("relPos   %d, %s\n", relAngle, (dataOKRel     ? "not ok" : "ok"));
		printf("tacho    %d, %s\n", tacho,    (tachoOverflow ? "not ok" : "ok"));
		printf("\n");
		usleep(100000);
	}


	// clean up our memory mapping and exit
	if( munmap( virtual_base, HW_REGS_SPAN ) != 0 ) {
		printf( "ERROR: munmap() failed...\n" );
		close( fd );
		return( 1 );
	}

	close( fd );

	return( 0 );
}
