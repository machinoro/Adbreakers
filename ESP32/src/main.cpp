#include <Arduino.h>
#include "servo_helper.h"
#include "audio_helper.h"

void setup() {
    Serial.begin(115200); 
    setupServos();
    setupAudio();
}

void loop() {
    loopAudio(); 

    if (Serial.available() > 0) {
        char command = Serial.read();
    }
}
