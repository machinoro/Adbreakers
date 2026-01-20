#include <Adafruit_PWMServoDriver.h>
#include <Arduino.h>
#include "servo_helper.h"

Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver();

#define SERVOMIN 150
#define SERVOMAX 600
#define FREQUENCY 60

struct legIndex {
    float knee; 
    float hip; 
};

legIndex legs[4] = {{0, 3}, {4, 7}, {8, 11}, {12, 15}};

int8_t orients[4] = {-1, -1, 1, 1};


legIndex current_angles[4] = {{90,120}, {90, 120}, {90, 120}, {90, 120}};

void setupServos() {
    pwm.begin();
    pwm.setOscillatorFrequency(27000000);
    pwm.setPWMFreq(FREQUENCY);
}

uint16_t pulseWidth(float angle) {
    angle = constrain(angle, 0, 180);
    float unit_angle = (SERVOMAX - SERVOMIN) / 180.0f;
    return (uint16_t)(angle * unit_angle + SERVOMIN);
}

void dynamicServoAssignment(legIndex targets[4]) {
    legIndex starts[4];
    for(int i=0; i<4; i++) {
        starts[i] = current_angles[i];
    }

    for (int step = 0; step < 50; step++) {
        float fraction = step / 50.0;
        for (int i = 0; i < 4; i++) {
            float move_h = starts[i].hip + (targets[i].hip - starts[i].hip) * fraction;
            float move_k = starts[i].knee + (targets[i].knee - starts[i].knee) * fraction;
            
            pwm.setPWM(legs[i].hip, 0, pulseWidth(move_h));
            pwm.setPWM(legs[i].knee, 0, pulseWidth(move_k));
            
            current_angles[i] = {move_h, move_k};
        }
        delay(2); 
    }
}

void targetGait(const float gait[8], const float orders[4]) {
    for (int step = 0; step < 4; step++) {
        legIndex targets[4]; 
        
        for (int leg = 0; leg < 4; leg++) {
            int index = step * 2; 
            
            if (leg % 2) {
                index = index + 4;
            }
            
            index = index % 8;

            float raw_hip  = gait[index];     
            float raw_knee = gait[index + 1]; 

            targets[leg].hip = 90 + (raw_hip * orders[leg] * orients[leg]);
            targets[leg].knee = 120 + raw_knee;
        }
        dynamicServoAssignment(targets);
    }
}

void moveForward() {
    static const float gait[] = {-20, -15, -20, 20, 30, 20, 30, -15}; 
    static const float order[] = {1.0, 1.0, 1.0, 1.0};
    targetGait(gait, order);
}

void moveBackward() {
    static const float gait[] = {-20, -15, -20, 20, 30, 20, 30, -15}; 
    static const float order[] = {-1.0, -1.0, -1.0, -1.0};
    targetGait(gait, order);
}

void moveLeft() {
    static const float gait[] = {-20, -15, -20, 20, 30, 20, 30, -15}; 
    static const float order[] = {-1.0, 1.0, -1.0, 1.0};
    targetGait(gait, order);
}

void moveRight() {
    static const float gait[] = {-20, -15, -20, 20, 30, 20, 30, -15}; 
    static const float order[] = {1.0, -1.0, 1.0, -1.0};
    targetGait(gait, order);
}

void turnLeft() {
    static const float gait[] = {-30, -30, -30, 0, 30, 20, 30, 0};
    static const float order[] = {-1.0, -1.0, 1.0, 1.0};
    targetGait(gait, order);
}

void turnRight() {
    static const float gait[] = {-30, -30, -30, 0, 30, 20, 30, 0};
    static const float order[] = {1.0, 1.0, -1.0, -1.0};
    targetGait(gait, order);
}

void setup() {
    Serial.begin(115200);
    setupServos(); 

    for (int i = 0; i <= 3; i++) {
        pwm.setPWM(legs[i].knee, 0, pulseWidth(120)); 
        pwm.setPWM(legs[i].hip, 0, pulseWidth(90));  
    }
    delay(1000);
}

void loop() {
    moveLeft();
}
