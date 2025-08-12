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

//#include "Main.h"
#include "xMain.h"



#ifdef DEBUG_ADS7128
  #define ADS_PRINT(msg)            Serial.print(msg)
  #define ADS_PRINT_HEX(msg, fmt)   Serial.println(msg, fmt)
  #define ADS_PRINTLN(msg)          Serial.println(msg)
  #define ADS_PRINTLN_HEX(msg, fmt) Serial.println(msg, fmt)
#else
  #define ADS_PRINT(msg)
  #define ADS_PRINT_HEX(msg, fmt) 
  #define ADS_PRINTLN(msg)
  #define ADS_PRINTLN_HEX(msg, fmt)
#endif


OSR_CFG  OSR_CFG_SETUP;
OPMODE_CFG OPMODE_CFG_SETUP;
SEQUENCE_CFG SEQUENCE_CFG_SETUP;

//          ADS7128()              
//
// Date              : 2025-04-29
// Description : Constructeur dfe la classe ADS7128
ADS7128::ADS7128(uint8_t i2c_address) {
   _addr = i2c_address;
}

//          BEGIN()              
//
// Date              : 2025-04-29 
// Description : Start I2C
void ADS7128::ActivateCRC(void)
{
   // 1) Clear BOR (bit 0) sans CRC
   uint8_t err = ADS7128_Write_REG_NoCRC(GENERAL_CFG_REG, 0x01);
   if (err != 0) {
      ADS_PRINTLN("❌ clear BOR failed");
      ADS_PRINTLN(err);
      return;
   }
   else ADS_PRINTLN("clear BOR : Success ✅");
   delay(2);

   err = ADS7128_Write_REG_NoCRC(GENERAL_CFG_REG, 0x40);
   // 2) Activer le CRC (bit 6) sans CRC
   if (err != 0) {
      ADS_PRINTLN("❌ enable CRC failed");
      return;
   }
   else ADS_PRINTLN("enable CRC : Success ✅");
   delay(2);
   
   AM_PLC_14M_Reset_I2C();
   delay(10);

}

//        ADS7128_Init_Manual() ** Necessite MODIF **
//
// Date              : 2025-04-29
// Description       : MANUAL INITIALISATION
//
// Parameter         :  OSR: oversampling ratio for ADC conversion result
void ADS7128::ADS7128_Init_Manual(unsigned char OSR)
{
   OSR_CFG_SETUP.OSR = OSR;
   ADS7128_Write_REG(OSR_CFG_REG, OSR_CFG_SETUP.OSR);
   
   ADS7128_OPMODE_CFG(0, 0, 1, 0b0000);

   ADS_PRINTLN("ADS7128 Initialized: ");
   ADS_PRINTLN("");
   ADS_PRINTLN("MODE : Manual");

   ADS_PRINT("OSC_SEL : ");
   if(OPMODE_CFG_SETUP.OSC_SEL == 1) ADS_PRINTLN("Low-power oscillator");
   else                              ADS_PRINTLN("High-speed oscillator");

   ADS_PRINT("CLK_DIV : "); ADS_PRINTLN(OPMODE_CFG_SETUP.CLK_DIV);

   ADS_PRINT("OSR : ");ADS_PRINTLN(OSR_CFG_SETUP.OSR);
}

//        ADS7128_Init_AutoSequence() ** Necessite MODIF **
//
// Date              : 2025-04-29
// Description       : AUTOSEQUENCE INITIALISATION
//
// Parameter         :  OSR: oversampling ratio for ADC conversion result
void ADS7128::ADS7128_Init_AutoSequence(unsigned char OSR)
{
   OSR_CFG_SETUP.OSR = OSR;
   ADS7128_Write_REG(OSR_CFG_REG, OSR_CFG_SETUP.OSR);
   
   ADS7128_OPMODE_CFG(0, 0, 1, 0b0000);
   ADS7128_SEQUENCE_CFG(0, 1);

   ADS_PRINTLN("ADS7128 Initialized: ");
   ADS_PRINTLN("");
   ADS_PRINTLN("MODE : AutoSequence");

   ADS_PRINT("OSC_SEL : ");
   if(OPMODE_CFG_SETUP.OSC_SEL == 1) ADS_PRINTLN("Low-power oscillator");
   else                              ADS_PRINTLN("High-speed oscillator");

   ADS_PRINT("CLK_DIV : ");ADS_PRINTLN(OPMODE_CFG_SETUP.CLK_DIV);

   ADS_PRINT("OSR : ");ADS_PRINTLN(OSR_CFG_SETUP.OSR);
}

