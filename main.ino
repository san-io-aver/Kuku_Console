#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Fonts/FreeSans9pt7b.h>      // Regular (Used for Menu Items)
#include <Fonts/FreeSansBold9pt7b.h>  // Bold (Used for Titles)

// --- Hardware Configuration ---
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET    -1 
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// --- Button Pins ---
#define BTN_UP      4
#define BTN_DOWN    3
#define BTN_TRIGGER 2

// --- STRUCT DEFINITION ---
struct Rect {
  int x; int y; int w; int h;
};

// --- ASSETS ---

// Player Ship
const unsigned char PROGMEM dioda16 [] = {
  0x00, 0x00, 0x00, 0x00, 0x1C, 0x00, 0x3F, 0xF0, 0x3C, 0x00, 0x3C, 0x00, 0xFF, 0x00, 0x7F, 0xFF,
  0x7F, 0xFF, 0xFF, 0x00, 0x3C, 0x00, 0x3C, 0x00, 0x1F, 0xF0, 0x1C, 0x00, 0x00, 0x00, 0x00, 0x00
};

// Dino
static const unsigned char PROGMEM dino_bmp[]={
  0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0xfe, 0x00, 0x00, 0x06, 0xff, 0x00, 0x00, 0x0e, 0xff, 0x00, 
  0x00, 0x0f, 0xff, 0x00, 0x00, 0x0f, 0xff, 0x00, 0x00, 0x0f, 0xff, 0x00, 0x00, 0x0f, 0xc0, 0x00, 
  0x00, 0x0f, 0xfc, 0x00, 0x40, 0x0f, 0xc0, 0x00, 0x40, 0x1f, 0x80, 0x00, 0x40, 0x7f, 0x80, 0x00, 
  0x60, 0xff, 0xe0, 0x00, 0x71, 0xff, 0xa0, 0x00, 0x7f, 0xff, 0x80, 0x00, 0x7f, 0xff, 0x80, 0x00, 
  0x7f, 0xff, 0x80, 0x00, 0x3f, 0xff, 0x00, 0x00, 0x1f, 0xff, 0x00, 0x00, 0x0f, 0xfe, 0x00, 0x00, 
  0x03, 0xfc, 0x00, 0x00, 0x01, 0xdc, 0x00, 0x00, 0x01, 0x8c, 0x00, 0x00, 0x01, 0x8c, 0x00, 0x00, 
  0x01, 0x0c, 0x00, 0x00, 0x01, 0x8e, 0x00, 0x00
};

// Trees
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

// Snowflake
#define LOGO_HEIGHT   16
#define LOGO_WIDTH    16
#define NUMFLAKES     10
static const unsigned char PROGMEM snowflake_bmp[] = { 
  0b00000000, 0b11000000, 0b00000001, 0b11000000, 0b00000001, 0b11000000, 0b00000011, 0b11100000,
  0b11110011, 0b11100000, 0b11111110, 0b11111000, 0b01111110, 0b11111111, 0b00110011, 0b10011111,
  0b00011111, 0b11111100, 0b00001101, 0b01110000, 0b00011011, 0b10100000, 0b00111111, 0b11100000,
  0b00111111, 0b11110000, 0b01111100, 0b11110000, 0b01110000, 0b01110000, 0b00000000, 0b00110000 
};

// --- VARIABLES ---

enum State { ANIMATION, MENU, GAME_DEATHSTAR, GAME_DINO, GAME_OVER };
State currentState = ANIMATION; // START WITH ANIMATION
State previousGame = GAME_DEATHSTAR;

// Menu
int menuSelection = 0; 
unsigned long lastDebounceTime = 0;
const int debounceDelay = 200;

// Snowflakes
int flakes[NUMFLAKES][3]; 
unsigned long lastFrameTime = 0;

// --- DINO VARIABLES ---
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

// --- DEATHSTAR VARIABLES ---
int metx = 0, mety = 0, postoji = 0, nep = 8, smjer = 0;
int rx = 200, ry = 0, rx2 = 200, ry2 = 0, rx3 = 200, ry3 = 0, rx4 = 200, ry4 = 0; 
int ds_score = 0; 
int ds_lives = 5; 
int ds_level = 1; 
int poz = 30; // Player Y
int brzina = 3, bkugle = 1, promjer = 10, ispaljeno = 0, poc = 0;
int centar = 95;
unsigned long pocetno = 0, odabrano = 0, nivovrije = 0;

