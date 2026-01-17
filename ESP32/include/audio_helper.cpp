#include "audio_helper.h"

#include "Audio.h"
#include "SD.h"
#include "FS.h"
#include <vector>

#define SD_CS          5
#define SPI_MOSI       23
#define SPI_MISO       19
#define SPI_SCK        18
#define DAC_PIN        25 

Audio audio;
bool isPaused = false;

void setupAudio() {
    pinMode(SD_CS, OUTPUT);
    digitalWrite(SD_CS, HIGH);
    SPI.begin(SPI_SCK, SPI_MISO, SPI_MOSI);
    
    if (!SD.begin(SD_CS)) {
        Serial.println("Error: SD Card Mount Failed");
        return;
    }
    
    audio.setPinout(0, 0, DAC_PIN);
    audio.setVolume(21);
}

void playSongByName(string fileName) {
    if (SD.exists(path)) {
        Serial.println("Playing: " + path);
        audio.connecttoFS(SD, path.c_str());
        isPaused = false; 
    } else {
        Serial.println("Error: Song " + fileName + " not found in /music/");
    }
}

void togglePause() {
    isPaused = !isPaused;
    audio.pauseResume();
}

void loopAudio() {
    audio.loop();
}