//        ADS7128_Init_Autonomous()
//
// Date              : 2025-04-29
// Description       : AUTONOMOUS INITIALISATION
//
// Parameter         :  OSR: oversampling ratio for ADC conversion result
//                      OSC_SEL: 1b = Low Power oscillator | 0b = High Speed oscillator
//                      CLK_DIV: Sampling speed = xxxxb  // All sampling Configurations -> "https://www.ti.com/lit/ds/symlink/ads7128.pdf" P.18 
//                      AUTO_SEQ_CH_SEL:  Select analog input channels AIN[7:0] in for auto sequencing mode.
//                      ALERT_CH_SEL : Select channels for which the alert flags can assert the ALERT pin.
//                      
//                      
void ADS7128::ADS7128_Init_Autonomous(unsigned char OSR, bool OSC_SEL, unsigned char CLK_DIV, unsigned char AUTO_SEQ_CH_SEL, unsigned char ALERT_CH_SEL)
{
   ActivateCRC(), delay(2);
   // oversampling ratio for ADC conversion result
   delay(2);
   OSR_CFG_SETUP.OSR = OSR;
   ADS7128_Write_REG(OSR_CFG_REG, OSR_CFG_SETUP.OSR);

   // Configure device all channels AIN/GPIO[7:0] as analog inputs
   delay(2);
   ADS7128_Write_REG(PIN_CFG_REG, 0b00000000);
   
   // Select analog input channels AIN[7:0] in for auto sequencing mode.
   delay(2);
   ADS7128_AUTO_SEQ_CH_SEL(AUTO_SEQ_CH_SEL);
   
   // Select channels for which the alert flags can assert the ALERT pin.
   delay(2);
   ADS7128_ALERT_CH_SEL(ALERT_CH_SEL);
   
   //Auto-sequence mode (SEQ_MODE = 01b)
   delay(2);
   ADS7128_SEQUENCE_CFG(0, 1);
   
   //Select Autonomous mode (CONV_MODE = 01b)
   delay(2);
   ADS7128_OPMODE_CFG(0, 1, OSC_SEL, CLK_DIV);
   
   // START
   delay(2);
   ADS7128_SEQUENCE_CFG(1, 1);
   
   delay(2);
   ADS7128_Write_REG(GENERAL_CFG_REG, 0x70);

   delay(10);
   ADS_PRINTLN("ADS7128 Initialized: ");
   ADS_PRINTLN("");
   ADS_PRINTLN("MODE : Autonomous");

   ADS_PRINT("OSC_SEL : "); ADS_PRINTLN(ADS7128_Read_REG(OSR_CFG_REG));

   ADS_PRINT("OPMODE_CFG : "); ADS_PRINTLN(ADS7128_Read_REG(OPMODE_CFG_REG));

   ADS_PRINT("SEQUENCE_CFG : "); ADS_PRINTLN(ADS7128_Read_REG(SEQUENCE_CFG_REG));
   
   ADS_PRINT("GENERAL_CFG : "); ADS_PRINTLN(ADS7128_Read_REG(GENERAL_CFG_REG));

}

//        ADS7128_Init_GPIO() ** Necessite MODIF **
//
// Date              : 2025-04-29
// Description       : GPIO INITIALISATION
//
// Parameter         :  Configure GPIO[7:0] as either digital inputs or digital outputs.
//                         0b = GPIO is configured as digital input.
//                         1b = GPIO is configured as digital output.
void ADS7128::ADS7128_Init_GPIO(unsigned char Channel_Configuration)
{
   //PIN_CFG_Configuration = ;   
   ADS7128_Write_REG(PIN_CFG_REG, 0b11111111);

   //GPIO_CFG_Configuration = Channel_Configuration;
   ADS7128_Write_REG(GPIO_CFG_REG, Channel_Configuration);

   ADS_PRINTLN("ADS7128 Initialized: ");
   ADS_PRINTLN("");
   ADS_PRINTLN("MODE : GPIO");
}

