/*
  Project 02: BLE Media & Volume Controller
  Target MCU: ESP32-C3
  Display: 0.96" / 1.3" I2C OLED (SSD1306 128x64)
  
  Controls:
    - Button 1 (GPIO 0): Play / Pause
    - Button 2 (GPIO 1): Volume Up
    - Button 3 (GPIO 2): Volume Down
    - Button 4 (GPIO 3): Next Track / Mute (Hold)
    
  Required Libraries:
    - ESP32-BLE-Keyboard (https://github.com/T-vK/ESP32-BLE-Keyboard)
    - Adafruit_SSD1306 & Adafruit_GFX
*/

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define USE_NIMBLE
#include <NimBLEDevice.h>
#include <BleKeyboard.h>

#define SCREEN_WIDTH  128
#define SCREEN_HEIGHT 64
#define OLED_RESET    -1
#define OLED_ADDRESS  0x3C
#define SDA_PIN 8
#define SCL_PIN 9

// Button Pins (ESP32-C3 GPIOs with internal pull-up)
#define BTN_PLAY_PAUSE 0
#define BTN_VOL_UP     1
#define BTN_VOL_DOWN   2
#define BTN_NEXT       3

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
BleKeyboard bleKeyboard("C3 Media Remote", "ESP32-C3", 100);

String currentAction = "Ready";
unsigned long actionTime = 0;
bool isConnected = false;

void setup() {
  Serial.begin(115200);
  Wire.begin(SDA_PIN, SCL_PIN);

  // Configure Button Pins
  pinMode(BTN_PLAY_PAUSE, INPUT_PULLUP);
  pinMode(BTN_VOL_UP, INPUT_PULLUP);
  pinMode(BTN_VOL_DOWN, INPUT_PULLUP);
  pinMode(BTN_NEXT, INPUT_PULLUP);

  // Display Setup
  if (!display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDRESS)) {
    Serial.println("OLED Allocation Failed");
    for(;;);
  }

  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);
  display.setTextSize(1);
  display.setCursor(10, 20);
  display.println("ESP32-C3 BLE Remote");
  display.setCursor(10, 35);
  display.println("Starting BLE...");
  display.display();

  bleKeyboard.begin();
  Serial.println("BLE Keyboard Started. Pair with 'C3 Media Remote'");
}

void loop() {
  bool bleStatus = bleKeyboard.isConnected();
  
  if (bleStatus != isConnected) {
    isConnected = bleStatus;
    currentAction = isConnected ? "BLE Connected!" : "Pairing Mode...";
    actionTime = millis();
  }

  if (isConnected) {
    // Read Buttons
    if (digitalRead(BTN_PLAY_PAUSE) == LOW) {
      bleKeyboard.write(KEY_MEDIA_PLAY_PAUSE);
      currentAction = "PLAY / PAUSE";
      actionTime = millis();
      delay(250); // Debounce
    }
    else if (digitalRead(BTN_VOL_UP) == LOW) {
      bleKeyboard.write(KEY_MEDIA_VOLUME_UP);
      currentAction = "VOLUME UP +";
      actionTime = millis();
      delay(150);
    }
    else if (digitalRead(BTN_VOL_DOWN) == LOW) {
      bleKeyboard.write(KEY_MEDIA_VOLUME_DOWN);
      currentAction = "VOLUME DOWN -";
      actionTime = millis();
      delay(150);
    }
    else if (digitalRead(BTN_NEXT) == LOW) {
      bleKeyboard.write(KEY_MEDIA_NEXT_TRACK);
      currentAction = "NEXT TRACK >>";
      actionTime = millis();
      delay(250);
    }
  }

  // Clear action highlight after 2 seconds
  if (millis() - actionTime > 2000 && isConnected) {
    currentAction = "Idle / Active";
  }

  renderUI();
  delay(50);
}

void renderUI() {
  display.clearDisplay();

  // Header
  display.setTextSize(1);
  display.setCursor(0, 0);
  display.print("BLE REMOTE");
  
  display.setCursor(70, 0);
  if (isConnected) {
    display.print("[BT: ON]");
  } else {
    display.print("[BT: WAIT]");
  }
  display.drawFastHLine(0, 10, 128, SSD1306_WHITE);

  // Status Box
  display.drawRect(4, 16, 120, 32, SSD1306_WHITE);
  display.setTextSize(1);
  display.setCursor(12, 28);
  display.print(currentAction);

  // Button Legend Footer
  display.drawFastHLine(0, 52, 128, SSD1306_WHITE);
  display.setCursor(2, 55);
  display.print("P1:Play  P2:+  P3:-  P4:Next");

  display.display();
}
