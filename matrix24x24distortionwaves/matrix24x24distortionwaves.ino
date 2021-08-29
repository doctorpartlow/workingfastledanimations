//based off of https://editor.soulmatelights.com/gallery/1089-distorsion-waves
//https://www.reddit.com/user/ldirko/

#include <FastLED.h>
#define LED_PIN D1    //this is for the esp8266
#define BRIGHTNESS 50 //it's blindingly bright at 255 or looks silly if you don't have enough power injected
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
#define enlargedOBJECT_MAX_COUNT WIDTH * 2
#define SPEED_ADJ (float)NUM_LEDS / 512

//speed control
byte speed = 127; // 1-255

//scale control
byte scale = 150; //1-255

void setup()
{
  delay(3000);
  LEDS.addLeds<LED_TYPE, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS);
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

static double fmap(const double x, const double in_min, const double in_max, const double out_min, const double out_max)
{
  return (out_max - out_min) * (x - in_min) / (in_max - in_min) + out_min;
}

const uint8_t exp_gamma[256] PROGMEM = {
0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   1,   1,   1,
1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,
1,   2,   2,   2,   2,   2,   2,   2,   2,   2,   3,   3,   3,   3,   3,
4,   4,   4,   4,   4,   5,   5,   5,   5,   5,   6,   6,   6,   7,   7,
7,   7,   8,   8,   8,   9,   9,   9,   10,  10,  10,  11,  11,  12,  12,
12,  13,  13,  14,  14,  14,  15,  15,  16,  16,  17,  17,  18,  18,  19,
19,  20,  20,  21,  21,  22,  23,  23,  24,  24,  25,  26,  26,  27,  28,
28,  29,  30,  30,  31,  32,  32,  33,  34,  35,  35,  36,  37,  38,  39,
39,  40,  41,  42,  43,  44,  44,  45,  46,  47,  48,  49,  50,  51,  52,
53,  54,  55,  56,  57,  58,  59,  60,  61,  62,  63,  64,  65,  66,  67,
68,  70,  71,  72,  73,  74,  75,  77,  78,  79,  80,  82,  83,  84,  85,
87,  89,  91,  92,  93,  95,  96,  98,  99,  100, 101, 102, 105, 106, 108,
109, 111, 112, 114, 115, 117, 118, 120, 121, 123, 125, 126, 128, 130, 131,
133, 135, 136, 138, 140, 142, 143, 145, 147, 149, 151, 152, 154, 156, 158,
160, 162, 164, 165, 167, 169, 171, 173, 175, 177, 179, 181, 183, 185, 187,
190, 192, 194, 196, 198, 200, 202, 204, 207, 209, 211, 213, 216, 218, 220,
222, 225, 227, 229, 232, 234, 236, 239, 241, 244, 246, 249, 251, 253, 254, 255};

const uint8_t cos_wave[256] = {
    0,   0,   0,   0,   1,   1,   1,   2,   2,   3,   4,   5,   6,   6,   8,
    9,   10,  11,  12,  14,  15,  17,  18,  20,  22,  23,  25,  27,  29,  31,
    33,  35,  38,  40,  42,  45,  47,  49,  52,  54,  57,  60,  62,  65,  68,
    71,  73,  76,  79,  82,  85,  88,  91,  94,  97,  100, 103, 106, 109, 113,
    116, 119, 122, 125, 128, 131, 135, 138, 141, 144, 147, 150, 153, 156, 159,
    162, 165, 168, 171, 174, 177, 180, 183, 186, 189, 191, 194, 197, 199, 202,
    204, 207, 209, 212, 214, 216, 218, 221, 223, 225, 227, 229, 231, 232, 234,
    236, 238, 239, 241, 242, 243, 245, 246, 247, 248, 249, 250, 251, 252, 252,
    253, 253, 254, 254, 255, 255, 255, 255, 255, 255, 255, 255, 254, 254, 253,
    253, 252, 252, 251, 250, 249, 248, 247, 246, 245, 243, 242, 241, 239, 238,
    236, 234, 232, 231, 229, 227, 225, 223, 221, 218, 216, 214, 212, 209, 207,
    204, 202, 199, 197, 194, 191, 189, 186, 183, 180, 177, 174, 171, 168, 165,
    162, 159, 156, 153, 150, 147, 144, 141, 138, 135, 131, 128, 125, 122, 119,
    116, 113, 109, 106, 103, 100, 97,  94,  91,  88,  85,  82,  79,  76,  73,
    71,  68,  65,  62,  60,  57,  54,  52,  49,  47,  45,  42,  40,  38,  35,
    33,  31,  29,  27,  25,  23,  22,  20,  18,  17,  15,  14,  12,  11,  10,
    9,   8,   6,   6,   5,   4,   3,   2,   2,   1,   1,   1,   0,   0,   0, 0};

