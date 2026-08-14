#include <Adafruit_NeoPixel.h> //NeoPixel LEDを光らせるためのライブラリ
#define PIN 2 //LEDのDINを接続したArduinoのピン（本に合わせてD2に変更）
//#define PIN 13 //LEDのDINを接続したArduinoのピン
#define NUM_LEDS 14 //LEDの数

//NeoPixelライブラリの初期化
Adafruit_NeoPixel strip = Adafruit_NeoPixel(NUM_LEDS, PIN, NEO_GRB + NEO_KHZ800);
void setup(){  //Arduino初期設定：電源ON時・リセット時に実行される
  strip.begin();
  strip.setBrightness(50); // 電力不足でのフリーズを防ぐため、明るさを少し絞っています（0〜255）
  strip.show();
}

void loop(){ //実行したいプログラムを記述：繰返し実行される
  rainbowUniform(5);  //一斉に色が変わるパターン
  Strobe(0xFF,0xFF,0x00,10, 15, 100); //黄色で光るパターン
  rainbowUniform(5);
  Strobe(0x0B, 0x3B, 0x17, 10, 15, 100);  //蛍光緑で光るパターン
  rainbowUniform(5);
  Strobe(0xDF, 0x3A, 0x01, 10, 15, 100);  //オレンジで光るパターン
 }

//一斉に色が変化する虹色パターン
void rainbowUniform(int SpeedDelay){
    byte *c;
    uint16_t i, j;
    for(j = 0; j < 256 * 5; j++){
        // j（時間経過）のみから色を取得するため、全LEDが同じ色になります
        c = Wheel(j & 255);
        for(i = 0; i < NUM_LEDS; i++){
            setPixel(i, *c, *(c+1), *(c+2));
        }
        strip.show();
        delay(SpeedDelay);
    }
}
byte * Wheel(byte WheelPos){
  static byte c[3];
  if(WheelPos < 85){
   c[0] = WheelPos * 3;
   c[1] = 255 - WheelPos * 3;
   c[2] = 0;
  } else if(WheelPos < 170){
   WheelPos -= 85;
   c[0] = 255 - WheelPos * 3;
   c[1] = 0;
   c[2] = WheelPos * 3;
  } else {
   WheelPos -= 170;
   c[0] = 0;
   c[1] = WheelPos * 3;
   c[2] = 255 - WheelPos * 3;
  }
  return c;
}

//各色を指定して光るパターン
void Strobe(byte red, byte green, byte blue, int StrobeCount, int FlashDelay, int EndPause){
  for(int j = 0; j < StrobeCount; j++){
    setAll(red, green, blue);
    strip.show();
    delay(FlashDelay);
    setAll(0, 0, 0);
    strip.show();
    delay(FlashDelay);
  }
 delay(EndPause);
}

//共通関数
void setPixel(int Pixel, byte red, byte green, byte blue){
  strip.setPixelColor(Pixel, strip.Color(red, green, blue));
}
void setAll(byte red, byte green, byte blue){
  for(int i = 0; i < NUM_LEDS; i++){
    setPixel(i, red, green, blue);
  }
  strip.show();
}
