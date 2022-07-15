#pragma once

#include "ch.h"
#include "hal.h"

#ifdef __cplusplus
extern "C" {
#endif

/*** ROS prototypes ***/

/**
 * @brief   Init ROS parameters
 * @param   prio    Priority of thread
 */
void rosInit( tprio_t prio );

void rosSendData( float data );

#ifdef __cplusplus
}
#endif
