#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include <math.h>
#include <wiringPi.h>
#include <wiringPiI2C.h>

#define BMP180_ADDR 0x77
#define AC1_REG 0xAA
#define AC2_REG 0xAC
#define AC3_REG 0xAE
#define AC4_REG 0xB0
#define AC5_REG 0xB2
#define AC6_REG 0xB4
#define B1_REG 0xB6
#define B2_REG 0xB8
#define MB_REG 0xBA
#define MC_REG 0xBC
#define MD_REG 0xBE

struct calibrationData {
	short int ac1;
	short int ac2;
	short int ac3;
	unsigned short int ac4;
	unsigned short int ac5;
	unsigned short int ac6;
	short int b1;
	short int b2;
	short int mb;
	short int mc;
	short int md;
};

// Initialize calibration data struct
struct calibrationData cal;

// Over-Sampling Setting (0 to 4)
const int OSS = 1;

uint16_t read16(int fd, int reg) {
	return (wiringPiI2CReadReg8(fd, reg) << 8) | wiringPiI2CReadReg8(fd, reg + 1);
}

// Read calibrate data from the EEPROM of the BMP180
void calibrate(int fd) {
	cal.ac1 = read16(fd, AC1_REG);
	cal.ac2 = read16(fd, AC2_REG);
	cal.ac3 = read16(fd, AC3_REG);
	cal.ac4 = read16(fd, AC4_REG);
	cal.ac5 = read16(fd, AC5_REG);
	cal.ac6 = read16(fd, AC6_REG);
	cal.b1 = read16(fd, B1_REG);
	cal.b2 = read16(fd, B2_REG);
	cal.mb = read16(fd, MB_REG);
	cal.mc = read16(fd, MC_REG);
	cal.md = read16(fd, MD_REG);
}

// Read uncompensated temperature value
unsigned int readRawTemperature(int fd) {
	wiringPiI2CWriteReg8(fd, 0xF4, 0x2E);
	usleep(4500);
	unsigned int ut = read16(fd, 0xF6);
	return ut;
}

// Read uncompensated pressure value
unsigned int readRawPressure(int fd) {
	wiringPiI2CWriteReg8(fd, 0xF4, 0x34 + (OSS << 6));
	switch (OSS) {
		case 0: usleep(4500); break;
		case 1: usleep(7500); break;
		case 2: usleep(13500); break;
		case 3: usleep(25500); break;
	}
	uint8_t byte1 = wiringPiI2CReadReg8(fd, 0xF6);
	uint8_t byte2 = wiringPiI2CReadReg8(fd, 0xF7);
	uint8_t byte3 = wiringPiI2CReadReg8(fd, 0xF8);
	unsigned int up = (((unsigned int) byte1 << 16) | ((unsigned int) byte2 << 8) | (unsigned int) byte3) >> (8 - OSS);
	return up;
}

// Compensate temperature
int compensateTemperature(int fd) {
	unsigned int ut = readRawTemperature(fd);
	int x1 = (((int) ut - (int) cal.ac6) * (int) cal.ac5) >> 15;
	int x2 = ((int) cal.mc << 11) / (x1 + cal.md);
	return x1 + x2;
}

// Calculate true temperature
double getTemperature(int fd) {
	int rawTemperature = (compensateTemperature(fd) + 8) >> 4;
	return ((double) rawTemperature) / 10;
}

// Calculate true pressure
int getPressure(int fd) {
	unsigned int up = readRawPressure(fd);
	int b6 = compensateTemperature(fd) - 4000;
	int x1 = (cal.b2 * (b6 * b6) >> 12) >> 11;
	int x2 = (cal.ac2 * b6) >> 11;
	int x3 = x1 + x2;
	int b3 = (((((int) cal.ac1) * 4 + x3) << OSS) + 2) >> 2;
	x1 = (cal.ac3 * b6) >> 13;
	x2 = (cal.b1 * ((b6 * b6) >> 12)) >> 16;
	x3 = ((x1 + x2) + 2) >> 2;
	unsigned int b4 = (cal.ac4 * (unsigned int) (x3 + 32768)) >> 15;
	unsigned int b7 = ((unsigned int) (up - b3) * (50000 >> OSS));
	int p = (b7 < 0x80000000) ? (b7 << 1) / b4 : (b7 / b4) << 1;
	x1 = (p >> 8) * (p >> 8);
	x1 = (x1 * 3038) >> 16;
	x2 = (-7357 * p) >> 16;
	p += (x1 + x2 + 3791) >> 4;
	return p;
}

// Calculate absolute altitude
double getAltitude(int fd) {
	double pressure = getPressure(fd);
	return 44330.0 * (1.0 - pow(pressure / 101325.0, (1.0 / 5.255)));
}

