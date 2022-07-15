#include "ch.h"
#include "hal.h"
#include <serial.h>

#include <lwipthread.h>
#include <lwip/netif.h>
#include <lwip/api.h>

#include "ros_proto.h"

int main(void)
{
    chSysInit();
    halInit();

    /* Init LWIP */
    lwipthread_opts_t opts;
    struct ip4_addr ip, gateway, netmask;
    IP4_ADDR(&ip, 9, 9, 9, 2);
    IP4_ADDR(&gateway, 9, 9, 9, 1);
    IP4_ADDR(&netmask, 255, 255, 255, 0);
    uint8_t macaddr[6] = {0xC2, 0xAF, 0x51, 0x05, 0xCF, 0x46};

    opts.address = ip.addr;
    opts.gateway = gateway.addr;
    opts.netmask = netmask.addr;
    opts.macaddress = macaddr;
    opts.link_up_cb = NULL;
    opts.link_down_cb = NULL;

    // BaseChannel *dbg_chn = comm_get_channel();
    lwipInit(&opts);

    /* For USB debug */
    debug_stream_init();

    rosInit(NORMALPRIO);

    int32_t counter = 0;

    while (1)
    {
        rosSendData(counter++);
        palToggleLine(LINE_LED2);
        chThdSleepMilliseconds(1000);
    }
}
