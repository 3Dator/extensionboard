#include <Wire.h>
#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
  #include <avr/power.h>
#endif

#define PIN 8
#define PIXEL 54

#define FAN_BACK OCR1B
#define FAN_BACK_PIN 10

#define FAN_TOP OCR1A
#define FAN_TOP_PIN 9

uint32_t snake_color;
bool snake_active = 0;

uint8_t r=0;
uint8_t g=0;
uint8_t b=0;

byte command;
byte programm;
uint8_t values[3];
bool new_action = 0;

Adafruit_NeoPixel strip = Adafruit_NeoPixel(PIXEL, PIN, NEO_GRB + NEO_KHZ800);

void setup() {
  pinMode(FAN_BACK_PIN,OUTPUT);
  pinMode(FAN_TOP_PIN,OUTPUT);
  
  TCCR1A |= (1<< WGM10) | (1 << COM1A1) | (1 << COM1B1);
  strip.begin();
  strip.show(); // Initialize all pixels to 'off'
  Wire.begin(4);                // join i2c bus with address #4
  Wire.onReceive(receiveEvent); // register event
}

void loop() {
  if(new_action){
    perform_actions(command, programm, values);
  }
  if(snake_active){
    colorSnake(snake_color,20);
    delay(100);
  }
}

void receiveEvent(int howMany) {
  new_action = 1;
  command = Wire.read();
  programm = Wire.read();
  uint8_t counter = 0;
  while (Wire.available()) { // loop through all
    values[counter] = Wire.read(); // receive byte
    counter++;
  }
}

//command:
// 1 -> LEDs
// 2 -> FANs

//programm LEDs:
// 1 -> fan1
// 2 -> fan2
// 3 -> fade leds
// 4 -> instant leds
// 5 -> move down
// 6 -> move leds
// 7 -> party modus

//programm FANs:
// 1 -> Fan1 normal mode
// 2 -> Fan2 normal mode
// 3 -> Fan1 PWM mode
// 4 -> Fan2 PWM mode

void perform_actions(byte command, byte programm, uint8_t values[3]){
  new_action = 0;
  switch(command){
    //LEDs
    case 1:
      switch(programm){
        case 1: 
          colorFade(values[0], values[1], values[2],0);
          break;
        case 2: 
          colorInstant(strip.Color(values[0], values[1], values[2]));
          break;
        case 3: 
          colorDown(strip.Color(values[0], values[1], values[2]),20);
          break;
        case 4: 
          colorUp(strip.Color(values[0], values[1], values[2]),20);
          break;
        case 5: 
          colorWipe(strip.Color(values[0], values[1], values[2]),20);
          break;
        case 6: 
          if(snake_active){
            snake_active = 0;
          }else{
            snake_color = strip.Color(values[0], values[1], values[2]);
            snake_active = 1;
          }
          break;
        case 7: 
          rainbowCycle(10);
          colorFade(r, g, b,0);
          break;
      }
      if(programm <=5){
        r = values[0];
        g = values[1];
        b = values[2];
      }
      break;
    //FANs
    case 2:
      switch(programm){
        case 1:
          setFan(0,values[0]);
          break;
        case 2:
          setFan(1,values[0]);
          break;
      }
      break;
  }
}

void colorFade(int new_r, int new_g, int new_b, uint8_t wait) {
  uint8_t steps = 100;
  uint32_t c;
  for(uint8_t s = 0;s<=steps;s++){
    c = strip.Color(((new_r-r)/(float)steps)*s+r, ((new_g-g)/(float)steps)*s+g, ((new_b-b)/(float)steps)*s+b);
    for(uint8_t i=0; i<strip.numPixels(); i++) {
      strip.setPixelColor(i, c);
    }
    strip.show();
    delay(wait);
  }
}

void colorWipe(uint32_t c, uint8_t wait) {
  for(uint16_t i=0; i<strip.numPixels(); i++) {
    strip.setPixelColor(i, c);
    strip.show();
    delay(wait);
  }
}

void colorSnake(uint32_t c, uint8_t wait) {
  uint8_t length = 10;
  for(uint16_t i=0; i<strip.numPixels()+length; i++) {
    if(i<strip.numPixels()){
      strip.setPixelColor(i, c);
    }
    if(i>=length){
      strip.setPixelColor(i-length, strip.Color(r,g,b));
    }
    strip.show();
    delay(wait);
  }
}

void colorInstant(uint32_t c) {
  for(uint16_t i=0; i<strip.numPixels(); i++) {
    strip.setPixelColor(i, c);
  }
  strip.show();
}

void colorUp(uint32_t c, uint8_t wait) {
  for(uint16_t i=0; i<=(strip.numPixels()/2); i++) {
    strip.setPixelColor(i, c);
    strip.setPixelColor(PIXEL-i, c);
    strip.show();
    delay(wait);
  }
}

void colorDown(uint32_t c, uint8_t wait) {
  for(uint16_t i=(strip.numPixels()/2); i>0; i--) {
    strip.setPixelColor(i-1, c);
    strip.setPixelColor(PIXEL-i, c);
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
      FAN_BACK = 255;
      delay(1000);
      FAN_BACK = fanspeed;
    }else{
      FAN_BACK = 0;
    }
    break;
  case 1:
    if(fanspeed>0){
      FAN_TOP = 255;
      delay(1000);
      FAN_TOP = fanspeed;
    }else{
      FAN_TOP = 0;
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
