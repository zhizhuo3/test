#ifndef APPTASK_H
#define APPTASK_H


#include "I2C_driver_master_top.h"
#include "soft_i2c.h"
#include "mpu9250_dri.h"

void mpu9250_task(void *arg);
void mpu9250_SNTP_static_task();
void mpu9250_12_SNTP_static_task();
void mpu9250_21_SNTP_static_task();
void mpu9250_22_SNTP_static_task();
void mpu9250_31_SNTP_static_task();
void mpu9250_32_SNTP_static_task();
void mpu9250_41_SNTP_static_task();
void mpu9250_42_SNTP_static_task();
#endif