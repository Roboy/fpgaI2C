#include "mlp3115A2.hpp"

MLP3115A2::MLP3115A2(void * i2cBaseAddr){
	i2c = new I2C(i2cBaseAddr);

	i2c->read(MPL3115A2_ADDRESS,WHO_AM_I,1);

//	i2c->write(MPL3115A2_ADDRESS, (CTRL_REG1<<8|0), 2);


//	i2c->read(MPL3115A2_ADDRESS,WHO_AM_I,1);
//	i2c->read(MPL3115A2_ADDRESS,WHO_AM_I,1);

//	if(i2c->read(MPL3115A2_ADDRESS,WHO_AM_I,1) == 196)
//		printf("MPL3115A2 online!\n");
//	else{
//		printf("No response - check connections\n");
//		return;
//	}
	setModeBarometer(); // Measure pressure in Pascals from 20 to 110 kPa
	setOversampleRate(1); // Set Oversample to the recommended 128
	enableEventFlags(); // Enable all three pressure and temp event flags
}

MLP3115A2::~MLP3115A2(){
	delete i2c;
}

void MLP3115A2::setModeBarometer(){
	uint8_t tempSetting = i2c->read(MPL3115A2_ADDRESS,CTRL_REG1,1); //Read current settings
	tempSetting &= ~(1<<7); //Clear ALT bit
	usleep(1000);
	i2c->write(MPL3115A2_ADDRESS, (tempSetting<<8|CTRL_REG1), 2);
}

void MLP3115A2::setOversampleRate(uint8_t sampleRate){
	if(sampleRate > 7) sampleRate = 7; //OS cannot be larger than 0b.0111
	sampleRate <<= 3; //Align it for the CTRL_REG1 register

	uint8_t tempSetting = i2c->read(MPL3115A2_ADDRESS,CTRL_REG1,1); //Read current settings
	tempSetting &= 0b11000111; //Clear out old OS bits
	tempSetting |= sampleRate; //Mask in new OS bits
	usleep(1000);
	i2c->write(MPL3115A2_ADDRESS, (tempSetting<<8|CTRL_REG1), 2);
}

void MLP3115A2::enableEventFlags(){
	usleep(1000);
	uint8_t val = 0x07;
	i2c->write(MPL3115A2_ADDRESS, (val<<8|PT_DATA_CFG), 2); // Enable all three pressure and temp event flags
}

float MLP3115A2::readPressure(){
	toggleOneShot(); //Toggle the OST bit causing the sensor to immediately take another reading

	//Wait for PDR bit, indicates we have new pressure data
	int counter = 0;
	while( (i2c->read(MPL3115A2_ADDRESS,STATUS,1) & (1<<2)) == 0)
	{
	  if(++counter > 100) return(-999); //Error out
	  usleep(10);
	}

	// Read pressure registers
	uint32_t val = i2c->read(MPL3115A2_ADDRESS,OUT_P_MSB,3);

	toggleOneShot(); //Toggle the OST bit causing the sensor to immediately take another reading

	uint8_t msb, lsb, csb;
	msb = val&(0xFF);
	csb = (val>>8)&(0xFF);
	lsb = (val>>16)&(0xFF);
	uint32_t pressure_whole = (uint8_t)msb<<16 | (uint8_t)csb<<8 | (uint8_t)lsb;

	// Pressure comes back as a left shifted 20 bit number
	pressure_whole >>= 6; //Pressure is an 18 bit number with 2 bits of decimal. Get rid of decimal portion.

	lsb = val & 0b00110000; //Bits 5/4 represent the fractional component
	lsb >>= 4; //Get it right aligned
	float pressure_decimal = (float)lsb/4.0; //Turn it into fraction

	float pressure = (float)pressure_whole + pressure_decimal;

	return(pressure);
}

void MLP3115A2::toggleOneShot(void){
	uint8_t tempSetting = i2c->read(MPL3115A2_ADDRESS,CTRL_REG1,1); //Read current settings
	tempSetting &= ~(1<<1); //Clear OST bit
	i2c->write(MPL3115A2_ADDRESS, (tempSetting<<8|CTRL_REG1), 2);

	tempSetting = i2c->read(MPL3115A2_ADDRESS,CTRL_REG1,1); //Read current settings to be safe
	tempSetting |= (1<<1); //Set OST bit
	i2c->write(MPL3115A2_ADDRESS, (tempSetting<<8|CTRL_REG1), 2);
}
