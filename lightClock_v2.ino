#include <Adafruit_NeoPixel.h>
#include <SevSeg.h>

//#define __SerialPrint__

#define LED_COUNT 60
#define LED_PIN 10
#define HR_INC_PIN 1
#define DEC_INC_PIN 0
#define EN_PIN 11

Adafruit_NeoPixel strip(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);
SevSeg sevseg;
typedef struct {
  byte hour;
  byte dec;
} Time;

Time curr;
byte hrButton, decButton, enButton;
int displayNum;
static uint32_t lastTime, now = 0;
char decPlace;

void setup(){
  #ifdef __SerialPrint__
  Serial.begin(9600);
  #endif
  byte numDigits = 2;
  byte digitPins[] = {12, 13};
  byte segmentPins[] = {9, 2, 3, 5, 6, 8, 7, 4};
  bool resistorsOnSegments = true;
  bool updateWithDelaysIn = true;
  byte hardwareConfig = COMMON_CATHODE;
  sevseg.begin(hardwareConfig, numDigits, digitPins, segmentPins, resistorsOnSegments);
  sevseg.setBrightness(90);
  curr.hour = 0;
  curr.dec = 0;
  pinMode(EN_PIN, INPUT_PULLUP);
  pinMode(HR_INC_PIN, INPUT_PULLUP);
  pinMode(DEC_INC_PIN, INPUT_PULLUP);
  now = millis();

  strip.begin();
  strip.show();
  strip.setBrightness(50);//0 to 255
}

void loop(){
  for( int i=0; i<1000; i++){
    while((now-lastTime)<360){
      now = millis();
    }
    lastTime = now;

    if(!digitalRead(EN_PIN)){
      hrButton = digitalRead(HR_INC_PIN);
      decButton = digitalRead(DEC_INC_PIN);
  
      if(!hrButton) curr.hour++;
      if(!decButton) curr.dec++;
  
      if(curr.dec > 9){
        curr.dec -= 10;
        curr.hour++;
      }
      if(curr.hour > 23){
        curr.hour = 0;
      }
      
      if(curr.hour > 12){
        decPlace = 1;
        displayNum = 16*(curr.hour-12) + curr.dec;
      }else {
        decPlace = -1;
        displayNum = 16*(curr.hour) + curr.dec;
      }
      sevseg.setNumber(displayNum, decPlace, true);
      sevseg.refreshDisplay();
      setLights(curr);
    }else{
      sevseg.blank();
      setLights(curr);
    }
  }
  curr.dec++;
  if(curr.dec > 9){
    curr.dec -= 10;
    curr.hour++;
  }
  if(curr.hour > 23){
    curr.hour = 0;
  }
  setLights(curr);
  #ifdef __SerialPrint__
  Serial.print("hour: ");
  Serial.print(curr.hour);
  Serial.print(" dec: ");
  Serial.print(curr.dec);
  Serial.print(" display: ");
  Serial.println(displayNum, HEX);
  #endif
}

void setLights(Time curr){
  int pixelHue;
  byte value;
  // set pixelhue, 0=red, range 0 to 65536
  if(curr.hour < 6 || curr.hour > 22){
    value = 0;
  }else if(curr.hour == 6){
    value = (255/10)*curr.dec;
  }else if(curr.hour == 22){
    value = 255 - ((255/10)*curr.dec);
  }else{
    value = 255;
  }

  if(curr.hour < 7 || curr.hour > 21){
    pixelHue = 0;
  }else{
    pixelHue = (65535/150)*((curr.hour-7)*10+curr.dec);
  }
  
  for(int i=0; i<strip.numPixels(); i++){
    strip.setPixelColor(i, strip.gamma32(strip.ColorHSV(pixelHue, 255, value)));
  }
  strip.show();

  #ifdef __SerialPrint__
  Serial.print("Hue: ");
  Serial.print(pixelHue);
  Serial.print(" value: ");
  Serial.println(value);
  #endif
}
