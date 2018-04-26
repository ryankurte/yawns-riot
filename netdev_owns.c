/*
 * Copyright (C) 2016 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @{
 *
 * @file
 * @author  Martine Lenders <mlenders@inf.fu-berlin.de>
 */

#include <errno.h>
#include <stddef.h>
#include <string.h>

#include <sys/uio.h>
#include <sys/types.h>

#include "netdev_owns.h"

#include "owns/owns.h"

static int _send(netdev_t *netdev, const struct iovec *vector, unsigned count);
static int _recv(netdev_t *netdev, void *buff, size_t len, void *info);
static int _init(netdev_t *netdev);
static void _isr(netdev_t *netdev);
static int _get(netdev_t *netdev, netopt_t opt, void *val, size_t max_len);
static int _set(netdev_t *netdev, netopt_t opt, void *val, size_t len);

static const netdev_driver_t _driver = {
    .send   = _send,
    .recv   = _recv,
    .init   = _init,
    .isr    = _isr,
    .get    = _get,
    .set    = _set,
};

void netdev_owns_setup(netdev_owns_t *dev, struct ons_radio_s* radio, void *state)
{
    netdev_t *netdev = (netdev_t *)dev;

    netdev->driver = &_driver;

    dev->radio = radio;
    dev->state = state;
    
    mutex_init(&dev->mutex);
}

static int _send(netdev_t *netdev, const struct iovec *vector, unsigned count)
{
    netdev_owns_t *dev = (netdev_owns_t *)netdev;
    printf("netdev_owns: send\n");
    return ONS_radio_send(dev->radio, 0, vector->iov_base, count);
}

static int _recv(netdev_t *netdev, void *buff, size_t len, void *info)
{
    netdev_owns_t *dev = (netdev_owns_t *)netdev;
    uint16_t pkt_len;
    int res;
   
    res = ONS_radio_get_received(dev->radio, len, buff, &pkt_len);
    if (res <= 0 ){
        return res;
    }

    #ifdef MODULE_NETSTATS_L2
        netdev->stats.rx_count++;
        netdev->stats.rx_bytes += pkt_len;
    #endif
    
    if (info != NULL) {
        //netdev_ieee802154_rx_info_t *radio_info = info;
        // TODO: info from simulator
    }

    return (int)pkt_len;
}

static int _init(netdev_t *netdev)
{
    netdev_owns_t *dev = (netdev_owns_t *)netdev;
    int res = 0;        /* assume everything would be fine */

    printf("netdev_owns: init\n");

    // TODO
    (void)dev;

    return res;
}

static void _isr(netdev_t *netdev)
{
    netdev_owns_t *dev = (netdev_owns_t *)netdev;

    printf("netdev_owns: isr\n");

   // TODO?
   (void)dev;
}

