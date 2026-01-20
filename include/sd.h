#ifndef SD_card
#define SD_card       
#include <SD.h>
#include <SPI.h>
#include <WebServer.h>
#include "audio_helper.h"
extern WebServer server;
extern File uploadFile;

void initSD(); 
void handleList();
void handleDelete();
void handleUpload();

#endif // !SD_card
