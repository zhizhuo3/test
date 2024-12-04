#ifndef WIFI_H
#define WIFI_H

#include "stdint.h"
void wifi_init_sta(void);
//调用以下函数启动sta模式wifi
void wifi_sta_init();
void sntp_server_();
int64_t obtaintime();

#endif


