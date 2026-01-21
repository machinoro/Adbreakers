#ifndef SERVO_HELPER_H
#define SERVO_HELPER_H

extern float current_height;
void setupServos();
void moveForward();
void moveBack();
void moveLeft();
void moveRight();
void turnLeft();
void turnRight();
void setHeight(float &height);
void stableKnee();

#endif
