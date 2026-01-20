#include "esp32-hal.h"
#include <Adafruit_PWMServoDriver.h>
#include <Arduino.h>
#include <math.h>

Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver();

#define SERVOMIN 100
#define SERVOMAX 600
#define FREQUENCY 60

struct legIndex {
    uint8_t knee;
    uint8_t hip;
};

legIndex legs[4] = {{0, 3}, {4, 7}, {8, 11}, {12, 15}};

uint16_t pulseWidth(int angle) {
    angle = constrain(angle, 0, 180);
    float unit_angle = (SERVOMAX - SERVOMIN) / 180.0f;
    return (uint16_t)(angle * unit_angle + SERVOMIN);
}

void moveTwoLegs(legIndex legA, int hA_Start, int hA_End, int kA_Start, int kA_End, legIndex legB, int hB_Start, int hB_End, int kB_Start, int kB_End, int stepDelayMs) {
    int hA_Delta = hA_End - hA_Start;
    int kA_Delta = kA_End - kA_Start;
    int hB_Delta = hB_End - hB_Start;
    int kB_Delta = kB_End - kB_Start;

    int steps = max(abs(hA_Delta), abs(kA_Delta));
    steps = max(steps, abs(hB_Delta));
    steps = max(steps, abs(kB_Delta));

    if (steps == 0)
        return;

    for (int i = 0; i <= steps; i++) {
        float progress = (float)i / steps;

        int current_hA = hA_Start + (hA_Delta * progress);
        int current_kA = kA_Start + (kA_Delta * progress);

        int current_hB = hB_Start + (hB_Delta * progress);
        int current_kB = kB_Start + (kB_Delta * progress);

        pwm.setPWM(legA.hip, 0, pulseWidth(current_hA));
        pwm.setPWM(legA.knee, 0, pulseWidth(current_kA));

        pwm.setPWM(legB.hip, 0, pulseWidth(current_hB));
        pwm.setPWM(legB.knee, 0, pulseWidth(current_kB));

        delay(stepDelayMs);
    }
}

void setup() {
    Serial.begin(115200);
    pwm.begin();
    pwm.setPWMFreq(FREQUENCY);

    for (int i = 0; i <= 3; i++) {
        pwm.setPWM(legs[i].knee, 0, pulseWidth(180));
        pwm.setPWM(legs[i].hip, 0, pulseWidth(90));
    }
    delay(3000);
}

void loop() {
    moveTwoLegs(legs[1], 90, 120, 180, 120, legs[3], 90, 120, 180, 120, 2);
    delay(100);
    // moveTwoLegs(legs[1], 120, 50, 120, 180, legs[3], 120, 50, 120, 180, 2);
    // delay(100);
    // pwm.setPWM(legs[3].knee, 0, pulseWidth(120));
    // delay(200);
    // moveTwoLegs(legs[0], 90, 50, 180, 160, legs[2], 90, 50, 180, 160, 2);
    // delay(100);
    // moveTwoLegs(legs[0], 50, 120, 160, 180, legs[2], 50, 120, 160, 180, 2);
    // delay(100);
}
