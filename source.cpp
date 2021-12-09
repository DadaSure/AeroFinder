//基于Arduino的航空模型综合辅助装置

/*
* SD card attached to SPI bus as follows:
** MOSI - pin 11
** MISO - pin 12
** CLK - pin 13
** CS - pin 4
*/

#include <SD.h>
#include <Wire.h>
#define BMP180ADD 0x77  //定义BMP180 IIC总线地址
#define LED 6
#define BUZZER 9
#define receiverPWM 2
#define receiverLED 3
#define receiverPOW 8

int PWMVal;  //用于判断接收机遥控启动报警装置的变量
int LEDVal;  //用于判断接收机遥控启动LED呼吸夜航灯的变量
int Val;  //用于判断接收机是否断电的变量
File myFile;  //文件流
int delayTime = 280;  //气压计算中的参数
int delayTime2 = 40;  //气压计算中的参数
unsigned char OSS; //气压计算采样模式参数

void setup()
{
	Serial.begin(9600);  //初始化串口通信
	Wire.begin();  //作为主机加入IIC总线
	OSS=2;  //气压计算采样模式
	BMP180start();
	pinMode(receiverPOW,INPUT);
	pinMode(receiverPWM,INPUT);
	pinMode(LED,OUTPUT);
	pinMode(BUZZER,OUTPUT);
	while(!Serial){
		;
	}
	Serial.print("Initializing SD card...");
    pinMode(10, OUTPUT);
    if (!SD.begin(4)) {
      Serial.println("initialization failed!");
      return;
    }
    Serial.println("initialization done.");
}

void loop()
{
	PWMVal=PWMJudge();//判断遥控器是否启动报警装置
	LEDVal=LEDJudge();//判断遥控器是否开启LED呼吸夜航灯
	Val=digitalRead(receiverPOW);//判断接收机是否断电
	if(Val==HIGH && PWMVal==1){
		Buzzer();
	}//若满足条件，启动报警装置
	if(LEDVal == 1){
	Led();
	}//根据遥控器信号控制LED呼吸夜航灯
	BMP180();//开始采集大气压强数据并写入SD卡
}

int PWMJudge(){
	if(pulseIn(receiverPWM,HIGH)<1000)
		return 0;
	if(pulseIn(receiverPWM,HIGH)>2000)
		return 1;
}

int LEDJudge(){
	if(pulseIn(receiverPWM,HIGH)<1000)
		return 0;
	if(pulseIn(receiverPWM,HIGH)>2000)
		return 1;
}

void Led(){
	for (int a=0; a<=255;a++)
  {
    analogWrite(11,a);
    delay(8);                                        
  }
    for (int a=255; a>=0;a--)           
  {
    analogWrite(11,a);
    delay(8);                         
  }
  delay(800);                             
}

void Buzzer(){
	for(int i=0; i<100; i++){
		digitalWrite(BUZZER,HIGH);
		delay(1);
		digitalWrite(BUZZER,LOW);
		delay(1);
	}
}

void BMP180()
{
    calculate();
    show();
    myFile = SD.open("BMP.txt", FILE_WRITE);
      if (myFile) {
      Serial.print("Writing to BMP.txt...");
      myFile.print("Temperature: ");
      myFile.print(temperature, 1);                            
      myFile.print(" C");
      myFile.print(" Pressure: ");
      myFile.print(pressure, 0);                              
      myFile.print(" Pa");
      myFile.print(" altitude: ");
      myFile.print(altitude);
      myFile.print(" m");
      myFile.close();  // close the file: 
      Serial.println("  done.");  
      } 
	  else {
      Serial.println("error opening BMP.txt");
	  // if the file didn't open, print an error
	}  
}

/** calculate center **/
void calculate()
{
  temperature = bmp180GetTemperature(bmp180ReadUT());
  temperature = temperature*0.1;
  pressure = bmp180GetPressure(bmp180ReadUP());
  pressure2 = pressure/101325;
  pressure2 = pow(pressure2,0.19029496);
  altitude = 44330*(1-pressure2);                            //altitude = 44330*(1-(pressure/101325)^0.19029496);
}

/** save result **/
void show()
{
    Serial.print("Temperature: ");
    Serial.print(temperature, 1);                            //10 hexadecimal
    Serial.print(" C");
    Serial.print(" Pressure: ");
    Serial.print(pressure, 0);                               //10 hexadecimal
    Serial.print(" Pa");
    Serial.print(" altitude: ");
    Serial.print(altitude);
    Serial.println(" m");
   //   delay(1000);
}