//        ADS7128_Write_REG_NoCRC()
//
// Date              : 2025-02-26
// Description       : Configure Register in ADS7128
//
// Parameter         :  _REG :Register 0x??
//                      Data : Data to write in the register without CRC
bool ADS7128::ADS7128_Write_REG_NoCRC(uint8_t _REG, uint8_t Data)
{
   Wire.beginTransmission(_addr);
   Wire.write(0x08);      // opcode write (pas de CRC)
   Wire.write(_REG);
   Wire.write(Data);
   return Wire.endTransmission();
}

//        ADS7128_Write_REG()
//
// Date              : 2025-02-26
// Description       : Configure Register in ADS7128
//
// Parameter         :  _REG :Register 0x??
//                      Data : Data to write in the register
void ADS7128::ADS7128_Write_REG(uint8_t _REG, uint8_t Data)
{
   uint8_t OPCODE = 0x08;
   uint8_t crc_OPCODE = calculateCRC8_CCITT(OPCODE);
   uint8_t crc_REG = calculateCRC8_CCITT(_REG);
   uint8_t crc_Data = calculateCRC8_CCITT(Data);

   Wire.beginTransmission(_addr);
   Wire.write(0x08);
   Wire.write(crc_OPCODE);
   Wire.write(_REG);
   Wire.write(crc_REG);
   Wire.write(Data);
   Wire.write(crc_Data);

   uint8_t error = Wire.endTransmission();

   if (error)
   {
      ADS_PRINT("❌ I2C Write Error! Code: ");
      ADS_PRINTLN(error);
      AM_PLC_14M_Reset_I2C();
      delayMicroseconds(500);

   }
   uint8_t Verification;
   if(_REG == 0x01 && Data == 0x01) return;  
   if(_REG == 0x0 && (Data == 0x01 || Data == 0x02))
   {
      Verification = ADS7128_Read_REG(_REG);
      if(Data == 0x02)
      {
         if(!(Verification & 0x02))
         {
            ADS_PRINT("CRC CLEARED : ");
            ADS_PRINT("SYSTEM_STATUS : 0x"); ADS_PRINTLN_HEX(ADS7128_Read_REG(0x00), HEX);
            return;
         }
      }
      else if(Data == 0x01)
      {
         if(!(Verification & 0x01))
         {
            ADS_PRINT("BOR CLEARED : "); 
            ADS_PRINT("SYSTEM_STATUS : 0x"); ADS_PRINTLN_HEX(ADS7128_Read_REG(0x00), HEX);
            return;
         }
      }
      return;
   }
   if(_REG == 0x1A)
   {
      return;
   }
   
   Verification = ADS7128_Read_REG(_REG);


   if(Verification != Data)
   {
      ADS_PRINT("0x"); ADS_PRINT_HEX(_REG, HEX); 
      ADS_PRINTLN(" Configuration : Failed ❌");
      ADS_PRINT("Tried to write 0x"); ADS_PRINT_HEX(Data, HEX); 
      ADS_PRINT(" but Register is still set to 0x"); ADS_PRINTLN(Verification);
   }
   else ADS_PRINT("0x");
   ADS_PRINT_HEX(_REG, HEX); 
   ADS_PRINTLN(" Configuration : Success ✅");
}

//        ADS7128_Read_REG()
//
// Date              : 2025-02-26
// Description       : Different initialisation for the ADS7128
//
// Parameter         :  _REG :Register 0x??
uint8_t ADS7128::ADS7128_Read_REG(uint8_t _REG) 
{
   uint8_t OPCODE = 0x10;
   uint8_t crc_OPCODE = calculateCRC8_CCITT(OPCODE);
   uint8_t crc_REG = calculateCRC8_CCITT(_REG);

   Wire.beginTransmission(_addr); // Device Address

   Wire.write(OPCODE);     // OCTET 1
   Wire.write(crc_OPCODE);

   Wire.write(_REG);       // OCTET 2
   Wire.write(crc_REG);

   uint8_t error = Wire.endTransmission(true); // Send repeated start
   
   if (error) 
   {
      ADS_PRINT("❌ I2C Read Error on Register 0x"); 
      ADS_PRINT_HEX(_REG, HEX); ADS_PRINT(" | Error Code: "); 
      ADS_PRINTLN(error);
      AM_PLC_14M_Reset_I2C();
      delayMicroseconds(500);
      return 0x00;  // Return clear error value
   }
   delayMicroseconds(150);
   Wire.requestFrom(_addr, 2, true); // ✅ Send NACK on last byte

   unsigned long timeout = millis() + 10; // sécurité 10 ms
   while (Wire.available() < 2) {
      if (millis() > timeout) {
         ADS_PRINTLN("❌ I2C Timeout - No data received");
         return 0x00;
      }
   }
   uint8_t Lecture_DATA = Wire.read();
   uint8_t Lecture_CRC = Wire.read();
   if(calculateCRC8_CCITT(Lecture_DATA) != Lecture_CRC)
   {
      CRC_ERR_Detected = 1;
      return 0;
   }
   return Lecture_DATA;
}

