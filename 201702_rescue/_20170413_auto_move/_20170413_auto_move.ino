// 20170413 超音波センサー
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
int threshold_ping = 30;       // PINGしきい値
int threshold_analog = 250;    // 赤外線測距しきい値
const int pingPin = 13;   // PINGセンサーを使うとき用
long duration, cm;        // PINGで距離を測るとき変数類
int motorPwr = 400;       // モーターパワー(0~400で設定)

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

// 距離を計算する関数
long microsecondsToCentimeters(long microseconds) {
  // 音速は340 m/sで，すなわち １センチ進むのに29マイクロ秒かかる．
  // 行き帰りで２倍の時間がかかるので，あとで２で割る．
  return microseconds / 29 / 2;
}

// setupは開始時に一回だけ実行される
void setup() {
  pinMode(ledPin, OUTPUT);    // LED用に出力に設定
  //pinMode(pin, mode);
  pinMode(inPin, INPUT);      // スイッチ用に入力に設定
  md.init();                  // モーター初期化
  lcd.begin(16, 2);           // LCD初期化
  lcd.setContrast(30);        // LCDコントラスト設定
  lcd.print("DEMO");          // LCD表示
  Serial.begin(9600);         // シリアル通信の初期化
}

// 以下、メイン関数
void loop() {
  lcd.setCursor(0, 1);                      // LCD描画位置設定
  val = HIGH;//digitalRead(inPin);                 // スイッチの状態を読み込む
  // 赤外線測距のキョリを取る
  analogVal1 = analogRead(analogPin1);    // アナログピンを読み取る
  analogVal2 = analogRead(analogPin2);    // アナログピンを読み取る
  Serial.print(analogVal1);              // センサー情報を送信
  Serial.print(" ");
  Serial.println(analogVal2);              // センサー情報を送信
  if (val==HIGH){
    /* スイッチがHIGHのとき、ロボットは動く */
    digitalWrite(ledPin, HIGH);             // LEDを光らせる

        // PINGセンサーで距離を取る
    pinMode(pingPin, OUTPUT);
    digitalWrite(pingPin, LOW);
    delayMicroseconds(2);
    digitalWrite(pingPin, HIGH);
    delayMicroseconds(5);
    digitalWrite(pingPin, LOW);
  
    //パルスを送ったのと同じピンで受け取る．
    /* HOW TO USE "pulseIn"
    ピンに入力されるパルスを検出します.
    (value)をHIGHに指定した場合、
    pulseIn関数は入力がHIGHに変わると同時に時間の計測を始め、
    またLOWに戻ったら、そこまでの時間(つまりパルスの長さ)をマイクロ秒単位で返します。
    タイムアウトを指定した場合は、その時間を超えた時点で0を返します。
    */
    pinMode(pingPin, INPUT);
    duration = pulseIn(pingPin, HIGH);
  
    // 時間を距離に変換．
    cm = microsecondsToCentimeters(duration);
    if(analogVal1 > threshold_analog){
      lcd.print("Stp     ");
      md.setM1Brake(400);
      md.setM2Brake(400);
      stopIfFault();
      delay(50);
      /*
      lcd.print("Bwd     ");
      md.setSpeeds(-motorPwr,-motorPwr);
      stopIfFault();
      delay(500);
      */
      lcd.print("Spn     ");
      md.setSpeeds(-motorPwr,motorPwr);
      stopIfFault();
      delay(50);
    }
    else if(analogVal2 > threshold_analog){
      lcd.print("Stp     ");
      md.setM1Brake(400);
      md.setM2Brake(400);
      stopIfFault();
      delay(50);
      /*
      lcd.print("Bwd     ");
      md.setSpeeds(-motorPwr,-motorPwr);
      stopIfFault();
      delay(200);
      */
      lcd.print("Spn     ");
      md.setSpeeds(motorPwr,-motorPwr);
      stopIfFault();
      delay(50);
    }
    else if(cm < threshold_ping){
      lcd.print("Stp     ");
      md.setM1Brake(400);
      md.setM2Brake(400);
      stopIfFault();
      delay(100);
      lcd.print("Bwd     ");
      md.setSpeeds(-motorPwr,-motorPwr);
      stopIfFault();
      delay(500);
      lcd.print("Spn     ");
      md.setSpeeds(-motorPwr,motorPwr);
      stopIfFault();
      delay(200);
    }else{
      lcd.print("Fwd     ");
      md.setSpeeds(motorPwr,motorPwr);
      stopIfFault();
      delay(10);
    }

  }else{
    // スイッチがLOWのとき、ロボットは停止
    lcd.print("OFF     ");
    digitalWrite(ledPin, LOW); // LOWなら消える
    md.setM1Brake(400);
    md.setM2Brake(400);
    delay(100);
  }
}
