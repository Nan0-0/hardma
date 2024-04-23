#include <Arduino.h>

const int INITIAL_CLEANLINESS_LEVEL = 10;
const int POOP_CLEANLINESS_DECREASE = 2;
const int UNCLEANED_POOP_THRESHOLD_1 = 5;
const int UNCLEANED_POOP_THRESHOLD_2 = 10;
const int CLEANLINESS_THRESHOLD = 5;

void setup () {

}

void loop () {
  checkCleanlisness
}



void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
}

void loop() {
  static int count = 0; 

  digitalWrite(LED_BUILTIN, HIGH); 
  delay(100); 
  digitalWrite(LED_BUILTIN, LOW); 
  delay(100); 

  count++; 

  if (count >= 100) {
    while (true) {
      
    }
  }
}


