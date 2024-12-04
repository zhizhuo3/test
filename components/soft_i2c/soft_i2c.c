/*
 * @Author: LiHONGYANG
 * @Date: 2024-10-16 13:50:42
 * @LastEditors: LiHONGYANG
 * @LastEditTime: 2024-10-16 13:50:42
 * @FilePath: \i2c_software_driver\main\myI2C.c
 * @Description: 软件模拟I2C
 * 
 * Copyright (c) 2024 by LIHONGYANG, All Rights Reserved. 
 */
#include <driver/gpio.h>
#include <esp_log.h>
#include <esp_rom_sys.h>
#include <soft_i2c.h>
// #define myI2C_3_SCL_Pin GPIO_NUM_1
// #define myI2C_3_SDA_Pin GPIO_NUM_2

// #define myI2C_4_SCL_Pin GPIO_NUM_48
// #define myI2C_4_SDA_Pin GPIO_NUM_47

#define myI2C_3_SCL_Pin GPIO_NUM_4
#define myI2C_3_SDA_Pin GPIO_NUM_5

#define myI2C_4_SCL_Pin GPIO_NUM_6
#define myI2C_4_SDA_Pin GPIO_NUM_7


/**
 * @description: 给SCL引脚写入电平 
 * @param {uint32_t} BitValue 写入的电平 取值：0（低电平） 1（高电平）
 * @warning 部分开发板如果频率慢的话需要加延时函数，下同
 * @return {*}
 */
void myI2C_Write_SCL(uint8_t BitValue)
{
    gpio_set_level(myI2C_3_SCL_Pin,(uint8_t)BitValue);
 
}

void myI2C_2_Write_SCL(uint8_t BitValue)
{
    gpio_set_level(myI2C_4_SCL_Pin,(uint8_t)BitValue);
 
}

/**
 * @description: 给SDA引脚写入电平 
 * @param {uint32_t} BitValue 写入的电平 取值：0（低电平） 1（高电平）
 * @return {*}
 */
void myI2C_Write_SDA(uint8_t BitValue)
{
    // gpio_set_level(myI2C_3_SCL_Pin,(uint8_t)BitValue);
  gpio_set_level(myI2C_3_SDA_Pin,(uint8_t)BitValue);
}
void myI2C_2_Write_SDA(uint8_t BitValue)
{
    //gpio_set_level(myI2C_4_SCL_Pin,(uint8_t)BitValue);
    gpio_set_level(myI2C_4_SDA_Pin,(uint8_t)BitValue);

}

/**
 * @description: 读取SCL的电平 
 * @tip:在 ESP32 上，GPIO 驱动程序没有直接提供获取输出引脚电平状态的函数。
 *      在下方myI2C_Init()初始化函数内，myI2C_SCL_Pin和myI2C_SDA_Pin都设为了开漏输出，
 *      但gpio_get_level()函数只能获取GPIO输入电平，因此，需要在该函数和myI2C_Read_SDA()函数内临时切换gpio的模式为输入模式，
 *      然后读取电平，再读取完电平以后再置回开漏输出模式。
 * @return {*}0：读取出来的电平为低电平 1：读取出来的电平为高电平
 */
uint8_t myI2C_Read_SCL()
{
    int BitVale;
    gpio_set_direction(myI2C_3_SCL_Pin, GPIO_MODE_INPUT);//临时设为输入模式
    BitVale=gpio_get_level(myI2C_3_SCL_Pin);//读取电平状态
    gpio_set_direction(myI2C_3_SCL_Pin, GPIO_MODE_INPUT_OUTPUT_OD);//切换为开漏输出模式
    return BitVale;
}

uint8_t myI2C_2_Read_SCL()
{
    int BitVale;
    gpio_set_direction(myI2C_4_SCL_Pin, GPIO_MODE_INPUT);//临时设为输入模式
    BitVale=gpio_get_level(myI2C_4_SCL_Pin);//读取电平状态
    gpio_set_direction(myI2C_4_SCL_Pin, GPIO_MODE_INPUT_OUTPUT_OD);//切换为开漏输出模式
    return BitVale;
}


