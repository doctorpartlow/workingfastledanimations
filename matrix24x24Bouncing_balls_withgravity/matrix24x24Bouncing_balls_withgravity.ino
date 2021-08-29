//based off of https://editor.soulmatelights.com/gallery/645-jumping-balls

#include <FastLED.h>
#define LED_PIN     D1  //this is for the esp8266
#define BRIGHTNESS  50 //it's blindingly bright at 255 or looks silly if you don't have enough power injected
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
CRGB leds[kMatrixWidth * kMatrixHeight];
const bool    kMatrixSerpentineLayout = true;
#define NUM_LEDS (kMatrixWidth * kMatrixHeight)
#define enlargedOBJECT_MAX_COUNT WIDTH*2
#define SPEED_ADJ (float)NUM_LEDS/512

//speed control
byte speed = 127; // 1-255

//scale control
byte scale = 150; //1-255

void setup() 
{
  delay(3000);
  LEDS.addLeds<LED_TYPE,LED_PIN,COLOR_ORDER>(leds,NUM_LEDS);
  LEDS.setBrightness(BRIGHTNESS);   
}
static const TProgmemRGBPalette16 MagmaColor_p FL_PROGMEM = {CRGB::Black, 0x240000, 0x480000, 0x660000, 0x9a1100, 0xc32500, 0xd12a00, 0xe12f17, 0xf0350f, 0xff3c00, 0xff6400, 0xff8300, 0xffa000, 0xffba00, 0xffd400, 0xffffff};                      
extern const TProgmemRGBPalette16 WoodFireColors_p FL_PROGMEM = {CRGB::Black, 0x330e00, 0x661c00, 0x992900, 0xcc3700, CRGB::OrangeRed, 0xff5800, 0xff6b00, 0xff7f00, 0xff9200, CRGB::Orange, 0xffaf00, 0xffb900, 0xffc300, 0xffcd00, CRGB::Gold};     
extern const TProgmemRGBPalette16 NormalFire_p FL_PROGMEM = {CRGB::Black, 0x330000, 0x660000, 0x990000, 0xcc0000, CRGB::Red, 0xff0c00, 0xff1800, 0xff2400, 0xff3000, 0xff3c00, 0xff4800, 0xff5400, 0xff6000, 0xff6c00, 0xff7800};                     
extern const TProgmemRGBPalette16 NormalFire2_p FL_PROGMEM = {CRGB::Black, 0x560000, 0x6b0000, 0x820000, 0x9a0011, CRGB::FireBrick, 0xc22520, 0xd12a1c, 0xe12f17, 0xf0350f, 0xff3c00, 0xff6400, 0xff8300, 0xffa000, 0xffba00, 0xffd400};              
extern const TProgmemRGBPalette16 LithiumFireColors_p FL_PROGMEM = {CRGB::Black, 0x240707, 0x470e0e, 0x6b1414, 0x8e1b1b, CRGB::FireBrick, 0xc14244, 0xd16166, 0xe08187, 0xf0a0a9, CRGB::Pink, 0xff9ec0, 0xff7bb5, 0xff59a9, 0xff369e, CRGB::DeepPink};
extern const TProgmemRGBPalette16 SodiumFireColors_p FL_PROGMEM = {CRGB::Black, 0x332100, 0x664200, 0x996300, 0xcc8400, CRGB::Orange, 0xffaf00, 0xffb900, 0xffc300, 0xffcd00, CRGB::Gold, 0xf8cd06, 0xf0c30d, 0xe9b913, 0xe1af1a, CRGB::Goldenrod};   
extern const TProgmemRGBPalette16 CopperFireColors_p FL_PROGMEM = {CRGB::Black, 0x001a00, 0x003300, 0x004d00, 0x006600, CRGB::Green, 0x239909, 0x45b313, 0x68cc1c, 0x8ae626, CRGB::GreenYellow, 0x94f530, 0x7ceb30, 0x63e131, 0x4bd731, CRGB::LimeGreen}; 
extern const TProgmemRGBPalette16 AlcoholFireColors_p FL_PROGMEM = {CRGB::Black, 0x000033, 0x000066, 0x000099, 0x0000cc, CRGB::Blue, 0x0026ff, 0x004cff, 0x0073ff, 0x0099ff, CRGB::DeepSkyBlue, 0x1bc2fe, 0x36c5fd, 0x51c8fc, 0x6ccbfb, CRGB::LightSkyBlue}; 
extern const TProgmemRGBPalette16 RubidiumFireColors_p FL_PROGMEM = {CRGB::Black, 0x0f001a, 0x1e0034, 0x2d004e, 0x3c0068, CRGB::Indigo, CRGB::Indigo, CRGB::Indigo, CRGB::Indigo, CRGB::Indigo, CRGB::Indigo, 0x3c0084, 0x2d0086, 0x1e0087, 0x0f0089, CRGB::DarkBlue}; 
extern const TProgmemRGBPalette16 PotassiumFireColors_p FL_PROGMEM = {CRGB::Black, 0x0f001a, 0x1e0034, 0x2d004e, 0x3c0068, CRGB::Indigo, 0x591694, 0x682da6, 0x7643b7, 0x855ac9, CRGB::MediumPurple, 0xa95ecd, 0xbe4bbe, 0xd439b0, 0xe926a1, CRGB::DeepPink}; 

static double fmap(const double x, const double in_min, const double in_max, const double out_min, const double out_max){
    return (out_max - out_min) * (x - in_min) / (in_max - in_min) + out_min;
}

#define SPARKS_AM WIDTH
float FADE_KOEF = 10;
float SpeedK = .98;
float SpeedDecX = .01;
float SpeedDecY = 0;
#define Board 1
#define GravityX 0
#define GravityY 1

