#include <SD.h>
#include <SPI.h>
#include "sd.h"

#define SD_CS          5
#define SPI_MOSI       23
#define SPI_MISO       19
#define SPI_SCK        18

void initSD() {
    pinMode(SD_CS, OUTPUT);
    digitalWrite(SD_CS, HIGH);
    SPI.begin(SPI_SCK, SPI_MISO, SPI_MOSI);
    
    if (!SD.begin(SD_CS)) {
        Serial.println("Error: SD Card Mount Failed");
        return;
    }

}

void handleList() {
    File dir = SD.open("/music");
    if (!dir || !dir.isDirectory()) {
        server.send(500, "application/json", "[]");
        return;
    }

    String json = "[";
    bool first = true;

    File file = dir.openNextFile();
    while (file) {
        if (!file.isDirectory()) {
            String name = String(file.name());
            int lastSlash = name.lastIndexOf('/');
            if (lastSlash >= 0) name = name.substring(lastSlash + 1);

            if (!first) json += ",";
            json += "\"" + name + "\"";
            first = false;
        }
        file = dir.openNextFile();
    }
    json += "]";
    dir.close(); 

    server.sendHeader("Cache-Control", "no-store");
    server.send(200, "application/json", json);
}

void handleDelete() {
    if (!server.hasArg("file")) {
        server.send(400, "text/plain", "Missing file");
        return;
    }

    if(isAudioPlaying()) stopAudio();

    String name = "/music/" + server.arg("file");
    if (SD.exists(name)) {
        SD.remove(name);
        server.send(200, "text/plain", "Deleted");
    } else {
        server.send(404, "text/plain", "Not found");
    }
}

void handleUpload() {
    HTTPUpload& upload = server.upload();

    if (upload.status == UPLOAD_FILE_START) {
        if(isAudioPlaying()) stopAudio();

        String filename = upload.filename;
        if (!filename.startsWith("/")) filename = "/" + filename;
        if (!filename.startsWith("/music/")) filename = "/music" + filename;

        Serial.println("Upload start: " + filename);
        if (SD.exists(filename)) SD.remove(filename);
        
        uploadFile = SD.open(filename, FILE_WRITE);
    }
    else if (upload.status == UPLOAD_FILE_WRITE) {
        if (uploadFile) {
            uploadFile.write(upload.buf, upload.currentSize);
        }
    }
    else if (upload.status == UPLOAD_FILE_END) {
        if (uploadFile) {
            uploadFile.close();
            Serial.println("Upload done: " + String(upload.totalSize));
        }
    }
}


