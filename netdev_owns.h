/*
* Copyright (C) 2016 Freie Universität Berlin
*
* This file is subject to the terms and conditions of the GNU Lesser
* General Public License v2.1. See the file LICENSE in the top level
* directory for more details.
*/

#ifndef NET_NETDEV_OWNS_H
#define NET_NETDEV_OWNS_H

#include <mutex.h>
#include <unistd.h>
#include <sys/uio.h>
#include <sys/types.h>

#ifdef MODULE_NETDEV_IEEE802154
#include "net/netdev/ieee802154.h"
#endif

#include "net/netdev.h"

#include "owns/owns.h"

#ifdef __cplusplus
extern "C" {
#endif


typedef struct {

    netdev_ieee802154_t netdev;         /* MODULE_NETDEV_IEEE802154 */

    void *state;                                    
    mutex_t mutex;

    struct ons_radio_s *radio;

    uint16_t address_short;
    uint64_t address_long;
    uint16_t net_id;
    uint16_t channel;


} netdev_owns_t;

void netdev_owns_setup(netdev_owns_t *dev, struct ons_radio_s *radio, void *state);

void netdev_owns_reset(netdev_owns_t *dev);

#ifdef __cplusplus
}
#endif

extern const netdev_driver_t owns_driver;

#ifdef __cplusplus
}
#endif

#endif /* NET_netdev_owns_H */
