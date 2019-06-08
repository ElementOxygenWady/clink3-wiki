/**
 * NOTE:
 *
 * HAL_TCP_xxx API reference implementation: wrappers/os/ubuntu/HAL_TCP_linux.c
 *
 */
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/prctl.h>
#include <sys/time.h>
#include <semaphore.h>
#include <errno.h>
#include <assert.h>
#include <net/if.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/ioctl.h>
#include <sys/reboot.h>
#include <sys/time.h>
#include <time.h>
#include <signal.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <unistd.h>
#include <fcntl.h>
#include <netinet/tcp.h>
#include <netdb.h>
#include <sys/stat.h>
#include <ieee80211_radiotap.h>
#include <linux/if_packet.h>
#include <net/ethernet.h> /* the L2 protocols */
#include "infra_types.h"
#include "infra_defs.h"
#include "infra_compat.h"
#include "wrappers_defs.h"
#include "iot_import_awss.h"
#include "stdarg.h"

char _product_key[IOTX_PRODUCT_KEY_LEN + 1]       = "a1I98ylaERB";
char _product_secret[IOTX_PRODUCT_SECRET_LEN + 1] = "cWIhFUrzd5UQ1cUk";
char _device_name[IOTX_DEVICE_NAME_LEN + 1]       = "tt_ap_test_01";
char _device_secret[IOTX_DEVICE_SECRET_LEN + 1]   = "fOOzj260nZ0q8v3tJ0n91B7CXjsSt22S";

/* please use ifconfig to get your wireless card's name, and replace g_ifname */
#define MAX_REV_BUFFER 8000

static int s_enable_sniffer = 1;
char *g_ifname = "wlx00259cf84f36";

char g_opened_ap[36] = {0};
#define ROUTER_INFO_PATH        "/proc/net/route"
#define ROUTER_RECORD_SIZE      256

static uint64_t _linux_get_time_ms(void)
{
    struct timeval tv = { 0 };
    uint64_t time_ms;

    gettimeofday(&tv, NULL);

    time_ms = tv.tv_sec * 1000 + tv.tv_usec / 1000;

    return time_ms;
}

static uint64_t _linux_time_left(uint64_t t_end, uint64_t t_now)
{
    uint64_t t_left;

    if (t_end > t_now) {
        t_left = t_end - t_now;
    } else {
        t_left = 0;
    }

    return t_left;
}

int HAL_Awss_Close_Ap()
{
    char buffer[256] = {0};
    int ret = 0;
    /* use nmcli commands to close the previous open Ap */
    snprintf(buffer, 256, "nmcli connection down %s", g_opened_ap);
    ret = system(buffer);
    memset(g_opened_ap, 0, sizeof(g_opened_ap));
    return 0;
}

void stop_sniff()
{
    s_enable_sniffer = 0;
}

void HAL_Awss_Close_Monitor(void)
{
		int ret = -1;
    char buffer[256] = {0};
    char *ifname = g_ifname;
    stop_sniff();
    memset(buffer, 0, 256);
    snprintf(buffer, 256, "ifconfig %s down", ifname);
    ret = system(buffer);

    memset(buffer, 0, 256);
    snprintf(buffer, 256, "iwconfig %s mode managed", ifname);
    ret =  system(buffer);

    memset(buffer, 0, 256);
    snprintf(buffer, 256, "ifconfig %s up", ifname);
    ret = system(buffer);
}


int HAL_Awss_Connect_Ap(
            _IN_ uint32_t connection_timeout_ms,
            _IN_ char ssid[HAL_MAX_SSID_LEN],
            _IN_ char passwd[HAL_MAX_PASSWD_LEN],
            _IN_OPT_ enum AWSS_AUTH_TYPE auth,
            _IN_OPT_ enum AWSS_ENC_TYPE encry,
            _IN_OPT_ uint8_t bssid[ETH_ALEN],
            _IN_OPT_ uint8_t channel)
{
		char buffer[128] = {0};
    char *wifi_name = "linkkit";
    int ret = -1;

    printf("ssid  : %s\n", ssid);
    printf("passwd: %s\n", passwd);

    /**
     * using ubuntu network manager for connecting ap
     * reference:
     * https://developer.gnome.org/NetworkManager/stable/nmcli.html
     * https://www.96boards.org/documentation/consumer/guides/wifi_commandline.md.html     */
    memset(buffer, 0, 128);
    snprintf(buffer, 128, "nmcli connection down %s", wifi_name);
    ret = system(buffer);

    memset(buffer, 0, 128);
    snprintf(buffer, 128, "nmcli connection delete %s", wifi_name);
    ret = system(buffer);

    memset(buffer, 0, 128);
    snprintf(buffer, 128, "nmcli connection add con-name %s ifname %s type wifi ssid %s", wifi_name, g_ifname,
             ssid);
    ret = system(buffer);

    /**
     * security reference:
     * https://developer.gnome.org/NetworkManager/stable/settings-802-11-wireless-security.html
     */
    if (strlen(passwd) == 0) {
        memset(buffer, 0, 128);
        snprintf(buffer, 128, "nmcli connection modify %s wifi-sec.key-mgmt %s", "none", wifi_name);
        ret = system(buffer);
    } else {
        memset(buffer, 0, 128);
        snprintf(buffer, 128, "nmcli connection modify %s wifi-sec.key-mgmt %s", wifi_name, "wpa-psk");
        ret = system(buffer);
        memset(buffer, 0, 128);
        snprintf(buffer, 128, "nmcli connection modify %s wifi-sec.psk %s", wifi_name, passwd);
        ret = system(buffer);
    }

    memset(buffer, 0, 128);
    snprintf(buffer, 128, "nmcli connection up %s", wifi_name);
    ret = system(buffer);

    return 0;
}


int HAL_Awss_Get_Channelscan_Interval_Ms(void)
{
	return 250;
}



int HAL_Awss_Get_Timeout_Interval_Ms(void)
{
	return 3 * 60 * 1000;
}


int HAL_Awss_Open_Ap(const char *ssid, const char *passwd, int beacon_interval, int hide)
{
	char buffer[256] = {0};
    char *sta_ssid = "linkkit";
    const char *ap_ssid = ssid; /*  "linkkit-ap"; */
    char *ap_passwd = "";
    int ret = -1;
    /**
     * using ubuntu16.04 network manager to create wireless access point
     * reference:
     * https://developer.gnome.org/NetworkManager/stable/nmcli.html
     * https://unix.stackexchange.com/questions/234552/create-wireless-access-point-and-share-internet-connection-with-nmcli
     */
    memset(buffer, 0, 256);
    snprintf(buffer, 256, "ifconfig %s down", g_ifname);
    ret = system(buffer);

    memset(buffer, 0, 256);
    snprintf(buffer, 256, "nmcli connection down %s", ap_ssid);
    ret = system(buffer);

    memset(buffer, 0, 256);
    snprintf(buffer, 256, "nmcli connection delete %s", sta_ssid);
    ret = system(buffer);

    memset(buffer, 0, 256);
    snprintf(buffer, 256, "nmcli connection delete %s", ap_ssid);
    ret = system(buffer);

    memset(buffer, 0, 256);
    snprintf(buffer, 256, "nmcli connection add con-name %s type wifi ifname %s autoconnect yes ssid %s mode ap", ap_ssid,
             g_ifname, ap_ssid);
    ret = system(buffer);

    if (strlen(ap_passwd) == 0) {
        memset(buffer, 0, 256);
        snprintf(buffer, 256, "nmcli connection modify %s 802-11-wireless.mode ap ipv4.method shared", ap_ssid);
        ret = system(buffer);
    } else {
        memset(buffer, 0, 256);
        snprintf(buffer, 256,
                 "nmcli connection modify %s 802-11-wireless.mode ap 802-11-wireless-security.key-mgmt wpa-psk ipv4.method shared 802-11-wireless-security.psk %s",
                 ap_ssid, ap_passwd);
        ret = system(buffer);
    }

    memset(buffer, 0, 256);
    snprintf(buffer, 256, "nmcli connection up %s", ap_ssid);
    ret = system(buffer);
    if (strlen(ap_ssid) < sizeof(g_opened_ap)) {
        memcpy(g_opened_ap, ap_ssid, strlen(ap_ssid));
    }
    return 0;
}

