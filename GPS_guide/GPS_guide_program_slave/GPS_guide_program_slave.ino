////////////////////////////////////////////////////////////////////////////////
// GPSによる誘導(テンプレート)
// 以下のライブラリをインストールしてください
// 1. TinyGPS++
//    以下のサイトからZIPファイルをダウンロード
//    http://arduiniana.org/libraries/tinygpsplus/
//    GPSロガーシールドの詳しい使い方は
//    https://learn.sparkfun.com/tutorials/gps-logger-shield-hookup-guide?_ga=1.96889503.1408024151.1440392747
//
//    Template by T. Kamimura, GitHub: https://github.com/t-kamimura
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// 設定とかは以下に書き込んで下さい
////////////////////////////////////////////////////////////////////////////////

/* includeファイルはここに追加してください*/
#include <Wire.h>
#include <TinyGPS++.h>
#include <math.h>
#include <SoftwareSerial.h>

/* 各種変数・定数の設定はここに書いてください */
// I/Oピンの設定
int ledPin = 3;           // I/OシールドのLEDは3番ピン

// GPS関連の定数
#define LNG_GOAL 135.68     // ゴールのおおまかな経度（計測して変更してください）
#define LAT_GOAL 34.98      // ゴールのおおまかな緯度（計測して変更してください）
#define SCALE 100000        // 座標がちょっとしか変化しないので，スケールしてやる

// GPS関連の変数
double latitude = 0;        // 現在の経度変数
double longitude = 0;       // 現在の緯度変数
double x_GOAL = 0;
double y_GOAL = 0;
double x = 0;
double y = 0;
int r = 0;
int theta = 0;

/* GPS関連の設定 */
TinyGPSPlus tinyGPS; // tinyGPSPlus object to be used throughout
#define GPS_BAUD 9600 // GPSモジュールのボーレート
// GPSの通信にはソフトウェアシリアルを使う
// 他のシールドとピンがかぶらないように注意する
#define ARDUINO_GPS_RX 9 // GPS TX, Arduino RX pin
#define ARDUINO_GPS_TX 8 // GPS RX, Arduino TX pin
SoftwareSerial ssGPS(ARDUINO_GPS_TX, ARDUINO_GPS_RX); // Create a SoftwareSerial
#define gpsPort ssGPS

/* モニタリング用のシリアル（ハードウェアシリアル）*/
#define SerialMonitor Serial

////////////////////////////////////////////////////////////////////////////////
// ユーザー関数は以下で定義して下さい
////////////////////////////////////////////////////////////////////////////////

// GPSデータ取得関数
void getGPSdata()
{
  while (gpsPort.available())
    tinyGPS.encode(gpsPort.read());
}

// r,thetaをマスターに送る関数（割り込み）
// 2バイト送るので，マスター側でも2バイトそれぞれ受け取ること
void sendData(){
  Wire.write(r);      // 1バイト送信(r)
  Wire.write(theta);  // 1バイト送信(theta)
}

// 処理にかかった時間に関わらず，同じ時間感覚でループを回すために，delayを改造する
static void smartDelay(unsigned long ms)
{
  unsigned long start = millis();
  do
  {
    // If data has come in from the GPS module
    while (gpsPort.available())
      tinyGPS.encode(gpsPort.read()); // Send it to the encode function
    // tinyGPS.encode(char) continues to "load" the tinGPS object with new
    // data coming in from the GPS module. As full NMEA strings begin to come in
    // the tinyGPS library will be able to start parsing them for pertinent info
  } while (millis() - start < ms);
}

// 日付をわかりやすくする関数 "dd/mm/yy"
void printDate()
{
  SerialMonitor.print(tinyGPS.date.day());
  SerialMonitor.print("/");
  SerialMonitor.print(tinyGPS.date.month());
  SerialMonitor.print("/");
  SerialMonitor.println(tinyGPS.date.year());
}

// 時刻をわかりやすくする関数 "hh:mm:ss"
void printTime()
{
  SerialMonitor.print(tinyGPS.time.hour());
  SerialMonitor.print(":");
  if (tinyGPS.time.minute() < 10) SerialMonitor.print('0');
  SerialMonitor.print(tinyGPS.time.minute());
  SerialMonitor.print(":");
  if (tinyGPS.time.second() < 10) SerialMonitor.print('0');
  SerialMonitor.println(tinyGPS.time.second());
}

// GPSデータ表示関数
void printGPSInfo()
{
  SerialMonitor.print("Lat: "); SerialMonitor.println(tinyGPS.location.lat(), 6);
  SerialMonitor.print("Long: "); SerialMonitor.println(tinyGPS.location.lng(), 6);
  SerialMonitor.print("Alt: "); SerialMonitor.println(tinyGPS.altitude.feet());
  SerialMonitor.print("Course: "); SerialMonitor.println(tinyGPS.course.deg());
  SerialMonitor.print("Speed: "); SerialMonitor.println(tinyGPS.speed.mph());
  SerialMonitor.print("Date: "); printDate();
  SerialMonitor.print("Time: "); printTime();
  SerialMonitor.print("Sats: "); SerialMonitor.println(tinyGPS.satellites.value());
  SerialMonitor.println();
}
// GPSデータ表示関数
void printGPSInfo_forLog()
{
  SerialMonitor.print("Lat: "); SerialMonitor.println(tinyGPS.location.lat(), 6);
  SerialMonitor.print("Long: "); SerialMonitor.println(tinyGPS.location.lng(), 6);
  SerialMonitor.print("Alt: "); SerialMonitor.println(tinyGPS.altitude.feet());
  SerialMonitor.print("Course: "); SerialMonitor.println(tinyGPS.course.deg());
  SerialMonitor.print("Speed: "); SerialMonitor.println(tinyGPS.speed.mph());
  SerialMonitor.print("Date: "); printDate();
  SerialMonitor.print("Time: "); printTime();
  SerialMonitor.print("Sats: "); SerialMonitor.println(tinyGPS.satellites.value());
  SerialMonitor.println();
}

