#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET 4
#define SCREEN_ADDRESS 0x3C
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// menus
bool menuOpened = false;
int menu = 0;
int subMenu = 1;
bool menuDepth = false;
bool justOpened = false;
#define MENUSIZE 8
#define STRING_SIZE 11

const char mainMenu[MENUSIZE][8][STRING_SIZE] PROGMEM = {
    {"food", "apple", "steak", "water", NULL},
    {"game", NULL},
    {"sleep", NULL},
    {"clean", NULL},
    {"doctor", NULL},
    {"discipline", NULL},
    {"stats", "hunger", "happiness", "health", "discipline", "weight", "age", NULL},
    {"settings", "sound",
     //"something",
     NULL}};

float hunger = 100;
float happiness = 100;
float health = 100;
float discipline = 100;
float weight = 1;
float age = 0;

const int button1Pin = 5;
const int button2Pin = 6;
const int button3Pin = 7;

void setup() {
  pinMode(5, INPUT_PULLUP);
  pinMode(7, INPUT_PULLUP);
  pinMode(6, INPUT_PULLUP);

  Serial.begin(9600);
  if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;);
  }
  display.clearDisplay();
  display.display();
}

void loop()
{
    int button1State = digitalRead(button1Pin);
    int button2State = digitalRead(button2Pin);
    int button3State = digitalRead(button3Pin);

    if (!menuOpened)
    {
        displayMenu();
        handleMainMenuNavigation(button1State, button2State);
    }
    else
    {
        updateStats();
        displayStats();
        handleSubMenuActions(button1State, button2State);
    }

    delay(100); //  delay as needed
}

void handleMainMenuNavigation(int button1State, int button2State)
{
    static unsigned long lastButtonPress = 0;
    unsigned long currentMillis = millis();

    if (currentMillis - lastButtonPress >= 200) // Button debounce delay
    {
        if (button1State == LOW)
        {
            menu--;
            if (menu < 0)
                menu = MENUSIZE - 1;
            lastButtonPress = currentMillis;
        }
        else if (button2State == LOW)
        {
            menu++;
            if (menu >= MENUSIZE)
                menu = 0;
            lastButtonPress = currentMillis;
        }

        // Open sub-menu 
        if ((const char *)pgm_read_word(&(mainMenu[menu][1])) != NULL)
        {
            menuOpened = true;
            subMenu = 1;
        }
    }
}

void handleSubMenuActions(int button1State, int button2State)
{
   
}


void displayMenu() {
    display.clearDisplay();
    display.setCursor(0, 0);
    display.setTextSize(1);
    display.setTextColor(WHITE);

    // Display menu items
    for (int i = 0; i < 8; i++) {
        if ((const char *)pgm_read_word(&(mainMenu[i][0])) != NULL) {
            if (i == menu) {
                display.setTextColor(WHITE, BLACK);  // Highlight current menu item
            }
            display.println((const char *)pgm_read_word(&(mainMenu[i][0])));
            display.setTextColor(WHITE);  // Reset text color
        }
    }

    // Display status bars or additional information
    switch (menu) {
        case 0:  // Food menu, display hunger bar
            drawBar(hunger);
            break;
        case 6:  // Stats menu, display various stats
            displayStats();
            break;
        // Add more cases for other menus as needed
    }

    display.display();
}

void updateStats()
{
    static unsigned long lastStatUpdate = 0;
    unsigned long currentMillis = millis();
    const unsigned long updateInterval = 1000; // Update every 1 second (adjust as needed)

    if (currentMillis - lastStatUpdate >= updateInterval)
    {
        // Update stats at the specified interval
        hunger -= 0.1;
        happiness -= 0.05;
        health -= 0.07;
        discipline -= 0.03;
        weight += 0.001;
        age += 0.01;

        // Ensure stats stay within bounds
        hunger = constrain(hunger, 0.0, 100.0);
        happiness = constrain(happiness, 0.0, 100.0);
        health = constrain(health, 0.0, 100.0);
        discipline = constrain(discipline, 0.0, 100.0);
        weight = constrain(weight, 0.0, 10.0);

        lastStatUpdate = currentMillis;
    }
}

void displayStats()
{
    display.clearDisplay();
    display.setCursor(0, 0);
    display.setTextSize(1);
    display.setTextColor(WHITE);

    display.print("Hunger: ");
    display.println(hunger);
    display.print("Happiness: ");
    display.println(happiness);
    display.print("Health: ");
    display.println(health);
    display.print("Discipline: ");
    display.println(discipline);
    display.print("Weight: ");
    display.println(weight);
    display.print("Age: ");
    display.println(age);

    display.display();
}

void drawBar(float value) {
    int barWidth = map(value, 0, 100, 0, 48);  // Map value to bar width
    display.fillRect(72, 19, barWidth, 3, WHITE);  // Draw the bar
}

const char *getItem(int menu, int index) {
    const char *menuItem = (const char *)pgm_read_word(&(mainMenu[menu][index]));
    return menuItem;
}

/* int countPoops() {
    int poopsCnt = 0;
    return poopsCnt;
}

void resetPoops() {
}
*/


