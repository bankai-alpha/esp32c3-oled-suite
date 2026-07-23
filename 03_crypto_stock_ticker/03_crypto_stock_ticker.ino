/*
  Project 03: Desktop Crypto & Stock Price Ticker
  Target MCU: ESP32-C3
  Display: 0.96" / 1.3" I2C OLED (SSD1306 128x64)
  
  Features:
    - WiFi connection & HTTPS fetch from CoinGecko API
    - Rotates between Bitcoin (BTC), Ethereum (ETH), and Solana (SOL)
    - Displays current USD price, 24h change %
    - Draws dynamic mini sparkline chart on OLED screen
    
  Required Libraries:
    - Adafruit_SSD1306 & Adafruit_GFX
    - ArduinoJson
    - WiFi & HTTPClient
*/

#include <WiFi.h>
#include <HTTPClient.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <ArduinoJson.h>

const char* WIFI_SSID     = "YOUR_WIFI_SSID";
const char* WIFI_PASSWORD = "YOUR_WIFI_PASSWORD";

#define SCREEN_WIDTH  128
#define SCREEN_HEIGHT 64
#define OLED_RESET    -1
#define OLED_ADDRESS  0x3C
#define SDA_PIN 8
#define SCL_PIN 9

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

struct CryptoData {
  String name;
  String symbol;
  float price;
  float change24h;
  float priceHistory[16];
};

CryptoData cryptoList[3] = {
  {"Bitcoin", "BTC", 64500.0, 2.45, {63000, 63200, 63500, 63400, 63800, 64000, 64200, 64500, 64100, 64300, 64600, 64500}},
  {"Ethereum", "ETH", 3450.0, -1.12, {3520, 3510, 3490, 3480, 3460, 3450, 3470, 3460, 3440, 3450, 3460, 3450}},
  {"Solana", "SOL", 145.8, 5.80, {138, 139, 140, 142, 141, 143, 144, 146, 145, 147, 146, 145.8}}
};

int currentCoinIndex = 0;
unsigned long lastFetchTime = 0;
unsigned long lastRotateTime = 0;

void setup() {
  Serial.begin(115200);
  Wire.begin(SDA_PIN, SCL_PIN);

  if (!display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDRESS)) {
    Serial.println("OLED Failed");
    for(;;);
  }

  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);
  display.setTextSize(1);
  display.setCursor(15, 25);
  display.println("Crypto Ticker C3");
  display.setCursor(15, 40);
  display.println("Connecting WiFi...");
  display.display();

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  int count = 0;
  while (WiFi.status() != WL_CONNECTED && count < 15) {
    delay(500);
    count++;
  }

  if (WiFi.status() == WL_CONNECTED) {
    fetchCryptoPrices();
  }
}

void loop() {
  if (millis() - lastRotateTime > 5000) { // Rotate coin every 5s
    currentCoinIndex = (currentCoinIndex + 1) % 3;
    lastRotateTime = millis();
  }

  if (WiFi.status() == WL_CONNECTED && (millis() - lastFetchTime > 60000)) { // Update every 60s
    fetchCryptoPrices();
  }

  renderScreen();
  delay(100);
}

void fetchCryptoPrices() {
  HTTPClient http;
  String url = "https://api.coingecko.com/api/v3/simple/price?ids=bitcoin,ethereum,solana&vs_currencies=usd&include_24hr_change=true";
  
  http.begin(url);
  int httpCode = http.GET();

  if (httpCode == HTTP_CODE_OK) {
    String payload = http.getString();
    DynamicJsonDocument doc(1024);
    deserializeJson(doc, payload);

    cryptoList[0].price = doc["bitcoin"]["usd"];
    cryptoList[0].change24h = doc["bitcoin"]["usd_24h_change"];

    cryptoList[1].price = doc["ethereum"]["usd"];
    cryptoList[1].change24h = doc["ethereum"]["usd_24h_change"];

    cryptoList[2].price = doc["solana"]["usd"];
    cryptoList[2].change24h = doc["solana"]["usd_24h_change"];

    // Push new prices to sparkline history
    for(int i = 0; i < 3; i++) {
      for(int j = 0; j < 15; j++) {
        cryptoList[i].priceHistory[j] = cryptoList[i].priceHistory[j+1];
      }
      cryptoList[i].priceHistory[15] = cryptoList[i].price;
    }
    lastFetchTime = millis();
  }
  http.end();
}

void renderScreen() {
  display.clearDisplay();
  CryptoData coin = cryptoList[currentCoinIndex];

  // Header: Symbol & Name
  display.setTextSize(1);
  display.setCursor(0, 0);
  display.printf("%s (%s)", coin.name.c_str(), coin.symbol.c_str());

  // 24h Change %
  display.setCursor(85, 0);
  if (coin.change24h >= 0) {
    display.printf("+%.1f%%", coin.change24h);
  } else {
    display.printf("%.1f%%", coin.change24h);
  }
  display.drawFastHLine(0, 10, 128, SSD1306_WHITE);

  // Big Price Display
  display.setTextSize(2);
  display.setCursor(0, 16);
  if (coin.price > 1000) {
    display.printf("$%.0f", coin.price);
  } else {
    display.printf("$%.2f", coin.price);
  }

  // Draw Mini Sparkline Chart at Bottom (Y: 40 to 63, X: 0 to 127)
  float minP = coin.priceHistory[0];
  float maxP = coin.priceHistory[0];
  for(int i = 1; i < 16; i++) {
    if (coin.priceHistory[i] < minP) minP = coin.priceHistory[i];
    if (coin.priceHistory[i] > maxP) maxP = coin.priceHistory[i];
  }
  if (maxP == minP) maxP += 1.0;

  int chartXStart = 0;
  int chartWidth = 127;
  int chartYBottom = 63;
  int chartHeight = 22;

  for (int i = 0; i < 15; i++) {
    int x1 = chartXStart + (i * (chartWidth / 15));
    int y1 = chartYBottom - (int)(((coin.priceHistory[i] - minP) / (maxP - minP)) * chartHeight);
    int x2 = chartXStart + ((i + 1) * (chartWidth / 15));
    int y2 = chartYBottom - (int)(((coin.priceHistory[i+1] - minP) / (maxP - minP)) * chartHeight);

    display.drawLine(x1, y1, x2, y2, SSD1306_WHITE);
  }

  display.display();
}
