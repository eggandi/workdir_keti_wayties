#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>
#include <fcntl.h>
#include <termios.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <netinet/in.h>
#include <errno.h>
#include <pthread.h>
#include <sys/timerfd.h>

#ifndef _D_HEADER_CONFIG
#define _D_HEADER_CONFIG

#include "pro_v2x.h"
#include "pro_pcap.h"
#include "utils.h"

#define PRO_INITAIL_SETUP_CONFIGURAION_FILE_PATH "./config"
#define PRO_INITAIL_SETUP_CONFIGURAION_FILE_NAME "pro_config.conf"

struct pro_config_t {
    bool config_enable;
    struct pro_config_utils_t utils;
    struct pro_config_v2x_t v2x;
    struct pro_config_pcap_t pcap;
};

#endif //?_D_HEADER_CONFIG

extern struct pro_config_t G_pro_config;
extern int PRO_Initial_Setup_Configuration_Read(struct pro_config_t *pro_config);