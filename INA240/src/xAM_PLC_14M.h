#ifndef XAM_PLC_14M_H
#define XAM_PLC_14M_H

//#include "PCA9685.h" PCA9685-Arduino by NachtraveVL

#define Pin_Relais_Urgence D6
#define Pin_LED_ERROR D11

#define Pin_Input_I1 D2
#define Pin_Input_I2 D3

#define Pin_OUTPUT_PWM1 D7
#define Pin_OUTPUT_PWM2 D8

#define Pin_DAC A0

#define SDA_PIN A4
#define SCL_PIN A5

byte I2C_Device_list[] = {0x40, 0x41, 0x13};
unsigned char I2C_Device_Error = 0;
unsigned long Time_error = 0;
unsigned char Error_State = 0;
unsigned char AError_State = 0;
unsigned char RF_Error_Count = 0;

unsigned int ValueCh[14] = {0, 0, 0, 0 , 0, 0, 0, 0, 0, 0, 0 ,0 ,0, 0};
unsigned int AValueCh[14] = {0, 0, 0, 0 , 0, 0, 0, 0, 0, 0, 0 ,0 ,0, 0};

void AM_PLC_14M_Init(void);

void AM_PLC_14M_Init_I2C(void);
void AM_PLC_14M_Reset_I2C(void);
void AM_PLC_14M_BusRecovery(void);
void AM_PLC_14M_GeneralCallReset(void);

void AM_PLC_14M_Init_CAN(void);

void AM_PLC_14M_Init_PCA9685(void);
void AM_PLC_14M_Activate_PCA9685_1(bool State);
void AM_PLC_14M_Activate_PCA9685_2(bool State);
void AM_PLC_14M_setAllMosfetsOff(void);
void AM_PLC_14M_SetMosfet(unsigned char PWM, int DesiredState);
void AM_PLC_14M_SetPCA9685_1(unsigned char PWM, int DesiredState);
void AM_PLC_14M_SetPCA9685_2(unsigned char PWM, int DesiredState);

void AM_PLC_14M_SetRelaisUrgence(bool State);
void AM_PLC_14M_SetRelaisK1(bool State);
void AM_PLC_14M_SetRelaisK2(bool State);

void AM_PLC_14M_SetError(bool State);

bool AM_PLC_14M_ReadI1(void);
bool AM_PLC_14M_ReadI2(void);

bool AM_PLC_14M_DAC(unsigned char Dac_Value);

void AM_PLC_14M_ScanI2C(void);
void AM_PLC_14M_Verify_RF(void);
void AM_PLC_14M_ErrorState(void);
void AM_PLC_14M_Verify_Error(void);

void Lecture(void);

void AM_PLC_14M_Test(void);
void AM_PLC_14M_TestMosfet(void);

#endif