#include <StandardCplusplus.h>
#include <system_configuration.h>
#include <unwind-cxx.h>
#include <utility.h>
#include <vector>
#include <string>
using namespace std;

#include <Adafruit_NeoPixel.h>
#include "graph.h"

typedef uint32_t color_t;
const color_t WHITE  = 0xffffff;
const color_t BLACK  = 0x000000;
const color_t GREEN  = 0xff0000;
const color_t RED    = 0x00ff00;
const color_t BLUE   = 0x0000ff;
const color_t PURPLE = 0x00ffff;
const color_t CYAN   = 0xff00ff;
const color_t YELLOW = 0xffff00;

#define PIN 5
#define NUM_LEDS 140
Adafruit_NeoPixel strip = Adafruit_NeoPixel(NUM_LEDS, PIN, NEO_GRB + NEO_KHZ800);
static uint32_t frameCounter = 0;
static void (*fPtr)(void) = NULL;
static uint32_t wait = 100;

const int pixelsToAge = 20;
static uint16_t pixelAge[pixelsToAge] = {255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255};
void age(uint16_t pixelId) {
  static uint8_t index = 0;
  uint16_t oldestPixel =  pixelAge[index];
  if ( oldestPixel != 255 ) {
    strip.setPixelColor( oldestPixel, BLACK );
  }
  pixelAge[index++] = pixelId;
  if ( index == 10 ) 
    index = 0;
}

void ramp_up_white() {
  uint16_t i;
  static uint16_t j = frameCounter % 256;
  for ( i = 0; i < NUM_LEDS; i++ )
    strip.setPixelColor(i, j,j,j);
}

void set_top( uint32_t color ) {
  uint16_t numPix = 75;
  for( uint16_t i=0; i < numPix ; i++) {
    strip.setPixelColor(i, color);
  }
}

void set_bottom( uint32_t color ) {
  uint16_t numPix = strip.numPixels();
  for( uint16_t i = 75; i < numPix; i++) {
    strip.setPixelColor(i, color);
  }
}

// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
color_t Wheel(byte WheelPos) {
  if(WheelPos < 85) {
   return strip.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
  } else if(WheelPos < 170) {
   WheelPos -= 85;
   return strip.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  } else {
   WheelPos -= 170;
   return strip.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
}

void rainbow() {
  for(uint16_t i=0; i<strip.numPixels(); i++) {
    strip.setPixelColor(i, Wheel(i+frameCounter) );
    //Serial.println( String(frameCounter) + " " + String(Wheel(i+frameCounter)) );
  }
}

void rainbowCrawl() {
    int pixel = frameCounter % NUM_LEDS;
    strip.setPixelColor(pixel, Wheel(frameCounter) );
    //Serial.println( String(pixel) + " " + String(Wheel(frameCounter)) );
}

void set_all( color_t color ) {
  for( uint16_t i=0; i <strip.numPixels(); i++) {
    strip.setPixelColor(i, color);
  }
}

// http://eduardofv.com/read_post/179-Arduino-RGB-LED-HSV-Color-Wheel-
 void HSBToRGB(
    unsigned int inHue, unsigned int inSaturation, unsigned int inBrightness,
    unsigned int *oR, unsigned int *oG, unsigned int *oB )
{
    if (inSaturation == 0) {
        // achromatic (grey)
        *oR = *oG = *oB = inBrightness;
    } else {
        unsigned int scaledHue = (inHue * 6);
        unsigned int sector = scaledHue >> 8; // sector 0 to 5 around the color wheel
        unsigned int offsetInSector = scaledHue - (sector << 8);  // position within the sector         
        unsigned int p = (inBrightness * ( 255 - inSaturation )) >> 8;
        unsigned int q = (inBrightness * ( 255 - ((inSaturation * offsetInSector) >> 8) )) >> 8;
        unsigned int t = (inBrightness * ( 255 - ((inSaturation * ( 255 - offsetInSector )) >> 8) )) >> 8;
 
        switch( sector ) {
        case 0:
            *oR = inBrightness;
            *oG = t;
            *oB = p;
            break;
        case 1:
            *oR = q;
            *oG = inBrightness;
            *oB = p;
            break;
        case 2:
            *oR = p;
            *oG = inBrightness;
            *oB = t;
            break;
        case 3:
            *oR = p;
            *oG = q;
            *oB = inBrightness;
            break;
        case 4:
            *oR = t;
            *oG = p;
            *oB = inBrightness;
            break;
        default:    // case 5:
            *oR = inBrightness;
            *oG = p;
            *oB = q;
            break;
        }
    }
}// END HSBToRGB

void newRainBow()  
{
  unsigned int r, g, b;
  frameCounter++;
  int hue = frameCounter % 255;
  
  for( uint16_t i = 0; i < NUM_LEDS; i++) {
  // red is blue
    HSBToRGB( (hue + i)%255, 255, 128, &r, &g, &b);
    strip.setPixelColor( i, g, b, r );
  }
  strip.show();
  delay(wait);
}

void notSureButCool()  
{
  unsigned int r, g, b;
  frameCounter++;
  int hue = frameCounter % 255;
  
  for( uint16_t i = 0; i < NUM_LEDS; i++) {
  // red is blue
    HSBToRGB(hue + i, 255, 128, &r, &g, &b);
    strip.setPixelColor( i, g, b, r );
  }
  
  Serial.print(" bright=");
  Serial.print(64);
  Serial.print(" hue=");
  Serial.print(hue);
  Serial.print(" red=");
  Serial.print(r);
  Serial.print(" green=");
  Serial.print(g);
  Serial.print(" blue=");
  Serial.print(b);
  Serial.println("");
  strip.show();
  delay(wait);
}

void lines() {
  static bool oddEven = false;
  oddEven = !oddEven;
  set_all( BLACK );
  uint16_t startPoint;
  if ( oddEven )
    startPoint = 140;
  else 
    startPoint = 141;

  path_t n;
  for ( unsigned int index = 0; index < sizeof(path_t); index++ ) {
    n[index] = ball[startPoint][index];
  }

  for (int j = 0; j < 9; j++ ) {
    for (int i = 0; i < 5; ++i) {
      strip.setPixelColor(n[i], Wheel((rand() %180 + 160) & 255 ));
      uint8_t element = 0;
      if ( oddEven )
        element = 3;
      else  
        element = 0;
        
      // get the next point upward or downward
      n[i] = ball[n[i]][element];
    }
//    strip.show();
//    delay(wait);
  }
}

void linesWhite() {
  static bool oddEven = false;
  oddEven = !oddEven;
  set_all( BLACK );
  uint16_t startPoint;
  if ( oddEven )
    startPoint = 140;
  else 
    startPoint = 141;

  path_t n;
  for ( unsigned int index = 0; index < sizeof(path_t); index++ ) {
    n[index] = ball[startPoint][index];
  }

  for (int j = 0; j < 9; j++ ) {
    for (int i = 0; i < 5; ++i) {
      strip.setPixelColor(n[i], WHITE );
      uint8_t element = 0;
      if ( oddEven )
        element = 3;
      else  
        element = 0;
        
      // get the next point upward or downward
      n[i] = ball[n[i]][element];
    }
//    strip.show();
//    delay(wait);
  }
}

void chasing_line() {
  static bool oddEven = false;
  static int lastLine = 1;
  oddEven = !oddEven;
  
  uint16_t startPoint;
  oddEven ? startPoint = 140: startPoint = 141;
  
  path_t n;
  for ( unsigned int index = 0; index < sizeof(path_t); index++ ) {
    n[index] = ball[startPoint][index];
  }


  for (int j = 0; j < 9; j++ ) {
    age( n[lastLine] );
    strip.setPixelColor(n[lastLine], Wheel((rand() %180 + 160) & 255 ));
    uint8_t element = 0;
    oddEven ? element = 3: element = 0;
    n[lastLine] = ball[n[lastLine]][element];
    strip.show();
    delay(wait);
  }

  if ( !oddEven ) {
    lastLine++;
    if ( lastLine > 4 )
      lastLine = 0; 
  } 
}

void chasing() {
  static uint16_t nextPixel = 140;
  path_t n;
  for ( unsigned int index = 0; index < sizeof(path_t); index++ ) {
    n[index] = ball[nextPixel][index];
  }
  age( nextPixel );
  if ( nextPixel != 255 )
    strip.setPixelColor(nextPixel, Wheel((rand() %180 + 160) & 255 ));
  nextPixel = n[rand()%5];
  strip.show();
  delay(wait);
}

String readDataFromSerial1() {
  String message = "";
  while ( 0 < Serial1.available() ) {  
    message += char( Serial1.read() );
  }
  return message;  
}

String readDataFromSerial() {
  String message = "";
  while ( 0 < Serial.available() ) {  
    message += char( Serial.read() );
  }
  return message;  
}

void initializeBlueTooth() {
  const int delayTime = 250;
  Serial1.println("AT");
  delay( delayTime );
  Serial.println(("AT response: " + readDataFromSerial1()).c_str());
  
  Serial1.println("AT+STATE");
  delay( delayTime );
  Serial.println(("AT+STATE response: " + readDataFromSerial1()).c_str());

  Serial1.println("AT+VERSION");
  delay( delayTime );
  Serial.println(("AT+VERSION response: " + readDataFromSerial1()).c_str());
  
  Serial1.println("AT+NAME");
  delay( delayTime );
  Serial.println(("AT+NAME response: " + readDataFromSerial1()).c_str());
  
  Serial1.println("AT+NAME:GeoBall1");
  delay( delayTime );
  Serial.println(("AT+NAME(Set) response: " + readDataFromSerial1()).c_str());
  
  Serial1.println("AT+NAME");
  delay( delayTime );
  Serial.println(("AT+NAME response: " + readDataFromSerial1()).c_str());

//  Serial1.println("AT+PSWD:");
//  delay( delayTime );
//  Serial.println(("AT+PSWD: " + readDataFromSerial1()).c_str());
//  
//  Serial1.println('AT+PSWD:"0000"');
//  delay( delayTime );
//  Serial.println(("AT+PSWD(Set) response: " + readDataFromSerial1()).c_str());
}

vector<string> split(const char *str, char c = ' ') {
    vector<string> result;
    do {
        const char *token = str;
        while(*str != c && *str)
            str++;

        result.push_back(string(token, str));
    } while (0 != *str++);
    return result;
}

void whole_ball_rainbow() {
  set_all(Wheel(frameCounter & 255));
  strip.show();
  delay(wait);
}

void ring() {
  static int count = 0;
  for ( uint8_t i = 0; i < 10; i++ ){
    strip.setPixelColor( ringCenter[i], WHITE );
  }
  strip.setPixelColor( ringCenter[count], BLACK );
  
  for ( uint8_t i = 0; i < 20; i++ ){
    strip.setPixelColor( ringCenterTop[i], WHITE );
  }
  strip.setPixelColor( ringCenterTop[count*2], BLACK );
  strip.setPixelColor( ringCenterTop[count*2+1], BLACK );
  
  for ( uint8_t i = 0; i < 20; i++ ){
    strip.setPixelColor( ringCenterBottom[i], WHITE );
  }
  strip.setPixelColor( ringCenterBottom[count*2], BLACK );
  strip.setPixelColor( ringCenterBottom[count*2+1], BLACK );

  count++;
  if ( count == 10 )
    count = 0;
    
  strip.show();
  delay(wait);
}
 
void ringInverse() {
  uint32_t color = Wheel(frameCounter & 255);

  static int count = 0;
  for ( uint8_t i = 0; i < 10; i++ ){
    strip.setPixelColor( ringCenter[i], BLACK );
  }
  strip.setPixelColor( ringCenter[count], color );
  
  for ( uint8_t i = 0; i < 20; i++ ){
    strip.setPixelColor( ringCenterTop[i], BLACK );
  }
  strip.setPixelColor( ringCenterTop[count*2], color );
  strip.setPixelColor( ringCenterTop[count*2+1], color );
  
  for ( uint8_t i = 0; i < 20; i++ ){
    strip.setPixelColor( ringCenterBottom[i], BLACK );
  }
  strip.setPixelColor( ringCenterBottom[count*2], color );
  strip.setPixelColor( ringCenterBottom[count*2+1], color );

  count++;
  if ( count == 10 )
    count = 0;
    
  strip.show();
  delay(wait);
}

void eyeOfSuron() {

  set_all( BLACK );
  static int count = 9;
  strip.setPixelColor( ringCenter[count], 0x00ffff );
  uint8_t center = ringCenter[count];
  const uint8_t nsize = 18;
  uint8_t neighbors[nsize];
  for ( uint8_t i = 0; i < nsize; i++ ){
    neighbors[i] = 0;
  }
  for ( unsigned int index = 0; index < 6; index++ ) {
    neighbors[index] = ball[center][index];
    if ( 74 < neighbors[index] ) {
      neighbors[index+6] = ball[neighbors[index]][3];
      if ( index == 3 ){
        if ( 0 == count % 2 ){
            neighbors[12] = ball[neighbors[index+6]][2];
            neighbors[13] = ball[neighbors[index+6]][3];
            neighbors[14] = ball[neighbors[13]][3];
        } else {
            neighbors[12] = ball[neighbors[index+6]][2];
            neighbors[13] = ball[neighbors[index+6]][3];
            neighbors[14] = ball[neighbors[index+6]][4];
        }
      }
    }
    else {
      neighbors[index+6] = ball[neighbors[index]][0];
      if ( index == 0 ) {
        if ( 0 == count % 2 ) {
          neighbors[15] = ball[neighbors[index+6]][0];
          neighbors[16] = ball[neighbors[index+6]][1];
          neighbors[17] = ball[neighbors[15]][1];
        } else {
          neighbors[15] = ball[neighbors[index+6]][0];
          neighbors[16] = ball[neighbors[index+6]][1];
          neighbors[17] = ball[neighbors[index+6]][5];
        }
      }
    }
  }
  
  for ( uint8_t i = 0; i < nsize; i++ ){
    strip.setPixelColor( neighbors[i], RED );
  }

  int dir = rand() % 3;
  if ( dir == 0 )
    count = count;
  else if ( dir == 1 )  
    count++;
  else
    count--;
  if ( count == 10 )
    count = 0;
  if ( count < 0 )
    count = 9;

  //Serial.println( count );
  strip.show();
}

void triColor( int range, triangleTB* triangleToUse, color_t color ) {
  for ( uint8_t i = 0; i < range; i++ ){
    path_t n;
    for ( unsigned int index = 0; index < sizeof(path_t); index++ ) {
      n[index] = (*triangleToUse)[i][index];
    }
    
    for ( uint8_t j = 0; j < 6; j++ ){
      if ( n[j] != 0xff )
        strip.setPixelColor( n[j], color );
    }
  }
}
void triangleTry() {
  linesWhite();
  //int range = rand() % 6;
  triColor( rand() % 6, &triangle1, RED );
  triColor( rand() % 6, &triangle2, BLUE );
  triColor( rand() % 6, &triangle3, GREEN );
  triColor( rand() % 6, &triangle4, CYAN );
  triColor( rand() % 6, &triangle5, PURPLE );
}

void lightsOut() {
  set_all( BLACK );
}

void process_serial_data() {
  static String message = "";
  message = readDataFromSerial();
  if ( 0 == message.length() )
     message = readDataFromSerial1();

  if ( 0 < message.length() ) {
    Serial.println("process_serial_data - New data found message: " + message );
    auto results = split( message.c_str() );

    if ( results[0] == "help" ){
      Serial1.println("List of commands" );
      Serial1.println("rain" );
      Serial1.println("iring" );
      Serial1.println("suron" );
      Serial1.println("green" );
      Serial1.println("red" );
      Serial1.println("blue" );
      Serial1.println("lines" );
      Serial1.println("chaseLine" );
      Serial1.println("chasing" );
      Serial1.println("black");
      Serial1.println("triangles");
      Serial1.println("rain speed(number 1 - 5000 )" );
      
      Serial1.println("dim" );
      Serial1.println("bright level(number 0 - 255)" );
      resetFunctionPtrAndWait();
    }
      
    if ( results[0] == "green" ){
      Serial1.println("Setting lights to GREEN" );
      set_all( GREEN );
      resetFunctionPtrAndWait();
    }
    else if ( results[0] == "red" ){
      Serial1.println("Setting lights to RED" );
      set_all( RED );
      resetFunctionPtrAndWait();
    }
    else if ( results[0] == "blue" ){
      Serial1.println("Setting lights to BLUE" );
      set_all( BLUE );
      resetFunctionPtrAndWait();
    }
    else if ( results[0] == "lines" ){
      Serial1.println("Setting lights to Lines" );
      set_all( BLACK );
      resetFunctionPtrAndWait();
      fPtr = &lines;
      wait = 25;
    }
    else if ( results[0] == "chaseLine" ){
      Serial1.println("Setting lights to ChaseLine" );
      set_all( BLACK );
      resetFunctionPtrAndWait();
      fPtr = &chasing_line;
      wait = 25;
    }
    else if ( results[0] == "chasing" ){
      Serial1.println("Setting lights to Chasing" );
      set_all( BLACK );
      resetFunctionPtrAndWait();
      fPtr = &chasing;
      wait = 25;
    }
    else if ( results[0] == "ring" ){
      Serial1.println("Setting lights to Ring" );
      resetFunctionPtrAndWait();
      fPtr = &ring;
      wait = 25;
    }
    else if ( results[0] == "iring" ){
      Serial1.println("Setting lights to InverseRing" );
      resetFunctionPtrAndWait();
      fPtr = &ringInverse;
      wait = 50;
    }
    else if ( results[0] == "black" ){
      Serial1.println("Setting lights to lights out" );
      resetFunctionPtrAndWait();
      fPtr = &lightsOut;
      wait = 5000;
    }
    else if ( results[0] == "triangle" ){
      Serial1.println("Setting lights to triangles" );
      resetFunctionPtrAndWait();
      fPtr = &triangleTry;
      wait = 5000;
    }
    else if ( results[0] == "suron" ){
      Serial1.println("Setting lights to Eye Of Suron" );
      resetFunctionPtrAndWait();
      fPtr = &eyeOfSuron;
      wait = 1000;
    }
    else if ( results[0] == "rain" ){
      if ( 1 < results.size() ) {
        wait = atoi( results[1].c_str());
        Serial1.println( ("Setting lights to Rainbow with delay of " + results[1]).c_str() );
      } else  {
        wait = 100;
        Serial1.println( "Setting lights to Rainbow No speed set, default it slow" );
      }
      fPtr = &rainbow;
    }
    else if ( results[0] == "new" ){
      if ( 1 < results.size() ) {
        wait = atoi( results[1].c_str());
        Serial1.println( ("Setting lights to new Rainbow with delay of " + results[1]).c_str() );
      } else  {
        wait = 1;
        Serial1.println( "Setting lights to new Rainbow No speed set, default it slow" );
      }
      fPtr = &newRainBow;
    }
    else if ( results[0] == "dim" ){
      Serial1.println("Setting lights to dim" );
      strip.setBrightness( 32 );
    }
    else if ( results[0] == "bright" ){
      if ( 1 < results.size() ) {
        strip.setBrightness( atoi( results[1].c_str()) );
        Serial1.println( ("Brightness set to " + results[1]).c_str() );
      } else {
        strip.setBrightness( 192 );
        Serial1.println("Brightness set default: 64" );
      }
    }
  }
}

void resetFunctionPtrAndWait() {
  wait = 1000;
  fPtr = NULL;
}

void loop() {
  process_serial_data();
  
  if ( fPtr != NULL )
    fPtr();
    
  strip.show();
  frameCounter++;
  delay(wait); //delay
}

void setup() {
  // put your setup code here, to run once:
  strip.begin();
  strip.setBrightness( 64 );
  Serial.begin(9600);
  Serial1.begin(9600);
  Serial.println("<Arduino Mega Initilized>");
  frameCounter = 0;
  initializeBlueTooth();
//  rainbow();
//  fPtr = &rainbow;
//  wait = 1;
  triangleTry();
  fPtr = &triangleTry;
  wait = 100;
}


    