/**
 * @description: 读取SDA的电平 
 * @return {*}0：读取出来的电平为低电平 1：读取出来的电平为高电平
 */
uint8_t myI2C_Read_SDA()
{
    int BitVale;
    gpio_set_direction(myI2C_3_SDA_Pin, GPIO_MODE_INPUT);//临时设为输入模式
    BitVale=gpio_get_level(myI2C_3_SDA_Pin);//读取电平状态
    gpio_set_direction(myI2C_3_SDA_Pin, GPIO_MODE_INPUT_OUTPUT_OD);//切换为开漏输出模式
    return BitVale;
}

uint8_t myI2C_2_Read_SDA()
{
    int BitVale;
    gpio_set_direction(myI2C_4_SDA_Pin, GPIO_MODE_INPUT);//临时设为输入模式
    BitVale=gpio_get_level(myI2C_4_SDA_Pin);//读取电平状态
    gpio_set_direction(myI2C_4_SDA_Pin, GPIO_MODE_INPUT_OUTPUT_OD);//切换为开漏输出模式
    return BitVale;
}

/**
 * @description: I2C初始化
 * @return {*}无
 */
void myI2C_Init(void)
{
    gpio_config_t gpio_config_InitStructure;//初始化gpio
    gpio_config_InitStructure.intr_type=GPIO_INTR_DISABLE;//中断失能
    gpio_config_InitStructure.pull_down_en=GPIO_PULLDOWN_ENABLE;//下拉失能
    gpio_config_InitStructure.pull_up_en=GPIO_PULLUP_DISABLE;//上拉失能
    gpio_config_InitStructure.mode=GPIO_MODE_OUTPUT_OD;//开漏输出
    gpio_config_InitStructure.pin_bit_mask=(1ull<<myI2C_3_SCL_Pin)|((1ull<<myI2C_3_SDA_Pin));//引脚掩码
    gpio_config(&gpio_config_InitStructure);
    //拉高电平，为起始信号做准备
    myI2C_Write_SCL(1);
    myI2C_Write_SDA(1);
}

void myI2C_2_Init(void)
{
    gpio_config_t gpio_config_InitStructure;//初始化gpio
    gpio_config_InitStructure.intr_type=GPIO_INTR_DISABLE;//中断失能
    gpio_config_InitStructure.pull_down_en=GPIO_PULLDOWN_ENABLE;//下拉失能
    gpio_config_InitStructure.pull_up_en=GPIO_PULLUP_DISABLE;//上拉失能
    gpio_config_InitStructure.mode=GPIO_MODE_OUTPUT_OD;//开漏输出
    gpio_config_InitStructure.pin_bit_mask=(1ull<<myI2C_4_SCL_Pin)|((1ull<<myI2C_4_SDA_Pin));//引脚掩码
    gpio_config(&gpio_config_InitStructure);
    //拉高电平，为起始信号做准备
    myI2C_2_Write_SCL(1);
    myI2C_2_Write_SDA(1);
}


/**
 * @description: I2C起始信号
 * @tip:scl高电平期间，sda由高电平切换为低电平
 * @return {*}无
 */
void myI2C_Start(void)
{
    //首先恢复空闲状态,由于在指定地址读时 会有个str ,此时SDA的电平并不知道，
	//如果先释放SCL，此时若SDA为低电平，然后在释放完SCL后才释放SDA，
    //会导致停止条件的产生，所以先释放sda，再释放scl
    myI2C_Write_SDA(1);
    myI2C_Write_SCL(1);
    //在scl高电平期间，sda由高电平切换为低电平，为保持后续的时序的连续性，将scl也拉低
    myI2C_Write_SDA(0);//拉低sda
    myI2C_Write_SCL(0);//拉低scl
}

void myI2C_2_Start(void)
{
    //首先恢复空闲状态,由于在指定地址读时 会有个str ,此时SDA的电平并不知道，
	//如果先释放SCL，此时若SDA为低电平，然后在释放完SCL后才释放SDA，
    //会导致停止条件的产生，所以先释放sda，再释放scl
    myI2C_2_Write_SDA(1);
    myI2C_2_Write_SCL(1);
    //在scl高电平期间，sda由高电平切换为低电平，为保持后续的时序的连续性，将scl也拉低
    myI2C_2_Write_SDA(0);//拉低sda
    myI2C_2_Write_SCL(0);//拉低scl
}


