#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Fonts/FreeSans9pt7b.h>      
#include <Fonts/FreeSansBold9pt7b.h>  

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET    -1 
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

#define BTN_UP      4
#define BTN_DOWN    3
#define BTN_TRIGGER 2

struct Rect {
  int x; int y; int w; int h;
};

const unsigned char PROGMEM dioda16 [] = {
  0x00, 0x00, 0x00, 0x00, 0x1C, 0x00, 0x3F, 0xF0, 0x3C, 0x00, 0x3C, 0x00, 0xFF, 0x00, 0x7F, 0xFF,
  0x7F, 0xFF, 0xFF, 0x00, 0x3C, 0x00, 0x3C, 0x00, 0x1F, 0xF0, 0x1C, 0x00, 0x00, 0x00, 0x00, 0x00
};

static const unsigned char PROGMEM dino_bmp[]={
  0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0xfe, 0x00, 0x00, 0x06, 0xff, 0x00, 0x00, 0x0e, 0xff, 0x00, 
  0x00, 0x0f, 0xff, 0x00, 0x00, 0x0f, 0xff, 0x00, 0x00, 0x0f, 0xff, 0x00, 0x00, 0x0f, 0xc0, 0x00, 
  0x00, 0x0f, 0xfc, 0x00, 0x40, 0x0f, 0xc0, 0x00, 0x40, 0x1f, 0x80, 0x00, 0x40, 0x7f, 0x80, 0x00, 
  0x60, 0xff, 0xe0, 0x00, 0x71, 0xff, 0xa0, 0x00, 0x7f, 0xff, 0x80, 0x00, 0x7f, 0xff, 0x80, 0x00, 
  0x7f, 0xff, 0x80, 0x00, 0x3f, 0xff, 0x00, 0x00, 0x1f, 0xff, 0x00, 0x00, 0x0f, 0xfe, 0x00, 0x00, 
  0x03, 0xfc, 0x00, 0x00, 0x01, 0xdc, 0x00, 0x00, 0x01, 0x8c, 0x00, 0x00, 0x01, 0x8c, 0x00, 0x00, 
  0x01, 0x0c, 0x00, 0x00, 0x01, 0x8e, 0x00, 0x00
};

static const unsigned char PROGMEM tree1_bmp[]={
  0x1e, 0x00, 0x1f, 0x00, 0x1f, 0x40, 0x1f, 0xe0, 0x1f, 0xe0, 0xdf, 0xe0, 0xff, 0xe0, 0xff, 0xe0, 
  0xff, 0xe0, 0xff, 0xe0, 0xff, 0xe0, 0xff, 0xe0, 0xff, 0xc0, 0xff, 0x00, 0xff, 0x00, 0x7f, 0x00, 
  0x1f, 0x00, 0x1f, 0x00, 0x1f, 0x00, 0x1f, 0x00, 0x1f, 0x00, 0x1f, 0x00, 0x1f, 0x00
};

static const unsigned char PROGMEM tree2_bmp[]={
  0x1e, 0x01, 0xe0, 0x1f, 0x03, 0xe0, 0x1f, 0x4f, 0xe8, 0x1f, 0xff, 0xfc, 0x1f, 0xff, 0xfc, 0xdf, 
  0xff, 0xfc, 0xff, 0xff, 0xfc, 0xff, 0xff, 0xfc, 0xff, 0xff, 0xfc, 0xff, 0xff, 0xfc, 0xff, 0xff, 
  0xfc, 0xff, 0xef, 0xfc, 0xff, 0x83, 0xfc, 0xff, 0x03, 0xfc, 0xff, 0x03, 0xf8, 0x7f, 0x03, 0xe0, 
  0x1f, 0x03, 0xe0, 0x1f, 0x03, 0xe0, 0x1f, 0x03, 0xe0, 0x1f, 0x03, 0xe0, 0x1f, 0x03, 0xe0, 0x1f, 
  0x03, 0xe0, 0x1f, 0x03, 0xe0
};

