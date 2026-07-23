/*
  Project 04: Environmental Monitoring Station & Gauge Dashboard
  Target MCU: ESP32-C3
  Display: 0.96" / 1.3" I2C OLED (SSD1306 128x64)
  Sensors: DHT22 / DHT11 (or BME280) on GPIO 4 with auto fallback simulation
  
  Controls:
    - Button (GPIO 0): Cycle screen views (Overview, Temp Gauge, Humidity Gauge, Air Quality)
    
  Required Libraries:
    - Adafruit_SSD1306 & Adafruit_GFX
    - DHT sensor library by Adafruit
*/

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <DHT.h>

#define SCREEN_WIDTH  128
#define SCREEN_HEIGHT 64
#define OLED_RESET    -1
#define OLED_ADDRESS  0x3C
#define SDA_PIN 8
#define SCL_PIN 9

#define DHTPIN 4
#define DHTTYPE DHT22

#define BTN_CYCLE 0

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
DHT dht(DHTPIN, DHTTYPE);

float tempC = 23.5;
float humidity = 48.0;
float heatIndex = 24.1;
int airQualityIndex = 42; // AQI 0-500 scale
int screenPage = 0; // 0: Overview, 1: Temp Gauge, 2: Humidity Gauge, 3: AQI

void setup() {
  Serial.begin(115200);
  Wire.begin(SDA_PIN, SCL_PIN);
  pinMode(BTN_CYCLE, INPUT_PULLUP);

  if (!display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDRESS)) {
    Serial.println("OLED Init Failed");
    for(;;);
  }

  dht.begin();

  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);
  display.setTextSize(1);
  display.setCursor(10, 25);
  display.println("ENV Station Booting");
  display.setCursor(10, 40);
  display.println("Initializing DHT22...");
  display.display();
  delay(1500);
}

void loop() {
  // Read Button to cycle screen
  if (digitalRead(BTN_CYCLE) == LOW) {
    screenPage = (screenPage + 1) % 4;
    delay(200); // Debounce
  }

  // Read Sensor (or simulate slight variations if disconnected)
  float t = dht.readTemperature();
  float h = dht.readHumidity();

  if (!isnan(t) && !isnan(h)) {
    tempC = t;
    humidity = h;
    heatIndex = dht.computeHeatIndex(tempC, humidity, false);
  } else {
    // Simulated smooth movement for testing
    tempC += (random(-5, 6) / 10.0);
    if (tempC < 15.0) tempC = 15.0;
    if (tempC > 38.0) tempC = 38.0;

    humidity += (random(-8, 9) / 10.0);
    if (humidity < 20.0) humidity = 20.0;
    if (humidity > 90.0) humidity = 90.0;
  }

  renderPage();
  delay(100);
}

void drawProgressBar(int x, int y, int w, int h, float percent) {
  display.drawRect(x, y, w, h, SSD1306_WHITE);
  int fillWidth = (int)((w - 4) * (percent / 100.0));
  if (fillWidth > w - 4) fillWidth = w - 4;
  if (fillWidth < 0) fillWidth = 0;
  display.fillRect(x + 2, y + 2, fillWidth, h - 4, SSD1306_WHITE);
}

void renderPage() {
  display.clearDisplay();

  // Header Bar
  display.setTextSize(1);
  display.setCursor(0, 0);
  display.print("ENV MONITOR");
  display.setCursor(85, 0);
  display.printf("P:%d/4", screenPage + 1);
  display.drawFastHLine(0, 10, 128, SSD1306_WHITE);

  if (screenPage == 0) { // Overview Page
    display.setTextSize(1);
    display.setCursor(0, 16);
    display.printf("Temp:    %.1f C", tempC);
    display.setCursor(0, 28);
    display.printf("Humid:   %.1f %%", humidity);
    display.setCursor(0, 40);
    display.printf("HeatIdx: %.1f C", heatIndex);
    
    display.setCursor(0, 53);
    if (humidity > 70) display.print("Status: Humid / Damp");
    else if (humidity < 30) display.print("Status: Dry Air");
    else display.print("Status: Comfort Ideal");

  } else if (screenPage == 1) { // Temp Gauge Page
    display.setTextSize(1);
    display.setCursor(0, 16);
    display.print("TEMPERATURE GAUGE");
    display.setTextSize(2);
    display.setCursor(35, 28);
    display.printf("%.1f C", tempC);

    // Temp range 0 to 50 C
    float tempPct = (tempC / 50.0) * 100.0;
    drawProgressBar(10, 50, 108, 10, tempPct);

  } else if (screenPage == 2) { // Humidity Gauge Page
    display.setTextSize(1);
    display.setCursor(0, 16);
    display.print("HUMIDITY LEVEL");
    display.setTextSize(2);
    display.setCursor(35, 28);
    display.printf("%.1f %%", humidity);

    drawProgressBar(10, 50, 108, 10, humidity);

  } else if (screenPage == 3) { // Air Quality Index Page
    display.setTextSize(1);
    display.setCursor(0, 16);
    display.print("AIR QUALITY (AQI)");
    display.setTextSize(2);
    display.setCursor(40, 28);
    display.printf("%d", airQualityIndex);

    float aqiPct = (airQualityIndex / 200.0) * 100.0;
    drawProgressBar(10, 50, 108, 10, aqiPct);
  }

  display.display();
}
