#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "driver/i2c.h"
#include "I2C_driver_master_top.h"
#include <portmacro.h>
#include "soft_i2c.h"
#include "mpu9250_dri.h"
#include "apptask.h"
#include "wifi.h"
#include "esp_sntp.h"
#include "esp_system.h"
#include "freertos/semphr.h"
#include "UDP.h"
//////////全局变量//////////////
#define IMU1 (char)1;
#define IMU2 (char)2;
#define IMU3 (char)3;
#define IMU4 (char)4;
#define IMU5 (char)5;
#define IMU6 (char)6;
#define IMU7 (char)7;
#define IMU8 (char)8;
#define IIC3_SOFT                         0x03 //Y-axis sensitivity adjustment value
#define IIC4_SOFT                         0x04 //Z-axis sensitivity adjustment value
//
 int64_t n=0;
//    soft_i2c_master_bus_t bus1 = NULL;
//    soft_i2c_master_bus_t bus2 = NULL;


static portMUX_TYPE myspinlock = portMUX_INITIALIZER_UNLOCKED;
    ////互斥句柄
    SemaphoreHandle_t xmutex1=NULL;
    SemaphoreHandle_t xmutex2=NULL;
    SemaphoreHandle_t xmutex3=NULL;
    SemaphoreHandle_t xmutex4=NULL;
    SemaphoreHandle_t xmutex5=NULL;
    SemaphoreHandle_t xmutex6=NULL;
    SemaphoreHandle_t xmutex7=NULL;
    SemaphoreHandle_t xmutex8=NULL;
///////////////////////////////

 const char *TAG = "MPU9250";

// 初始化 I2C;

// 读取 MPU9250 数据;
    //队列
    QueueHandle_t xQueue1,xQueue2,xQueue3,xQueue4;
    //队列消息结构体
typedef struct message1
    {
      char messageID;
      int64_t time;
      int16_t imu[9];
    }message1;

#define QUEUE_ITEM_SIZE sizeof(message1)

void app_main(void) {

    ///////////I2C设备主机+从机初始化////////
        i2c_master_init();
        i2c2_master_init();
        i2c3_master_init(); 
        i2c4_master_init();
        mpu9250_init_hardi2c1(); 
        mpu9250_init_hardi2c2();
        mpu9250_init_softi2c3();
        mpu9250_init_softi2c4();
    //I2C资源互斥保护
    xmutex1 = xSemaphoreCreateBinary();
    xmutex2 = xSemaphoreCreateBinary();
    xmutex3 = xSemaphoreCreateBinary();
    xmutex4 = xSemaphoreCreateBinary();
    xmutex5 = xSemaphoreCreateBinary();
    xmutex6 = xSemaphoreCreateBinary();
    xmutex7 = xSemaphoreCreateBinary();
    xmutex8 = xSemaphoreCreateBinary();
    //
    //队列创建
     xQueue1 = xQueueCreate( 20, QUEUE_ITEM_SIZE );
    if(xmutex1!=NULL)
    {
        xSemaphoreGive(xmutex1);
    }
    if(xmutex2!=NULL)
    {
        xSemaphoreGive(xmutex2);
    }
    if(xmutex3!=NULL)
    {
        xSemaphoreGive(xmutex3);
    }
    if(xmutex4!=NULL)
    {
        xSemaphoreGive(xmutex4);
    }
    if(xmutex5!=NULL)
    {
        xSemaphoreGive(xmutex5);
    }
    if(xmutex6!=NULL)
    {
        xSemaphoreGive(xmutex6);
    }
    if(xmutex7!=NULL)
    {
        xSemaphoreGive(xmutex7);
    }
    if(xmutex8!=NULL)
    {
        xSemaphoreGive(xmutex8);
    }
    //
      //  mpu9250_init_softi2c3();
      //  mpu9250_init_softi2c4();
        wifi_sta_init();
        sntp_server_();
        sntp_init();
    //////////////////////////////////
    //I2C互斥量-一个I2C一个互斥--共4个互斥
    //队列设置-2条队列-4任务输出/1条
    ///////////SMP任务创建/////////////
    //任务调度器挂起
   // AppTaskCreate();
   vTaskSuspendAll();
  //校时采集任务
  AppTaskCreate();
 xTaskCreatePinnedToCore(mpu9250_SNTP_static_task,"mputasktest",4096,NULL,4,NULL,0);//0x68
 xTaskCreatePinnedToCore(mpu9250_12_SNTP_static_task,"mputasktest",4096,NULL,4,NULL,0);//0x69
xTaskCreatePinnedToCore(mpu9250_21_SNTP_static_task,"mputasktest",4096,NULL,4,NULL,1);
 xTaskCreatePinnedToCore(mpu9250_22_SNTP_static_task,"mputasktest",4096,NULL,4,NULL,1);
xTaskCreatePinnedToCore(mpu9250_31_SNTP_static_task,"mputasktest",4096,NULL,4,NULL,1);
 xTaskCreatePinnedToCore(mpu9250_32_SNTP_static_task,"mputasktest",4096,NULL,4,NULL,1);   
   xTaskCreatePinnedToCore(mpu9250_41_SNTP_static_task,"mputasktest",4096,NULL,2,NULL,1);   
  xTaskCreatePinnedToCore(mpu9250_42_SNTP_static_task,"mputasktest",4096,NULL,2,NULL,1);   
    //任务调度器就绪
    //////////////////////////////////
    xTaskResumeAll ();

}   
