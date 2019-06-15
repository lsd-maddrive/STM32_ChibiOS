
# C source files
PROJECT_CSRC 	= main.c web/web.c $(CHIBIOS)/os/various/evtimer.c
# C++ source files
PROJECT_CPPSRC 	= 
# Directories to search headers in
PROJECT_INCDIR	= 
# Additional libraries
PROJECT_LIBS	=
# Compiler options
PROJECT_OPT     =

# Additional .mk files are included here
include $(CHIBIOS)/os/hal/lib/streams/streams.mk
include $(CHIBIOS)/os/various/lwip_bindings/lwip.mk
