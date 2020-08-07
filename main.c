#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <pthread.h>
#include <wiringPi.h>
#include <wiringPiI2C.h>
#include "time.h"
#include "bmp280.h"
#include "bno055.h"
#include "altimeter.h"
#include "imu.h"

int running; // Running flag
unsigned int startTime, endTime;
double currTime;
int psens;

int main(void) {
    running = 1;
    wiringPiSetup();

    /* BMP280 */
    printf("Initializing BMP280...\t");
    delay(1000);
    struct bmp280_dev bmp;
    struct bmp280_config conf;
    struct bmp280_uncomp_data ucomp_data;
    double pressure;
    double altitude;
    double temperature;

    psens = wiringPiI2CSetup(BMP280_I2C_ADDR_PRIM);

    bmp.dev_id = BMP280_I2C_ADDR_PRIM;
    bmp.read = i2c_reg_read;
    bmp.write = i2c_reg_write;
    bmp.delay_ms = delay_ms;
    bmp.intf = BMP280_I2C_INTF;

    uint8_t rslt = bmp280_init(&bmp);
    if (rslt != BMP280_OK) {
	print_rslt("Fail!\n", rslt);
	return 0;
    } else {
	printf("Success!\n");
	delay(1000);
    }
    bmp280_get_config(&conf, &bmp);
    conf.filter = BMP280_FILTER_COEFF_4;
    conf.os_pres = BMP280_OS_4X;
    conf.odr = BMP280_ODR_0_5_MS;
    bmp280_set_config(&conf, &bmp);
    bmp280_set_power_mode(BMP280_NORMAL_MODE, &bmp);

    startTime = millis();
    while (running) {
	endTime = millis();
	currTime = (double) (endTime - startTime) / (double) 1000;
	bmp280_get_uncomp_data(&ucomp_data, &bmp);
	bmp280_get_comp_pres_double(&pressure, ucomp_data.uncomp_press, &bmp);
	bmp280_get_comp_temp_double(&temperature, ucomp_data.uncomp_temp, &bmp);
	altitude = (pow(101325/pressure, 1/5.257) - 1)*(temperature + 273.15)/0.0065;
	printf("t: %.2f\tP: %.0f Pa\t A: %.1f m\t T: %.1f C\n", currTime, pressure, altitude, temperature);
	bmp.delay_ms(100);
    }

    /* BNO055 */
    /*
    struct bno055_t bno;
    bno.BNO055_RD_FUNC_PTR = BNO055_I2C_bus_read;
    bno.BNO055_WR_FUNC_PTR = BNO055_I2C_bus_write;
    bno.delay_msec = delay;
    bno.dev_addr = BNO055_I2C_ADDR1;

    bno055_init(&bno);
    bno055_set_operation_mode(BNO055_OPERATION_MODE_NDOF);

    struct bno055_euler_float_t eulerData;
    bno055_convert_float_euler_hpr_deg(&eulerData);
    unsigned char accel_calib_status = 0;
    unsigned char gyro_calib_status = 0;
    unsigned char mag_calib_status = 0;
    unsigned char sys_calib_status = 0;

    bno055_get_accel_calib_stat(&accel_calib_status);
    bno055_get_gyro_calib_stat(&gyro_calib_status);
    bno055_get_mag_calib_stat(&mag_calib_status);
    bno055_get_sys_calib_stat(&sys_calib_status);
    */

    return 0;
}
