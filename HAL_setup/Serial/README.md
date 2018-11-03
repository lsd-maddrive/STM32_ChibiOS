# Serial HAL driver task

The code represents configuration and different variants to work with serial.  
The configuration steps are:
- <halconf.h> / <mcuconf.h> - in the code (lines: 7-10)
- Configuration structure
- Start driver
- Setup pins for alternate function

The tasks are:
1) Create clean code (you dont need all of the variant how to use serial) for sending your name through serial.
2) Send table of sine/cosine function through serial to MATLAB and plot it.
3) Send to the uC (microcontroller) the input of the system, calculate equation and send state back to MATLAB. Start with polynomial, end with differential equation and state vector.