/**
 * @brief   设置Wi-Fi网卡工作在监听(Monitor)模式, 并在收到802.11帧的时候调用被传入的回调函数
 *
 * @param[in] cb @n A function pointer, called back when wifi receive a frame.
 */

static void *func_Sniffer(void *cb)
{
    int32_t raw_socket = 0;
    struct ifreq ifr;
    struct sockaddr_ll sll;
    char rev_buffer[MAX_REV_BUFFER];
    int skipLen = 26;/* radiotap 默认长度为26 */
    char *ifname = g_ifname;
    memset(&ifr, 0, sizeof(struct ifreq));
    memset(&sll, 0, sizeof(struct sockaddr_ll));
    /* ifr.ifr_name can take 16 chars at most*/
    if (strlen(ifname) > 15) {
        return NULL;
    }
    raw_socket = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ALL));
    if (raw_socket < 0) {
        printf("raw socket error: ");
        return NULL ;
    }

    memcpy(ifr.ifr_name, ifname, strlen(ifname));
    if (ioctl(raw_socket, SIOCGIFINDEX, &ifr) < 0) {
        close(raw_socket);
        printf("SIOCGIFINDED error: ");
        return NULL ;
    }

    sll.sll_family = AF_PACKET;
    sll.sll_ifindex = ifr.ifr_ifindex;
    sll.sll_protocol = htons(ETH_P_ALL);
    if (bind(raw_socket, (struct sockaddr *)&sll, sizeof(sll)) < 0) {
        close(raw_socket);
        printf("bind error: ");
        return NULL ;
    }

    if (ioctl(raw_socket, SIOCGIFFLAGS, &ifr) < 0) {
        close(raw_socket);
        printf("SIOCGIFFLAGS error: ");
        return NULL ;
    }
    ifr.ifr_flags |= IFF_PROMISC;
    if (ioctl(raw_socket, SIOCSIFFLAGS, &ifr) < 0) {
        close(raw_socket);
        printf("SIOCSIFFLAGS error: ");
        return NULL ;
    }

    printf("Sniffer Thread Create\r\n");

    while ((1 == s_enable_sniffer)) {
        int rev_num = recvfrom(raw_socket, rev_buffer, MAX_REV_BUFFER, 0, NULL, NULL);
        struct ieee80211_radiotap_header *pHeader = (struct ieee80211_radiotap_header *)rev_buffer;
        skipLen = pHeader->it_len;

#ifdef WIFI_CHIP_7601
        skipLen = 144;
#endif
        if (skipLen >= MAX_REV_BUFFER) {
            continue;
        }
        if (0) {
            int index = 0;
            /* printf("skipLen:%d ", skipLen); */
            for (index = 0; index < 180; index++) {
                printf("%02X-", rev_buffer[index]);
            }
            printf("\r\n");
        }
        if (rev_num > skipLen) {
            /* TODO fix the link type, with fcs, rssi */
            ((awss_recv_80211_frame_cb_t)cb)(rev_buffer + skipLen, rev_num - skipLen, AWSS_LINK_TYPE_NONE, 0, 0);
        }
    }

    close(raw_socket);

    printf("Sniffer Proc Finish\r\n");
    return (void *)0;
}

/**
 * @brief  create a thread
 *
 * @param[out] thread_handle @n The new thread handle, memory allocated before thread created and return it, free it after thread joined or exit.
 * @param[in] start_routine @n A pointer to the application-defined function to be executed by the thread.
        This pointer represents the starting address of the thread.
 * @param[in] arg @n A pointer to a variable to be passed to the start_routine.
 * @param[in] hal_os_thread_param @n A pointer to stack params.
 * @param[out] stack_used @n if platform used stack buffer, set stack_used to 1, otherwise set it to 0.
 * @return
   @verbatim
     = 0: on success.
     = -1: error occur.
   @endverbatim
 * @see None.
 * @note None.
 */
int HAL_ThreadCreate(
            void **thread_handle,
            void *(*work_routine)(void *),
            void *arg,
            hal_os_thread_param_t *hal_os_thread_param,
            int *stack_used)
{
    int ret = -1;

    if (stack_used) {
        *stack_used = 0;
    }

    ret = pthread_create((pthread_t *)thread_handle, NULL, work_routine, arg);
    if (ret != 0) {
        printf("pthread_create error: %d\n", (int)ret);
        return -1;
    }
    pthread_detach((pthread_t) *thread_handle);
    return 0;
}

void start_sniff(_IN_ awss_recv_80211_frame_cb_t cb)
{
    static void *g_sniff_thread = NULL;
    int stack_used;
    hal_os_thread_param_t task_parms = {0};
    HAL_ThreadCreate(&g_sniff_thread, func_Sniffer, (void *)cb, &task_parms, &stack_used);
}

void HAL_Awss_Open_Monitor(_IN_ awss_recv_80211_frame_cb_t cb)
{
	extern void start_sniff(_IN_ awss_recv_80211_frame_cb_t cb);
    char *ifname = g_ifname;

    char buffer[256] = {0};
    int ret = 0;
    memset(buffer, 0, 256);
    snprintf(buffer, 256, "ifconfig %s down", ifname);
    ret = system(buffer);
    printf("ret1 is %d\r\n", ret);

    memset(buffer, 0, 256);
    snprintf(buffer, 256, "iwconfig %s mode monitor", ifname);
    ret = system(buffer);
    printf("ret2 is %d\r\n", ret);

    memset(buffer, 0, 256);
    snprintf(buffer, 256, "ifconfig %s up", ifname);
    ret = system(buffer);

    printf("ret3 is %d\r\n", ret);

    start_sniff(cb);
}


void HAL_Awss_Switch_Channel(char primary_channel, char secondary_channel, uint8_t bssid[ETH_ALEN])
{
    char cmd[255] = {0};
    int ret = -1;
    snprintf(cmd, 255, "iwconfig %s channel %d", g_ifname, primary_channel);
    printf("switch:%s\n", cmd);
    ret = system(cmd);
    assert(0 == ret);
}


/**
 * @brief Deallocate memory block
 *
 * @param[in] ptr @n Pointer to a memory block previously allocated with platform_malloc.
 * @return None.
 * @see None.
 * @note None.
 */
void HAL_Free(void *ptr)
{
	free(ptr);
}


