#include "DualVNH5019MotorShield.h"

//モーターオブジェクト"md"生成
DualVNH5019MotorShield md;

//モーターエラー時の処理を行う関数
void stopIfFault()
{
  if (md.getM1Fault())
  {
    Serial.println("M1 fault");
    while(1);
  }
  if (md.getM2Fault())
  {
    Serial.println("M2 fault");
    while(1);
  }
}

 
void setup()
{
  Serial.begin(9600);
  Serial.println("Dual VNH5019 Motor Shield");
  md.init();  //モーターオブジェクトの初期化
}
 
void loop()
{
  // 0から400まで加速．電流センサーも観察してみよう
  for (int i = 0; i <= 400; i++)
  {
    md.setM1Speed(i);       // M1の速度を決める．
    stopIfFault();          // エラー処理
    if (i%200 == 100)
    {
      Serial.print("M1 current: ");
      Serial.println(md.getM1CurrentMilliamps()); //モーターに流れている電流値
    }
    delay(2);
  }

  // 400から-400まで．
  for (int i = 400; i >= -400; i--)
  {
    md.setM1Speed(i);
    stopIfFault();
    if (i%200 == 100)
    {
      Serial.print("M1 current: ");
      Serial.println(md.getM1CurrentMilliamps());
    }
    delay(2);
  }

  // -400から0まで．
  for (int i = -400; i <= 0; i++)
  {
    md.setM1Speed(i);
    stopIfFault();
    if (i%200 == 100)
    {
      Serial.print("M1 current: ");
      Serial.println(md.getM1CurrentMilliamps());
    }
    delay(2);
  }
}
