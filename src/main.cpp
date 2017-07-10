#include <Arduino.h>
#include "FastLED.h"

#if 0
// MKR1000
#define RED_BUTTON 7
#define BLUE_BUTTON 8
#define STRIP_0_PIN 0
#define STRIP_1_PIN 1
#define STRIP_2_PIN 2
#define STRIP_3_PIN 3
#define STRIP_4_PIN 4
#define MODE_STRIP 0
#define OPTIONS_STRIP 4
#else
// Arduino mega
#define RED_BUTTON 2
#define BLUE_BUTTON 3
#define STRIP_0_PIN 22
#define STRIP_1_PIN 24
#define STRIP_2_PIN 26
#define STRIP_3_PIN 28
#define STRIP_4_PIN 30
#define MODE_STRIP 0
#define OPTIONS_STRIP 0
#endif

#define NUM_LEDS 10

CRGB ledArray[5][NUM_LEDS];

volatile boolean redPushed = false;
volatile boolean bluePushed = false;
volatile boolean redFirst = false;

volatile boolean ledState = false;
volatile int ledBrightness = 25;

CRGB::HTMLColorCode currentColour;

//
// Interrupt routines
//
void redPushedInt() {
  redPushed = true;
  while(digitalRead(RED_BUTTON) == LOW)
  {
    if (digitalRead(BLUE_BUTTON) == LOW)
    {
      bluePushed = true;
      redFirst = true;
    }
  }
}

void bluePushedInt() {
  bluePushed = true;
  while(digitalRead(BLUE_BUTTON) == LOW)
  {
    if (digitalRead(RED_BUTTON) == LOW)
    {
      redPushed = true;
      redFirst = false;
    }
  }
}

void readButtons()
{
    if(digitalRead(RED_BUTTON) == LOW) {
      redPushed = true;
      while(digitalRead(RED_BUTTON) == LOW)
      {
        if(digitalRead(BLUE_BUTTON) == LOW) {
          redFirst = true;
          bluePushed = true;
        }
        delay(85);
      }
    }
    if(digitalRead(BLUE_BUTTON) == LOW) {
      bluePushed = true;
      while(digitalRead(BLUE_BUTTON) == LOW)
      {
        if(digitalRead(RED_BUTTON) == LOW) {
          redFirst = false;
          redPushed = true;
        }
        delay(85);
      }
    }
}

void setColorRow(CRGB *led, int num_leds, CRGB::HTMLColorCode colour)
{
  for(int i = 0; i < num_leds; i++)
  {
    led[i] = colour;
    led[i] %= ledBrightness;
  }
}

void setColourAllRows(CRGB ledArray[][NUM_LEDS], int num_rows, int num_leds, CRGB::HTMLColorCode colour)
{
  for(int i = 0; i < num_rows; i++)
  {
    setColorRow(ledArray[i], num_leds, colour);
  }
}

void fadeLightBy(CRGB *led, int num_leds, int percentage)
{
  for(int i = 0; i < num_leds; i++)
  {
    led[i] %= percentage;
  }
}

void fadeAllLightBy(CRGB ledArray[][NUM_LEDS], int num_rows, int num_leds, int percentage)
{
  for(int i = 0; i < num_rows; i++)
  {
    fadeLightBy(ledArray[i], num_leds, percentage);
  }
}


void allLedsOff(CRGB ledArray[][NUM_LEDS], int num_rows, int num_leds)
{
  for(int i=0; i < num_rows; i++)
  {
    setColorRow(ledArray[i], num_leds, CRGB::Black);
  }
  FastLED.show();
}

void allLedsOn(CRGB ledArray[][NUM_LEDS], int num_rows, int num_leds)
{
  for(int i=0; i < num_rows; i++)
  {
    setColorRow(ledArray[i], num_leds, currentColour);
  }
  FastLED.show();
}


void setup()
{
  Serial.begin(115200);
  FastLED.addLeds<NEOPIXEL, STRIP_0_PIN>(ledArray[0], NUM_LEDS);
  FastLED.addLeds<NEOPIXEL, STRIP_1_PIN>(ledArray[1], NUM_LEDS);
  FastLED.addLeds<NEOPIXEL, STRIP_2_PIN>(ledArray[2], NUM_LEDS);
  FastLED.addLeds<NEOPIXEL, STRIP_3_PIN>(ledArray[3], NUM_LEDS);
  FastLED.addLeds<NEOPIXEL, STRIP_4_PIN>(ledArray[4], NUM_LEDS);

  // digitalWrite(RED_BUTTON, HIGH);
  pinMode(RED_BUTTON, INPUT_PULLUP);
  // attachInterrupt(RED_BUTTON, redPushedInt, LOW);

  // digitalWrite(BLUE_BUTTON, HIGH);
  pinMode(BLUE_BUTTON, INPUT_PULLUP);
  // attachInterrupt(BLUE_BUTTON, bluePushedInt, LOW);

  currentColour = CRGB::Green;
  allLedsOn(ledArray, 5, NUM_LEDS);
  delay(300);
  allLedsOff(ledArray, 5, NUM_LEDS);
  delay(300);
  allLedsOn(ledArray, 5, NUM_LEDS);
  delay(300);
  allLedsOff(ledArray, 5, NUM_LEDS);
  randomSeed(analogRead(0));
  Serial.println("End setup");
}

