# EXT HAL driver task

The code represents configuration and empty callback of triggering the driver.  
The configuration steps:
- Set callback with palSetPadCallback() setting port, pad and callback
- Enable pad event with palEnablePadEvent() setting port, pad and event type

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
