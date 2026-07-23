/*
  Project 05: Retro Dino Runner Arcade Game
  Target MCU: ESP32-C3
  Display: 0.96" / 1.3" I2C OLED (SSD1306 128x64)
  
  Controls:
    - Button 1 (GPIO 0): Jump / Start Game
    - Button 2 (GPIO 1): Duck / Restart Game
    
  Required Libraries:
    - Adafruit_SSD1306 & Adafruit_GFX
    - Preferences (Included in ESP32 Arduino Core)
*/

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Preferences.h>

#define SCREEN_WIDTH  128
#define SCREEN_HEIGHT 64
#define OLED_RESET    -1
#define OLED_ADDRESS  0x3C
#define SDA_PIN 8
#define SCL_PIN 9

#define BTN_JUMP 0
#define BTN_DUCK 1

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
Preferences prefs;

// Dino Constants & Variables
const int GROUND_Y = 54;
float dinoY = GROUND_Y - 14;
float dinoVelocityY = 0;
bool isJumping = false;
bool isDucking = false;
const float GRAVITY = 0.9;
const float JUMP_FORCE = -8.5;

// Obstacle Structure
struct Obstacle {
  float x;
  int width;
  int height;
  bool isBird;
  int y;
  bool active;
};

Obstacle obstacles[2];
float gameSpeed = 3.5;
int score = 0;
int highScore = 0;
bool gameState = 0; // 0: Start Screen, 1: Playing, 2: Game Over

// Bitmaps (Dino 12x14, Cactus 8x12)
const uint8_t PROGMEM dino_bmp[] = {
  0b00000111, 0b11000000,
  0b00000111, 0b11100000,
  0b00000111, 0b11100000,
  0b00000111, 0b11000000,
  0b00000111, 0b11110000,
  0b00001111, 0b11000000,
  0b00011111, 0b11100000,
  0b00111111, 0b11110000,
  0b01111111, 0b11100000,
  0b01111111, 0b11000000,
  0b00111111, 0b00000000,
  0b00011001, 0b10000000,
  0b00011000, 0b10000000,
  0b00010000, 0b01000000
};

const uint8_t PROGMEM cactus_bmp[] = {
  0b00011000,
  0b00011000,
  0b01011010,
  0b01011010,
  0b01111110,
  0b00011000,
  0b00011000,
  0b00011000,
  0b00011000,
  0b00011000,
  0b00011000,
  0b00011000
};

void setup() {
  Serial.begin(115200);
  Wire.begin(SDA_PIN, SCL_PIN);
  pinMode(BTN_JUMP, INPUT_PULLUP);
  pinMode(BTN_DUCK, INPUT_PULLUP);

  if (!display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDRESS)) {
    Serial.println("OLED Failed");
    for(;;);
  }

  // Load High Score from NVS
  prefs.begin("dino_game", false);
  highScore = prefs.getInt("highscore", 0);

  resetGame();
}

void resetGame() {
  dinoY = GROUND_Y - 14;
  dinoVelocityY = 0;
  isJumping = false;
  isDucking = false;
  score = 0;
  gameSpeed = 3.5;

  obstacles[0] = {130.0, 8, 12, false, GROUND_Y - 12, true};
  obstacles[1] = {200.0, 8, 12, false, GROUND_Y - 12, true};
}

void loop() {
  if (gameState == 0) { // Start Screen
    renderStartScreen();
    if (digitalRead(BTN_JUMP) == LOW || digitalRead(BTN_DUCK) == LOW) {
      resetGame();
      gameState = 1;
      delay(200);
    }
  } 
  else if (gameState == 1) { // Gameplay
    updatePhysics();
    renderGame();
  } 
  else if (gameState == 2) { // Game Over
    renderGameOver();
    if (digitalRead(BTN_JUMP) == LOW || digitalRead(BTN_DUCK) == LOW) {
      resetGame();
      gameState = 1;
      delay(200);
    }
  }

  delay(20); // ~50 FPS
}

void updatePhysics() {
  // Input Handling
  if (digitalRead(BTN_JUMP) == LOW && !isJumping) {
    dinoVelocityY = JUMP_FORCE;
    isJumping = true;
  }
  
  isDucking = (digitalRead(BTN_DUCK) == LOW && !isJumping);

  // Apply Gravity
  dinoY += dinoVelocityY;
  dinoVelocityY += GRAVITY;

  if (dinoY >= GROUND_Y - 14) {
    dinoY = GROUND_Y - 14;
    dinoVelocityY = 0;
    isJumping = false;
  }

  // Move Obstacles
  for (int i = 0; i < 2; i++) {
    obstacles[i].x -= gameSpeed;
    if (obstacles[i].x < -10) {
      obstacles[i].x = 130 + random(20, 60);
      obstacles[i].isBird = (random(0, 5) == 0);
      obstacles[i].y = obstacles[i].isBird ? (GROUND_Y - 22) : (GROUND_Y - 12);
    }

    // Collision Detection (Bounding Box)
    int dinoX = 12;
    int dinoH = isDucking ? 8 : 14;
    int dinoYPos = isDucking ? (GROUND_Y - 8) : (int)dinoY;

    if (dinoX + 10 > obstacles[i].x && dinoX < obstacles[i].x + obstacles[i].width) {
      if (dinoYPos + dinoH > obstacles[i].y) {
        gameState = 2; // Game Over
        if (score > highScore) {
          highScore = score;
          prefs.putInt("highscore", highScore);
        }
      }
    }
  }

  score++;
  gameSpeed += 0.001; // Gradually increase difficulty
}

void renderStartScreen() {
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(20, 15);
  display.print("C3 DINO RUNNER");
  display.setCursor(15, 32);
  display.print("Press Jump to Play");
  display.setCursor(25, 48);
  display.printf("High Score: %d", highScore);
  display.display();
}

void renderGameOver() {
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(35, 12);
  display.print("GAME OVER");
  display.setCursor(20, 28);
  display.printf("Score: %d", score);
  display.setCursor(20, 40);
  display.printf("Best:  %d", highScore);
  display.setCursor(10, 52);
  display.print("Press Button to Retry");
  display.display();
}

void renderGame() {
  display.clearDisplay();

  // Draw Ground Line
  display.drawFastHLine(0, GROUND_Y, 128, SSD1306_WHITE);

  // Draw Dino
  if (!isDucking) {
    display.drawBitmap(12, (int)dinoY, dino_bmp, 12, 14, SSD1306_WHITE);
  } else {
    display.fillRect(12, GROUND_Y - 8, 14, 8, SSD1306_WHITE); // Ducking sprite box
  }

  // Draw Obstacles
  for (int i = 0; i < 2; i++) {
    if (!obstacles[i].isBird) {
      display.drawBitmap((int)obstacles[i].x, obstacles[i].y, cactus_bmp, 8, 12, SSD1306_WHITE);
    } else {
      // Draw Bird
      display.drawTriangle((int)obstacles[i].x, obstacles[i].y + 4,
                           (int)obstacles[i].x + 8, obstacles[i].y,
                           (int)obstacles[i].x + 8, obstacles[i].y + 8, SSD1306_WHITE);
    }
  }

  // Score HUD
  display.setTextSize(1);
  display.setCursor(75, 0);
  display.printf("%05d", score);

  display.display();
}
