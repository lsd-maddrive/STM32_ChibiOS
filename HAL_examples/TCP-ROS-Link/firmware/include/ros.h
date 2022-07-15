#ifndef ROS_H
#define ROS_H

#include <ros/node_handle.h>
#include <ros_socket_hardware.h>

#ifdef __cplusplus
extern "C" {
#endif

namespace ros
{
    // Max publishers, max subscribers, input size, output size
    typedef NodeHandle_<RosSocketHardware, 10, 10, 512, 512> NodeHandle;
}

#ifdef __cplusplus
}
#endif

#endif /* ROS_H */
