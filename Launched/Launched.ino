#define receiverPWM 2
#define receiverPOW 8
#define receiverLED 3
#define LED 11
#define BUZZER 10
//定义各接线针脚

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  pinMode(receiverPWM,INPUT);
  pinMode(receiverPOW,INPUT);
  pinMode(receiverLED,INPUT);
  pinMode(LED,OUTPUT);
  pinMode(BUZZER,OUTPUT);
}

void loop() {
  // put your main code here, to run repeatedly:
  int PWMVal=0;
  int Val=0;
  int LEDVal=0;
  //初始化作为触发条件的变量
  
  Val=digitalRead(receiverPOW);
  PWMVal=PWMJudge();
  LEDVal=LEDJudge();
  //调用判断函数，将从接收机得到的信号转化为1或0传给触发变量

  if(PWMVal==1 || Val!=1){
    Buzzer();
  }//若符合触发条件（接收机触发或接收机断电），触发蜂鸣器
  
  if(LEDVal==1 && Val!=0){
    Led();
  }//若符合触发条件（接收机触发且接收机未断电），触发Led

}

int PWMJudge(){
  if(pulseIn(receiverPWM,HIGH)<1200){
    return 1;
  }
  return 0;
}//若接收机对应通道PWM值小于1200，返回1，否则返回0

int LEDJudge(){
  if(pulseIn(receiverLED,HIGH)<1200){
    return 1;
  }
  return 0;
}//若接收机对应通道PWM值小于1200，返回1，否则返回0

void Buzzer(){
  for(int i=0; i<100; i++){
    digitalWrite(BUZZER,HIGH);
    delay(1);
    digitalWrite(BUZZER,LOW);
    delay(1);
  }
}//蜂鸣器控制

void Led(){
  for (int a=0; a<=255;a++)
  {
    analogWrite(LED,a);
    delay(8);                                        
  }
    for (int a=255; a>=0;a--)           
  {
    analogWrite(LED,a);
    delay(8);                         
  }
  delay(800);                             
}
//LED灯控制

