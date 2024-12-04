#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>

#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_spi_flash.h"
#include "esp_wifi.h"

#include <sys/socket.h>
#include "freertos/event_groups.h"
#include "esp_event.h"
#include "esp_err.h"
#include "nvs_flash.h"
#include "esp_smartconfig.h"
#include "esp_netif.h"
#include "esp_wpa2.h"

#include "esp_log.h"
#include "esp_ota_ops.h"
#include "nvs.h"
#include "udp.h"


//任务句柄 
static TaskHandle_t xHandleTaskUdp = NULL;

//任务堆栈大小，主要是函数嵌套参数入栈和局部变量的内存
#define Udp_TASK_STACK_SIZE     8192

//任务优先级，越大越高，跟ucos相反
#define Udp_TASK_PRIO           2

static int udp_connect_socket = 0;                     //客户端连接socket
static struct sockaddr_in udp_client_addr;                  //client地址

extern QueueHandle_t xQueue1,xQueue2;
extern EventGroupHandle_t s_wifi_event_group;
//函数声明
static void vTaskUdp(void *pvParameters);
static esp_err_t CreateUdpClient(const char *ip,uint16_t port);

typedef struct message1
    {
      char messageID;
      int64_t time;
      int16_t imu[9];
    }message1;

/***********************************************************************
* 函数:  
* 描述:   创建UDP接收发送任务，给外部使用
* 参数:
* 返回: 无
* 备注：
************************************************************************/
void AppTaskCreate(void)
{
    // xTaskCreate(vTaskUdp,                               // 任务函数 
    //         "vTaskUdp",                                 // 任务名
    //         Udp_TASK_STACK_SIZE,                        // 任务栈大小，单位 word，也就是 4 字节
    //         NULL,                                       // 任务参数 
    //         Udp_TASK_PRIO,                              // 任务优先级
    //         &xHandleTaskUdp);                           // 任务句柄 

    xTaskCreatePinnedToCore(vTaskUdp,"vTaskUdp",1024*12,NULL,5,NULL,0);
}
/*****************************************************
 * 函数：struct2spring
 * 描述：结构体转字符串，包括uint16_t,uint64_t,最后需要字符串拼接
 * 参数：time，imu[9]
 * 返回：字符串
 * 备注: 初始版本使用的是itoa函数，该函数会在字符串后加入\0字符串终止标志，第二版本使用sprintf函数
 ****************************************************/
void struct2spring(char ID, uint64_t time, int16_t * imudata,char * databuff)
{

sprintf(databuff,"%d %lld %hd %hd %hd %hd %hd %hd %hd %hd %hd\r\n",ID,time,imudata[0],imudata[1],imudata[2],imudata[3],imudata[4],imudata[5],imudata[6],imudata[7],imudata[8]);

}
/***********************************************************************
* 函数:  
* 描述:   创建UDP客户端连接
* 参数:ip：IP地址，port：端口号
* 返回: 无
* 备注：
************************************************************************/
static esp_err_t CreateUdpClient(const char *ip,uint16_t port)
{
    //等待连接成功，或已经连接有断开连接，此函数会一直阻塞，只有有连接
   EventBits_t bits =  xEventGroupWaitBits(s_wifi_event_group,// 事件标志组句柄 
                        WIFI_CONNECTED_BIT, // 等待bit0和bit1被设置 
                        pdFALSE,             //TRUE退出时bit0和bit1被清除，pdFALSE退出时bit0和bit1不清除
                        pdFALSE,             //设置为pdTRUE表示等待bit1和bit0都被设置,pdFALSE表示等待bit1或bit0其中一个被设置
                        portMAX_DELAY);      //等待延迟时间，一直等待 
    if (bits & WIFI_CONNECTED_BIT) 
    {
        //新建socket
        udp_connect_socket = socket(AF_INET, SOCK_DGRAM, 0);                         /*参数和TCP不同*/
        if (udp_connect_socket < 0)
        {
            //新建失败后，关闭新建的socket，等待下次新建
            close(udp_connect_socket);
            return ESP_FAIL;
        }
        //配置连接服务器信息
        udp_client_addr.sin_family = AF_INET;
        udp_client_addr.sin_port = htons(port);
        udp_client_addr.sin_addr.s_addr = inet_addr(ip);
        int len = 0;            //长度
        char databuff[30] = "Hello Server,Please ack!!";    //不管有没有连接先发送一波数据再说
        //测试udp server,返回发送成功的长度
        len = sendto(udp_connect_socket, databuff, 30, 0, (struct sockaddr *) &udp_client_addr,sizeof(udp_client_addr));
        if (len > 0)
        {
        } 
        else 
        {
            close(udp_connect_socket);
            return ESP_FAIL;
        }
        return ESP_OK;
    }
    else
    {
        return ESP_FAIL;
    }
}
/***********************************************************************
* 函数:  
* 描述:   客户端任务
* 参数:
* 返回: 无
* 备注：
************************************************************************/
 void vTaskUdp(void *pvParameters)
{
    int len = 0;            //长度
    char databuff[512];    //缓存
    //创建udp客户端
    CreateUdpClient(UDP_CLIENT_ADRESS,UDP_CLIENT_PORT);
    unsigned int udpsocklen = sizeof(udp_client_addr);      //地址长度
    message1 m;
    for (;;)
    {
 if( xQueue1 != 0 )
 {
// Receive a message on the created queue.  Block for 10 ticks if a
// message is not immediately available.
if( xQueueReceive( xQueue1, &m, ( TickType_t ) 10 ) )
     {
// pcRxedMessage now points to the struct AMessage variable posted
// by vATask.
//结构体转字符串
    memset(databuff,0,sizeof(databuff));
    struct2spring(m.messageID,m.time, m.imu, databuff);
    len=sendto(udp_connect_socket, databuff, 75 , 0,(struct sockaddr *) &udp_client_addr, sizeof(udp_client_addr));
        //vTaskDelay(10 / portTICK_PERIOD_MS);
     }
 }
    }
}
