#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "wifi.h"
#include "esp_sntp.h"
#include "lwip/err.h"
#include "lwip/sys.h"
#include "mpu9250_dri.h"
#include "UDP.h"
TaskHandle_t mpu9250TaskHandle;
TaskHandle_t SNTPTaskHandle;
TaskHandle_t wifiTaskHandle;

/////////////////自旋锁变量区-任务处使用了互斥锁////////////////////////////////////////////
static portMUX_TYPE my_staticspinlock  = portMUX_INITIALIZER_UNLOCKED;
static portMUX_TYPE my_staticspinlock2 = portMUX_INITIALIZER_UNLOCKED;
static portMUX_TYPE my_staticspinlock3 = portMUX_INITIALIZER_UNLOCKED;
static portMUX_TYPE my_staticspinlock4 = portMUX_INITIALIZER_UNLOCKED;
static portMUX_TYPE my_staticspinlock5 = portMUX_INITIALIZER_UNLOCKED;
static portMUX_TYPE my_staticspinlock6 = portMUX_INITIALIZER_UNLOCKED;
static portMUX_TYPE my_staticspinlock7 = portMUX_INITIALIZER_UNLOCKED;
static portMUX_TYPE my_staticspinlock8 = portMUX_INITIALIZER_UNLOCKED;
//////////////////互斥句柄////////////////////////////////////////////////////////////////
extern  SemaphoreHandle_t xmutex1;
extern  SemaphoreHandle_t xmutex2;
extern  SemaphoreHandle_t xmutex3;
extern  SemaphoreHandle_t xmutex4;
extern  SemaphoreHandle_t xmutex5;
extern  SemaphoreHandle_t xmutex6;
extern  SemaphoreHandle_t xmutex7;
extern  SemaphoreHandle_t xmutex8;
/////////////////////////////////////////////////////////////////////////////////////////
///extern soft_i2c_master_bus_t bus3;
//extern soft_i2c_master_bus_t bus4;
/////////////IMU回传数组9-30/////////////////////////////////////////////////////////////
int16_t IMUdata1[9], IMUdata2[9], IMUdata3[9], IMUdata4[9], IMUdata5[9], IMUdata6[9], IMUdata7[9], IMUdata7[9], IMUdata8[9];
///////////////////////main.c队列句柄//////////////////////////////
extern QueueHandle_t xQueue1,xQueue2;

///////////////////////main.c消息队列//////////////////////////////
typedef struct message1
    {
      char messageID;
      int64_t time;
      int16_t imu[9];
    }message1;
///////////////////////////////////////////////////////////////////
const char* TAGtt = "tasktest";
int64_t timen=0;

//动态化自旋锁任务函数
void mpu9250_task(void *arg)
{
//临界区动态自旋锁
portMUX_TYPE *my_spinlock1 = malloc(sizeof(portMUX_TYPE));
//动态初始化自旋锁
portMUX_INITIALIZE(my_spinlock1);
taskENTER_CRITICAL(my_spinlock1);
//访问资源
taskEXIT_CRITICAL(my_spinlock1);
}



//互斥量
//SNTP时间校准+mpu9250数据读取
//需要考虑队列设置
void mpu9250_SNTP_static_task()
{
    message1 m1;
    uint8_t i=0;
    //自旋临界区1开始
   //extern struct message1 m1;
    while(1)
    {
     m1.time=obtaintime();
////////////////////////软件互斥锁//////////////////////////////
    if(xmutex1 !=NULL)
    {
    if( xSemaphoreTake(xmutex1,( TickType_t ) 10)==pdTRUE )
    {
     read_mpu9250_data_hard(0,m1.imu,i);
     xSemaphoreGive( xmutex1 );
     i++;
     if(i==6)
     {i=0;}
    }
    }
///////////////////////////////////////////////////////////////
   m1.messageID=1;
   //ESP_LOGI("1", "%d %d %d %d %d %d %d %d %d %lld/r/n",m1.imu[0], m1.imu[1], m1.imu[2],m1.imu[3], m1.imu[4], m1.imu[5],m1.imu[6], m1.imu[7], m1.imu[8], m1.time);
    xQueueSend( xQueue1,  &m1, ( TickType_t ) 0 );
    vTaskDelay(pdMS_TO_TICKS(10)); // 每10ms读取一次-100Hz
    }
}

