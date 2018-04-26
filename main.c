/*
 * Copyright (C) 2008, 2009, 2010  Kaspar Schleiser <kaspar@schleiser.de>
 * Copyright (C) 2013 INRIA
 * Copyright (C) 2013 Ludwig Knüpfer <ludwig.knuepfer@fu-berlin.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#include <stdio.h>
#include <string.h>

#include "thread.h"
#include "log.h"
#include "shell.h"
#include "shell_commands.h"

#if FEATURE_PERIPH_RTC
#include "periph/rtc.h"
#endif

#ifdef MODULE_NETIF
#include "net/gnrc/pktdump.h"
#include "net/gnrc/netdev.h"
#include "net/gnrc/netdev/ieee802154.h"
#include "net/gnrc.h"
#endif

#include "netdev_owns.h"
#include "owns/owns.h"

static netdev_owns_t dev;
struct ons_config_s ons_config;
struct ons_s ons;
struct ons_radio_s radio;
gnrc_netdev_t radio_gnrc;

kernel_pid_t radio_pid;

char radio_stack[16 * 1024];

int owns_conn_cmd(int argc, char **argv) {
    int res = 0;

    if (argc != 3) {
        printf("USAGE: %s [SERVER] [ADDRESS]", argv[0]);
        return -1;
    }

    printf("Connecting to OWNS server %s with address %s\n", argv[1], argv[2]);

    ons_config.intercept_signals = false;

    //res = ONS_init(&ons, argv[1], argv[2], &ons_config);
    if (res < 0) {
        LOG_ERROR("Error %d creating ONS connector\n", res);
        return -2;
    }

    //res = ONS_radio_init(&ons, &radio, "433MHz");
    if (res < 0) {
        (void) puts("ERR2");
        LOG_ERROR("Error %d creating ONS virtual radio\n", res);
        return -3;
    }

    //netdev_owns_setup(&dev, &radio, NULL);

    res = gnrc_netdev_ieee802154_init(&radio_gnrc,(netdev_ieee802154_t *)&dev);
    if (res < 0) {
        LOG_ERROR("OWNS error intiialising GNRC netdev");
        return -4;
    }

    radio_pid = gnrc_netdev_init(radio_stack, sizeof(radio_stack), GNRC_NETDEV_MAC_PRIO, "ownsc", &radio_gnrc);

    //gnrc_netif_add(radio_pid);

    printf("Configured OWNS connector (PID: %d)\n", radio_pid);

    return 0;
}

int owns_stat_cmd(int argc, char **argv) {
    if (argc != 1) {
        printf("USAGE: %s", argv[0]);
        return -1;
    }

    ONS_status(&ons);

    return 0;
}

int owns_dc_cmd(int argc, char **argv) {
    if (argc != 1) {
        printf("USAGE: %s", argv[0]);
        return -1;
    }

    printf("Disconnecting OWNS\n");

    ONS_radio_close(&ons, &radio);

    ONS_close(&ons);

    return 0;
}


static const shell_command_t shell_commands[] = {
    { "owns-conn", "Connect to OWNS server", owns_conn_cmd },
    { "owns-stat", "Get OWNS server status", owns_stat_cmd },
    { "owns-dc", "Get OWNS server status", owns_dc_cmd },
    { NULL, NULL, NULL }
};

int main() {
    printf("OWNS-RIOT\n");

    char line_buf[SHELL_DEFAULT_BUFSIZE];
    shell_run(shell_commands, line_buf, SHELL_DEFAULT_BUFSIZE);

    return 0;
}
