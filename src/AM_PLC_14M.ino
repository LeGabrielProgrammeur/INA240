/*
###############################################################################
# File:          RN2903.ino
# Author:        Amérifor
# Created:       2025-04-26
# Last Updated:  2025-04-26
#
# Description:   Script handling RF module initialization, communication,
#               sleep management, and message transmission.
# 
# 
#      █████╗ ███╗   ███╗███████╗██████╗ ██╗███████╗ ██████╗ ██████╗ 
#     ██╔══██╗████╗ ████║██╔════╝██╔══██╗██║██╔════╝██╔═══██╗██╔══██╗
#     ███████║██╔████╔██║█████╗  ██████╔╝██║█████╗  ██║   ██║██████╔╝
#     ██╔══██║██║╚██╔╝██║██╔══╝  ██╔══██╗██║██╔══╝  ██║   ██║██╔══██╗
#     ██║  ██║██║ ╚═╝ ██║███████╗██║  ██║██║██║     ╚██████╔╝██║  ██║
#      ╚═╝  ╚═╝╚═╝     ╚═╝╚══════╝╚═╝  ╚═╝╚═╝╚═╝      ╚═════╝ ╚═╝  ╚═╝
#                                                                        
#
###############################################################################
*/

#include "xMain.h"

PCA9685 pwmController1(0x40);
PCA9685 pwmController2(0x41);

//          AM_PLC_14M_Init()              
//
// Date              : 2025-04-29
// Description : Initialisation of the AM-PLC-14M
void AM_PLC_14M_Init(void)
{

  // Start I2C
  AM_PLC_14M_Init_I2C(), delayMicroseconds(500);
  
  // Soft-Reset PCA et ADS
  AM_PLC_14M_GeneralCallReset();
  
  //Relais Urgence
  pinMode(Pin_Relais_Urgence, OUTPUT), AM_PLC_14M_SetRelaisUrgence(false);

  //LED ERROR
  pinMode(Pin_LED_ERROR, OUTPUT), AM_PLC_14M_SetError(false);
  
  // I1 et I2
  pinMode(Pin_Input_I1, INPUT_PULLUP), pinMode(Pin_Input_I2, INPUT_PULLUP);
  
  // DAC
  pinMode(Pin_DAC, OUTPUT);
}

//          AM_PLC_14M_Init_I2C()              
//
// Date              : 2025-05-16
// Description : Initialisation of I2C
void AM_PLC_14M_Init_I2C(void)
{
  AM_PLC_14M_BusRecovery(); // 
  Wire.begin();
  delay(100);
}

//          AM_PLC_14M_Init_I2C()              
//
// Date              : 2025-05-16
// Description : Soft Reset des chip I2C
void AM_PLC_14M_GeneralCallReset(void)
{
  Wire.beginTransmission(0x00);          // GENERAL CALL RESET 
  Wire.write(0x06);
  Wire.endTransmission(true);

  delay(10);
}

//          AM_PLC_14M_Reset_I2C()              
//
// Date              : 2025-05-16
// Description : Reset I2C
void AM_PLC_14M_Reset_I2C(void)
{
  Wire.end();
  AM_PLC_14M_BusRecovery();
  Wire.begin();
  Lecture();
}

//          AM_PLC_14M_BusRecovery()              
//
// Date              : 2025-05-16
// Description : Bus Recuperation
void AM_PLC_14M_BusRecovery(void)
{
  pinMode(SDA_PIN, INPUT_PULLUP);
  pinMode(SCL_PIN, INPUT_PULLUP);

  if (digitalRead(SDA_PIN) == LOW) {
    pinMode(SCL_PIN, OUTPUT);
    for (int i = 0; i < 9; i++) {
      digitalWrite(SCL_PIN, HIGH);
      delayMicroseconds(5);
      digitalWrite(SCL_PIN, LOW);
      delayMicroseconds(5);
    }
    pinMode(SCL_PIN, INPUT_PULLUP);
  }

  pinMode(SDA_PIN, OUTPUT);
  digitalWrite(SDA_PIN, LOW);
  delayMicroseconds(5);

  pinMode(SCL_PIN, OUTPUT);
  digitalWrite(SCL_PIN, HIGH);
  delayMicroseconds(5);

  digitalWrite(SDA_PIN, HIGH);
  delayMicroseconds(5);
}

