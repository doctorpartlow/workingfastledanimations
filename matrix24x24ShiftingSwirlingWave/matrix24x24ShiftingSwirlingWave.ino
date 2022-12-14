//change the delay at the bottom if it's too slow. this was ran an esp32-s3 and was nice and smooth.
#include <Arduino.h>
#include <FastLED.h>
#define LED_PIN 42    //this is for the esp32-s3 
#define BRIGHTNESS 150 //it's blindingly bright at 255 or looks silly if you don't have enough power injected
#define LED_TYPE WS2812B
#define COLOR_ORDER GRB
const uint8_t kMatrixWidth = 24;
const uint8_t kMatrixHeight = 24;
#define WIDTH kMatrixWidth
#define HEIGHT kMatrixHeight
#define ROWS kMatrixWidth
#define COLS kMatrixHeight
#define LED_ROWS kMatrixWidth
#define LED_COLS kMatrixHeight
CRGB leds[kMatrixWidth * kMatrixHeight];
const bool kMatrixSerpentineLayout = true;
#define NUM_LEDS (kMatrixWidth * kMatrixHeight)
#define NUM_PARTICLES 15  //set this to the number of particles. the varialbe describes what it's supposed to be. it works with 50 but it's a little slow. on an esp32 it looks pretty nice at that number 15 is a safe number
 bool loadingFlag = true;  
static const int count = NUM_PARTICLES;
int8_t zD;
int8_t zF;
uint8_t noise3d[LED_COLS][LED_ROWS];
uint32_t noise32_x;
uint32_t noise32_y;
uint32_t noise32_z;
uint32_t scale32_x;
uint32_t scale32_y;
bool isSetup = 1;
uint8_t noisesmooth;
bool eNs_isSetupped;

//This is how the pixels is placed on the matrix. 
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
int iterator = -1;
int otheriterator = 1;
bool switchflag = true;
//modified stepkos pattern a little. 
bool setupm = 1;
#define C_X LED_COLS / 2
#define C_Y LED_ROWS / 2
byte XY_angle[LED_COLS][LED_ROWS];
byte XY_radius[LED_COLS][LED_ROWS];
int number = 1;
int othernumber = 11;
void draw() {
 // FastLED.clear();
  if (setupm) {
    setupm = 0;
    for (int8_t x = -C_X; x < C_X + (LED_COLS % 2); x++) {
      for (int8_t y = -C_Y; y < C_Y + (LED_ROWS % 2); y++) {
        // XY_angle[x + C_X][y + C_Y] = atan2(y, x) * (180. / 2. / PI) * LED_COLS;
        XY_angle[x + C_X][y + C_Y] =  sin16((atan2(x, y) / PI) * 128 + 127); //thanks ldirko 
        XY_radius[x + C_X][y + C_Y] = hypot(x, y); //thanks Sutaburosu
      }
    }
  }

   EVERY_N_MILLISECONDS(5000) {  
    if (number <= 1 || number >= 10) {
      iterator = -iterator;
    }
     number += iterator;
    }
    EVERY_N_MILLISECONDS(4000) {  
    
    
    if (othernumber <= 1 || othernumber >= 15) {
      otheriterator = -otheriterator;
    }
     othernumber += otheriterator;
    
    }
  static byte speed = number;
  static uint32_t t;
  t += speed;
  for (uint8_t x = 0; x < LED_COLS; x++) {
    for (uint8_t y = 0; y < LED_ROWS; y++) {
      byte angle = XY_angle[x][y];
      byte radius = XY_radius[x][y];
      leds[XY(x, y)] = CHSV(t + radius * (255 / LED_COLS) - 20, 255, sin8(cos8(t+angle*number+( radius * (255 / LED_COLS)))+t * othernumber + cos8(t * othernumber - radius * (255 / LED_COLS)) + angle * number*2));
      //delayMicroseconds(100);
    }
  }
  
}

void setup()
{
  delay(3000);
  LEDS.addLeds<LED_TYPE, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS);
  LEDS.setBrightness(BRIGHTNESS);
}

void loop(){
draw();

LEDS.show();
delayMicroseconds(25000);
}