/**
 * @brief Get device name from user's system persistent storage
 *
 * @param [ou] device_name: array to store device name, max length is IOTX_DEVICE_NAME_LEN
 * @return the actual length of device name
 */
int HAL_GetDeviceName(char device_name[IOTX_DEVICE_NAME_LEN + 1])
{
    int len = strlen(_device_name);
    memset(device_name, 0x0, IOTX_DEVICE_NAME_LEN + 1);

    strncpy(device_name, _device_name, len);

    return strlen(device_name);
}


/**
 * @brief Get device secret from user's system persistent storage
 *
 * @param [ou] device_secret: array to store device secret, max length is IOTX_DEVICE_SECRET_LEN
 * @return the actual length of device secret
 */
int HAL_GetDeviceSecret(char device_secret[IOTX_DEVICE_SECRET_LEN + 1])
{
    int len = strlen(_device_secret);
    memset(device_secret, 0x0, IOTX_DEVICE_SECRET_LEN + 1);

    strncpy(device_secret, _device_secret, len);

    return len;
}


/**
 * @brief Get firmware version
 *
 * @param [ou] version: array to store firmware version, max length is IOTX_FIRMWARE_VER_LEN
 * @return the actual length of firmware version
 */
int HAL_GetFirmwareVersion(char *version)
{
    char *ver = "app-1.0.0-20180101.1000";
    int len = strlen(ver);
    memset(version, 0x0, IOTX_FIRMWARE_VER_LEN);
    strncpy(version, ver, IOTX_FIRMWARE_VER_LEN);
    version[len] = '\0';
    return strlen(version);
}


/**
 * @brief Get product key from user's system persistent storage
 *
 * @param [ou] product_key: array to store product key, max length is IOTX_PRODUCT_KEY_LEN
 * @return  the actual length of product key
 */
int HAL_GetProductKey(char product_key[IOTX_PRODUCT_KEY_LEN + 1])
{
    int len = strlen(_product_key);
    memset(product_key, 0x0, IOTX_PRODUCT_KEY_LEN + 1);

    strncpy(product_key, _product_key, len);

    return len;
}


int HAL_GetProductSecret(char product_secret[IOTX_PRODUCT_SECRET_LEN + 1])
{
    int len = strlen(_product_secret);
    memset(product_secret, 0x0, IOTX_PRODUCT_SECRET_LEN + 1);

    strncpy(product_secret, _product_secret, len);

    return len;
}

#define ITEM_MAX_KEY_LEN     128 /* The max key length for key-value item */
#define ITEM_MAX_VAL_LEN     512 /* The max value length for key-value item */
#define ITEM_MAX_LEN         sizeof(kv_item_t)
#define kv_err(...)               do{printf(__VA_ARGS__);printf("\r\n");}while(0)
#define MAX_REV_BUFFER 8000
#define WLAN_DEV "wlan0"
#define KV_FILE_NAME         "linkkit_kv.bin"
#define TABLE_COL_SIZE    (384)
#define TABLE_ROW_SIZE    (2)

typedef struct kv {
    char key[ITEM_MAX_KEY_LEN];
    uint8_t value[ITEM_MAX_VAL_LEN];
    int value_len;
} kv_item_t;

typedef struct kv_file_s {
    const char *filename;
    pthread_mutex_t lock;
} kv_file_t;

static int kv_get(const char *key, void *value, int *value_len);
static int kv_set(const char *key, void *value, int value_len);
static int kv_del(const char *key);
static unsigned int hash_gen(const char *key);
static int hash_table_put(kv_file_t *file, const  char *key, void *value, int value_len);
static int hash_table_get(kv_file_t *file, const char *key, void *value, int *len);
static int hash_table_rm(kv_file_t *file,  const  char *key);
static kv_file_t *kv_open(const char *filename);
static int read_kv_item(const char *filename, void *buf, int location);
static int write_kv_item(const char *filename, void *data, int location);

static void free_kv(struct kv *kv)
{
    if (kv) {
        kv->value_len = 0;
        free(kv);
    }
}

static unsigned int hash_gen(const char *key)
{
    unsigned int hash = 0;
    while (*key) {
        hash = (hash << 5) + hash + *key++;
    }
    return hash % TABLE_COL_SIZE;
}

/* insert or update a value indexed by key */
static int hash_table_put(kv_file_t *file, const  char *key, void *value, int value_len)
{
    int i;
    int read_size;
    kv_item_t *kv;
    int j = 0;
    kv_item_t *p;
    if (!file || !file->filename ||  !key || !value  || value_len <= 0) {
        kv_err("paras err");
        return -1;
    }

    value_len = value_len > ITEM_MAX_VAL_LEN ? ITEM_MAX_VAL_LEN : value_len;
    i = hash_gen(key);
    kv_err("hash i= %d", i);
    read_size = ITEM_MAX_LEN * TABLE_ROW_SIZE;
    kv = malloc(read_size);
    if (kv == NULL) {
        kv_err("malloc kv err");
        return -1;
    }

    memset(kv, 0, read_size);
    if (read_kv_item(file->filename, kv, i) != 0) {
        kv_err("read kv err");
        free_kv(kv);
        return -1;
    }
    p = &kv[j];

    while (p && p->value_len) { /* if key is already stroed, update its value */

        if (strcmp(p->key, key) == 0) {
            memset(p->value, 0, ITEM_MAX_VAL_LEN);
            memcpy(p->value, value, value_len);
            p->value_len = value_len;
            break;
        }

        if (++j == TABLE_ROW_SIZE) {
            kv_err("hash row full");
            free(kv);
            return -1;
        }
        p = &kv[j];
    }

    p = &kv[j];
    if (p && !p->value_len) {/* if key has not been stored, then add it */
        //p->next = NULL;
        strncpy(p->key, key, ITEM_MAX_KEY_LEN - 1);
        memcpy(p->value, value, value_len);
        p->value_len = value_len;
    }

    if (write_kv_item(file->filename, kv, i) < 0) {
        kv_err("write_kv_item err");
        free(kv);
        return -1;
    }
    free(kv);
    return 0;
}

/* get a value indexed by key */
static int hash_table_get(kv_file_t *file, const char *key, void *value, int *len)
{
    int i;
    int read_size;
    kv_item_t *kv;
    int j = 0;
    struct kv *p;
    if (!file || !file->filename || !key || !value || !len  || *len <= 0) {
        kv_err("paras err");
        return -1;
    }

    i = hash_gen(key);

    read_size = sizeof(kv_item_t) * TABLE_ROW_SIZE;
    kv = malloc(read_size);
    if (kv == NULL) {
        kv_err("malloc kv err");
        return -1;
    }

    memset(kv, 0, read_size);
    if (read_kv_item(file->filename, kv, i) != 0) {
        kv_err("read kv err");
        free_kv(kv);
        return -1;
    }

    // struct kv *p = ht->table[i];
    p = &kv[j];

    while (p && p->value_len) {
        if (strcmp(key, p->key) == 0) {
            *len = p->value_len < *len ? p->value_len : *len;
            memcpy(value, p->value, *len);
            free_kv(kv);
            return 0;
        }
        if (++j == TABLE_ROW_SIZE) {
            break;
        }
        p = &kv[j];
    }
    free_kv(kv);
    kv_err("not found");
    return -1;
}

