#include <Arduino.h>
#include <FastLED.h>
#define LED_PIN 42    //this is for the esp32-s3 
#define BRIGHTNESS 255 //it's blindingly bright at 255 or looks silly if you don't have enough power injected
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
#define NUM_PARTICLES 50  //set this to the number of particles. the varialbe describes what it's supposed to be. it works with 50 but it's a little slow. on an esp32 it looks pretty nice at that number 15 is a safe number
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
//end of pixel placement


//vector library -- \/ \/ \/ \/ \/ -- this is where that 10th grade math is applied.
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
//<-- end of the vector library
//end of vector library ---- /\ /\ /\ /\ /\ 


//this is where the pixles are blended to make them not look like single dots stuttering across the screen. 
uint32_t getPixColor(uint32_t thisSegm)
{
  uint32_t thisPixel = thisSegm;
  if (thisPixel > NUM_LEDS - 1) return 0;
    return (((uint32_t)leds[thisPixel].r << 16) | ((uint32_t)leds[thisPixel].g << 8 ) | (uint32_t)leds[thisPixel].b);
}
uint32_t getPixColorXY(uint8_t x, uint8_t y)
{
  return getPixColor(XY(x, y));
}
void drawPixelXY(int8_t x, int8_t y, CRGB color)
{
  if (x < 0 || x > (WIDTH - 1) || y < 0 || y > (HEIGHT - 1)) return;
  uint32_t thisPixel = XY((uint8_t)x, (uint8_t)y);
    leds[thisPixel] = color;
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
// //-----------------> end of color blending stuff


// //------------boid stuff----------->
class Boid {
  public:

    PVector location;
    PVector velocity;
    PVector acceleration;
    float maxforce;    // Maximum steering force
    float maxspeed;    // Maximum speed
    int hue;
    float desiredseparation = 20;
    float neighbordist = 20;
    byte colorIndex = 0;
    float mass;
    boolean enabled = true;
    Boid() {}
    Boid(float x, float y) {
      acceleration = PVector(0, 0);
      velocity = PVector(randomf(), randomf());
      location = PVector(x, y);
      maxspeed = 1.8;
      maxforce = 1.9;
      mass = random(1.1,1.7);
      hue = random(40,255);
    }
    static float randomf() {
      return mapfloat(random(0, 255), 0, 255, -.5, .5);
    }

    static float mapfloat(float x, float in_min, float in_max, float out_min, float out_max) {
      return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
    }

    void run(Boid boids [], uint8_t boidCount) {
      align(boids, boidCount);
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
      // // uncomment below to make them bounds off the boarders.
      //bounceOffBorders(2);
      // // uncomment below to have them avoid the boards. 
      // avoidBoarders();
      // Arbitrarily weight these forces
      //change these to make the forces more or less forcful, depending what it does. sep will make them seperate more, ali , well you get the point.
      sep *= 3.0;
      ali *= 1.0;
      coh *= 2.0;
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
    }

    void wrapAroundBorders() {
      if (location.x < 0) location.x = ROWS - 1;
      if (location.y < 0) location.y = COLS - 1;
      if (location.x >= ROWS) location.x = 0;
      if (location.y >= COLS) location.y = 0;
    }

    void avoidBorders() {
      PVector desired = velocity;

      if (location.x < 8) desired = PVector(maxspeed, velocity.y);
      if (location.x >= ROWS - 8) desired = PVector(-maxspeed, velocity.y);
      if (location.y < 8) desired = PVector(velocity.x, maxspeed);
      if (location.y >= COLS - 8) desired = PVector(velocity.x, -maxspeed);

      if (desired != velocity) {
        PVector steer = desired - velocity;
        steer.limit(maxforce);
        applyForce(steer);
      }

      if (location.x < 0) location.x = 0;
      if (location.y < 0) location.y = 0;
      if (location.x >= ROWS) location.x = ROWS - 1;
      if (location.y >= COLS) location.y = COLS - 1;
    }

    bool bounceOffBorders(float bounce) {
      bool bounced = false;
      if (location.x >= ROWS) {
        location.x = ROWS - 1;
        velocity.x *= -bounce;
        bounced = true;
      }
      else if (location.x < 0) {
        location.x = 0;
        velocity.x *= -bounce;
        bounced = true;
      }
      if (location.y >= COLS) {
        location.y = COLS - 1;
        velocity.y *= -bounce;
        bounced = true;
      }
      else if (location.y < 0) {
        location.y = 0;
        velocity.y *= -bounce;
        bounced = true;
      }
      return bounced;
    }

};

Boid boids[NUM_PARTICLES];    //this makes the boids, instantiate is what the nerds call it. 
uint16_t x;
uint16_t y;
uint16_t z;

uint16_t speed = 2;
uint16_t scale = 75;
  
//it's pointless to change this sectio nbecause all of the variables get overwritten
void start() {
  int direction = random(0, 2);
  if (direction == 0)
    direction = -1;  
  for (int i = 0; i < NUM_PARTICLES; i++) {
    Boid boid = Boid(random(1,23),random(1,23));
    boid.velocity.x = ((float) random(40, 50)) / 100.0;
    boid.velocity.x *= direction;
    boid.velocity.y = 0;
    boid.colorIndex = i * 32;    
    boids[i] = boid;
    boid.hue = i * 10;  
  }
}
///

//  -- this makes the boids come together all of the parmameters are random that are updated. d can be changed for the contraint to make them move apart or suck closer together.
class Attractor {
  public:
    float mass; // Mass, tied to size
    float G; // Gravitational Constant
    PVector location; // Location
    
  Attractor() {    
    location = PVector(ROWS / (float)random((float)1.1,(float)4.0), COLS / (float)random((float)1.1,(float)4.0));
    mass = random(5.5,8);
    G = (float)random((float)1.5F,(float)2.9F);//random(.5,1.1);
  }
  void UpdateLocation()
  {
    location = PVector(ROWS / (float)random((float)1.1,(float)4.0), COLS / (float)random((float)1.1,(float)4.0));
    mass = random(5.5,10);
    G = (float)random((float)1.5F,(float)2.9F);//random(.5,1.1);
  }
  PVector attract(Boid m) {
    PVector force = location - m.location; // Calculate direction of force
    float d = force.mag(); // Distance between objects
    d = constrain(d, 7.0, 10.0); // Limiting the distance to eliminate "extreme" results for very close or very far objects
    force.normalize(); // Normalize vector (distance doesn't matter here, we just want this vector for direction)        
    float strength = (G * mass * m.mass) / (d * d); // Calculate gravitional force magnitude
    force *= strength; // Get force vector --> magnitude * direction
    return force;
  }
};
// -- end of boids coming together

/// this is that standard palette rotation that's everywhere \/\/
CRGBPalette16 currentPalette;
TBlendType    currentBlending;
extern CRGBPalette16 myRedWhiteBluePalette;
extern const TProgmemPalette16 myRedWhiteBluePalette_p PROGMEM;
void SetupTotallyRandomPalette()
{
    for( int i = 0; i < 16; i++) {
        currentPalette[i] = CHSV( random8(), 255, random8());
    }
}
void SetupBlackAndWhiteStripedPalette()
{
    // 'black out' all 16 palette entries...
    fill_solid( currentPalette, 16, CRGB::Black);
    // and set every fourth one to white.
    currentPalette[0] = CRGB::White;
    currentPalette[4] = CRGB::White;
    currentPalette[8] = CRGB::White;
    currentPalette[12] = CRGB::White;
    
}
void SetupPurpleAndGreenPalette()
{
    CRGB purple = CHSV( HUE_PURPLE, 255, 255);
    CRGB green  = CHSV( HUE_GREEN, 255, 255);
    CRGB black  = CRGB::Black;
    
    currentPalette = CRGBPalette16(
                                   green,  green,  black,  black,
                                   purple, purple, black,  black,
                                   green,  green,  black,  black,
                                   purple, purple, black,  black );
}
const TProgmemPalette16 myRedWhiteBluePalette_p PROGMEM =
{
    CRGB::Red,
    CRGB::Gray, // 'white' is too bright compared to red and blue
    CRGB::Blue,
    CRGB::Black,
    
    CRGB::Red,
    CRGB::Gray,
    CRGB::Blue,
    CRGB::Black,
    
    CRGB::Red,
    CRGB::Red,
    CRGB::Gray,
    CRGB::Gray,
    CRGB::Blue,
    CRGB::Blue,
    CRGB::Black,
    CRGB::Black
};
void ChangePalettePeriodically()
{
    uint8_t secondHand = (millis() / 1000) % 60;
    static uint8_t lastSecond = 99;
    
    if( lastSecond != secondHand) {
        lastSecond = secondHand;
        if( secondHand ==  0)  { currentPalette = RainbowColors_p;         currentBlending = LINEARBLEND; }
        if( secondHand == 10)  { currentPalette = RainbowStripeColors_p;   currentBlending = NOBLEND;  }
        if( secondHand == 15)  { currentPalette = RainbowStripeColors_p;   currentBlending = LINEARBLEND; }
        if( secondHand == 20)  { SetupPurpleAndGreenPalette();             currentBlending = LINEARBLEND; }
        if( secondHand == 25)  { SetupTotallyRandomPalette();              currentBlending = LINEARBLEND; }
        if( secondHand == 30)  { SetupBlackAndWhiteStripedPalette();       currentBlending = NOBLEND; }
        if( secondHand == 35)  { SetupBlackAndWhiteStripedPalette();       currentBlending = LINEARBLEND; }
        if( secondHand == 40)  { currentPalette = CloudColors_p;           currentBlending = LINEARBLEND; }
        if( secondHand == 45)  { currentPalette = PartyColors_p;           currentBlending = LINEARBLEND; }
        if( secondHand == 50)  { currentPalette = myRedWhiteBluePalette_p; currentBlending = NOBLEND;  }
        if( secondHand == 55)  { currentPalette = myRedWhiteBluePalette_p; currentBlending = LINEARBLEND; }
    }
}
///endof palette /\/\/\


Attractor attractor;
Attractor attractor2;

void draw(){
  if (loadingFlag)
  {
    loadingFlag = false;    
    start();     
    attractor.location = PVector(ROWS/4,COLS/1.5);
    attractor2.location = PVector(ROWS/1.5,COLS/4);
  }
  ChangePalettePeriodically(); //changes the palette 
  for (int i = 0; i < NUM_PARTICLES; i++) {
    Boid boid = boids[i];    
    PVector force = attractor.attract(boid);
    PVector force2 = attractor2.attract(boid);
    boid.applyForce(force);
    boid.applyForce(force2);
    
    boid.update();

    EVERY_N_MILLISECONDS(8000) {  //applies new force every 8 seconds
      attractor.UpdateLocation();
      attractor2.UpdateLocation();
    }
    drawPixelXYF(boid.location.x, boid.location.y, ColorFromPalette(currentPalette, boid.hue, 255, currentBlending));
    boids[i] = boid;
    fadeToBlackBy(leds, NUM_LEDS, 1);        

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

