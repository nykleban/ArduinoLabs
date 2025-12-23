#include <Arduino.h>
#include <DHT.h>
#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <index_html.h>


AsyncWebServer server(80);

static const char* AP_SSID = "KLEBANNNNNNNN";
static const char* AP_PASS = "12345678";
// static const char* OAwifiName = "OA_Inet_Free";
// static const char* OAwifiPass = "iloveoa";
static const char* OAwifiName = "A36_nykleban";
static const char* OAwifiPass = "1234567z";

const int pinRealey = 27;
const int dth11Pin = 22;
DHT sensorDHT(dth11Pin, DHT11);

float tempC = 0.0;
float humidity = 0.0;
float targetTemperature = 30.0;
bool relayState = false;

unsigned long lastSensorRead = 0;
const long sensorInterval = 200;

void startOwnWifi() {
    WiFi.mode(WIFI_AP);
    WiFi.softAP(AP_SSID, AP_PASS);
    Serial.print("AP Started. IP: ");
    Serial.println(WiFi.softAPIP());
}

void startLocalWifi() {
    WiFi.mode(WIFI_STA);
    WiFi.begin(OAwifiName, OAwifiPass);
    Serial.print("Connecting STA");

    unsigned long startAttempt = millis();
    while(WiFi.status() != WL_CONNECTED && (millis() - startAttempt) < 8000) {
        delay(500);
        Serial.print(".");
    }
    Serial.println();

    if(WiFi.status() == WL_CONNECTED) {
        Serial.print("Connected! IP: ");
        Serial.println(WiFi.localIP());
    } else {
        Serial.println("Failed to connect. Starting AP...");
        startOwnWifi();
    }
}

void setupRoutes() {
    server.on("/", HTTP_GET, [](AsyncWebServerRequest * req) {
        req->send(200, "text/html", INDEX_HTML);
    });

    server.on("/readings", HTTP_GET, [](AsyncWebServerRequest *req){
        String json = "{";
        json += "\"temperature\":" + String(tempC) + ",";
        json += "\"humidity\":" + String(humidity) + ",";
        json += "\"target\":" + String(targetTemperature) + ",";
        json += "\"relay\":" + String(relayState ? 1 : 0);
        json += "}";
        req->send(200, "application/json", json);
    });

    server.on("/set-target", HTTP_GET, [](AsyncWebServerRequest *req){
        if (req->hasParam("val")) {
            String val = req->getParam("val")->value();
            targetTemperature = val.toFloat();
            Serial.print("New Target Temp: ");
            Serial.println(targetTemperature);
        }
        req->send(200, "text/plain", "OK");
    });

    server.begin();
}

void setup() {
    Serial.begin(9600);
    pinMode(pinRealey, OUTPUT);
    digitalWrite(pinRealey, LOW);
    sensorDHT.begin();
    startLocalWifi();
    setupRoutes();
}

void loop() {
    unsigned long currentMillis = millis();

    if (currentMillis - lastSensorRead >= sensorInterval) {
        lastSensorRead = currentMillis;

        float newH = sensorDHT.readHumidity();
        float newT = sensorDHT.readTemperature();

        if (!isnan(newH) && !isnan(newT)) {
            tempC = newT;
            humidity = newH;
            Serial.printf("Temp: %.1f C, Hum: %.1f %%, Target: %.1f\n", tempC, humidity, targetTemperature);
        } else {
            Serial.println("DHT read failed!");
        }

        if (tempC < targetTemperature) {
            relayState = true;
            digitalWrite(pinRealey, HIGH);
        } else {
            relayState = false;
            digitalWrite(pinRealey, LOW);
        }
    }
}