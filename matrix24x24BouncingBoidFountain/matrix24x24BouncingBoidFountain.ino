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

uint8_t noise3d[NUM_LAYERSMAX][WIDTH][HEIGHT];     // двухслойная маска или хранилище свойств в размер всей матрицы
uint8_t line[WIDTH];                               // свойство пикселей в размер строки матрицы
uint8_t shiftHue[HEIGHT];                          // свойство пикселей в размер столбца матрицы
uint8_t shiftValue[HEIGHT];       
bool loadingFlag = true;
uint8_t selectedSettings = 1;
struct ModeType
{
  uint8_t Brightness = 255U; // not used here
  uint8_t Speed = 255;       // odd or even only
  uint8_t Scale = 100;       // 1-100
};
ModeType modes[1];

uint8_t currentMode = 0;
#define EFF_MATRIX 16U                 // свойство пикселей в размер столбца матрицы ещё одно
uint16_t ff_x, ff_y, ff_z;     
float accel;   
#define trackingOBJECT_MAX_COUNT                         (100U)  // максимальное количество отслеживаемых объектов (очень влияет на расход памяти)
float   trackingObjectPosX[trackingOBJECT_MAX_COUNT];
float   trackingObjectPosY[trackingOBJECT_MAX_COUNT];
float   trackingObjectSpeedX[trackingOBJECT_MAX_COUNT];
float   trackingObjectSpeedY[trackingOBJECT_MAX_COUNT];
float   trackingObjectShift[trackingOBJECT_MAX_COUNT];
uint8_t trackingObjectHue[trackingOBJECT_MAX_COUNT];
uint8_t trackingObjectState[trackingOBJECT_MAX_COUNT];
bool    trackingObjectIsShift[trackingOBJECT_MAX_COUNT];
#define enlargedOBJECT_MAX_COUNT                     (WIDTH * 2) // максимальное количество сложных отслеживаемых объектов (меньше, чем trackingOBJECT_MAX_COUNT)
uint8_t enlargedObjectNUM;                                       // используемое в эффекте количество объектов
long    enlargedObjectTime[enlargedOBJECT_MAX_COUNT];
uint8_t hue2[trackingOBJECT_MAX_COUNT];
//константы размера матрицы вычисляется только здесь и не меняется в эффектах
const uint8_t CENTER_X_MINOR =  (WIDTH / 2) -  ((WIDTH - 1) & 0x01); // центр матрицы по ИКСУ, сдвинутый в меньшую сторону, если ширина чётная
const uint8_t CENTER_Y_MINOR = (HEIGHT / 2) - ((HEIGHT - 1) & 0x01); // центр матрицы по ИГРЕКУ, сдвинутый в меньшую сторону, если высота чётная
const uint8_t CENTER_X_MAJOR =   WIDTH / 2  + (WIDTH % 2);           // центр матрицы по ИКСУ, сдвинутый в большую сторону, если ширина чётная
const uint8_t CENTER_Y_MAJOR =  HEIGHT / 2  + (HEIGHT % 2);          // центр матрицы по ИГРЕКУ, сдвинутый в большую сторону, если высота чётная
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
// стандартные функции библиотеки LEDraw от @Palpalych (для адаптаций его эффектов)
void blurScreen(fract8 blur_amount, CRGB *LEDarray = leds)
{
  blur2d(LEDarray, WIDTH, HEIGHT, blur_amount);
}

void dimAll(uint8_t value, CRGB *LEDarray = leds) {
  //for (uint16_t i = 0; i < NUM_LEDS; i++) {
  //  leds[i].nscale8(value); //fadeToBlackBy
  //}
  // теперь короткий вариант
  nscale8(LEDarray, NUM_LEDS, value);
  //fadeToBlackBy(LEDarray, NUM_LEDS, 255U - value); // эквивалент  
}

template <class T>
class vec2 {
public:
	T x, y;
	
