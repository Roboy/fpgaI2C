#pragma once

#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include "hwlib.h"
#include <chrono>
#include "hps_0.h"

using namespace std::chrono;

class Timer{
public:
    void start(){
        t1 = high_resolution_clock::now();
    }
    double stop(){
        t2 = high_resolution_clock::now();
        time_span = duration_cast<duration<double>>(t2 - t1);
        return time_span.count(); // in seconds
    }
    double elapsedTime(){
        t2 = high_resolution_clock::now();
        time_span = duration_cast<duration<double>>(t2 - t1);
        return time_span.count(); // in seconds
    }
    double elapsedTimeMilliSeconds(){
        t2 = high_resolution_clock::now();
        time_span = duration_cast<milliseconds>(t2 - t1);
        return time_span.count(); // in milliseconds
    }
    double elapsedTimeMicroSeconds(){
        t2 = high_resolution_clock::now();
        time_span = duration_cast<microseconds>(t2 - t1);
        return time_span.count(); // in microseconds
    }
private:
    high_resolution_clock::time_point t1;
    high_resolution_clock::time_point t2;
    duration<double> time_span;
};

class I2C {
private:
	void * h2p_lw_i2c_addr;

public:
	I2C(void * baseAddr);

	// registers: read or write
	const uint8_t reg_addr = 0;
	const uint8_t reg_data = 1;  // note that internally there are 2 registers for data: data_rd and data_wr
	const uint8_t reg_rw = 2;
	const uint8_t reg_ena = 3;
	const uint8_t reg_number_of_bytes = 4;

	// registers: only read
	const uint8_t reg_busy = 4;
	const uint8_t reg_ack_error = 5;

	// operations
	const uint8_t opt_write = 0;
	const uint8_t opt_read = 1;

	void i2cWrite(uint8_t i2cAddr, uint32_t data, uint8_t number_of_bytes);
	uint32_t i2cRead(uint8_t i2cAddr, uint8_t number_of_bytes);
	uint32_t readSensor(uint8_t i2cAddr);

};

#define IORD(base,reg) (*(((volatile int32_t*)base)+reg))
#define IOWR(base,reg,data) (*(((volatile int32_t*)base)+reg)=data)
#define IORD_8DIRECT(base, offset) (*(((volatile int8_t*)base)+offset))
#define IORD_16DIRECT(base, offset) (*(((volatile int16_t*)base)+(offset>>1)))
#define IORD_32DIRECT(base, offset) (*(((volatile int32_t*)base)+(offset>>2)))
#define IOWR_8DIRECT(base, offset, data) (*(((volatile int8_t*)base)+offset)=data)
#define IOWR_16DIRECT(base, offset, data) (*(((volatile int16_t*)base)+(offset>>1))=data)
#define IOWR_32DIRECT(base, offset, data) (*(((volatile int32_t*)base)+(offset>>2))=data)
