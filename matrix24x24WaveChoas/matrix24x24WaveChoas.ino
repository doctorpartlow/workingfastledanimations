#include <fastled.h>
#define LED_PIN 42    //this is for the esp32-s3 
#define BRIGHTNESS 75 //it's blindingly bright at 255 or looks silly if you don't have enough power injected
#define LED_TYPE WS2812B
#define COLOR_ORDER GRB
#define WIDTH (COLS)
#define HEIGHT (ROWS)
#define NUM_LEDS (N_LEDS)
#define NUM_PARTICLES 50
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
uint8_t hue ;                                 // постепенный сдвиг оттенка или какой-нибудь другой цикличный счётчик
uint8_t deltaHue, deltaHue2;                       // ещё пара таких же, когда нужно много
uint8_t step;                                      // какой-нибудь счётчик кадров или последовательностей операций
uint8_t pcnt;                                      // какой-то счётчик какого-то прогресса
uint8_t deltaValue;                                // просто повторно используемая переменная
float speedfactor;                                 // регулятор скорости в эффектах реального времени
float emitterX, emitterY;                          // какие-то динамичные координаты
CRGB ledsbuff[NUM_LEDS];                           // копия массива leds[] целиком
#define NUM_LAYERSMAX 2
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

uint8_t i = 0;
uint8_t j = COLS; 
uint8_t offset = 1;
uint8_t m = 5;
void draw() {
  for (i = 0; i < ROWS; i+=1){
    for (j = COLS; j > 0;j -= 1){
      leds[XY(i,j)] = CHSV(offset,m,offset);
      EVERY_N_MILLISECONDS(m) { m += random(1,5); }
      if (m >= 254) m=1;
      offset = offset + (beatsin16(offset,m,offset+m));
    }
  }
 blur2d(leds, LED_COLS, LED_ROWS, 145);
}

void setup()
{
  delay(3000);
  LEDS.addLeds<LED_TYPE, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS);
  LEDS.setBrightness(BRIGHTNESS);
}

void loop()
{
draw();
LEDS.show();
}