/**
 * @description: 发送一个字节
 * @param {uint8_t} Byte 发送的字节数据 范围:0x00~0xff
 * @tip:由于是高位先行，所以要使用&进行一位一位的取出
 * @warning：SCL低电平期间，主机将数据位依次放到SDA线上（高位先行），然后释放SCL，从机将在SCL高电平期间读取数据位，所以SCL高电平期间SDA不允许有数据变化，依次循环上述过程8次，即可发送一个字节
 * @return {*}
 */
void myI2C_SendByte(uint8_t Byte)
{
   uint8_t i;
   for(i=0;i<8;i++)
   {
        myI2C_Write_SDA(Byte&(0x80>>i));//将数据放至数据线上
        myI2C_Write_SCL(1);//释放scl,释放主动权，让从机读取，此时不允许数据再有变化
        myI2C_Write_SCL(0);//拉低scl
   }
}

void myI2C_2_SendByte(uint8_t Byte)
{
   //...循环八次
   uint8_t i;
   for(i=0;i<8;i++)
   {
        myI2C_2_Write_SDA(Byte&(0x80>>i));//将数据放至数据线上
        myI2C_2_Write_SCL(1);//释放scl,释放主动权，让从机读取，此时不允许数据再有变化
        myI2C_2_Write_SCL(0);//拉低scl
   }
}


/**
 * @description: 主机接收一个字节
 * @tip:接收一个字节：SCL低电平期间，从机将数据位依次放到SDA线上（高位先行），然后释放SCL，主机将在SCL高电平期间读取数据位，所以SCL高电平期间SDA不允许有数据变化，依次循环上述过程8次，即可接收一个字节（主机在接收之前，需要释放SDA）
 * @return {*}接收的字节数据 范围:0x00~0xff
 */

uint8_t myI2C_ReceiveByte(void)
{
    uint8_t byte=0x00,i=0;  
    myI2C_Write_SDA(1);//先释放sda，防止主机一直占用sda信号，从机无法掌握主动权
    for(i=0;i<8;i++)
    {
        myI2C_Write_SCL(1);//拉高scl
        if(myI2C_Read_SDA()==1)//如果该位为1
        {
            byte|=(0x80>>i);//接收数据，此时不允许数据变化
        }
         myI2C_Write_SCL(0);//拉低scl
    }
    return byte;//返回接收的字节
}

uint8_t myI2C_2_ReceiveByte(void)
{
    uint8_t byte=0x00,i=0;  
    myI2C_2_Write_SDA(1);//先释放sda，防止主机一直占用sda信号，从机无法掌握主动权
    for(i=0;i<8;i++)
    {
        myI2C_2_Write_SCL(1);//拉高scl
        if(myI2C_2_Read_SDA()==1)//如果该位为1
        {
            byte|=(0x80>>i);//接收数据，此时不允许数据变化
        }
         myI2C_2_Write_SCL(0);//拉低scl
    }
    return byte;//返回接收的字节

}

/**
 * @description: 发送应答，主机发送一位应答，确认从机是否收到一个字节的数据
 * @param {uint8_t} ack 应答 取值：0或1
 * @tip:发送应答：主机在接收完一个字节之后，在下一个时钟发送一位数据，数据0表示应答，数据1表示非应答
 * @return {*}无
 */
void myI2C_SendAck(uint8_t ack)
{
    myI2C_Write_SDA(ack);//发送应答
    myI2C_Write_SCL(1);//拉高scl，主动权交给从机
    myI2C_Write_SCL(0);//拉高scl
}

void myI2C_2_SendAck(uint8_t ack)
{
    myI2C_2_Write_SDA(ack);//发送应答
    myI2C_2_Write_SCL(1);//拉高scl，主动权交给从机
    myI2C_2_Write_SCL(0);//拉高scl
}

