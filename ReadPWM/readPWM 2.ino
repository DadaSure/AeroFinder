#define receiverPWM 2
#define receiverPOW 8
#define receiverLED 3
//定义各接线针脚

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  pinMode(receiverPWM,INPUT);
  pinMode(receiverPOW,INPUT);
  pinMode(receiverLED,INPUT);
  Serial.print("Serial Initialized...\n");
  //串口初始化
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
  /*
  int a=0;
  int b=0;
  a=pulseIn(receiverPWM,HIGH);
  b=pulseIn(receiverLED,HIGH);
  Serial.print(a);
  Serial.print(" ");
  Serial.print(b);
  Serial.print("\n");
  //调试时通过串口监测接收机PWM值的部分
  */
  
  delay(300);
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
  Serial.print("BEEP!BEEP!BEEP!\n");
}//蜂鸣器控制

void Led(){
  Serial.print("LED activated...\n");
}//LED灯控制

