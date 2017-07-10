# nightLightStairs

## Overview
Night light based on NeoPixel strips and Arduino.
Tested using Arduino mkr1000USB and a mega 2560 v3.

## Dependencies
* [FastLED](http://www.fastled.io)

## User guide
The program lights up a number of strips.
With two buttons a number of settings can be adjusted.

|State  |Button | Action |
|-------|:------|--------|
|Root   |blue   |Change led state ON/OFF|
|Root   |red    |Enter colour mode|
|Colour mode| blue | Change colour|
|Colour mode| red  | Select colour, return to root|
|Root   |red + blue| Enter brightness mode|
|Brightness mode| blue| Change brightness|
|Brightness mode| red| Select brightness, return to root|
|Root| blue + red| Enter disco mode|
|Disco mode| blue| Change led flash speed|
|Disco mode| red| Return to root|
