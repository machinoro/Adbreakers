#include "audio_helper.h"
#include "Audio.h"
#include "SD.h"
#include "FS.h"
#include <vector>

#define DAC_PIN        25 

Audio audio;
bool isPaused = false;

void setupAudio() {
    audio.setPinout(0, 0, DAC_PIN);
    audio.setVolume(21);
}

void playSongByName(String fileName) { 
    String path = "/music/" + fileName; 
    if (SD.exists(path)) {
        Serial.println("Playing: " + path);
        audio.connecttoFS(SD, path.c_str());
        isPaused = false; 
    } else {
        Serial.println("Error: Song " + fileName + " not found in /music/");
    }
}

void stopAudio() {
    audio.stopSong();
}

void togglePause() {
    isPaused = !isPaused;
    audio.pauseResume();
}

void loopAudio() {
    audio.loop();
}

bool isAudioPlaying() {
    return audio.isRunning();
}
