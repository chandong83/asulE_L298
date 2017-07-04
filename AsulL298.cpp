#include <Arduino.h>

#define TOLERANCE   30
#define CENTER_POS  511
#define IS_CENTER(x) ((x >= (CENTER_POS-TOLERANCE))&&(x <= (CENTER_POS+TOLERANCE)))

#include "AsulL298.h"

AsulL298::AsulL298(uint8_t index)
{
}
AsulL298::AsulL298(uint8_t in1, uint8_t in2, uint8_t enAPin, uint8_t in3, uint8_t in4,uint8_t enBPin)
{
 
  IN1 = in1;
  IN2 = in2;
  IN3 = in3;
  IN4 = in4;
  ENA = enAPin;
  ENB = enBPin; 
}

void AsulL298::attach()
{
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);

  pinMode(ENA, OUTPUT);
  pinMode(ENB, OUTPUT);
}

void AsulL298::setMotorSpeed(unsigned char mode, unsigned char speed){
  if(mode == CH1){
    analogWrite(ENA, speed);
  }else if(mode == CH2){
    analogWrite(ENB, speed);    
  }else{
    analogWrite(ENA, speed);
    analogWrite(ENB, speed);    
  }
}

//앞으로
void AsulL298::goForward(){
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
}


//뒤로
void AsulL298::goBackward(){
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
}

//왼쪽
void AsulL298::turnLeft(){
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, HIGH);
}

//오른쪽
void AsulL298::turnRight(){
  
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);  
}

//스톱
void AsulL298::stopMotor(){
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, HIGH);  
}

//개별 축 모터 제어 전진 (ch 채널, speed 속도)
void AsulL298::go(uint8_t ch, uint8_t speed){
   if(ch == CH1)
   {
      digitalWrite(IN1, HIGH);
      digitalWrite(IN2, LOW);
      setMotorSpeed(ch, speed);  
   }
   else if(ch == CH2)
   {
      digitalWrite(IN3, HIGH);
      digitalWrite(IN4, LOW);
      setMotorSpeed(ch, speed);  
   }
   else
   {    
      digitalWrite(IN1, HIGH);
      digitalWrite(IN2, LOW);
      digitalWrite(IN3, HIGH);
      digitalWrite(IN4, LOW);
      setMotorSpeed(ALL_CH, speed);
   }
}

//개별 축 모터 제어 후진(ch 채널, speed 속도)
void AsulL298::back(uint8_t ch, uint8_t speed){
  
   if(ch == CH1)
   {
     digitalWrite(IN1, LOW);
     digitalWrite(IN2, HIGH);   
     setMotorSpeed(ch, speed);
   }
   else if(ch == CH2)
   {
      digitalWrite(IN3, LOW);
      digitalWrite(IN4, HIGH);  
      setMotorSpeed(ch, speed);     
   }
   else
   {    
      digitalWrite(IN1, LOW);
      digitalWrite(IN2, HIGH);
      digitalWrite(IN3, LOW);
      digitalWrite(IN4, HIGH); 
      setMotorSpeed(ALL_CH, speed);
   }
   
}

/*
 * http://www.impulseadventure.com/elec/robot-differential-steering.html
 */
void AsulL298::moveMotor(int xAxis, int yAxis, uint8_t speed)
{
  int nJoyX;
  int nJoyY;
  int     nMotMixL;           // Motor (left)  mixed output           (-128..+127)
  int     nMotMixR;           // Motor (right) mixed output           (-128..+127)
  
  // TEMP VARIABLES
  float   nMotPremixL;    // Motor (left)  premixed output        (-128..+127)
  float   nMotPremixR;    // Motor (right) premixed output        (-128..+127)
  int     nPivSpeed;      // Pivot Speed                          (-128..+127)
  float   fPivScale;      // Balance scale b/w drive and pivot    (   0..1   )
  
  float fPivYLimit = 32.0;
  Serial.print(xAxis);
  Serial.print(" ");
  Serial.println(yAxis);
 
  if(IS_CENTER(xAxis))
    xAxis = CENTER_POS;
  if(IS_CENTER(yAxis))
    yAxis = CENTER_POS;
  nJoyX = map(xAxis, 0, 1023, -128, 127);
  nJoyY = map(yAxis, 0, 1023, -128, 127);
  Serial.print(xAxis);
  Serial.print(" ");
  Serial.println(yAxis);
 
    
  // Calculate Drive Turn output due to Joystick X input
  if (nJoyY >= 0) {
    // Forward
    nMotPremixL = (nJoyX>=0)? 127.0 : (127.0 + nJoyX);
    nMotPremixR = (nJoyX>=0)? (127.0 - nJoyX) : 127.0;
  } else {
    // Reverse
    nMotPremixL = (nJoyX>=0)? (127.0 - nJoyX) : 127.0;
    nMotPremixR = (nJoyX>=0)? 127.0 : (127.0 + nJoyX);
  }
  
  // Scale Drive output due to Joystick Y input (throttle)
  nMotPremixL = nMotPremixL * nJoyY/128.0;
  nMotPremixR = nMotPremixR * nJoyY/128.0;
  
  // Now calculate pivot amount
  // - Strength of pivot (nPivSpeed) based on Joystick X input
  // - Blending of pivot vs drive (fPivScale) based on Joystick Y input
  nPivSpeed = nJoyX;
  fPivScale = (abs(nJoyY)>fPivYLimit)? 0.0 : (1.0 - abs(nJoyY)/fPivYLimit);
  
  // Calculate final mix of Drive and Pivot
  nMotMixL = (1.0-fPivScale)*nMotPremixL + fPivScale*( nPivSpeed);
  nMotMixR = (1.0-fPivScale)*nMotPremixR + fPivScale*(-nPivSpeed);



 
  if(nMotMixL < 0)
  {
    back(CH1, map(abs(nMotMixL), 0, 127, 0, 255));  
  }
  else
  {
    go(CH1, map(abs(nMotMixL), 0, 127, 0, 255));  
  }
  if(nMotMixR < 0)
  {
    back(CH2, map(abs(nMotMixR), 0, 127, 0, 255));  
  }
  else
  {
    go(CH2, map(abs(nMotMixR), 0, 127, 0, 255));  
  }  
}


String AsulL298::getVersion()
{
  return Version;
}

