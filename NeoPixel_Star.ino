#include "FastLED.h"

FASTLED_USING_NAMESPACE


#define DATA_PIN    2
#define LED_TYPE    WS2812
#define COLOR_ORDER GRB
#define NUM_LEDS    10
CRGB leds[NUM_LEDS];

#define BRIGHTNESS         255  // 0-255
#define FRAMES_PER_SECOND  120

void setup() {
  delay(500); // delay for recovery

  // tell FastLED about the LED strip configuration
  FastLED.addLeds<LED_TYPE, DATA_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);

  // set master brightness control
  FastLED.setBrightness(BRIGHTNESS);
}

// List of patterns to cycle through.  Each is defined as a separate function below.
typedef void (*SimplePatternList[])();
SimplePatternList gPatterns = { rainbow, rainbowWithGlitter, colorwheel, oddeven, colorwipeX1, colorwipeX2, colorwipeX3, tim, tim2, tim3, HarkHowTheBells, confetti, sinelon, onlyGlitter, juggle, bpm };
//SimplePatternList gPatterns = { HarkHowTheBells };     // use this line to display only one pattern

uint8_t gCurrentPatternNumber = 0; // Index number of which pattern is current
uint8_t gHue = 0; // rotating "base color" used by many of the patterns


void loop() {
  // Call the current pattern function once, updating the 'leds' array
  gPatterns[gCurrentPatternNumber]();

  // send the 'leds' array out to the actual LED strip
  FastLED.show();
  // insert a delay to keep the framerate modest
  FastLED.delay(1000 / FRAMES_PER_SECOND);

  // do some periodic updates
  EVERY_N_MILLISECONDS( 20 ) {
    gHue++;  // slowly cycle the "base color" through the rainbow
  }
  EVERY_N_SECONDS( 10 ) {
    nextPattern();  // change patterns periodically
  }
}

#define ARRAY_SIZE(A) (sizeof(A) / sizeof((A)[0]))

void nextPattern() {
  // add one to the current pattern number, and wrap around at the end
  gCurrentPatternNumber = (gCurrentPatternNumber + 1) % ARRAY_SIZE( gPatterns);
}

void tim() {
  // Idea by tim, for 5 point star with 10 LED's, lights one point up, then skips one, repeat
  int stepnum = (millis() / 500) % 5;
  fadeToBlackBy( leds, NUM_LEDS, 1);
  for (int i; i < NUM_LEDS; i++) {
    if (i == (stepnum * 4) % (NUM_LEDS)) {
      leds[i] = CHSV(gHue, 255, 255);
      leds[i + 1] = CHSV(gHue, 255, 255);
    }
  }
}

void tim2() {
  // Another by Tim, makes ever other light red or green, and alternates every second.
  int oddeven = (millis() / 1000) % 2;
  for (int i; i < NUM_LEDS; i++) {
    if (oddeven == i % 2) {
      leds[i] = CRGB::Red;
    } else {
      leds[i] = CRGB::Green;
    }
  }
}

void tim3() {
  // Yet another by Tim, red and green glitter
  fadeToBlackBy( leds, NUM_LEDS, 100);
  if ( random8() < 50) {
    if (random8() < 128 ) {
      leds[ random16(NUM_LEDS) ] += CRGB::Red;
    } else {
      leds[ random16(NUM_LEDS) ] += CRGB::Green;
    }
  }
}

void HarkHowTheBells() {
  fadeToBlackBy( leds, NUM_LEDS, 2);
  int beat = (millis() / 200) % 6;
  int point = (millis() / 200) % NUM_LEDS;
  if (beat == 0) {
    leds[point] = CRGB::White;
    leds[point+1] = CRGB::White;
  }
  if (beat == 1) {
    leds[point] = CRGB::White;
    leds[point-1] = CRGB::White;
  }
  if (beat == 2) {
    leds[point+1] = CHSV(gHue, 255, 255);
  }
  if (beat == 3) {
    leds[point-1] = CHSV(gHue, 255, 255);
  }
  if (beat == 4) {
    leds[point] = CHSV(gHue, 255, 255);
    leds[point+1] = CHSV(gHue, 255, 255);
  }
  if (beat == 5) {
// rest
  }
}

