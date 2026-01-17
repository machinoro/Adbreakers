#include <Arduino.h>
#include "Audio.h"
#include "SD.h"
#include "FS.h"

#define SD_CS          5
#define SPI_MOSI       23
#define SPI_MISO       19
#define SPI_SCK        18
#define DAC_PIN        25 

Audio audio;

void setup() {
    Serial.begin(115200);

    pinMode(SD_CS, OUTPUT);
    digitalWrite(SD_CS, HIGH);
    SPI.begin(SPI_SCK, SPI_MISO, SPI_MOSI);
    
    if (!SD.begin(SD_CS)) {
        Serial.println("Error: SD Card Mount Failed");
        return;
    }
    Serial.println("SD Card Initialized");

    audio.setPinout(0, 0, DAC_PIN); 
    
    audio.setVolume(21); 

    audio.connecttoFS(SD, "/music/one.mp3"); 
}

void loop() {
    audio.loop();
}

void audio_info(const char *info) {
    Serial.print("info        "); Serial.println(info);
}
void audio_eof_mp3(const char *info) { 
    Serial.print("End of file: "); Serial.println(info);
}