/**
 * @description: 主机接收来自从机的应答
 * @tip:接收应答：主机在发送完一个字节之后，在下一个时钟接收一位数据，判断从机是否应答，数据0表示应答，数据1表示非应答（主机在接收之前，需要释放SDA）
 * @return {*}ack 返回的应答位：0应答，1非应答
 */
uint8_t myI2C_ReceiveAck(void)
{
    uint8_t ack=0;
    myI2C_Write_SDA(1);//拉高释放，主动权交给从机，让从机发送应答
    myI2C_Write_SCL(1);//拉高scl，主动权交给从机，采样
    ack=myI2C_Read_SDA();//接收应答
    myI2C_Write_SCL(0);//拉高scl
    return ack;//返回应答
}

uint8_t myI2C_2_ReceiveAck(void)
{
    uint8_t ack=0;
    myI2C_2_Write_SDA(1);//拉高释放，主动权交给从机，让从机发送应答
    myI2C_2_Write_SCL(1);//拉高scl，主动权交给从机，采样
    ack=myI2C_2_Read_SDA();//接收应答
    myI2C_2_Write_SCL(0);//拉高scl
    return ack;//返回应答
}

/**
 * @description: I2C停止信号
 * @tip:终止条件：SCL高电平期间，SDA从低电平切换到高电平
 * @return {*}无
 */
void myI2C_Stop(void)
{
    //SDA若一开始就是低电平，则可直接拉高 ，但如果SDA开始时是高电平 则需要先拉低再拉高
    myI2C_Write_SDA(0);
    myI2C_Write_SCL(1);//在scl高电平期间，SDA从低电平切换到高电平
    myI2C_Write_SDA(1);
}

void myI2C_2_Stop(void)
{
    //SDA若一开始就是低电平，则可直接拉高 ，但如果SDA开始时是高电平 则需要先拉低再拉高
    myI2C_2_Write_SDA(0);
    myI2C_2_Write_SCL(1);//在scl高电平期间，SDA从低电平切换到高电平
    myI2C_2_Write_SDA(1);
}

void MPU9250_Write_Reg(uint8_t Slave_add,uint8_t reg_add,uint8_t reg_dat)
{
	myI2C_Start();
	myI2C_SendByte(Slave_add);
	myI2C_ReceiveAck();
	myI2C_SendByte(reg_add);
	myI2C_ReceiveAck();
	myI2C_SendByte(reg_dat);
	myI2C_ReceiveAck();
	myI2C_Stop();
}

void MPU9250_2_Write_Reg(uint8_t Slave_add,uint8_t reg_add,uint8_t reg_dat)
{
	myI2C_2_Start();
	myI2C_2_SendByte(Slave_add);
	myI2C_2_ReceiveAck();
	myI2C_2_SendByte(reg_add);
	myI2C_2_ReceiveAck();
	myI2C_2_SendByte(reg_dat);
	myI2C_2_ReceiveAck();
	myI2C_2_Stop();
}

uint8_t MPU9250_Read_Reg(uint8_t Slave_add,uint8_t reg_add)
{
	uint8_t temp=0;

	myI2C_Start();
	myI2C_SendByte(Slave_add);
	temp=myI2C_ReceiveAck();
	myI2C_SendByte(reg_add);
	temp=myI2C_ReceiveAck();
	myI2C_Start();	//
	myI2C_SendByte(Slave_add+1);
	temp=myI2C_ReceiveAck();
	temp=myI2C_ReceiveByte();
	myI2C_Stop();
	return temp;
}

uint8_t MPU9250_2_Read_Reg(uint8_t Slave_add,uint8_t reg_add)
{
	uint8_t temp=0;

	myI2C_2_Start();
	myI2C_2_SendByte(Slave_add);
	temp=myI2C_2_ReceiveAck();
	myI2C_2_SendByte(reg_add);
	temp=myI2C_2_ReceiveAck();
	myI2C_2_Start();	//
	myI2C_2_SendByte(Slave_add+1);
	temp=myI2C_2_ReceiveAck();
	temp=myI2C_2_ReceiveByte();
	myI2C_2_Stop();
	return temp;
}
//I2C3-MPU9250-1




