#ifndef ROS_SOCKET_HARDWARE_H
#define ROS_SOCKET_HARDWARE_H

#ifdef __cplusplus
extern "C"
{
#endif

#include <math.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <lwip/opt.h>
#include <lwip/tcpip.h>
#include <netif/etharp.h>
#include <lwip/tcp.h>
#include <lwip/arch.h>
#include <lwip/api.h>

#include "serial.h"

// #define CONNECTION_PORT 80
#define SERVER_PORT 23456
#define SET_SERVER_ADDR(x) IP4_ADDR(x, 9, 9, 9, 1);

#define RECEIVE_TIMEOUT 100

    class RosSocketHardware
    {
    private:
        struct netconn *tcpConnection = NULL;
        err_t error;
        ip_addr_t ServerIPaddr;
        uint32_t startTime;
        float st2ms;
        uint8_t reconnectFlag = 0;

    public:
        RosSocketHardware()
        {
            // Constructor
            SET_SERVER_ADDR(&ServerIPaddr);
            startTime = current_time();
        }

        void init()
        {
            if (tcpConnection != NULL)
            {
                comm_dbgprintf_info("Delete TCP connection...");
                netconn_delete(tcpConnection);
            }

            tcpConnection = netconn_new(NETCONN_TCP);
            if (tcpConnection == NULL)
            {
                chSysHalt("Failed to create *netconn");
            }

            tcpConnection->recv_timeout = RECEIVE_TIMEOUT;

            comm_dbgprintf_info("Binded, TCP connection start");

            error = netconn_connect(tcpConnection, &ServerIPaddr, SERVER_PORT);
            if (error != ERR_OK)
            {
                comm_dbgprintf_error("Server not connected: %d", error);
                reconnectFlag = 1;
                return;
            }

            comm_dbgprintf_info("Connected");
            reconnectFlag = 0;
            st2ms = 1000.0 / CH_CFG_ST_FREQUENCY;
        }

        int read()
        {
            if (reconnectFlag == 1)
            {
                init();
                return -1;
            }

            err_t receiveError;
            uint8_t retBuff;
            static struct netbuf *inbuf;
            static uint8_t *buf;
            static uint16_t buflen = 0;
            static uint16_t readStep = 0;
            static uint8_t nextBuffFlag = 0;
            if (readStep == 0)
            {
                if (nextBuffFlag == 0)
                {
                    receiveError = netconn_recv(tcpConnection, &inbuf);
                }
                if (receiveError == ERR_OK || nextBuffFlag == 1)
                {
                    nextBuffFlag = 0;
                    netbuf_data(inbuf, (void **)&buf, &buflen);
                }
                else
                {
                    /* ERR_RST ~ -11, ERR_CLSD ~ -12, ERR_CONN ~ -13 */
                    if (receiveError <= ERR_RST)
                    {
                        reconnectFlag = 1;
                        comm_dbgprintf_error("Start reconnection: %d", receiveError);
                    }
                    return -1;
                }
            }
            retBuff = buf[readStep];
            readStep++;
            if (readStep == (buflen))
            {
                readStep = 0;
                int hasNext = netbuf_next(inbuf); 
                if (hasNext < 0)
                    netbuf_delete(inbuf);
                else
                    nextBuffFlag = 1;
            }

            return retBuff;
        }

        void write(uint8_t *data, int length)
        {
            err_t sentError;
            sentError = netconn_write(tcpConnection, data, length, NETCONN_COPY);
            error = sentError * -1;
        }

        uint32_t current_time()
        {
            /* TODO --- this function may be critical as uses floats */
            /* replaces ST2MS as it overflows on (2^32 / 1000) */
            return ceilf(chVTGetSystemTimeX() * st2ms);
            // return ST2MS(chVTGetSystemTimeX());
        }

        uint32_t time()
        {
            uint32_t c_time = current_time() - startTime;
            return c_time;
        }
    };

#ifdef __cplusplus
}
#endif

#endif /* ROS_SOCKET_HARDWARE_H */
