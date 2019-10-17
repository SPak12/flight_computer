#include "stdio.h"
#include <stdlib.h>
#include <pthread.h>
#include <wiringPi.h>
#include <wiringPiI2C.h>
#include "bmp280.h"

int running; // Running flag
int psens;

void delay_ms(uint32_t period_ms);
int8_t i2c_reg_write(uint8_t i2c_addr, uint8_t reg_addr, uint8_t *reg_data, uint16_t length);
int8_t i2c_reg_read(uint8_t i2c_addr, uint8_t reg_addr, uint8_t *reg_data, uint16_t length);
void print_rslt(const char api_name[], int8_t rslt);

int main(void) {
    running = 1;
    wiringPiSetup();
    psens = wiringPiI2CSetup(BMP280_I2C_ADDR_SEC); // 0x77

    int8_t rslt;
    struct bmp280_dev bmp;
    bmp.delay_ms = delay_ms;
    bmp.dev_id = BMP280_I2C_ADDR_SEC;
    bmp.intf = BMP280_I2C_INTF;
    bmp.read = i2c_reg_read;
    bmp.write = i2c_reg_write;
    rslt = bmp280_init(&bmp);
    print_rslt("BMP280 initialization status: ", rslt);

    return 0;
}


/* BMP280 Pressure Sensor */



/* BNO055 9-Axis IMU */
