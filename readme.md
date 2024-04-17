# Arduino Project - Auto Trashcan
Project using multiple different sensors for an Automatic trash segragation


## IR SENSOR

### setup
 - output to pin 9
 - distance up to approx. 5cm
 - output 0 if detected

## Inductive Sensor

### setup
 - output pin A1
 - inductive sensitivity = 150
 - less than that is detected
 - 
## Capacitive Sensor

### setup
 - output pin 10
 - output 1 if detected

## Ultrasonic Sensor

### setup
 - sensor 1: trigger = 6, echo = 7
 - trashcan limit if <10 cm

## LCD
 - SCL pin = A5 clock
 - SDA pin = A4 data

## Stepper motor
 - pins ln1 = 2, ln2 = 3, ln3 = 2m ln4 = 4 digital
 - using selector function
 - rollSensitivity - how much roll to next trashcan

## Regular Motor
 - for dropper
 - open and close - clockwise and counterclockwise
 - using 2 relays to reverse the polarity of motor  
