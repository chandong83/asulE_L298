#include "AsulBT.h"
#include "AsulL298.h"

AsulL298 l298(6,7,5,8,9,10);
AsulBT bt;
JOYSTICK_DATA joyData;

void setup() {
  Serial.begin(9600);
  bt.begin(9600);
  l298.attach();
}

void loop() {
    int ret = bt.update();
    
    if(ret < 0)
      return;
    
    switch(ret)
    {
      case CMD_JOYSTICK:
        if(bt.readJoystick(&joyData))
        {        
          /*
          Serial.print(joyData.xAxis);        
          Serial.print(",");        
          Serial.print(joyData.yAxis);      
          Serial.print(",");         
          Serial.println(joyData.btn);*/
          l298.moveMotor(joyData.xAxis, joyData.yAxis, 0);
        }
       break;
       default:
       break;
    }    
}
