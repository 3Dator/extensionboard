#include <Wire.h>
#ifdef __AVR__
  #include <avr/power.h>
#endif

#define PIN_R 11
#define PIN_G 9
#define PIN_B 10

#define FAN_BACK OCR1B
#define FAN_BACK_PIN 10

#define FAN_TOP OCR1A
#define FAN_TOP_PIN 9

bool rainbow_active = 0;

uint8_t pulse_color_r;
uint8_t pulse_color_g;
uint8_t pulse_color_b;
bool pulse_active = 0;

//0 means endless and 1 looping stopped all above indicates how often the effect will run
byte rainbow_cycles = 0;
byte pulse_cycles = 0;

uint8_t r=0;
uint8_t g=0;
uint8_t b=0;

byte command;
byte programm;
uint8_t values[6];
bool new_action = 0;

void setup() {
  pinMode(FAN_BACK_PIN,OUTPUT);
  pinMode(FAN_TOP_PIN,OUTPUT);

  pinMode(PIN_R,OUTPUT);
  pinMode(PIN_G,OUTPUT);
  pinMode(PIN_B,OUTPUT);

  Wire.begin(4);                // join i2c bus with address #4
  Wire.onReceive(receiveEvent); // register event
}

void loop() {
  if(new_action){
    perform_actions(command, programm, values);
  }
  //perform all looping animations. Perform endless if cycles 0 otherwise loop till cycle is 1
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
          colorInstant(values[0], values[1], values[2]);
          break;
        case 3:
          colorInstant(values[0], values[1], values[2]);
          break;
        case 4:
          colorInstant(values[0], values[1], values[2]);
          break;
        case 5:
          colorInstant(values[0], values[1], values[2]);
          break;
        case 6:
          colorInstant(values[0], values[1], values[2]);
          break;
        case 7:
          rainbow_cycles = values[3];
          if(rainbow_cycles > 0) rainbow_cycles++;
          turn_looping_off();
          rainbow_active = 1;
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
          //todo implement brightness
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
    //test programms
    case 4:
      switch(programm){
        case 1:
          colorInstant(0,0,0);
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
          colorInstant(255,0,0);
          delay(1000);
          colorInstant(0,255,0);
          delay(1000);
          colorInstant(0,0,255);
          delay(1000);
          break;
        case 2:
          setFan(0,200);
          setFan(1,150);
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
  rainbow_active = 0;
  colorInstant(r, g, b);
}

//use this function instead of strip.setPixelColor to enable overwritten pixels
void set_color(uint8_t r,uint8_t g,uint8_t b){
  analogWrite(PIN_R,r);
  analogWrite(PIN_G,g);
  analogWrite(PIN_B,b);
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
  for(uint8_t s = 0;s<=steps;s++){
    set_color(((new_r-r)/(float)steps)*s+r, ((new_g-g)/(float)steps)*s+g, ((new_b-b)/(float)steps)*s+b);
    delay(wait);
  }
}

void colorInstant(uint8_t new_r, uint8_t new_g, uint8_t new_b) {
  set_color(new_r ,new_g ,new_b);
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
    Wheel(1);
    delay(wait);
  }
}

// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
void Wheel(byte WheelPos) {
  WheelPos = 255 - WheelPos;
  if(WheelPos < 85) {
    set_color(255 - WheelPos * 3, 0, WheelPos * 3);
  }
  if(WheelPos < 170) {
    WheelPos -= 85;
    set_color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
  WheelPos -= 170;
  set_color(WheelPos * 3, 255 - WheelPos * 3, 0);
}
