//based off of https://editor.soulmatelights.com/gallery/743-sand-automata-level-control

#include <FastLED.h>
#define LED_PIN     D1
#define BRIGHTNESS  50
#define LED_TYPE    WS2812B
#define COLOR_ORDER GRB
const uint8_t kMatrixWidth  = 24;
const uint8_t kMatrixHeight = 24;
#define WIDTH kMatrixWidth
#define HEIGHT kMatrixHeight
#define ROWS kMatrixWidth
#define COLS kMatrixHeight
#define LED_ROWS kMatrixWidth
#define LED_COLS kMatrixHeight
const bool    kMatrixSerpentineLayout = true;
#define NUM_LEDS (kMatrixWidth * kMatrixHeight)
#define N_LEDS NUM_LEDS
#define MAX_DIMENSION ((kMatrixWidth>kMatrixHeight) ? kMatrixWidth : kMatrixHeight)
CRGB leds[kMatrixWidth * kMatrixHeight];
uint16_t offset = 0;
void setup() 

{
  delay(3000);
  LEDS.addLeds<LED_TYPE,LED_PIN,COLOR_ORDER>(leds,NUM_LEDS);
  LEDS.setBrightness(BRIGHTNESS);  
}


void randomdot() {
  offset++;
  //byte a= LED_COLS/2; //
  byte a = random8(LED_COLS / 4) + LED_COLS * 3 / 8; //
  if (!random8(4)) leds[XY(a, LED_ROWS - 1)].setHue(offset * 15); // 0 or 1
  if (offset > 250) {
    offset = 10;
  }
      
}

void updatesand() {
  int index, indexXadd1Y, indexXsub1Y, indexXYadd1;
  for (int y = 0; y < LED_ROWS - 1; y++) {
    for (int x = 1; x < LED_COLS - 1; x++) {
      index = XY(x, y);
      indexXadd1Y = XY(x + 1, y);
      indexXsub1Y = XY(x - 1, y);
      indexXYadd1 = XY(x, y + 1);
      if (!leds[index] && !leds[indexXYadd1]) continue;
      if (!leds[index] && leds[indexXYadd1]) {
        leds[index] = leds[indexXYadd1];
        leds[indexXYadd1] = 0;
      }
      if (leds[index] && leds[indexXYadd1] && !leds[indexXsub1Y] && !leds[indexXadd1Y]) {
        if (random8(4)) {
          leds[indexXsub1Y] = leds[indexXYadd1];
          leds[indexXYadd1] = 0;
      } else {
          leds[indexXadd1Y] = leds[indexXYadd1];
          leds[indexXYadd1] = 0;
        }
      }
      if (leds[index] && leds[indexXYadd1] && !leds[indexXsub1Y] && leds[indexXadd1Y]) {
        leds[indexXsub1Y] = leds[indexXYadd1];
        leds[indexXYadd1] = 0;
      }
      if (leds[index] && leds[indexXYadd1] && leds[indexXsub1Y] && !leds[indexXadd1Y]) {
        leds[indexXadd1Y] = leds[indexXYadd1];
        leds[indexXYadd1] = 0;
      }
    }
  }
}

void randomdel() {
  for (int i = 0; i < N_LEDS; i++) {
    if (!random8(20)) leds[i] = 0;
  }
  //leds[XY(0, 0)] = 0;
}

void falldown() {
  for (int y = 0; y < LED_ROWS - 1; y++) {
    for (int x = 0; x < LED_COLS; x++) {
      if (!leds[XY(x, y)] && leds[XY(x, y + 1)]) {
        leds[XY(x, y)] = leds[XY(x, y + 1)];
        leds[XY(x, y + 1)] = 0;
      }
    }
  }
}

void draw() {
  EVERY_N_MILLISECONDS(1) {
    updatesand();
    randomdot();
  }
  // Level controled by LED_ROWS/3   
  if ((uint32_t) leds[XY(0, LED_ROWS / 3)] > 0) {
    EVERY_N_MILLISECONDS(2000) {
      randomdel();
      falldown();
      falldown();
      falldown();
    }
  }
}
void loop() {

  draw();
  LEDS.show();

}
uint16_t XY(uint8_t x, uint8_t y)
{
  uint16_t i;
  if (kMatrixSerpentineLayout == false)
  {
    i = (y * kMatrixWidth) + x;
  }
  if (kMatrixSerpentineLayout == true)
  {
    if (y & 0x01)
    {
      // Odd rows run backwards
      uint8_t reverseX = (kMatrixWidth - 1) - x;
      i = (y * kMatrixWidth) + reverseX;
    }
    else
    {
      // Even rows run forwards
      i = (y * kMatrixWidth) + x;
    }
  }
  return i;
}