	vec2() :x(0), y(0) {}
	vec2(T x, T y) : x(x), y(y) {}
	vec2(const vec2& v) : x(v.x), y(v.y) {}
	
	vec2& operator=(const vec2& v) {
		x = v.x;
		y = v.y;
		return *this;
	}
    bool operator==(vec2& v) {
        return x == v.x && y == v.y;
    }

    bool operator!=(vec2& v) {
        return !(x == y);
    }
	
	vec2 operator+(vec2& v) {
		return vec2(x + v.x, y + v.y);
	}
	vec2 operator-(vec2& v) {
		return vec2(x - v.x, y - v.y);
	}
	
	vec2& operator+=(vec2& v) {
		x += v.x;
		y += v.y;
		return *this;
	}
	vec2& operator-=(vec2& v) {
		x -= v.x;
		y -= v.y;
		return *this;
	}	
	vec2 operator+(double s) {
		return vec2(x + s, y + s);
	}
	vec2 operator-(double s) {
		return vec2(x - s, y - s);
	}
	vec2 operator*(double s) {
		return vec2(x * s, y * s);
	}
	vec2 operator/(double s) {
		return vec2(x / s, y / s);
	}
	
	
	vec2& operator+=(double s) {
		x += s;
		y += s;
		return *this;
	}
	vec2& operator-=(double s) {
		x -= s;
		y -= s;
		return *this;
	}
	vec2& operator*=(double s) {
		x *= s;
		y *= s;
		return *this;
	}
	vec2& operator/=(double s) {
		x /= s;
		y /= s;
		return *this;
	}
	
	void set(T x, T y) {
		this->x = x;
		this->y = y;
	}
	
	void rotate(double deg) {
		double theta = deg / 180.0 * M_PI;
		double c = cos(theta);
		double s = sin(theta);
		double tx = x * c - y * s;
		double ty = x * s + y * c;
		x = tx;
		y = ty;
	}
	
	vec2& normalize() {
		if (length() == 0) return *this;
		*this *= (1.0 / length());
		return *this;
	}
	
	float dist(vec2 v) const {
		vec2 d(v.x - x, v.y - y);
		return d.length();
	}
	float length() const {
		return sqrt(x * x + y * y);
	}
	void truncate(double length) {
		double angle = atan2f(y, x);
		x = length * cos(angle);
		y = length * sin(angle);
	}
	
	vec2 ortho() const {
		return vec2(y, -x);
	}
	
	static float dot(vec2 v1, vec2 v2) {
		return v1.x * v2.x + v1.y * v2.y;
	}
	static float cross(vec2 v1, vec2 v2) {
		return (v1.x * v2.y) - (v1.y * v2.x);
	}

  float mag() const {
      return length();
  }

  float magSq() {
        return (x * x + y * y);
  }

  void limit(float max) {
    if (magSq() > max*max) {
        normalize();
        *this *= max;
    }
  }
	
};

typedef vec2<float> PVector;
typedef vec2<double> vec2d;

uint32_t getPixColor(uint32_t thisSegm)
{
  uint32_t thisPixel = thisSegm;// * SEGMENTS;
  if (thisPixel > NUM_LEDS - 1) return 0;
  return (((uint32_t)leds[thisPixel].r << 16) | ((uint32_t)leds[thisPixel].g << 8 ) | (uint32_t)leds[thisPixel].b); // а почему не просто return (leds[thisPixel])?
}

// функция получения цвета пикселя в матрице по его координатам
uint32_t getPixColorXY(uint8_t x, uint8_t y)
{
  return getPixColor(XY(x, y));
}

