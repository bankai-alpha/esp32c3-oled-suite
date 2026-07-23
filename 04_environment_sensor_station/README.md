# Project 04: Environmental Monitoring Station & Gauge Dashboard

## Overview
Monitors ambient indoor/outdoor environmental conditions using a DHT22/DHT11 or BME280 sensor connected to an **ESP32-C3**. Features multi-screen page navigation (Overview, Temperature Progress Bar, Humidity Gauge, Air Quality Index) controlled by a push button.

## Hardware Wiring

| ESP32-C3 Pin | Component | Function |
| :--- | :--- | :--- |
| `GPIO 4` | DHT22 Pin 2 (Data) | Temperature & Humidity Data (with 10k pull-up to 3.3V) |
| `GPIO 0` | Push Button | Page Navigation Interrupt Button |
| `GPIO 8` | OLED **SDA** | I2C Data |
| `GPIO 9` | OLED **SCL** | I2C Clock |

## Setup Instructions

1. Install `DHT sensor library` by Adafruit in Arduino IDE.
2. Connect sensor and button as shown in the table.
3. Upload `04_environment_sensor_station.ino` to your ESP32-C3 board.
4. Press the button to cycle through the 4 animated metric pages!