//        ADS7128_ReadChannel_Manual()
//
// Date              : 2025-02-26
// Description       : Read channel in manual
//
// Parameter         :  Channel: Channel to read
uint16_t ADS7128::ADS7128_ReadChannel_Manual(uint8_t Channel)
{
   ADS7128_Write_REG(CHANNEL_SEL_REG, Channel);
   uint16_t adcValue = 0;
   Wire.beginTransmission(_addr); // ADS7128 I2C address
   Wire.endTransmission(false);  // Send restart condition (conversion starts here)

   Wire.requestFrom(_addr, 2); // Request 2 bytes from ADS7128

   if (Wire.available() >= 2) 
   {
      uint8_t msb = Wire.read(); // First byte (D11-D4)
      uint8_t lsb = Wire.read(); // Second byte (D3-D0 + 4 padding bits)
      uint8_t osrConfig = ADS7128_Read_REG(OSR_CFG_REG);
      if(osrConfig > 0x00)
      {
         adcValue = (msb << 8) | lsb;
         //Voltage = convertADCtoVoltage(adcValue, true);
      }
      else
      {
         adcValue = ((msb << 8) | lsb) >> 4; // Shift to get 12-bit value
         //Voltage = convertADCtoVoltage(adcValue, false);
      }
   }
   return adcValue;
}

//        ADS72128_Read_CHx_Autonomous()
//
// Date              : 2025-04-29
// Description       : Read channel in Autonomous
//
// Parameter         :  Channel: Channel to read
uint16_t ADS7128::ADS72128_Read_CHx_Autonomous(uint8_t Channel)
{
   uint8_t MSB = 0;
   uint8_t LSB = 0;
   uint8_t AddrMSB = 0;
   uint8_t AddrLSB = 0;
   switch(Channel)
   {
      case 0:
         AddrMSB = RECENT_CH0_MSB_REG;
         AddrLSB = RECENT_CH0_LSB_REG;
         break;
      case 1:
         AddrMSB = RECENT_CH1_MSB_REG;
         AddrLSB = RECENT_CH1_LSB_REG;
         break;
      case 2:
         AddrMSB = RECENT_CH2_MSB_REG;
         AddrLSB = RECENT_CH2_LSB_REG;
         break;
      case 3:
         AddrMSB = RECENT_CH3_MSB_REG;
         AddrLSB = RECENT_CH3_LSB_REG;
         break;
      case 4:
         AddrMSB = RECENT_CH4_MSB_REG;
         AddrLSB = RECENT_CH4_LSB_REG;
         break;
      case 5:
         AddrMSB = RECENT_CH5_MSB_REG;
         AddrLSB = RECENT_CH5_LSB_REG;
         break;
      case 6:
         AddrMSB = RECENT_CH6_MSB_REG;
         AddrLSB = RECENT_CH6_LSB_REG;
         break;
      case 7:
         AddrMSB = RECENT_CH7_MSB_REG;
         AddrLSB = RECENT_CH7_LSB_REG;
         break;
      default:
         ADS_PRINT("Channel "); ADS_PRINT(Channel); ADS_PRINTLN(" does not exist.");
         break;
   }

   MSB = ADS7128_Read_REG(AddrMSB);
   LSB = ADS7128_Read_REG(AddrLSB);
   uint16_t Value = ((MSB << 8) | LSB) >> 4;
   return Value;
}

//        ADS7128_AUTO_SEQ_CH_SEL()
//
// Date              : 2025-04-29
// Description       : Select analog input channels AIN[7:0] in for auto sequencing mode.
//
// Parameter         :  Configuration: channels AIN[7:0] in for auto sequencing mode.
void ADS7128::ADS7128_AUTO_SEQ_CH_SEL(unsigned char _Config)     // Fonction qui permet de choisir quels channel est scanner en auto
{
   ADS7128_Write_REG(AUTO_SEQ_CH_SEL_REG, _Config);
}

