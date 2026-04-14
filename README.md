# Escape Room Game using ESP32

## Overview
A multi-level escape room system built using ESP32 that integrates hardware components and embedded programming to create an interactive puzzle-solving experience.

## Features
- Keypad-based password authentication
- Magnetic puzzle using reed switches
- Tilt detection using MPU6050 sensor
- Real-time timer with dynamic difficulty
- Score tracking system
- Servo motor-based locking mechanism

## Tech Stack
- ESP32
- Arduino IDE (Embedded C)
- MPU6050 Sensor
- Reed Switches
- Servo Motors
- LCD Display (I2C)

## Game Flow
Level 1: Enter correct password using keypad  
Level 2: Identify correct magnetic position  
Level 3: Tilt device to unlock final stage  

## File Structure
Escape-Room-ESP32/
  EscapeRoom.ino

## Setup Instructions
1. Connect all components as per circuit design  
2. Upload the code using Arduino IDE  
3. Power the ESP32  
4. Follow on-screen instructions on LCD

## Hardware Design Note
An external Arduino board was used to provide stable power to the servo motors. This prevents overloading of the ESP32 and ensures reliable operation of actuators during gameplay.

## Author
Vedika Thorat
