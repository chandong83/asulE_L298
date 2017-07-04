#ifndef __ASULL298_H__
#define __ASULL298_H__

#include <inttypes.h>
#include <Arduino.h>

#define Version "0.0.0"

//모터 번호
#define CH1 0
#define CH2 1
#define ALL_CH 2

class AsulL298
{
public:
   AsulL298(uint8_t index = 0);
   AsulL298(uint8_t in1, uint8_t in2, uint8_t enAPin, uint8_t in3, uint8_t in4,uint8_t enBPin);
   void attach();
   String getVersion();   
   void setMotorSpeed(unsigned char mode, unsigned char speed);
   void goForward();
   void goBackward();
   void turnLeft();
   void turnRight();
   void stopMotor();
   void moveMotor(int xAxis, int yAxis, uint8_t speed);
private:
  void go(uint8_t ch, uint8_t speed);
  void back(uint8_t ch, uint8_t speed);
  uint8_t IN1;
  uint8_t IN2;
  uint8_t IN3;
  uint8_t IN4;
  uint8_t ENA;
  uint8_t ENB;  

  
};
  
#endif