// функция отрисовки точки по координатам X Y
void drawPixelXY(int8_t x, int8_t y, CRGB color)
{
  if (x < 0 || x > (WIDTH - 1) || y < 0 || y > (HEIGHT - 1)) return;
  uint32_t thisPixel = XY((uint8_t)x, (uint8_t)y);// * SEGMENTS;
  //for (uint8_t i = 0; i < SEGMENTS; i++)
  //{
    leds[thisPixel] = color;
  //}
}
void drawPixelXYF(float x, float y, CRGB color) //, uint8_t darklevel = 0U)
{
//  if (x<0 || y<0) return; //не похоже, чтобы отрицательные значения хоть как-нибудь учитывались тут // зато с этой строчкой пропадает нижний ряд
  // extract the fractional parts and derive their inverses
  uint8_t xx = (x - (int)x) * 255, yy = (y - (int)y) * 255, ix = 255 - xx, iy = 255 - yy;
  // calculate the intensities for each affected pixel
  #define WU_WEIGHT(a,b) ((uint8_t) (((a)*(b)+(a)+(b))>>8))
  uint8_t wu[4] = {WU_WEIGHT(ix, iy), WU_WEIGHT(xx, iy),
                   WU_WEIGHT(ix, yy), WU_WEIGHT(xx, yy)};
  // multiply the intensities by the colour, and saturating-add them to the pixels
  for (uint8_t i = 0; i < 4; i++) {
    int16_t xn = x + (i & 1), yn = y + ((i >> 1) & 1);
    CRGB clr = getPixColorXY(xn, yn);
    clr.r = qadd8(clr.r, (color.r * wu[i]) >> 8);
    clr.g = qadd8(clr.g, (color.g * wu[i]) >> 8);
    clr.b = qadd8(clr.b, (color.b * wu[i]) >> 8);
//if (darklevel) drawPixelXY(xn, yn, makeDarker(clr, darklevel));
//else
    drawPixelXY(xn, yn, clr);
  }
}



class Boid {
  public:

    PVector location;
    PVector velocity;
    PVector acceleration;
    float maxforce;    // Maximum steering force
    float maxspeed;    // Maximum speed

    float desiredseparation = 15
    
    
    ;
    float neighbordist = 10;
    byte colorIndex = 0;
    float mass;

    boolean enabled = true;

    Boid() {}

    Boid(float x, float y) {
      acceleration = PVector(0, 0);
      velocity = PVector(randomf(), randomf());
      location = PVector(x, y);
      maxspeed = 15.0;
      maxforce = 0.05;
    }

    static float randomf() {
      return mapfloat(random(0, 255), 0, 255, -.5, .5);
    }

    static float mapfloat(float x, float in_min, float in_max, float out_min, float out_max) {
      return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
    }

    void run(Boid boids [], uint8_t boidCount) {
      flock(boids, boidCount);
      update();
      // wrapAroundBorders();
      // render();
    }

    // Method to update location
    void update() {
      // Update velocity
      velocity += acceleration;
      // Limit speed
      velocity.limit(maxspeed);
      location += velocity;
      // Reset acceleration to 0 each cycle
      acceleration *= 0;
    }

    void applyForce(PVector force) {
      // We could add mass here if we want A = F / M
      acceleration += force;
    }

    void repelForce(PVector obstacle, float radius) {
      //Force that drives boid away from obstacle.

      PVector futPos = location + velocity; //Calculate future position for more effective behavior.
      PVector dist = obstacle - futPos;
      float d = dist.mag();

      if (d <= radius) {
        PVector repelVec = location - obstacle;
        repelVec.normalize();
        if (d != 0) { //Don't divide by zero.
          // float scale = 1.0 / d; //The closer to the obstacle, the stronger the force.
          repelVec.normalize();
          repelVec *= (maxforce * 7);
          if (repelVec.mag() < 0) { //Don't let the boids turn around to avoid the obstacle.
            repelVec.y = 0;
          }
        }
        applyForce(repelVec);
      }
    }

    // We accumulate a new acceleration each time based on three rules
    void flock(Boid boids [], uint8_t boidCount) {
      PVector sep = separate(boids, boidCount);   // Separation
      PVector ali = align(boids, boidCount);      // Alignment
      PVector coh = cohesion(boids, boidCount);   // Cohesion
      // Arbitrarily weight these forces
      sep *= 1.5;
      ali *= 1.0;
      coh *= 1.0;
      // Add the force vectors to acceleration
      applyForce(sep);
      applyForce(ali);
      applyForce(coh);
    }

