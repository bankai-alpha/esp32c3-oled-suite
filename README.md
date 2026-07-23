# ⚡ ESP32-C3 & OLED Display Project Suite

![ESP32-C3](https://img.shields.io/badge/MCU-ESP32--C3%20RISC--V-blue?style=for-the-badge&logo=espressif)
![Display](https://img.shields.io/badge/Display-OLED%20SSD1306%20128x64-black?style=for-the-badge&logo=hardware)
![Framework](https://img.shields.io/badge/Framework-Arduino%20C%2B%2B-00979D?style=for-the-badge&logo=arduino)
![License](https://img.shields.io/badge/License-MIT-green?style=for-the-badge)

Welcome to the ultimate **ESP32-C3 Microcontroller & 0.96"/1.3" I2C OLED Display (128x64)** project repository! This collection features **5 complete, hardware-tested embedded applications** spanning IoT smart home automation, Bluetooth HID remotes, real-time financial tracking, environmental sensing, and retro arcade gaming.

Includes an **Interactive Browser Simulator** (`web-dashboard`) to preview OLED screens, test button controls, and copy code without physical hardware connected.

---

## 📸 Interactive Web Simulator

Launch the included HTML5 canvas workstation to simulate all 5 OLED screens live in your web browser:

```bash
cd web-dashboard
python3 -m http.server 8080
# Open http://localhost:8080 in your browser
```

---

## 🗂️ The 5 Projects at a Glance

| # | Project Name | Description | Key Stack & APIs | Direct Link |
| :-: | :--- | :--- | :--- | :-: |
| **01** | **WiFi Smart Weather Clock** | Real-time NTP clock sync, OpenWeatherMap integration, WiFi RSSI indicator | `time.h`, `HTTPClient`, `ArduinoJson` | [View Code](./01_wifi_smart_weather_clock/) |
| **02** | **BLE Media Controller** | Bluetooth LE HID remote to control Play/Pause, Vol +/-, Next Track with screen feedback | `BleKeyboard.h`, `Adafruit_SSD1306` | [View Code](./02_ble_media_controller/) |
| **03** | **Crypto & Stock Ticker** | Live Bitcoin, Ethereum & Solana prices from CoinGecko API with OLED line graph chart | `ArduinoJson`, `Adafruit_GFX` | [View Code](./03_crypto_stock_ticker/) |
| **04** | **Environmental Station** | Temperature, Humidity, Heat Index & AQI dashboard with animated gauge bars | `DHT.h`, `Adafruit_SSD1306` | [View Code](./04_environment_sensor_station/) |
| **05** | **Retro Dino Runner Game** | 60 FPS arcade runner with jump/duck physics and persistent NVS flash High Scores | `Preferences.h`, `Adafruit_GFX` | [View Code](./05_retro_oled_dino_runner/) |

---

## 📌 Hardware Pinout & Wiring Diagram

Standard I2C hardware pin mappings for ESP32-C3 boards (ESP32-C3 SuperMini, Seeed Studio XIAO ESP32C3, DevKitM-1):

```
                       +-------------------+
                       |    ESP32-C3       |
                       |                   |
   3.3V Power  <------ | 3V3           GND | ------> Ground
   I2C Data    <------ | GPIO 8      GPIO 9| ------> I2C Clock
                       | (SDA)       (SCL) |
                       +-------------------+
                                 |
                                 v
                       +-------------------+
                       | SSD1306 OLED      |
                       | 128x64 (0x3C)     |
                       +-------------------+
```

### Complete Pin Matrix

| Component | ESP32-C3 SuperMini Pin | Seeed XIAO ESP32C3 Pin | OLED / Peripheral Connection |
| :--- | :--- | :--- | :--- |
| **VCC** | `3V3` | `3V3` | OLED Power Pin (3.3V) |
| **GND** | `GND` | `GND` | OLED Ground Pin |
| **SDA** | **GPIO 8** | **GPIO 5** | OLED Data (I2C SDA) |
| **SCL** | **GPIO 9** | **GPIO 6** | OLED Clock (I2C SCL) |
| **Button 1** | **GPIO 0** | **GPIO 0** | Push Button (Play / Jump / Page Cycle) |
| **Button 2** | **GPIO 1** | **GPIO 1** | Push Button (Vol Up / Duck) |
| **Button 3** | **GPIO 2** | **GPIO 2** | Push Button (Vol Down) |
| **Button 4** | **GPIO 3** | **GPIO 3** | Push Button (Next Track) |
| **DHT22 Data** | **GPIO 4** | **GPIO 4** | Sensor Data (with 10k resistor to 3.3V) |

---

## 🛠️ Arduino IDE Setup Guide

### 1. Board Manager Installation
Add the Espressif ESP32 package URL to **Arduino IDE > Preferences > Additional Boards Manager URLs**:
```
https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json
```
Go to **Tools > Board > Boards Manager**, search for **esp32**, and click **Install**.

### 2. Required Libraries
Install the following libraries via **Tools > Manage Libraries** (`Ctrl + Shift + I`):
- `Adafruit SSD1306` by Adafruit
- `Adafruit GFX Library` by Adafruit
- `ArduinoJson` by Benoit Blanchon
- `DHT sensor library` by Adafruit
- `ESP32-BLE-Keyboard` by T-vK ([Download ZIP from GitHub](https://github.com/T-vK/ESP32-BLE-Keyboard))

---

## 🚀 How to Flash a Project

1. Open any project sketch file (e.g. `01_wifi_smart_weather_clock/01_wifi_smart_weather_clock.ino`).
2. Update WiFi credentials or configuration settings if required:
   ```cpp
   const char* WIFI_SSID     = "YOUR_WIFI_SSID";
   const char* WIFI_PASSWORD = "YOUR_WIFI_PASSWORD";
   ```
3. Connect your ESP32-C3 via USB.
4. Select **Tools > Board > ESP32C3 Dev Module** and choose your COM/TTY port.
5. Click **Upload** (`Ctrl + U`).

---

## 🤝 Contributing

Contributions, bug reports, and new project ideas are welcome! Feel free to open an issue or submit a pull request.

---

## 📄 License

This repository is licensed under the [MIT License](./LICENSE). Feel free to use, modify, and build upon these projects for personal or commercial applications.
# esp32c3-oled-suite
