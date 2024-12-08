#include "ota.h"
#include <WiFi.h>
#include <LittleFS.h>
#include <ESPAsyncWebServer.h>
//#include <U8g2lib.h>

U8G2_SH1106_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE);

AsyncWebServer server(80);
unsigned long previousMillis = 0; // Biến để lưu thời gian lần cuối kiểm tra kết nối
const long interval = 10000; // Khoảng thời gian giữa các lần kiểm tra (10 giây)

void setupOTA(const char* ssid, const char* password, const char* otaPassword) {
    Serial.begin(115200);

    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println("\nWiFi connected");
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());

    u8g2.begin();
    u8g2.clearBuffer();
    u8g2.setFont(u8g2_font_ncenB08_tr);
    u8g2.drawStr(0, 10, "WiFi connected");
    u8g2.drawStr(0, 30, "IP address:");
    String url = WiFi.localIP().toString();
    u8g2.drawStr(0, 50, url.c_str());
    u8g2.sendBuffer();

    // Mount LittleFS
    if (!LittleFS.begin()) {
        Serial.println("An Error has occurred while mounting LittleFS");
        return;
    }

    ArduinoOTA.setPassword(otaPassword);
    ArduinoOTA.begin();

    ArduinoOTA.onStart([]() {
        String type;
        if (ArduinoOTA.getCommand() == U_FLASH) {
            type = "sketch";
        } else {
            type = "filesystem";
        }
        Serial.println("Start updating " + type);

        u8g2.clearBuffer();
        u8g2.setFont(u8g2_font_ncenB08_tr);
        u8g2.drawStr(0, 30, "Start updating...");
        u8g2.sendBuffer();
    });

    ArduinoOTA.onEnd([]() {
        Serial.println("\nEnd");

        u8g2.clearBuffer();
        u8g2.setFont(u8g2_font_ncenB08_tr);
        u8g2.drawStr(0, 30, "Update finished!");
        u8g2.sendBuffer();
    });

    ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
        Serial.printf("Progress: %u%%\r", (progress / (total / 100)));

        char progressStr[20];
        sprintf(progressStr, "Progress: %u%%", (progress / (total / 100)));
        u8g2.clearBuffer();
        u8g2.setFont(u8g2_font_ncenB08_tr);
        u8g2.drawStr(0, 30, progressStr);
        u8g2.sendBuffer();
    });

    ArduinoOTA.onError([](ota_error_t error) {
        Serial.printf("Error[%u]: ", error);
        if (error == OTA_AUTH_ERROR) {
            Serial.println("Auth Failed");

            u8g2.clearBuffer();
            u8g2.setFont(u8g2_font_ncenB08_tr);
            u8g2.drawStr(0, 30, "Auth Failed");
            u8g2.sendBuffer();
        } else if (error == OTA_BEGIN_ERROR) {
            Serial.println("Begin Failed");

            u8g2.clearBuffer();
            u8g2.setFont(u8g2_font_ncenB08_tr);
            u8g2.drawStr(0, 30, "Begin Failed");
            u8g2.sendBuffer();
        } else if (error == OTA_CONNECT_ERROR) {
            Serial.println("Connect Failed");

            u8g2.clearBuffer();
            u8g2.setFont(u8g2_font_ncenB08_tr);
            u8g2.drawStr(0, 30, "Connect Failed");
            u8g2.sendBuffer();
        } else if (error == OTA_RECEIVE_ERROR) {
            Serial.println("Receive Failed");

            u8g2.clearBuffer();
            u8g2.setFont(u8g2_font_ncenB08_tr);
            u8g2.drawStr(0, 30, "Receive Failed");
            u8g2.sendBuffer();
        } else if (error == OTA_END_ERROR) {
            Serial.println("End Failed");

            u8g2.clearBuffer();
            u8g2.setFont(u8g2_font_ncenB08_tr);
            u8g2.drawStr(0, 30, "End Failed");
            u8g2.sendBuffer();
        }
    });

    setupWebServer();
}

void handleOTA() {
    ArduinoOTA.handle();
}

void setupWebServer() {
    // Serve index.html
    server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
        request->send(LittleFS, "/index.html", "text/html");
    });

    // Upload file to the filesystem
    server.on("/upload", HTTP_POST, 
    [](AsyncWebServerRequest *request) {},
    [](AsyncWebServerRequest *request, const String& filename, size_t index, uint8_t *data, size_t len, bool final) {
        static File uploadFile; // Khai báo static để duy trì trạng thái giữa các lệnh gọi

        if (index == 0) {
            Serial.printf("UploadStart: %s\n", filename.c_str());
            uploadFile = LittleFS.open("/" + filename, "w");
            if (!uploadFile) {
                Serial.println("Failed to open file for writing");
                return;
            }
        }
        if (len) {
            Serial.printf("UploadChunk: %s\n", filename.c_str());
            if (uploadFile) {
                uploadFile.write(data, len);
            }
        }
        if (final) {
            Serial.printf("UploadEnd: %s\n", filename.c_str());
            if (uploadFile) {
                uploadFile.close();
            }
            request->send(200, "text/plain", "File uploaded successfully");
        }
    });

    // Download file from the filesystem
    server.on("/download", HTTP_GET, [](AsyncWebServerRequest *request) {
        request->send(LittleFS, "/config.json", "application/json");
    });

    server.begin();
}


void checkWiFiConnection(const char* ssid, const char* password) {
    unsigned long currentMillis = millis();
    if (currentMillis - previousMillis >= interval) {
        previousMillis = currentMillis;
        if (WiFi.status() != WL_CONNECTED) {
            Serial.println("WiFi disconnected, attempting to reconnect...");
            WiFi.disconnect();
            WiFi.begin(ssid, password);
            while (WiFi.status() != WL_CONNECTED) {
                delay(500);
                Serial.print(".");
            }
            Serial.println("\nWiFi reconnected");
            Serial.print("IP address: ");
            Serial.println(WiFi.localIP());

            u8g2.clearBuffer();
            u8g2.setFont(u8g2_font_ncenB08_tr);
            u8g2.drawStr(0, 10, "WiFi reconnected");
            u8g2.drawStr(0, 30, "IP address:");
            String url = WiFi.localIP().toString();
            u8g2.drawStr(0, 50, url.c_str());
            u8g2.sendBuffer();
        }
    }
}