    // Separation
    // Method checks for nearby boids and steers away
    PVector separate(Boid boids [], uint8_t boidCount) {
      PVector steer = PVector(0, 0);
      int count = 0;
      // For every boid in the system, check if it's too close
      for (int i = 0; i < boidCount; i++) {
        Boid other = boids[i];
        if (!other.enabled)
          continue;
        float d = location.dist(other.location);
        // If the distance is greater than 0 and less than an arbitrary amount (0 when you are yourself)
        if ((d > 0) && (d < desiredseparation)) {
          // Calculate vector pointing away from neighbor
          PVector diff = location - other.location;
          diff.normalize();
          diff /= d;        // Weight by distance
          steer += diff;
          count++;            // Keep track of how many
        }
      }
      // Average -- divide by how many
      if (count > 0) {
        steer /= (float) count;
      }

      // As long as the vector is greater than 0
      if (steer.mag() > 0) {
        // Implement Reynolds: Steering = Desired - Velocity
        steer.normalize();
        steer *= maxspeed;
        steer -= velocity;
        steer.limit(maxforce);
      }
      return steer;
    }

    // Alignment
    // For every nearby boid in the system, calculate the average velocity
    PVector align(Boid boids [], uint8_t boidCount) {
      PVector sum = PVector(0, 0);
      int count = 0;
      for (int i = 0; i < boidCount; i++) {
        Boid other = boids[i];
        if (!other.enabled)
          continue;
        float d = location.dist(other.location);
        if ((d > 0) && (d < neighbordist)) {
          sum += other.velocity;
          count++;
        }
      }
      if (count > 0) {
        sum /= (float) count;
        sum.normalize();
        sum *= maxspeed;
        PVector steer = sum - velocity;
        steer.limit(maxforce);
        return steer;
      }
      else {
        return PVector(0, 0);
      }
    }

    // Cohesion
    // For the average location (i.e. center) of all nearby boids, calculate steering vector towards that location
    PVector cohesion(Boid boids [], uint8_t boidCount) {
      PVector sum = PVector(0, 0);   // Start with empty vector to accumulate all locations
      int count = 0;
      for (int i = 0; i < boidCount; i++) {
        Boid other = boids[i];
        if (!other.enabled)
          continue;
        float d = location.dist(other.location);
        if ((d > 0) && (d < neighbordist)) {
          sum += other.location; // Add location
          count++;
        }
      }
      if (count > 0) {
        sum /= count;
        return seek(sum);  // Steer towards the location
      }
      else {
        return PVector(0, 0);
      }
    }

    // A method that calculates and applies a steering force towards a target
    // STEER = DESIRED MINUS VELOCITY
    PVector seek(PVector target) {
      PVector desired = target - location;  // A vector pointing from the location to the target
      // Normalize desired and scale to maximum speed
      desired.normalize();
      desired *= maxspeed;
      // Steering = Desired minus Velocity
      PVector steer = desired - velocity;
      steer.limit(maxforce);  // Limit to maximum steering force
      return steer;
    }

    // A method that calculates a steering force towards a target
    // STEER = DESIRED MINUS VELOCITY
    void arrive(PVector target) {
      PVector desired = target - location;  // A vector pointing from the location to the target
      float d = desired.mag();
      // Normalize desired and scale with arbitrary damping within 100 pixels
      desired.normalize();
      if (d < 4) {
        float m = map(d, 0, 100, 0, maxspeed);
        desired *= m;
      }
      else {
        desired *= maxspeed;
      }

      // Steering = Desired minus Velocity
      PVector steer = desired - velocity;
      steer.limit(maxforce);  // Limit to maximum steering force
      applyForce(steer);
      //Serial.println(d);
    }