#define LOGO_HEIGHT   16
#define LOGO_WIDTH    16
#define NUMFLAKES     10
static const unsigned char PROGMEM snowflake_bmp[] = { 
  0b00000000, 0b11000000, 0b00000001, 0b11000000, 0b00000001, 0b11000000, 0b00000011, 0b11100000,
  0b11110011, 0b11100000, 0b11111110, 0b11111000, 0b01111110, 0b11111111, 0b00110011, 0b10011111,
  0b00011111, 0b11111100, 0b00001101, 0b01110000, 0b00011011, 0b10100000, 0b00111111, 0b11100000,
  0b00111111, 0b11110000, 0b01111100, 0b11110000, 0b01110000, 0b01110000, 0b00000000, 0b00110000 
};

enum State { ANIMATION, MENU, GAME_DEATHSTAR, GAME_DINO, GAME_OVER };
State currentState = ANIMATION; 
State previousGame = GAME_DEATHSTAR;

int menuSelection = 0; 
unsigned long lastDebounceTime = 0;
const int debounceDelay = 200;

int flakes[NUMFLAKES][3]; 
unsigned long lastFrameTime = 0;

#define DINO_WIDTH 25
#define DINO_HEIGHT 26
#define DINO_BASE_Y 35 
#define TREE1_WIDTH 11
#define TREE1_HEIGHT 23
#define TREE2_WIDTH 22
#define TREE2_HEIGHT 23

float dinoY = DINO_BASE_Y;
float dinoVel = 0;
bool isJumping = false;
float gravity = 0.6;
float jumpStrength = -8.0;
int obstacleX = 128;
int obstacleType = 0; 
int dino_score = 0;
unsigned long dino_last_update = 0;

int bulletX = 0, bulletY = 0, bulletActive = 0, enemyY = 8, enemyDirection = 0;
int rx = 200, ry = 0, rx2 = 200, ry2 = 0, rx3 = 200, ry3 = 0, rx4 = 200, ry4 = 0; 
int ds_score = 0; 
int ds_lives = 5; 
int ds_level = 1; 
int playerY = 30; 
int enemySpeed = 3, bulletCount = 1, enemyRadius = 10, bulletsFired = 0, isFiringSequence = 0;
int enemyX = 95;
unsigned long firingTimestamp = 0, randomDelay = 0, levelTimestamp = 0;

bool checkCollision(Rect r1, Rect r2);
void resetDeathStar();
void resetDino();
void waitForRelease();

void setup() {
  Serial.begin(9600);
  pinMode(BTN_UP, INPUT_PULLUP);
  pinMode(BTN_DOWN, INPUT_PULLUP);
  pinMode(BTN_TRIGGER, INPUT_PULLUP);

  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { 
    for(;;); 
  }

  for(int f=0; f<NUMFLAKES; f++) {
    flakes[f][0] = random(0, SCREEN_WIDTH);
    flakes[f][1] = random(-20, 0);
    flakes[f][2] = random(1, 3);
  }
  
  display.clearDisplay();
  display.display();
}

void loop() {
  switch(currentState) {
    case ANIMATION: runAnimation(); break;
    case MENU: runMenu(); break;
    case GAME_DEATHSTAR: runDeathStar(); break;
    case GAME_DINO: runDino(); break;
    case GAME_OVER: runGameOver(); break;
  }
}

void runAnimation() {
  if (millis() - lastFrameTime > 40) {
    lastFrameTime = millis();
    display.clearDisplay();

    display.setFont(&FreeSans9pt7b);
    display.setTextColor(WHITE);
    display.setCursor(5, 20);  display.println("Kukuchiiiiiiiii!");
    display.setCursor(35, 40); display.println("Happy");
    display.setCursor(25, 60); display.println("Birthday");

    for(int f=0; f<NUMFLAKES; f++) {
      display.drawBitmap(flakes[f][0], flakes[f][1], snowflake_bmp, LOGO_WIDTH, LOGO_HEIGHT, 1);
      flakes[f][1] += flakes[f][2];
      if (flakes[f][1] > SCREEN_HEIGHT) {
        flakes[f][1] = -LOGO_HEIGHT;
        flakes[f][0] = random(0, SCREEN_WIDTH);
      }
    }
    display.display();
  }
  
  if (digitalRead(BTN_TRIGGER) == LOW) {
    waitForRelease();
    currentState = MENU;
  }
}

