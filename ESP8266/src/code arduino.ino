#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <SD.h>
#include <SPI.h>

ESP8266WebServer server(80);
File uploadFile;

const char* ssid = "tên wifi";
const char* pass = "mật khẩu";
const char PAGE[] PROGMEM = R"rawliteral(

#dán file html vào đây

)rawliteral";
#define SD_CS D8   


void connectWiFi() {
  WiFi.begin(ssid, pass);
  Serial.print("Connecting WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nIP: " + WiFi.localIP().toString());
}



void handleUpload() {
  HTTPUpload& upload = server.upload();

  if (upload.status == UPLOAD_FILE_START) {
  String filename = upload.filename;

  
  int slash = filename.lastIndexOf('/');
  int backslash = filename.lastIndexOf('\\');
  int idx = max(slash, backslash);
  if (idx >= 0) {
    filename = filename.substring(idx + 1);
  }

  filename = "/music/" + filename;

  
  if (SD.exists(filename)) {
    SD.remove(filename);
    Serial.println("Old file removed");
  }

  Serial.println("Upload start: " + filename);
  uploadFile = SD.open(filename, FILE_WRITE);
  Serial.println("Final path: " + filename);

}

  else if (upload.status == UPLOAD_FILE_WRITE) {
    if (uploadFile) {
      uploadFile.write(upload.buf, upload.currentSize);
    }
  }  

  else if (upload.status == UPLOAD_FILE_END) {
    if (uploadFile) {
      uploadFile.flush();
      uploadFile.close();
      Serial.println("Upload done");
    }
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
      String name = file.name();
      name.replace("/music/", ""); 

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

  String name = "/music/" + server.arg("file");

  if (SD.exists(name)) {
    SD.remove(name);
    server.send(200, "text/plain", "Deleted");
  } else {
    server.send(404, "text/plain", "Not found");
  }
}


void setup() {
  Serial.begin(115200);

  connectWiFi();

  if (!SD.begin(SD_CS)) {
    Serial.println("SD init failed");
    return;
  }
  Serial.println("SD OK");

  if (!SD.exists("/music")) {
    SD.mkdir("/music");
    Serial.println("Created /music");
  }

  server.on("/", []() {
    server.send_P(200, "text/html", PAGE);
  });

  server.on("/music", HTTP_GET, []() {
    if (!server.hasArg("cmd")) {
      server.send(400, "text/plain", "Missing cmd");
      return;
    }

    String cmd  = server.arg("cmd");
    String file = server.hasArg("file") ? server.arg("file") : "";

    if (cmd == "select") {
      Serial.println("Đang chọn file " + file);
    }
    else if (cmd == "play") {
      Serial.println("Phát file " + file);
    }
    else if (cmd == "stop") {
      Serial.println("Dừng file " + file);
    }

    server.send(200, "text/plain", "OK");
  });


  server.on("/arrow", HTTP_GET, []() {
    if (!server.hasArg("dir")) {
      server.send(400, "text/plain", "Missing dir");
      return;
    }

    String dir = server.arg("dir");

    if (dir == "TURN_LEFT") {
      Serial.println("Turn left");
    }
    else if (dir == "TURN_RIGHT") {
      Serial.println("Turn right");
    }
    else if (dir == "UP") {
      Serial.println("Move forward");
    }
    else if (dir == "DOWN") {
      Serial.println("Move backward");
    }
    else if (dir == "LEFT") {
      Serial.println("Move left");
    }
    else if (dir == "RIGHT") {
      Serial.println("Move right");
    }

    server.send(200, "text/plain", "OK");
  });


  server.on("/upload", HTTP_POST,
    []() { server.send(200); },
    handleUpload
  );

  server.on("/list", HTTP_GET, handleList);
  server.on("/delete", HTTP_GET, handleDelete);

  server.begin();
  Serial.println("Web server started");
}


void loop() {
  server.handleClient();
}
