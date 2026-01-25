#include <Arduino.h>
#include <Adafruit_PWMServoDriver.h>
#include "servo.h"
#include "esp_newlib.h"

Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver();

#define SERVOMIN 150
#define SERVOMAX 600
#define FREQUENCY 60
#define MIN_HEIGHT_CM 4.5
#define MAX_HEIGHT_CM 8.0

struct legIndex {
    float knee;
    float hip;
};

legIndex legs[4] = {
    {6, 7},    // leg 0
    {8, 9},    // leg 1
    {10, 15},   // leg 2
    {0, 5}   // leg 3
};

int8_t orients[4] = {-1, -1, 1, 1};

legIndex current_angles[4] = {
    {180, 90},
    {180, 90},
    {180, 90},
    {180, 90}
};

float current_height = 4.5, knee_angle;

uint16_t pulseWidth(float angle) {
    angle = constrain(angle, 0.0f, 180.0f);
    float unit = (SERVOMAX - SERVOMIN) / 180.0f;
    return (uint16_t)(angle * unit + SERVOMIN);
}
void dynamicServoAssignment(legIndex targets[4]) {
    legIndex starts[4];

    for (int i = 0; i < 4; i++) {
        starts[i] = current_angles[i];
    }

    for (int step = 0; step < 50; step++) {
        float t = step / 50.0f;

        for (int i = 0; i < 4; i++) {
            float hip  = starts[i].hip  + (targets[i].hip  - starts[i].hip)  * t;
            float knee = starts[i].knee + (targets[i].knee - starts[i].knee) * t;

            pwm.setPWM(legs[i].hip,  0, pulseWidth(hip));
            pwm.setPWM(legs[i].knee, 0, pulseWidth(knee));

            current_angles[i].hip  = hip;
            current_angles[i].knee = knee;
        }
        delay(1);
    }
}

void setHeight(float &height) {
    height = constrain(height, MIN_HEIGHT_CM, MAX_HEIGHT_CM);
    knee_angle = 180.0 - (height - 4.3) * (90.0 / (8.5 - 4.3));

    legIndex targets[4];
    for (int i = 0; i < 4; i++) {
        targets[i] = current_angles[i];
        targets[i].knee = knee_angle;
    }

    dynamicServoAssignment(targets);
}

void propagateGait(const float gait[8], const float turn[4]) {
    for (int step = 0; step < 4; step++) {
        legIndex targets[4];

        for (int leg = 0; leg < 4; leg++) {
            int idx = step * 2;
            if (leg % 2) idx += 4;
            idx %= 8;

            float raw_knee = gait[idx];
            float raw_hip  = gait[idx + 1];

            targets[leg].hip  = 90.0 + raw_hip * turn[leg] * orients[leg];
            targets[leg].knee = knee_angle + raw_knee;
        }

        dynamicServoAssignment(targets);
    }
}

void stableKnee() {
    legIndex targets[4];
    for(int leg = 0; leg < 4; leg++) {
        targets[leg].hip = current_angles[leg].hip;
        targets[leg].knee = knee_angle;
    }
    dynamicServoAssignment(targets);
}

void moveForward() {
    float gait[]  = {-15, -20, 20, -20, 20, 30, -15, 30};
    float order[] = {1, 1, 1, 1};
    propagateGait(gait, order);
}

void moveBack() {
    float gait[]  = {-15, -20, 20, -20, 20, 30, -15, 30};
    float order[] = {-1, -1, -1, -1};
    propagateGait(gait, order);
}

void moveLeft() {
    float gait[]  = {-15, -20, 20, -20, 20, 30, -15, 30};
    float order[] = {-1, 1, -1, 1};
    propagateGait(gait, order);
}

void moveRight() {
    float gait[]  = {-15, -20, 20, -20, 20, 30, -15, 30};
    float order[] = {1, -1, 1, -1};
    propagateGait(gait, order);
}

void turnLeft() { 
    float gait[] = {-30, -30, 0, -30, 20, 30, 0, 30}; 
    float order[] = {-1.0, -1.0, 1.0, 1.0}; 
    propagateGait(gait, order); 
} 

void turnRight() { 
    float gait[] = {-30, -30, 0, -30, 20, 30, 0, 30}; 
    float order[] = {1.0, 1.0, -1.0, -1.0}; 
    propagateGait(gait, order); 
}


void setupServos() { 
    pwm.begin();
    pwm.setPWMFreq(FREQUENCY);
    legIndex zeroPose[4] = {{180,90}, {180, 90}, {180, 90}, {180, 90}};
    dynamicServoAssignment(zeroPose);
}