void mpu9250_12_SNTP_static_task()
{
   // taskENTER_CRITICAL(&my_staticspinlock);
    //自旋临界区1开始
        message1 m2;
        uint8_t i=0;
    while(1)
    {
     m2.time=obtaintime();
////////////////////////软件互斥锁//////////////////////////////
    if(xmutex5 !=NULL)
    {
    if( xSemaphoreTake(xmutex5,( TickType_t ) 10)==pdTRUE )
    {
     read_mpu9250_data_hard(1,m2.imu,i);
     xSemaphoreGive( xmutex5 );
     i++;
     if(i==6)
     {i=0;}
    }
    }
///////////////////////////////////////////////////////////////
   m2.messageID=2;
 //  ESP_LOGI("2", "%d %d %d %d %d %d %d %d %d %lld/r/n",m2.imu[0], m2.imu[1], m2.imu[2],m2.imu[3], m2.imu[4], m2.imu[5],m2.imu[6], m2.imu[7], m2.imu[8], m2.time);
    xQueueSend( xQueue1,  &m2, ( TickType_t ) 0 );
     vTaskDelay(pdMS_TO_TICKS(10)); // 每5ms读取一次-100Hz
    }
   //taskEXIT_CRITICAL(&my_staticspinlock);
   //自旋临界区1结束
}

void mpu9250_21_SNTP_static_task()
{
   // taskENTER_CRITICAL(&my_staticspinlock);
    //自旋临界区1开始
    message1 m3;
    uint8_t i=0;
    while(1)
    {
    //测试函数
     m3.time=obtaintime();
////////////////////////软件互斥锁//////////////////////////////
    if(xmutex2 !=NULL)
    {
    if( xSemaphoreTake(xmutex2,( TickType_t ) 10)==pdTRUE )
    {
     read_mpu9250_2_data_hard(0,m3.imu,i);
     xSemaphoreGive( xmutex2 );
      i++;
     if(i==6)
     {i=0;}
    }
    }
///////////////////////////////////////////////////////////////
   m3.messageID=3;
 //  ESP_LOGI("3", "%d %d %d %d %d %d %d %d %d %lld/r/n",m3.imu[0], m3.imu[1], m3.imu[2],m3.imu[3], m3.imu[4], m3.imu[5],m3.imu[6], m3.imu[7], m3.imu[8], m3.time);
    xQueueSend( xQueue1,  &m3, ( TickType_t ) 0 );
     vTaskDelay(pdMS_TO_TICKS(10)); // 每10ms读取一次-100Hz
    }
    //taskEXIT_CRITICAL(&my_staticspinlock);
    //自旋临界区1结束
}

void mpu9250_22_SNTP_static_task()
{
    message1 m4;
    uint8_t i=0;
    while(1)
    {
     m4.time=obtaintime();
////////////////////////软件互斥锁//////////////////////////////
    if(xmutex2 !=NULL)
    {
    if( xSemaphoreTake(xmutex2,( TickType_t ) 10)==pdTRUE )
    {
     read_mpu9250_2_data_hard(0,m4.imu,i);
     xSemaphoreGive( xmutex2 );
     i++;
     if(i==6)
     {i=0;}
    }
    }
///////////////////////////////////////////////////////////////
   m4.messageID=4;
    //  ESP_LOGI("4", "%d %d %d %d %d %d %d %d %d %lld/r/n",m4.imu[0], m4.imu[1], m4.imu[2],m4.imu[3], m4.imu[4], m4.imu[5],m4.imu[6], m4.imu[7], m4.imu[8], m4.time);
    xQueueSend( xQueue1,  &m4, ( TickType_t ) 0 );
     vTaskDelay(pdMS_TO_TICKS(10)); // 每10ms读取一次-100Hz
    }
}

