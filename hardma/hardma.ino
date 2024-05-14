#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET 4
#define SCREEN_ADDRESS 0x3C

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

const int BUTTON_PIN_COUNT = 3;
const int buttonPins[] = {5, 6, 7};
const int soundPin = 4;

const int MENU_SIZE = 8;
const int STRING_SIZE = 1;

const char mainMenu[MENU_SIZE][8][STRING_SIZE] PROGMEM = {
  {"food","apple","steak","water",NULL},
  {"game",NULL},
  {"sleep",NULL},
  {"clean",NULL},
  {"doctor",NULL},
  {"discipline",NULL},
  {"stats","hunger","happiness","health","discipline","weight","age",NULL},
  {"settings","sound",NULL}
};

class Pet {
public:
  float hunger;
  float happiness;
  float health;
  float discipline;
  float weight;
  float age;
  bool sleeping;
  bool dead;
  bool soundEnabled;
  int poops[3];

  Pet() : hunger(100), happiness(100), health(100), discipline(100), weight(1), age(0), sleeping(false), dead(false), soundEnabled(true) {}

  void updateStats() {
    // update pet stats here
  }

  void drawStats() {
    // draw pet stats here
  }
};

Pet pet;

void setup() {
  for (int i = 0; i < BUTTON_PIN_COUNT; i++) {
    pinMode(buttonPins[i], INPUT);
    digitalWrite(buttonPins[i], HIGH);
  }
  pinMode(soundPin, OUTPUT);

  randomSeed(analogRead(0));

  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.clearDisplay();

  // splash screen
  display.setTextColor(WHITE);
  display.print(F("nano presents"));
  display.display();
  tone(sound, 500, 200);
  delay(200);
  tone(sound, 1000, 200);
  delay(400);
  tone(sound, 700, 200);
  delay(200);
  tone(sound, 1100, 200);
  delay(2200);
  display.clearDisplay();
}

void loop() {
  int buttonStates[BUTTON_PIN_COUNT];
  for (int i = 0; i < BUTTON_PIN_COUNT; i++) {
    buttonStates[i] = digitalRead(buttonPins[i]);
  }

  if (!pet.dead) {
    pet.updateStats();
    pet.drawStats();

    // handle button presses
    if (buttonStates[0] == LOW) {
      // handle button 1 press
    }
    if (buttonStates[1] == LOW) {
      // handle button 2 press
    }
    if (buttonStates[2] == LOW) {
      // handle button 3 press
    }
  } else {
    // pet is dead, display game over screen
  }
}

void drawBar(float value) {
  display.fillRect(72, 19, 48 * value / 100, 3, WHITE);
}

int countPoops() {
  int poopsCnt = 0;
  for (int i = 0; i < 3; i++) {
    if (pet.poops[i] > 0) {
      ++poopsCnt;
    }
  }
  return poopsCnt;
}

void resetPoops() {
  for (int i = 0; i < 3; i++) {
    pet.poops[i] = 0;
  }
}





