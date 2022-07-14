ROSLIB = src/ros_lib
include $(ROSLIB)/ros.mk

# C source files
PROJECT_CSRC    = src/main.c src/serial.c
# C++ source files
PROJECT_CPPSRC 	= $(ROSSRC) src/ros.cpp
# Directories to search headers in
PROJECT_INCDIR	= $(ROSINC) include
# Additional libraries
PROJECT_LIBS	= -lm
# Compiler options
PROJECT_OPT     = -specs=nosys.specs

# Additional .mk files are included here
