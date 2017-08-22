# FOCT v0.3
### Field Oriented Control on a Teensy 3.2

[FOCT v0.2 Hardware](https://github/com/jakeread/tesc/Documentation/foct-board-v0-2.png)

## Briefly
FOCT does closed-loop control of brushless dc (BLDC) motors: the type normally found in RC Cars, Airplanes, Drones etc. These motors are incredibly power-dense, making them an excellent candidate for Robotics applications. However, it's very difficult to control them at low speeds, and control them reliably at high speeds.  
FOCT implements 'all the bells and whistles' found in high-end robotics motor controllers (encoder feedback, BEMF & current sensing, etc) in a relatively low-cost solution. Code is written in Arduino for the Teensy Microcontroller, and is meant to be easily extensible and pick-through-able.
FOCT is in development! So is it's documentation!

## The Hardware  
// link to hw .md

## The Software
// ibid, sw  
// flochart -> inputs ->

## How it Works: Closed-Loop BLDC Control
// 6-step commutation w/ position reference

## How it Works: Field-Oriented Control
// FOC ! Transforms, current measurement, UVW || AB || IQ frames  
