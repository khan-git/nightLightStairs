#include <Adafruit_NeoPixel.h>

#define LOWER_BUTTON 2
#define UPPER_BUTTON 3

// Stairs from top to bottom 
Adafruit_NeoPixel strip0 = Adafruit_NeoPixel(10, 22, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel strip1 = Adafruit_NeoPixel(10, 24, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel strip2 = Adafruit_NeoPixel(10, 26, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel strip3 = Adafruit_NeoPixel(10, 28, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel strip4 = Adafruit_NeoPixel(10, 30, NEO_GRB + NEO_KHZ800);

volatile int lowPushed = 0;
volatile int highPushed = 0;

Adafruit_NeoPixel *low2HighArr[] = {&strip0, &strip1, &strip2, &strip3, &strip4};
Adafruit_NeoPixel *high2LowArr[] = {&strip4, &strip3, &strip2, &strip1, &strip0};

void setup() {
  Serial.begin(115200);
  digitalWrite(LOWER_BUTTON, HIGH);
  attachInterrupt(0, lowPushedInt, LOW);
  digitalWrite(UPPER_BUTTON, HIGH);
  attachInterrupt(1, highPushedInt, LOW);

  stripSetup(&strip0, 10);
  stripSetup(&strip1, 10);
  stripSetup(&strip2, 10);
  stripSetup(&strip3, 10);
  stripSetup(&strip4, 10);
  Serial.println("Setup done");
}

//
// Set colour but turn it off
//
void stripSetup(Adafruit_NeoPixel *strip, int leds) {
  strip->begin();
  strip->setBrightness(25);
  toggleOnFirstLeds(strip, leds);
  delay(300);
  toggleOnFirstLeds(strip, leds);
}

//
// Main loop
//
void loop() {
//    setBrightnessSequence(low2HighArr, 5, 100, 10);
//    delay(1000);
//    unsetBrightnessSequence(high2LowArr, 5, 10);
//    delay(1000);

  if(lowPushed > 1 || highPushed > 1) {
    nightMode(low2HighArr, 5, 25);
  }
  
  if(lowPushed == 1) {
    // Do bottom up sequence
    // Clear button
    Serial.println("Low pushed");
    setBrightnessSequence(low2HighArr, 5, 55, 10);
    delay(5000);
    unsetBrightnessSequence(high2LowArr, 5, 10);
    lowPushed = false;
  } else if(highPushed == 1) {
    // Do top down sequence
    // Clear button
    Serial.println("High pushed");
    setBrightnessSequence(high2LowArr, 5, 55, 10);
    delay(5000);
    unsetBrightnessSequence(low2HighArr, 5, 10);
    highPushed = false;
  }
  delay(1000);
}

//
// Interrupt routines
//
void lowPushedInt() {
  lowPushed += 1;
  while(digitalRead(LOWER_BUTTON) == LOW) {
    delay(5);
  }
}

void highPushedInt() {
  highPushed += 1;
  while(digitalRead(UPPER_BUTTON) == LOW) {
    delay(5);
  }
}

//
// Slowly get each strip up to brightness.
//
void setBrightnessSequence(Adafruit_NeoPixel *strips[], int numStrips, int brightness, int seconds) {
  for(int i=0; i < numStrips; i++) {
    ledsOn(strips[i], 10);
    int bri = 1;
    while(bri++ < brightness) {
      strips[i]->setBrightness(bri);
      strips[i]->show();
      delay(seconds);
    }
  }
}

//
// Slowly reduce brightness for each strip.
//
void unsetBrightnessSequence(Adafruit_NeoPixel *strips[], int numStrips, int seconds) {
  for(int i=0; i < numStrips; i++) {
    uint16_t brightness = strips[i]->getBrightness();
    while(brightness-- > 1) {
      strips[i]->setBrightness(brightness);
      strips[i]->show();
      delay(seconds);
    }
    ledsOff(strips[i], 10);
  }
}
  
void ledsOff(Adafruit_NeoPixel *strip, int leds) {
    for(int j=0; j < leds; j++) {
      strip->setPixelColor(j , strip->Color(0,0,0));
    }
    strip->show(); 
}

void ledsOn(Adafruit_NeoPixel *strip, int leds) {
    for(int j=0; j < leds; j++) {
      strip->setPixelColor(j , strip->Color(255,255,255));
    }
    strip->show();
}

//
// If number pushes are more than one we want light on all the time
// Stay so until button is pushed again
//
void nightMode(Adafruit_NeoPixel *strips[], int numStrips, int brightness) {
  for(int i=0; i < numStrips; i++) {
    strips[i]->setBrightness(brightness);
    ledsOn(strips[i], 10);
  }

  Serial.println("Night mode");
  lowPushed = 0;
  highPushed = 0;

  while(highPushed == 0 && lowPushed == 0) {

    delay(1000);
  }
  for(int i=0; i < numStrips; i++) {
    strips[i]->setBrightness(1);
    ledsOff(strips[i], 10);
  }
  Serial.println("Night mode end");
  lowPushed = 0;
  highPushed = 0;

}

//
// Toggle on and off
//
void toggleOnFirstLeds(Adafruit_NeoPixel *strip, int leds) {
  ledsOn(strip, leds);
  delay(300);
  ledsOff(strip, leds);
}

void toggleOffFirstLeds(Adafruit_NeoPixel *strip, int leds) {
  ledsOff(strip, leds);
  delay(300);
  ledsOn(strip, leds);
}