/* remove a value indexed by key */
static int hash_table_rm(kv_file_t *file,  const  char *key)
{
    int i;
    int read_size;
    kv_item_t *kv;
    int j = 0;
    struct kv *p;
    if (!file || !file->filename ||  !key) {
        return -1;
    }
    i = hash_gen(key) % TABLE_COL_SIZE;
    read_size = sizeof(kv_item_t) * TABLE_ROW_SIZE;
    kv = malloc(read_size);
    if (kv == NULL) {
        return -1;
    }

    memset(kv, 0, read_size);
    if (read_kv_item(file->filename, kv, i) != 0) {
        free_kv(kv);
        return -1;
    }

    p = &kv[j];

    while (p && p->value_len) {
        if (strcmp(key, p->key) == 0) {
            memset(p, 0, ITEM_MAX_LEN);
        }
        if (++j == TABLE_ROW_SIZE) {
            break;
        }
        p = &kv[j];
    }

    if (write_kv_item(file->filename, kv, i) < 0) {
        free_kv(kv);
        return -1;
    }
    free_kv(kv);
    return 0;
}

static int read_kv_item(const char *filename, void *buf, int location)
{
    struct stat st;
    int ret = 0;
    int offset;
    int fd = open(filename, O_RDONLY);

    if (fd < 0) {
        kv_err("open err");
        return -1;
    }

    if (fstat(fd, &st) < 0) {
        kv_err("fstat err");
        close(fd);
        return -1;
    }

    if (st.st_size < (location + 1) *ITEM_MAX_LEN * TABLE_ROW_SIZE) {
        kv_err("read overstep");
        close(fd);
        return -1;
    }

    offset =  location * ITEM_MAX_LEN * TABLE_ROW_SIZE;
    ret = lseek(fd, offset, SEEK_SET);
    if (ret < 0) {
        kv_err("lseek err");
        close(fd);
        return -1;
    }

    if (read(fd, buf, ITEM_MAX_LEN * TABLE_ROW_SIZE) != ITEM_MAX_LEN * TABLE_ROW_SIZE) {
        kv_err("read err");
        close(fd);
        return -1;
    }

    close(fd);
    return 0;
}

static int write_kv_item(const char *filename, void *data, int location)
{
    struct stat st;
    int offset;
    int ret;
    int fd = open(filename, O_WRONLY);
    if (fd < 0) {
        return -1;
    }

    if (fstat(fd, &st) < 0) {
        kv_err("fstat err");
        close(fd);
        return -1;
    }

    if (st.st_size < (location + 1) *ITEM_MAX_LEN * TABLE_ROW_SIZE) {
        kv_err("overstep st.st_size = %ld location =%d cur loc=%ld", st.st_size, location,
               (location + 1) *ITEM_MAX_LEN * TABLE_ROW_SIZE);
        close(fd);
        return -1;
    }

    offset = (location) * ITEM_MAX_LEN * TABLE_ROW_SIZE;
    ret = lseek(fd, offset, SEEK_SET);
    if (ret < 0) {
        kv_err("lseek err");
        close(fd);
        return -1;
    }

    if (write(fd, data, ITEM_MAX_LEN * TABLE_ROW_SIZE) != ITEM_MAX_LEN * TABLE_ROW_SIZE) {
        kv_err("kv write failed");
        close(fd);
        return -1;
    }

    fsync(fd);
    close(fd);

    return 0;
}

static int create_hash_file(kv_file_t *hash_kv)
{
    int i;
    int fd;
    char init_data[ITEM_MAX_LEN * TABLE_ROW_SIZE] = {0};
    if (hash_kv == NULL) {
        return -1;
    }
    fd = open(hash_kv->filename, O_CREAT | O_RDWR, 0644);
    if (fd < 0) {
        return -1;
    }

    for (i = 0; i < TABLE_COL_SIZE ; i++) {

        if (write(fd, init_data, ITEM_MAX_LEN * TABLE_ROW_SIZE) != ITEM_MAX_LEN *
            TABLE_ROW_SIZE) { /* 3 = '{}' + null terminator */
            kv_err("write err");
            close(fd);
            return -1;
        }
    }

    if (fsync(fd) < 0) {
        close(fd);
        return -1;
    }

    close(fd);
    return 0;
}


static kv_file_t *kv_open(const char *filename)
{
    kv_file_t *file = malloc(sizeof(kv_file_t));
    if (!file) {
        return NULL;
    }
    memset(file, 0, sizeof(kv_file_t));

    file->filename = filename;
    pthread_mutex_init(&file->lock, NULL);
    pthread_mutex_lock(&file->lock);

    if (access(file->filename, F_OK) < 0) {
        /* create KV file when not exist */
        if (create_hash_file(file) < 0) {
            goto fail;
        }
    }
    pthread_mutex_unlock(&file->lock);
    return file;
fail:
    pthread_mutex_unlock(&file->lock);
    free(file);

    return NULL;
}

static int __kv_get(kv_file_t *file, const char *key, void *value, int *value_len)
{
    int ret;
    if (!file || !key || !value || !value_len || *value_len <= 0) {
        return -1;
    }

    pthread_mutex_lock(&file->lock);
    ret = hash_table_get(file, key, value, value_len);
    pthread_mutex_unlock(&file->lock);

    return ret;
}

static int __kv_set(kv_file_t *file, const char *key, void *value, int value_len)
{
    int ret;
    if (!file || !key || !value || value_len <= 0) {
        return -1;
    }

    pthread_mutex_lock(&file->lock);
    ret = hash_table_put(file, key, value, value_len);
    pthread_mutex_unlock(&file->lock);

    return ret;
}

int __kv_del(kv_file_t *file, const  char *key)
{
    int ret;
    if (!file || !key) {
        return -1;
    }

    /* remove old value if exist */
    pthread_mutex_lock(&file->lock);
    ret = hash_table_rm(file, key);
    pthread_mutex_unlock(&file->lock);

    return ret;
}

static kv_file_t *file = NULL;
static int kv_get(const char *key, void *value, int *value_len)
{
    if (!file) {
        file = kv_open(KV_FILE_NAME);
        if (!file) {
            kv_err("kv_open failed");
            return -1;
        }
    }

    return __kv_get(file, key, value, value_len);
}

static int kv_set(const char *key, void *value, int value_len)
{
    if (!file) {
        file = kv_open(KV_FILE_NAME);
        if (!file) {
            kv_err("kv_open failed");
            return -1;
        }
    }

    return __kv_set(file, key, value, value_len);
}

static int kv_del(const char *key)
{
    if (!file) {
        file = kv_open(KV_FILE_NAME);
        if (!file) {
            kv_err("kv_open failed");
            return -1;
        }
    }

    return __kv_del(file, key);
}

int HAL_Kv_Get(const char *key, void *val, int *buffer_len)
{
	return kv_get(key, val, buffer_len);
}


int HAL_Kv_Set(const char *key, const void *val, int len, int sync)
{
	return kv_set(key, (void *)val, len);
}


/**
 * @brief Allocates a block of size bytes of memory, returning a pointer to the beginning of the block.
 *
 * @param [in] size @n specify block size in bytes.
 * @return A pointer to the beginning of the block.
 * @see None.
 * @note Block value is indeterminate.
 */
void *HAL_Malloc(uint32_t size)
{
	return malloc(size);
}


