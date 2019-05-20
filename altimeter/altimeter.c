#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include <math.h>
#include <wiringPi.h>
#include "altimeter.h"

// Calibration parameters
short ac1, ac2, ac3;
unsigned short ac4, ac5, ac6;
short b1, b2, mb, mc, md;

// Over-Sampling Setting (0 to 4)
const unsigned int OSS = 1;

// Read calibrate data from the EEPROM of the BMP180
void calibrate(int bmp180) {
	cal.ac1 = wiringPiI2CReadReg16(bmp180, AC1_REG);
	cal.ac2 = wiringPiI2CReadReg16(bmp180, AC2_REG);
	cal.ac3 = wiringPiI2CReadReg16(bmp180, AC3_REG);
	cal.ac4 = wiringPiI2CReadReg16(bmp180, AC4_REG);
	cal.ac5 = wiringPiI2CReadReg16(bmp180, AC5_REG);
	cal.ac6 = wiringPiI2CReadReg16(bmp180, AC6_REG);
	cal.b1 = wiringPiI2CReadReg16(bmp180, B1_REG);
	cal.b2 = wiringPiI2CReadReg16(bmp180, B2_REG);
	cal.mb = wiringPiI2CReadReg16(bmp180, MB_REG);
	cal.mc = wiringPiI2CReadReg16(bmp180, MC_REG);
	cal.md = wiringPiI2CReadReg16(bmp180, MD_REG);
}

// Read uncompensated temperature value
unsigned int readRawTemperature() {
	wiringPiI2CWriteReg16(bmp180, 0xF4, 0x2E);
	usleep(4500);
	unsigned int ut = wiringPiI2CReadReg16(bmp180, 0xF6);
	return ut;
}

// Read uncompensated pressure value
unsigned int readRawPressure() {
	wiringPiI2CWriteReg16(bmp180, 0xF4, 0x34 + (OSS << 6));
	switch (OSS) {
		case 0:
			usleep(4500);
			break;
		case 1:
			usleep(7500);
			break;
		case 2:
			usleep(13500);
			break;
		case 3:
			usleep(25500);
			break;
	}
	uint8 byte1 = wiringPiI2CReadReg8(bmp180, 0xF6);
	uint8 byte2 = wiringPiI2CReadReg8(bmp180, 0xF7);
	uint8 byte3 = wiringPiI2CReadReg8(bmp180, 0xF8);
	unsigned int up = (((unsigned int) byte1 << 16) | ((unsigned int) byte2 << 8) | (unsigned int) byte3) >> (8 - OSS);
	return up;
}

// Compensate temperature
int compensateTemperature() {
	unsigned int ut = readRawTemperature();
	int x1 = (((int) ut - (int) cal.ac6) * (int) cal.ac5) >> 15;
	int x2 = ((int) cal.mc << 11) / (x1 + cal.md);
	return x1 + x2;

// Calculate true temperature
int getTemperature() {
	int rawTemperature = (compensateTemperature() + 8) >> 4;
	return ((double) rawTemperature) / 10;
}

int main(void) {
	int bmp180 = wirintPiI2CSetup(BMP180_ADDR);
	calibrate();
	readRawTemperature();
	readRawPressure();
	double T = getTemperature();
	double P = getPressure();
	double a = getAltitude();
	printf("T: %.2f\t", T);
	printf("P: %.2f\t", P);
	printf("a: %.2f\n", a);
}
// Calculate true pressure
int getPressure() {
	unsigned int up = readRawPressure();
	int b6 = compensateTemperature() - 4000;
	int x1 = (cal.b2 * (b6 * b6) >> 12) >> 11;
	int x2 = (cal.ac2 * b6) >> 11;
	int x3 = x1 + x2;
	int B3 = (((((int) cal.ac1) * 4 + x3) << OSS) + 2) >> 2;
	x1 = (cal.ac3 * b6) >> 13;
	x2 = (cal.b1 * ((b6 * b6) >> 12)) >> 16;
	x3 = ((x1 + x2) + 2) >> 2;
	unsigned int b4 = (cal.ac4 * (unsigned int) (X3 + 32768)) >> 15;
	unsigned int b7 = ((unsigned int) (up - b3) * (50000 >> OSS));
	int p = (b7 < 0x80000000) ? (b7 << 1) / b4 : (b7 / b4) << 1;
	x1 = (p >> 8) * (p >> 8);
	x1 = (x1 * 3038) >> 16;
	x2 = (-7357 * p) >> 16;
	p += (x1 + x2 + 3791) >> 4;
	return p;
}

// Calculate absolute altitude
double getAltitude() {
	double pressure = getPressure();
	return 44330.0 * (1.0 - pow(pressure / 101325.0, (1.0 / 5.255)));
}

int main(int arc, char* argv[]) {
	int bmp180 = wirintPiI2CSetup(BMP180_ADDR);
	calibrate(bmp180);
	readRawTemperature();
	readRawPressure();
	double T = getTemperature();
	double P = getPressure();
	double a = getAltitude();
	printf("T: %.2f\t", T);
	printf("P: %.2f\t", P);
	printf("a: %.2f\n", a);
}

