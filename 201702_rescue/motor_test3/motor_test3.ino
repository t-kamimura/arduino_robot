/*モータードライバ制御用プログラム*/

// モーター制御ピンの番号割当
const int motor1_INA = 2;
const int motor1_INB = 4;
const int motor1_pwm = 9;

// モーターパワー
int power = 200;

void setup(){
  pinMode(motor1_INA,OUTPUT); //信号用ピン
  pinMode(motor1_INB,OUTPUT); //信号用ピン
  pinMode(motor1_pwm,OUTPUT);
}

void loop(){
  
  //時計回り
  digitalWrite(motor1_INA,HIGH);
  digitalWrite(motor1_INB,LOW);
  analogWrite(motor1_pwm, power);
  delay(1000);
  
  //反時計回り
  digitalWrite(motor1_INA,LOW);
  digitalWrite(motor1_INB,HIGH);
  analogWrite(motor1_pwm, power);
  delay(1000);
  
  //フリー
  digitalWrite(motor1_INA,LOW);
  digitalWrite(motor1_INB,LOW);
  analogWrite(motor1_pwm, 0);
  delay(1000);
  
  //ブレーキ
  digitalWrite(motor1_INA,HIGH);
  digitalWrite(motor1_INB,HIGH);
  analogWrite(motor1_pwm, 0);
  delay(1000);
}
