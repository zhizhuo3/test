#include "I2C_driver_master_top.h"
#include "soft_i2c.h"
#include "esp_log.h"
#include "mpu9250_dri.h"
#include "esp_check.h"
#include "esp_log.h"
#define I2C_SLV0_ADDR                       0x25  
#define I2C_SLV0_REG                        0x26
#define I2C_SLV0_CTRL                       0x27 
#define I2C_SLV0_DO                         0x63 //output reg
#define EXT_SENS_DATA_00                    0x49  //MPU9250 IIC外挂器件读取返回寄存器00
#define INT_PIN_CFG                         0x37 //中断配置
#define USER_CTRL                           0x6a
#define I2C_MST_CTRL                        0x24
#define I2C_MST_DELAY_CTRL                  0x67

#define MPU9250_AK8963_ADDR                 0x0C  //AKM addr
#define AK8963_WHOAMI_REG                   0x00  //AKM ID addr
#define AK8963_WHOAMI_ID                    0x48  //ID
#define AK8963_ST1_REG                      0x02  //Data Status1
#define AK8963_ST2_REG                      0x09  //Data reading end register & check Magnetic sensor overflow occurred
#define AK8963_ST1_DOR                      0x02
#define AK8963_ST1_DRDY                     0x01 //Data Ready
#define AK8963_ST2_BITM                     0x10
#define AK8963_ST2_HOFL                     0x08 // Magnetic sensor overflow 
#define AK8963_CNTL1_REG                    0x0A
#define AK8963_CNTL2_REG                    0x0B
#define AK8963_CNTL2_SRST                   0x01 //soft Reset
#define AK8963_ASAX                         0x10 //X-axis sensitivity adjustment value 
#define AK8963_ASAY                         0x11 //Y-axis sensitivity adjustment value
#define AK8963_ASAZ                         0x12 //Z-axis sensitivity adjustment value



const char *TAG1 = "1";
const char *TAG2 = "2";
const char *TAG3 = "3";
const char *TAG4 = "4";
const char *TAG5 = "5";
const char *TAG6 = "6";
const char *TAG7 = "7";
const char *TAG8 = "8";

const char* tag="mpu9250init";
//////////SNTP协议/////////////////////
       extern int64_t timen;
///////////////////////////////////////
const char* EXAMPLE_TAG1 = "soft_i2c_slave";



//////////////////////////////////////////////////////////////////////排查//////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static esp_err_t mpu9250_register_read(uint8_t sensoraddr,uint8_t reg_addr, uint8_t *data, size_t len)
{
    return i2c_master_write_read_device(I2C_MASTER_NUM, sensoraddr, &reg_addr, 1, data, len, 1000 / portTICK_PERIOD_MS);
}

static esp_err_t mpu9250_register_2read(uint8_t sensoraddr,uint8_t reg_addr, uint8_t *data, size_t len)
{
    return i2c_master_write_read_device(I2C2_MASTER_NUM, sensoraddr, &reg_addr, 1, data, len, 1000 / portTICK_PERIOD_MS);
}

static esp_err_t mpu9250_register_write_byte(uint8_t sensoraddr,uint8_t reg_addr, uint8_t data)
{
    int ret;
    uint8_t write_buf[2] = {reg_addr, data};

    ret = i2c_master_write_to_device(I2C_MASTER_NUM, sensoraddr, write_buf, sizeof(write_buf), 1000 / portTICK_PERIOD_MS);

    return ret;
}


static esp_err_t mpu9250_register_2write_byte(uint8_t sensoraddr,uint8_t reg_addr, uint8_t data)
{
    int ret;
    uint8_t write_buf[2] = {reg_addr, data};

    ret = i2c_master_write_to_device(I2C2_MASTER_NUM, sensoraddr, write_buf, sizeof(write_buf), 1000 / portTICK_PERIOD_MS);

    return ret;
}


///////////////////////////////////////////////////////////////////mpu9250AUXi2c读取/////////延迟需调整////////////////////////////////////////////////////////////////////

void i2c_mpu_mag_wirte(i2c_port_t port, uint8_t addres, uint8_t reg,uint8_t value)
{
uint8_t data_buf[2]={I2C_SLV0_ADDR,0x0C};
i2c_master_write_to_device(port, addres, data_buf, sizeof(data_buf), 1000 / portTICK_PERIOD_MS);//设置磁力计地址，mode：write
data_buf[0]=I2C_SLV0_REG;
data_buf[1]=reg;
i2c_master_write_to_device(port, addres, data_buf, sizeof(data_buf), 1000 / portTICK_PERIOD_MS);//set reg addr
data_buf[0]=I2C_SLV0_DO;
data_buf[1]=value;
i2c_master_write_to_device(port, addres, data_buf, sizeof(data_buf), 1000 / portTICK_PERIOD_MS);//send value	
vTaskDelay(10/portTICK_PERIOD_MS);
}

uint8_t i2c_mpu_mag_read(i2c_port_t port, uint8_t addres,uint8_t reg)
{
uint8_t buf;
uint8_t data_buf[2]={I2C_SLV0_ADDR,0x0C|0x80};
i2c_master_write_to_device(port, addres, data_buf, sizeof(data_buf), 1000 / portTICK_PERIOD_MS);//设置磁力计地址，mode：read
data_buf[0]=I2C_SLV0_REG;
data_buf[1]=reg;
i2c_master_write_to_device(port, addres, data_buf, sizeof(data_buf), 1000 / portTICK_PERIOD_MS);//set reg addr
data_buf[0]=I2C_SLV0_DO;
data_buf[1]=0xff;
i2c_master_write_to_device(port, addres, data_buf, sizeof(data_buf), 1000 / portTICK_PERIOD_MS);//send value	
vTaskDelay(10/portTICK_PERIOD_MS);
if(port == I2C_MASTER_NUM)
{mpu9250_register_read(addres,EXT_SENS_DATA_00,&buf,1);}
else
{mpu9250_register_2read(addres,EXT_SENS_DATA_00,&buf,1);}
return buf;
}

void si2c_mpu_mag_wirte(uint8_t port, uint8_t addres, uint8_t reg,uint8_t value)
{
// uint8_t data_buf[2]={I2C_SLV0_ADDR,0x0C};
// i2c_master_write_to_device(port, addres, data_buf, sizeof(data_buf), 1000 / portTICK_PERIOD_MS);//设置磁力计地址，mode：write
// data_buf[0]=I2C_SLV0_REG;
// data_buf[1]=reg;
// i2c_master_write_to_device(port, addres, data_buf, sizeof(data_buf), 1000 / portTICK_PERIOD_MS);//set reg addr
// data_buf[0]=I2C_SLV0_DO;
// data_buf[1]=value;
// i2c_master_write_to_device(port, addres, data_buf, sizeof(data_buf), 1000 / portTICK_PERIOD_MS);//send value	
// vTaskDelay(10/portTICK_PERIOD_MS);
if(port == 0X03)
{
    MPU9250_Write_Reg(addres,I2C_SLV0_ADDR,0x0C);//设置磁力计地址，mode：write
    //加延迟
    MPU9250_Write_Reg(addres,I2C_SLV0_REG,reg);//set reg addr
    //加延迟
    MPU9250_Write_Reg(addres,I2C_SLV0_DO,value);//send value 
    vTaskDelay(10/portTICK_PERIOD_MS);
}
else
{
    MPU9250_2_Write_Reg(addres,I2C_SLV0_ADDR,0x0C);//设置磁力计地址，mode：write
    MPU9250_2_Write_Reg(addres,I2C_SLV0_REG,reg);//set reg addr
    MPU9250_2_Write_Reg(addres,I2C_SLV0_DO,value);//send value 
    vTaskDelay(10/portTICK_PERIOD_MS);
}
}


