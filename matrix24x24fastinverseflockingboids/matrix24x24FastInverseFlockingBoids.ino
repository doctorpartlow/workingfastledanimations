#include <Arduino.h>
#include <FastLED.h>
#include <Math.h>
#include <vector>


#define LED_PIN 42    //this is for the esp32-s3 
#define BRIGHTNESS  255//it's blindingly bright at 255 or looks silly if you don't have enough power injected
#define LED_TYPE WS2812B
#define COLOR_ORDER GRB
const uint8_t ROWS = 24;
const uint8_t COLS = 24;

CRGB leds[ROWS * COLS];
const bool kMatrixSerpentineLayout = true;
#define NUM_LEDS (ROWS * COLS)
bool firstPass = true;
float separa = 2.0;
float alignm = 1.0;
float cohesi = 1.0;

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
//this is the fast inverse square root. 
  float Q_rsqrt( float number )
  {
        long i;
        float x2, y;
        const float threehalfs = 1.5F;
 
        x2 = number * 0.5F;
        y  = number;
        i  = * ( long * ) &y;                       // evil floating point bit level hacking
        i  = 0x5f3759df - ( i >> 1 );               // what the fuck?
        y  = * ( float * ) &i;
        y  = y * ( threehalfs - ( x2 * y * y ) );   // 1st iteration
 
        return y;
}
	float length() const {
		return sqrt(x * x + y * y);
	}
	//your processor likes this way of normalizing a lot more than the other way
  //the fast inverse square root approximates the result using far less taxing calculations
  //increasing over all perofrmance significantly 
  //^^^ it's the function above this one. 

  //verdict is the normal 1/sqrt shows no noticable slow down and it loioks better. so oh well. 
	vec2& normalize() {
		if (length() == 0) return *this;
		*this *= (1.0 / length());// Q_rsqrt(x * x + y * y); // --> this is the slow way to normalize  (1.0 / length());
		return *this;
	}
	
	float dist(vec2 v) const {
		vec2 d(v.x - x, v.y - y);
		return d.length();
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

uint16_t XY( uint8_t x, uint8_t y)
{
  uint16_t i;
  if( kMatrixSerpentineLayout == false) {
    i = (y * ROWS) + x;
  }
  if( kMatrixSerpentineLayout == true) {
    if( y & 0x01) {
      // Odd rows run backwards
      uint8_t reverseX = (ROWS - 1) - x;
      i = (y * ROWS) + reverseX;
    } else {
      // Even rows run forwards
      i = (y * ROWS) + x;
    }
  }
  return i;
}

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
  if (x < 0 || x > (ROWS - 1) || y < 0 || y > (COLS - 1)) return;
  uint32_t thisPixel = XY((uint8_t)x, (uint8_t)y);
    leds[thisPixel] = color;
  
}
void drawPixelXYF(float x, float y, CRGB color) 
{
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
    drawPixelXY(xn, yn, clr);
  }
}

//this makes the boids.
class Boid {
  public:

    PVector location;
    PVector velocity;
    PVector acceleration;
    float maxforce;    // Maximum steering force
    float maxspeed;    // Maximum speed
    int hue;
    float desiredseparation = 3; //this this is the variable that keeps the boids at a certain distance from eachother. the higher the number the more space between. keep this lower than the neighbor distance otherwise the boids will probably not move.
    float neighbordist = 8; //this will determine the length that a boid will interact with another boid. the lower the number the more isolated the boid will be. the higher the number the boid will interact with another boid at a further distance.
    byte colorIndex = 0;
    float scale;    
    float mass;
    float bounce = 1;    
    boolean enabled = true;

    Boid() {}

    Boid(float x, float y) {
      acceleration = PVector(0, 0);
      velocity = PVector(randomf(), randomf());
      location = PVector(x, y);
      maxspeed = 0.6;
      maxforce = 0.08;
      scale = random(0.5,68.5);     
      bounce = random(2.1,2.5);     
      mass = random(0.8,1.3);
      hue = random(40,255);
    }

    static float randomf() {
      return mapfloat(random(0, 255), 0, 255, -.5, .5);
    }

    static float mapfloat(float x, float in_min, float in_max, float out_min, float out_max) {
      return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
    }