//        ADS7128_AUTO_SEQ_CH_SEL()
//
// Date              : 2025-04-29
// Description       : Configure the register OPMODE_CFG
//
// Parameter         :  CONV_ON_ERR : Control continuation of autonomous modes if CRC error is detected
//                                        1b = devices stop if CRC
//                                        0b = devices continues if CRC
//                      CONV_MODE : 1b = Autonomous               
//                                  0b = Manual
//                      OSC_SEL : 1b = Low Power oscillator
//                                0b = High Speed oscillator
//                      CLK_DIV : Sampling speed control in autonomous monitoring mode
void ADS7128::ADS7128_OPMODE_CFG(bool CONV_ON_ERR, unsigned char CONV_MODE, bool OSC_SEL, unsigned char CLK_DIV)
{
   OPMODE_CFG_SETUP.CONV_ON_ERR = CONV_ON_ERR, OPMODE_CFG_SETUP.CONV_MODE = CONV_MODE, OPMODE_CFG_SETUP.OSC_SEL = OSC_SEL, OPMODE_CFG_SETUP.CLK_DIV = CLK_DIV;
   OPMODE_CFG_SETUP.Configuration = (OPMODE_CFG_SETUP.CONV_ON_ERR << 7) + (OPMODE_CFG_SETUP.CONV_MODE << 5) + (OPMODE_CFG_SETUP.OSC_SEL << 4) + OPMODE_CFG_SETUP.CLK_DIV;
   ADS7128_Write_REG(OPMODE_CFG_REG, OPMODE_CFG_SETUP.Configuration);
}

//        ADS7128_SEQUENCE_CFG()
//
// Date              : 2025-04-29
// Author            : Amerifor
// Description       : Configure the register SEQUENCE_CFG
//
// Parameter         :  SEQ_START : 1b = Stop channel sequencing.
//                                  0b = Start channel sequencing.
//                      SEQ_MODE : 1b =  Manual sequence mode;           
//                                 0b = Auto sequence mode;
void ADS7128::ADS7128_SEQUENCE_CFG(bool SEQ_START, bool SEQ_MODE)
{
   SEQUENCE_CFG_SETUP.SEQ_START = SEQ_START, SEQUENCE_CFG_SETUP.SEQ_MODE = SEQ_MODE;
   SEQUENCE_CFG_SETUP.Configuration = (SEQUENCE_CFG_SETUP.SEQ_START << 4) + SEQUENCE_CFG_SETUP.SEQ_MODE;
   ADS7128_Write_REG(SEQUENCE_CFG_REG, SEQUENCE_CFG_SETUP.Configuration);
}

//        ADS7128_ALERT_CH_SEL()
//
// Date              : 2025-04-29
// Description       : Configure the register SEQUENCE_CFG
//
// Parameter         :  _Config : Select channels for which the alert flags can assert the ALERT pin.
void ADS7128::ADS7128_ALERT_CH_SEL(unsigned char _Config)
{
   ADS7128_Write_REG(ALERT_CH_SEL_REG, _Config);
}

//        ADS7128_Read_EVENT_FLAG()
//
// Date              : 2025-04-30
// Description       : Configure the register SEQUENCE_CFG
//
// return            :  0b = Event condition not detected.
//                      1b = Event condition detected.
uint8_t ADS7128::ADS7128_Read_EVENT_FLAG(void)
{
   return ADS7128_Read_REG(EVENT_FLAG_REG);
}

//        ADS7128_Read_EVENT_HIGH_FLAG()
//
// Date              : 2025-04-30
// Description       : Configure the register SEQUENCE_CFG
//
// return            :  0b = Event condition not detected.
//                      1b = Event condition detected.
uint8_t ADS7128::ADS7128_Read_EVENT_HIGH_FLAG()
{
   uint8_t Flag_State = ADS7128_Read_REG(EVENT_HIGH_FLAG_REG);
   return Flag_State;
}