void MPU9250_READ_ACCEL(int a, short *accData)
{ 
    if(a==0)
    {
	uint8_t BUF[6];
	BUF[0]=MPU9250_Read_Reg(ACCEL_ADDRESS,ACCEL_XOUT_L); 
	BUF[1]=MPU9250_Read_Reg(ACCEL_ADDRESS,ACCEL_XOUT_H);
	accData[0]=(BUF[1]<<8)|BUF[0];
	//accData[0]/=164;								//读取并计算X轴数据

	BUF[2]=MPU9250_Read_Reg(ACCEL_ADDRESS,ACCEL_YOUT_L);
	BUF[3]=MPU9250_Read_Reg(ACCEL_ADDRESS,ACCEL_YOUT_H);
	accData[1]=(BUF[3]<<8)|BUF[2];
	//accData[1]/=164;								//读取并计算Y轴数据

	BUF[4]=MPU9250_Read_Reg(ACCEL_ADDRESS,ACCEL_ZOUT_L);
  BUF[5]=MPU9250_Read_Reg(ACCEL_ADDRESS,ACCEL_ZOUT_H);
  accData[2]=(BUF[5]<<8)|BUF[4];
	//accData[2]/=164;								//读取并计算Z轴数据
    }
    else
    {
    uint8_t BUF[6];
	BUF[0]=MPU9250_Read_Reg(ACCEL_ADDRESS_2,ACCEL_XOUT_L); 
	BUF[1]=MPU9250_Read_Reg(ACCEL_ADDRESS_2,ACCEL_XOUT_H);
	accData[0]=(BUF[1]<<8)|BUF[0];
	//accData[0]/=164;								//读取并计算X轴数据

	BUF[2]=MPU9250_Read_Reg(ACCEL_ADDRESS_2,ACCEL_YOUT_L);
	BUF[3]=MPU9250_Read_Reg(ACCEL_ADDRESS_2,ACCEL_YOUT_H);
	accData[1]=(BUF[3]<<8)|BUF[2];
	//accData[1]/=164;								//读取并计算Y轴数据

	BUF[4]=MPU9250_Read_Reg(ACCEL_ADDRESS_2,ACCEL_ZOUT_L);
    BUF[5]=MPU9250_Read_Reg(ACCEL_ADDRESS_2,ACCEL_ZOUT_H);
    accData[2]=(BUF[5]<<8)|BUF[4];
	//accData[2]/=164;								//读取并计算Z轴数据  
    }
}


void MPU9250_2_READ_ACCEL(int a,short *accData)
{ 
    if(a==0)
    {
	uint8_t BUF[6];
	BUF[0]=MPU9250_2_Read_Reg(ACCEL_ADDRESS,ACCEL_XOUT_L); 
	BUF[1]=MPU9250_2_Read_Reg(ACCEL_ADDRESS,ACCEL_XOUT_H);
	accData[0]=(BUF[1]<<8)|BUF[0];
	//accData[0]/=164;								//读取并计算X轴数据

	BUF[2]=MPU9250_2_Read_Reg(ACCEL_ADDRESS,ACCEL_YOUT_L);
	BUF[3]=MPU9250_2_Read_Reg(ACCEL_ADDRESS,ACCEL_YOUT_H);
	accData[1]=(BUF[3]<<8)|BUF[2];
	//accData[1]/=164;								//读取并计算Y轴数据

	BUF[4]=MPU9250_2_Read_Reg(ACCEL_ADDRESS,ACCEL_ZOUT_L);
  BUF[5]=MPU9250_2_Read_Reg(ACCEL_ADDRESS,ACCEL_ZOUT_H);
  accData[2]=(BUF[5]<<8)|BUF[4];
	//accData[2]/=164;								//读取并计算Z轴数据
    }
    else
    {
    uint8_t BUF[6];
	BUF[0]=MPU9250_2_Read_Reg(ACCEL_ADDRESS_2,ACCEL_XOUT_L); 
	BUF[1]=MPU9250_2_Read_Reg(ACCEL_ADDRESS_2,ACCEL_XOUT_H);
	accData[0]=(BUF[1]<<8)|BUF[0];
	//accData[0]/=164;								//读取并计算X轴数据

	BUF[2]=MPU9250_2_Read_Reg(ACCEL_ADDRESS_2,ACCEL_YOUT_L);
	BUF[3]=MPU9250_2_Read_Reg(ACCEL_ADDRESS_2,ACCEL_YOUT_H);
	accData[1]=(BUF[3]<<8)|BUF[2];
	//accData[1]/=164;								//读取并计算Y轴数据

	BUF[4]=MPU9250_2_Read_Reg(ACCEL_ADDRESS_2,ACCEL_ZOUT_L);
    BUF[5]=MPU9250_2_Read_Reg(ACCEL_ADDRESS_2,ACCEL_ZOUT_H);
    accData[2]=(BUF[5]<<8)|BUF[4];
	//accData[2]/=164;								//读取并计算Z轴数据
    }

}

