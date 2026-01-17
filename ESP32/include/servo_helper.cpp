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
legIndex current_angles[4] = {{180,90}, {180, 90}, {180, 90}, {180, 90}};
float current_height = 4.3;

void setupServos() {
    pwm.begin();
    pwm.setOscillatorFrequency(27000000);
    pwm.setPWMFreq(FREQUENCY);
}

uint16_t pulseWidth(int angle) {
    angle = constrain(angle, 0, 180);
    float unit_angle = (SERVOMAX - SERVOMIN) / 180.0f;
    return (uint16_t)(angle * unit_angle + SERVOMIN);
}

void dynamicServoAssignment(legIndex targets[4]) {
    legIndex starts[4];
    for(int i=0; i<4; i++) {
        starts[i].hip = current_angles[i].hip;
        starts[i].knee = current_angles[i].knee;
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

void propagateGait(const float gait[8], const float orders[4]) {
    for (int step = 0; step < 4; step++) {
        legIndex targets[4]; 
        for (int leg = 0; leg < 4; leg++) {
            int index = step * 2; 
            if (leg % 2) {
                index = index + 4;
            }
            index = index % 8;
            float raw_knee = gait[index];     
            float raw_hip  = gait[index + 1]; 

            targets[leg].hip = 90 + (raw_hip * orders[leg] * orients[leg]);
            targets[leg].knee = 120 + raw_knee;
        }
        dynamicServoAssignment(targets);
    }
}

void setHeight(float height) {
    // 180: lowest - 90: Highest
    height = constrain(height, 4.3, 8.3);
    float corres_angle = 180.0 - (height - 4.3)*(180.0-90.0)/(8.5-4.3);
    legIndex targets[4];
    for(int i = 0; i <= 3; i++) {
        targets[i] = current_angles[i];
        targets[i].knee = corres_angle;
    }
    dynamicServoAssignment(targets);
}

void moveForward() {
    static const float gait[] = {-15, -20, 20, -20, 20, 30, -15, 30}; 
    static const float order[] = {1.0, 1.0, 1.0, 1.0};
    propagateGait(gait, order);
}
void moveRight() {
    static const float gait[] = {-15, -20, 20, -20, 20, 30, -15, 30}; 
    static const float order[] = {-1.0, 1.0, -1.0, 1.0}; 
    propagateGait(gait, order);
}

void moveLeft() {
    static const float gait[] = {-15, -20, 20, -20, 20, 30, -15, 30}; 
    static const float order[] = {1.0, -1.0, 1.0, -1.0};
    propagateGait(gait, order);
}

void moveBack() {
    static const float gait[] = {-15, -20, 20, -20, 20, 30, -15, 30}; 
    static const float order[] = {-1.0, -1.0, -1.0, -1.0};
    propagateGait(gait, order);
}
void turnLeft() {
    static const float gait[] = {-30, -30, 0, -30, 20, 30, 0, 30};
    static const float order[] = {-1.0, -1.0, 1.0, 1.0};
    propagateGait(gait, order);
}

void turnRight() {
    static const float gait[] = {-30, -30, 0, -30, 20, 30, 0, 30};
    static const float order[] = {1.0, 1.0, -1.0, -1.0};
    propagateGait(gait, order);
}

