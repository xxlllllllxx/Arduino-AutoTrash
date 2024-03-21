# Arduino Project - Auto Trashcan
Project using multiple different sensors for an Automatic trash segragation


## IR SENSOR

### setup
 - output to pin 9
 - distance up to approx. 5cm
 - output 0 if detected
 
## MOISTURE SENSOR

### setup
 - output to pin A0
 - moist sensitivity, no value == +1000
 - output is >1000 for no touch, <1000 for moist
 - not yet fully calibrated

## Inductive Sensor

### setup
 - output pin 10
 - not yet finished

## Ultrasonic Sensor

### setup
 - sensor 1: trigger = 6, echo = 7
 - trashcan limit if <10 cm

## LCD
 - SCL pin = A5 clock
 - SDA pin = A4 data

### Possible updates
 - Add trashcan dashboard for trash content: 
 - if trashcan is 1000cm  content = 1000 - distance
 - dashboard "[ 1: 100cm, 2: 100cm, 3: 100cm ]" 