static int _get(netdev_t *netdev, netopt_t opt, void *val, size_t max_len)
{
    int res;

    if (netdev == NULL) {
        return -ENODEV;
    }

    netdev_owns_t *dev = (netdev_owns_t *)netdev;

    int ext = netdev_ieee802154_get(&dev->netdev, opt, val, max_len);
    if (ext > 0) {
        return ext;
    }

    switch (opt) {

        case NETOPT_ADDRESS:
            assert(max_len >= sizeof(uint16_t));
            memcpy(val, &dev->address_short, sizeof(uint16_t));
            res = sizeof(uint16_t);
            break;

        case NETOPT_ADDRESS_LONG:
            assert(max_len >= 8);
            memcpy(val, &dev->address_long, sizeof(uint64_t));
            res = sizeof(uint64_t);
            break;

        case NETOPT_NID:
            assert(max_len >= sizeof(uint16_t));
            memcpy(val, &dev->net_id, sizeof(uint16_t));
            res = sizeof(uint16_t);
            break;

        case NETOPT_CHANNEL:
            assert(max_len >= sizeof(uint16_t));
            memcpy(val, &dev->channel, sizeof(uint16_t));
            res = sizeof(uint16_t);
            break;

/*
        case NETOPT_TX_POWER:
            assert(max_len >= sizeof(int16_t));
            return w_i16(val, cc2420_get_txpower(dev));

        case NETOPT_STATE:
            assert(max_len >= sizeof(netopt_state_t));
            *((netopt_state_t *)val) = cc2420_get_state(dev);
            return sizeof(netopt_state_t);

        case NETOPT_IS_CHANNEL_CLR:
            return opt_state(val, cc2420_cca(dev));

        case NETOPT_AUTOACK:
            return opt_state(val, (dev->options & CC2420_OPT_AUTOACK));

        case NETOPT_CSMA:
            return opt_state(val, (dev->options & CC2420_OPT_CSMA));

        case NETOPT_PRELOADING:
            return opt_state(val, (dev->options & CC2420_OPT_PRELOADING));

        case NETOPT_PROMISCUOUSMODE:
            return opt_state(val, (dev->options & CC2420_OPT_PROMISCUOUS));

        case NETOPT_RX_START_IRQ:
            return opt_state(val, (dev->options & CC2420_OPT_TELL_RX_START));

        case NETOPT_RX_END_IRQ:
            return opt_state(val, (dev->options & CC2420_OPT_TELL_TX_END));

        case NETOPT_TX_START_IRQ:
            return opt_state(val, (dev->options & CC2420_OPT_TELL_RX_START));

        case NETOPT_TX_END_IRQ:
            return opt_state(val, (dev->options & CC2420_OPT_TELL_RX_END));
*/
        default:
            res = -ENOTSUP;
            break;
    }

    return res;
}

static int _set(netdev_t *netdev, netopt_t opt, void *val, size_t val_len)
{
    int res = 0;

    if (netdev == NULL) {
        return -ENODEV;
    }

    netdev_owns_t *dev = (netdev_owns_t *)netdev;

    int ext = netdev_ieee802154_set(&dev->netdev, opt, val, val_len);

    switch (opt) {
        case NETOPT_ADDRESS:
            assert(val_len == 2);
            memcpy(&dev->address_short, val, sizeof(uint16_t));
            res = sizeof(uint16_t);

        case NETOPT_ADDRESS_LONG:
            assert(val_len == 8);
            memcpy(&dev->address_long, val, sizeof(uint64_t));
            res = sizeof(uint64_t);

        case NETOPT_NID:
            assert(val_len == sizeof(uint16_t));
            memcpy(&dev->net_id, val, sizeof(uint16_t));
            res = sizeof(uint16_t);

        case NETOPT_CHANNEL:
            assert(val_len == sizeof(uint16_t));
            memcpy(&dev->channel, val, sizeof(uint16_t));
            res = sizeof(uint16_t);
/*
        case NETOPT_TX_POWER:
            assert(val_len == sizeof(int16_t));
            cc2420_set_txpower(dev, to_i16(val));
            return sizeof(int16_t);

        case NETOPT_STATE:
            assert(val_len == sizeof(netopt_state_t));
            return cc2420_set_state(dev, *((netopt_state_t *)val));

        case NETOPT_AUTOACK:
            return cc2420_set_option(dev, CC2420_OPT_AUTOACK, to_bool(val));

        case NETOPT_CSMA:
            return cc2420_set_option(dev, CC2420_OPT_CSMA, to_bool(val));

        case NETOPT_PRELOADING:
            return cc2420_set_option(dev, CC2420_OPT_PRELOADING, to_bool(val));

        case NETOPT_PROMISCUOUSMODE:
            return cc2420_set_option(dev, CC2420_OPT_PROMISCUOUS, to_bool(val));

        case NETOPT_RX_START_IRQ:
            return cc2420_set_option(dev, CC2420_OPT_TELL_RX_START, to_bool(val));

        case NETOPT_RX_END_IRQ:
            return cc2420_set_option(dev, CC2420_OPT_TELL_RX_END, to_bool(val));

        case NETOPT_TX_START_IRQ:
            return cc2420_set_option(dev, CC2420_OPT_TELL_TX_START, to_bool(val));

        case NETOPT_TX_END_IRQ:
            return cc2420_set_option(dev, CC2420_OPT_TELL_TX_END, to_bool(val));
*/
        default:
            res = ext;
    }

    return res;
}


/** @} */
