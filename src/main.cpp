#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>
#include <SD.h>
#include <SPI.h>
#include "servo.h"
#include "audio_helper.h"
#include "sd.h"
#include "index.h" 

WebServer server(80);
File uploadFile;

const char* ssid = "Vo";
const char* pass = "hieu2007";

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

        if (dir == "TURN_LEFT") { turnLeft(); turnLeft(); }
        else if (dir == "TURN_RIGHT") { turnRight(); turnRight(); }
        else if (dir == "UP") { moveForward(); moveForward(); stableKnee(); }
        else if (dir == "DOWN") { moveBack(); moveBack(); stableKnee(); }
        else if (dir == "LEFT") { moveLeft(); moveLeft(); stableKnee(); }
        else if (dir == "RIGHT") { moveRight(); moveRight(); stableKnee(); }
        else if (dir == "HeD") {
            current_height -= 0.5;
            setHeight(current_height);
        } else if (dir == "HeU") {
            current_height += 0.5;
            setHeight(current_height);
        }
        

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
    

    setupServos();
    connectWiFi();
    setupWebServer();
}

void loop() {
    loopAudio();
    
    server.handleClient();
}
