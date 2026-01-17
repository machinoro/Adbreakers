#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <SD.h>
#include <SPI.h>

ESP8266WebServer server(80);
File uploadFile;

const char* ssid = "tên wifi";
const char* pass = "mật khẩu";
/* ===== HTML TRANG WEB ===== */
const char PAGE[] PROGMEM = R"rawliteral(

#dán file html vào đây

)rawliteral";
#define SD_CS D8   // chân CS của SD

// ================= WIFI =================
void connectWiFi() {
  WiFi.begin(ssid, pass);
  Serial.print("Connecting WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nIP: " + WiFi.localIP().toString());
}


// ================= UPLOAD =================
void handleUpload() {
  HTTPUpload& upload = server.upload();

  if (upload.status == UPLOAD_FILE_START) {
  String filename = upload.filename;

  // 🔥 CẮT fakepath nếu có
  int slash = filename.lastIndexOf('/');
  int backslash = filename.lastIndexOf('\\');
  int idx = max(slash, backslash);
  if (idx >= 0) {
    filename = filename.substring(idx + 1);
  }

  filename = "/music/" + filename;

  // 🔥 XÓA FILE CŨ TRƯỚC
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

// ================= LIST FILE =================
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
      name.replace("/music/", ""); // bỏ path

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



// ================= DELETE =================
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

// ================= SETUP =================
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

  server.on("/arrow", []() {
    if (server.hasArg("dir")) {
      Serial.println(server.arg("dir"));
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

// ================= LOOP =================
void loop() {
  server.handleClient();
}
