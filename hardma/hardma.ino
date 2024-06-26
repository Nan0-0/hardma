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

enum Actions {
  ACTION_FOOD_APPLE,
  ACTION_FOOD_STEAK,
  ACTION_FOOD_WATER,
  ACTION_GAME,
  ACTION_SLEEP,
  ACTION_CLEAN,
  ACTION_DOCTOR,
  ACTION_DISCIPLINE,
  ACTION_STATS,
  ACTION_SETTINGS_SOUND,
  NUM_ACTIONS
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
int menu = 0;
int action = NULL;
int setting = 0;
bool justOpened = false;
bool paused = false;
bool gameOver = false;
bool game = false;
bool level = 0;
bool soundEnabled = true;

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
    memcpy_P(&oneItem, &mainMenu[menu][0], sizeof oneItem);
    display.println(oneItem);
    if (subMenu) {
      display.setTextColor(WHITE);
      display.setCursor(8, 16);
      char subItem[STRING_SIZE];
      memcpy_P(&subItem, &mainMenu[menu][subMenu], sizeof subItem);
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
  // Button 1 - MENU
  if (menuButtonState == ACTIVATED) {
    if (soundEnabled) {
      tone(soundPin, 300, 80);
    }

    if (!menuOpened) {
      menuOpened = true;
    } else {
      if (menuDepth) {
        if ((const char*)pgm_read_word(&(mainMenu[menu][subMenu + 1])) == NULL) {
          subMenu = 1;
        } else {
          ++subMenu;
        }
        setting = 100 * (menu + 1) + subMenu;
      } else {
        if (menu == MENUSIZE - 1) {
          menu = 0;
        } else {
          ++menu;
        }

        if ((const char*)pgm_read_word(&(mainMenu[menu][1])) != NULL) {
          subMenu = 1;

          justOpened = true;
        }
        setting = 100 * (menu + 1) + subMenu;
      }
    }

    delay(60);
  }

  // Button 2 - SELECT
  if (selectButtonState == ACTIVATED) {
    if (game) {
      if (!gameOver) {
        paused = !paused;
        if (soundEnabled) {
          tone(soundPin, 600, 80);
        }
        delay(60);
      }

    } else {
      if (soundEnabled) {
        tone(soundPin, 600, 80);
      }

      if (menuOpened) {

        if (subMenu != 1 && (const char*)pgm_read_word(&(mainMenu[menu][1][0])) != NULL) {
          action = 100 * (menu + 1) + subMenu;
        }
        if (subMenu == 1 && (const char*)pgm_read_word(&(mainMenu[menu][1][0])) == NULL) {
          action = 100 * (menu + 1) + subMenu;
        }
        if (subMenu == 1 && (const char*)pgm_read_word(&(mainMenu[menu][1][0])) != NULL && menuDepth) {
          action = 100 * (menu + 1) + subMenu;
        }
        if ((const char*)pgm_read_word(&(mainMenu[menu][1][0])) != NULL) {
          setting = 100 * (menu + 1) + subMenu;
          menuDepth = true;
        }

      } else {
        action = NULL;

        menuOpened = true;
        menuDepth = true;
        subMenu = 1;
        menu = 6;
        action = 701;
        setting = 701;
      }
      justOpened = false;

      delay(60);
    }
  }

  // Button 3 - BACK
  if (backButtonState == ACTIVATED) {
    if (soundEnabled) {
      tone(soundPin, 1000, 80);
    }

    if (game || gameOver) {

      game = false;
      gameOver = false;
      level = 0;
      paused = false;
    } else {
      if (!menuDepth) {
        menuOpened = false;
        menu = 0;
        setting = 0;
      } else {
        menuDepth = false;
        setting = 100 * (menu + 1) + 1;
      }
      action = NULL;
      subMenu = 1;
    }

    delay(60);
  }
}
void handleAction(int action) {
  switch (action) {
    case ACTION_FOOD_APPLE:
      break;
    case ACTION_FOOD_STEAK:
      break;
    case ACTION_FOOD_WATER:
      break;
    case ACTION_GAME:
      break;
    case ACTION_SLEEP:
      break;
    case ACTION_CLEAN:
      break;
    case ACTION_DOCTOR:
      break;
    case ACTION_DISCIPLINE:
      break;
    case ACTION_STATS:
      break;
    case ACTION_SETTINGS_SOUND:
      break;
    default:
      break;
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