//        ADS7128_Clear_EVENT_HIGH_FLAG()
//
// Date              : 2025-04-30
// Description       : Configure the register SEQUENCE_CFG
//
// return            :  0b = Event condition not detected.
//                      1b = Event condition detected.
void ADS7128::ADS7128_Clear_EVENT_HIGH_FLAG(uint8_t Channel)
{
   switch (Channel)
   {
      case 0:
         ADS7128_Write_REG(EVENT_HIGH_FLAG_REG, 0b1);
      break;
      case 1:
         ADS7128_Write_REG(EVENT_HIGH_FLAG_REG, 0b10);
         break;
      case 2:
         ADS7128_Write_REG(EVENT_HIGH_FLAG_REG, 0b100);
         break;
      case 3:
         ADS7128_Write_REG(EVENT_HIGH_FLAG_REG, 0b1000);
         break;
      case 4:
         ADS7128_Write_REG(EVENT_HIGH_FLAG_REG, 0b10000);
         break;
      case 5:
         ADS7128_Write_REG(EVENT_HIGH_FLAG_REG, 0b100000);
         break;
      case 6:
         ADS7128_Write_REG(EVENT_HIGH_FLAG_REG, 0b1000000);
         break;
      case 7:
         ADS7128_Write_REG(EVENT_HIGH_FLAG_REG, 0b10000000);
         break;

      default: 
         ADS_PRINT("Channel "); ADS_PRINT(Channel); ADS_PRINTLN(" does not exist.");
         break;
   }
}

//        ADS7128_Read_EVENT_LOW_FLAG()
//
// Date              : 2025-04-30
// Description       : Configure the register SEQUENCE_CFG
//
// return            :  0b = Event condition not detected.
//                      1b = Event condition detected.
uint8_t ADS7128::ADS7128_Read_EVENT_LOW_FLAG()
{
   bool Flag_State = ADS7128_Read_REG(EVENT_LOW_FLAG_REG);
   return Flag_State;
}

//        ADS7128_Clear_EVENT_LOW_FLAG()
//
// Date              : 2025-04-30
// Description       : Configure the register SEQUENCE_CFG
//
// return            :  0b = Event condition not detected.
//                      1b = Event condition detected.
void ADS7128::ADS7128_Clear_EVENT_LOW_FLAG(uint8_t Channel)
{
   switch (Channel)
   {
      case 0:
         ADS7128_Write_REG(EVENT_LOW_FLAG_REG, 0b1);
         break;
      case 1:
         ADS7128_Write_REG(EVENT_LOW_FLAG_REG, 0b10);
         break;
      case 2:
         ADS7128_Write_REG(EVENT_LOW_FLAG_REG, 0b100);
         break;
      case 3:
         ADS7128_Write_REG(EVENT_LOW_FLAG_REG, 0b1000);
         break;
      case 4:
         ADS7128_Write_REG(EVENT_LOW_FLAG_REG, 0b10000);
         break;
      case 5:
         ADS7128_Write_REG(EVENT_LOW_FLAG_REG, 0b100000);
         break;
      case 6:
         ADS7128_Write_REG(EVENT_LOW_FLAG_REG, 0b1000000);
         break;
      case 7:
         ADS7128_Write_REG(EVENT_LOW_FLAG_REG, 0b10000000);
         break;
      
      default: 
         ADS_PRINT("Channel "); ADS_PRINT(Channel); ADS_PRINTLN(" does not exist.");
         break;
   }
}


void ADS7128::ADS7128_Clear_CRC_ERR_IN(void)
{
   ADS7128_Write_REG(0x00, 0x02);         // Clear CRC Error
}
bool ADS7128::ADS7128_Read_CRC_ERR_IN(void)
{
   uint8_t Data = ADS7128_Read_REG(0x00);

   if(!(Data & 0x02))
   {
      ADS_PRINT("CRC ERR False : "); 
      ADS_PRINT("SYSTEM_STATUS : 0x"); 
      ADS_PRINTLN_HEX(ADS7128_Read_REG(0x00), HEX);
      return 0;
   }
   ADS_PRINT("CRC ERR True : ");
   ADS_PRINT("SYSTEM_STATUS : 0x"); ADS_PRINTLN_HEX(ADS7128_Read_REG(0x00), HEX);
   return 1;
}
//        ADS7128_Configure_TH_CHx()
//
// Date              : 2025-04-30
// Description       : Configure the register SEQUENCE_CFG
//
// return            :  Channel : Channel to configure

