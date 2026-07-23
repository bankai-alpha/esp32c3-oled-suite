# Project 02: Bluetooth BLE Media & Volume Controller

## Overview
Transform your **ESP32-C3** into a wireless Bluetooth LE (BLE) Human Interface Device (HID) Media Remote. You can control media playback (Play/Pause, Volume Up, Volume Down, Next Track) on your PC, Mac, Android, or iOS device while viewing real-time status on the OLED screen.

## Hardware Pinout & Wiring

| ESP32-C3 Pin | Connected Component | Function |
| :--- | :--- | :--- |
| `GPIO 0` | Push Button 1 | Play / Pause |
| `GPIO 1` | Push Button 2 | Volume Up (+) |
| `GPIO 2` | Push Button 3 | Volume Down (-) |
| `GPIO 3` | Push Button 4 | Next Track (>>) |
| `GPIO 8` | OLED **SDA** | I2C Data |
| `GPIO 9` | OLED **SCL** | I2C Clock |

*Note: Connect the other leg of each push button to `GND`. Internal pull-ups are enabled in software.*

## Setup Instructions

1. Install the `ESP32-BLE-Keyboard` library in Arduino IDE:
   - Download `.zip` from GitHub repository: `https://github.com/T-vK/ESP32-BLE-Keyboard`
2. Compile and flash `02_ble_media_controller.ino` to your ESP32-C3.
3. Turn on Bluetooth on your Phone or PC and pair with **"C3 Media Remote"**.