uint8_t si2c_mpu_mag_read(i2c_port_t port, uint8_t addres,uint8_t reg)
{
// uint8_t buf;
// uint8_t data_buf[2]={I2C_SLV0_ADDR,0x0C|0x80};
// i2c_master_write_to_device(port, addres, data_buf, sizeof(data_buf), 1000 / portTICK_PERIOD_MS);//设置磁力计地址，mode：read
// data_buf[0]=I2C_SLV0_REG;
// data_buf[1]=reg;
// i2c_master_write_to_device(port, addres, data_buf, sizeof(data_buf), 1000 / portTICK_PERIOD_MS);//set reg addr
// data_buf[0]=I2C_SLV0_DO;
// data_buf[1]=0xff;
// i2c_master_write_to_device(port, addres, data_buf, sizeof(data_buf), 1000 / portTICK_PERIOD_MS);//send value	
// vTaskDelay(10/portTICK_PERIOD_MS);
// if(port == I2C_MASTER_NUM)
// {mpu9250_register_read(addres,EXT_SENS_DATA_00,&buf,1);}
// else
// {mpu9250_register_2read(addres,EXT_SENS_DATA_00,&buf,1);}
// return buf;
if(port == 0X03)
{
    MPU9250_Write_Reg(addres,I2C_SLV0_ADDR,0x0C|0x80);//设置磁力计地址，mode：write
    //加延迟
    MPU9250_Write_Reg(addres,I2C_SLV0_REG,reg);//set reg addr
    //加延迟
    MPU9250_Write_Reg(addres,I2C_SLV0_DO,0xff);//send value 
    vTaskDelay(10/portTICK_PERIOD_MS);
    uint8_t buf = MPU9250_Read_Reg(addres,EXT_SENS_DATA_00);
    return buf;
}
else
{
    MPU9250_2_Write_Reg(addres,I2C_SLV0_ADDR,0x0C|0x80);//设置磁力计地址，mode：write
    //加延迟
    MPU9250_2_Write_Reg(addres,I2C_SLV0_REG,reg);//set reg addr
    //加延迟
    MPU9250_2_Write_Reg(addres,I2C_SLV0_DO,0xff);//send value 
    vTaskDelay(10/portTICK_PERIOD_MS);
    uint8_t buf = MPU9250_2_Read_Reg(addres,EXT_SENS_DATA_00);
    return buf;
}
}



/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////mpu9250初始化/////////////////////////////////////////////////////////////
void mpu9250_init_hardi2c1(){
    uint8_t data_buf[2];
    data_buf[0] = 0x6B; /// 寄存器地址
    data_buf[1] = 0x00;    /// 唤醒 MPU9250
    i2c_master_write_to_device(I2C_MASTER_NUM, MPU9250_ADDRESS, data_buf, sizeof(data_buf), 1000 / portTICK_PERIOD_MS);
    i2c_master_write_to_device(I2C_MASTER_NUM, MPU9250_ADDRESS2, data_buf, sizeof(data_buf), 1000 / portTICK_PERIOD_MS);

    data_buf[0] = 0x37;    /// 
    data_buf[1] = 0x02;    /// 唤醒 MPU9250
    i2c_master_write_to_device(I2C_MASTER_NUM, MPU9250_ADDRESS, data_buf, sizeof(data_buf), 1000 / portTICK_PERIOD_MS);
    i2c_master_write_to_device(I2C_MASTER_NUM, MPU9250_ADDRESS2, data_buf, sizeof(data_buf), 1000 / portTICK_PERIOD_MS);

    //陀螺仪采样率设置
    data_buf[0]= 0x19;   //SMPLRT_DIV寄存器地址
    data_buf[1]= 0x07;   //1KHz 
    i2c_master_write_to_device(I2C_MASTER_NUM, MPU9250_ADDRESS, data_buf, sizeof(data_buf), 1000 / portTICK_PERIOD_MS);
    i2c_master_write_to_device(I2C_MASTER_NUM, MPU9250_ADDRESS2, data_buf, sizeof(data_buf), 1000 / portTICK_PERIOD_MS);

    data_buf[0]= 0x1B;   //GYRO_CONFIG寄存器地址
    data_buf[1]= 0x18;   //关自检，2000deg/s
    i2c_master_write_to_device(I2C_MASTER_NUM, MPU9250_ADDRESS, data_buf, sizeof(data_buf), 1000 / portTICK_PERIOD_MS);
    i2c_master_write_to_device(I2C_MASTER_NUM, MPU9250_ADDRESS2, data_buf, sizeof(data_buf), 1000 / portTICK_PERIOD_MS);

    data_buf[0]= 0x1C;   //ACCEL_CONFIG寄存器地址
    data_buf[1]= 0<<3;   //自检，16g
    i2c_master_write_to_device(I2C_MASTER_NUM, MPU9250_ADDRESS, data_buf, sizeof(data_buf), 1000 / portTICK_PERIOD_MS);
    i2c_master_write_to_device(I2C_MASTER_NUM, MPU9250_ADDRESS2, data_buf, sizeof(data_buf), 1000 / portTICK_PERIOD_MS);

    data_buf[0]= INT_PIN_CFG;   //ACCEL_CONFIG寄存器地址
    data_buf[1]= 0x02;   //自检，16g
    i2c_master_write_to_device(I2C_MASTER_NUM, MPU9250_ADDRESS, data_buf, sizeof(data_buf), 1000 / portTICK_PERIOD_MS);
    i2c_master_write_to_device(I2C_MASTER_NUM, MPU9250_ADDRESS2, data_buf, sizeof(data_buf), 1000 / portTICK_PERIOD_MS);
	vTaskDelay(10/portTICK_PERIOD_MS);


//IMU1
    data_buf[0]=INT_PIN_CFG;// INT Pin / Bypass Enable Configuration  
    data_buf[1]=0x30;
    i2c_master_write_to_device(I2C_MASTER_NUM, MPU9250_ADDRESS, data_buf, sizeof(data_buf), 1000 / portTICK_PERIOD_MS);
    data_buf[0]=I2C_MST_CTRL;//I2C MAster mode and Speed 400 kHz
    data_buf[1]=0x4d;
    i2c_master_write_to_device(I2C_MASTER_NUM, MPU9250_ADDRESS, data_buf, sizeof(data_buf), 1000 / portTICK_PERIOD_MS);
    data_buf[0]=USER_CTRL;//I2C_MST _EN 
    data_buf[1]=0x20;    
    i2c_master_write_to_device(I2C_MASTER_NUM, MPU9250_ADDRESS, data_buf, sizeof(data_buf), 1000 / portTICK_PERIOD_MS);
    data_buf[0]=I2C_MST_DELAY_CTRL;//延时使能I2C_SLV0 _DLY_ enable 
    data_buf[1]=0x01;   
    i2c_master_write_to_device(I2C_MASTER_NUM, MPU9250_ADDRESS, data_buf, sizeof(data_buf), 1000 / portTICK_PERIOD_MS);
    data_buf[0]=I2C_SLV0_CTRL;//enable IIC	and EXT_SENS_DATA==1 Byte	
    data_buf[1]=0x81;   
    i2c_master_write_to_device(I2C_MASTER_NUM, MPU9250_ADDRESS, data_buf, sizeof(data_buf), 1000 / portTICK_PERIOD_MS);
    i2c_mpu_mag_wirte(I2C_MASTER_NUM,MPU9250_ADDRESS,AK8963_CNTL2_REG,AK8963_CNTL2_SRST);// Reset AK8963
    i2c_mpu_mag_wirte(I2C_MASTER_NUM,MPU9250_ADDRESS,AK8963_CNTL1_REG,0x11);// use i2c to set AK8963 working on Continuous measurement mode1 & 16-bit output & 100Hz
    vTaskDelay(10/portTICK_PERIOD_MS);
//IMU2
    data_buf[0]=INT_PIN_CFG;// INT Pin / Bypass Enable Configuration  
    data_buf[1]=0x30;
    i2c_master_write_to_device(I2C_MASTER_NUM, MPU9250_ADDRESS2, data_buf, sizeof(data_buf), 1000 / portTICK_PERIOD_MS);
    data_buf[0]=I2C_MST_CTRL;//I2C MAster mode and Speed 400 kHz
    data_buf[1]=0x4d;
    i2c_master_write_to_device(I2C_MASTER_NUM, MPU9250_ADDRESS2, data_buf, sizeof(data_buf), 1000 / portTICK_PERIOD_MS);
    data_buf[0]=USER_CTRL;//I2C_MST _EN 
    data_buf[1]=0x20;    
    i2c_master_write_to_device(I2C_MASTER_NUM, MPU9250_ADDRESS2, data_buf, sizeof(data_buf), 1000 / portTICK_PERIOD_MS);
    data_buf[0]=I2C_MST_DELAY_CTRL;//延时使能I2C_SLV0 _DLY_ enable 
    data_buf[1]=0x01;   
    i2c_master_write_to_device(I2C_MASTER_NUM, MPU9250_ADDRESS2, data_buf, sizeof(data_buf), 1000 / portTICK_PERIOD_MS);
    data_buf[0]=I2C_SLV0_CTRL;//enable IIC	and EXT_SENS_DATA==1 Byte	
    data_buf[1]=0x81;   
    i2c_master_write_to_device(I2C_MASTER_NUM, MPU9250_ADDRESS2, data_buf, sizeof(data_buf), 1000 / portTICK_PERIOD_MS);
    i2c_mpu_mag_wirte(I2C_MASTER_NUM,MPU9250_ADDRESS2,AK8963_CNTL2_REG,AK8963_CNTL2_SRST);// Reset AK8963
    i2c_mpu_mag_wirte(I2C_MASTER_NUM,MPU9250_ADDRESS2,AK8963_CNTL1_REG,0x11);// use i2c to set AK8963 working on Continuous measurement mode1 & 16-bit output & 100Hz
};




