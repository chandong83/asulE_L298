#ifndef __ASULBT_H__
#define __ASULBT_H__

#include <inttypes.h>
#include <Arduino.h>

#define RSP_CMD_MAX_LENGTH            128
#define RSP_CMD_STX                   0x02
#define RSP_CMD_ETX                   0x03

#define RSP_CMD_STATE_WAIT_STX        0
#define RSP_CMD_STATE_WAIT_CMD        1
#define RSP_CMD_STATE_WAIT_SIZE       2
#define RSP_CMD_STATE_WAIT_DATA       3
#define RSP_CMD_STATE_WAIT_CHECKSUM   4
#define RSP_CMD_STATE_WAIT_ETX        5

#define CMD_RAWDATA                   0x00
#define CMD_JOYSTICK                  0x01
#define Version "0.0.0"


typedef struct 
{
  uint8_t Cmd;
  uint8_t Length;
  uint8_t CheckSum;
  uint8_t CheckSumRecv; 
  char  Data[RSP_CMD_MAX_LENGTH]; 
} RSP_CMD_OBJ;

typedef struct{
  int xAxis;
  int yAxis;
  uint8_t btn;
}JOYSTICK_DATA;

class AsulBT
{
public:
   AsulBT(uint8_t index = 0);
   AsulBT(uint8_t rxPin, uint8_t txPin);
   void begin(uint32_t baud);
   int update();
   String getVersion();
   void writeJoystick(JOYSTICK_DATA joyData);
   int readJoystick(JOYSTICK_DATA* joyData);   
   uint8_t write(uint8_t val);
   uint8_t write(String str);
   uint8_t write(uint8_t *buf, uint8_t len);
   uint8_t read(uint8_t *buf);

private:
  RSP_CMD_OBJ   Cmd;
  void SendCmd( RSP_CMD_OBJ *pCmd );
  uint8_t  Cmd_State;
  uint8_t  Index_Data;

  uint32_t CurrentTime;
  uint32_t PreviousTime;   
};
  
#endif
