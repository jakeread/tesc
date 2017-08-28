# FOCT v0.3
### Field Oriented Control on a Teensy 3.2

![FOCT System](https://raw.githubusercontent.com/jakeread/tesc/master/Documentation/foct-test-v0-1.png)

## Briefly
FOCT does closed-loop control of brushless dc (BLDC) motors: the type normally found in RC Cars, Airplanes, Drones etc. These motors are incredibly power-dense, making them an excellent candidate for Robotics applications. However, it's very difficult to control them at low speeds and in small position intervals. 

FOCT implements 'all the bells and whistles' found in high-end robotics motor controllers (encoder feedback, BEMF & current sensing, etc) in a relatively low-cost solution. Code is written in Arduino for the Teensy Microcontroller, and is meant to be easily extensible and pick-through-able.

FOCT is in development! So is it's documentation!

## The Hardware  
FOCT uses a Teensy 3.2 to control Three H-Bridges, which each drive one phase of a three-phase brushless motor. The H-Bridge gates are driven with a DRV8302, a wunderkid IC that is also home to a 5V Buck Converter (for your peripheral needs) and two differential Op-Amps, used to measure phase current. The board also has simple voltage-dividers connected to Teensy ADCs for measuring phase voltage, and includes an SPI out connector used to interface with an AS5047 encoder for absolute position sensing. For now, input is done over Teensy's blazing-fast serial port.  
  
Up to ~24v supply  
Up to 30A without a heatsink, 42 with good cooling, peaks of 60  
Planned PWM in, SPI slave modes for control. Serial over USB Config / testing.  

![FOCT v0.2 Hardware](https://raw.githubusercontent.com/jakeread/tesc/master/Documentation/foct-board-v0-2.png)

![FOCT v0.3 Schematic](https://raw.githubusercontent.com/jakeread/tesc/master/Documentation/foct-schematic-v0-3.png)

## The Software
FOCT runs on Arduino! It uses Teensy's built interval timers to handle fast-loop-insurance, and only occasionally reaches into the chip's registers to make PWM work properly. Code will be well commented, I hope / promise...

## How it Works: Closed-Loop BLDC Control
// 6-step commutation w/ position reference

## How it Works: Field-Oriented Control
// FOC ! Transforms, current measurement, UVW || AB || IQ frames  

## TBD's, software
- Proper filtering on Current measurement, encoder measurement  
- Tuning the FOC loop  
- Encoder Offset Discovery routine  
- Auto Tuning, interface, system integration, oh my

## TBD's, hardware
- ADC voltage divider to sense VBUS voltage
- More DAC outputs / testing pins so to tune FOC
- SMD Thermistor near the FETS ?  
- Go to STM32F4 & chibios ?
- Caps could be closer to FETS, heatsink strap 
