#include "DualVNH5019MotorShield.h"
#include <Wire.h>
#include <ST7032.h>

int ledPin = 3;   // LEDを3番ピンに
int inPin = 11;   // デジタルピン11にプッシュボタン
int analogPin1 = 2;        // ここにセンサーを接続
int analogPin2 = 3;        // ここにセンサーを接続
int val = 0;
int analogVal1 = 0;
int analogVal2 = 0;
int valError = 0;
int threshold = 15;      // しきい値
const int pingPin = 13;

DualVNH5019MotorShield md;
ST7032 lcd;

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

void setup() {
  pinMode(ledPin, OUTPUT);   // LED用に出力に設定
  pinMode(inPin, INPUT);     // スイッチ用に入力に設定
  md.init();
  lcd.begin(16, 2);
  lcd.setContrast(30);
  lcd.print("DEMO");
}

void loop() {
  lcd.setCursor(0, 1);
  val = digitalRead(inPin);  // 入力ピンを読む
  if (val==HIGH){    
    digitalWrite(ledPin, HIGH);
    analogVal1 = analogRead(analogPin1);    // アナログピンを読み取る
    analogVal2 = analogRead(analogPin2);    // アナログピンを読み取る

    /*
    lcd.print(analogVal1/10);
    lcd.print("  ");
    lcd.print(analogVal2/10);
    */
    if (analogVal1>analogVal2){
      valError = analogVal1 - analogVal2;
      if (valError<30){
        lcd.print("Fwd");
        md.setSpeeds(400,400);
        stopIfFault();
        delay(100);
      }else{ 
        lcd.print("Spin");
        md.setSpeeds(400,0);
        stopIfFault();
        delay(100);
      }
    }else{
      valError = analogVal2 -analogVal1;
      if (valError<30){
        lcd.print("Fwd");
        md.setSpeeds(400,400);
        stopIfFault();
        delay(100);
      }else{
        lcd.print("Spin");
        md.setSpeeds(0,400);
        stopIfFault();
        delay(100);
      }
    }
    
  }else{
    lcd.print("OFF     ");
    digitalWrite(ledPin, LOW); // LOWなら消える
    md.setM1Brake(400);
    md.setM2Brake(400);
    delay(100);
  }
}

