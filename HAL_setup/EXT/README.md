# EXT HAL driver task

The code represents configuration and empty callback of triggering the driver.  
The configuration steps:
- Setup HAL_USE_EXT in <halconf.h> (Nothing to do in <mcuconf.h>)
- Create structure of EXTConfig type that is configuration of channels
  There are EXT_MAX_CHANNELS channels (for us it is 24) to setup
- Fill the fields of the config (2 ways: static const or during indexing (no const))
  Index of <channels> array is port number, port name setup is realized with mask like EXT_MODE_GPIO*
  The code represents config for PD_0 and PB_3 triggering, callback is empty
- Start the driver with extStart()

The tasks are:
1) Change code to trigger the blue button and toggle LED when button is pressed. The task is basic and requires just reconfiguration and small code changes.
You can find pin port and number with https://os.mbed.com/platforms/ST-Nucleo-F767ZI/ or with macro LINE_BUTTON. The code must trigger on button push and toggle one of leds (1, 2 or 3).
2) Avoid noize of previous task, make is stable with software. Hint: try to control LED in thread, make callback just notifying the thread when user clicked the button.
3) Technical requirements:
- System must switch to "enabled" state after button pressed
- If there is no input for 5 seconds, the system must switch to "disabled" state
- Enabled state - green LED is turned on, blue flashes with frequency 1 Hz
- Disabled state - green LED is turned off, blue LED notifies user with 4 Hz flash
- Double button press (≤ 1 second interval) toggles red LED
- Double press must be allowed only once per "enabled" state, "disabled" state turns off red LED and resets double press acceptance
