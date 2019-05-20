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
} cal;

// Calibrate BMP180
void calibrate();

// Calibrate pressure in Pa
int getPressure();

// Calculate temperature in Celsius
double getTemperature();

// Calculate altitude in m
double getAltitude();

