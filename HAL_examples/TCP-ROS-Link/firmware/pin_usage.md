# Controller periphery usage
 
## NOTE!!!
1) PE6 do not connected to board, PH1 ??
2) PC11, PC10 do not working when try to pull down    


## Throttle position (DAC)
Driver | Pins | Input / Output | Description
-------|------|----------------|----------
SPI1   | PA5  |                | clock
SPI1   | PA6  |                | MISO
SPI1   | PA7  |                | MOSI
PAL    | PD14 |                | CS


## CAN 
Driver | Pins | Input / Output | Description
-------|------|----------------|----------
CAN1   | PD1  |                |CAN RX
CAN1   | PD0  |                |CAN TX


## Clutch pedal actuator control unit 
Driver | Pins | Input / Output | Description
-------|------|----------------|----------
PWM3   | PC6  | Output         |   
PAL    | PB8  | Output         | Direction


## Brake pedal actuator control unit 
Driver | Pins | Input / Output  | Description
-------|------|-----------------|------------
PWM4   | PD12 | Output          |   
PAL    | PD11 | Output          | Direction


## Steer (Step Motor) actuator control unit 
Driver | Pins | Input / Output | Description
-------|------|----------------|----------
PWM12  | PB14 | Output         | 
PAL    | PF2  | Output         | Direction


## Transmission control unit 
Driver | Pins | Input / Output | Description
-------|------|----------------|------------
PWM11  | PF7  | Output         | vertical
PWM14  | PF9  | Output         | horizontal
PAL    | PG1  | Output         | vertical Direction
PAL    | PE3  | Output         | horizontal Direction

## Transmission calibration sensors 
Driver | Pins | Input / Output | Description
-------|------|----------------|------------
EXT1   |PG2   | Input          | horizontal axis right sensor
EXT1   |PG3   | Input          | horizontal axis left sensor
EXT1   |PC11  | Input          | vertical   axis upper sensor
EXT1   |PC12  | Input          | vertical   axis lower sensor   


## Absolute encoder (steer unit)
Driver | Pins | Input / Output | Description
-------|------|----------------|----------
SPI2   | PB13 |                | clock
SPI2   | PC2  |                | MISO
SPI2   | PB15 |                | MOSI
PAL    | PB12 |                | CS


## Steer motor driver
Driver | Pins | Input / Output | Description
-------|------|----------------|----------
SPI4   | PE2  |                | clock
SPI4   | PE5  |                | MISO
SPI4   | PE6  |                | MOSI //??????
PAL    | PD3  |                | CS
PAL    | PD4  |                | Start/Stop
PAL    | PD5  |                | Dir left/Dir right



## Emergency stop button
Driver | Pins | Input / Output | Description
-------|------|----------------|----------
EXT1   | PA0  |                | interrupt

## Engine Ignition control
Driver | Pins | Input / Output | Description
-------|------|----------------|----------
PAL    | PF12 |   Output       | Ignition
PAL    | PF13 |   Output       | Starter


## Avto Signal (bi-bi)
Driver | Pins | Input / Output | Description
-------|------|----------------|----------
PAL    | PB2  |   Output       | 