void GammaCorrection(){   //gamma correction function 
byte r,g,b;
for (uint16_t i=0; i<NUM_LEDS; i++){
r=leds[i].r;
g=leds[i].g;
b=leds[i].b;
leds[i].r = pgm_read_byte(exp_gamma + r);
leds[i].g = pgm_read_byte(exp_gamma + g);
leds[i].b = pgm_read_byte(exp_gamma + b);
}
}

void draw() {
byte speed = 5;

  uint8_t w = 2;
  uint8_t scale = 4;

  uint16_t a=millis()/32;
  uint16_t a2=a/2;
  uint16_t a3=a/3;

  uint16_t cx =  beatsin8 (10-speed,0,COLS)*scale;
  uint16_t cy =  beatsin8 (12-speed,0,ROWS)*scale;
  uint16_t cx1 = beatsin8 (13-speed,0,COLS)*scale;
  uint16_t cy1 = beatsin8 (15-speed,0,ROWS)*scale;
  uint16_t cx2 = beatsin8 (17-speed,0,COLS)*scale;
  uint16_t cy2 = beatsin8 (14-speed,0,ROWS)*scale;
  
  uint16_t xoffs=0;

  for (int x = 0; x < COLS; x++) {

     xoffs += scale;
     uint16_t yoffs = 0;

    for (int y = 0; y < ROWS; y++) {

       yoffs += scale;

      // byte rdistort = cos_wave [((x+y)*8+a2)&255]>>1; 
      // byte gdistort = cos_wave [((x+y)*8+a3+32)&255]>>1;
      // byte bdistort = cos_wave [((x+y)*8+a+64)&255]>>1;

      byte rdistort = cos_wave [(cos_wave[((x<<3)+a )&255]+cos_wave[((y<<3)-a2)&255]+a3   )&255]>>1; 
      byte gdistort = cos_wave [(cos_wave[((x<<3)-a2)&255]+cos_wave[((y<<3)+a3)&255]+a+32 )&255]>>1; 
      byte bdistort = cos_wave [(cos_wave[((x<<3)+a3)&255]+cos_wave[((y<<3)-a) &255]+a2+64)&255]>>1; 

      byte valueR = rdistort+ w*  (a- ( ((xoffs - cx) *  (xoffs - cx) +  (yoffs - cy) *   (yoffs - cy))>>7  ));
      byte valueG = gdistort+ w*  (a2-( ((xoffs - cx1) * (xoffs - cx1) + (yoffs - cy1) *  (yoffs - cy1))>>7 ));
      byte valueB = bdistort+ w*  (a3-( ((xoffs - cx2) * (xoffs - cx2) + (yoffs - cy2) *  (yoffs - cy2))>>7 ));

      valueR = cos_wave [(valueR)];
      valueG = cos_wave [(valueG)];
      valueB = cos_wave [(valueB)];

      uint16_t index = XY(x, y);
      leds[index].setRGB (valueR,valueG,valueB); 
    }
  }
  
  GammaCorrection();
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