/**
 * @brief Create a mutex.
 *
 * @retval NULL : Initialize mutex failed.
 * @retval NOT_NULL : The mutex handle.
 * @see None.
 * @note None.
 */
void *HAL_MutexCreate(void)
{
    int err_num;
    pthread_mutex_t *mutex = (pthread_mutex_t *)HAL_Malloc(sizeof(pthread_mutex_t));
    if (NULL == mutex) {
        return NULL;
    }

    if (0 != (err_num = pthread_mutex_init(mutex, NULL))) {
        perror("create mutex failed\n");
        HAL_Free(mutex);
        return NULL;
    }

    return mutex;
}


/**
 * @brief Destroy the specified mutex object, it will release related resource.
 *
 * @param [in] mutex @n The specified mutex.
 * @return None.
 * @see None.
 * @note None.
 */
void HAL_MutexDestroy(void *mutex)
{
    int err_num;

    if (!mutex) {
        perror("mutex want to destroy is NULL!\n");
        return;
    }
    if (0 != (err_num = pthread_mutex_destroy((pthread_mutex_t *)mutex))) {
        perror("destroy mutex failed\n");
    }

    HAL_Free(mutex);
}


/**
 * @brief Waits until the specified mutex is in the signaled state.
 *
 * @param [in] mutex @n the specified mutex.
 * @return None.
 * @see None.
 * @note None.
 */
void HAL_MutexLock(void *mutex)
{
    int err_num;
    if (0 != (err_num = pthread_mutex_lock((pthread_mutex_t *)mutex))) {
        printf("lock mutex failed: - '%s' (%d)\n", strerror(err_num), err_num);
    }
}


/**
 * @brief Releases ownership of the specified mutex object..
 *
 * @param [in] mutex @n the specified mutex.
 * @return None.
 * @see None.
 * @note None.
 */
void HAL_MutexUnlock(void *mutex)
{
    int err_num;
    if (0 != (err_num = pthread_mutex_unlock((pthread_mutex_t *)mutex))) {
        printf("unlock mutex failed - '%s' (%d)\n", strerror(err_num), err_num);
    }
}


/**
 * @brief Writes formatted data to stream.
 *
 * @param [in] fmt: @n String that contains the text to be written, it can optionally contain embedded format specifiers
     that specifies how subsequent arguments are converted for output.
 * @param [in] ...: @n the variable argument list, for formatted and inserted in the resulting string replacing their respective specifiers.
 * @return None.
 * @see None.
 * @note None.
 */
void HAL_Printf(const char *fmt, ...)
{
    va_list args;

    va_start(args, fmt);
    vprintf(fmt, args);
    va_end(args);

    fflush(stdout);
}


uint32_t HAL_Random(uint32_t region)
{
	FILE *handle;
    ssize_t ret = 0;
    uint32_t output = 0;
    handle = fopen("/dev/urandom", "r");
    if (handle == NULL) {
        perror("open /dev/urandom failed\n");
        return 0;
    }
    ret = fread(&output, sizeof(uint32_t), 1, handle);
    if (ret != 1) {
        printf("fread error: %d\n", (int)ret);
        fclose(handle);
        return 0;
    }
    fclose(handle);
    return (region > 0) ? (output % region) : 0;
}


void HAL_Reboot()
{
	if (system("reboot")) {
        perror("HAL_Reboot failed");
    }
}


/**
 * @brief   create a semaphore
 *
 * @return semaphore handle.
 * @see None.
 * @note The recommended value of maximum count of the semaphore is 255.
 */
void *HAL_SemaphoreCreate(void)
{
    sem_t *sem = (sem_t *)malloc(sizeof(sem_t));
    if (NULL == sem) {
        return NULL;
    }

    if (0 != sem_init(sem, 0, 0)) {
        free(sem);
        return NULL;
    }

    return sem;
}


/**
 * @brief   destory a semaphore
 *
 * @param[in] sem @n the specified sem.
 * @return None.
 * @see None.
 * @note None.
 */
void HAL_SemaphoreDestroy(void *sem)
{
    sem_destroy((sem_t *)sem);
    free(sem);
}


/**
 * @brief   signal thread wait on a semaphore
 *
 * @param[in] sem @n the specified semaphore.
 * @return None.
 * @see None.
 * @note None.
 */
void HAL_SemaphorePost(void *sem)
{
	sem_post((sem_t *)sem);
}


/**
 * @brief   wait on a semaphore
 *
 * @param[in] sem @n the specified semaphore.
 * @param[in] timeout_ms @n timeout interval in millisecond.
     If timeout_ms is PLATFORM_WAIT_INFINITE, the function will return only when the semaphore is signaled.
 * @return
   @verbatim
   =  0: The state of the specified object is signaled.
   =  -1: The time-out interval elapsed, and the object's state is nonsignaled.
   @endverbatim
 * @see None.
 * @note None.
 */
int HAL_SemaphoreWait(void *sem, uint32_t timeout_ms)
{
    if (PLATFORM_WAIT_INFINITE == timeout_ms) {
        sem_wait(sem);
        return 0;
    } else {
        struct timespec ts;
        int s;
        /* Restart if interrupted by handler */
        do {
            if (clock_gettime(CLOCK_REALTIME, &ts) == -1) {
                return -1;
            }

            s = 0;
            ts.tv_nsec += (timeout_ms % 1000) * 1000000;
            if (ts.tv_nsec >= 1000000000) {
                ts.tv_nsec -= 1000000000;
                s = 1;
            }

            ts.tv_sec += timeout_ms / 1000 + s;

        } while (((s = sem_timedwait(sem, &ts)) != 0) && errno == EINTR);

        return (s == 0) ? 0 : -1;
    }
}


/**
 * @brief Sleep thread itself.
 *
 * @param [in] ms @n the time interval for which execution is to be suspended, in milliseconds.
 * @return None.
 * @see None.
 * @note None.
 */
void HAL_SleepMs(uint32_t ms)
{
    usleep(1000 * ms);
}


/**
 * @brief Writes formatted data to string.
 *
 * @param [out] str: @n String that holds written text.
 * @param [in] len: @n Maximum length of character will be written
 * @param [in] fmt: @n Format that contains the text to be written, it can optionally contain embedded format specifiers
     that specifies how subsequent arguments are converted for output.
 * @param [in] ...: @n the variable argument list, for formatted and inserted in the resulting string replacing their respective specifiers.
 * @return bytes of character successfully written into string.
 * @see None.
 * @note None.
 */
int HAL_Snprintf(char *str, const int len, const char *fmt, ...)
{
    va_list args;
    int     rc;

    va_start(args, fmt);
    rc = vsnprintf(str, len, fmt, args);
    va_end(args);

    return rc;
}


void HAL_Srandom(uint32_t seed)
{
    srandom(seed);
}

static int do_cmd_exec(char *cmd, char *result, int len)
{
    char buf[1024];
    FILE *filp;
    int ret = 0;
    filp = popen(cmd, "r");
    if (!filp) {
        printf("cmd: %s failed\n", cmd);
        return -1;
    }
    memset(buf, '\0', sizeof(buf));
    ret = fread(buf, sizeof(char), sizeof(buf) - 1, filp);
    pclose(filp);

    return snprintf(result, len, "%s", buf);
}

