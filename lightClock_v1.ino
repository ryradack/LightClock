#include <Adafruit_NeoPixel.h>
#include <SevSeg.h>

#define LED_PIN 10
#define LED_COUNT 60

#define HR_INC_PIN 1
#define DEC_INC_PIN 0

SevSeg sevseg;
byte hour, dec;
byte hrButton, decButton;
int displayNum;
static uint32_t lastTime, now = 0;
char decPlace;

Adafruit_NeoPixel strip(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);

void setup(){
  Serial.begin(9600);
  byte numDigits = 2;
  byte digitPins[] = {12, 13};
  byte segmentPins[] = {9, 2, 3, 5, 6, 8, 7, 4};
  bool resistorsOnSegments = true;
  bool updateWithDelaysIn = true;
  byte hardwareConfig = COMMON_CATHODE;
  sevseg.begin(hardwareConfig, numDigits, digitPins, segmentPins, resistorsOnSegments);
  sevseg.setBrightness(90);
  hour = 0;
  dec = 0;

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

    hrButton = digitalRead(HR_INC_PIN);
    decButton = digitalRead(DEC_INC_PIN);

    if(!hrButton) hour++;
    if(!decButton) dec++;

    if(dec > 9){
      dec -= 10;
      hour++;
    }
    if(hour > 23){
      hour = 0;
    }
    
    if(hour > 12){
      decPlace = 1;
      displayNum = 16*(hour-12) + dec;
    }else {
      decPlace = -1;
      displayNum = 16*(hour) + dec;
    }
    Serial.print("hour: ");
    Serial.print(hour);
    Serial.print(" dec: ");
    Serial.print(dec);
    Serial.print(" display: ");
    Serial.print(displayNum, HEX);
    Serial.print(" i ");
    Serial.println(i);
    sevseg.setNumber(displayNum, decPlace, true);
    sevseg.refreshDisplay();
    setLights(hour, dec);
  }
  dec++;
  setLights(hour, dec);
}

void setLights(byte hour, byte decimal){
  int pixelHue;
  byte value;
  // set pixelhue, 0=red, range 0 to 65536
  if(hour < 6 || hour > 22){
    value = 0;
  }else if(hour == 6){
    value = (255/10)*decimal;
  }else if(hour == 22){
    value = 255 - ((255/10)*decimal);
  }else{
    value = 255;
  }

  if(hour < 7 || hour > 21){
    pixelHue = 0;
  }else{
    pixelHue = (65535/150)*((hour-7)*10+decimal);
  }
  
  for(int i=0; i<strip.numPixels(); i++){
    strip.setPixelColor(i, strip.gamma32(strip.ColorHSV(pixelHue, 255, value)));
  }
  strip.show();

  Serial.print("Hue: ");
  Serial.print(pixelHue);
  Serial.print(" value: ");
  Serial.println(value);
}