void mpu9250_init_hardi2c2(){
    uint8_t data_buf[2];
    data_buf[0] = 0x6B; /// 寄存器地址
    data_buf[1] = 0x00;    /// 唤醒 MPU9250
    i2c_master_write_to_device(I2C2_MASTER_NUM, MPU9250_ADDRESS, data_buf, sizeof(data_buf), 1000 / portTICK_PERIOD_MS);
    i2c_master_write_to_device(I2C2_MASTER_NUM, MPU9250_ADDRESS2, data_buf, sizeof(data_buf), 1000 / portTICK_PERIOD_MS);

    data_buf[0] = 0x37;    /// 
    data_buf[1] = 0x02;    /// 唤醒 MPU9250
    i2c_master_write_to_device(I2C2_MASTER_NUM, MPU9250_ADDRESS, data_buf, sizeof(data_buf), 1000 / portTICK_PERIOD_MS);
    i2c_master_write_to_device(I2C2_MASTER_NUM, MPU9250_ADDRESS2, data_buf, sizeof(data_buf), 1000 / portTICK_PERIOD_MS);

    //陀螺仪采样率设置
    data_buf[0]= 0x19;   //SMPLRT_DIV寄存器地址
    data_buf[1]= 0x07;   //1KHz 
    i2c_master_write_to_device(I2C2_MASTER_NUM, MPU9250_ADDRESS, data_buf, sizeof(data_buf), 1000 / portTICK_PERIOD_MS);
    i2c_master_write_to_device(I2C2_MASTER_NUM, MPU9250_ADDRESS2, data_buf, sizeof(data_buf), 1000 / portTICK_PERIOD_MS);

    data_buf[0]= 0x1B;   //GYRO_CONFIG寄存器地址
    data_buf[1]= 0x18;   //自检，2000deg/s
    i2c_master_write_to_device(I2C2_MASTER_NUM, MPU9250_ADDRESS, data_buf, sizeof(data_buf), 1000 / portTICK_PERIOD_MS);
    i2c_master_write_to_device(I2C2_MASTER_NUM, MPU9250_ADDRESS2, data_buf, sizeof(data_buf), 1000 / portTICK_PERIOD_MS);

    data_buf[0]= 0x1C;   //ACCEL_CONFIG寄存器地址
    data_buf[1]= 0<<3;   //自检，16g
    i2c_master_write_to_device(I2C2_MASTER_NUM, MPU9250_ADDRESS, data_buf, sizeof(data_buf), 1000 / portTICK_PERIOD_MS);
    i2c_master_write_to_device(I2C2_MASTER_NUM, MPU9250_ADDRESS2, data_buf, sizeof(data_buf), 1000 / portTICK_PERIOD_MS);

//IMU1
    data_buf[0]=INT_PIN_CFG;// INT Pin / Bypass Enable Configuration  
    data_buf[1]=0x30;
    i2c_master_write_to_device(I2C2_MASTER_NUM, MPU9250_ADDRESS, data_buf, sizeof(data_buf), 1000 / portTICK_PERIOD_MS);
    data_buf[0]=I2C_MST_CTRL;//I2C MAster mode and Speed 400 kHz
    data_buf[1]=0x4d;
    i2c_master_write_to_device(I2C2_MASTER_NUM, MPU9250_ADDRESS, data_buf, sizeof(data_buf), 1000 / portTICK_PERIOD_MS);
    data_buf[0]=USER_CTRL;//I2C_MST _EN 
    data_buf[1]=0x20;    
    i2c_master_write_to_device(I2C2_MASTER_NUM, MPU9250_ADDRESS, data_buf, sizeof(data_buf), 1000 / portTICK_PERIOD_MS);
    data_buf[0]=I2C_MST_DELAY_CTRL;//延时使能I2C_SLV0 _DLY_ enable 
    data_buf[1]=0x01;   
    i2c_master_write_to_device(I2C2_MASTER_NUM, MPU9250_ADDRESS, data_buf, sizeof(data_buf), 1000 / portTICK_PERIOD_MS);
    data_buf[0]=I2C_SLV0_CTRL;//enable IIC	and EXT_SENS_DATA==1 Byte	
    data_buf[1]=0x81;   
    i2c_master_write_to_device(I2C2_MASTER_NUM, MPU9250_ADDRESS, data_buf, sizeof(data_buf), 1000 / portTICK_PERIOD_MS);
    i2c_mpu_mag_wirte(I2C2_MASTER_NUM,MPU9250_ADDRESS,AK8963_CNTL2_REG,AK8963_CNTL2_SRST);// Reset AK8963
    i2c_mpu_mag_wirte(I2C2_MASTER_NUM,MPU9250_ADDRESS,AK8963_CNTL1_REG,0x11);// use i2c to set AK8963 working on Continuous measurement mode1 & 16-bit output & 100Hz

//IMU2
    data_buf[0]=INT_PIN_CFG;// INT Pin / Bypass Enable Configuration  
    data_buf[1]=0x30;
    i2c_master_write_to_device(I2C2_MASTER_NUM, MPU9250_ADDRESS2, data_buf, sizeof(data_buf), 1000 / portTICK_PERIOD_MS);
    data_buf[0]=I2C_MST_CTRL;//I2C MAster mode and Speed 400 kHz
    data_buf[1]=0x4d;
    i2c_master_write_to_device(I2C2_MASTER_NUM, MPU9250_ADDRESS2, data_buf, sizeof(data_buf), 1000 / portTICK_PERIOD_MS);
    data_buf[0]=USER_CTRL;//I2C_MST _EN 
    data_buf[1]=0x20;    
    i2c_master_write_to_device(I2C2_MASTER_NUM, MPU9250_ADDRESS2, data_buf, sizeof(data_buf), 1000 / portTICK_PERIOD_MS);
    data_buf[0]=I2C_MST_DELAY_CTRL;//延时使能I2C_SLV0 _DLY_ enable 
    data_buf[1]=0x01;   
    i2c_master_write_to_device(I2C2_MASTER_NUM, MPU9250_ADDRESS2, data_buf, sizeof(data_buf), 1000 / portTICK_PERIOD_MS);
    data_buf[0]=I2C_SLV0_CTRL;//enable IIC	and EXT_SENS_DATA==1 Byte	
    data_buf[1]=0x81;   
    i2c_master_write_to_device(I2C2_MASTER_NUM, MPU9250_ADDRESS2, data_buf, sizeof(data_buf), 1000 / portTICK_PERIOD_MS);
    i2c_mpu_mag_wirte(I2C2_MASTER_NUM,MPU9250_ADDRESS2,AK8963_CNTL2_REG,AK8963_CNTL2_SRST);// Reset AK8963
    i2c_mpu_mag_wirte(I2C2_MASTER_NUM,MPU9250_ADDRESS2,AK8963_CNTL1_REG,0x11);// use i2c to set AK8963 working on Continuous measurement mode1 & 16-bit output & 100Hz

};

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////可能有问题需要排查///////////////////////////////////////////////////////////////////////


