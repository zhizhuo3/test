#ifndef MPU9250_DRI_H
#define MPU9250_DRI_H


#define ACCEL_XOUT_H 0x3B
#define GYRO_XOUT_H 0x43
#define MAG_XOUT_L 0x03



void mpu9250_init_hardi2c1();
void mpu9250_init_hardi2c2();
//MPU9250软件I2C初始化
void mpu9250_init_softi2c3();
void mpu9250_init_softi2c4();
//
void read_mpu9250_data_hard(int a,int16_t *calldata,uint8_t i);
void read_mpu9250_2_data_hard(int a,int16_t *calldata,uint8_t i);
//MPU9250软件I2C读取
void mpu9250_soft_read(int a,int16_t *calldata,uint8_t i);
void mpu9250_2_soft_read(int a,int16_t *calldata,uint8_t i);
//
#endif
