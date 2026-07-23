/*
  Project 01: WiFi Smart Weather & NTP Clock
  Target MCU: ESP32-C3
  Display: 0.96" / 1.3" I2C OLED (SSD1306 128x64)
  
  Pinout (ESP32-C3):
    SDA -> GPIO 8 (or GPIO 5 / custom)
    SCL -> GPIO 9 (or GPIO 6 / custom)
    VCC -> 3.3V
    GND -> GND
    
  Required Libraries (install via Arduino Library Manager):
    - Adafruit SSD1306
    - Adafruit GFX Library
    - ArduinoJson (v6 or v7)
*/

#include <WiFi.h>
#include <HTTPClient.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <ArduinoJson.h>
#include "time.h"

// --- Configuration Settings ---
const char* WIFI_SSID     = "YOUR_WIFI_SSID";
const char* WIFI_PASSWORD = "YOUR_WIFI_PASSWORD";

// OpenWeatherMap Configuration (Get free key at openweathermap.org)
const String CITY         = "Tokyo";
const String COUNTRY_CODE = "JP";
const String API_KEY      = "YOUR_OPENWEATHERMAP_API_KEY";

// NTP Server Settings
const char* NTP_SERVER_1  = "pool.ntp.org";
const char* NTP_SERVER_2  = "time.nist.gov";
const long  GMT_OFFSET_SEC= 3600 * 9; // e.g. UTC+9 for JST (Adjust for your timezone)
const int   DAYLIGHT_OFFSET_SEC = 0;

// OLED Config
#define SCREEN_WIDTH  128
#define SCREEN_HEIGHT 64
#define OLED_RESET    -1
#define OLED_ADDRESS  0x3C // 0x3C or 0x3D

#define SDA_PIN 8
#define SCL_PIN 9

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// Global Variables
float currentTemp = 0.0;
int currentHumidity = 0;
String weatherDesc = "Loading...";
unsigned long lastWeatherUpdate = 0;
const unsigned long WEATHER_UPDATE_INTERVAL = 10 * 60 * 1000; // 10 minutes

void setup() {
  Serial.begin(115200);
  delay(1000);
  Serial.println("\n[ESP32-C3] Starting WiFi Smart Weather Clock...");

  // Initialize Wire for ESP32-C3
  Wire.begin(SDA_PIN, SCL_PIN);

  // Initialize Display
  if (!display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDRESS)) {
    Serial.println("SSD1306 allocation failed! Check I2C wiring.");
    for (;;);
  }

  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);
  display.setTextSize(1);
  display.setCursor(10, 20);
  display.println("ESP32-C3 Booting...");
  display.setCursor(10, 35);
  display.println("Connecting WiFi...");
  display.display();

  // Connect WiFi
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED && attempts < 20) {
    delay(500);
    Serial.print(".");
    attempts++;
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\nWiFi Connected! IP: " + WiFi.localIP().toString());
    configTime(GMT_OFFSET_SEC, DAYLIGHT_OFFSET_SEC, NTP_SERVER_1, NTP_SERVER_2);
    fetchWeatherData();
  } else {
    Serial.println("\nWiFi Connection Failed! Proceeding in offline mode.");
  }
}

void loop() {
  if (WiFi.status() == WL_CONNECTED) {
    if (millis() - lastWeatherUpdate > WEATHER_UPDATE_INTERVAL || lastWeatherUpdate == 0) {
      fetchWeatherData();
    }
  }

  updateDisplay();
  delay(1000);
}

void fetchWeatherData() {
  if (API_KEY == "YOUR_OPENWEATHERMAP_API_KEY") {
    weatherDesc = "No API Key";
    currentTemp = 24.5;
    currentHumidity = 55;
    return;
  }

  HTTPClient http;
  String url = "http://api.openweathermap.org/data/2.5/weather?q=" + CITY + "," + COUNTRY_CODE + "&units=metric&appid=" + API_KEY;
  
  http.begin(url);
  int httpCode = http.GET();

  if (httpCode > 0) {
    if (httpCode == HTTP_CODE_OK) {
      String payload = http.getString();
      DynamicJsonDocument doc(1024);
      deserializeJson(doc, payload);

      currentTemp = doc["main"]["temp"];
      currentHumidity = doc["main"]["humidity"];
      weatherDesc = doc["weather"][0]["main"].as<String>();
      lastWeatherUpdate = millis();
      Serial.printf("Weather: %.1fC, %d%%, %s\n", currentTemp, currentHumidity, weatherDesc.c_str());
    }
  } else {
    Serial.printf("HTTP GET failed, error: %s\n", http.errorToString(httpCode).c_str());
  }
  http.end();
}

void updateDisplay() {
  display.clearDisplay();

  // Draw Header Line & Status Bar
  display.setTextSize(1);
  display.setCursor(0, 0);
  display.print("ESP32-C3");
  
  // WiFi signal indicator
  int rssi = WiFi.RSSI();
  display.setCursor(85, 0);
  if (WiFi.status() == WL_CONNECTED) {
    display.printf("WiFi:%d", rssi);
  } else {
    display.print("No-WiFi");
  }
  display.drawFastHLine(0, 10, 128, SSD1306_WHITE);

  // Time Display
  struct tm timeinfo;
  if (getLocalTime(&timeinfo)) {
    char timeStr[10];
    char dateStr[20];
    strftime(timeStr, sizeof(timeStr), "%H:%M:%S", &timeinfo);
    strftime(dateStr, sizeof(dateStr), "%a, %b %d", &timeinfo);

    display.setTextSize(2);
    display.setCursor(16, 16);
    display.print(timeStr);

    display.setTextSize(1);
    display.setCursor(24, 35);
    display.print(dateStr);
  } else {
    display.setTextSize(2);
    display.setCursor(20, 20);
    display.print("00:00:00");
  }

  display.drawFastHLine(0, 46, 128, SSD1306_WHITE);

  // Bottom Weather Bar
  display.setTextSize(1);
  display.setCursor(0, 52);
  display.printf("%.1fC  %d%%  %s", currentTemp, currentHumidity, weatherDesc.substring(0, 6).c_str());

  display.display();
}