uint8_t MPU92501_Init(void)
{
    uint8_t id = MPU9250_Read_Reg(GYRO_ADDRESS,WHO_AM_I);
	// if(id==0x71)
	// {
		//MPU9250_Write_Reg(GYRO_ADDRESS,GYRO_ADDRESS,PWR_MGMT_1,0X80);	//电源管理,复位MPU9250
		MPU9250_Write_Reg(GYRO_ADDRESS,PWR_MGMT_1,0x00);	//解除休眠状态
		MPU9250_Write_Reg(GYRO_ADDRESS,SMPLRT_DIV,0x07);	//采样频率125Hz
		MPU9250_Write_Reg(GYRO_ADDRESS,CONFIG,0X06);			//低通滤波器5Hz
		MPU9250_Write_Reg(GYRO_ADDRESS,GYRO_CONFIG,0X18);//陀螺仪量程,正负2000度
		MPU9250_Write_Reg(GYRO_ADDRESS,ACCEL_CONFIG,0<<3);//加速度量程,正负16g
		//AK8963初始化
		MPU9250_Write_Reg(GYRO_ADDRESS,INT_PIN_CFG,0x30);//INT Pin / Bypass Enable Configuration
		MPU9250_Write_Reg(GYRO_ADDRESS,I2C_MST_CTRL,0x4d);//INT Pin / Bypass Enable Configuration
		MPU9250_Write_Reg(GYRO_ADDRESS,USER_CTRL,0x20);//INT Pin / Bypass Enable Configuration	
		MPU9250_Write_Reg(GYRO_ADDRESS,I2C_MST_DELAY_CTRL,0x01);//INT Pin / Bypass Enable Configuration	
		MPU9250_Write_Reg(GYRO_ADDRESS,I2C_SLV0_CTRL,0x81);//INT Pin / Bypass Enable Configuration						
        i2c_mpu_mag_wirte(0x03,GYRO_ADDRESS,AK8963_CNTL2_REG,AK8963_CNTL2_SRST);// Reset AK8963
        i2c_mpu_mag_wirte(0x03,GYRO_ADDRESS,AK8963_CNTL1_REG,0x11);// use i2c to set AK8963 working on continuous measurement mode1 & 16-bit output & 100Hz

	 	return 0;
	// }
	// return 1;
}

//I2C3-MPU9250-2
uint8_t MPU92502_Init(void)
{
	// if(MPU9250_Read_Reg(GYRO_ADDRESS_2,WHO_AM_I)==0x71)
	// {
		//MPU9250_Write_Reg(GYRO_ADDRESS,GYRO_ADDRESS,PWR_MGMT_1,0X80);	//电源管理,复位MPU9250
		MPU9250_Write_Reg(GYRO_ADDRESS_2,PWR_MGMT_1,0x00);	//解除休眠状态
		MPU9250_Write_Reg(GYRO_ADDRESS_2,SMPLRT_DIV,0x07);	//采样频率125Hz
		MPU9250_Write_Reg(GYRO_ADDRESS_2,CONFIG,0X06);			//低通滤波器5Hz
		MPU9250_Write_Reg(GYRO_ADDRESS_2,GYRO_CONFIG,0X18);//陀螺仪量程,正负2000度
		MPU9250_Write_Reg(GYRO_ADDRESS_2,ACCEL_CONFIG,0<<3);//加速度量程,正负16g
        //AK8963初始化
		MPU9250_Write_Reg(GYRO_ADDRESS_2,INT_PIN_CFG,0x30);//INT Pin / Bypass Enable Configuration
		MPU9250_Write_Reg(GYRO_ADDRESS_2,I2C_MST_CTRL,0x4d);//INT Pin / Bypass Enable Configuration
		MPU9250_Write_Reg(GYRO_ADDRESS_2,USER_CTRL,0x20);//INT Pin / Bypass Enable Configuration	
		MPU9250_Write_Reg(GYRO_ADDRESS_2,I2C_MST_DELAY_CTRL,0x01);//INT Pin / Bypass Enable Configuration	
		MPU9250_Write_Reg(GYRO_ADDRESS_2,I2C_SLV0_CTRL,0x81);//INT Pin / Bypass Enable Configuration						
        i2c_mpu_mag_wirte(0x03,GYRO_ADDRESS_2,AK8963_CNTL2_REG,AK8963_CNTL2_SRST);// Reset AK8963
        i2c_mpu_mag_wirte(0x03,GYRO_ADDRESS_2,AK8963_CNTL1_REG,0x11);// use i2c to set AK8963 working on continuous measurement mode1 & 16-bit output & 100Hz

	 	return 0;
	// }
	// return 1;
}


//I2C4-MPU9250-1
uint8_t MPU92501_2_Init(void)
{
	// if(MPU9250_2_Read_Reg(GYRO_ADDRESS,WHO_AM_I)==0x71)
	// {
		//MPU9250_Write_Reg(GYRO_ADDRESS,GYRO_ADDRESS,PWR_MGMT_1,0X80);	//电源管理,复位MPU9250
		MPU9250_2_Write_Reg(GYRO_ADDRESS,PWR_MGMT_1,0x00);//解除休眠状态
		MPU9250_2_Write_Reg(GYRO_ADDRESS,SMPLRT_DIV,0x07);//采样频率125Hz
		MPU9250_2_Write_Reg(GYRO_ADDRESS,CONFIG,0X06);//低通滤波器5Hz
		MPU9250_2_Write_Reg(GYRO_ADDRESS,GYRO_CONFIG,0X18);//陀螺仪量程,正负2000度
		MPU9250_2_Write_Reg(GYRO_ADDRESS,ACCEL_CONFIG,0<<3);//加速度量程,正负16g
        //AK8963初始化
		MPU9250_Write_Reg(GYRO_ADDRESS,INT_PIN_CFG,0x30);//INT Pin / Bypass Enable Configuration
		MPU9250_Write_Reg(GYRO_ADDRESS,I2C_MST_CTRL,0x4d);//INT Pin / Bypass Enable Configuration
		MPU9250_Write_Reg(GYRO_ADDRESS,USER_CTRL,0x20);//INT Pin / Bypass Enable Configuration	
		MPU9250_Write_Reg(GYRO_ADDRESS,I2C_MST_DELAY_CTRL,0x01);//INT Pin / Bypass Enable Configuration	
		MPU9250_Write_Reg(GYRO_ADDRESS,I2C_SLV0_CTRL,0x81);//INT Pin / Bypass Enable Configuration						
        i2c_mpu_mag_wirte(0x04,GYRO_ADDRESS,AK8963_CNTL2_REG,AK8963_CNTL2_SRST);// Reset AK8963
        i2c_mpu_mag_wirte(0x04,GYRO_ADDRESS,AK8963_CNTL1_REG,0x11);// use i2c to set AK8963 working on continuous measurement mode1 & 16-bit output & 100Hz
	 	return 0;
	// }
	// return 1;
}