void ADS7128::ADS7128_Configure_TH_CHx(uint8_t Channel, uint16_t HIGH_TH, uint16_t LOW_TH, uint8_t EVENT_COUNT, uint8_t HYSTERESIS)
{
   EVENT_COUNT = EVENT_COUNT + ((HIGH_TH & 0b0000000000001111)<< 4);
   HIGH_TH = (HIGH_TH >> 4) & 0x0F;

   HYSTERESIS = HYSTERESIS + ((LOW_TH & 0b0000000000001111)<< 4);
   LOW_TH = (LOW_TH >> 4) & 0x0F;

   ADS_PRINT("Hysteresis : "); ADS_PRINT(HYSTERESIS);
   ADS_PRINT("Low TH : "); ADS_PRINT(LOW_TH);
   uint8_t _REG_HIGH_TH = 0;
   uint8_t _REG_EVENT_COUNT = 0;
   uint8_t _REG_LOW_TH = 0;
   uint8_t _REG_HYSTERESIS = 0;

   switch (Channel)
   {
      case 0:
         _REG_HIGH_TH = HIGH_TH_CH0_REG;
         _REG_EVENT_COUNT = EVENT_COUNT_CH0_REG;
         _REG_LOW_TH = LOW_TH_CH0_REG;
         _REG_HYSTERESIS = HYSTERESIS_CH0_REG;
         break;
      
      case 1:
         _REG_HIGH_TH = HIGH_TH_CH1_REG;
         _REG_EVENT_COUNT = EVENT_COUNT_CH1_REG;
         _REG_LOW_TH = LOW_TH_CH1_REG;
         _REG_HYSTERESIS = HYSTERESIS_CH1_REG;
         break;
      case 2:
         _REG_HIGH_TH = HIGH_TH_CH2_REG;
         _REG_EVENT_COUNT = EVENT_COUNT_CH2_REG;
         _REG_LOW_TH = LOW_TH_CH2_REG;
         _REG_HYSTERESIS = HYSTERESIS_CH2_REG;
         break;
      case 3:
         _REG_HIGH_TH = HIGH_TH_CH3_REG;
         _REG_EVENT_COUNT = EVENT_COUNT_CH3_REG;
         _REG_LOW_TH = LOW_TH_CH3_REG;
         _REG_HYSTERESIS = HYSTERESIS_CH3_REG;
         break;
      case 4:
         _REG_HIGH_TH = HIGH_TH_CH4_REG;
         _REG_EVENT_COUNT = EVENT_COUNT_CH4_REG;
         _REG_LOW_TH = LOW_TH_CH4_REG;
         _REG_HYSTERESIS = HYSTERESIS_CH4_REG;
         break;
      case 5:
         _REG_HIGH_TH = HIGH_TH_CH5_REG;
         _REG_EVENT_COUNT = EVENT_COUNT_CH5_REG;
         _REG_LOW_TH = LOW_TH_CH5_REG;
         _REG_HYSTERESIS = HYSTERESIS_CH5_REG;
         break;
      case 6:
         _REG_HIGH_TH = HIGH_TH_CH6_REG;
         _REG_EVENT_COUNT = EVENT_COUNT_CH6_REG;
         _REG_LOW_TH = LOW_TH_CH6_REG;
         _REG_HYSTERESIS = HYSTERESIS_CH6_REG;
         break;
      case 7:
         _REG_HIGH_TH = HIGH_TH_CH7_REG;
         _REG_EVENT_COUNT = EVENT_COUNT_CH7_REG;
         _REG_LOW_TH = LOW_TH_CH7_REG;
         _REG_HYSTERESIS = HYSTERESIS_CH7_REG;
         break;
      
      default: 
         ADS_PRINT("Channel "); ADS_PRINT(Channel); ADS_PRINTLN(" does not exist.");
         break;
   }

   ADS7128_Write_REG(_REG_HIGH_TH, HIGH_TH);
   delay(5);
   if(HIGH_TH  != ADS7128_Read_REG(_REG_HIGH_TH))  ADS_PRINTLN("Configuration _REG_HIGH_TH Complete ! = Failed");
   else                                            ADS_PRINTLN("Configuration _REG_HIGH_TH Complete ! = Sucess");

   ADS7128_Write_REG(_REG_EVENT_COUNT, EVENT_COUNT);
   delay(5);
   if(EVENT_COUNT  != ADS7128_Read_REG(_REG_EVENT_COUNT))  ADS_PRINTLN("Configuration _REG_EVENT_COUNT Complete ! = Failed");
   else                                            ADS_PRINTLN("Configuration _REG_EVENT_COUNT Complete ! = Sucess");

   ADS7128_Write_REG(_REG_LOW_TH, LOW_TH);
   delay(5);
   if(LOW_TH != ADS7128_Read_REG(_REG_LOW_TH))  ADS_PRINTLN("Configuration _REG_LOW_TH Complete ! = Failed");
   else                                            ADS_PRINTLN("Configuration _REG_LOW_TH Complete ! = Sucess");

   ADS7128_Write_REG(_REG_HYSTERESIS, HYSTERESIS);
   delay(5);
   if(HYSTERESIS != ADS7128_Read_REG(_REG_HYSTERESIS))  ADS_PRINTLN("Configuration _REG_HYSTERESIS Complete ! = Failed");
   else                                            ADS_PRINTLN("Configuration _REG_HYSTERESIS Complete ! = Sucess");

}

