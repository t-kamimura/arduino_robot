// 20170706 GPSによる誘導
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
#include <SPI.h>
#include <SD.h>
#include <TinyGPS++.h>
#include <math.h>

// 各種変数・定数の設定
int ledPin = 3;           // LEDは3番ピン
//int inPin = 11;
int motorPwr = 400;       // モーターパワー(0~400で設定)
double latitude = 0;
double longitude = 0;
double x = 0;
double y = 0;
double x_GOAL = 0;
double y_GOAL = 0;
double r = 0;
double theta = 0;
int val=0;
#define LNG_GOAL 135.68
#define LAT_GOAL 34.98

//GPS関連の設定
TinyGPSPlus tinyGPS; // tinyGPSPlus object to be used throughout
#define GPS_BAUD 9600 // GPS module's default baud rate
#include <SoftwareSerial.h>
#define ARDUINO_GPS_RX 11 // GPS TX, Arduino RX pin
#define ARDUINO_GPS_TX 13 // GPS RX, Arduino TX pin
SoftwareSerial ssGPS(ARDUINO_GPS_TX, ARDUINO_GPS_RX); // Create a SoftwareSerial
#define gpsPort ssGPS  // Alternatively, use Serial1 on the Leonardo
#define SerialMonitor Serial

//モーター
DualVNH5019MotorShield md;

//LCD
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
  //pinMode(pin, mode);
  pinMode(inPin, INPUT);      // スイッチ用に入力に設定
  md.init();                  // モーター初期化
  lcd.begin(16, 2);           // LCD初期化
  lcd.setContrast(30);        // LCDコントラスト設定
  SerialMonitor.begin(9600);
  lcd.begin(16, 2);           // LCD初期化
  lcd.setContrast(30);        // LCDコントラスト設定
  lcd.setCursor(0,0);
  lcd.print("GPS demo");
  lcd.setCursor(0,1);
  lcd.print("Starting");
  pinMode(ledPin, OUTPUT); //ledと名付けたピンを，出力に設定
  gpsPort.begin(GPS_BAUD);
  while(!tinyGPS.location.isUpdated())
  {
    digitalWrite(ledPin, LOW);
    SerialMonitor.println("Searching...");
    SerialMonitor.print("satellites: ");
    SerialMonitor.println(tinyGPS.satellites.value());

    lcd.setCursor(0,0);
    lcd.print("No data ");
    lcd.setCursor(1,0);
    lcd.print("Waiting ");
    while (gpsPort.available())
      tinyGPS.encode(gpsPort.read());
  }
  x_GOAL = (tinyGPS.location.lng() - LNG_GOAL)*10000;
  y_GOAL = (tinyGPS.location.lat()- LAT_GOAL)*10000;
  SerialMonitor.println("Goal position has been saved");
  SerialMonitor.print("x_GOAL: ");
  SerialMonitor.println(x_GOAL);
  SerialMonitor.print("y_GOAL: ");
  SerialMonitor.println(y_GOAL);
  lcd.setCursor(0,0);
  lcd.print("Start   ");
  lcd.setCursor(1,0);
  lcd.print("        ");
  delay(1000);
}

// 以下、メイン関数
void loop() {
  //val = digitalRead(inPin);                 // スイッチの状態を読み込む
  while (gpsPort.available())
    tinyGPS.encode(gpsPort.read());
  if (tinyGPS.location.isUpdated()) // If the GPS data is vaild
  {
    digitalWrite(ledPin, HIGH);
    SerialMonitor.println("GPS updated.");
    /*
    SerialMonitor.print("Lng: ");
    SerialMonitor.println(tinyGPS.location.lng());
    SerialMonitor.print("Lat: ");
    SerialMonitor.println(tinyGPS.location.lat());
    */
    /*
    lcd.setCursor(0,0);
    lcd.print("Ln:");
    lcd.setCursor(3,0);
    lcd.print(tinyGPS.location.lng());
    lcd.setCursor(0,1);
    lcd.print("Lt:");
    lcd.setCursor(3,1);
    lcd.print(tinyGPS.location.lat());
    */

    longitude = (tinyGPS.location.lng() - LNG_GOAL)*10000;
    latitude = (tinyGPS.location.lat()- LAT_GOAL)*10000;

    SerialMonitor.print("x_GOAL: ");
    SerialMonitor.println(x_GOAL);
    SerialMonitor.print("y_GOAL: ");
    SerialMonitor.println(y_GOAL);

    SerialMonitor.print("Lng: ");
    SerialMonitor.println(longitude);
    //SerialMonitor.println(tinyGPS.location.lng());
    SerialMonitor.print("Lat: ");
    SerialMonitor.println(latitude);
    //SerialMonitor.println(tinyGPS.location.lat());

    x = x_GOAL - longitude;
    y = y_GOAL - latitude;
    theta = atan2(y,x);
    r = x*x + y*y;
    SerialMonitor.print("x: ");
    SerialMonitor.println(x);
    SerialMonitor.print("y: ");
    SerialMonitor.println(y);
    SerialMonitor.print("theta: ");
    SerialMonitor.println(theta);
    SerialMonitor.print("r: ");
    SerialMonitor.println(r);

    /*

    while(abs(theta)<0.1){
      lcd.print("Spin    ");
      md.setSpeeds(-motorPwr,motorPwr);
      stopIfFault();
      delay(200);
    }*/
    if(r>0.1){
      lcd.print("Fwd     ");
      md.setSpeeds(motorPwr,motorPwr);
      stopIfFault();
      delay(10);
    }else{
      lcd.print("Stop    ");
      md.setM1Brake(400);
      md.setM2Brake(400);
      stopIfFault();
      delay(50);
    }

  }else{
    digitalWrite(ledPin, LOW);
    SerialMonitor.println("GPS failed.");
    lcd.setCursor(0,0);
    lcd.print("No data ");
    lcd.setCursor(0,1);
    lcd.print("Stop    ");
    md.setM1Brake(400);
    md.setM2Brake(400);
    stopIfFault();
    delay(50);
  }
}