int HAL_Sys_Net_Is_Ready()
{
    char result_buf[1024] = {0};
    do_cmd_exec("ifconfig", result_buf, sizeof(result_buf));
    if (strstr(result_buf, "inet addr")) {
        return 1;
    }
    return 0;
}


/**
 * @brief Destroy the specific TCP connection.
 *
 * @param [in] fd: @n Specify the TCP connection by handle.
 *
 * @return The result of destroy TCP connection.
 * @retval < 0 : Fail.
 * @retval   0 : Success.
 */
    int HAL_TCP_Destroy(uintptr_t fd)
{
    int rc;

    /* Shutdown both send and receive operations. */
    rc = shutdown((int) fd, 2);
    if (0 != rc) {
        printf("shutdown error\n");
        return -1;
    }

    rc = close((int) fd);
    if (0 != rc) {
        printf("closesocket error\n");
        return -1;
    }

    return 0;
}


/**
 * @brief Establish a TCP connection.
 *
 * @param [in] host: @n Specify the hostname(IP) of the TCP server
 * @param [in] port: @n Specify the TCP port of TCP server
 *
 * @return The handle of TCP connection.
   @retval (uintptr_t)(-1): Fail.
   @retval All other values(0 included): Success, the value is handle of this TCP connection.
 */
    uintptr_t HAL_TCP_Establish(const char *host, uint16_t port)
{
    struct addrinfo hints;
    struct addrinfo *addrInfoList = NULL;
    struct addrinfo *cur = NULL;
    int fd = 0;
    int rc = 0;
    char service[6];

    memset(&hints, 0, sizeof(hints));

    printf("establish tcp connection with server(host='%s', port=[%u])\n", host, port);

    hints.ai_family = AF_INET; /* only IPv4 */
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;
    sprintf(service, "%u", port);

    if ((rc = getaddrinfo(host, service, &hints, &addrInfoList)) != 0) {
        printf("getaddrinfo error(%d), host = '%s', port = [%d]\n", rc, host, port);
        return (uintptr_t)(-1);
    }

    for (cur = addrInfoList; cur != NULL; cur = cur->ai_next) {
        if (cur->ai_family != AF_INET) {
            printf("socket type error\n");
            rc = -1;
            continue;
        }

        fd = socket(cur->ai_family, cur->ai_socktype, cur->ai_protocol);
        if (fd < 0) {
            printf("create socket error\n");
            rc = -1;
            continue;
        }

        if (connect(fd, cur->ai_addr, cur->ai_addrlen) == 0) {
            rc = fd;
            break;
        }

        close(fd);
        printf("connect error\n");
        rc = -1;
    }

    if (-1 == rc) {
        printf("fail to establish tcp\n");
    } else {
        printf("success to establish tcp, fd=%d\n", rc);
    }
    freeaddrinfo(addrInfoList);

    return (uintptr_t)rc;
}


/**
 * @brief Read data from the specific TCP connection with timeout parameter.
 *        The API will return immediately if 'len' be received from the specific TCP connection.
 *
 * @param [in] fd @n A descriptor identifying a TCP connection.
 * @param [out] buf @n A pointer to a buffer to receive incoming data.
 * @param [out] len @n The length, in bytes, of the data pointed to by the 'buf' parameter.
 * @param [in] timeout_ms @n Specify the timeout value in millisecond. In other words, the API block 'timeout_ms' millisecond maximumly.
 *
 * @retval       -2 : TCP connection error occur.
 * @retval       -1 : TCP connection be closed by remote server.
 * @retval        0 : No any data be received in 'timeout_ms' timeout period.
 * @retval (0, len] : The total number of bytes be received in 'timeout_ms' timeout period.

 * @see None.
 */
    int32_t HAL_TCP_Read(uintptr_t fd, char *buf, uint32_t len, uint32_t timeout_ms)
{
    int ret, err_code, tcp_fd;
    uint32_t len_recv;
    uint64_t t_end, t_left;
    fd_set sets;
    struct timeval timeout;

    t_end = _linux_get_time_ms() + timeout_ms;
    len_recv = 0;
    err_code = 0;

    if (fd >= FD_SETSIZE) {
        return -1;
    }
    tcp_fd = (int)fd;

    do {
        t_left = _linux_time_left(t_end, _linux_get_time_ms());
        if (0 == t_left) {
            break;
        }
        FD_ZERO(&sets);
        FD_SET(tcp_fd, &sets);

        timeout.tv_sec = t_left / 1000;
        timeout.tv_usec = (t_left % 1000) * 1000;

        ret = select(tcp_fd + 1, &sets, NULL, NULL, &timeout);
        if (ret > 0) {
            ret = recv(tcp_fd, buf + len_recv, len - len_recv, 0);
            if (ret > 0) {
                len_recv += ret;
            } else if (0 == ret) {
                printf("connection is closed\n");
                err_code = -1;
                break;
            } else {
                if (EINTR == errno) {
                    continue;
                }
                printf("recv fail\n");
                err_code = -2;
                break;
            }
        } else if (0 == ret) {
            break;
        } else {
            if (EINTR == errno) {
                continue;
            }
            printf("select-recv fail\n");
            err_code = -2;
            break;
        }
    } while ((len_recv < len));

    /* priority to return data bytes if any data be received from TCP connection. */
    /* It will get error code on next calling */
    return (0 != len_recv) ? len_recv : err_code;
}


/**
 * @brief Write data into the specific TCP connection.
 *        The API will return immediately if 'len' be written into the specific TCP connection.
 *
 * @param [in] fd @n A descriptor identifying a connection.
 * @param [in] buf @n A pointer to a buffer containing the data to be transmitted.
 * @param [in] len @n The length, in bytes, of the data pointed to by the 'buf' parameter.
 * @param [in] timeout_ms @n Specify the timeout value in millisecond. In other words, the API block 'timeout_ms' millisecond maximumly.
 *
 * @retval      < 0 : TCP connection error occur..
 * @retval        0 : No any data be write into the TCP connection in 'timeout_ms' timeout period.
 * @retval (0, len] : The total number of bytes be written in 'timeout_ms' timeout period.

 * @see None.
 */
    int32_t HAL_TCP_Write(uintptr_t fd, const char *buf, uint32_t len, uint32_t timeout_ms)
{
    int ret, tcp_fd;
    uint32_t len_sent;
    uint64_t t_end, t_left;
    fd_set sets;
    int net_err = 0;

    t_end = _linux_get_time_ms() + timeout_ms;
    len_sent = 0;
    ret = 1; /* send one time if timeout_ms is value 0 */

    if (fd >= FD_SETSIZE) {
        return -1;
    }
    tcp_fd = (int)fd;

    do {
        t_left = _linux_time_left(t_end, _linux_get_time_ms());

        if (0 != t_left) {
            struct timeval timeout;

            FD_ZERO(&sets);
            FD_SET(tcp_fd, &sets);

            timeout.tv_sec = t_left / 1000;
            timeout.tv_usec = (t_left % 1000) * 1000;

            ret = select(tcp_fd + 1, NULL, &sets, NULL, &timeout);
            if (ret > 0) {
                if (0 == FD_ISSET(tcp_fd, &sets)) {
                    printf("Should NOT arrive\n");
                    /* If timeout in next loop, it will not sent any data */
                    ret = 0;
                    continue;
                }
            } else if (0 == ret) {
                printf("select-write timeout %d\n", tcp_fd);
                break;
            } else {
                if (EINTR == errno) {
                    printf("EINTR be caught\n");
                    continue;
                }

                printf("select-write fail, ret = select() = %d\n", ret);
                net_err = 1;
                break;
            }
        }

        if (ret > 0) {
            ret = send(tcp_fd, buf + len_sent, len - len_sent, 0);
            if (ret > 0) {
                len_sent += ret;
            } else if (0 == ret) {
                printf("No data be sent\n");
            } else {
                if (EINTR == errno) {
                    printf("EINTR be caught\n");
                    continue;
                }

                printf("send fail, ret = send() = %d\n", ret);
                net_err = 1;
                break;
            }
        }
    } while (!net_err && (len_sent < len) && (_linux_time_left(t_end, _linux_get_time_ms()) > 0));

    if (net_err) {
        return -1;
    } else {
        return len_sent;
    }
}

