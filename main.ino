#include "src/LiquidCrystal_I2C.h"

// OUTPUT VALUES:
// 0 = No Trash detected
// 1 = Trash detected

const bool debug = true;
const int output = 0;
const int moisture_sensitivity = 990; // less than this is detected
const int trashcan_limit = 10; // less than this in cm is detected
const int sensor_delay = 100;
const int passes = 10;

const int infrared_sensor = 9;
const int moisture_sensor = A0;
const int inductive_sensor = 11;
const int ultrasonic_trigger_1 = 6;
const int ultrasonic_echo_1 = 7;
const int ultrasonic_trigger_2 = 4;
const int ultrasonic_echo_2 = 5;
const int ultrasonic_trigger_3 = 2;
const int ultrasonic_echo_3 = 3;

const int stepper_1 = 13;

bool check_array[passes][3] ={};


// LCD
const int col = 16;
const int row = 2;
LiquidCrystal_I2C lcd(0x27, col, row);
// setup: 40 chars per line 16 in display

const char* messages[] = {
  "Arduino Trash                           ",//0
  "Trashcan 1: FULL                        ",//1
  "Trashcan 2: FULL                        ",//2
  "Trashcan 3: FULL                        ",//3
  "Trash Detected !                        ",//4
  "                                        ",//5
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
    Serial.println("DEBUG!!!");
}

void loop()
{
  bool ir_flag = digitalRead(infrared_sensor) == LOW;
  bool mo_flag = analogRead(moisture_sensor) < moisture_sensitivity;
  bool in_flag = digitalRead(inductive_sensor) == HIGH;
  
  if (ir_flag || mo_flag || in_flag)
    calculate(ir_flag, mo_flag, in_flag);
  else {
    delay(500);
    check_level(1, ultrasonic_trigger_1, ultrasonic_echo_1);
  // check_level(2, ultrasonic_trigger_2, ultrasonic_echo_2);
  // check_level(3, ultrasonic_trigger_3, ultrasonic_echo_3);
  }
  
  if (debug) {
    Serial.print("moisture : ");
    Serial.println(analogRead(moisture_sensor));
    Serial.print("inductive: ");
    Serial.println(digitalRead(inductive_sensor));
  }
}

void print(int l1 = 0, int l2 = 5){
  lcd.clear();
  lcd.print(messages[l1]);
  lcd.print(messages[l2]);
  delay(sensor_delay);
}

void monitor(){
  if(debug){
    //Serial.println("MONITOR START");
    //delay(5000);
  }
}

void calculate(bool ir_flag, bool mo_flag, bool in_flag)
{
  // LOGIC
  if (ir_flag || mo_flag || in_flag){
    print(4, 5);
    monitor();
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

  int m_flag = 5;
  if(distance < trashcan_limit){
    m_flag = index;
  }
  print(0, m_flag);
}