//          AM_PLC_14M_Init_CAN()              
//
// Date              : 2025-05-16
// Description : Initalisation CAN Bus
void AM_PLC_14M_Init_CAN(void)
{
  if (!CAN.begin(CanBitRate::BR_250k))
  {
    SYSTEM_PRINTLN("CAN.begin(...) failed.");
    for (;;) {}
  }
}

//          AM_PLC_14M_Init_PCA9685()              
//
// Date              : 2025-05-16
// Description : Initalisation of PCA9685
void AM_PLC_14M_Init_PCA9685(void)
{
  pinMode(Pin_OUTPUT_PWM1, OUTPUT);
  pinMode(Pin_OUTPUT_PWM2, OUTPUT);

  AM_PLC_14M_Activate_PCA9685_1(false);
  AM_PLC_14M_Activate_PCA9685_2(false);

  pwmController1.init(), delay(5); // Initializes module using default totem-pole driver mode, and default disabled phase balancer
  pwmController2.init(), delay(5); // Initializes module using default totem-pole driver mode, and default disabled phase balancer
  
  pwmController1.setPWMFrequency(100), delay(5); // Set PWM freq to 100Hz (default is 200Hz, supports 24Hz to 1526Hz)
  pwmController2.setPWMFrequency(100), delay(5); // Set PWM freq to 100Hz (default is 200Hz, supports 24Hz to 1526Hz)

  AM_PLC_14M_setAllMosfetsOff(), delay(5);
  Lecture();
}

//          AM_PLC_14M_Activate_PCA9685_X()              
//
// Date              : 2025-05-16
// Description : Activate or desactivate PCA9685
void AM_PLC_14M_Activate_PCA9685_1(bool State)
{
  State = !State;
  digitalWrite(Pin_OUTPUT_PWM1, State);
}
void AM_PLC_14M_Activate_PCA9685_2(bool State)
{
  State = !State;
  digitalWrite(Pin_OUTPUT_PWM2, State);
}

//          AM_PLC_14M_setAllMosfetsOff()              
//
// Date              : 2025-05-16
// Description : Set all mosfet to zero
void AM_PLC_14M_setAllMosfetsOff(void)
{
  pwmController1.setAllChannelsPWM(0);
  pwmController2.setAllChannelsPWM(0);
}

//          AM_PLC_14M_SetMosfet()              
//
// Date              : 2025-05-16
// Description : Set mosfet to desired PWM
void AM_PLC_14M_SetMosfet(unsigned char PWM, int DesiredState)
{
  if(PWM <= 7)
  {
    AM_PLC_14M_SetPCA9685_1(PWM - 1, DesiredState);
  }
  else if(PWM > 7)
  {
    AM_PLC_14M_SetPCA9685_2(PWM - 8, DesiredState);
  }
}
void AM_PLC_14M_SetPCA9685_1(unsigned char PWM, int DesiredState)
{
  pwmController1.setChannelPWM(PWM, DesiredState); // Set PWM to 128/255, shifted into 4096-land
}
void AM_PLC_14M_SetPCA9685_2(unsigned char PWM, int DesiredState)
{
  pwmController2.setChannelPWM(PWM, DesiredState); // Set PWM to 128/255, shifted into 4096-land
}

//          AM_PLC_14M_SetRelaisUrgence()              
//
// Date              : 2025-05-16
// Description : set relais urgence
void AM_PLC_14M_SetRelaisUrgence(bool State)
{
  digitalWrite(Pin_Relais_Urgence, State);
}

//          AM_PLC_14M_SetRelaisK1()              
//
// Date              : 2025-05-16
// Description : set relais K1
void AM_PLC_14M_SetRelaisK1(bool State)
{
  if(State == 0)  pwmController2.setChannelPWM(7, 0);
  else pwmController2.setChannelPWM(7, 4096);
}

//          AM_PLC_14M_SetRelaisK2()              
//
// Date              : 2025-05-16
// Description : set relais K2
void AM_PLC_14M_SetRelaisK2(bool State)
{
  if(State == 0)  pwmController1.setChannelPWM(7, 0);
  else pwmController1.setChannelPWM(7, 4096);
}

//          AM_PLC_14M_SetError()              
//
// Date              : 2025-05-16
// Description : set Error
void AM_PLC_14M_SetError(bool State)
{
  digitalWrite(Pin_LED_ERROR, State);
}

