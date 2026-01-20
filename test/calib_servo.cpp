#include <Adafruit_PWMServoDriver.h>
#include <Arduino.h>
#include <math.h>

Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver(0x40);

#define SERVOMIN 100
#define SERVOMAX 600
#define FREQUENCY 60

int chosenChannel;
int centerOffsetAngleTest;

int pulseWidth(byte angle) {
    float unit_angle = float((SERVOMAX - SERVOMIN) * 0.005555);
    return angle * unit_angle + SERVOMIN;
}

void printState() {
    Serial.print("Current Channel: ");
    Serial.print(chosenChannel);
    Serial.print("       Current Offset: ");
    Serial.println(centerOffsetAngleTest);
}

void adjustOffset(int delta) {
    centerOffsetAngleTest = constrain(centerOffsetAngleTest + delta, 0, 180);
    printState();
}

void changeChannel(int delta) {
    chosenChannel = constrain(chosenChannel + delta, 0, 15);
    centerOffsetAngleTest = 90;
    for (int i = 90; i > 80; i--) {
        pwm.setPWM(chosenChannel, 0, pulseWidth(i));
        delay(10);
    }
    for (int i = 80; i < 100; i++) {
        pwm.setPWM(chosenChannel, 0, pulseWidth(i));
        delay(10);
    }
    for (int i = 100; i > 90; i--) {
        pwm.setPWM(chosenChannel, 0, pulseWidth(i));
        delay(10);
    }
    printState();
}

void setup() {
    Serial.begin(115200);
    pwm.begin();
    pwm.setPWMFreq(FREQUENCY);
    delay(10);

    for (int i = 0; i < 12; i++)
        pwm.setPWM(i, 0, pulseWidth(90));
    delay(2000);

    chosenChannel = 0;
    centerOffsetAngleTest = 90;
}

void loop() {
    int data = 4;
    if (Serial.available() > 0)
        data = Serial.read() - '0';

    switch (data) {
    case 0:
        adjustOffset(-1);
        break;
    case 1:
        adjustOffset(1);
        break;
    case 2:
        adjustOffset(-10);
        break;
    case 9:
        adjustOffset(10);
        break;
    case 3:
        changeChannel(-1);
        break;
    case 8:
        changeChannel(1);
        break;
    default:
        break;
    }

    pwm.setPWM(chosenChannel, 0, pulseWidth(centerOffsetAngleTest));
}