void MPU9250_READ_GYRO(int a,short *gyroData)
{
	uint8_t BUF[8];
    if(a==0)
    {
	BUF[0]=MPU9250_Read_Reg(GYRO_ADDRESS,GYRO_XOUT_L);
	BUF[1]=MPU9250_Read_Reg(GYRO_ADDRESS,GYRO_XOUT_H);
	gyroData[0]=(BUF[1]<<8)|BUF[0];
	//gyroData[0]/=16.4;

	BUF[2]=MPU9250_Read_Reg(GYRO_ADDRESS,GYRO_YOUT_L);
	BUF[3]=MPU9250_Read_Reg(GYRO_ADDRESS,GYRO_YOUT_H);
	gyroData[1]=(BUF[3]<<8)|BUF[2];
	//gyroData[1]/=16.4;

	BUF[4]=MPU9250_Read_Reg(GYRO_ADDRESS,GYRO_ZOUT_L);
	BUF[5]=MPU9250_Read_Reg(GYRO_ADDRESS,GYRO_ZOUT_H);
	gyroData[2]=(BUF[5]<<8)|BUF[4];
    }
    else
    {
    BUF[0]=MPU9250_Read_Reg(GYRO_ADDRESS_2,GYRO_XOUT_L);
	BUF[1]=MPU9250_Read_Reg(GYRO_ADDRESS_2,GYRO_XOUT_H);
	gyroData[0]=(BUF[1]<<8)|BUF[0];
	//gyroData[0]/=16.4;

	BUF[2]=MPU9250_Read_Reg(GYRO_ADDRESS_2,GYRO_YOUT_L);
	BUF[3]=MPU9250_Read_Reg(GYRO_ADDRESS_2,GYRO_YOUT_H);
	gyroData[1]=(BUF[3]<<8)|BUF[2];
	//gyroData[1]/=16.4;

	BUF[4]=MPU9250_Read_Reg(GYRO_ADDRESS_2,GYRO_ZOUT_L);
	BUF[5]=MPU9250_Read_Reg(GYRO_ADDRESS_2,GYRO_ZOUT_H);
	gyroData[2]=(BUF[5]<<8)|BUF[4];   
    }
}