    void wrapAroundBorders() {
      if (location.x < 0) location.x = WIDTH - 1;
      if (location.y < 0) location.y = HEIGHT - 1;
      if (location.x >= WIDTH) location.x = 0;
      if (location.y >= HEIGHT) location.y = 0;
    }

    void avoidBorders() {
      PVector desired = velocity;

      if (location.x < 8) desired = PVector(maxspeed, velocity.y);
      if (location.x >= WIDTH - 8) desired = PVector(-maxspeed, velocity.y);
      if (location.y < 8) desired = PVector(velocity.x, maxspeed);
      if (location.y >= HEIGHT - 8) desired = PVector(velocity.x, -maxspeed);

      if (desired != velocity) {
        PVector steer = desired - velocity;
        steer.limit(maxforce);
        applyForce(steer);
      }

      if (location.x < 0) location.x = 0;
      if (location.y < 0) location.y = 0;
      if (location.x >= WIDTH) location.x = WIDTH - 1;
      if (location.y >= HEIGHT) location.y = HEIGHT - 1;
    }

    bool bounceOffBorders(float bounce) {
      bool bounced = false;

      if (location.x >= WIDTH) {
        location.x = WIDTH - 1;
        velocity.x *= -bounce-(random(0.5,3.));
        bounced = true;
      }
      else if (location.x < 0) {
        location.x = 0;
        velocity.x *= -bounce-(random(0.5,3.));
        bounced = true;
      }

      if (location.y >= HEIGHT) {
        location.y = HEIGHT - 1;
        velocity.y *= -bounce-(random(0.5,3.));
        bounced = true;
      }
      else if (location.y < 0) {
        location.y = 0;
        velocity.y *= -bounce-(random(0.5,3.));
        bounced = true;
      }

      return bounced;
    }

    void render() {
      // // Draw a triangle rotated in the direction of velocity
      // float theta = velocity.heading2D() + radians(90);
      // fill(175);
      // stroke(0);
      // pushMatrix();
      // translate(location.x,location.y);
      // rotate(theta);
      // beginShape(TRIANGLES);
      // vertex(0, -r*2);
      // vertex(-r, r*2);
      // vertex(r, r*2);
      // endShape();
      // popMatrix();
      // backgroundLayer.drawPixel(location.x, location.y, CRGB::Blue);
    }
};
static const uint8_t AVAILABLE_BOID_COUNT = 1U;
Boid boids[AVAILABLE_BOID_COUNT];
void particlesUpdate2(uint8_t i){
  trackingObjectState[i]--; 
  trackingObjectPosX[i] += trackingObjectSpeedX[i] + accel;
  trackingObjectPosY[i] += trackingObjectSpeedY[i]+ accel;
  if(trackingObjectState[i] == 0 || trackingObjectPosX[i] <= -1 || trackingObjectPosX[i] >= WIDTH || trackingObjectPosY[i] <= -1 || trackingObjectPosY[i] >= HEIGHT) 
    trackingObjectIsShift[i] = false;
}

// ============= ЭФФЕКТ ИСТОЧНИКИ ===============
// (c) SottNick
// такое себе зрелище
void fountainsDrift(uint8_t j){
  //float shift = random8()
  boids[j].location.x += boids[j].velocity.x + accel;
  boids[j].location.y += boids[j].velocity.y + accel;
  if (boids[j].location.x + boids[j].velocity.x < +3){
    //boids[j].location.x = WIDTH - 1 + boids[j].location.x;
    boids[j].location.x = -boids[j].location.x-(random(0.5,1.5));
    boids[j].velocity.x = -boids[j].velocity.x-(random(0.5,1.5));
  }
  if (boids[j].location.x > WIDTH - 3){    
    boids[j].location.x = WIDTH + WIDTH - 3 - boids[j].location.x-(random(0.5,1.5));
    boids[j].velocity.x = -boids[j].velocity.x-(random(0.5,1.5));
  }
  if (boids[j].location.y < +3){  
    boids[j].location.y = -boids[j].location.y-(random(0.5,1.5));
    boids[j].velocity.y = -boids[j].velocity.y-(random(0.5,1.5));
  }
  if (boids[j].location.y > HEIGHT - 3){    
    boids[j].location.y = HEIGHT + HEIGHT - 2 - boids[j].location.y-(random(0.5,1.8));
    boids[j].velocity.y = -boids[j].velocity.y-(random(0.5,1.8));
  }
}