//I2C4-MPU9250-2
uint8_t MPU92502_2_Init(void)
{
	// if(MPU9250_2_Read_Reg(GYRO_ADDRESS_2,WHO_AM_I)==0x71)
	// {
		//MPU9250_Write_Reg(GYRO_ADDRESS,GYRO_ADDRESS,PWR_MGMT_1,0X80);	//电源管理,复位MPU9250
		MPU9250_2_Write_Reg(GYRO_ADDRESS_2,PWR_MGMT_1,0x00);//解除休眠状态
		MPU9250_2_Write_Reg(GYRO_ADDRESS_2,SMPLRT_DIV,0x07);//采样频率125Hz
		MPU9250_2_Write_Reg(GYRO_ADDRESS_2,CONFIG,0X06);//低通滤波器5Hz
		MPU9250_2_Write_Reg(GYRO_ADDRESS_2,GYRO_CONFIG,0X18);//陀螺仪量程,正负2000度
		MPU9250_2_Write_Reg(GYRO_ADDRESS_2,ACCEL_CONFIG,0<<3);//加速度量程,正负16g
        //AK8963初始化
		MPU9250_Write_Reg(GYRO_ADDRESS_2,INT_PIN_CFG,0x30);//INT Pin / Bypass Enable Configuration
		MPU9250_Write_Reg(GYRO_ADDRESS_2,I2C_MST_CTRL,0x4d);//INT Pin / Bypass Enable Configuration
		MPU9250_Write_Reg(GYRO_ADDRESS_2,USER_CTRL,0x20);//INT Pin / Bypass Enable Configuration	
		MPU9250_Write_Reg(GYRO_ADDRESS_2,I2C_MST_DELAY_CTRL,0x01);//INT Pin / Bypass Enable Configuration	
		MPU9250_Write_Reg(GYRO_ADDRESS_2,I2C_SLV0_CTRL,0x81);//INT Pin / Bypass Enable Configuration						
        i2c_mpu_mag_wirte(0x04,GYRO_ADDRESS_2,AK8963_CNTL2_REG,AK8963_CNTL2_SRST);// Reset AK8963
        i2c_mpu_mag_wirte(0x04,GYRO_ADDRESS_2,AK8963_CNTL1_REG,0x11);// use i2c to set AK8963 working on continuous measurement mode1 & 16-bit output & 100Hz
	 	return 0;
	// }
	// return 1;
}




void mpu9250_init_softi2c3()
{
    myI2C_Init();
    MPU92501_Init();
	// while(MPU92501_Init())
	// {
    //     ESP_LOGE(tag, "MPU92501 error !!\r\n");
	// 	vTaskDelay(500/portTICK_PERIOD_MS);
    //     ESP_LOGE(tag, "Please check !!\r\n");
	// 	vTaskDelay(500/portTICK_PERIOD_MS);
	// }
    ESP_LOGE(tag, "finish mpu9250init!!\r\n");
    MPU92502_Init();
	// while(MPU92502_Init())
	// {
    //     ESP_LOGE(tag, "MPU92502 error !!\r\n");
	// 	vTaskDelay(500/portTICK_PERIOD_MS);
    //     ESP_LOGE(tag, "Please check !!\r\n");
	// 	vTaskDelay(500/portTICK_PERIOD_MS);
	// }
    ESP_LOGE(tag, "finish mpu9250init!!\r\n");

}


void mpu9250_init_softi2c4()
{
    myI2C_2_Init();
    MPU92501_2_Init();
	// while(MPU92501_2_Init())
	// {
    //     ESP_LOGE(tag, "MPU92503 error !!\r\n");
	// 	vTaskDelay(500/portTICK_PERIOD_MS);
    //     ESP_LOGE(tag, "Please check !!\r\n");
	// 	vTaskDelay(500/portTICK_PERIOD_MS);
	// }
ESP_LOGE(tag, "finish mpu9250init!!\r\n");
MPU92502_2_Init();
	// while(MPU92502_2_Init())
	// {
    //     ESP_LOGE(tag, "MPU92504 error !!\r\n");
	// 	vTaskDelay(500/portTICK_PERIOD_MS);
    //     ESP_LOGE(tag, "Please check !!\r\n");
	// 	vTaskDelay(500/portTICK_PERIOD_MS);
	// }
ESP_LOGE(tag, "finish mpu9250init!!\r\n");


};


////////////////////////////////////////////////////////////////mpu9250初始化//////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void  read_mpu9250_data_hard(int a,int16_t *calldata,uint8_t i) {
    
    uint8_t data[14];

    if(a==0)
   {
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    // 向 MPU9250 请求数据
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, MPU9250_ADDRESS << 1 | I2C_MASTER_WRITE, true);
    i2c_master_write_byte(cmd, 0x3B, true);  // 加速度计数据起始地址
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, MPU9250_ADDRESS << 1 | I2C_MASTER_READ, true);
    i2c_master_read(cmd, data, 14, I2C_MASTER_LAST_NACK);
    i2c_master_stop(cmd);
    i2c_master_cmd_begin(I2C_MASTER_NUM, cmd, 10 / portTICK_PERIOD_MS);
    i2c_cmd_link_delete(cmd);
    // 将数据解析为整数
    calldata[0] = (data[0] << 8) | data[1];
    calldata[1] = (data[2] << 8) | data[3];
    calldata[2] = (data[4] << 8) | data[5];
    calldata[3] = (data[8] << 8) | data[9];
    calldata[4] = (data[10] << 8) | data[11];
    calldata[5] = (data[12] << 8) | data[13];

    uint8_t BUF[6]={0,0,0,0,0,0};
//     uint8_t stat=0;
//      mpu9250_register_read(0X0C  ,0X02, &stat, 1);
// //    if((stat&0x01)==1)
//  //   {      
//     //  uint8_t stat2=0;
//     //  mpu9250_register_read(0X0C  ,0X09, &stat2, 1);检测数据有无问题
  
//         mpu9250_register_read(0X0C  ,0X03, &data_1[0], 1);
//         mpu9250_register_read(0X0C  ,0X04, &data_1[1], 1);
//         mpu9250_register_read(0X0C  ,0X05, &data_1[2], 1);
//         mpu9250_register_read(0X0C  ,0X06, &data_1[3], 1);
//         mpu9250_register_read(0X0C  ,0X07, &data_1[4], 1);
//         mpu9250_register_read(0X0C  ,0X08, &data_1[5], 1);
//   //  }
//     //mpu9250_register_read(0X0C  ,0X03, data_1, 6);
//     calldata[6] = (((int16_t)data_1[0]) << 8) | data_1[1]*(((175-128)>>8)+1);
//     calldata[7] = (((int16_t)data_1[2]) << 8) | data_1[3]*(((175-128)>>8)+1);
//     calldata[8] = (((int16_t)data_1[4]) << 8) | data_1[5]*(((164-128)>>8)+1);