void runMenu() {
  display.clearDisplay();
  
  int itemHeight = 18;
  int startY = 35; 

  if (menuSelection > 0) {
    startY = 20; 
  }

  if (menuSelection == 0) {
    display.setFont(&FreeSansBold9pt7b);
    display.setTextColor(WHITE);
    display.setCursor(40, 15);
    display.println("MENU");
    display.drawLine(0, 18, 128, 18, WHITE);
  }

  int listTopY = startY;
  if (menuSelection > 0) {
     listTopY = 35 - (menuSelection * itemHeight); 
  }

  display.setFont(&FreeSans9pt7b); 

  int y0 = listTopY;
  if (y0 > 10 && y0 < 60) {
    if (menuSelection == 0) {
       display.fillRect(5, y0 - 11, 118, 15, WHITE);
       display.setTextColor(BLACK);
       display.setFont(&FreeSansBold9pt7b);
    } else display.setTextColor(WHITE);
    display.setCursor(28, y0); display.println("HOME");
    display.setFont(&FreeSans9pt7b); 
  }

  int y1 = listTopY + itemHeight;
  if (y1 > 10 && y1 < 60) {
    if (menuSelection == 1) {
       display.fillRect(5, y1 - 11, 118, 15, WHITE);
       display.setTextColor(BLACK);
       display.setFont(&FreeSansBold9pt7b);
    } else display.setTextColor(WHITE);
    display.setCursor(22, y1); display.println("DINO RUN");
    display.setFont(&FreeSans9pt7b); 
  }

  int y2 = listTopY + (itemHeight * 2);
  if (y2 > 10 && y2 < 60) {
    if (menuSelection == 2) {
       display.fillRect(5, y2 - 11, 118, 15, WHITE);
       display.setTextColor(BLACK);
       display.setFont(&FreeSansBold9pt7b);
    } else display.setTextColor(WHITE);
    display.setCursor(15, y2); display.println("DEATHSTAR");
    display.setFont(&FreeSans9pt7b); 
  }

  display.display();

  if (millis() - lastDebounceTime > debounceDelay) {
    if (digitalRead(BTN_DOWN) == LOW) {
      menuSelection++;
      if (menuSelection > 2) menuSelection = 0;
      lastDebounceTime = millis();
    }
    if (digitalRead(BTN_UP) == LOW) {
      menuSelection--;
      if (menuSelection < 0) menuSelection = 2;
      lastDebounceTime = millis();
    }
    if (digitalRead(BTN_TRIGGER) == LOW) {
      waitForRelease(); 
      lastDebounceTime = millis();
      if (menuSelection == 0) currentState = ANIMATION;
      if (menuSelection == 1) { resetDino(); currentState = GAME_DINO; }
      if (menuSelection == 2) { resetDeathStar(); currentState = GAME_DEATHSTAR; }
    }
  }
}

void resetDeathStar() {
  ds_score = 0; ds_lives = 5; ds_level = 1; playerY = 30;
  bulletX = 0; bulletY = 0; bulletActive = 0; 
  enemyY = 8; enemyDirection = 0;
  enemySpeed = 3; bulletCount = 1; enemyRadius = 10; bulletsFired = 0; isFiringSequence = 0;
  enemyX = 95;
  rx = 200; ry = 0; rx2 = 200; ry2 = 0; rx3 = 200; ry3 = 0; rx4 = 200; ry4 = 0;
  levelTimestamp = millis();
}

