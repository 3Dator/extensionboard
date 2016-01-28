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

//manage pixel
bool pixel_overwrite[PIXEL];
uint32_t pixel_color[PIXEL];

uint32_t snake_color;
bool snake_active = 0;


bool rainbow_active = 0;


uint8_t pulse_color_r;
uint8_t pulse_color_g;
uint8_t pulse_color_b;
bool pulse_active = 0;

//0 means endless and 1 looping stopped all above indicates how often the effect will run
byte snake_cycles = 0;
byte rainbow_cycles = 0;
byte pulse_cycles = 0;

uint8_t r=0;
uint8_t g=0;
uint8_t b=0;

byte command;
byte programm;
uint8_t values[6];
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
  //perform all looping animations. Perform endless if cycles 0 otherwise loop till cycle is 1
  if(snake_active && snake_cycles != 1){
    if(snake_cycles > 1){
      snake_cycles--;
    }
    colorSnake(snake_color,20,10);
    delay(100);
  }
  if(rainbow_active && rainbow_cycles != 1){
    if(rainbow_cycles > 1){
      rainbow_cycles--;
    }
    rainbowCycle(10);
  }
  if(pulse_active && pulse_cycles != 1){
    if(pulse_cycles > 1){
      pulse_cycles--;
    }
    colorPulse(pulse_color_r,pulse_color_g,pulse_color_b,5);
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
// 3 -> overwrite
// 4 -> stuff

//programm LEDs:
// 1 -> fade LEDs
// 2 -> instant leds
// 3 -> move down
// 4 -> move up
// 5 -> move/wipe leds
// 6 -> jitter LEDs
// 7 -> rainbow (looping)
// 8 -> snake (looping)
// 9 -> pulse (looping)
//10 -> turn off all looping

//programm FANs:
// 1 -> Fan1 normal mode
// 2 -> Fan2 normal mode
// 3 -> Fan1 PWM mode
// 4 -> Fan2 PWM mode

//programm overwrite (pixels)
// 1 -> single pixel
// 2 -> single pixel stop overwrite
// 3 -> range
// 4 -> range stop overwrite

//programm stuff
// 1 -> Test mode
// 2 -> demo mode

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
          colorJitter(strip.Color(values[0], values[1], values[2]),20);
          break;
        case 7: 
          rainbow_cycles = values[3];
          if(rainbow_cycles > 0) rainbow_cycles++;
          turn_looping_off();
          rainbow_active = 1;
          break;
        case 8: 
          snake_cycles = values[3];
          if(snake_cycles > 0) snake_cycles++;
          turn_looping_off();
          snake_color = strip.Color(values[0], values[1], values[2]);
          snake_active = 1;
          break;
        case 9:
          pulse_cycles = values[3];
          if(pulse_cycles > 0) pulse_cycles++;
          turn_looping_off();
          pulse_color_r = values[0];
          pulse_color_g = values[1];
          pulse_color_b = values[2];
          pulse_active = 1;
          break;
        case 10:
          turn_looping_off();
          break;
        case 11:
          strip.setBrightness(values[2]);
          strip.show();
          break;
      }
      if(programm <= 6){
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
    //overwrite pixels
    case 3:
      switch(programm){
        case 1:
          if(values[0] <= PIXEL){
            //set pixel to overwrite with which color
            pixel_overwrite[values[0]] = 1;
            pixel_color[values[0]] = strip.Color(values[1], values[2], values[3]);
            set_overwrite_pixel();
          }
          break;
        case 2:
          if(values[0] <= PIXEL){
            //set pixel to overwrite with which color
            pixel_overwrite[values[0]] = 0;
            set_overwrite_pixel();
          }
          break;
        case 3:
          if(values[0] <= PIXEL and values[1] <= PIXEL and values[0] <= values[1]){
            for(uint8_t i = values[0];i<=values[1];i++){
              //set pixel to overwrite with which color
              pixel_overwrite[i] = 1;
              pixel_color[i] = strip.Color(values[2], values[3], values[4]);
            }
            set_overwrite_pixel();
          }
          break;
        case 4:
          if(values[0] <= PIXEL and values[1] <= PIXEL and values[0] <= values[1]){
            for(uint8_t i = values[0];i<=values[1];i++){
              //set pixel to overwrite with which color
              pixel_overwrite[i] = 0;
            }
            set_overwrite_pixel();
          }
          break;
      }
      break;
    //stuff
    case 4:
      switch(programm){
        case 1:
          colorInstant(0);
          r = 0;
          g = 0;
          b = 0;
          setFan(0,255);
          delay(1000);
          setFan(0,0);
          delay(1000);
          setFan(1,255);
          delay(1000);
          setFan(1,0);
          delay(1000);
          colorSnake(strip.Color(255,0,0),50,1);
          colorSnake(strip.Color(0,255,0),50,1);
          colorSnake(strip.Color(0,0,255),50,1);
          colorInstant(strip.Color(255,0,0));
          delay(1000);
          colorInstant(strip.Color(0,255,0));
          delay(1000);
          colorInstant(strip.Color(0,0,255));
          delay(1000);
          break;
        case 2:
          setFan(0,200);
          setFan(1,150);
          colorUp(strip.Color(0,0,255),20);
          colorDown(strip.Color(0,255,0),20);
          colorUp(strip.Color(255,0,0),20);
          colorDown(strip.Color(255,255,0),20);
          colorUp(strip.Color(255,255,255),20);
          colorDown(strip.Color(0,255,255),20);
          colorUp(strip.Color(0,100,255),20);
          colorDown(strip.Color(150,255,0),20);
          colorSnake(strip.Color(255,255,255),30,5);
          delay(1000);
          rainbowCycle(10);
          setFan(0,0);
          setFan(1,0);
          break;
      }
      break;
  }
}