//     mpu9250_register_write_byte(0X0C  ,0X0A, 0x11);//单次测量模式
//     vTaskDelay(5/portTICK_PERIOD_MS);
//保护代码
if(i==5)
{
    //     uint8_t id = i2c_mpu_mag_read(I2C_MASTER_NUM,MPU9250_ADDRESS,0x00);
	// 	 BUF[0]=i2c_mpu_mag_read(I2C_MASTER_NUM,MPU9250_ADDRESS,MAG_XOUT_L); //Low data	
    //     vTaskDelay(1/portTICK_PERIOD_MS);
	// 	 BUF[1]=i2c_mpu_mag_read(I2C_MASTER_NUM,MPU9250_ADDRESS,MAG_XOUT_H); //High data	
    //      vTaskDelay(1/portTICK_PERIOD_MS);
	// 	 BUF[2]=i2c_mpu_mag_read(I2C_MASTER_NUM,MPU9250_ADDRESS,MAG_YOUT_L); //Low data	
    //     vTaskDelay(1/portTICK_PERIOD_MS);
	// 	 BUF[3]=i2c_mpu_mag_read(I2C_MASTER_NUM,MPU9250_ADDRESS,MAG_YOUT_H); //High data	
    //       vTaskDelay(1/portTICK_PERIOD_MS);
    //      BUF[4]=i2c_mpu_mag_read(I2C_MASTER_NUM,MPU9250_ADDRESS,MAG_ZOUT_L); //Low data	
    //     vTaskDelay(1/portTICK_PERIOD_MS);
	// 	 BUF[5]=i2c_mpu_mag_read(I2C_MASTER_NUM,MPU9250_ADDRESS,MAG_ZOUT_H); //High data	
    // i2c_mpu_mag_wirte(I2C_MASTER_NUM,MPU9250_ADDRESS,AK8963_CNTL1_REG,0x11);// use i2c to set AK8963 working on Continuous measurement mode1 & 16-bit output & 100Hz
    // vTaskDelay(1/portTICK_PERIOD_MS);
    	calldata[6]=((BUF[1]<<8)|BUF[0])*(((175-128)>>8)+1);		//灵敏度纠正 公式见/RM-MPU-9250A-00 PDF/ 5.13
        calldata[7]=((BUF[3]<<8)|BUF[2])*(((175-128)>>8)+1);
        calldata[8]=((BUF[5]<<8)|BUF[4])*(((164-128)>>8)+1);
}
        //保护代码
//    uint8_t id=0;
    // mpu9250_register_read(0X0C,0x00,&id,1);
    //  ESP_LOGI(TAG1, "%u",id);
    // 打印数据  
   // ESP_LOGI(TAG1, "%d %d %d %d %d %d %d %d %d %lld", calldata[0], calldata[1], calldata[2], calldata[3], calldata[4], calldata[5], calldata[6], calldata[7], calldata[8], timen);
    }
    else
    {
    // 向 MPU9250 请求数据
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, MPU9250_ADDRESS2 << 1 | I2C_MASTER_WRITE, true);
    i2c_master_write_byte(cmd, 0x3B, true);  // 加速度计数据起始地址
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, MPU9250_ADDRESS2 << 1 | I2C_MASTER_READ, true);
    i2c_master_read(cmd, data, 14, I2C_MASTER_LAST_NACK);
    i2c_master_stop(cmd);
    i2c_master_cmd_begin(I2C_MASTER_NUM, cmd, 10 / portTICK_PERIOD_MS);
    i2c_cmd_link_delete(cmd);

    // 将数据解析为整数
    calldata[0] = (data[0] << 8) | data[1];
    calldata[1] = (data[2] << 8) | data[3];
    calldata[2] = (data[4] << 8) | data[5];
    calldata[3] = (data[8] << 8) | data[9];
    calldata[4] = (data[10] << 8) | data[11];
    calldata[5] = (data[12] << 8) | data[13];

    uint8_t BUF[6]={0,0,0,0,0,0};
if(i==5)
{
    //     uint8_t id = i2c_mpu_mag_read(I2C_MASTER_NUM,MPU9250_ADDRESS2,0x00);
	// 	 BUF[0]=i2c_mpu_mag_read(I2C_MASTER_NUM,MPU9250_ADDRESS2,MAG_XOUT_L); //Low data	
    //     vTaskDelay(1/portTICK_PERIOD_MS);
	// 	 BUF[1]=i2c_mpu_mag_read(I2C_MASTER_NUM,MPU9250_ADDRESS2,MAG_XOUT_H); //High data	
    //      vTaskDelay(1/portTICK_PERIOD_MS);
	// 	 BUF[2]=i2c_mpu_mag_read(I2C_MASTER_NUM,MPU9250_ADDRESS2,MAG_YOUT_L); //Low data	
    //     vTaskDelay(1/portTICK_PERIOD_MS);
	// 	 BUF[3]=i2c_mpu_mag_read(I2C_MASTER_NUM,MPU9250_ADDRESS2,MAG_YOUT_H); //High data	
    //       vTaskDelay(1/portTICK_PERIOD_MS);
    //      BUF[4]=i2c_mpu_mag_read(I2C_MASTER_NUM,MPU9250_ADDRESS2,MAG_ZOUT_L); //Low data	
    //     vTaskDelay(1/portTICK_PERIOD_MS);
	// 	 BUF[5]=i2c_mpu_mag_read(I2C_MASTER_NUM,MPU9250_ADDRESS2,MAG_ZOUT_H); //High data	
    // i2c_mpu_mag_wirte(I2C_MASTER_NUM,MPU9250_ADDRESS2,AK8963_CNTL1_REG,0x11);// use i2c to set AK8963 working on Continuous measurement mode1 & 16-bit output & 100Hz
    // vTaskDelay(1/portTICK_PERIOD_MS);
    	calldata[6]=((BUF[1]<<8)|BUF[0])*(((175-128)>>8)+1);		//灵敏度纠正 公式见/RM-MPU-9250A-00 PDF/ 5.13
        calldata[7]=((BUF[3]<<8)|BUF[2])*(((175-128)>>8)+1);
        calldata[8]=((BUF[5]<<8)|BUF[4])*(((164-128)>>8)+1);
}
    }

}


void read_mpu9250_2_data_hard(int a,int16_t *calldata,uint8_t i) {
    
    uint8_t data[14];
    if(a==0)
    {
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    // 向 MPU9250 请求数据
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, MPU9250_ADDRESS << 1 | I2C_MASTER_WRITE, true);
    i2c_master_write_byte(cmd, 0x3B, true);  // 加速度计数据起始地址
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, MPU9250_ADDRESS << 1 | I2C_MASTER_READ, true);
    i2c_master_read(cmd, data, 14, I2C_MASTER_LAST_NACK);
    i2c_master_stop(cmd);
    i2c_master_cmd_begin(I2C2_MASTER_NUM, cmd, 10 / portTICK_PERIOD_MS);
    i2c_cmd_link_delete(cmd);

    // 将数据解析为整数
    calldata[0] = (data[0] << 8) | data[1];
    calldata[1] = (data[2] << 8) | data[3];
    calldata[2] = (data[4] << 8) | data[5];
    calldata[3] = (data[8] << 8) | data[9];
    calldata[4] = (data[10] << 8) | data[11];
    calldata[5] = (data[12] << 8) | data[13];

    uint8_t BUF[6]={0,0,0,0,0,0};

    // mpu9250_register_2read(0X0C  ,0X03, data_1, 6);

    // calldata[6] = (((int16_t)data_1[0]) << 8) | data_1[1]*(((177-128)>>8)+1);
    // calldata[7] = (((int16_t)data_1[2]) << 8) | data_1[3]*(((178-128)>>8)+1);
    // calldata[8] = (((int16_t)data_1[4]) << 8) | data_1[5]*(((167-128)>>8)+1);

    // mpu9250_register_2write_byte(0X0C  ,0X0A, 0x11);//单次测量模式

    // vTaskDelay(2 / portTICK_PERIOD_MS);