////////////////////////////////////////////////////////////////////////////////
// setupは開始時に一回だけ実行される
////////////////////////////////////////////////////////////////////////////////

void setup() {
  // 初期化処理
  pinMode(ledPin, OUTPUT);    // LED用に出力に設定
  SerialMonitor.begin(9600);  // シリアルの開始
  gpsPort.begin(GPS_BAUD);    // GPSソフトウェアシリアルの開始
  Wire.begin(8) ;             // I2Cアドレスを8に設定(こちらがスレーブ側)
  Wire.onRequest(sendData);   // Masterからリクエストが来たときの関数の指定

  // GPS受信を確認するまで待つ
  digitalWrite(ledPin, LOW);
  while(!tinyGPS.location.isUpdated())
  {
    SerialMonitor.println("Searching...");
    SerialMonitor.print("satellites: ");
    SerialMonitor.println(tinyGPS.satellites.value());
    getGPSdata();
  }

  // 現在の位置を最終目的地とする．(10回ほど取ってノイズを減らす)
  x_GOAL = (tinyGPS.location.lng() - LNG_GOAL)*SCALE;
  y_GOAL = (tinyGPS.location.lat()- LAT_GOAL)*SCALE;
  for (int i = 0; i<10; i++)
  {
    x_GOAL = x_GOAL*0.8+((tinyGPS.location.lng() - LNG_GOAL)*SCALE)*0.2;
    y_GOAL = y_GOAL*0.8+((tinyGPS.location.lat()- LAT_GOAL)*SCALE)*0.2;
    smartDelay(500);
  }
  longitude = x_GOAL;
  latitude = y_GOAL;
  // 目的地を表示
  SerialMonitor.println("Goal position has been saved");
  SerialMonitor.print("x_GOAL: ");
  SerialMonitor.println(x_GOAL);
  SerialMonitor.print("y_GOAL: ");
  SerialMonitor.println(y_GOAL);

  delay(1000);            //ちょっとだけ待つ
}

////////////////////////////////////////////////////////////////////////////////
// 以下、メイン関数
////////////////////////////////////////////////////////////////////////////////
void loop() {

  getGPSdata();     // GPSデータ取得

  if (tinyGPS.location.isUpdated()) // GPSデータを取得したら
  {
    digitalWrite(ledPin, HIGH);
    //SerialMonitor.println("GPS updated.");

    // GPSの受信したデータを詳細に見たければ，以下のコメントアウトを解除
    //printGPSInfo();

    /*
    // デバッグ用．目的地を表示
    SerialMonitor.print("x_GOAL: ");
    SerialMonitor.println(x_GOAL);
    SerialMonitor.print("y_GOAL: ");
    SerialMonitor.println(y_GOAL);
    */

    // 現在地を計算(始めの数ケタが変わるほど移動しないので，排除する)
    longitude = longitude*0.8+((tinyGPS.location.lng() - LNG_GOAL)*SCALE)*0.2;
    latitude = latitude*0.8+((tinyGPS.location.lat()- LAT_GOAL)*SCALE)*0.2;



    // 現在地と目的地の誤差を計算
    x = x_GOAL - longitude;         //東西方向（東が正）
    y = y_GOAL - latitude;          //南北方向（北が正）
    theta = atan2(x,y)*180/3.14;    //角度（北から時計回りに何度か）
    r = x*x + y*y;                  //目的地との距離の2乗


    // 現在地と計算結果を表示
    SerialMonitor.print("Lng: ");
    SerialMonitor.println(longitude);
    SerialMonitor.print("Lat: ");
    SerialMonitor.println(latitude);
    SerialMonitor.print("x: ");
    SerialMonitor.println(x);
    SerialMonitor.print("y: ");
    SerialMonitor.println(y);
    SerialMonitor.print("theta: ");
    SerialMonitor.println(theta);
    SerialMonitor.print("r: ");
    SerialMonitor.println(r);

    /*
    // ログ取り用
    SerialMonitor.print(x);       SerialMonitor.print(", ");
    SerialMonitor.print(y);       SerialMonitor.print(", ");
    SerialMonitor.print(theta);   SerialMonitor.print(", ");
    SerialMonitor.println(r);
    */

    smartDelay(100);   //ループは一定時間で回しておく

  }else{
    digitalWrite(ledPin, LOW);
    SerialMonitor.println("GPS failed.");
    // GPSが受信できないなら，ロボットを停止する
    delay(100);
  }
}
