#ifndef AUDIO_HELPER_H
#define AUDIO_HELPER_H
#include <Arduino.h>

void setupAudio();
void loopAudio();
void togglePause();
void playSongByName(String fileName);
void stopAudio(); 
bool isAudioPlaying(); 

#endif
