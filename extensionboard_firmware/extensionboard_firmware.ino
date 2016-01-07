#include <Wire.h>
#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
  #include <avr/power.h>
#endif

#define PIN 8
#define PIXEL 54

int oldColor;
int newColor;
int fanTop=10;
int fanBack=9;
int fanRPM=2;
int fanspeed;

int r=0;
int g=0;
int b=0;

Adafruit_NeoPixel strip = Adafruit_NeoPixel(PIXEL, PIN, NEO_GRB + NEO_KHZ800);

void setup() {
  pinMode(fanBack,OUTPUT);
  pinMode(fanTop,OUTPUT);
  TCCR1A |= (1<< WGM10) | (1 << COM1A1) | (1 << COM1B1) | (1 << COM1A0) | (1 << COM1B0);
  TCCR1B |= (1<< CS10);
  strip.begin();
  strip.show(); // Initialize all pixels to 'off'
  Wire.begin(4);                // join i2c bus with address #4
  Wire.onReceive(receiveEvent); // register event
}

void loop() {
  delay(100);
}

// 1 -> fan1
// 2 -> fan2
// 3 -> fade leds
// 4 -> instant leds
// 5 -> move down
// 6 -> move leds
// 7 -> party modus

void receiveEvent(int howMany) {
  byte command = Wire.read();
  int values[3];
  int counter = 0;
  while (Wire.available()) { // loop through all
    values[counter] = Wire.read(); // receive byte
    counter++;
  }
  
  switch(command){
    case 1: {
      setFan(0,values[0]);
      break;
    }
    case 2: {
      setFan(1,values[0]);
      break;
    }
    case 3: {
      colorFade(values[0], values[1], values[2],0);
      r = values[0];
      g = values[1];
      b = values[2];
      break;
    }
    case 4: {
      colorWipe(strip.Color(values[0], values[1], values[2]),0);
      r = values[0];
      g = values[1];
      b = values[2];
      break;
    }
    case 5: {
      colorDown(strip.Color(values[0], values[1], values[2]),30);
      r = values[0];
      g = values[1];
      b = values[2];
      break;
    }
    case 6: {
      colorWipe(strip.Color(values[0], values[1], values[2]),30);
      r = values[0];
      g = values[1];
      b = values[2];
      break;
    }
    case 7: {
      rainbowCycle(10);
      colorFade(r, g, b,0);
      break;
    }
  }
}

void colorFade(int new_r, int new_g, int new_b, uint8_t wait) {
  int steps = 10;
  uint32_t c;
  for(int s = 0;s<=steps;s++){
    c = strip.Color(((new_r-r)/steps)*s+r, ((new_g-g)/steps)*s+g, ((new_b-b)/steps)*s+b);
    for(uint16_t i=0; i<strip.numPixels(); i++) {
      strip.setPixelColor(i, c);
      strip.show();
    }
    delay(wait);
  }
  colorWipe(strip.Color(new_r, new_g, new_b),0);
}

void colorWipe(uint32_t c, uint8_t wait) {
  
  for(uint16_t i=0; i<strip.numPixels(); i++) {
    strip.setPixelColor(i, c);
    strip.show();
    delay(wait);
  }
}

void colorUp(uint32_t c, uint8_t wait) {
  
  for(uint16_t i=0; i<(strip.numPixels()/2); i++) {
    strip.setPixelColor(i, c);
    strip.setPixelColor(PIXEL-i, c);
    strip.show();
    delay(wait);
  }
}

void colorDown(uint32_t c, uint8_t wait) {
  for(uint16_t i=(strip.numPixels()/2); i>=0; i--) {
    strip.setPixelColor(i-1, c);
    strip.setPixelColor(PIXEL-i-1, c);
    strip.show();
    delay(wait);
  }
  
}

void colorPulse(uint8_t wait) {
    
    for(int i=0;i<255;i++){
    strip.setBrightness(i);
    strip.show();
    delay(wait/1000);
    }
    delay(wait);
    
    strip.setBrightness(255);
    strip.show();
    delay(wait);
  
}

void setFan(uint8_t fan,uint8_t fanspeed) {
switch (fan){ 
  case 0:
    if(fanspeed>0){
      OCR1A = 255;
      delay(1000);
      OCR1A = fanspeed;
    }else{
      OCR1B = 0;
    }
    break;
  case 1:
    if(fanspeed>0){
      OCR1B = 255;
      delay(1000);
      OCR1B = fanspeed;
    }else{
      OCR1B = 0;
    }
    break;
  }
}

// Slightly different, this makes the rainbow equally distributed throughout
void rainbowCycle(uint8_t wait) {
  uint16_t i, j;

  for(j=0; j<256*5; j++) { // 5 cycles of all colors on wheel
    for(i=0; i< strip.numPixels(); i++) {
      strip.setPixelColor(i, Wheel(((i * 256 / strip.numPixels()) + j) & 255));
    }
    strip.show();
    delay(wait);
  }
}

// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
uint32_t Wheel(byte WheelPos) {
  WheelPos = 255 - WheelPos;
  if(WheelPos < 85) {
    return strip.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  }
  if(WheelPos < 170) {
    WheelPos -= 85;
    return strip.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
  WheelPos -= 170;
  return strip.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
}
