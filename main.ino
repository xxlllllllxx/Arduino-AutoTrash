#include "src/LiquidCrystal_I2C.h"

// OUTPUT VALUES:
// 0 = No Trash detected
// 1 = Trash detected

const bool debug = true;
const int output = 0;
const int moisture_sensitivity = 990; // less than this is detected
const int trashcan_limit = 10; // less than this in cm is detected

const int infrared_sensor = 9;
const int moisture_sensor = A0;
const int inductive_sensor = 10;
const int ultrasonic_trigger_1 = 6;
const int ultrasonic_echo_1 = 7;

const int stepper_1 = 13;


// LCD
LiquidCrystal_I2C lcd(0x27, 16, 2);
const char* messages[] = {
  "Arduino Trash",
  "Trashcan 1: FULL",
  "Trashcan 2: FULL",
  "Trashcan 3: FULL"
};




void setup()
{
  Serial.begin(9600);
  pinMode(infrared_sensor, INPUT);
  pinMode(inductive_sensor, INPUT);
  pinMode(ultrasonic_trigger_1, OUTPUT);
  pinMode(ultrasonic_echo_1, INPUT);
  pinMode(stepper_1, INPUT);
  lcd.begin();
  if(debug)
    Serial.print("DEBUG!!!");
}

void loop()
{
  bool ir_flag = digitalRead(infrared_sensor) == 0;
  bool mo_flag = analogRead(moisture_sensor) < moisture_sensitivity;
  bool in_flag = digitalRead(inductive_sensor) == 1 && false;

  
  check_level(1, ultrasonic_trigger_1, ultrasonic_echo_1);

  
  
  
  if (ir_flag || mo_flag || in_flag)
    calculate(ir_flag, mo_flag, in_flag);
    
  if (debug) {
    Serial.print("moisture : ");
    Serial.println(analogRead(moisture_sensor));
    Serial.print("inductive: ");
    Serial.println(digitalRead(inductive_sensor));
  }
}

void calculate(bool ir_flag, bool mo_flag, bool in_flag)
{
  // LOGIG
  if (ir_flag || mo_flag || in_flag){
    lcd.clear();
    lcd.print("Trash Detected !");
    delay(1000);
  } else {
    lcd.clear();
  }
}

void check_level(int index,int trigPin, int echoPin) {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  
  float distance;
  // calibrated
  distance = (pulseIn(echoPin, HIGH)*.0343)/2;
  
  if(distance < trashcan_limit){
    lcd.clear();
    lcd.print(messages[index]);
    delay(1000);
  } else {
    lcd.clear();
    lcd.print(messages[0]);
  }
}
