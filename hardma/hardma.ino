#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// Constants
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET 4
#define SCREEN_ADDRESS 0x3C

#define ACTIVATED LOW
#define MENUSIZE 8
#define STRING_SIZE 1

// Enums
enum GameState {
  GAME_IDLE,
  GAME_PLAYING,
  GAME_PAUSED,
  GAME_OVER
};

// Global variables
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
GameState gameState = GAME_IDLE;
bool notification = false;
bool dead = false;
bool sleeping = false;
int soundPin = 4;
bool menuOpened = false;
int menuDepth = 0;
int subMenu = 0;

// Button pin constants
const int menuButtonPin = 5;
const int selectButtonPin = 6;
const int backButtonPin = 7;

// Button pin states
int menuButtonState = 0;
int selectButtonState = 0;
int backButtonState = 0;

// Pet stats
float hunger = 100;
float happiness = 100;
float health = 100;
float discipline = 100;
float weight = 1;
float age = 0;

// Poopometer
float poopometer = 0;
int poops[3] = {0, 0, 0};

// Settings
bool soundEnabled = true;

// Menu constants
const char mainMenu[MENUSIZE][8][STRING_SIZE] PROGMEM = {
  {"food","apple","steak","water",NULL},
  {"game",NULL},
  {"sleep",NULL},
  {"clean",NULL},
  {"doctor",NULL},
  {"discipline",NULL},
  {"stats","hunger","happiness","health","discipline","weight","age",NULL},
  {"settings","sound",NULL}
};

// Menu state
int menuSelectedIndex = 0;
bool menuState = false;

void setup() {
  // Initialize button pins as inputs with pull-up resistors
  pinMode(menuButtonPin, INPUT_PULLUP);
  pinMode(selectButtonPin, INPUT_PULLUP);
  pinMode(backButtonPin, INPUT_PULLUP);

  // Initialize sound pin as output
  pinMode(soundPin, OUTPUT);

  // Initialize random seed
  randomSeed(analogRead(0));

  // Initialize display
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.clearDisplay();

  // Display splash screen
  display.setTextColor(WHITE);
  display.print(F("nano presents"));
  display.display();

  // Play splash tone
  tone(soundPin, 500, 200);
  delay(200);
  tone(soundPin, 1000, 200);
  delay(400);
  tone(soundPin, 700, 200);
  delay(200);
  tone(soundPin, 1100, 200);

  delay(2200);

  // Clear display
  display.clearDisplay();
}

void loop() {
  // Read button states
  menuButtonState = digitalRead(menuButtonPin);
  selectButtonState = digitalRead(selectButtonPin);
  backButtonState = digitalRead(backButtonPin);

  // Update pet stats
  if (gameState != GAME_OVER) {
    updatePetStats();
  }

  // Handle button presses
  handleButtonPresses();

  // Update display
  display.display();

  // Display menu
  if (menuOpened && !gameState) {
    display.fillRect(0, 0, display.width(), 30, BLACK);
    display.drawRect(0, 0, display.width(), 29, WHITE);
    display.fillRect(1, 1, display.width() - 2, 27, BLACK);
    display.drawRect(0, 0, display.width(), 12, WHITE);
    display.setCursor(8, 2);
    display.setTextSize(1);
    if (menuDepth) {
      display.fillRect(0, 0, display.width(), 12, WHITE);
      display.fillRect(1, 18, 1, 5, WHITE);
      display.fillRect(2, 19, 1, 3, WHITE);
      display.fillRect(3, 20, 1, 1, WHITE);
      display.setTextColor(BLACK, WHITE);
    } else {
      display.fillRect(1, 3, 1, 5, WHITE);
      display.fillRect(2, 4, 1, 3, WHITE);
      display.fillRect(3, 5, 1, 1, WHITE);
      display.setTextColor(WHITE);
    }
    char oneItem[STRING_SIZE];
    memcpy_P(&oneItem, &mainMenu[menuSelectedIndex][0], sizeof oneItem);
    display.println(oneItem);
    if (subMenu) {
      display.setTextColor(WHITE);
      display.setCursor(8, 16);
      char subItem[STRING_SIZE];
      memcpy_P(&subItem, &mainMenu[menuSelectedIndex][subMenu], sizeof subItem);
      display.println(subItem);
    }
  }
}

void updatePetStats() {
  // Update hunger
  if (sleeping) {
    hunger -= 0.00005;
  } else {
    hunger -= 0.00025;
  }

  // Update happiness
  if (happiness - 0.0002 > 0) {
    happiness -= 0.0002;
  }

  // Update health
  health -= 0.0001 + countPoops() * 0.0001;

  // Update discipline
  if (!sleeping) {
    discipline -= 0.0002;
  }

  // Update age
  age += 0.0000025;

  // Update poopometer
  if (poopometer < 10) {
    poopometer += 0.00025;
  }

  // Check for poop
  if (poopometer >= 10) {
    poopometer = countPoops();
    poops[round(poopometer)] = random(10, 50);
  }

  // Check for notifications
  checkForNotifications();

  // Check for death
  if (hunger <= 0 || health <= 0 || happiness <= 0) {
    dead = true;
    if (soundEnabled) {
      tone(soundPin, 500, 500);
      delay(550);
      tone(soundPin, 400, 500);
      delay(550);
      tone(soundPin, 300, 600);
    }
  }
}

void checkForNotifications() {
  if (hunger <= 20 || countPoops() > 0 || happiness <= 20 || health <= 20) {
    notification = true;
  } else {
    notification = false;
    digitalWrite(13, LOW);
  }
}

void handleButtonPresses() {
  // Check for menu button press
  if (menuButtonState == LOW) {
    menuButtonPressed();
  }

  // Check for select button press
  if (selectButtonState == LOW) {
    selectButtonPressed();
  }

  // Check for back button press
  if (backButtonState == LOW) {
    backButtonPressed();
  }
}

void menuButtonPressed() {
  if (gameState == GAME_IDLE) {
    menuSelectedIndex = 0;
    menuOpened = true;
  } else if (gameState == GAME_PLAYING) {
    menuSelectedIndex = 0;
    gameState = GAME_PAUSED;
  } else if (gameState == GAME_PAUSED) {
    menuSelectedIndex = 0;
    gameState = GAME_PLAYING;
  }
}

void selectButtonPressed() {
  if (menuOpened && !gameState) {
    subMenu++;
    if (subMenu >= STRING_SIZE) {
      subMenu = 0;
    }
    if (!mainMenu[menuSelectedIndex][subMenu]) {
      subMenu = 0;
      menuDepth++;
      if (menuDepth >= STRING_SIZE) {
        menuDepth = 0;
      }
    }
  }
}

void backButtonPressed() {
  if (menuOpened && !gameState) {
    if (subMenu) {
      subMenu--;
    } else if (menuDepth) {
      menuDepth--;
      subMenu = mainMenu[menuSelectedIndex][menuDepth];
    } else {
      menuOpened = false;
      subMenu = 0;
      menuDepth = 0;
    }
  }
}

float countPoops() {
  float poopCount = 0;

  for (int i = 0; i < 3; i++) {
    poopCount += poops[i];
  }

  return poopCount;
}

void resetGame() {
  // Reset pet stats
  age = 0;
  health = 1;
  hunger = 1;
  happiness = 1;
  discipline = 1;
  poopometer = 0;

  // Reset game state
  gameState = GAME_IDLE;

  // Reset display
  display.clearDisplay();
  display.display();
}