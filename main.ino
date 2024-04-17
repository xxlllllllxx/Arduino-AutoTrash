#include "src/LiquidCrystal_I2C.h"
#include "src/Stepper.h"

// Constants
const int inductive_sensitivity = 150; // sensitivity of inductor
const int trashcan_limit = 10; // limit of cm in ultrasonic
const int sensor_delay = 100; // in miliseconds
const int stepsPerRevolution = 240; // 1 stepper revolutions
const int rollSensitivity = 20; // how many rotations to next trashcan
const int passes = 3; // verifications
const int dropper_delay = 500; // how many miliseconds does opening took

// Pin Definitions
const int infrared_sensor = 9;
const int capacitive_sensor = 10;
const int inductive_sensor = A1;
const int dropper_motor = 8;
const int dropper_motor_r = 11;
const int ultrasonic_trigger_1 = 6;
const int ultrasonic_echo_1 = 7;

// Global Variables
Stepper myStepper(stepsPerRevolution, 2, 3, 4, 5); // Stepper motor pins: IN1 = 2, IN2 = 4, IN3 = 3, IN4 = 5
LiquidCrystal_I2C lcd(0x27, 16, 2); // LCD: SCL = A5, SDA = A4

// state
bool hasTrash = false;
int pass_bin = 0;
int pass_count = 0;

// Messages
const char* messages[] = {
    "Arduino Trash                           ", // 0
    "Trashcan 1: FULL                        ", // 1
    "Trashcan 2: FULL                        ", // 2
    "Trashcan 3: FULL                        ", // 3
    "Trash Detected !                        ", // 4
    "                                        ", // 5
    "Identifying...                          ", // 6
    "Recyclable Waste                        ", // 7
    "Non Bio Waste                           ", // 8
    "Bio Waste                               ", // 9
    "Dropping...                             ", // 10
    "Verifying...                            "  // 11
};

// Function Declarations
void setup();
void loop();
void print(int l1 = 0, int l2 = 5);
void drop();
bool selector(int num);
void calculate(bool ir_flag, bool in_flag, bool ca_flag);
int check_level(int index);

void setup() {
    Serial.begin(9600);
    pinMode(infrared_sensor, INPUT);
    pinMode(ultrasonic_trigger_1, OUTPUT);
    pinMode(ultrasonic_echo_1, INPUT);
    pinMode(capacitive_sensor, INPUT);
    pinMode(dropper_motor, OUTPUT);
    pinMode(dropper_motor_r, OUTPUT);
    myStepper.setSpeed(100);
    lcd.begin();
    print();
}

void loop() {
    bool ir_flag = digitalRead(infrared_sensor) == LOW;
    bool in_flag = analogRead(inductive_sensor) < inductive_sensitivity;
    bool ca_flag = digitalRead(capacitive_sensor) == HIGH;

    if (ir_flag || in_flag || ca_flag)
        calculate(ir_flag, in_flag, ca_flag);
    else
        print();

    delay(sensor_delay);
}

void print(int l1, int l2) {
    lcd.clear();
    lcd.print(messages[l1]);
    lcd.print(messages[l2]);
    delay(sensor_delay);
}

void drop() {
    if (hasTrash) {
        digitalWrite(dropper_motor, HIGH);
        delay(dropper_delay);
        digitalWrite(dropper_motor, LOW);
        digitalWrite(dropper_motor_r, HIGH);
        delay(dropper_delay);
        digitalWrite(dropper_motor_r, LOW);
    }
}

bool selector(int num) {
    Serial.println(messages[num + 6]);
    for (int i = 0; i < rollSensitivity; i++) {
        myStepper.step((loc - num) * stepsPerRevolution);
    }
    loc = num;
    return check_level(num) > trashcan_limit;
}

void calculate(bool ir_flag, bool in_flag, bool ca_flag) {
    if (ir_flag || in_flag || ca_flag) {
        hasTrash = true;
        if (in_flag && ir_flag && !ca_flag) {
            if (pass_bin == 1)
                pass_count++;
            else
                pass_count = 0;
            pass_bin = 1;
        } else if (ir_flag && !ca_flag && !in_flag) {
            if (pass_bin == 2)
                pass_count++;
            else
                pass_count = 0;
            pass_bin = 2;
        } else if (ir_flag && ca_flag && !in_flag) {
            if (pass_bin == 3)
                pass_count++;
            else
                pass_count = 0;
            pass_bin = 3;
        }

        Serial.println("PASSES: " + String(pass_count) + " " + String(pass_bin));
        if (pass_bin != 0)
            print(11, pass_bin + 6);

        if (pass_count > passes) {
            print(4, pass_bin + 6);
            if (selector(pass_bin)) {
                pass_count = 0;
                drop();
            } else {
                selector(1);
                print(pass_bin + 6, pass_bin);
            }
            pass_bin = 0;
        }
    } else {
        hasTrash = false;
        pass_count = 0;
        pass_bin = 0;
    }
}

int check_level(int index) {
    digitalWrite(ultrasonic_trigger_1, LOW);
    delayMicroseconds(2);
    digitalWrite(ultrasonic_trigger_1, HIGH);
    delayMicroseconds(10);
    digitalWrite(ultrasonic_trigger_1, LOW);

    float distance = (pulseIn(ultrasonic_echo_1, HIGH) * 0.0343) / 2; // calibrated
    int m_flag = 5;
    if (distance < trashcan_limit)
        m_flag = index;
    print(0, m_flag);
    return distance;
}