// Prototypes
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

  // Init flakes
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

// --------------------------------------------------------------------------
// ANIMATION STATE
// --------------------------------------------------------------------------
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

// --------------------------------------------------------------------------
// MENU STATE (INTELLIGENT HEADER)
// --------------------------------------------------------------------------
void runMenu() {
  display.clearDisplay();
  
  // LOGIC: If selection is 0, show Title. If >0, hide title and shift items up.
  int itemHeight = 18;
  int titleHeight = 20; 
  int startY = 35; // Base Position for Item 0 when Title is VISIBLE

  // If scrolled down, shift everything up to hide title area
  if (menuSelection > 0) {
    startY = 20; // Shift items up so they occupy the top area
  }

  // Draw Title ONLY if at top
  if (menuSelection == 0) {
    display.setFont(&FreeSansBold9pt7b);
    display.setTextColor(WHITE);
    display.setCursor(40, 15);
    display.println("MENU");
    display.drawLine(0, 18, 128, 18, WHITE);
  }

  // Draw Items based on dynamic startY
  int currentY = startY; 
  
  // We calculate offsets relative to selection
  // If selected=0, offsets are 0, 18, 36
  // If selected=1, offsets are -18, 0, 18 (visually centered)
  
  // Calculate Top Y for the List based on Selection to keep Selected Item in middle
  int listTopY = startY;
  if (menuSelection > 0) {
     listTopY = 35 - (menuSelection * itemHeight); // Keep selected item near center Y=35
  }

  display.setFont(&FreeSans9pt7b); 

  // --- ITEM 0: HOME ---
  int y0 = listTopY;
  if (y0 > 10 && y0 < 60) { // Only draw if visible on screen
    if (menuSelection == 0) {
       display.fillRect(5, y0 - 11, 118, 15, WHITE);
       display.setTextColor(BLACK);
       display.setFont(&FreeSansBold9pt7b);
    } else display.setTextColor(WHITE);
    display.setCursor(28, y0); display.println("HOME");
    display.setFont(&FreeSans9pt7b); 
  }

  // --- ITEM 1: DINO ---
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

  // --- ITEM 2: DEATHSTAR ---
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

  // Inputs
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

// --------------------------------------------------------------------------
// GAME: DEATH STAR
// --------------------------------------------------------------------------
void resetDeathStar() {
  ds_score = 0; ds_lives = 5; ds_level = 1; poz = 30;
  metx = 0; mety = 0; postoji = 0; 
  nep = 8; smjer = 0;
  brzina = 3; bkugle = 1; promjer = 10; ispaljeno = 0; poc = 0;
  centar = 95;
  rx = 200; ry = 0; rx2 = 200; ry2 = 0; rx3 = 200; ry3 = 0; rx4 = 200; ry4 = 0;
  nivovrije = millis();
}

void runDeathStar() {
  unsigned long trenutno = millis();
  display.clearDisplay();
  
  // Background
  display.drawPixel(50, 30, 1); display.drawPixel(30, 17, 1);
  display.drawPixel(60, 18, 1); display.drawPixel(25, 43, 1);

  // Level Logic
  if ((trenutno - nivovrije) > 50000) {
    nivovrije = trenutno;
    ds_level++;
    brzina++;
    if (ds_level % 2 == 0) { bkugle++; promjer = max(4, promjer - 1); }
  }

  // Enemy Firing
  if (poc == 0) {
    pocetno = millis();
    odabrano = random(400, 1200);
    poc = 1;
  }
  if ((odabrano + pocetno) < trenutno) {
    poc = 0; ispaljeno++;
    if (ispaljeno == 1) { rx = 95; ry = nep; }
    if (ispaljeno == 2) { rx2 = 95; ry2 = nep; }
    if (ispaljeno == 3) { rx3 = 95; ry3 = nep; }
    if (ispaljeno == 4) { rx4 = 95; ry4 = nep; }
  }

  // Draw Bullets
  if (ispaljeno > 0) { display.drawCircle(rx, ry, 2, 1); rx -= brzina; }
  if (ispaljeno > 1) { display.drawCircle(rx2, ry2, 1, 1); rx2 -= brzina; }
  if (ispaljeno > 2) { display.drawCircle(rx3, ry3, 4, 1); rx3 -= brzina; }
  if (ispaljeno > 3) { display.drawCircle(rx4, ry4, 2, 1); rx4 -= brzina; }

  // Controls
  if (digitalRead(BTN_UP) == LOW && poz >= 2) poz -= 2;
  if (digitalRead(BTN_DOWN) == LOW && poz <= 46) poz += 2;
  if (digitalRead(BTN_TRIGGER) == LOW && postoji == 0) {
    postoji = 1; metx = 6; mety = poz + 8;
  }

  // Player Bullet
  if (postoji == 1) {
    metx += 8;
    display.drawLine(metx, mety, metx + 4, mety, 1);
    if (metx > 128) postoji = 0;
  }

  // Draw Ships
  display.drawBitmap(4, poz, dioda16, 16, 16, 1);
  display.fillCircle(centar, nep, promjer, 1);
  display.fillCircle(centar + 2, nep + 3, promjer / 3, 0);

  // --- HUD (FIXED) ---
  display.setFont(); // Switch to Default Font for HUD
  display.setTextColor(WHITE); // Ensure White Color
  display.setTextSize(1);
  display.setCursor(33, 57); display.print("Score:"); display.print(ds_score);
  display.setCursor(33, 0);  display.print("Lives:"); display.print(ds_lives);
  display.setCursor(110, 0); display.print("L:"); display.print(ds_level);

  // Logic
  if (smjer == 0) nep += bkugle; else nep -= bkugle;
  if (nep >= (64 - promjer)) smjer = 1;
  if (nep <= promjer) smjer = 0;

  if (mety >= nep - promjer && mety <= nep + promjer) {
    if (metx > (centar - promjer) && metx < (centar + promjer)) {
      ds_score++; postoji = 0; metx = -20;
    }
  }

  int pMid = poz + 8;
  if (rx < 12 && rx > 4 && ry >= pMid - 8 && ry <= pMid + 8) { ds_lives--; rx = -50; }
  if (rx2 < 12 && rx2 > 4 && ry2 >= pMid - 8 && ry2 <= pMid + 8) { ds_lives--; rx2 = -50; }
  if (rx3 < 12 && rx3 > 4 && ry3 >= pMid - 8 && ry3 <= pMid + 8) { ds_lives--; rx3 = -50; }
  if (rx4 < 12 && rx4 > 4 && ry4 >= pMid - 8 && ry4 <= pMid + 8) { ds_lives--; rx4 = -50; ispaljeno = 0; }
  
  if (rx4 < 1 && ispaljeno >=4) { ispaljeno = 0; rx4 = 200; }
  
  if (ds_lives <= 0) {
    previousGame = GAME_DEATHSTAR;
    currentState = GAME_OVER;
  }
  
  display.display();
}

// --------------------------------------------------------------------------
// GAME: DINO
// --------------------------------------------------------------------------
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

  // --- MORE STARS ---
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

  // HUD
  display.setFont();
  display.setTextColor(WHITE);
  display.setTextSize(1);
  display.setCursor(80, 0); display.print("Score:"); display.print(dino_score);

  // Logic
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

// --------------------------------------------------------------------------
// GAME OVER (FIXED TEXT ALIGNMENT)
// --------------------------------------------------------------------------
void runGameOver() {
  display.clearDisplay();
  
  // Big "GAME OVER" Text
  // Using Custom Font (Bottom-Left coordinate system)
  display.setFont(&FreeSansBold9pt7b);
  display.setTextColor(WHITE);
  
  // Centering "GAME" and "OVER"
  display.setCursor(38, 20); // X=38 centers "GAME" nicely
  display.println("GAME");
  
  display.setCursor(38, 40); // X=38 centers "OVER" nicely
  display.println("OVER");
  
  // Score and Prompt (Using Default Font for clarity & alignment)
  display.setFont(); // Reset to system font
  display.setTextSize(1);
  
  display.setCursor(40, 45); // Center-ish
  display.print("Score: "); 
  if (previousGame == GAME_DEATHSTAR) display.print(ds_score);
  else display.print(dino_score);
  
  display.setCursor(25, 55);
  display.print("Press Trigger");
  
  display.display();

  // Wait for input to return to menu
  if (digitalRead(BTN_TRIGGER) == LOW) {
    waitForRelease();
    currentState = MENU;
  }
}

// --------------------------------------------------------------------------
// UTILS
// --------------------------------------------------------------------------
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