//          AM_PLC_14M_ReadI1()              
//
// Date              : 2025-05-16
// Description : Read input 1
bool AM_PLC_14M_ReadI1(void)
{
  return (!digitalRead(Pin_Input_I1));
}

//          AM_PLC_14M_ReadI2()              
//
// Date              : 2025-05-16
// Description : Read input 2
bool AM_PLC_14M_ReadI2(void)
{
  return (!digitalRead(Pin_Input_I2));
}

//          AM_PLC_14M_DAC()              
//
// Date              : 2025-05-16
// Description : Set DAC
bool AM_PLC_14M_DAC(unsigned char Dac_Value)
{
  analogWrite(Pin_DAC, Dac_Value);
}

void AM_PLC_14M_ScanI2C(void)
{
  for(unsigned char Position = 0; Position < (sizeof(I2C_Device_list)/sizeof(I2C_Device_list[0])); Position++)
  {
    Wire.beginTransmission(I2C_Device_list[Position]);
    byte error = Wire.endTransmission();
    if(error > 0) 
    {
      if(I2C_Device_Error < 10) I2C_Device_Error++;
    }
  }

  if(I2C_Device_Error >= 10)
  {
    AM_PLC_14M_SetRelaisUrgence(false);
    AM_PLC_14M_Activate_PCA9685_1(false);
    AM_PLC_14M_Activate_PCA9685_2(false);

    Error_State = Error_State | 0x01;
  }
  else Error_State = (Error_State & 0xFE);                
}
 
void AM_PLC_14M_Verify_RF(void)
{
  if(RF_Error_Count >= 3)
  {
    AM_PLC_14M_SetRelaisUrgence(false);
    AM_PLC_14M_Activate_PCA9685_1(false);
    AM_PLC_14M_Activate_PCA9685_2(false);
    Error_State = Error_State | 0x02;
    return;
  }
  else Error_State = Error_State & 0xFD;

  if((millis() - Delay_timeout) > 250)
  {
    Delay_timeout += 250;
    RF_Error_Count++;
    //SYSTEM_PRINT("RF_Error_Count: "); SYSTEM_PRINTLN(RF_Error_Count);
  }
}

void AM_PLC_14M_ErrorState(void)
{

  if((Error_State == 0) && (AError_State == Error_State)) return;
  if((Error_State & 0x04) == 4)
  {
    AError_State = Error_State;
  }
  else if((Error_State & 0x02) == 2)
  {
    AError_State = Error_State;
    if ((millis() - Time_error) < 200) AM_PLC_14M_SetError(true);
    else if ((millis() - Time_error) < 400) AM_PLC_14M_SetError(false);
    else if ((millis() - Time_error) < 600) AM_PLC_14M_SetError(true);
    else if ((millis() - Time_error) < 2000) AM_PLC_14M_SetError(false);
    else if((millis() - Time_error) > 2000) Time_error = millis();

  }
  else if((Error_State & 0x01) == 1)
  {
    AError_State = Error_State;
    if ((millis() - Time_error) < 200) AM_PLC_14M_SetError(true);
    else if ((millis() - Time_error) < 2000) AM_PLC_14M_SetError(false);
    else if((millis() - Time_error) > 2000) Time_error = millis();
  }
  if((Error_State == 0) && (AError_State != Error_State))
  {
    AM_PLC_14M_SetRelaisUrgence(true);
    AM_PLC_14M_Activate_PCA9685_1(true);
    AM_PLC_14M_Activate_PCA9685_2(true);
    AM_PLC_14M_SetError(false);
    AError_State = Error_State;
  }
}
unsigned long Delay_Lecture = millis();
void AM_PLC_14M_Verify_Error(void)
{
  if((millis() - Delay_Lecture) < 125) return;
  Delay_Lecture += 125;
  
  Lecture();
  AM_PLC_14M_ScanI2C();
  if((Error_State & 0x04) != 0x04) AM_PLC_14M_Verify_RF();
  AM_PLC_14M_ErrorState();
}


