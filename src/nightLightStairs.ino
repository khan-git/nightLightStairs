#include <Arduino.h>
#include "FastLED.h"

// #include <Adafruit_NeoPixel.h>
// #include <SPI.h>
// #include <WiFi101.h>

#define RED_BUTTON 7
#define BLUE_BUTTON 8
#define NUMBER_OF_LEDS 10
CRGB leds[10];

// Stairs from top to bottom
Adafruit_NeoPixel strip0 = Adafruit_NeoPixel(NUMBER_OF_LEDS, 0, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel strip1 = Adafruit_NeoPixel(NUMBER_OF_LEDS, 1, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel strip2 = Adafruit_NeoPixel(NUMBER_OF_LEDS, 2, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel strip3 = Adafruit_NeoPixel(NUMBER_OF_LEDS, 3, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel strip4 = Adafruit_NeoPixel(NUMBER_OF_LEDS, 4, NEO_GRB + NEO_KHZ800);

volatile boolean redPushed = false;
volatile boolean bluePushed = false;

#define MODE_FIRST 0
#define MODE_NORMAL 0
#define MODE_COLOUR 1
#define MODE_BRIGHTNESS 2
#define MODE_DISCO 3
#define MODE_GAME 4
#define MODE_LAST 4

#define BRIGHTNESSLVL_4 100
#define BRIGHTNESSLVL_3 75
#define BRIGHTNESSLVL_2 50
#define BRIGHTNESSLVL_1 25

volatile int currentBrightness = BRIGHTNESSLVL_2;

volatile int currentMode = MODE_NORMAL;

Adafruit_NeoPixel *modeStrip = &strip4;
Adafruit_NeoPixel *menuStrip = &strip4;

Adafruit_NeoPixel *allLedsArr[] = {&strip0, &strip1, &strip2, &strip3, &strip4};
Adafruit_NeoPixel **low2HighArr = allLedsArr;
Adafruit_NeoPixel *high2LowArr[] = {&strip4, &strip3, &strip2, &strip1, &strip0};
volatile int ledArraySize = 0;
boolean ledState = false;

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

void setup() {
  Serial.begin(115200);
  Serial.println("Setup begin");
  ledArraySize = sizeof(low2HighArr)/sizeof(*low2HighArr);
  pinMode(RED_BUTTON, INPUT_PULLUP);
  attachInterrupt(RED_BUTTON, redPushedInt, LOW);

  pinMode(BLUE_BUTTON, INPUT_PULLUP);
  attachInterrupt(BLUE_BUTTON, bluePushedInt, LOW);
  Serial.println("Leds");

  stripSetup(&strip0, NUMBER_OF_LEDS);
  stripSetup(&strip1, NUMBER_OF_LEDS);
  stripSetup(&strip2, NUMBER_OF_LEDS);
  stripSetup(&strip3, NUMBER_OF_LEDS);
  stripSetup(&strip4, NUMBER_OF_LEDS);
  Serial.println("Setup done");
}

//
// Initiate strip
//
void stripSetup(Adafruit_NeoPixel *strip, int leds) {
  strip->begin();
  strip->setBrightness(currentBrightness);
  toggleOnFirstLeds(strip, leds);
  delay(300);
  toggleOnFirstLeds(strip, leds);
}

//
// Turn all leds off
//
void allLedsOff()
{
  int i = 0;
  for(; i < 5; i++)
  {
    ledsOff(low2HighArr[i], NUMBER_OF_LEDS);
  }
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

//
// Light mode leds
//
void setMode(Adafruit_NeoPixel *modeStrip, int mode)
{
  allLedsOff();
  ledsOn(modeStrip, mode+1);
  Serial.print("Mode ");
  Serial.println(mode);
}
int colors[4][3] = {{255,0,0},{0,255,0},{0,0,255},{0,255,255}};
void colourMode(Adafruit_NeoPixel *ledStrips[], Adafruit_NeoPixel *menuStrip)
{
  redPushed = false;
  bluePushed = false;
  while(!bluePushed)
  {
    if(redPushed)
    {
      // Do next colour
      redPushed = false;
    }
  }
}

void brightnessMode(Adafruit_NeoPixel *ledStrips[], Adafruit_NeoPixel *menuStrip)
{
  int ledPos = currentBrightness/25;
  menuStrip->setBrightness(currentBrightness);
  ledsOff(menuStrip, NUMBER_OF_LEDS);
  ledsOn(menuStrip, ledPos);
  Serial.println("Enter brightness mode");
  delay(5);
  redPushed = false;
  bluePushed = false;
  while(!bluePushed)
  {
    if(redPushed)
    {
      switch(currentBrightness)
      {
        case BRIGHTNESSLVL_1:
          currentBrightness = BRIGHTNESSLVL_2;
          ledPos = 2;
          break;
        case BRIGHTNESSLVL_2:
          currentBrightness = BRIGHTNESSLVL_3;
          ledPos = 3;
          break;
        case BRIGHTNESSLVL_3:
          currentBrightness = BRIGHTNESSLVL_4;
          ledPos = 4;
          break;
        case BRIGHTNESSLVL_4:
        default:
          currentBrightness = BRIGHTNESSLVL_1;
          ledPos = 1;
          break;
      }
      menuStrip->setBrightness(currentBrightness);
      ledsOff(menuStrip, NUMBER_OF_LEDS);
      ledsOn(menuStrip, ledPos);
      redPushed = false;
    }
  }
  toggleOffFirstLeds(menuStrip, ledPos);
  delay(300);
  Serial.println("Exit brightness mode");
}


//
// Main loop
//
void loop() {
  uint32_t point;
  if( redPushed == true)
  {
    Serial.println("Enter settings");
    Serial.println(ledArraySize);
    setMode(modeStrip, currentMode);
    redPushed = false;
    while(true)
    {
      if(redPushed)
      {
        point = millis();
        currentMode++;
        if(currentMode > MODE_LAST)
        {
          currentMode = MODE_FIRST;
        }
        setMode(modeStrip, currentMode);
        redPushed = false;
        Serial.println(millis()-point);
      }
      else if (bluePushed)
      {
        switch(currentMode)
        {
          case MODE_COLOUR:
            colourMode(allLedsArr, menuStrip);
            break;
          case MODE_BRIGHTNESS:
            brightnessMode(allLedsArr, menuStrip);
            break;
          case MODE_DISCO:
            break;
          case MODE_GAME:
            break;
          default:
            break;
        }
        break;
      }
    }
    allLedsOff();
    redPushed = false;
    bluePushed = false;
    Serial.println("Exit settings");
  }
  else if(bluePushed)
  {
    bluePushed = false;
    if(ledState)
    {
      ledsOff(&strip4, 1);
    }
    else
    {
      ledsOn(&strip4, 1);
    }
    ledState = !ledState;
    Serial.print("Ledstate ");
    Serial.println(ledState);
  }
}

//
// Interrupt routines
//
void redPushedInt() {
  redPushed = true;
  // while(digitalRead(RED_BUTTON) == LOW)
  // {
  // }
}

void bluePushedInt() {
  bluePushed = true;
  // while(digitalRead(BLUE_BUTTON) == LOW)
  // {
  // }
}