/**BMP180 start program**/
void BMP180start()
{                     /*MSB*/
  ac1 = bmp180ReadDate(0xAA);                      //get full data
  ac2 = bmp180ReadDate(0xAC);  
  ac3 = bmp180ReadDate(0xAE);  
  ac4 = bmp180ReadDate(0xB0);  
  ac5 = bmp180ReadDate(0xB2);  
  ac6 = bmp180ReadDate(0xB4);  
  b1  = bmp180ReadDate(0xB6);  
  b2  = bmp180ReadDate(0xB8);  
  mb  = bmp180ReadDate(0xBA);  
  mc  = bmp180ReadDate(0xBC);  
  md  = bmp180ReadDate(0xBE);
}

/***BMP180 temperature Calculate***/
short bmp180GetTemperature(unsigned int ut)
{
  long x1, x2;
  x1 = (((long)ut - (long)ac6)*(long)ac5) >> 15;  //x1=((ut-ac6)*ac5)/(2^15)
  x2 = ((long)mc << 11)/(x1 + md);                //x2=(mc*2^11)/(x1+md)
  b5 = x1 + x2;                                   //b5=x1+x2
  return ((b5 + 8)>>4);                           //t=(b5+8)/(2^4)
}

/***BMP180 pressure Calculate***/
long bmp180GetPressure(unsigned long up)
{
  long x1, x2, x3, b3, b6, p;
  unsigned long b4, b7;

  b6 = b5 - 4000;

  x1 = (b2 * (b6 * b6)>>12)>>11;
  x2 = (ac2 * b6)>>11;
  x3 = x1 + x2;
  b3 = (((((long)ac1)*4 + x3)<<OSS) + 2)>>2;

  x1 = (ac3 * b6)>>13;
  x2 = (b1 * ((b6 * b6)>>12))>>16;
  x3 = ((x1 + x2) + 2)>>2;
  b4 = (ac4 * (unsigned long)(x3 + 32768))>>15;

  b7 = ((unsigned long)(up - b3) * (50000>>OSS));
  if (b7 < 0x80000000)
    p = (b7<<1)/b4;
  else
    p = (b7/b4)<<1;

  x1 = (p>>8) * (p>>8);
  x1 = (x1 * 3038)>>16;
  x2 = (-7357 * p)>>16;
  p += (x1 + x2 + 3791)>>4;

  return p;
}

/*** Read 1 bytes from the BMP180  ***/ 
int bmp180Read(unsigned char address)
{
  unsigned char data;

  Wire.beginTransmission(BMP180ADD);
  Wire.write(address);
  Wire.endTransmission();

  Wire.requestFrom(BMP180ADD, 1);
  while(!Wire.available());

  return Wire.read();
}

/*** Read 2 bytes from the BMP180 ***/
int bmp180ReadDate(unsigned char address)
{
  unsigned char msb, lsb;
  Wire.beginTransmission(BMP180ADD);
  Wire.write(address);
  Wire.endTransmission();
  Wire.requestFrom(BMP180ADD, 2);
  while(Wire.available()<2);
  msb = Wire.read();
  lsb = Wire.read();
  return (int) msb<<8 | lsb;
}

/*** read uncompensated temperature value ***/
unsigned int bmp180ReadUT()
{
  unsigned int ut;
  Wire.beginTransmission(BMP180ADD);
  Wire.write(0xF4);                       // Write 0x2E into Register 0xF4
  Wire.write(0x2E);                       // This requests a temperature reading
  Wire.endTransmission();  
  delay(5);                               // Wait at least 4.5ms
  ut = bmp180ReadDate(0xF6);               // read MSB from 0xF6 read LSB from (16 bit)
  return ut;
}

/*** Read uncompensated pressure value from BMP180 ***/
unsigned long bmp180ReadUP()
{
  unsigned char msb, lsb, xlsb;
  unsigned long up = 0;

  Wire.beginTransmission(BMP180ADD);
  Wire.write(0xF4);                        // Write 0x34+(OSS<<6) into register 0xF4
  Wire.write(0x34 + (OSS<<6));             // 0x34+oss*64
  Wire.endTransmission(); 
  delay(2 + (3<<OSS));                     // Wait for conversion, delay time dependent on OSS

  Wire.beginTransmission(BMP180ADD);
  Wire.write(0xF6);                        // Read register 0xF6 (MSB), 0xF7 (LSB), and 0xF8 (XLSB)
  Wire.endTransmission();

  Wire.requestFrom(BMP180ADD, 3); 
  while(Wire.available() < 3);             // Wait for data to become available
  msb = Wire.read();
  lsb = Wire.read();
  xlsb = Wire.read();
  up = (((unsigned long) msb << 16) | ((unsigned long) lsb << 8) | (unsigned long) xlsb) >> (8-OSS);//16 to 19 bit
  return up;
}



























	
