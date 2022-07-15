#include <ros_proto.h>
#include <ros.h>
#include <ros/node_handle.h>

#include "serial.h"

/**************/
/* ROS things */
/**************/

#include <std_msgs/Int8.h>
#include <std_msgs/Float32.h>


#define THREAD_SLEEP_TIME_MS 30

/* Useful for watchdog */

// static virtual_timer_t  watchdog_vt;
// static void watchdog_cb(void *arg)
// {
//     (void)arg;
//     steerExtTask = speedExtTask = 0;
// }
// chVTSet( &watchdog_vt, MS2ST( CONTROL_SET_TIMEOUT_MS ), watchdog_cb, NULL );

/* =================== */

ros::NodeHandle ros_node;

void in_data_cb(const std_msgs::Int8 &msg)
{
    comm_dbgprintf_info("New in data: %d", msg.data);
}

ros::Subscriber<std_msgs::Int8> topic_in_data("in_data", &in_data_cb);
std_msgs::Float32 outMsg;
ros::Publisher topic_out_data("out_data", &outMsg);
//=======================================================

float st2s = 1.0 / CH_CFG_ST_FREQUENCY;
bool initialized = false;

/* Function to set timestamp to messages like this */
/* Header: http://docs.ros.org/en/noetic/api/std_msgs/html/msg/Header.html */
static void setTime(ros::Time &stamp)
{
    RosSocketHardware* hardware = ros_node.getHardware();

    float curr_time_ms = hardware->time();
    float seconds;
    float fract_part = modf(curr_time_ms/1000, &seconds);

    stamp.sec = seconds;
    stamp.nsec = fract_part * 1e9;
}

void rosSendData(float data)
{
    if (!initialized) {
        return;
    }

    outMsg.data = data;

    topic_out_data.publish(&outMsg);
    comm_dbgprintf_info("Send data: %d", (int)outMsg.data);
}

/*
 * ROS spin thread - used to receive messages
 */

static THD_WORKING_AREA(waSpinner, 1024);
static THD_FUNCTION(Spinner, arg)
{
    (void)arg;
    chRegSetThreadName("ROS Spinner");

    initialized = true;

    comm_dbgprintf_info("Start spinning");

    while (true)
    {
        systime_t time = chVTGetSystemTimeX();

        int spin_status = ros_node.spinOnce();
        if (0 != spin_status ) {
            comm_dbgprintf_error("Spin failed: %d", spin_status);
        }

        chThdSleepUntilWindowed( time, time + TIME_MS2I( THREAD_SLEEP_TIME_MS ) );
    }
}

void rosInit(tprio_t prio)
{
    /* ROS setup */
    ros_node.initNode();
    /* NOTE - better set about 20-50 */
    ros_node.setSpinTimeout(200);

    /* ROS publishers */
    ros_node.advertise(topic_out_data);

    /* ROS subscribers */
    ros_node.subscribe(topic_in_data);
    /* ROS service client */

    /* Main ROS thread */
    chThdCreateStatic(waSpinner, sizeof(waSpinner), prio, Spinner, NULL);
}