//turn all looping off
void turn_looping_off(){
  pulse_active = 0;
  snake_active = 0;
  rainbow_active = 0;
  colorInstant(strip.Color(r, g, b));
}

//use this function instead of strip.setPixelColor to enable overwritten pixels
void manage_pixel(uint16_t pixel, uint32_t color){
  if(!pixel_overwrite[pixel]){
    strip.setPixelColor(pixel, color);
  }
}

//set all overwritten pixel to their color
void set_overwrite_pixel(){
  for(uint16_t i=0; i<strip.numPixels(); i++) {
    if(pixel_overwrite[i]){
      strip.setPixelColor(i, pixel_color[i]);
    }else{
      strip.setPixelColor(i, strip.Color(r,g,b));
    }
  }
  strip.show();
}

void colorPulse(uint8_t color_r,uint8_t color_g,uint8_t color_b,uint8_t wait){
  uint8_t old_r;
  uint8_t old_g;
  uint8_t old_b;
  colorFade(color_r,color_g,color_b, wait);
  old_r = r;
  old_g = g;
  old_b = b;
  r = color_r;
  g = color_g;
  b = color_b;
  colorFade(old_r,old_g,old_b, wait);
  r = old_r;
  g = old_g;
  b = old_b;
}

void colorFade(uint8_t new_r, uint8_t new_g, uint8_t new_b, uint8_t wait) {
  uint8_t steps = 100;
  uint32_t c;
  for(uint8_t s = 0;s<=steps;s++){
    c = strip.Color(((new_r-r)/(float)steps)*s+r, ((new_g-g)/(float)steps)*s+g, ((new_b-b)/(float)steps)*s+b);
    for(uint8_t i=0; i<strip.numPixels(); i++) {
      manage_pixel(i, c);
    }
    strip.show();
    delay(wait);
  }
}

void colorJitter(uint32_t c, uint8_t wait) {
  colorInstant(c);
  delay(wait/4);
  colorInstant(strip.Color(r,g,b));
  delay(wait*6);
  colorInstant(c);
  delay(wait/2);
  colorInstant(strip.Color(r,g,b));
  delay(wait*2);
  colorInstant(c);
  delay(wait*5);
  colorInstant(strip.Color(r,g,b));
  delay(wait/2);
  colorInstant(c);
  delay(wait*10);
  colorInstant(strip.Color(r,g,b));
  delay(wait/2);
  colorInstant(c);
  delay(wait/2);
  colorInstant(strip.Color(r,g,b));
  delay(wait/4);
  colorInstant(c);
  delay(wait);
}

void colorWipe(uint32_t c, uint8_t wait) {
  for(uint16_t i=0; i<strip.numPixels(); i++) {
    manage_pixel(i, c);
    strip.show();
    delay(wait);
  }
}

void colorSnake(uint32_t c, uint8_t wait, uint8_t length) {
  for(uint16_t i=0; i<strip.numPixels()+length; i++) {
    if(i<strip.numPixels()){
      manage_pixel(i, c);
    }
    if(i>=length){
      manage_pixel(i-length, strip.Color(r,g,b));
    }
    strip.show();
    delay(wait);
  }
}

void colorInstant(uint32_t c) {
  for(uint16_t i=0; i<strip.numPixels(); i++) {
    manage_pixel(i, c);
  }
  strip.show();
}

void colorUp(uint32_t c, uint8_t wait) {
  for(uint16_t i=0; i<=(strip.numPixels()/2); i++) {
    manage_pixel(i, c);
    manage_pixel(PIXEL-i, c);
    strip.show();
    delay(wait);
  }
}

void colorDown(uint32_t c, uint8_t wait) {
  for(uint16_t i=(strip.numPixels()/2); i>0; i--) {
    manage_pixel(i-1, c);
    manage_pixel(PIXEL-i, c);
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
    if(fanspeed>0 && fanspeed < 150 && FAN_BACK == 0){
      FAN_BACK = 255;
      delay(1000);
      FAN_BACK = fanspeed;
    }else{
      FAN_BACK = fanspeed;
    }
    break;
  case 1:
    if(fanspeed>0 && fanspeed < 150 && FAN_TOP == 0){
      FAN_TOP = 255;
      delay(1000);
      FAN_TOP = fanspeed;
    }else{
      FAN_TOP = fanspeed;
    }
    break;
  }
}


// Slightly different, this makes the rainbow equally distributed throughout
void rainbowCycle(uint8_t wait) {
  uint16_t i, j;

  for(j=0; j<256*1; j++) { // 1 cycle of all colors on wheel
    for(i=0; i< strip.numPixels(); i++) {
      manage_pixel(i, Wheel(((i * 256 / strip.numPixels()) + j) & 255));
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