if(i==5)
{
    //     uint8_t id = i2c_mpu_mag_read(I2C2_MASTER_NUM,MPU9250_ADDRESS,0x00);
	// 	 BUF[0]=i2c_mpu_mag_read(I2C2_MASTER_NUM,MPU9250_ADDRESS,MAG_XOUT_L); //Low data	
    //     vTaskDelay(1/portTICK_PERIOD_MS);
	// 	 BUF[1]=i2c_mpu_mag_read(I2C2_MASTER_NUM,MPU9250_ADDRESS,MAG_XOUT_H); //High data	
    //      vTaskDelay(1/portTICK_PERIOD_MS);
	// 	 BUF[2]=i2c_mpu_mag_read(I2C2_MASTER_NUM,MPU9250_ADDRESS,MAG_YOUT_L); //Low data	
    //     vTaskDelay(1/portTICK_PERIOD_MS);
	// 	 BUF[3]=i2c_mpu_mag_read(I2C2_MASTER_NUM,MPU9250_ADDRESS,MAG_YOUT_H); //High data	
    //       vTaskDelay(1/portTICK_PERIOD_MS);
    //      BUF[4]=i2c_mpu_mag_read(I2C2_MASTER_NUM,MPU9250_ADDRESS,MAG_ZOUT_L); //Low data	
    //     vTaskDelay(1/portTICK_PERIOD_MS);
	// 	 BUF[5]=i2c_mpu_mag_read(I2C2_MASTER_NUM,MPU9250_ADDRESS,MAG_ZOUT_H); //High data	
    // i2c_mpu_mag_wirte(I2C2_MASTER_NUM,MPU9250_ADDRESS,AK8963_CNTL1_REG,0x11);// use i2c to set AK8963 working on Continuous measurement mode1 & 16-bit output & 100Hz
    // vTaskDelay(1/portTICK_PERIOD_MS);
    	calldata[6]=((BUF[1]<<8)|BUF[0])*(((175-128)>>8)+1);		//灵敏度纠正 公式见/RM-MPU-9250A-00 PDF/ 5.13
        calldata[7]=((BUF[3]<<8)|BUF[2])*(((175-128)>>8)+1);
        calldata[8]=((BUF[5]<<8)|BUF[4])*(((164-128)>>8)+1);
}


    }

    else
    {
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    // 向 MPU9250 请求数据
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, MPU9250_ADDRESS2 << 1 | I2C_MASTER_WRITE, true);
    i2c_master_write_byte(cmd, 0x3B, true);  // 加速度计数据起始地址
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, MPU9250_ADDRESS2 << 1 | I2C_MASTER_READ, true);
    i2c_master_read(cmd, data, 14, I2C_MASTER_LAST_NACK);
    i2c_master_stop(cmd);
    i2c_master_cmd_begin(I2C2_MASTER_NUM, cmd, 10 / portTICK_PERIOD_MS);
    i2c_cmd_link_delete(cmd);

    // 将数据解析为整数
    calldata[0] = (data[0] << 8) | data[1];
    calldata[1] = (data[2] << 8) | data[3];
    calldata[2] = (data[4] << 8) | data[5];
    calldata[3] = (data[8] << 8) | data[9];
    calldata[4] = (data[10] << 8) | data[11];
    calldata[5] = (data[12] << 8) | data[13];

    uint8_t BUF[6]={0,0,0,0,0,0};

    // mpu9250_register_2read(0X0C  ,0X03, data_1, 6);

    // calldata[6] = (((int16_t)data_1[0]) << 8) | data_1[1]*(((175-128)>>8)+1);
    // calldata[7] = (((int16_t)data_1[2]) << 8) | data_1[3]*(((175-128)>>8)+1);
    // calldata[8] = (((int16_t)data_1[4]) << 8) | data_1[5]*(((165-128)>>8)+1);

    // mpu9250_register_2write_byte(0X0C  ,0X0A, 0x11);//单次测量模式

    // vTaskDelay(2 / portTICK_PERIOD_MS);

if(i==5)
{
    //     uint8_t id = i2c_mpu_mag_read(I2C2_MASTER_NUM,MPU9250_ADDRESS2,0x00);
	// 	BUF[0]=i2c_mpu_mag_read(I2C2_MASTER_NUM,MPU9250_ADDRESS2,MAG_XOUT_L); //Low data	
    //     vTaskDelay(1/portTICK_PERIOD_MS);
	// 	BUF[1]=i2c_mpu_mag_read(I2C2_MASTER_NUM,MPU9250_ADDRESS2,MAG_XOUT_H); //High data	
    //     vTaskDelay(1/portTICK_PERIOD_MS);
	// 	BUF[2]=i2c_mpu_mag_read(I2C2_MASTER_NUM,MPU9250_ADDRESS2,MAG_YOUT_L); //Low data	
    //     vTaskDelay(1/portTICK_PERIOD_MS);
	// 	BUF[3]=i2c_mpu_mag_read(I2C2_MASTER_NUM,MPU9250_ADDRESS2,MAG_YOUT_H); //High data	
    //     vTaskDelay(1/portTICK_PERIOD_MS);
    //     BUF[4]=i2c_mpu_mag_read(I2C2_MASTER_NUM,MPU9250_ADDRESS2,MAG_ZOUT_L); //Low data	
    //     vTaskDelay(1/portTICK_PERIOD_MS);
	// 	BUF[5]=i2c_mpu_mag_read(I2C2_MASTER_NUM,MPU9250_ADDRESS2,MAG_ZOUT_H); //High data	
    // i2c_mpu_mag_wirte(I2C2_MASTER_NUM,MPU9250_ADDRESS2,AK8963_CNTL1_REG,0x11);// use i2c to set AK8963 working on Continuous measurement mode1 & 16-bit output & 100Hz
    // vTaskDelay(1/portTICK_PERIOD_MS);
    	calldata[6]=((BUF[1]<<8)|BUF[0])*(((175-128)>>8)+1);		//灵敏度纠正 公式见/RM-MPU-9250A-00 PDF/ 5.13
        calldata[7]=((BUF[3]<<8)|BUF[2])*(((175-128)>>8)+1);
        calldata[8]=((BUF[5]<<8)|BUF[4])*(((164-128)>>8)+1);
}


    }
}



///////////////////////////////////////////////////////////////////////////////////读取///////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//mpu9250软件数据读取

