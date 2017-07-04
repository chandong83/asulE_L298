#include <Arduino.h>
#include "SoftwareSerial.h"
//#define _USE_DEBUG

#include "AsulBT.h"


//#define btSerial Serial
SoftwareSerial btSerial(3, 2);
//SoftwareSerial *pBtSerial;

AsulBT::AsulBT(uint8_t index)
{
  Cmd_State = RSP_CMD_STATE_WAIT_STX;
  switch(index)
  {
    case 0:
    break;
    case 1:
    break;
    case 2:
    break;
    case 3:
    break;
  }
}

AsulBT::AsulBT(uint8_t rxPin, uint8_t txPin)
{
  Cmd_State = RSP_CMD_STATE_WAIT_STX;  
//  pBtSerial = new SoftwareSerial(rxPin, txPin);
}

void AsulBT::begin(uint32_t baud)
{  
  btSerial.begin(baud);   
}

int AsulBT::update(void)
{    
    int    Ret = -1;
    uint8_t ch;
  //-- 명령어 수신
  //
  if( btSerial.available() )
  {
    ch = btSerial.read();
    #ifdef _USE_DEBUG
    Serial.print(ch, HEX);
    Serial.print(", ");
    #endif
  }
  else
  {
    return -1;
  }


  //-- 바이트간 타임아웃 설정(50ms)
  //
  CurrentTime = millis();

  if( (CurrentTime - PreviousTime) > 1000 )
  {
    Cmd_State    = RSP_CMD_STATE_WAIT_STX;
    PreviousTime = CurrentTime;
  } 

  //-- 명령어 상태
  //
  switch( Cmd_State )
  {

    //-- STX 문자 기다리는 상태 
    //
    case RSP_CMD_STATE_WAIT_STX:

      // 시작 문자를 기다림
      if( ch == RSP_CMD_STX )
      {
        Cmd_State    = RSP_CMD_STATE_WAIT_CMD;
        Cmd.CheckSum = 0x00;
        Cmd.Length   = 0;   
        //Serial.println("STX");    
      }
      break;

    //-- 명령어 기다리는 상태 
    //
    case RSP_CMD_STATE_WAIT_CMD:
      Cmd.Cmd       = ch;
      Cmd.CheckSum ^= ch;
      Cmd_State = RSP_CMD_STATE_WAIT_SIZE;
      //Serial.println("CMD");
      break;

    //-- 데이터 사이즈 기다리는 상태(128까지)
    //
    case RSP_CMD_STATE_WAIT_SIZE:
      //Serial.println("SIZE");
      if( ch <= RSP_CMD_MAX_LENGTH )
      {
        Cmd.Length    = ch;
        Index_Data    = 0;
        Cmd.CheckSum ^= ch;
        if( Cmd.Length > 0 )
        {
          Cmd_State = RSP_CMD_STATE_WAIT_DATA;
        }
        else
        {
          Cmd_State = RSP_CMD_STATE_WAIT_CHECKSUM;  
        }
      }
      else
      {
        Cmd_State = RSP_CMD_STATE_WAIT_STX; 
      }
      break;

    //-- 데이터를 기다리는 상태
    //
    case RSP_CMD_STATE_WAIT_DATA:
      
      //Serial.println("DATA");

      Cmd.CheckSum          ^= ch;
      Cmd.Data[ Index_Data ] = ch;

      Index_Data++;

      if( Index_Data >= Cmd.Length )
      {
        Cmd_State     = RSP_CMD_STATE_WAIT_CHECKSUM;
      } 
      break;

    //-- 체크섬을 기다리는 상태
    //
    case RSP_CMD_STATE_WAIT_CHECKSUM:

      //Serial.println("CHECKSUM");

      Cmd.CheckSumRecv = ch;
      Cmd_State        = RSP_CMD_STATE_WAIT_ETX;    
      break;

    //-- ETX 기다리는 상태
    //
    case RSP_CMD_STATE_WAIT_ETX:

      //Serial.println("ETX");
      #ifdef _USE_DEBUG
      Serial.print(Cmd.CheckSumRecv);
      Serial.print(" ");
      Serial.println(Cmd.CheckSum);
      #endif

      if( ch == RSP_CMD_ETX )
      {       
        if( Cmd.CheckSumRecv == Cmd.CheckSum )
        {
          Ret = Cmd.Cmd;
        }
      }
      Cmd_State = RSP_CMD_STATE_WAIT_STX;   
      break;
  }

  return Ret;
}

void AsulBT::SendCmd( RSP_CMD_OBJ *pCmd )
{
  uint8_t i;
  uint8_t CheckSum = 0;
#ifdef _USE_DEBUG
  Serial.print("STX: ");
  Serial.println(RSP_CMD_STX, HEX);
  Serial.print("CMD: ");
  Serial.println(pCmd->Cmd, HEX);
  Serial.print("LEN: ");
  Serial.println(pCmd->Length, DEC);
#endif
  btSerial.write( RSP_CMD_STX );  
  btSerial.write( pCmd->Cmd );      CheckSum ^= pCmd->Cmd;  
  btSerial.write( pCmd->Length );   CheckSum ^= pCmd->Length;

  //Serial.println("DATA:");
  for( i=0; i<pCmd->Length; i++ )
  {
    btSerial.write( pCmd->Data[i] );
    CheckSum ^= pCmd->Data[i];    
#ifdef _USE_DEBUG    
    Serial.println(pCmd->Data[i], DEC);
#endif    
  }

  btSerial.write( CheckSum );
  btSerial.write( RSP_CMD_ETX );
#ifdef _USE_DEBUG  
  Serial.print("CHK: ");
  Serial.println(CheckSum, HEX);
  Serial.print("ETX: ");
  Serial.println(RSP_CMD_ETX, HEX);
#endif    
  
}

void AsulBT::writeJoystick(JOYSTICK_DATA joyData)
{
  Cmd.Cmd = CMD_JOYSTICK;
  Cmd.Length = sizeof(JOYSTICK_DATA);
  memcpy(Cmd.Data, (uint8_t*)&joyData, sizeof(joyData));  
  SendCmd(&Cmd);
}

int AsulBT::readJoystick(JOYSTICK_DATA* joyData)
{  
  if(Cmd.Cmd == CMD_JOYSTICK)
  {
    memcpy((uint8_t*)joyData, Cmd.Data, sizeof(JOYSTICK_DATA));  
    return 1;
  }
  return -1;
}

uint8_t  AsulBT::write(uint8_t val)
{
  Cmd.Cmd = CMD_RAWDATA;
  Cmd.Length = 1;
  Cmd.Data[0] = val;
  SendCmd(&Cmd);
  return 1;
}

uint8_t  AsulBT::write(String str)
{
  Cmd.Cmd = CMD_RAWDATA;
  Cmd.Length = str.length();
  if(RSP_CMD_MAX_LENGTH < Cmd.Length)
    return 0;
  str.getBytes((unsigned char*)Cmd.Data, Cmd.Length);  
  SendCmd(&Cmd);
  return Cmd.Length;
}

uint8_t AsulBT::write(uint8_t *buf, uint8_t len)
{
  Cmd.Cmd = CMD_RAWDATA;
  Cmd.Length = len;
  if(RSP_CMD_MAX_LENGTH < Cmd.Length)
    return 0;
  memcpy(Cmd.Data, buf, Cmd.Length);  
  SendCmd(&Cmd);
  return Cmd.Length;
}

uint8_t AsulBT::read(uint8_t *buf)
{
  memcpy(buf, Cmd.Data, Cmd.Length); 
  return Cmd.Length;
}

String AsulBT::getVersion()
{
  return Version;
}


