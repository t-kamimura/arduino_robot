// 20170207 レスキューロボット
// 以下のライブラリをインストールしてください
// 1. DualVNH5019MotorShield
//    「スケッチ」→「ライブラリをインクルード」→「ライブラリの管理」
//     →「DualVNH5019MotorShield」と入力
// 2. ST7032
//    以下のサイトを参考にしてZIPファイルをダウンロード
//    http://blog.icchi.me/arduino-aqm0802a/

#include "DualVNH5019MotorShield.h"
#include <Wire.h>
#include <ST7032.h>

// 各種変数・定数の設定
int ledPin = 3;           // LEDは3番ピン
int inPin = 11;           // デジタルピン11にスイッチ
int analogPin1 = 2;       // 赤外線測距センサー(1)
int analogPin2 = 3;       // 赤外線測距センサー(2)
int val = 0;              // デジタルピン11の状態
int analogVal1 = 0;       // 赤外線測距センサー(1)の値保存用
int analogVal2 = 0;       // 赤外線測距センサー(2)の値保存用
int valError = 0;         // 前後のセンサーの値を比較する用
int threshold = 15;       // しきい値
const int pingPin = 13;   // PINGセンサーを使うとき用

DualVNH5019MotorShield md;
ST7032 lcd;

// モーターエラー関数
void stopIfFault()
{
  if (md.getM1Fault())
  {
    lcd.setCursor(0, 1);
    lcd.print("M1 err");
    while(1);
  }
  if (md.getM2Fault())
  {
    lcd.setCursor(0, 1);
    lcd.print("M2 err");
    while(1);
  }
}

// setupは開始時に一回だけ実行される
void setup() {
  pinMode(ledPin, OUTPUT);    // LED用に出力に設定
  pinMode(inPin, INPUT);      // スイッチ用に入力に設定
  md.init();                  // モーター初期化
  lcd.begin(16, 2);           // LCD初期化
  lcd.setContrast(30);        // LCDコントラスト設定
  lcd.print("DEMO");          // LCD表示
}

// 以下、メイン関数
void loop() {
  lcd.setCursor(0, 1);                      // LCD描画位置設定
  val = digitalRead(inPin);                 // スイッチの状態を読み込む
  if (val==HIGH){
    /* スイッチがHIGHのとき、ロボットは動く */
    digitalWrite(ledPin, HIGH);             // LEDを光らせる
    analogVal1 = analogRead(analogPin1);    // アナログピンを読み取る
    analogVal2 = analogRead(analogPin2);    // アナログピンを読み取る

    /*
    lcd.print(analogVal1/10);
    lcd.print("  ");
    lcd.print(analogVal2/10);
    */

    // 左手法アルゴリズム　壁に沿って動く
    if (analogVal1>analogVal2){
      valError = analogVal1 - analogVal2;   //センサーの値はどれくらい差があるか？
      if (valError<30){
        // センサーの値が近ければ前進
        lcd.print("Fwd");
        md.setSpeeds(400,400);
        stopIfFault();
        delay(100);
      }else{
        // 大きく差があれば回転
        lcd.print("Spin");
        md.setSpeeds(400,0);
        stopIfFault();
        delay(100);
      }
    }else{
      valError = analogVal2 -analogVal1;
      if (valError<30){
        // センサーの値が近ければ前進
        lcd.print("Fwd");
        md.setSpeeds(400,400);
        stopIfFault();
        delay(100);
      }else{
        // 大きく差があれば回転
        lcd.print("Spin");
        md.setSpeeds(0,400);
        stopIfFault();
        delay(100);
      }
    }
    // 左手法アルゴリズム　ここまで

  }else{
    // スイッチがLOWのとき、ロボットは停止
    lcd.print("OFF     ");
    digitalWrite(ledPin, LOW); // LOWなら消える
    md.setM1Brake(400);
    md.setM2Brake(400);
    delay(100);
  }
}
