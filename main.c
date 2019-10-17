#include "stdio.h"
#include <stdlib.h>
#include <pthread.h>
#include <wiringPi.h>
#include <wiringPiI2C.h>
#include "bmp280.h"
#include "bno055.h"

int running; // Running flag

int main(void) {
    running = 1;
    wiringPiSetup();
    int psens = wiringPiI2CSetup(BMP280_I2C_ADDR_PRIM); // 0x76
    int imu = wiringPiI2CSetup(BNO055_I2C_ADDR1)

    /* BMP280 */
    int8_t rslt;
    struct bmp280_dev bmp;
    bmp.dev_id = BMP280_I2C_ADDR_PRIM;
    bmp.read = i2c_reg_read;
    bmp.write = i2c_reg_write;
    bmp.delay_ms = delay_ms;
    bmp.intf = BMP280_I2C_INTF;

    rslt = bmp280_init(&bmp);
    print_rslt("BMP280 initialization status: ", rslt);

    /* BNO055 */
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

    return 0;
}
