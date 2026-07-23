# Project 05: Retro OLED Dino Runner Arcade Game

## Overview
A 60 FPS retro arcade runner game built for **ESP32-C3 and OLED (128x64)** displays! Play as a dinosaur jumping over cacti and ducking under birds using push buttons. Features automatic high score saving to ESP32-C3 Non-Volatile Storage (NVS) flash memory so your high scores persist even after powering off!

## Hardware Wiring

| ESP32-C3 Pin | Component | Function |
| :--- | :--- | :--- |
| `GPIO 0` | Push Button 1 | Jump / Start / Retry |
| `GPIO 1` | Push Button 2 | Duck / Fast Drop |
| `GPIO 8` | OLED **SDA** | I2C Data |
| `GPIO 9` | OLED **SCL** | I2C Clock |

## Features
- **60 FPS Graphics Engine:** Fast raster rendering using `Adafruit_SSD1306`.
- **Persistent High Scores:** Saves scores using `Preferences` library (ESP32 NVS).
- **Multiple Obstacles:** Cacti & airborne pterodactyl birds.
- **Increasing Difficulty:** Speed ramps up smoothly as score increases.