void fountainsEmit(uint8_t i){
  uint8_t j = random8(enlargedObjectNUM);
  fountainsDrift(j);
  trackingObjectPosX[i] = boids[j].location.x;
  trackingObjectPosY[i] = boids[j].location.y;
  if (hue++ & 0x01)
    hue2[j]+=1;
  trackingObjectSpeedX[i] = ((float)random8()-(float)random(75.,255.))/512.; // random(_hVar)-_constVel; // particle->vx
  trackingObjectSpeedY[i] = ((float)random8()-(float)random(75.,255.))/512.;//sqrt(0.00626-trackingObjectSpeedX[i]*trackingObjectSpeedX[i]); // sqrt(pow(_constVel,2)-pow(trackingObjectSpeedX[i],2)); // particle->vy зависит от particle->vx - не ошибка
  if(random8(2U)) { trackingObjectSpeedY[i]=-trackingObjectSpeedY[i]; }
  trackingObjectState[i] = random8(1, 768); // random8(minLife, maxLife);// particle->ttl
  if (modes[currentMode].Speed & 0x01)
    trackingObjectHue[i] = hue2[j];// (counter/2)%255; // particle->hue
  else
    trackingObjectHue[i] = boids[j].colorIndex;//random8();
  trackingObjectIsShift[i] = true; // particle->isAlive
}
//void fountainsRoutine(){
void draw(){
  if (loadingFlag)
  {
    loadingFlag = false;
    enlargedObjectNUM = (modes[currentMode].Scale + 5U);// / 99.0 * (AVAILABLE_BOID_COUNT ) ;
    if (enlargedObjectNUM > AVAILABLE_BOID_COUNT) enlargedObjectNUM = AVAILABLE_BOID_COUNT;
    deltaValue = trackingOBJECT_MAX_COUNT / (sqrt(CENTER_X_MAJOR*CENTER_X_MAJOR + CENTER_Y_MAJOR*CENTER_Y_MAJOR) * 4U) + 1U; // 4 - это потому что за 1 цикл частица пролетает ровно четверть расстояния между 2мя соседними пикселями
    for(int i = 0; i<trackingOBJECT_MAX_COUNT; i++)
      trackingObjectIsShift[i] = false;
    
    for(int j = 0; j<enlargedObjectNUM; j++){
      boids[j] = Boid(random8(WIDTH), random8(HEIGHT));
      boids[j].velocity.x = random(1.1,1.4);
      boids[j].velocity.y = random(1.1,1.4);
      hue2[j] = random(0,255); 
    }
  }
  step = deltaValue; 
  
  fadeToBlackBy(leds, NUM_LEDS, 8);  
  //go over particles and update matrix cells on the way
  for(int i = 0; i<trackingOBJECT_MAX_COUNT; i++) {
    if (!trackingObjectIsShift[i] && step) {
      //emitter->emit(&particles[i], this->g);
      fountainsEmit(i);
      step--;
    }
    if (trackingObjectIsShift[i]){ // particle->isAlive
      //particles[i].update(this->g);
      particlesUpdate2(i); 
      //generate RGB values for particle
      CRGB baseRGB = CHSV(trackingObjectHue[i], 255,255);      
      baseRGB.nscale8(trackingObjectState[i]);//эквивалент
      drawPixelXYF(trackingObjectPosX[i], trackingObjectPosY[i], baseRGB);
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
}

