#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>
#include <SD.h>
#include <SPI.h>
#include "servo_helper.h"
#include "audio_helper.h"
#include "sd.h"
#include "index.h" 

WebServer server(80);
File uploadFile;

const char* ssid = "Hehehe";
const char* pass = "khongbiet";


void setupWebServer() {
    server.on("/", HTTP_GET, []() {
        server.send_P(200, "text/html", PAGE);
    });

    server.on("/music", HTTP_GET, []() {
        if (!server.hasArg("cmd")) {
            server.send(400, "text/plain", "Missing cmd, bruh");
            return;
        }

        String cmd = server.arg("cmd");
        String file = server.hasArg("file") ? server.arg("file") : "";

        
        if (cmd == "select" || cmd == "play") {
             Serial.println("Play req: " + file);
             playSongByName(file);
        }
        else if (cmd == "stop") {
            Serial.println("Stop req");
            stopAudio();
        }

        server.send(200, "text/plain", "OK");
    });

    server.on("/arrow", HTTP_GET, []() {
        if (isAudioPlaying()) {
            Serial.println("Denied: Music is playing");
            server.send(400, "text/plain", "Music is playing! Stop it first.");
            return;
        }

        if (!server.hasArg("dir")) {
            server.send(400, "text/plain", "Missing dir");
            return;
        }

        String dir = server.arg("dir");
        Serial.println("Move: " + dir);

        if (dir == "TURN_LEFT") turnLeft();
        else if (dir == "TURN_RIGHT") turnRight();
        else if (dir == "UP") moveForward();
        else if (dir == "DOWN") moveBack();
        else if (dir == "LEFT") moveLeft();
        else if (dir == "RIGHT") moveRight();

        server.send(200, "text/plain", "OK");
    });

    server.on("/upload", HTTP_POST, []() { server.send(200); }, handleUpload);
    server.on("/list", HTTP_GET, handleList);
    server.on("/delete", HTTP_GET, handleDelete);

    server.begin();
    Serial.println("Web server started");
}

void connectWiFi() {
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, pass);
    Serial.print("Connecting WiFi");
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println("\nIP: " + WiFi.localIP().toString());
}

void setup() {
    Serial.begin(115200);
    initSD();
    setupAudio();
    
    if (!SD.exists("/music")) {
        SD.mkdir("/music");
        Serial.println("Created /music");
    }

    setupServos();
    connectWiFi();
    setupWebServer();
}

void loop() {
    loopAudio();
    
    server.handleClient();
}
