# Project 03: Desktop Crypto & Stock Price Ticker

## Overview
A desktop price ticker for your favorite cryptocurrencies (Bitcoin, Ethereum, Solana). The **ESP32-C3** fetches current live market rates via CoinGecko REST APIs over WiFi and renders the price, 24h change %, and a dynamic **OLED sparkline price graph**.

## Hardware Pinout

| ESP32-C3 Pin | OLED Module | Function |
| :--- | :--- | :--- |
| `GPIO 8` | **SDA** | I2C Serial Data |
| `GPIO 9` | **SCL** | I2C Serial Clock |
| `3V3` | **VCC** | Power Supply |
| `GND` | **GND** | Ground |

## Setup Instructions

1. Open `03_crypto_stock_ticker.ino`.
2. Configure `WIFI_SSID` and `WIFI_PASSWORD`.
3. Install `ArduinoJson` (v6 or v7) and `Adafruit_SSD1306` libraries.
4. Upload to ESP32-C3! The display will rotate between BTC, ETH, and SOL every 5 seconds.