void lightLedAccordingToState(boolean currentState)
{
  if (currentState)
  {
    FastLED.showColor(currentColour, ledBrightness);
  }
  else
  {
    FastLED.showColor(CRGB::Black);
  }
}
CRGB::HTMLColorCode pallet[] {CRGB::SkyBlue, CRGB::Blue, CRGB::DarkSlateBlue,
  CRGB::DarkGreen, CRGB::Yellow, CRGB::DarkOrange, CRGB::Red,
  CRGB::DarkRed, CRGB::Magenta, CRGB::White};

void colourMode(CRGB ledArray[][NUM_LEDS], int num_rows)
{
  allLedsOff(ledArray, num_rows, NUM_LEDS);
  int selected = 0;
  for(int i=0; i < 10; i++)
  {
    ledArray[OPTIONS_STRIP][i] = pallet[i];
    if (currentColour != pallet[i]) { ledArray[OPTIONS_STRIP][i] %= 25; }
    else { selected = i; }
  }
  FastLED.show();
  redPushed = false;
  bluePushed = false;
  Serial.println("Enter colourMode");
  while(!redPushed)
  {
    readButtons();
    if(bluePushed)
    {
      ledArray[OPTIONS_STRIP][selected].maximizeBrightness();
      ledArray[OPTIONS_STRIP][selected] %=25;
      selected = selected == 9? 0: selected+1;
      ledArray[OPTIONS_STRIP][selected].maximizeBrightness();
      FastLED.show();
      bluePushed = false;
    }
    currentColour = pallet[selected];
  }
  lightLedAccordingToState(ledState);
  Serial.println("Exit colourMode");
}

void brightnessMode(CRGB ledArray[][NUM_LEDS], int num_rows)
{
  int lvl = ledBrightness/25;
  int i = 0;
  allLedsOff(ledArray, 5, NUM_LEDS);
  // FastLED.showColor(CRGB::Black);
  for(; i < lvl; i++)
  {
    ledArray[OPTIONS_STRIP][i] = currentColour;
    ledArray[OPTIONS_STRIP][i] %= ledBrightness;
  }
  FastLED.show();
  redPushed = false;
  bluePushed = false;
  while(!redPushed)
  {
    readButtons();
    if (bluePushed)
    {
      lvl = lvl < 10? lvl+1: 1;
      int i = 0;
      for(; i < lvl; i++)
      {
        ledArray[OPTIONS_STRIP][i] = currentColour;
        ledArray[OPTIONS_STRIP][i] %= lvl*25;
      }
      for(; i < 10; i++)
      {
        ledArray[OPTIONS_STRIP][i] = CRGB::Black;
      }
      FastLED.show();
      bluePushed = false;
    }
  }
  ledBrightness = lvl*25;
}

CRGB::HTMLColorCode discoPallets[] {CRGB::Blue, CRGB::DarkSlateBlue,
  CRGB::DarkGreen, CRGB::Yellow, CRGB::DarkOrange, CRGB::Red, CRGB::Magenta};

int discoPatterns[][NUM_LEDS] = {
  {0,1,2,3,4,5,6,0,1,2},
  {1,0,3,4,1,4,5,0,2,5},
  {0,3,4,3,2,5,6,2,1,1},
  {3,4,2,0,4,2,6,0,1,2},
  {4,1,2,3,0,5,2,1,1,3},

  {5,6,2,3,4,0,1,2,3,4},
  {6,1,5,3,4,1,0,3,2,2},
  {0,5,2,3,1,5,3,0,1,2},
  {6,1,2,4,4,3,4,5,0,2},
  {0,2,1,3,3,4,6,0,1,0},
};
void discoMode()
{
  int line = 0;
  uint32_t point = millis();
  uint32_t speed = 200;
  redPushed  = false;
  bluePushed = false;
  redFirst = false;
  while(!redPushed)
  {
    readButtons();
    if (bluePushed)
    {
      speed = speed > 450? 50: speed +50;
      bluePushed = false;
    }
    if((millis() - point) > speed)
    {
      for(int i = 0; i < 5; i++)
      {
        line = random(0,7);
        for(int j=0; j < NUM_LEDS; j++)
        {
          ledArray[i][j] = discoPallets[discoPatterns[line][j]];
        }
      }
      point = millis();
      FastLED.show();
    }
    bluePushed  = false;

  }
  redFirst = false;
  redPushed  = false;
  bluePushed = false;
}

void loop()
{
  readButtons();
  if (redPushed && bluePushed)
  {
    redPushed = false;
    bluePushed = false;
    if (redFirst)
    {
      brightnessMode(ledArray, 5);
    }
    else
    {
      redPushed = false;
      discoMode();
    }
    lightLedAccordingToState(ledState);

    redPushed = false;
    bluePushed = false;
    redFirst = false;
  }
  else if (redPushed)
  {
    colourMode(ledArray, 5);
    redPushed = false;
    bluePushed = false;
  }
  else if (bluePushed)
  {
    ledState = !ledState;
    lightLedAccordingToState(ledState);
    bluePushed = false;
  }

}