void runDeathStar() {
  unsigned long currentTime = millis();
  display.clearDisplay();
  
  display.drawPixel(50, 30, 1); display.drawPixel(30, 17, 1);
  display.drawPixel(60, 18, 1); display.drawPixel(25, 43, 1);

  if ((currentTime - levelTimestamp) > 50000) {
    levelTimestamp = currentTime;
    ds_level++;
    enemySpeed++;
    if (ds_level % 2 == 0) { bulletCount++; enemyRadius = max(4, enemyRadius - 1); }
  }

  if (isFiringSequence == 0) {
    firingTimestamp = millis();
    randomDelay = random(400, 1200);
    isFiringSequence = 1;
  }
  if ((randomDelay + firingTimestamp) < currentTime) {
    isFiringSequence = 0; bulletsFired++;
    if (bulletsFired == 1) { rx = 95; ry = enemyY; }
    if (bulletsFired == 2) { rx2 = 95; ry2 = enemyY; }
    if (bulletsFired == 3) { rx3 = 95; ry3 = enemyY; }
    if (bulletsFired == 4) { rx4 = 95; ry4 = enemyY; }
  }

  if (bulletsFired > 0) { display.drawCircle(rx, ry, 2, 1); rx -= enemySpeed; }
  if (bulletsFired > 1) { display.drawCircle(rx2, ry2, 1, 1); rx2 -= enemySpeed; }
  if (bulletsFired > 2) { display.drawCircle(rx3, ry3, 4, 1); rx3 -= enemySpeed; }
  if (bulletsFired > 3) { display.drawCircle(rx4, ry4, 2, 1); rx4 -= enemySpeed; }

  if (digitalRead(BTN_UP) == LOW && playerY >= 2) playerY -= 2;
  if (digitalRead(BTN_DOWN) == LOW && playerY <= 46) playerY += 2;
  if (digitalRead(BTN_TRIGGER) == LOW && bulletActive == 0) {
    bulletActive = 1; bulletX = 6; bulletY = playerY + 8;
  }

  if (bulletActive == 1) {
    bulletX += 8;
    display.drawLine(bulletX, bulletY, bulletX + 4, bulletY, 1);
    if (bulletX > 128) bulletActive = 0;
  }

  display.drawBitmap(4, playerY, dioda16, 16, 16, 1);
  display.fillCircle(enemyX, enemyY, enemyRadius, 1);
  display.fillCircle(enemyX + 2, enemyY + 3, enemyRadius / 3, 0);

  display.setFont(); 
  display.setTextColor(WHITE); 
  display.setTextSize(1);
  display.setCursor(33, 57); display.print("Score:"); display.print(ds_score);
  display.setCursor(33, 0);  display.print("Lives:"); display.print(ds_lives);
  display.setCursor(110, 0); display.print("L:"); display.print(ds_level);

  if (enemyDirection == 0) enemyY += bulletCount; else enemyY -= bulletCount;
  if (enemyY >= (64 - enemyRadius)) enemyDirection = 1;
  if (enemyY <= enemyRadius) enemyDirection = 0;

  if (bulletY >= enemyY - enemyRadius && bulletY <= enemyY + enemyRadius) {
    if (bulletX > (enemyX - enemyRadius) && bulletX < (enemyX + enemyRadius)) {
      ds_score++; bulletActive = 0; bulletX = -20;
    }
  }

  int pMid = playerY + 8;
  if (rx < 12 && rx > 4 && ry >= pMid - 8 && ry <= pMid + 8) { ds_lives--; rx = -50; }
  if (rx2 < 12 && rx2 > 4 && ry2 >= pMid - 8 && ry2 <= pMid + 8) { ds_lives--; rx2 = -50; }
  if (rx3 < 12 && rx3 > 4 && ry3 >= pMid - 8 && ry3 <= pMid + 8) { ds_lives--; rx3 = -50; }
  if (rx4 < 12 && rx4 > 4 && ry4 >= pMid - 8 && ry4 <= pMid + 8) { ds_lives--; rx4 = -50; bulletsFired = 0; }
  
  if (rx4 < 1 && bulletsFired >=4) { bulletsFired = 0; rx4 = 200; }
  
  if (ds_lives <= 0) {
    previousGame = GAME_DEATHSTAR;
    currentState = GAME_OVER;
  }
  
  display.display();
}

