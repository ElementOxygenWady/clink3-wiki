# <a name="目录">目录</a>
+ [Linux实现设备一键配网的参考实现](#Linux实现设备一键配网的参考实现)
        - [HAL_Awss_Open_Monitor的参考实现](#HAL_Awss_Open_Monitor的参考实现)
        - [HAL_Awss_Close_Monitor的参考实现](#HAL_Awss_Close_Monitor的参考实现)
        - [HAL_Awss_Switch_Channel的参考实现](#HAL_Awss_Switch_Channel的参考实现)
        - [HAL_Sys_Net_Is_Ready的参考实现](#HAL_Sys_Net_Is_Ready的参考实现)
        - [HAL_Wifi_Get_Ap_Info的参考实现](#HAL_Wifi_Get_Ap_Info的参考实现)

# <a name="Linux实现设备一键配网的参考实现">Linux实现设备一键配网的参考实现</a>


如何配置SDK进入一键配网模式，以及如何调用配网的API,请见[一键配网](https://help.aliyun.com/document_detail/111047.html)

本文就如何在捷高摄像头(Linux)的环境里面实现一键配网需要的HAL提供一个参考实现


### <a name="HAL_Awss_Open_Monitor的参考实现">HAL_Awss_Open_Monitor的参考实现</a>
```
#include <string.h>
#include <stdlib.h>
#include "infra_defs.h"
#include "dev_bind_wrapper.h"
#include "iot_import_awss.h"
#include "assert.h"
#include "stdio.h"

static int s_enable_sniffer = 1;

static void * func_Sniffer(void *cb)
{
    int sock = socket(PF_PACKET, SOCK_RAW, htons(0x03)); /* ETH_P_ALL */
    char rev_buffer[MAX_REV_BUFFER];

    int skipLen = 26;/* radiotap 默认长度为26 */
    printf("Sniffer Thread Create\r\n");
    if(sock < 0)
    {
        printf("Sniffer Socket Alloc Fails %d \r\n", sock);
        perror("Sniffer Socket Alloc");
        return (void *)0;
    }

    {/* 强制绑定到wlan0 上。后续可以考虑去掉 */
        struct ifreq ifr;
        memset(&ifr, 0x00, sizeof(ifr));
        strncpy(ifr.ifr_name, WLAN_DEV , strlen(WLAN_DEV) + 1);
        setsockopt(sock, SOL_SOCKET, SO_BINDTODEVICE, (char *)&ifr, sizeof(ifr));
    }

    while((1 == s_enable_sniffer))
    {
        int rev_num = recvfrom(sock, rev_buffer, MAX_REV_BUFFER, 0, NULL, NULL);
        struct ieee80211_radiotap_header *pHeader = (struct ieee80211_radiotap_header *)rev_buffer;
        skipLen = pHeader->it_len;

#ifdef WIFI_CHIP_7601
        skipLen = 144;
#endif
        if(skipLen >= MAX_REV_BUFFER)
        {/* 有出现过header全ff的情况，这里直接丢掉这个包 */
            /* printf("Sniffer skip len > MAX_REV_BUFFER\n"); */
            continue;
        }

        if(0)
        {
            int index = 0;
            /* printf("skipLen:%d ", skipLen); */
            for(index = 0; index < 180; index++)
            {
                printf("%02X-", rev_buffer[index]);
            }
            printf("\r\n");
        }
        if(rev_num > skipLen)
        {
            /* TODO fix the link type, with fcs, rssi */
            ((awss_recv_80211_frame_cb_t)cb)(rev_buffer + skipLen, rev_num - skipLen, AWSS_LINK_TYPE_80211_RADIO, 1, 0);
        }
    }

    close(sock);

    printf("Sniffer Proc Finish\r\n");
    return (void *)0;
}

void start_sniff(_IN_ awss_recv_80211_frame_cb_t cb){
    static void *g_sniff_thread = NULL;
    int stack_used;
    hal_os_thread_param_t task_parms = {0};
    HAL_ThreadCreate(&g_sniff_thread, func_Sniffer, (void *)cb, &task_parms, &stack_used);
}


void HAL_Awss_Open_Monitor(_IN_ awss_recv_80211_frame_cb_t cb)
{
extern void start_sniff(_IN_ awss_recv_80211_frame_cb_t cb);
int ret = system("ifconfig wlan0 down; ifconfig wlan0 up");
printf("wlan0 up ret is %d", ret);
assert(0 == ret);
ret = system("iwconfig wlan0 mode monitor");
printf("wlan0 mode monitor ret is %d", ret);
assert(0 == ret);
ret = system("echo \"10 1 1\" > /proc/net/rtl8188fu/wlan0/monitor");
printf("wlan0 echo data to monitor ret is  %d", ret);
assert(0 == ret);
start_sniff(cb);
}
```


### <a name="HAL_Awss_Close_Monitor的参考实现">HAL_Awss_Close_Monitor的参考实现</a>
```
void stop_sniff() {
  s_enable_sniffer = 0;
}

void HAL_Awss_Close_Monitor(void)
{
int ret = -1;
extern int stop_sniff();
stop_sniff();
ret = system("iwconfig wlan0 mode managed");
printf("close monitor ret is %d", ret);
}
```

### <a name="HAL_Awss_Switch_Channel的参考实现">HAL_Awss_Switch_Channel的参考实现</a>
```
void HAL_Awss_Switch_Channel(
            _IN_ char primary_channel,
            _IN_OPT_ char secondary_channel,
            _IN_OPT_ uint8_t bssid[ETH_ALEN])
{
char cmd[255] = {0};
int ret = -1;
snprintf(cmd, 255, "iwconfig wlan0 channel %d", primary_channel);
printf("switch:%s\n", cmd);
ret = system(cmd);
assert(0 == ret);
}
```

### <a name="HAL_Sys_Net_Is_Ready的参考实现">HAL_Sys_Net_Is_Ready的参考实现</a>
```
int HAL_Sys_Net_Is_Ready()
{
   int ret = system("ifconfig wlan0 | grep inet ");
   printf("cxc net_is_ready ret is %d\n", ret);
    return (0 == ret);
}
```

### <a name="HAL_Wifi_Get_Ap_Info的参考实现">HAL_Wifi_Get_Ap_Info的参考实现</a>
```
int HAL_Wifi_Get_Ap_Info(
            _OU_ char ssid[HAL_MAX_SSID_LEN],
            _OU_ char passwd[HAL_MAX_PASSWD_LEN],
            _OU_ uint8_t bssid[ETH_ALEN])
{

    FILE *fp;
    char *p1, *p2;
#define MAXLINE (256)
    char buff[MAXLINE]={0};
    system("str=$(iwconfig wlan0 | grep ESSID); echo $str > /tmp/ssid");
    fp=fopen("/tmp/ssid","r");
    if(NULL == fp) {
        return -1;
    }
    while(fgets(buff, MAXLINE, fp )!= NULL)
    pclose(fp);
    if(NULL == buff || strlen(buff) < 2) {
        return -1;
    }
    p1 = strstr(buff, "\"");
    p2 = strstr(p1+1, "\"");
    memcpy(ssid, p1+1, p2-p1-1);
    return 0;
}
```