void MPU9250_2_READ_GYRO(int a,short *gyroData)
{
	uint8_t BUF[8];
    if(a==0)
    {
	BUF[0]=MPU9250_2_Read_Reg(GYRO_ADDRESS,GYRO_XOUT_L);
	BUF[1]=MPU9250_2_Read_Reg(GYRO_ADDRESS,GYRO_XOUT_H);
	gyroData[0]=(BUF[1]<<8)|BUF[0];
	//gyroData[0]/=16.4;

	BUF[2]=MPU9250_2_Read_Reg(GYRO_ADDRESS,GYRO_YOUT_L);
	BUF[3]=MPU9250_2_Read_Reg(GYRO_ADDRESS,GYRO_YOUT_H);
	gyroData[1]=(BUF[3]<<8)|BUF[2];
	//gyroData[1]/=16.4;

	BUF[4]=MPU9250_2_Read_Reg(GYRO_ADDRESS,GYRO_ZOUT_L);
	BUF[5]=MPU9250_2_Read_Reg(GYRO_ADDRESS,GYRO_ZOUT_H);
	gyroData[2]=(BUF[5]<<8)|BUF[4];
    }
    else
    {
    BUF[0]=MPU9250_2_Read_Reg(GYRO_ADDRESS_2,GYRO_XOUT_L);
	BUF[1]=MPU9250_2_Read_Reg(GYRO_ADDRESS_2,GYRO_XOUT_H);
	gyroData[0]=(BUF[1]<<8)|BUF[0];
	//gyroData[0]/=16.4;

	BUF[2]=MPU9250_2_Read_Reg(GYRO_ADDRESS_2,GYRO_YOUT_L);
	BUF[3]=MPU9250_2_Read_Reg(GYRO_ADDRESS_2,GYRO_YOUT_H);
	gyroData[1]=(BUF[3]<<8)|BUF[2];
	//gyroData[1]/=16.4;

	BUF[4]=MPU9250_2_Read_Reg(GYRO_ADDRESS_2,GYRO_ZOUT_L);
	BUF[5]=MPU9250_2_Read_Reg(GYRO_ADDRESS_2,GYRO_ZOUT_H);
	gyroData[2]=(BUF[5]<<8)|BUF[4];   
    }
}


void MPU9250_READ_MAG(short *magData)
{ 
	uint8_t BUF[6];
	MPU9250_Write_Reg(GYRO_ADDRESS,INT_PIN_CFG,0x02);//turn on Bypass Mode 
	//delay_ms(10);
	MPU9250_Write_Reg(MAG_ADDRESS,0x0A,0x01);//用来启动单次转换,否则磁力计输出的数据不变
	//delay_ms(10);
		 
	BUF[0]=MPU9250_Read_Reg(MAG_ADDRESS,MAG_XOUT_L);
	BUF[1]=MPU9250_Read_Reg(MAG_ADDRESS,MAG_XOUT_H);
	magData[0]=(BUF[1]<<8)|BUF[0];

	BUF[2]=MPU9250_Read_Reg(MAG_ADDRESS,MAG_YOUT_L);
	BUF[3]=MPU9250_Read_Reg(MAG_ADDRESS,MAG_YOUT_H);
	magData[1]=(BUF[3]<<8)|BUF[2];
		 
	BUF[4]=MPU9250_Read_Reg(MAG_ADDRESS,MAG_ZOUT_L);
	BUF[5]=MPU9250_Read_Reg(MAG_ADDRESS,MAG_ZOUT_H);
	magData[2]=(BUF[5]<<8)|BUF[4];
}

void MPU9250_2_READ_MAG(short *magData)
{ 
	uint8_t BUF[6];
	MPU9250_2_Write_Reg(GYRO_ADDRESS,INT_PIN_CFG,0x02);//turn on Bypass Mode 
	//delay_ms(10);
	MPU9250_2_Write_Reg(MAG_ADDRESS,0x0A,0x01);//用来启动单次转换,否则磁力计输出的数据不变
	//delay_ms(10);
		 
	BUF[0]=MPU9250_2_Read_Reg(MAG_ADDRESS,MAG_XOUT_L);
	BUF[1]=MPU9250_2_Read_Reg(MAG_ADDRESS,MAG_XOUT_H);
	magData[0]=(BUF[1]<<8)|BUF[0];

	BUF[2]=MPU9250_2_Read_Reg(MAG_ADDRESS,MAG_YOUT_L);
	BUF[3]=MPU9250_2_Read_Reg(MAG_ADDRESS,MAG_YOUT_H);
	magData[1]=(BUF[3]<<8)|BUF[2];
		 
	BUF[4]=MPU9250_2_Read_Reg(MAG_ADDRESS,MAG_ZOUT_L);
	BUF[5]=MPU9250_2_Read_Reg(MAG_ADDRESS,MAG_ZOUT_H);
	magData[2]=(BUF[5]<<8)|BUF[4];
}











