#include <Adafruit_NeoMatrix.h>
#include <gamma.h>

#include <gfxfont.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SPITFT.h>
#include <Adafruit_SPITFT_Macros.h>

#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
#include <avr/power.h>
#endif

#define OUTER_PIN 6
#define OUTER_NUM_LEDS 24
#define INNER_PIN 17
#define INNER_NUM_LEDS 12
#define TOTAL_NUM_LEDS 36
#define BRIGHTNESS 50
#define RESOLUTION 50
#define TRANSITION_LENGTH 2000
#define INNER_RADIUS 12

int t = 0;

Adafruit_NeoPixel outerStrip = Adafruit_NeoPixel(OUTER_NUM_LEDS, OUTER_PIN, NEO_RGB + NEO_KHZ800);
Adafruit_NeoPixel innerStrip = Adafruit_NeoPixel(INNER_NUM_LEDS, INNER_PIN, NEO_RGB + NEO_KHZ800);

int positions[TOTAL_NUM_LEDS][2];

int currentLEDColors[TOTAL_NUM_LEDS][3];
int formerLEDColors[TOTAL_NUM_LEDS][3];
int nextLEDColors[TOTAL_NUM_LEDS][3];

int currentStartColor[3];
int currentEndColor[3];
int formerStartColor[3];
int formerEndColor[3];
int nextStartColor[3];
int nextEndColor[3];

int dotPos[2] = { 0, 0 };
boolean showDot = true;
int dotColor[3] = { 0, 0, 255 };



void setup() {
  setEverythingUp();
}


void loop() {
  displayColors(formerStartColor, formerEndColor, nextStartColor, nextEndColor, t);

  if (t < TRANSITION_LENGTH) {
    t++;
  } else {
    t = 0;
    updateColors();
  }
}



////////////////////////////////////////////
// MAIN FUNCTIONS
////////////////////////////////////////////
void displayColors(int formerStartColor[3], int formerEndColor[3], int nextStartColor[3], int nextEndColor[3], int t) {
  // mix in between former and next colors using t to define current colors
  mixColorsAnimated(t, TRANSITION_LENGTH, formerStartColor, nextStartColor, currentStartColor);
  mixColorsAnimated(t, TRANSITION_LENGTH, formerEndColor, nextEndColor, currentEndColor);
  
  // create gradient and update current colors
  createGradient(45, currentStartColor, currentEndColor, positions, nextLEDColors);

  // display current colors
  for (int i = 0; i < TOTAL_NUM_LEDS; i++) {
    if (i<24) {
      outerStrip.setPixelColor(i, nextLEDColors[i][0], nextLEDColors[i][1], nextLEDColors[i][2]);
    } else {
      innerStrip.setPixelColor(i-24, nextLEDColors[i][0], nextLEDColors[i][1], nextLEDColors[i][2]);
    }
  }
  outerStrip.show();
  innerStrip.show();
}


void updateColors() {
  // update the upcoming colors. formerColors = nextColors & nextColors = newColors
}



////////////////////////////////////////////
// SETUP FUNCTION
////////////////////////////////////////////
void setEverythingUp() {
  // calculate position of each Neopixel LED and save in an array
  calcPositions(RESOLUTION, OUTER_NUM_LEDS, INNER_NUM_LEDS, RESOLUTION / 2, INNER_RADIUS);
  
  // turn both strips off to start with
  outerStrip.setBrightness(BRIGHTNESS);
  outerStrip.begin();
  outerStrip.show();
  
  innerStrip.setBrightness(BRIGHTNESS);
  innerStrip.begin();
  innerStrip.show();

  //initial setup of color arrays; all white
  for (int i = 0; i < TOTAL_NUM_LEDS; i++) {
    currentLEDColors[i][0] = 255;
    currentLEDColors[i][1] = 255;
    currentLEDColors[i][2] = 255;
    formerLEDColors[i][0] = 255;
    formerLEDColors[i][1] = 255;
    formerLEDColors[i][2] = 255;
    nextLEDColors[i][0] = 255;
    nextLEDColors[i][1] = 255;
    nextLEDColors[i][2] = 255;
  }
  for (int i=0; i<3; i++) {
    currentStartColor[i] = 255;
    currentEndColor[i] = 255;
    formerStartColor[i] = 255;
    formerEndColor[i] = 255;
  }
}