void resetDino() {
  dinoY = DINO_BASE_Y;
  dinoVel = 0;
  isJumping = false;
  obstacleX = SCREEN_WIDTH + 50; 
  dino_score = 0;
  dino_last_update = millis();
}

void runDino() {
  if (millis() - dino_last_update < 20) return; 
  dino_last_update = millis();

  display.clearDisplay();

  display.drawPixel(5, 5, WHITE);
  display.drawPixel(25, 12, WHITE);
  display.drawPixel(45, 4, WHITE);
  display.drawPixel(65, 15, WHITE);
  display.drawPixel(85, 6, WHITE);
  display.drawPixel(105, 10, WHITE);
  display.drawPixel(125, 3, WHITE);
  display.drawPixel(15, 25, WHITE);
  display.drawPixel(55, 22, WHITE);
  display.drawPixel(95, 28, WHITE);

  display.setFont();
  display.setTextColor(WHITE);
  display.setTextSize(1);
  display.setCursor(80, 0); display.print("Score:"); display.print(dino_score);

  if ((digitalRead(BTN_UP) == LOW || digitalRead(BTN_TRIGGER) == LOW) && !isJumping) {
    isJumping = true;
    dinoVel = jumpStrength;
  }

  if (isJumping) {
    dinoY += dinoVel;
    dinoVel += gravity;
    if (dinoY >= DINO_BASE_Y) {
      dinoY = DINO_BASE_Y;
      isJumping = false;
      dinoVel = 0;
    }
  }

  display.drawLine(0, DINO_BASE_Y + DINO_HEIGHT, SCREEN_WIDTH, DINO_BASE_Y + DINO_HEIGHT, WHITE);
  display.drawBitmap(10, (int)dinoY, dino_bmp, DINO_WIDTH, DINO_HEIGHT, 1);

  obstacleX -= 4; 
  if (obstacleX < -25) {
    obstacleX = SCREEN_WIDTH;
    obstacleType = random(0, 2);
    dino_score++;
  }

  int obsW = (obstacleType == 0) ? TREE1_WIDTH : TREE2_WIDTH;
  int obsH = (obstacleType == 0) ? TREE1_HEIGHT : TREE2_HEIGHT;
  int treeY = (DINO_BASE_Y + DINO_HEIGHT) - obsH;

  if (obstacleType == 0) display.drawBitmap(obstacleX, treeY, tree1_bmp, obsW, obsH, 1);
  else display.drawBitmap(obstacleX, treeY, tree2_bmp, obsW, obsH, 1);

  Rect dinoRect = { 15, (int)dinoY + 4, DINO_WIDTH - 10, DINO_HEIGHT - 8 };
  Rect obsRect = { obstacleX + 2, treeY + 2, obsW - 4, obsH - 4 };

  if (checkCollision(dinoRect, obsRect)) {
    previousGame = GAME_DINO;
    currentState = GAME_OVER;
  }

  display.display();
}

void runGameOver() {
  display.clearDisplay();
  
  display.setFont(&FreeSansBold9pt7b);
  display.setTextColor(WHITE);
  
  display.setCursor(38, 20); 
  display.println("GAME");
  
  display.setCursor(38, 40); 
  display.println("OVER");
  
  display.setFont(); 
  display.setTextSize(1);
  
  display.setCursor(40, 45); 
  display.print("Score: "); 
  if (previousGame == GAME_DEATHSTAR) display.print(ds_score);
  else display.print(dino_score);
  
  display.setCursor(25, 55);
  display.print("Press Trigger");
  
  display.display();

  if (digitalRead(BTN_TRIGGER) == LOW) {
    waitForRelease();
    currentState = MENU;
  }
}

bool checkCollision(Rect r1, Rect r2) {
  return (r1.x < r2.x + r2.w &&
          r1.x + r1.w > r2.x &&
          r1.y < r2.y + r2.h &&
          r1.y + r1.h > r2.y);
}

void waitForRelease() {
  delay(50); 
  while(digitalRead(BTN_TRIGGER) == LOW) { }
  delay(50); 
}