void testdot() {
  // makes ever other light on every second. Used to troubleshoot cross light bleed
  int oddeven = (millis() / 1000) % 2;
  fill_solid( leds, NUM_LEDS, CRGB::Black);
  for (int i; i < NUM_LEDS; i++) {
    if (oddeven == i % 2) {
      leds[i] = CRGB::White;
    }
  }
}

void testdot2() {
  // toggles between led 4 and 5 every second, also used to troubleshoot crosslight bleed
  fill_solid( leds, NUM_LEDS, CRGB::Black);
  if ((millis() / 1000) % 2 == 0) {
    leds[4] = CRGB::White;
  } else {
    leds[5] = CRGB::White;
  }
}


void rainbow() {
  // FastLED's built-in rainbow generator
  fill_rainbow( leds, NUM_LEDS, gHue, 7);
}

void rainbowWithGlitter() {
  // built-in FastLED rainbow, plus some random sparkly glitter
  rainbow();
  addGlitter(80);
}

void addGlitter( fract8 chanceOfGlitter) {
  if ( random8() < chanceOfGlitter) {
    leds[ random16(NUM_LEDS) ] += CRGB::White;
  }
}

void onlyGlitter() {
  fadeToBlackBy( leds, NUM_LEDS, 100);
  addGlitter(50);
}

void confetti() {
  // random colored speckles that blink in and fade smoothly
  fadeToBlackBy( leds, NUM_LEDS, 10);
  int pos = random16(NUM_LEDS);
  leds[pos] += CHSV( gHue + random8(64), 200, 255);
}

void sinelon() {
  // a colored dot sweeping back and forth, with fading trails
  fadeToBlackBy( leds, NUM_LEDS, 20);
  int pos = beatsin16(13, 0, NUM_LEDS);
  leds[pos] += CHSV( gHue, 255, 192);
}

void bpm() {
  // colored stripes pulsing at a defined Beats-Per-Minute (BPM)
  uint8_t BeatsPerMinute = 62;
  CRGBPalette16 palette = PartyColors_p;
  uint8_t beat = beatsin8( BeatsPerMinute, 64, 255);
  for ( int i = 0; i < NUM_LEDS; i++) { //9948
    leds[i] = ColorFromPalette(palette, gHue + (i * 2), beat - gHue + (i * 10));
  }
}

void colorwipeX(int numPoints) {
  //Continuous wipe with configurable number of dots
  fadeToBlackBy( leds, NUM_LEDS, 20 * numPoints);
  int pointPitch = NUM_LEDS / numPoints;
  int inc = (millis() / 150) % pointPitch;
  for ( int i = 0; i < NUM_LEDS; i++ ) {
    if (i % pointPitch == inc) {
      leds[i] = CHSV(gHue, 255, 255);
    }
  }
}

void colorwipeX1() {
  colorwipeX(1);
}
void colorwipeX2() {
  colorwipeX(2);
}
void colorwipeX3() {
  colorwipeX(3);
}

void colorwheel() {
  //Sweeps entire rainbow accross all leds
  fill_rainbow( leds, NUM_LEDS, gHue, 255 / NUM_LEDS);
}

void oddeven() {
  //Alternates between odd and even leds
  for (int i; i < NUM_LEDS; i++) {
    if ((millis() / 1000) % 2 == i % 2) {
      leds[i] = CHSV(gHue, 255, 255);
    } else {
      leds[i] = CRGB::Black;
    }
  }
}

void juggle() {
  // 3 colored dots, weaving in and out of sync with each other
  fadeToBlackBy( leds, NUM_LEDS, 20);
  byte dothue = 0;
  for ( int i = 0; i < 3; i++) {
    leds[beatsin16(i + 7, 0, NUM_LEDS)] |= CHSV(dothue, 200, 255);
    dothue += 32;
  }
}