void HAL_ThreadDelete(void *thread_handle)
{
    if (NULL == thread_handle) {
        pthread_exit(0);
    } else {
        /*main thread delete child thread*/
        pthread_cancel((pthread_t)thread_handle);
        pthread_join((pthread_t)thread_handle, 0);
    }
}


void *HAL_Timer_Create(const char *name, void (*func)(void *), void *user_data)
{
    timer_t *timer = NULL;

    struct sigevent ent;

    /* check parameter */
    if (func == NULL) {
        return NULL;
    }

    timer = (timer_t *)malloc(sizeof(time_t));
    if (timer == NULL) {
        return NULL;
    }

    /* Init */
    memset(&ent, 0x00, sizeof(struct sigevent));

    /* create a timer */
    ent.sigev_notify = SIGEV_THREAD;
    ent.sigev_notify_function = (void (*)(union sigval))func;
    ent.sigev_value.sival_ptr = user_data;

    printf("HAL_Timer_Create\n");

    if (timer_create(CLOCK_MONOTONIC, &ent, timer) != 0) {
        free(timer);
        return NULL;
    }

    return (void *)timer;
}


int HAL_Timer_Delete(void *timer)
{
    int ret = 0;

    /* check parameter */
    if (timer == NULL) {
        return -1;
    }

    ret = timer_delete(*(timer_t *)timer);

    free(timer);

    return ret;
}


int HAL_Timer_Start(void *timer, int ms)
{
    struct itimerspec ts;

    /* check parameter */
    if (timer == NULL) {
        return -1;
    }

    /* it_interval=0: timer run only once */
    ts.it_interval.tv_sec = 0;
    ts.it_interval.tv_nsec = 0;

    /* it_value=0: stop timer */
    ts.it_value.tv_sec = ms / 1000;
    ts.it_value.tv_nsec = (ms % 1000) * 1000000;

    return timer_settime(*(timer_t *)timer, 0, &ts, NULL);
}


int HAL_Timer_Stop(void *timer)
{
    struct itimerspec ts;

    /* check parameter */
    if (timer == NULL) {
        return -1;
    }

    /* it_interval=0: timer run only once */
    ts.it_interval.tv_sec = 0;
    ts.it_interval.tv_nsec = 0;

    /* it_value=0: stop timer */
    ts.it_value.tv_sec = 0;
    ts.it_value.tv_nsec = 0;

    return timer_settime(*(timer_t *)timer, 0, &ts, NULL);
}


int HAL_UDP_close_without_connect(intptr_t sockfd)
{
	return close((int)sockfd);
}


intptr_t HAL_UDP_create_without_connect(const char *host, unsigned short port)
{
    struct sockaddr_in addr;
    long sockfd;
    int opt_val = 1;
    struct hostent *hp;
    struct in_addr in;
    uint32_t ip;

    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        printf("socket");
        return -1;
    }
    if (0 == port) {
        return (intptr_t)sockfd;
    }

    memset(&addr, 0, sizeof(struct sockaddr_in));

    if (0 != setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR | SO_BROADCAST, &opt_val, sizeof(opt_val))) {
        printf("setsockopt");
        close(sockfd);
        return -1;
    }

    if (NULL == host) {
        addr.sin_addr.s_addr = htonl(INADDR_ANY);
    } else {
        if (inet_aton(host, &in)) {
            ip = *(uint32_t *)&in;
        } else {
            hp = gethostbyname(host);
            if (!hp) {
                printf("can't resolute the host address \n");
                close(sockfd);
                return -1;
            }
            ip = *(uint32_t *)(hp->h_addr);
        }
        addr.sin_addr.s_addr = ip;
    }
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);

    if (-1 == bind(sockfd, (struct sockaddr *)&addr, sizeof(struct sockaddr_in))) {
        close(sockfd);
        return -1;
    }
    printf("success to establish udp, fd=%d", (int)sockfd);

    return (intptr_t)sockfd;
}


int HAL_UDP_joinmulticast(intptr_t sockfd,
                          char *p_group)
{
    int err = -1;
    int socket_id = -1;
    int loop = 0;
    struct ip_mreq mreq;

    if (NULL == p_group) {
        return -1;
    }

    /*set loopback*/
    socket_id = (int)sockfd;
    err = setsockopt(socket_id, IPPROTO_IP, IP_MULTICAST_LOOP, &loop, sizeof(loop));
    if (err < 0) {
        printf("setsockopt");
        return err;
    }

    mreq.imr_multiaddr.s_addr = inet_addr(p_group);
    mreq.imr_interface.s_addr = htonl(INADDR_ANY); /*default networt interface*/

    /*join to the multicast group*/
    err = setsockopt(socket_id, IPPROTO_IP, IP_ADD_MEMBERSHIP, &mreq, sizeof(mreq));
    if (err < 0) {
        printf("setsockopt");
        return err;
    }

    return 0;
}


int HAL_UDP_recvfrom(intptr_t sockfd,
                     NetworkAddr *p_remote,
                     unsigned char *p_data,
                     unsigned int datalen,
                     unsigned int timeout_ms)
{
    int ret;
    struct sockaddr_in addr;
    socklen_t addr_len = sizeof(addr);
    fd_set read_fds;
    struct timeval timeout = {timeout_ms / 1000, (timeout_ms % 1000) * 1000};

    FD_ZERO(&read_fds);
    FD_SET(sockfd, &read_fds);

    ret = select(sockfd + 1, &read_fds, NULL, NULL, &timeout);
    if (ret == 0) {
        return 0;    /* receive timeout */
    }

    if (ret < 0) {
        if (errno == EINTR) {
            return -3;    /* want read */
        }
        return -4; /* receive failed */
    }

    ret = recvfrom(sockfd, p_data, datalen, 0, (struct sockaddr *)&addr, &addr_len);
    if (ret > 0) {
        if (NULL != p_remote) {
            p_remote->port = ntohs(addr.sin_port);

            strcpy((char *)p_remote->addr, inet_ntoa(addr.sin_addr));
        }

        return ret;
    }

    return -1;
}


