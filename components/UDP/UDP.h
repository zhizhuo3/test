#ifndef __UDP_H__
#define __UDP_H__
#ifdef __cplusplus
extern "C" {
#endif
 //作为UDP CLIENT，255.255.255.255，表示关闭数据
 //如果需要发送到指定IP，这里直接输入对方IP
#define UDP_CLIENT_ADRESS       "172.20.10.2"    
#define UDP_CLIENT_PORT          8086               //UDP客户端端口

#define WIFI_CONNECTED_BIT  BIT0
#define WIFI_FAIL_BIT       BIT1

void AppTaskCreate(void);

#ifdef __cplusplus
}
#endif
#endif /*#ifndef __UDP_BSP_H__*/