void mpu9250_31_SNTP_static_task()
{

    message1 m5;
    uint8_t i=0;
    while(1)
    {
     m5.time=obtaintime();
        if(xmutex3 !=NULL)
    {
    if( xSemaphoreTake(xmutex3,( TickType_t ) 10)==pdTRUE )
    {
    mpu9250_soft_read(0,m5.imu,i);
    xSemaphoreGive( xmutex3 );
    i++;
    if(i==6)
    {i=0;}
    }
    }
   m5.messageID=5;
   // ESP_LOGI("5", "%d %d %d %d %d %d %d %d %d %lld/r/n",m5.imu[0], m5.imu[1], m5.imu[2],m5.imu[3], m5.imu[4], m5.imu[5],m5.imu[6], m5.imu[7], m5.imu[8], m5.time);
    xQueueSend( xQueue1,  &m5, ( TickType_t ) 0 );
         vTaskDelay(pdMS_TO_TICKS(10)); // 每10ms读取一次-100Hz
    }
}

void mpu9250_32_SNTP_static_task()
{
    message1 m6;
    uint8_t i=0;
    while(1)
    {
     m6.time=obtaintime();
        if(xmutex3 !=NULL)
    {
    if( xSemaphoreTake(xmutex3,( TickType_t ) 10)==pdTRUE )
    {
    mpu9250_soft_read(1,m6.imu,i);
     xSemaphoreGive( xmutex3 );
    i++;
    if(i==6)
     {i=0;}
    }
    }
   m6.messageID=6;
//   ESP_LOGI("6", "%d %d %d %d %d %d %d %d %d %lld/r/n",m6.imu[0], m6.imu[1], m6.imu[2],m6.imu[3], m6.imu[4], m6.imu[5],m6.imu[6], m6.imu[7], m6.imu[8], m6.time);
    xQueueSend( xQueue1,  &m6, ( TickType_t ) 0 );
         vTaskDelay(pdMS_TO_TICKS(10)); // 每10ms读取一次-100Hz
    }
}

void mpu9250_41_SNTP_static_task()
{
    message1 m7;
    uint8_t i=0;    
    while(1)
    {
     m7.time=obtaintime();
        if(xmutex4 !=NULL)
    {
    if( xSemaphoreTake(xmutex4,( TickType_t ) 10)==pdTRUE )
    {
    mpu9250_2_soft_read(0,m7.imu,i);
     xSemaphoreGive( xmutex4 );
     i++;
    if(i==6)
     {i=0;}
    }
    }
   m7.messageID=7;
///   ESP_LOGI("7", "%d %d %d %d %d %d %d %d %d %lld/r/n",m7.imu[0], m7.imu[1], m7.imu[2],m7.imu[3], m7.imu[4], m7.imu[5],m7.imu[6], m7.imu[7], m7.imu[8], m7.time);
    xQueueSend( xQueue1,  &m7, ( TickType_t ) 0 );
         vTaskDelay(pdMS_TO_TICKS(10)); // 每10ms读取一次-100Hz
    }
}

void mpu9250_42_SNTP_static_task()
{
    message1 m8;
    uint8_t i=0;   
    while(1)
    {
     m8.time=obtaintime();
        if(xmutex4 !=NULL)
    {
    if( xSemaphoreTake(xmutex4,( TickType_t ) 10)==pdTRUE )
    {
    mpu9250_2_soft_read(1,m8.imu,i);
     xSemaphoreGive( xmutex4 );
     i++;
    if(i==6)
     {i=0;}
    }
    }
   m8.messageID=8;
 // ESP_LOGI("8", "%d %d %d %d %d %d %d %d %d %lld/r/n",m8.imu[0], m8.imu[1], m8.imu[2],m8.imu[3], m8.imu[4], m8.imu[5],m8.imu[6], m8.imu[7], m8.imu[8], m8.time);
    xQueueSend( xQueue1,  &m8, ( TickType_t ) 0 );
         vTaskDelay(pdMS_TO_TICKS(10)); // 每10ms读取一次-100Hz
    }
}