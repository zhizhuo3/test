#ifndef SOFT_I2C_H
#define SOFT_I2C_H

#include "stdint.h"


#define ACCEL_ADDRESS				0xD0
#define GYRO_ADDRESS				0xD0
#define MAG_ADDRESS				    0x18

#define ACCEL_ADDRESS_2				0xD2
#define GYRO_ADDRESS_2				0xD2
#define MAG_ADDRESS_2				0x19


typedef struct{
				int16_t X;
				int16_t Y;
				int16_t Z;}S_INT16_XYZ;

extern S_INT16_XYZ MPU9250_ACC_LAST;
extern S_INT16_XYZ MPU9250_GYRO_LAST;
extern S_INT16_XYZ MPU9250_MAG_LAST;
extern int16_t MPU9250_TEMP_LAST;

//寄存器定义

#define SELF_TEST_X_GYRO		0X00
#define SELF_TEST_Y_GYRO		0X01
#define SELF_TEST_Z_GYRO		0X02

#define MAG_CNTL						0X0A

#define SELF_TEST_X_ACCEL		0X0D
#define SELF_TEST_Y_ACCEL		0X0E
#define SELF_TEST_Z_ACCEL		0X0F

#define XG_OFFSET_H					0X13
#define XG_OFFSET_L					0X14
#define YG_OFFSET_H					0X15
#define YG_OFFSET_L					0X16
#define ZG_OFFSET_H					0X17
#define ZG_OFFSET_L					0X18

#define SMPLRT_DIV					0X19	//陀螺仪采样率 典型值为0x07 1000/(1+7)=125 Hz
#define CONFIG						0X1A	//低通滤波器 典型值为0x06 5Hz
#define GYRO_CONFIG					0X1B	//陀螺仪测量范围 0x18 正负2000度
#define ACCEL_CONFIG				0X1C	//加速度计测量范围 0x18 正负16g
#define ACCEL_CONFIG2				0X1D	//加速度计低通滤波器 0x06 5Hz

#define LP_ACCEL_ODR				0X1E
#define WOM_THR						0X1F
#define FIFO_EN						0X23

#define INT_PIN_CFG					0x37
#define I2C_SLV0_ADDR                       0x25  
#define I2C_SLV0_REG                        0x26
#define I2C_SLV0_CTRL                       0x27 
#define I2C_SLV0_DO                         0x63 //output reg
#define EXT_SENS_DATA_00                    0x49  //MPU9250 IIC外挂器件读取返回寄存器00
#define INT_PIN_CFG                         0x37 //中断配置
#define USER_CTRL                           0x6a
#define I2C_MST_CTRL                        0x24
#define I2C_MST_DELAY_CTRL                  0x67

//加速度输出数据
#define ACCEL_XOUT_H				0X3B
#define ACCEL_XOUT_L				0X3C
#define ACCEL_YOUT_H				0X3D
#define ACCEL_YOUT_L				0X3E
#define ACCEL_ZOUT_H				0X3F
#define ACCEL_ZOUT_L				0X40

//温度计输出数据
#define TEMP_OUT_H					0X41
#define TEMP_OUT_L					0X42

//陀螺仪输出数据
#define GYRO_XOUT_H					0X43
#define GYRO_XOUT_L					0X44
#define GYRO_YOUT_H					0X45
#define GYRO_YOUT_L					0X46
#define GYRO_ZOUT_H					0X47
#define GYRO_ZOUT_L					0X48

#define MAG_XOUT_L		0x03
#define MAG_XOUT_H		0x04
#define MAG_YOUT_L		0x05
#define MAG_YOUT_H		0x06
#define MAG_ZOUT_L		0x07
#define MAG_ZOUT_H		0x08

//电源管理1 典型值为0x00
#define PWR_MGMT_1					0X6B

//电源管理2 典型值为0x00
#define PWR_MGMT_2					0X6C

//器件ID MPU9250默认ID为0x71
#define WHO_AM_I						0X75

//用户配置,当为0x10时使用SPI模式
#define USER_CTRL						0X6A


//指南针设备地址
#define MAG_ADDR	0x18
//指南针设备ID寄存器
#define MAG_WIA		0x00
//指南针设备ID
#define MAG_ID		0x48
//启动单次传输
#define MAG_CNTL1 0x0a




void myI2C_Init(void);
void myI2C_2_Init(void);
void myI2C_Start(void);
void myI2C_SendByte(uint8_t Byte);
uint8_t myI2C_ReceiveByte(void);
void myI2C_SendAck(uint8_t ack);
uint8_t myI2C_ReceiveAck(void);
void myI2C_Stop(void);
void MPU9250_Write_Reg(uint8_t Slave_add,uint8_t reg_add,uint8_t reg_dat);
void MPU9250_2_Write_Reg(uint8_t Slave_add,uint8_t reg_add,uint8_t reg_dat);
uint8_t MPU9250_Read_Reg(uint8_t Slave_add,uint8_t reg_add);
uint8_t MPU9250_2_Read_Reg(uint8_t Slave_add,uint8_t reg_add);
uint8_t MPU92501_Init(void);
uint8_t MPU92502_Init(void);
uint8_t MPU92501_2_Init(void);
uint8_t MPU92502_2_Init(void);
void MPU9250_READ_ACCEL(int a, short *accData);
void MPU9250_2_READ_ACCEL(int a,short *accData);
void MPU9250_READ_GYRO(int a,short *gyroData);
void MPU9250_2_READ_GYRO(int a,short *gyroData);
void MPU9250_READ_MAG(short *magData);
void MPU9250_2_READ_MAG(short *magData);
#endif
