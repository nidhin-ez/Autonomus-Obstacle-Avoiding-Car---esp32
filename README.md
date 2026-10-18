# Autonomous Obstacle-Avoiding Car Using ESP32

An embedded-systems project that uses an ESP32, an ultrasonic distance sensor, and a servo motor to detect nearby obstacles and choose a direction for movement.

## Project Overview

This project demonstrates a small autonomous robot that detects obstacles and changes its movement direction based on distance measurements.

The ESP32 controls the motor driver, reads distance measurements from the ultrasonic sensor, and moves the servo to scan different directions. Based on the measured distances, the program decides whether to move forward or turn.

The project combines embedded C/C++ programming, sensor interfacing, PWM motor control, GPIO programming, and state-based control logic.

## Demonstration

The working video demonstrates the assembled car and its obstacle-avoidance behaviour.

**Demo video:** [View demo.mp4](demo.mp4)

## Components Used

| Component | Purpose |
|---|---|
| ESP32 development board | Main controller |
| HC-SR04 ultrasonic sensor | Measures distance to obstacles |
| SG90 servo motor | Rotates the ultrasonic sensor to scan directions |
| TB6612FNG motor driver | Controls the two DC motors |
| Two DC geared motors | Drive the car |
| Robot chassis and wheels | Mechanical structure |
| Resistors for voltage divider | Reduce the ultrasonic sensor's ECHO signal voltage |
| Power supply and jumper wires | Power and connect the circuit |

## Hardware Connections

### ESP32 GPIO Pin Mapping

| Component signal | ESP32 GPIO |
|---|---:|
| Servo signal | GPIO 18 |
| Ultrasonic TRIG | GPIO 5 |
| Ultrasonic ECHO | GPIO 19 |
| Motor driver STBY | GPIO 13 |
| Motor driver PWMA | GPIO 25 |
| Motor driver AIN1 | GPIO 26 |
| Motor driver AIN2 | GPIO 27 |
| Motor driver PWMB | GPIO 14 |
| Motor driver BIN1 | GPIO 32 |
| Motor driver BIN2 | GPIO 33 |

### Power and Ground

- The HC-SR04 is powered from 5 V.
- The TB6612FNG logic supply (VCC) is connected to the ESP32's 3.3 V supply.
- The motor driver's VM supply powers the motors and must match their voltage requirements.
- The ESP32, sensor, motor driver, and motor supply must share a common ground.
- The servo's power supply must meet its requirements.

**Important:** The ESP32 GPIO pins are not 5 V tolerant. The HC-SR04 ECHO output must be reduced before connecting it to GPIO 19.

## Voltage Divider for the Ultrasonic Sensor

The HC-SR04 can output a 5 V signal on its ECHO pin, while the ESP32 uses 3.3 V logic. A resistor voltage divider reduces the signal voltage to a safer level.

The divider uses two resistors connected between ECHO and ground, with the ESP32 input connected to their midpoint.

i used 2 1k ohm resistors,

So with a 5 V input and equal resistors, the midpoint voltage is approximately 2.5 V.

See the circuit diagram: [voltageDivider.jpeg](voltageDivider.jpeg)

## How the Robot Works

### 1. Distance Measurement

The ESP32 sends a short trigger pulse to the HC-SR04. The sensor emits an ultrasonic burst and returns an ECHO pulse whose duration depends on the distance to the object.

The program measures the ECHO pulse duration and converts it into an approximate distance in centimetres.

### 2. Servo-Based Scanning

The SG90 servo changes the direction in which the ultrasonic sensor faces. The program scans the left, right, and centre directions to gather distance readings.

### 3. Obstacle Detection

The program uses distance thresholds to decide whether the forward path is clear.

- The movement safety check stops forward movement when a valid distance reading is below 20 cm.
- The direction-selection logic uses a 25 cm threshold when evaluating the scanned directions.

These thresholds serve different purposes: one is used during forward movement, and the other is used when selecting a direction.

### 4. Direction Selection

After scanning, the program compares the measured distances and selects a movement action based on its decision logic. It can choose to move forward, turn left, turn right, or stop.

This is a rule-based obstacle-avoidance method. It responds to local distance measurements rather than building a map of the environment.

### 5. Motor Control

The TB6612FNG motor driver receives control signals from the ESP32 and drives the two DC motors.

The direction pins determine each motor's rotation direction, while PWM signals control the motor drive level. By controlling both motors, the ESP32 can move the car forward, turn it, or stop it.

### 6. State-Based Program Flow

The program uses states to organise the scanning and movement sequence. These states separate actions such as returning the sensor to the centre, scanning each side, waiting for the servo to move, reading distances, deciding on a direction, and executing the selected movement.

This structure makes the sequence easier to organise than placing every action into one long block of code.

## Software

- **Microcontroller:** ESP32
- **Programming environment:** Arduino framework
- **Language:** C/C++
- **Sensor interface:** GPIO timing and pulse measurement
- **Motor control:** Digital GPIO and PWM
- **Control approach:** State-based logic

## Source Code

The complete source code is available in [`code.ino`](code.ino).

## Project Media

- **Car photograph:** See the uploaded robot image in this repository.
- **Voltage-divider diagram:** [voltageDivider.jpeg](voltageDivider.jpeg)
- **Working demonstration:** [demo.mp4](demo.mp4)

## Learning Outcomes

Through this project, I explored:

- Interfacing an ultrasonic sensor with an ESP32.
- Using a servo motor to scan different directions.
- Controlling DC motors through a motor driver.
- Using PWM for motor control.
- Implementing state-based program logic.
- Applying distance thresholds to make movement decisions.
- Using a voltage divider to interface a 5 V sensor signal with a 3.3 V microcontroller input.

## Author

**Nidhin**  
First-year Electronics and Communication Engineering student

[GitHub Profile](https://github.com/nidhin-ez)
