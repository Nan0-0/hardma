#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 
#define SCREEN_HEIGHT 64 
#define OLED_RESET     4 
#define SCREEN_ADDRESS 0x3C 
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

int selected = 0;
int entered = -1;

void displaymenu(void) {

  int down = digitalRead(5);
  int up = digitalRead(7);
  int enter = digitalRead(6);

  if (up == LOW && down == LOW) {
  };
  if (up == LOW) {
    selected = selected + 1;
    delay(200);
  };
  if (down == LOW) {
    selected = selected - 1;
    delay(200);
  };
  if (enter == LOW) {
    entered = selected;
  };

  const char *options[4] = {
    " Menu 1",
    " Menu 2",
    " Menu 3",
    " Menu 4 "
  };

  if (entered == -1) {
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(0, 0);
    display.println(F("Button test"));
    display.println("");
    for (int i = 0; i < 4; i++) {
      if (i == selected) {
        display.setTextColor(SSD1306_BLACK, SSD1306_WHITE);
        display.println(options[i]);
      } else if (i != selected) {
        display.setTextColor(SSD1306_WHITE);
        display.println(options[i]);
      }
    }
  } else if (entered == 0) {
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(0, 0);
    display.println(" option 1");
    display.setTextColor(SSD1306_WHITE);
    display.setTextSize(2);
    display.println("-Nano");
  } else if (entered == 1) {
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(0, 0);
    display.println(" option 2");
    display.setTextColor(SSD1306_WHITE);
    display.setTextSize(2);
    display.println("Nano");
  }
  else if (entered == 2) {
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(0, 0);
    display.println("Menu option 3");
    display.setTextColor(SSD1306_WHITE);
    display.setTextSize(2);
    display.println("Nans");
  } else if (entered == 3) {
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(0, 0);
    display.println("Menu option 4");
    display.setTextColor(SSD1306_WHITE);
    display.setTextSize(2);
    display.println(" naa");
  } else if (entered == 4) {
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(0, 0);
    display.println("button");
    display.setTextColor(SSD1306_WHITE);
    display.setTextSize(2);
    display.println("nono");
  }

  display.display();
}

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
  display.drawPixel(10, 10, SSD1306_WHITE);
  display.display();
  delay(2000); 

void loop() {
  displaymenu();
}


