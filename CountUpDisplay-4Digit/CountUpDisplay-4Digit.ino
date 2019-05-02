#include <Arduino.h>

//Board used: Digispark Tiny 85
//Select Board: Digispark (Default - 16.5mhz)

// Input connector -  wire switch/button to pin below and 5v.
#define SwitchPin 1

// TM1637 4-Digit Display
#include "TM1637.h"
#define CLK 2
#define DIO 3
TM1637 tm1637(CLK, DIO);

// WS2812 info
#include <Adafruit_NeoPixel.h>
#define PIXEL_PIN    0    // Digital IO pin connected to the WS2812
#define PIXEL_COUNT  1
Adafruit_NeoPixel strip = Adafruit_NeoPixel(PIXEL_COUNT, PIXEL_PIN, NEO_GRB + NEO_KHZ800);

// Color changing times, in milliseconds
long Timer1  = 32400000; // 9hrs When to start the color change from green
long Timer2  = 39600000; // 11hrs When to set the color to full red
long Timer3  = 43200000; // 12hrs When to set the color to flashing red

int8_t TimeDisp[] = {0x00, 0x00, 0x00, 0x00};
boolean SwitchState = false;
long LastSwitch = 0;

void setup() {
  //Initialize the switch pin and variables
  pinMode(SwitchPin, INPUT);
  SwitchState = digitalRead(SwitchPin);
  LastSwitch = millis();  // the last time the switch was toggled, update to now

  //Initialize the tm1637 display
  tm1637.set(BRIGHT_TYPICAL); //BRIGHT_TYPICAL = 2,BRIGHT_DARKEST = 0,BRIGHTEST = 7;
  tm1637.init();

  //Initialize the ws2812
  strip.begin();
  strip.show(); // Initialize all pixels to 'off'
  strip.setBrightness(255);

}


void loop() {
  int reading = digitalRead(SwitchPin);          // read the state of the switch value:
  if (reading != SwitchState) {                  // if the switch state has changed
    SwitchState = reading;                       // update SwitchState
    LastSwitch = millis();                       // update last change to now
  }
  
  long Elapsed = (millis() - LastSwitch);        // Calculate the time since the switch has changed
  int VarHrs = (Elapsed / 60 / 60 / 1000);       // Calculate hours
  int VarMin = (Elapsed / 60 / 1000) % 60;       // Calculate minutes
  int VarSec = (Elapsed / 1000) % 60;            // Calculate seconds

  //First, update the numeric display
  if (VarHrs < 100) {
    if (VarHrs < 1) {                            // if it's been less than 1 hour, show mm:ss
      TimeDisp[0] = VarMin / 10;
      TimeDisp[1] = VarMin % 10;
      TimeDisp[2] = VarSec / 10;
      TimeDisp[3] = VarSec % 10;
    } else {                                     // if it's been more than 1 hour, show hh:mm
      TimeDisp[0] = VarHrs / 10;
      TimeDisp[1] = VarHrs % 10;
      TimeDisp[2] = VarMin / 10;
      TimeDisp[3] = VarMin % 10;
    }
  } else {                                       // if it's been more than 99 hours, show 99:99
    TimeDisp[0] = 9;
    TimeDisp[1] = 9;
    TimeDisp[2] = 9;
    TimeDisp[3] = 9;
  }
  tm1637.display(TimeDisp);                      // update the display

  // Toggle the point every half a second
  if ((Elapsed % 1000) < 500) {
    tm1637.point(POINT_ON);
  } else {
    tm1637.point(POINT_OFF);
  }

  // Now, update the indicator light
  // Start with a low green, for always on
  byte IndR = 0;
  byte IndG = 32;
  byte IndB = 0;

  if ((Elapsed >= Timer1) && (Elapsed < Timer2)) { //If it's between Timer1 and Timer2
    byte Progress = (float)(Elapsed - Timer1) * 255 / (Timer2 - Timer1); //Set variable (0-255) that shows the progress between Timer1 and Timer2
    if (Progress < 128) { //If it's under 50%
      if (Progress > IndG) { //If progress is more than the existing green
        IndG = Progress; //then increase green to make yellow
      }
    } else { // if it's over 50%
      IndG = 255 - Progress; //then decrease green to make red
    }
    IndR = Progress; //Increase red between Timer1 and Timer2
  }

  if ((Elapsed >= Timer2) && (Elapsed < Timer3)) { //If it's between Timer2 and Timer3
    IndG = 0;
    IndR = 255; //Make it solid Red
  }

  if (Elapsed >= Timer3) { //If it's after Timer3
    if ((Elapsed % 1000) < 500) {  //Blink
      IndR = 255;
      IndG = 0;
    } else {
      IndR = 255;
      IndG = 255;
      IndB = 255;
    }
  }

  strip.setPixelColor(0, strip.Color(IndR, IndG, IndB));
  strip.show();

  delay(100);
}