//          Lecture()              
//
// Date              : 2025-05-16
// Description : Lecture des channel 0 a 13
void Lecture(void)
{
  for(unsigned char i = 0; i < 14; i++)
  {
    if(ValueCh[i] != AValueCh[i]) return; // Si pas le temps de changer

    if(i < 7)
    {
      if(pwmController1.getChannelPWM(i) != ValueCh[i])
      {
        //SYSTEM_PRINT("MOSFET["); SYSTEM_PRINT(i); SYSTEM_PRINT("] : "); SYSTEM_PRINTLN("Not Correct");
        AM_PLC_14M_SetMosfet(i + 1, ValueCh[i]), delayMicroseconds(10);
        AValueCh[i] = ValueCh[i];
      }
      //else SYSTEM_PRINT("MOSFET["); SYSTEM_PRINT(i); SYSTEM_PRINT("] : "); SYSTEM_PRINTLN("Correct");
    }
    else
    {
      if(pwmController2.getChannelPWM(i-7) != ValueCh[i])
      {
        //SYSTEM_PRINT("MOSFET["); SYSTEM_PRINT(i); SYSTEM_PRINT("] : "); SYSTEM_PRINTLN("Not Correct");
        AM_PLC_14M_SetMosfet(i + 1, ValueCh[i]), delayMicroseconds(10);
        AValueCh[i] = ValueCh[i];
      }     
      //else SYSTEM_PRINT("MOSFET["); SYSTEM_PRINT(i); SYSTEM_PRINT("] : "); SYSTEM_PRINTLN("Correct");
    }
  }
}

void AM_PLC_14M_Test(void)
{
  SYSTEM_PRINT("Input1: "); SYSTEM_PRINTLN(AM_PLC_14M_ReadI1());
  SYSTEM_PRINT("Input2: "); SYSTEM_PRINTLN(AM_PLC_14M_ReadI2());


  AM_PLC_14M_SetError(0);
  AM_PLC_14M_DAC(125);
  AM_PLC_14M_SetRelaisUrgence(0);

  delay(500);

  AM_PLC_14M_SetError(1);
  AM_PLC_14M_DAC(256);
  AM_PLC_14M_SetRelaisUrgence(0);   
  delay(500);
}

void AM_PLC_14M_TestMosfet(void)
{
  AM_PLC_14M_SetMosfet(0, 4000);
  delay(100);
  AM_PLC_14M_SetMosfet(1, 4000);
  delay(100);
  AM_PLC_14M_SetMosfet(2, 4000);
  delay(100);
  AM_PLC_14M_SetMosfet(3, 4000);
  delay(100);
  AM_PLC_14M_SetMosfet(4, 4000);
  delay(100);
  AM_PLC_14M_SetMosfet(5, 4000);
  delay(100);
  AM_PLC_14M_SetMosfet(6, 4000);
  delay(100);
  AM_PLC_14M_SetMosfet(8, 4000);
  delay(100);
  AM_PLC_14M_SetMosfet(9, 4000);
  delay(100);
  AM_PLC_14M_SetMosfet(10, 4000);
  delay(100);
  AM_PLC_14M_SetMosfet(11, 4000);
  delay(100);
  AM_PLC_14M_SetMosfet(12, 4000);
  delay(100);
  AM_PLC_14M_SetMosfet(13, 4000);
  delay(100);
  AM_PLC_14M_SetMosfet(14, 4000);
  delay(100);
  AM_PLC_14M_SetMosfet(14, 0000);
  delay(100);
  AM_PLC_14M_SetMosfet(13, 0000);
  delay(100);
  AM_PLC_14M_SetMosfet(12, 0000);
  delay(100);
  AM_PLC_14M_SetMosfet(11, 0000);
  delay(100);
  AM_PLC_14M_SetMosfet(10, 0000);
  delay(100);
  AM_PLC_14M_SetMosfet(9, 0000);
  delay(100);
  AM_PLC_14M_SetMosfet(8, 0000);
  delay(100);
  AM_PLC_14M_SetMosfet(6, 0000);
  delay(100);
  AM_PLC_14M_SetMosfet(5, 0000);
  delay(100);
  AM_PLC_14M_SetMosfet(4, 0000);
  delay(100);
  AM_PLC_14M_SetMosfet(3, 0000);
  delay(100);
  AM_PLC_14M_SetMosfet(2, 0000);
  delay(100);
  AM_PLC_14M_SetMosfet(1, 0000);
  delay(100);
  AM_PLC_14M_SetMosfet(0, 0000);
  delay(100);
}