int sparksPos[2][SPARKS_AM];
float sparksSpeed[2][SPARKS_AM];
float sparksFade[SPARKS_AM];
byte sparksColor[SPARKS_AM];

int genPos[2];
int gravityPos[2];
bool run = true;

bool loadingFlag = true;
void reg(byte id) {
  sparksPos[0][id] = genPos[0];
  sparksPos[1][id] = genPos[1];
  sparksSpeed[0][id] = random(-10, 10);
  sparksSpeed[1][id] = random(-10, 10);
  sparksFade[id] = 255;
  sparksColor[id] = random();
}

void physics(byte id) {
  if (SpeedK) {
    if (GravityX) {
      if (gravityPos[0] < sparksPos[0][id])
        sparksSpeed[0][id] -= SpeedK;
      else
        sparksSpeed[0][id] += SpeedK;
    }
    if (GravityY) {
      if (gravityPos[1] < sparksPos[1][id])
        sparksSpeed[1][id] -= SpeedK;
      else
        sparksSpeed[1][id] += SpeedK;
    }
  }
  sparksFade[id] -= (255. / (float)((HEIGHT + WIDTH) * FADE_KOEF));
  if (SpeedDecX || sparksSpeed[0][id]) {
    if (sparksSpeed[0][id] > 0)
      sparksSpeed[0][id] -= SpeedDecX;
    else
      sparksSpeed[0][id] += SpeedDecX;
    if (abs(sparksSpeed[0][id]) <= SpeedDecX)
      sparksSpeed[0][id] = 0;
  }
  if (SpeedDecY || sparksSpeed[1][id]) {
    if (sparksSpeed[1][id] > 0)
      sparksSpeed[1][id] -= SpeedDecY;
    else
      sparksSpeed[1][id] += SpeedDecY;
    if (abs(sparksSpeed[1][id]) <= SpeedDecY)
      sparksSpeed[1][id] = 0;
  }
  if (Board) { if (sparksPos[0][id] < 0 || sparksPos[0][id] >= WIDTH * 10) sparksSpeed[0][id] = -sparksSpeed[0][id]; if (sparksPos[1][id] < 0) sparksSpeed[1][id] = -sparksSpeed[1][id]; }
    sparksPos[0][id] += constrain(sparksSpeed[0][id], -10, 10);
    sparksPos[1][id] += constrain(sparksSpeed[1][id], -10, 10);
}

void wu_pixel(uint32_t x, uint32_t y, CRGB * col) { //awesome wu_pixel procedure by reddit u/sutaburosu
  // extract the fractional parts and derive their inverses
  uint8_t xx = x & 0xff, yy = y & 0xff, ix = 255 - xx, iy = 255 - yy;
  // calculate the intensities for each affected pixel
  #define WU_WEIGHT(a, b)((uint8_t)(((a) * (b) + (a) + (b)) >> 8))
  uint8_t wu[4] = {
    WU_WEIGHT(ix, iy),
    WU_WEIGHT(xx, iy),
    WU_WEIGHT(ix, yy),
    WU_WEIGHT(xx, yy)
  };
  // multiply the intensities by the colour, and saturating-add them to the pixels
  for (uint8_t i = 0; i < 4; i++) {
    uint16_t xy = XY((x >> 8) + (i & 1), (y >> 8) + ((i >> 1) & 1));
    leds[xy].r = qadd8(leds[xy].r, col -> r * wu[i] >> 8);
    leds[xy].g = qadd8(leds[xy].g, col -> g * wu[i] >> 8);
    leds[xy].b = qadd8(leds[xy].b, col -> b * wu[i] >> 8);
  }
}

void render(byte id, CRGB Col) {
  if (loadingFlag) {
    for (byte i = 0; i < SPARKS_AM; i++) {
      reg(i);
      for (byte a = 0; a < i; a++) {
        physics(a);
      }
    }
    loadingFlag = false;
  }
  physics(id);
  if (sparksPos[1][id] < ((HEIGHT - 1) * 10) and sparksPos[1][id] >= 0)
    if (sparksPos[0][id] < ((WIDTH - 1) * 10) and sparksPos[0][id] >= 0) {
      CRGB color = Col;
      wu_pixel(sparksPos[0][id] * 25.6, sparksPos[1][id] * 25.6, & color);
    }
}

void setGenPos(int x, int y) {
  genPos[0] = x;
  genPos[1] = y;
}

void setGravityPos(int x, int y) {
  gravityPos[0] = x;
  gravityPos[1] = y;
}

void setRegenRule(byte id, bool b) {
  if (b) reg(id);
}

void draw() {
  fadeToBlackBy(leds, NUM_LEDS, 20);
  setGenPos(beatsin16(10, 0, WIDTH * 10), beatsin16(10, 0, HEIGHT * 10, 0, 16384));
  setGravityPos(0, 0);
  for (byte i = 0; i < SPARKS_AM; i++) {
    setRegenRule(i, (sparksFade[i] <= 35) ? 1 : 0);
    render(i, CHSV(sparksColor[i], 255, constrain(sparksFade[i], 32, 255)));
  }
  delay(16);
}

void loop(){

draw();
LEDS.show();
}

uint16_t XY( uint8_t x, uint8_t y)
{
  uint16_t i;
  if( kMatrixSerpentineLayout == false) {
    i = (y * kMatrixWidth) + x;
  }
  if( kMatrixSerpentineLayout == true) {
    if( y & 0x01) {
      // Odd rows run backwards
      uint8_t reverseX = (kMatrixWidth - 1) - x;
      i = (y * kMatrixWidth) + reverseX;
    } else {
      // Even rows run forwards
      i = (y * kMatrixWidth) + x;
    }
  }
  return i;
}
