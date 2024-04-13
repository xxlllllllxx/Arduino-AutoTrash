#include "src/LiquidCrystal_I2C.h"
#include "src/Stepper.h"


// OUTPUT VALUES:
// 0 = No Trash detected
// 1 = Trash detected

const bool debug = true;
const int output = 0;
const int moisture_sensitivity = 600;
const int inductive_sensitivity = 170;
const int trashcan_limit = 10;
const int sensor_delay = 100;
const int stepsPerRevolution = 240;
const int rollSensitivity = 20;
const int passes = 3;
const int dropper_delay = 500;

const int infrared_sensor = 9;
const int moisture_sensor = A0;
const int inductive_sensor = A1;
const int capacitive_sensor = 10;
const int dropper_motor = 8;
const int dropper_motor_r = 11;
const int ultrasonic_trigger_1 = 6;
const int ultrasonic_echo_1 = 7;

Stepper myStepper(stepsPerRevolution, 2, 3, 4, 5);
int loc = 1;

int pass_bin = 0;
int pass_count = 0;
const int num_bins = 3;


// LCD
const int col = 16;
const int row = 2;
LiquidCrystal_I2C lcd(0x27, col, row);
// 4 pins SCL: A5 SDA:A4
// setup: 40 chars per line 16 in display

const char* messages[] = {
  "Arduino Trash                           ",//0
  "Trashcan 1: FULL                        ",//1 // Metallic
  "Trashcan 2: FULL                        ",//2 // Bio
  "Trashcan 3: FULL                        ",//3 // Non Bio
  "Trash Detected !                        ",//4
  "                                        ",//5
  "Identifiyng...                          ",//6
  "Metallic Waste                          ",//7
  "No Bio Waste                            ",//8
  "Bio Waste                               ",//9
  "Dropping...                             ",//10
};

void setup()
{
  Serial.begin(9600);
  pinMode(infrared_sensor, INPUT);
  pinMode(ultrasonic_trigger_1, OUTPUT);
  pinMode(ultrasonic_echo_1, INPUT);
  pinMode(capacitive_sensor, INPUT);
  pinMode(dropper_motor, OUTPUT);
  pinMode(dropper_motor_r, OUTPUT);
  myStepper.setSpeed(100); 
  lcd.begin();
  if(debug)
    Serial.println("DEBUG!!!");
}

void loop()
{
  bool ir_flag = digitalRead(infrared_sensor) == LOW;
  bool mo_flag = analogRead(moisture_sensor) < moisture_sensitivity;
  bool in_flag = analogRead(inductive_sensor) < inductive_sensitivity;
  bool ca_flag = digitalRead(capacitive_sensor) == HIGH;
  
  if (ir_flag || mo_flag || in_flag || ca_flag)
    calculate(ir_flag, mo_flag, in_flag, ca_flag);

  delay(500);
  
  if (debug) {
    Serial.print("infrared : ");
    Serial.println(digitalRead(infrared_sensor));
    Serial.print("moisture: ");
    Serial.println(analogRead(moisture_sensor));
    Serial.print("inductive: ");
    Serial.println(analogRead(inductive_sensor));
    Serial.print("capacitive: ");
    Serial.println(digitalRead(capacitive_sensor));
    Serial.print("ultrasonic 1: ");
    Serial.println(check_level(1));
  }
//  drop();
}

void print(int l1 = 0, int l2 = 5){
  lcd.clear();
  lcd.print(messages[l1]);
  lcd.print(messages[l2]);
  delay(sensor_delay);
}

void drop() {
  digitalWrite(dropper_motor, HIGH);
  delay(dropper_delay);
  digitalWrite(dropper_motor, LOW);
  digitalWrite(dropper_motor_r, HIGH);
  delay(dropper_delay);
  digitalWrite(dropper_motor_r, LOW);
}

void selector(int num){
  Serial.println(messages[num + 6]);
  for(int i = 0; i < rollSensitivity; i++){
    myStepper.step((loc - num) * stepsPerRevolution);
  }
  loc = num;
}


void calculate(bool ir_flag, bool mo_flag, bool in_flag, bool ca_flag)
{
  // LOGIC
  if (ir_flag || mo_flag || in_flag || ca_flag ){
    print(4, 6);

    
    if(in_flag && ir_flag && !mo_flag && !ca_flag){
      // Metallic Waste
      if(pass_bin == 1){
        pass_count = pass_count + 1;
      } else {
        pass_count = 0;
      }
      pass_bin = 1;
    }
    if(mo_flag && ir_flag && !ca_flag && !in_flag){
      // wet Bio Waste 
      if(pass_bin == 2){
        pass_count = pass_count + 1;
      } else {
        pass_count = 0;
      }
      pass_bin = 2;
    }
    if(ir_flag && ca_flag && !in_flag && !mo_flag){
      // not wet Non Bio waste
      if(pass_bin == 3){
        pass_count = pass_count + 1;
      } else {
        pass_count = 0;
      }
      pass_bin = 3;
    }
    delay(10);
    Serial.println("PASSES: " + String(pass_count) + " " + String(pass_bin));

    if(pass_count > passes){
      pass_count = 0;
      print(4, pass_bin + 6);
      selector(pass_bin);
      drop();
      pass_bin = 0;
    }
  }
}

//void loop_bins(){
//  for(int i = 1; i <= num_bins; i++){
//    selector(i);
//    bins[i-1] = check_level(i);
//  }
//}


int check_level(int index) {
  digitalWrite(ultrasonic_trigger_1, LOW);
  delayMicroseconds(2);
  digitalWrite(ultrasonic_trigger_1, HIGH);
  delayMicroseconds(10);
  digitalWrite(ultrasonic_trigger_1, LOW);
  
  float distance;
  // calibrated
  distance = (pulseIn(ultrasonic_echo_1, HIGH)*.0343)/2;

  int m_flag = 5;
  if(distance < trashcan_limit){
    m_flag = index;
  }
  print(0, m_flag);
  return distance;
}