    void run(Boid boids [], uint8_t boidCount) {
      flock(boids, boidCount);
    }
    // Method to update location
    void update(Boid boids [], uint8_t boidCount) {
      flock(boids, boidCount);
      velocity += acceleration;
      velocity.limit(maxspeed);
      location += velocity;      
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
        repelVec.normalize(); //no more slow 1/sqrt(float)  to normalize
        if (d != 0) { //Don't divide by zero.
          // float boid->boid.scale = 1.0 / d; //The closer to the obstacle, the stronger the force.
          repelVec.normalize(); //no more slow 1/sqrt(float)  to normalize
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
      sep *= separa; //2.8; //change these to change the behavior of the flock 
      ali *= alignm; //1.2; // 
      coh *= cohesi;//1.2; //
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
        steer.normalize(); //no more slow 1/sqrt(float)  to normalize
        steer *= maxspeed;
        steer -= velocity;
        steer.limit(maxforce);
      }
      return steer;
    }

    // Alignment    
    PVector align(Boid boids [], uint8_t boidCount) {// For every nearby boid in the system, calculate the average velocity
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
        sum.normalize(); //no more slow 1/sqrt(float)  to normalize 
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
    PVector cohesion(Boid boids [], uint8_t boidCount) { // For the average location (i.e. center) of all nearby boids, calculate steering vector towards that location
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
      // Normalize desired and boid->boid.scale to maximum speed
      desired.normalize(); //no more slow 1/sqrt(float)  to normalize
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
      // Normalize desired and boid->boid.scale with arbitrary damping within 100 pixels
      desired.normalize(); //no more slow 1/sqrt(float)  to normalize
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
    bool bounceOffBorders() {
      bool bounced = false;

      if (location.x >= COLS) {
        location.x = COLS - 1;
        velocity.x *= -bounce;
        bounced = true;
      }
      else if (location.x < 0) {
        location.x = 0;
        velocity.x *= -bounce;
        bounced = true;
      }

      if (location.y >= ROWS) {
        location.y = ROWS - 1;
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
    void render() {

    }
};
int fadebyvalue = 60;
#define NUM_PARTICLES 30 
Boid boids[NUM_PARTICLES];       
int speed = 1; 
int count = 29;
int boidseperation = 3;
int neighbordistance = 6;
void start() {
  for (int i = 0; i < count; i++) {
    boids[i] = Boid(random(COLS), 0);
  }
}
int countdir = 1;
void draw(){
  if (firstPass)
  {
    firstPass = false;
    start();     
   }
  // this section is where the boids rotate around between whatever the random number's value tells it to do.  the random numbers can be changed to whatever you want.  
  //the numbers are in the order of separation, alignment, cohesion, boidseperation, neighbordistance, fadebyvalue
  //count  - this is the number of boids on the display at the same time it's currently set to a max of 29. it will probably be fine with over 100 on a disoplay but on a 24 x 23 it just looks like a blob of color so 29 is the sweet spot I think
  //separa - separation will seperate the boids.
  //alignm - this will align the boids more evenly
  //cohesi - this will make the boids more cohesive
  //boidseperation - this will make the boids more seperated
  //neighbordistance - this will make the boids interact with the boids at a further distance
  //fadebyvalue - this will make the boids fade out faster or slower the higher the number the faster it will fade out but the dimmer the boid will be initially.
  EVERY_N_MILLISECONDS(8000) {  //every 8 seconds it will change the variables
   if (count <= 3 || count >= 29) countdir = -countdir;
   count += countdir;
   separa = random(1.5, 3.5);
   alignm = random(0.9,3.5);
   cohesi = random(0.9,3.5);
   boidseperation = random(3,8);
   neighbordistance = random(5,10);
   fadebyvalue = random(1,8)*20;
  }
  for (int i = 0; i < count; i++) {
    Boid * boid = &boids[i];
    boid->update(boids, count);
    boid->wrapAroundBorders();      //you can use the bounce or avoid borders functions instead of this one. this one is better with more than 15 boids on a small matrix
    drawPixelXYF(boid->location.x, boid->location.y, ColorFromPalette(ForestColors_p,boid->hue*15,255,NOBLEND));            
    boid->desiredseparation = boidseperation;
    boid->neighbordist = neighbordistance;
  }
  fadeToBlackBy(leds, NUM_LEDS,fadebyvalue);
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