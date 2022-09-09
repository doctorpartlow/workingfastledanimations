
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
                            // просто повторно используемая переменная
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

const uint8_t COUNT = 15;
float posx[COUNT];
float posy[COUNT];
CRGB color;
float velx[COUNT], vely[COUNT];
float accel[COUNT];
byte initt = 1;
void drawPixelXYF(float x, float y, CRGB color) {
  if (x < 0 || y < 0 || x > ((float) LED_COLS - 1) || y > ((float) LED_ROWS - 1)) return;
  uint8_t xx = (x - (int) x) * 255, yy = (y - (int) y) * 255, ix = 255 - xx, iy = 255 - yy;
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
    int16_t xn = x + (i & 1), yn = y + ((i >> 1) & 1);
    CRGB clr = leds[XY(xn, yn)];
    clr.r = qadd8(clr.r, (color.r * wu[i]) >> 8);
    clr.g = qadd8(clr.g, (color.g * wu[i]) >> 8);
    clr.b = qadd8(clr.b, (color.b * wu[i]) >> 8);
    leds[XY(xn, yn)] = clr;
  }
}
void initvalues(){
  for (int i = 0; i < COUNT;i++){
    velx[i] = (beatsin16(random(1,2) + vely[i],0,COLS,ROWS)/ROWS)+0.1F;
    vely[i] = (beatsin16(5 + vely[i],0,COLS,ROWS)/ROWS)+0.5F;
    posx[i] = random (1.01F,float(ROWS));
    posy[i] = random (1.0F,float(COLS));
    accel[i] = random(0.1F,1.0F);
  }
}
float newpos(){
  return random(0.1F,22.0F);
}
float newvel(){
  return random(0.1F,1.0F);
}
void draw() {
  if (initt == 1){
    initvalues();
    initt=0;
  }
  
    for (int i = 0; i < COUNT; i++)
    {
        if (posx[i] < 1 || posx[i] > COLS -2 ) 
        { 
            velx[i] = -velx[i];
            if (velx[i] * velx[i] > 5)
            {
                velx[i] = newvel();
            }
            if (posx[i] < 0 || posx[i] > COLS)
            {
                posx[i] = newpos();
            }        
        }
        if (posy[i] < 1 || posy[i] > ROWS -2 )
        {    
            vely[i] = -vely[i];
            if (vely[i] * vely[i] > 10)
            {
                vely[i] = newvel();
            }
            if (posy[i] < 0 || posy[i] > ROWS)
            {
                posy[i] = newpos();
            }        
        }
        vely[i] += 0.1F;
        posx[i] = posx[i] -  velx[i] + accel[i]; //accel[i] ;//  posx[i] = posx[i] + velx[i] * accel[i];

        posy[i] = posy[i] - vely[i] - accel[i];
        drawPixelXYF(posx[i],posy[i],ColorFromPalette(PartyColors_p,i*80,255,LINEARBLEND));    
    }

    blur2d(leds,ROWS,COLS,15);
    fadeToBlackBy(leds,NUM_LEDS,20);
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