int HAL_UDP_sendto(intptr_t sockfd,
                   const NetworkAddr *p_remote,
                   const unsigned char *p_data,
                   unsigned int datalen,
                   unsigned int timeout_ms)
{
    int ret;
    uint32_t ip;
    struct in_addr in;
    struct hostent *hp;
    struct sockaddr_in addr;
    fd_set write_fds;
    struct timeval timeout = {timeout_ms / 1000, (timeout_ms % 1000) * 1000};

    if (inet_aton((char *)p_remote->addr, &in)) {
        ip = *(uint32_t *)&in;
    } else {
        hp = gethostbyname((char *)p_remote->addr);
        if (!hp) {
            printf("can't resolute the host address \n");
            return -1;
        }
        ip = *(uint32_t *)(hp->h_addr);
    }

    FD_ZERO(&write_fds);
    FD_SET(sockfd, &write_fds);

    ret = select(sockfd + 1, NULL, &write_fds, NULL, &timeout);
    if (ret == 0) {
        return 0;    /* write timeout */
    }

    if (ret < 0) {
        if (errno == EINTR) {
            return -3;    /* want write */
        }
        return -4; /* write failed */
    }

    addr.sin_addr.s_addr = ip;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(p_remote->port);

    ret = sendto(sockfd, p_data, datalen, 0, (struct sockaddr *)&addr, sizeof(struct sockaddr_in));

    if (ret < 0) {
        printf("sendto");
    }

    return (ret) > 0 ? ret : -1;
}


/**
 * @brief Retrieves the number of milliseconds that have elapsed since the system was boot.
 *
 * @return the number of milliseconds.
 * @see None.
 * @note None.
 */
uint64_t HAL_UptimeMs(void)
{
    uint64_t            time_ms;
    struct timespec     ts;

    clock_gettime(CLOCK_MONOTONIC, &ts);
    time_ms = ((uint64_t)ts.tv_sec * (uint64_t)1000) + (ts.tv_nsec / 1000 / 1000);

    return time_ms;
}


int HAL_Vsnprintf(char *str, const int len, const char *format, va_list ap)
{
    return vsnprintf(str, len, format, ap);
}

static void read_string_from_file(char *dst, const char *file, int dst_max_len)
{
    FILE *fp;
    int ret = 0;
    int lSize;
    char buffer[256] = {0};
    fp = fopen(file, "r");
    if (NULL == fp) {
        return;
    }
    fseek(fp, 0, SEEK_END);
    lSize = ftell(fp);
    rewind(fp);
    if (lSize > 1 && lSize <= dst_max_len) {
        unsigned char pos = lSize - 1;
        ret = fread(dst, 1, lSize, fp);
        dst[pos] = 0;
    } else {
        /* the ssid has only one char, or illeagal */
        /*  put terminator at the first position */
        dst[0] = 0;
    }
    fclose(fp);
    snprintf(buffer, 256, "rm %s -f", file);
    ret = system(buffer);
    if (ret != 0) {
        printf("delete file %s error", file);
    }
}

int HAL_Wifi_Get_Ap_Info(char ssid[HAL_MAX_SSID_LEN],char passwd[HAL_MAX_PASSWD_LEN],uint8_t bssid[ETH_ALEN])
{
#define MAXLINE 256
    char buffer[256] = {0};
    int ret = 0;
    char *data;

    if (NULL != ssid) {
        ret = system("wpa_cli status | grep ^ssid | sed 's/^ssid=//g' > /tmp/ssid");
        read_string_from_file(ssid, "/tmp/ssid", HAL_MAX_SSID_LEN);
    }

    if (NULL != passwd) {
        ret = system("wpa_cli status | grep ^passphrase | sed 's/^passphrase=//g' > /tmp/passphrase");
        read_string_from_file(passwd, "/tmp/passphrase", HAL_MAX_PASSWD_LEN);
    }

    if (NULL != bssid) {
        ret = system("wpa_cli status | grep ^bssid | sed 's/^bssid=//g' > /tmp/bssid");
        read_string_from_file((char *)bssid, "/tmp/bssid", ETH_ALEN);
    }

    return 0;
}

char *_get_default_routing_ifname(char *ifname, int ifname_size)
{
    FILE *fp = NULL;
    char line[ROUTER_RECORD_SIZE] = {0};
    char iface[IFNAMSIZ] = {0};
    char *result = NULL;
    unsigned int destination, gateway, flags, mask;
    unsigned int refCnt, use, metric, mtu, window, irtt;
    char *buff = NULL;

    fp = fopen(ROUTER_INFO_PATH, "r");
    if (fp == NULL) {
        perror("fopen");
        return result;
    }

    buff = fgets(line, sizeof(line), fp);
    if (buff == NULL) {
        perror("fgets");
        goto out;
    }

    while (fgets(line, sizeof(line), fp)) {
        if (11 !=
            sscanf(line, "%s %08x %08x %x %d %d %d %08x %d %d %d",
                   iface, &destination, &gateway, &flags, &refCnt, &use,
                   &metric, &mask, &mtu, &window, &irtt)) {
            perror("sscanf");
            continue;
        }

        /*default route */
        if ((destination == 0) && (mask == 0)) {
            strncpy(ifname, iface, ifname_size - 1);
            result = ifname;
            break;
        }
    }

out:
    if (fp) {
        fclose(fp);
    }

    return result;
}

uint32_t HAL_Wifi_Get_IP(char ip_str[NETWORK_ADDR_LEN], const char *ifname)
{
    struct ifreq ifreq;
    int sock = -1;
    char ifname_buff[IFNAMSIZ] = {0};

    if ((NULL == ifname || strlen(ifname) == 0) &&
        NULL == (ifname = _get_default_routing_ifname(ifname_buff, sizeof(ifname_buff)))) {
        perror("get default routeing ifname");
        return -1;
    }

    if ((sock = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("socket");
        return -1;
    }

    ifreq.ifr_addr.sa_family = AF_INET;
    strncpy(ifreq.ifr_name, ifname, IFNAMSIZ - 1);

    if (ioctl(sock, SIOCGIFADDR, &ifreq) < 0) {
        close(sock);
        perror("ioctl");
        return -1;
    }

    close(sock);

    strncpy(ip_str,
            inet_ntoa(((struct sockaddr_in *)&ifreq.ifr_addr)->sin_addr),
            NETWORK_ADDR_LEN);

    return ((struct sockaddr_in *)&ifreq.ifr_addr)->sin_addr.s_addr;
}


char *HAL_Wifi_Get_Mac(char mac_str[HAL_MAC_LEN])
{
    int fd = -1;
    int ret = -1;
    struct ifreq if_hwaddr;

    fd = socket(PF_INET, SOCK_STREAM, 0);
    if (fd < 0) {
        printf("socket error: ");
        return NULL ;
    }

    memset(&if_hwaddr, 0, sizeof(if_hwaddr));
    strncpy(if_hwaddr.ifr_name, g_ifname, sizeof(if_hwaddr.ifr_name) - 1);

    ret = ioctl(fd, SIOCGIFHWADDR, &if_hwaddr);
    if (ret >= 0) {
        unsigned char *hwaddr = (unsigned char *)if_hwaddr.ifr_hwaddr.sa_data;
        sprintf(mac_str, "%2.2x:%2.2x:%2.2x:%2.2x:%2.2x:%2.2x",
                hwaddr[0], hwaddr[1], hwaddr[2], hwaddr[3], hwaddr[4], hwaddr[5]);
    }

    close(fd);
    printf("mac_str: %s\n", mac_str);
    return mac_str;
}


