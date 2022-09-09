///Stepko made this this pattern. 
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@(,@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@,  . .#@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@(   #@@@#  ,@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@, ,*  #@@@&,   .#@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@( ./#*  .#@@@# %@&   ,@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//@@@@@@@@@@@@@@@@@@@@@@@@@@&, %/@@/ .&.#@@@@@@@@@&,  .#@@@@@@@@@@@@@@@@@@@@@@@@@@
//@@@@@@@@@@@@@@@@@@@@@@@@( /*  .    .,.## (@@&.@@(,     ,@@@@@@@@@@@@@@@@@@@@@@@@
//@@@@@@@@@@@@@@@@@@@@@&, ,%  .&/   %@&.%(  /#  *@@@@@@@*  .#@@@@@@@@@@@@@@@@@@@@@
//@@@@@@@@@@@@@@@@@@@(   /*  /@@@,*&@@@%.        ,,  #@@@@    ,@@@@@@@@@@@@@@@@@@@
//@@@@@@@@@@@@@@@@&,       ,,@@@@@@@@@@&,              (@@@@@*  .#@@@@@@@@@@@@@@@@
//@@@@@@@@@@@@@@(  .,  .&,**@@@@@@@@@@@@/              ,,.,&@@/    ,@@@@@@@@@@@@@@
//@@@@@@@@@@@&,   /&,  /#,&@@@@@@@@@@@@@(                 *@@@@@,     #@@@@@@@@@@@
//@@@@@@@@@(    .***   ,##@@@@@@@@@@@@@@#                    .(@%.      ,@@@@@@@@@
//@@@@@@&,           * &@@@@@@@@@@@@@@@@%.              .,,,#@(*.          #@@@@@@
//@@@@(   *@@      ..*@@@@@@@@@@@@@@@@@@@,                 . /@.  %((@#.     ,@@@@
//@&,    .%*      .&/ .@@@@@@@@@@@@@@@@@@*   .*,            #(/@@(              #@
//,      ,.         .* ,@@@@@@@,     /@@@/         .*#.  .. (@@@@@@@@@@@@@@#.    .
//@@%.               ,@@@@@@&         .%@#   /@@@%.  @@@* .@@@@@%.  ## %@*     /@@
//@@@@@,          ,.,&,/@@@@&.        .%@@&.*%@@&,  (@@&. (@@@(   #@&       .@@@@@
//@@@@@@@%.   .#@(,. *@@@@@@%          (@@@&, *%*... .*%@,#@#.  /@@@.     /@@@@@@@
//@@@@@@@@@@,    /     (@@@@@@&.    ,@@@@@@@@@@@@@@@@@@@@.@,  @@*.     .@@@@@@@@@@
//@@@@@@@@@@@@%.         .&@@@@@@@@@@@@@@@@@@@@@@@@@@@@@(,**%@@.     /@@@@@@@@@@@@
//@@@@@@@@@@@@@@@, ,&@/   %%.,%@@@@@@@@@@@@@@@@@@@@@@,./* //(     .@@@@@@@@@@@@@@@
//@@@@@@@@@@@@@@@@@%. @.((.*  (@@* .%@@@@@@@@@@* ,%/  /&/**&,   /@@@@@@@@@@@@@@@@@
//@@@@@@@@@@@@@@@@@@@@, .%@@&,(@/ *@@@&, .(.(@(   */ .*%@@@/ .@@@@@@@@@@@@@@@@@@@@
//@@@@@@@@@@@@@@@@@@@@@@%/@@@@@@#*&@@(/&@@(*  ..  .*@@@@@@./@@@@@@@@@@@@@@@@@@@@@@
//@@@@@@@@@@@@@@@@@@@@@@@@@,*@@@@@@@/%@@@@@,   .#@@@@@@*.@@@@@@@@@@@@@@@@@@@@@@@@@
//@@@@@@@@@@@@@@@@@@@@@@@@@@@%.*@@@@@@@@&@@,*@@@@@@@/ /@@@@@@@@@@@@@@@@@@@@@@@@@@@
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@, /@@@@@@@@@@@@@@% .@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@%. *@@@@@@@@%  /@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@,  /@@&, .@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@%.   /@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//https://editor.soulmatelights.com/gallery/user/193-stepko

#include <arduino.h>
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

#define M_HEIGHT LED_ROWS + !(LED_ROWS % 2)
#define M_WIDTH LED_COLS + !(LED_COLS % 2)
#define M_SHIFT_X !(LED_COLS % 2)
#define M_SHIFT_Y !(LED_ROWS % 2)
bool maze[M_WIDTH][M_HEIGHT];
bool start = true;
byte posX, posY;
byte color;
byte Lookdir;
bool checkFlag;
bool tale = 0;
int SubPos;

CRGB colorsmear(const CRGB & col1,
  const CRGB & col2, byte l) {
  CRGB temp = col1;
  nblend(temp, col2, l);
  return temp;
}

void drawPixelXYF(float x, float y, CRGB col) {
  byte ax = byte(x);
  byte xsh = (x - ax) * 255;
  byte ay = byte(y);
  byte ysh = (y - ay) * 255;
  CRGB colP1 = colorsmear(col, CRGB(0, 0, 0), xsh);
  CRGB colP2 = colorsmear(CRGB(0, 0, 0), col, xsh);
  CRGB col1 = colorsmear(colP1, CRGB(0, 0, 0), ysh);
  CRGB col2 = colorsmear(colP2, CRGB(0, 0, 0), ysh);
  CRGB col3 = colorsmear(CRGB(0, 0, 0), colP1, ysh);
  CRGB col4 = colorsmear(CRGB(0, 0, 0), colP2, ysh);
  
  leds[XY(ax, ay)] += col1;
  leds[XY(ax + 1, ay)] += col2;
  leds[XY(ax, ay + 1)] += col3;
  leds[XY(ax + 1, ay + 1)] += col4;
}

void digMaze(int x, int y) {
  int x1, y1;
  int x2, y2;
  int dx, dy;
  int dir, count;
  
  dir = random(10) % 4;
  count = 0;
  while (count < 4) {
    dx = 0;
    dy = 0;
    switch (dir) {
      case 0:
        dx = 1;
        break;
      case 1:
        dy = 1;
        break;
      case 2:
        dx = -1;
        break;
      default:
        dy = -1;
        break;
    }
    x1 = x + dx;
    y1 = y + dy;
    x2 = x1 + dx;
    y2 = y1 + dy;
    if (x2 > 0 && x2 < M_WIDTH && y2 > 0 && y2 < M_HEIGHT &&
      maze[x1][y1] && maze[x2][y2]) {
      maze[x1][y1] = 0;
      maze[x2][y2] = 0;
      x = x2;
      y = y2;
      dir = random(10) % 4;
      count = 0;
    } else {
      dir = (dir + 1) % 4;
      count += 1;
    }
  }
}

void generateMaze() {
  for (byte y = 0; y < M_HEIGHT; y++) {
    for (byte x = 0; x < M_WIDTH; x++) {
      maze[x][y] = 1;
    }
  }
  maze[1][1] = 0;
  for (byte y = 1; y < M_HEIGHT; y += 2) {
    for (byte x = 1; x < M_WIDTH; x += 2) {
      digMaze(x, y);
    }
  }
  maze[0][1] = 0;
  maze[M_WIDTH - 2][M_HEIGHT - 1] = 0;
}

void draw() {
  if (start) {
    start = 0;
    color = random();
    generateMaze();
    posX = 0, posY = 1;
    checkFlag = 1;
    tale = random() % 2;
    for (byte x = 0; x < LED_COLS; x++) {
      for (byte y = 0; y < LED_ROWS; y++) {
        leds[XY(x, y)] = (maze[x + M_SHIFT_X][y + M_SHIFT_Y]) ? CHSV(color, 200, 255) : CHSV(0, 0, 0);
      }
    }
  }
  if (!tale) {
    for (byte x = 0; x < LED_COLS; x++) {
      for (byte y = 0; y < LED_ROWS; y++) {
        leds[XY(x, y)] = (maze[x + M_SHIFT_X][y + M_SHIFT_Y]) ? CHSV(color, 200, 255) : CHSV(0, 0, 0);
      }
    }
  }
  if (checkFlag) {
    switch (Lookdir) {
      case 0:
        if (!maze[posX][posY - 1]) {
          Lookdir = 1;
        }
        break;
      case 1:
        if (!maze[posX - 1][posY]) {
          Lookdir = 2;
        }
        break;
      case 2:
        if (!maze[posX][posY + 1]) {
          Lookdir = 3;
        }
        break;
      case 3:
        if (!maze[posX + 1][posY]) {
          Lookdir = 0;
        }
        break;
    }
    while (true) {
      bool et1 = 0;
      switch (Lookdir) {
        case 0:
          if (maze[posX + 1][posY]) {
            Lookdir = 3;
            et1 = 1;
          }
          break;
        case 1:
          if (maze[posX][posY - 1]) {
            Lookdir = 0;
            et1 = 1;
          }
          break;
        case 2:
          if (maze[posX - 1][posY]) {
            Lookdir = 1;
            et1 = 1;
          }
          break;
        case 3:
          if (maze[posX][posY + 1]) {
            Lookdir = 2;
            et1 = 1;
          }
          break;
      }
      if (!et1) break;
    }
    checkFlag = 0;
  }
  SubPos += 64;
  if (SubPos >= 255) {
    SubPos = 0;
    checkFlag = 1;
    switch (Lookdir) {
      case 0:
        posX += 1;
        break;
      case 1:
        posY -= 1;
        break;
      case 2:
        posX -= 1;
        break;
      case 3:
        posY += 1;
        break;
    }
  }
  switch (Lookdir) {
    case 0:
      drawPixelXYF(float(posX - M_SHIFT_X) + (float(SubPos) / 255.), (posY - M_SHIFT_Y), CHSV(0, 0, 255));
      break;
    case 1:
      drawPixelXYF((posX - M_SHIFT_X), float(posY - M_SHIFT_Y) - (float(SubPos) / 255.), CHSV(0, 0, 255));
      break;
    case 2:
      drawPixelXYF(float(posX - M_SHIFT_X) - (float(SubPos) / 255.), (posY - M_SHIFT_Y), CHSV(0, 0, 255));
      break;
    case 3:
      drawPixelXYF((posX - M_SHIFT_X), float(posY - M_SHIFT_Y) + (float(SubPos) / 255.), CHSV(0, 0, 255));
      break;
  }
  if ((posX == M_WIDTH - 2) & (posY == M_HEIGHT - 1))
    start = 1;
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