void mpu9250_soft_read(int a,int16_t *calldata,uint8_t i)
{
short data_acc[3];
short data_gyro[3];
short data_mag[3];

MPU9250_READ_ACCEL(a, data_acc);
MPU9250_READ_GYRO(a,data_gyro);
//MPU9250_READ_MAG(data_mag);
if(a==0)
{
uint8_t BUF[6]={0,0,0,0,0,0};
if(i==5)
{
    // uint8_t id = si2c_mpu_mag_read(0x03,GYRO_ADDRESS,0x00);
    // BUF[0] = si2c_mpu_mag_read(0x03,GYRO_ADDRESS,MAG_XOUT_L);
    // vTaskDelay(1/portTICK_PERIOD_MS);
    // BUF[1] = si2c_mpu_mag_read(0x03,GYRO_ADDRESS,MAG_XOUT_H);
    // vTaskDelay(1/portTICK_PERIOD_MS);
    // BUF[2] = si2c_mpu_mag_read(0x03,GYRO_ADDRESS,MAG_YOUT_L);
    // vTaskDelay(1/portTICK_PERIOD_MS);
    // BUF[3] = si2c_mpu_mag_read(0x03,GYRO_ADDRESS,MAG_YOUT_H);
    // vTaskDelay(1/portTICK_PERIOD_MS);
    // BUF[4] = si2c_mpu_mag_read(0x03,GYRO_ADDRESS,MAG_ZOUT_L);
    // vTaskDelay(1/portTICK_PERIOD_MS);
    // BUF[5] = si2c_mpu_mag_read(0x03,GYRO_ADDRESS,MAG_ZOUT_H);
    // si2c_mpu_mag_wirte(0x03,GYRO_ADDRESS,AK8963_CNTL1_REG,0x11);
    // vTaskDelay(1/portTICK_PERIOD_MS);    
    data_mag[0]=((BUF[1]<<8)|BUF[0])*(((175-128)>>8)+1);		//灵敏度纠正 公式见/RM-MPU-9250A-00 PDF/ 5.13
    data_mag[1]=((BUF[3]<<8)|BUF[2])*(((175-128)>>8)+1);
    data_mag[2]=((BUF[5]<<8)|BUF[4])*(((164-128)>>8)+1);
}
}

else
{
uint8_t BUF[6]={0,0,0,0,0,0};
if(i==5)
{
    // uint8_t id = si2c_mpu_mag_read(0x04,GYRO_ADDRESS,0x00);
    // BUF[0] = si2c_mpu_mag_read(0x04,GYRO_ADDRESS,MAG_XOUT_L);
    // vTaskDelay(1/portTICK_PERIOD_MS);
    // BUF[1] = si2c_mpu_mag_read(0x04,GYRO_ADDRESS,MAG_XOUT_H);
    // vTaskDelay(1/portTICK_PERIOD_MS);
    // BUF[2] = si2c_mpu_mag_read(0x04,GYRO_ADDRESS,MAG_YOUT_L);
    // vTaskDelay(1/portTICK_PERIOD_MS);
    // BUF[3] = si2c_mpu_mag_read(0x04,GYRO_ADDRESS,MAG_YOUT_H);
    // vTaskDelay(1/portTICK_PERIOD_MS);
    // BUF[4] = si2c_mpu_mag_read(0x04,GYRO_ADDRESS,MAG_ZOUT_L);
    // vTaskDelay(1/portTICK_PERIOD_MS);
    // BUF[5] = si2c_mpu_mag_read(0x04,GYRO_ADDRESS,MAG_ZOUT_H);
    // si2c_mpu_mag_wirte(0x04,GYRO_ADDRESS,AK8963_CNTL1_REG,0x11);
    // vTaskDelay(1/portTICK_PERIOD_MS);    
    data_mag[0]=((BUF[1]<<8)|BUF[0])*(((175-128)>>8)+1);		//灵敏度纠正 公式见/RM-MPU-9250A-00 PDF/ 5.13
    data_mag[1]=((BUF[3]<<8)|BUF[2])*(((175-128)>>8)+1);
    data_mag[2]=((BUF[5]<<8)|BUF[4])*(((164-128)>>8)+1);
}
}

 for(int i=0;i<3;i++)
 {
calldata[i]=data_acc[i];
 }
 for(int i=0;i<3;i++)
 {
calldata[i+3]=data_gyro[i];
 }
 for(int i=0;i<3;i++)
 {
calldata[i+6]=data_mag[i];
 }
}



void mpu9250_2_soft_read(int a,int16_t *calldata,uint8_t i)
{
short data_acc[3];
short data_gyro[3];
short data_mag[3];
MPU9250_2_READ_ACCEL(a, data_acc);
MPU9250_2_READ_GYRO(a,data_gyro);
uint8_t BUF[6]={0,0,0,0,0,0};
if(a ==0)
{
if(i==5)
{
    // uint8_t id = si2c_mpu_mag_read(0x04,GYRO_ADDRESS,0x00);
    // BUF[0] = si2c_mpu_mag_read(0x04,GYRO_ADDRESS,MAG_XOUT_L);
    // vTaskDelay(1/portTICK_PERIOD_MS);
    // BUF[1] = si2c_mpu_mag_read(0x04,GYRO_ADDRESS,MAG_XOUT_H);
    // vTaskDelay(1/portTICK_PERIOD_MS);
    // BUF[2] = si2c_mpu_mag_read(0x04,GYRO_ADDRESS,MAG_YOUT_L);
    // vTaskDelay(1/portTICK_PERIOD_MS);
    // BUF[3] = si2c_mpu_mag_read(0x04,GYRO_ADDRESS,MAG_YOUT_H);
    // vTaskDelay(1/portTICK_PERIOD_MS);
    // BUF[4] = si2c_mpu_mag_read(0x04,GYRO_ADDRESS,MAG_ZOUT_L);
    // vTaskDelay(1/portTICK_PERIOD_MS);
    // BUF[5] = si2c_mpu_mag_read(0x04,GYRO_ADDRESS,MAG_ZOUT_H);
    // si2c_mpu_mag_wirte(0x04,GYRO_ADDRESS,AK8963_CNTL1_REG,0x11);
    // vTaskDelay(1/portTICK_PERIOD_MS);    
    data_mag[0]=((BUF[1]<<8)|BUF[0])*(((175-128)>>8)+1);		//灵敏度纠正公式见/RM-MPU-9250A-00 PDF/ 5.13
    data_mag[1]=((BUF[3]<<8)|BUF[2])*(((175-128)>>8)+1);
    data_mag[2]=((BUF[5]<<8)|BUF[4])*(((164-128)>>8)+1);
}
}
else
{
if(i==5)
{
    // uint8_t id = si2c_mpu_mag_read(0x04,GYRO_ADDRESS_2,0x00);
    // BUF[0] = si2c_mpu_mag_read(0x04,GYRO_ADDRESS_2,MAG_XOUT_L);
    // vTaskDelay(1/portTICK_PERIOD_MS);
    // BUF[1] = si2c_mpu_mag_read(0x04,GYRO_ADDRESS_2,MAG_XOUT_H);
    // vTaskDelay(1/portTICK_PERIOD_MS);
    // BUF[2] = si2c_mpu_mag_read(0x04,GYRO_ADDRESS_2,MAG_YOUT_L);
    // vTaskDelay(1/portTICK_PERIOD_MS);
    // BUF[3] = si2c_mpu_mag_read(0x04,GYRO_ADDRESS_2,MAG_YOUT_H);
    // vTaskDelay(1/portTICK_PERIOD_MS);
    // BUF[4] = si2c_mpu_mag_read(0x04,GYRO_ADDRESS_2,MAG_ZOUT_L);
    // vTaskDelay(1/portTICK_PERIOD_MS);
    // BUF[5] = si2c_mpu_mag_read(0x04,GYRO_ADDRESS_2,MAG_ZOUT_H);
    // si2c_mpu_mag_wirte(0x04,GYRO_ADDRESS_2,AK8963_CNTL1_REG,0x11);
    // vTaskDelay(1/portTICK_PERIOD_MS);    
    data_mag[0]=((BUF[1]<<8)|BUF[0])*(((175-128)>>8)+1);		//灵敏度纠正 公式见/RM-MPU-9250A-00 PDF/ 5.13
    data_mag[1]=((BUF[3]<<8)|BUF[2])*(((175-128)>>8)+1);
    data_mag[2]=((BUF[5]<<8)|BUF[4])*(((164-128)>>8)+1);
}
}

 for(int i=0;i<3;i++)
 {
calldata[i]=data_acc[i];
 }
 for(int i=0;i<3;i++)
 {
calldata[i+3]=data_gyro[i];
 }
 for(int i=0;i<3;i++)
 {
calldata[i+6]=data_mag[i];
 }
}
