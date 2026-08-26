#include <Adafruit_NeoPixel.h> //NeoPixel LEDを光らせるためのライブラリ
#define PIN 2 //LEDのDINを接続したArduinoのピン（本に合わせてD2に変更）
//#define PIN 13 //LEDのDINを接続したArduinoのピン
#define NUM_LEDS 20 //LEDの数
#define SENSOR_PIN A0 //音センサーを接続したピン

int hitCount = 0; // 音に反応した回数
int currentBrightness = 255; // 光の強さ（最初は最大）
float ambientNoise = 0.0; // 【追加】環境音（ノイズ）の基準レベル
int consecutiveVoiceCount = 0; // 【長さフィルター用】連続で音を検知した回数

//NeoPixelライブラリの初期化
Adafruit_NeoPixel strip = Adafruit_NeoPixel(NUM_LEDS, PIN, NEO_GRB + NEO_KHZ800);
void setup(){  //Arduino初期設定：電源ON時・リセット時に実行される
  strip.begin();
  strip.setBrightness(50); // 電力不足でのフリーズを防ぐため、明るさを少し絞っています（0〜255）
  strip.show();
  Serial.begin(9600); // 音の大きさをパソコンで確認できるようにする
}

void loop(){ //実行したいプログラムを記述：繰返し実行される
  int soundLevel = getSoundLevel(); // 音の大きさを測定（1回の測定に約30ミリ秒かかります）

  // 【ノイズキャンセリング処理の強化】
  // 大きな声が出ている最中に、環境音の基準が跳ね上がってしまうのを防ぐため、
  // 「普段の環境音に近い時」だけ学習を更新するようにします。
  if (soundLevel - (int)ambientNoise < 20) {
    ambientNoise = (ambientNoise * 9.0 + soundLevel) / 10.0;
  }

  // 現在の音量から、学習したノイズ基準を引き算し、突出した音（ユーザーの声）を抽出
  int voiceLevel = soundLevel - (int)ambientNoise;

  // しきい値を超えているかチェック（マスク内の小さな声に反応できるよう12に設定）
  if (voiceLevel > 22) { 
//  if (voiceLevel > 12) { 
    consecutiveVoiceCount++; // 連続カウントを増やす
  } else {
    consecutiveVoiceCount = 0; // 音が途切れたらカウントをゼロに戻す
  }

  // 【音の長さフィルター（疑似AI的なアプローチ）】
  // 3回連続（約0.1秒間）音が続いた時だけ「人間の声」とみなす
  // マスクにカチッと当たったような一瞬の物音（1回）は無視される
  if (consecutiveVoiceCount >= 3) { 
    hitCount++; // 反応した回数を1増やす

    // 5回反応するごとに色を「濃く（暗く）」していく
    if (hitCount % 5 == 0) {
      currentBrightness = currentBrightness - 40; 
      if (currentBrightness < 50) {
        currentBrightness = 255; 
      }
    }

    // HSVを使って鮮やかな色を作る
    long randomHue = random(0, 65536); 
    uint32_t color = strip.ColorHSV(randomHue, 255, currentBrightness); 
    
    for(int i = 0; i < NUM_LEDS; i++){
      strip.setPixelColor(i, color);
    }
    strip.show();

    delay(100); // 0.1秒間光らせたままにする
    
    // 一度光ったら、連続カウントをリセットして次の声を待つ
    consecutiveVoiceCount = 0;
  } else {
    // 連続条件を満たしていない時（または音が無い時）は消灯する
    setAll(0, 0, 0);
  }
}

// 音の波の振れ幅を計算する関数（どんな環境でも安定して音を拾うため）
int getSoundLevel() {
  int minVal = 1023;
  int maxVal = 0;
  for (int i = 0; i < 30; i++) {
    int val = analogRead(SENSOR_PIN);
    if (val < minVal) minVal = val;
    if (val > maxVal) maxVal = val;
    delay(1);
  }
  return maxVal - minVal; 
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
