# Project 01: WiFi Smart Weather & NTP Clock

## Overview
This project turns an **ESP32-C3** paired with a **0.96" or 1.3" I2C OLED display (SSD1306)** into a smart desk clock. It synchronizes exact time via Network Time Protocol (NTP) and pulls real-time temperature, humidity, and weather conditions from the OpenWeatherMap API over WiFi.

## Hardware Connections (I2C)

| ESP32-C3 Pin | OLED Module | Function |
| :--- | :--- | :--- |
| `GPIO 8` | **SDA** | I2C Serial Data |
| `GPIO 9` | **SCL** | I2C Serial Clock |
| `3V3` | **VCC** | 3.3V Power |
| `GND` | **GND** | Ground |

## Setup Instructions

1. Open `01_wifi_smart_weather_clock.ino` in Arduino IDE or PlatformIO.
2. Configured for **Lagos, Nigeria** (`CITY = "Lagos"`, `COUNTRY_CODE = "NG"`, `GMT_OFFSET_SEC = 3600` for WAT / UTC+1).
3. Replace `"WIFI_SSID"` and `"WIFI_PASSWORD"` with your local WiFi network details.
4. Get a free API Key from [OpenWeatherMap](https://openweathermap.org/api) and set `API_KEY`.
5. Compile and upload to your ESP32-C3 board!
