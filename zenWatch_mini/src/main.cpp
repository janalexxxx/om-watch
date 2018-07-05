#include <Arduino.h>
#include <Wire.h>
#include "RTClib.h"
#include <Adafruit_NeoPixel.h>
#include <avr/power.h>

#define PIN            8
#define NUMPIXELS      1
#define STRIPPIN       6
#define STRIPSIZE     16

Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel strip  = Adafruit_NeoPixel(STRIPSIZE, STRIPPIN, NEO_GRB + NEO_KHZ800);
int delayval = 500;

#if defined(ARDUINO_ARCH_SAMD)
// for Zero, output on USB Serial console, remove line below if using programming port to program the Zero!
   #define Serial SerialUSB
#endif

RTC_Millis rtc;

const int colors[24][3] = {
  {23,  55,  115},
  {38,  87,  160},
  {70, 148,  242},

  { 67, 146, 240},
  { 79, 156, 212},
  { 93, 167, 179},

  {105, 178, 150},
  {117, 118, 119},
  {133, 196,  90},

  {157, 194,  71},
  {183, 192,  49},
  {208, 191,  31},

  {232, 188,  10},
  {243, 171,   3},
  {240, 144,   8},

  {236, 114,  14},
  {233,  85,  19},
  {225,  84,  42},

  {215, 91,   77},
  {207, 96,  107},
  {182, 93,  123},

  {125, 72,  109},
  {78,  56,   98},
  {20,  33,   82}
};

void setup(){
  // Checks if the correct cpu is responding
  #if defined (__AVR_ATtiny85__)
    if (F_CPU == 16000000) clock_prescale_set(clock_div_1);
  #endif
  Serial.begin(9600);
  pixels.begin();
  strip.begin();

  rtc.begin(DateTime(F(__DATE__), F(__TIME__)));
}

void loop(){
    DateTime now = rtc.now();

    /*Serial.print(now.year(), DEC);
    Serial.print('/');
    Serial.print(now.month(), DEC);
    Serial.print('/');
    Serial.print(now.day(), DEC);
    Serial.print(' ');*/


    //Serial.print(now.hour(), DEC);
    //Serial.print(':');
    //Serial.print(now.minute(), DEC);
    //Serial.print(':');
    //Serial.print(now.second(), DEC);
    //Serial.println();
    /*

    Serial.print(" seconds since 1970: ");
    Serial.println(now.unixtime());*/

    // calculate a date which is 7 days and 30 seconds into the future
    DateTime future (now.unixtime() + 7 * 86400L + 30);

    /*Serial.print(" now + 7d + 30s: ");
    Serial.print(future.year(), DEC);
    Serial.print('/');
    Serial.print(future.month(), DEC);
    Serial.print('/');
    Serial.print(future.day(), DEC);
    Serial.print(' ');
    Serial.print(future.hour(), DEC);
    Serial.print(':');
    Serial.print(future.minute(), DEC);
    Serial.print(':');
    Serial.print(future.second(), DEC);
    Serial.println();

    Serial.println();*/

    //int reference = now.hour();

    int current_time = now.minute();

    int reference = map(current_time, 0, 59, 0, 23);

    int current_r = map(current_time, 0, 59, colors[reference][0], colors[reference + 1][0]);
    int current_g = map(current_time, 0, 59, colors[reference][1], colors[reference + 1][1]);
    int current_b = map(current_time, 0, 59, colors[reference][2], colors[reference + 1][2]);

    pixels.setBrightness(100);
    strip.setBrightness(100);

    pixels.setPixelColor(0, pixels.Color(
      current_r,
      current_g,
      current_b
    ));

    pixels.show();

    for(int i = 0; i < STRIPSIZE; i++){
      Serial.println(i);
      strip.setPixelColor(i, strip.Color(
        current_r,
        current_g,
        current_b
      ));
    }
    strip.show();


     // This sends the updated pixel color to the hardware.
    delay(delayval); // Delay for a period of time (in milliseconds).

    // Fast Switch of colors for scheme debugging
    /*for (int i = 0; i < 23; i++){
      pixels.setPixelColor(0, pixels.Color(
        colors[i][0],
        colors[i][1],
        colors[i][2]
    ));
    pixels.show(); // This sends the updated pixel color to the hardware.
    delay(delayval); // Delay for a period of time (in milliseconds).
    }
    */

}