//        calculateCRC8_CCITT()
//
// Date              : 2025-04-29
// Description       : CALCULE CRC
//

// ********ATTTENTION NE MARCHE PEUX ETRE PAS **************///
uint8_t ADS7128::calculateCRC8_CCITT(const uint8_t data) 
{
   uint8_t crc = 0x00;       // seed initiale
   const uint8_t poly = 0x07; // polynôme x⁸ + x² + x + 1

   // Traitement du seul octet 'data'
   crc ^= data;             
   // 8 itérations MSB-first
   for (uint8_t b = 0; b < 8; b++) {
       if (crc & 0x80) {
           crc = (uint8_t)((crc << 1) ^ poly);
       } else {
           crc <<= 1;
       }
   }

   return crc;
}

//        Test()
//
// Date              : 2025-04-29
// Description       : Lis les 8 channel en boucle en mode Autonomous
void ADS7128::Test(void)
{
   //ADS_PRINT("CH0: "), ADS_PRINTLN(ADS7128_Read_REG(0x01));

   ADS_PRINT("Alert HIGH: "); ADS_PRINTLN(ADS7128_Read_EVENT_HIGH_FLAG());
   ADS_PRINT("Alert LOW: "); ADS_PRINTLN(ADS7128_Read_EVENT_LOW_FLAG());
   
   ADS_PRINT("CH1: "); ADS_PRINTLN(ADS72128_Read_CHx_Autonomous(1));
   ADS_PRINT("ERROR CRC:"); ADS_PRINTLN(ADS7128_Read_CRC_ERR_IN());
   //ADS_PRINT("CH2: "), ADS_PRINTLN(ADS72128_Read_CHx_Autonomous(2));
   //ADS_PRINT("CH3: "), ADS_PRINTLN(ADS72128_Read_CHx_Autonomous(3));
   //ADS_PRINT("CH4: "), ADS_PRINTLN(ADS72128_Read_CHx_Autonomous(4));
   //ADS_PRINT("CH5: "), ADS_PRINTLN(ADS72128_Read_CHx_Autonomous(5));
   //ADS_PRINT("CH6: "), ADS_PRINTLN(ADS72128_Read_CHx_Autonomous(6));
   //ADS_PRINT("CH7: "), ADS_PRINTLN(ADS72128_Read_CHx_Autonomous(7));
}

//        ScanI2C()
//
// Date              : 2025-04-29
// Description       : Trouve l'addresse du ADS7128
void ADS7128::ScanI2C(void)
{
   ADS_PRINTLN("Scan des adresses I2C...");

   // Scanner toutes les adresses I2C possibles
   for (byte address = 1; address < 127; address++) {
     Wire.beginTransmission(address);
     byte error = Wire.endTransmission();
     
     if (error == 0) {
       // Si la transmission réussit, l'adresse est valide
       ADS_PRINT("Dispositif trouvé à l'adresse 0x");
       ADS_PRINTLN_HEX(address, HEX);
     }
   }

   ADS_PRINTLN("Scan terminé.");
}

//        ScanI2C()
//
// Date              : 2025-02-26
// Description       : Convertie valeur ADC en voltage 0 a 5 volt
float ADS7128::convertADCtoVoltage(uint16_t adcValue, bool is16Bit) 
{
   float voltage;
   
   if (is16Bit) {
       voltage = ((float)adcValue / 65535.0) * 5.00; // 16-bit OSR mode
   } else {
       voltage = ((float)adcValue / 4095.0) * 5.00; // 12-bit normal mode
   }

   return voltage;
}