////////////////////////////////////////////
// CALCULATE VIRTUAL POSITION OF EACH LED
////////////////////////////////////////////
void calcPositions (int resolution, int outerNumLEDs, int innerNumLEDs, float outerRadius, float innerRadius) {
  float originX = resolution / 2;
  float originY = resolution / 2;
  float outerDegree = 360 / outerNumLEDs;
  float outerRadian = (outerDegree * 71) / 4068;
  float innerDegree = 360 / innerNumLEDs * -1;
  float innerRadian = (innerDegree * 71) / 4068;

  for (int i = 0; i < outerNumLEDs; i++) {
    positions[i][0] = int(calcX(originX, outerRadius, outerRadian * i));
    positions[i][1] = int(calcY(originY, outerRadius, outerRadian * i));
  }

  for (int i = 24; i < outerNumLEDs + innerNumLEDs; i++) {
    positions[i][0] = int(calcX(originX, innerRadius, innerRadian * i));
    positions[i][1] = int(calcY(originY, innerRadius, innerRadian * i));
  }
}

float calcX (float originX, float radius, float angle) {
  return originX + radius * cos(angle);
}

float calcY (float originY, float radius, float angle) {
  return originY + radius * sin(angle);
}



///////////////////////////////////////////
// SETUP COLOR GRADIENT
////////////////////////////////////////////
// Setup various color arrays
// Setup a gradient along an axis
void createGradient(float angle, int startColor[3], int endColor[3], int ledPositions[TOTAL_NUM_LEDS][2], int nextLEDColors[TOTAL_NUM_LEDS][3]) {
  // iterate through each LED position and get the color based on that progress and save it in new Grid.
  // The next step then is to provide the LEDs with the right color.
  // Start with a simple 45 degree angle. Make it free to transform later on.
  int a[2] = { RESOLUTION/2, 0 };
  int b[2] = { RESOLUTION/2, RESOLUTION };

  for (int i = 0; i < TOTAL_NUM_LEDS; i++) {
    int currentLEDPos[2] = { ledPositions[i][0], ledPositions[i][1] };
    float progress = getProgressOnGradient(a , b, currentLEDPos);

    int currentLEDColor[3];
    getColorOnGradient(progress, startColor, endColor, currentLEDColor);
 
    nextLEDColors[i][0] = currentLEDColor[0];
    nextLEDColors[i][1] = currentLEDColor[1];
    nextLEDColors[i][2] = currentLEDColor[2];
  }
}

void getColorOnGradient(float progress, int startColor[3], int endColor[3], int currentColor[3]) {
  currentColor[0] = map(progress, 0, 1000, startColor[0], endColor[0]);
  currentColor[1] = map(progress, 0, 1000, startColor[1], endColor[1]);
  currentColor[2] = map(progress, 0, 1000, startColor[2], endColor[2]);
}

// LINEAR GRADIENT
//float getProgressOnGradient(int a[2], int b[2], int p[2]) {
//  int a_to_p[2] = { p[0] - a[0], p[1] - a[1] }; // Storing vector A->P
//  int a_to_b[2] = { b[0] - a[0], b[1] - a[1] }; // Storing vector A->B
//  float atb2 = a_to_b[0] * a_to_b[0] + a_to_b[1] * a_to_b[1]; // Finding the squared magnitude of a_to_b
//
//  float atp_dot_atb = a_to_p[0] * a_to_b[0] + a_to_p[1] * a_to_b[1]; // The dot product of a_to_p and a_to_b
//
//  float t = atp_dot_atb / atb2; // The normalized distance from a to your closest point
//
//  float xPos = a[0] + a_to_b[0] * t;
//  float yPos = a[1] + a_to_b[1] * t;
//
//  // calculate the normalized progress of that point on gradient to then define the color
//  // length of AP divided by lenght of AB
//  return sqrt(xPos * xPos + yPos * yPos) / sqrt(a_to_b[0] * a_to_b[0] + a_to_b[1] * a_to_b[1]);
//}

// CIRCULAR GRADIENT
float getProgressOnGradient(int a[2], int b[2], int p[2]) {
  int a_to_p[2] = { p[0] - a[0], p[1] - a[1] };
  int a_to_b[2] = { b[0] - a[0], b[1] - a[1] };
  float distAB = sqrt(a_to_b[0] * a_to_b[0] + a_to_b[1] * a_to_b[1]);
  float distAP = sqrt(a_to_p[0] * a_to_p[0] + a_to_p[1] * a_to_p[1]);
  float progress = map(distAP*10, 0, distAB*10, 0, 1000);
  
  return progress;
}


////////////////////////////////////////////
// MIXING COLORS ANIMATION
////////////////////////////////////////////
void mixColorsAnimated(int t, int transitionLength, int startColor[3], int endColor[3], int currentColor[3]) {
  currentColor[0] = map(t, 0, transitionLength, startColor[0], endColor[0]);
  currentColor[1] = map(t, 0, transitionLength, startColor[1], endColor[1]);
  currentColor[2] = map(t, 0, transitionLength, startColor[2], endColor[2]);
}



////////////////////////////////////////////
// RAIN ANIMATION
////////////////////////////////////////////
void displayRain() {
  
}


