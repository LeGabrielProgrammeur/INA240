/*
###############################################################################
# File:          MRF-RTC.ino
# Author:        Mikael Lavoie-Fortin et Gabriel Dubeau / Amérifor
# Created:       2025-02-26
# Last Updated:  2025-02-26
#
# Description:   Script to use the PCF8563 RTC module
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

#ifndef xADS7128_H
#define xADS7128_H



#include <Arduino.h>


// ADS7128 Registers
#define SYSTEM_STATUS_REG    0x00
#define GENERAL_CFG_REG      0x01
#define DATA_CFG_REG         0x02

/*                  OSR_CFG_REG                         */

#define OSR_CFG_REG          0x03    // "https://www.ti.com/lit/ds/symlink/ads7128.pdf" P.35
struct OSR_CFG
{
    unsigned char OSR = 0b00000000; // Selects the oversampling ratio for ADC conversion result.
                                    // 0b = No averaging
                                    // 1b = 2 samples
                                    // 10b = 4 samples
                                    // 11b = 8 samples
                                    // 100b = 16 samples
                                    // 101b = 32 samples
                                    // 110b = 64 samples
                                    // 111b = 128 samples
};
extern OSR_CFG  OSR_CFG_SETUP;

/*                  OPMODE_CFG Register                 */
#define OPMODE_CFG_REG       0x04    // "https://www.ti.com/lit/ds/symlink/ads7128.pdf" P.36
struct OPMODE_CFG
{
    bool CONV_ON_ERR = 0;           // Control continuation of autonomous modes if CRC error is detected
                                    // 1b = devices stop if CRC      |  0b = devices continues if CRC
    bool CONV_MODE = 0;             // 1b = Autonomous               |  0b = Manual
    bool OSC_SEL = 0;               // 1b = Low Power oscillator     |  0b = High Speed oscillator
    unsigned char CLK_DIV = 0b0000; // Sampling speed = xxxxb
                                    // All sampling Configurations -> "https://www.ti.com/lit/ds/symlink/ads7128.pdf" P.18 
    unsigned char Configuration = 0b00000000;
};
extern OPMODE_CFG OPMODE_CFG_SETUP;

/*                  PIN_CFG                             */
#define PIN_CFG_REG          0x05    // "https://www.ti.com/lit/ds/symlink/ads7128.pdf" P.36
//unsigned char PIN_CFG_Configuration = 0b00000000;   // 1 = GPIO   |   0 = ANALOG

/*                  GPIO_CFG                             */
#define GPIO_CFG_REG         0x07    // "https://www.ti.com/lit/ds/symlink/ads7128.pdf" P.36
//unsigned char GPIO_CFG_Configuration = 0b00000000;  // 1 = INPUTS |   0 = OUTPUTS
#define ADS7128_OUTPUT 1
#define ADS7128_INPUT 0

#define GPO_DRIVE_CFG_REG    0x09    // Configure digital outputs GPO[7:0] as either open-drain or push-pull outputs.
#define GPO_VALUE_REG        0x0B    // Logic level to be set on digital outputs GPO[7:0].
#define GPI_VALUE_REG        0x0D    // READ GPIO in Input
#define ZCD_BLANKING_CFG_REG 0x0F    // Manual mode: Conversion start register

/*                  SEQUENCE_CFG                         */
#define SEQUENCE_CFG_REG     0x10  // "https://www.ti.com/lit/ds/symlink/ads7128.pdf" P.38
struct SEQUENCE_CFG
{
    bool SEQ_START = 0;            // 0b = Stop channel sequencing.  | 1b = Start channel sequencing
    bool SEQ_MODE = 0;             // 0b = Manual sequence mode      | 1b = Auto sequence mode  
    unsigned char Configuration = 0;

};
extern SEQUENCE_CFG SEQUENCE_CFG_SETUP;
#define CHANNEL_SEL_REG 0x11
/*                  AUTO_SEQ_CH_SEL                      */
#define AUTO_SEQ_CH_SEL_REG  0x12    // "https://www.ti.com/lit/ds/symlink/ads7128.pdf" P.39
//unsigned char AUTO_SEQ_CH_SEL_Configuration = 0b00000000;   //  0b | 0 | 0 | 0 | 0 | 0 | 0 | 0 | 0
                                                            //      CH7 CH6 CH5 CH4 CH3 CH2 CH1 CH0

#define ALERT_CH_SEL_REG     0x14
#define ALERT_MAP_REG        0x16
#define ALERT_PIN_CFG_REG    0x17
#define EVENT_FLAG_REG       0x18   // 0 = Event not detected | 1 = Event Detected
#define EVENT_HIGH_FLAG_REG  0x1A
#define EVENT_LOW_FLAG_REG   0x1C
#define EVENT_RGN_REG        0x1E

//CHANNELS REGISTERS

//CHANNEL 0
#define RECENT_CH0_LSB_REG  0xA0
#define RECENT_CH0_MSB_REG  0xA1

#define HYSTERESIS_CH0_REG  0x20
#define HIGH_TH_CH0_REG     0x21
#define EVENT_COUNT_CH0_REG 0x22
#define LOW_TH_CH0_REG      0x23

//CHANNEL 1
#define RECENT_CH1_LSB_REG  0xA2
#define RECENT_CH1_MSB_REG  0xA3

#define HYSTERESIS_CH1_REG  0x24
#define HIGH_TH_CH1_REG     0x25
#define EVENT_COUNT_CH1_REG 0x26
#define LOW_TH_CH1_REG      0x27

//CHANNEL 2
#define RECENT_CH2_LSB_REG  0xA4
#define RECENT_CH2_MSB_REG  0xA5

#define HYSTERESIS_CH2_REG  0x28
#define HIGH_TH_CH2_REG     0x29
#define EVENT_COUNT_CH2_REG 0x2A
#define LOW_TH_CH2_REG      0x2B
//CHANNEL 3
#define RECENT_CH3_LSB_REG  0xA6
#define RECENT_CH3_MSB_REG  0xA7

#define HYSTERESIS_CH3_REG  0x2C
#define HIGH_TH_CH3_REG     0x2D
#define EVENT_COUNT_CH3_REG 0x2E
#define LOW_TH_CH3_REG      0x2F
//CHANNEL 4
#define RECENT_CH4_LSB_REG  0xA8
#define RECENT_CH4_MSB_REG  0xA9

#define HYSTERESIS_CH4_REG  0x30
#define HIGH_TH_CH4_REG     0x31
#define EVENT_COUNT_CH4_REG 0x32
#define LOW_TH_CH4_REG      0x33
//CHANNEL 5
#define RECENT_CH5_LSB_REG  0xAA
#define RECENT_CH5_MSB_REG  0xAB

#define HYSTERESIS_CH5_REG  0x34
#define HIGH_TH_CH5_REG     0x35
#define EVENT_COUNT_CH5_REG 0x36
#define LOW_TH_CH5_REG      0x37
//CHANNEL 6
#define RECENT_CH6_LSB_REG  0xAC
#define RECENT_CH6_MSB_REG  0xAD

#define HYSTERESIS_CH6_REG  0x38
#define HIGH_TH_CH6_REG     0x39
#define EVENT_COUNT_CH6_REG 0x3A
#define LOW_TH_CH6_REG      0x3B
//CHANNEL 7
#define RECENT_CH7_LSB_REG  0xAE
#define RECENT_CH7_MSB_REG  0xAF

#define HYSTERESIS_CH7_REG  0x3C
#define HIGH_TH_CH7_REG     0x3D
#define EVENT_COUNT_CH7_REG 0x3E
#define LOW_TH_CH7_REG      0x3F

void begin_I2C(void);

class ADS7128 {
    public:
        bool CRC_ERR_Detected = 0;

        ADS7128(uint8_t i2c_address); // constructeur

        void ADS7128_Clear_EVENT_HIGH_FLAG(uint8_t Channel);
        void ADS7128_Clear_EVENT_LOW_FLAG(uint8_t Channel);

        // Initialisation ADS7128
        void ADS7128_Init_Manual(unsigned char OSR);
        void ADS7128_Init_AutoSequence(unsigned char OSR);
        void ADS7128_Init_Autonomous(unsigned char OSR, bool OSC_SEL, unsigned char CLK_DIV, unsigned char AUTO_SEQ_CH_SEL, unsigned char ALERT_CH_SEL);
        void ADS7128_Init_GPIO(unsigned char Channel_Configuration);

        // Channel Read
        uint16_t ADS7128_ReadChannel_Manual(uint8_t Channel);
        uint16_t ADS72128_Read_CHx_Autonomous(uint8_t Channel);
        uint8_t ADS7128_Read_EVENT_FLAG(void);
        uint8_t ADS7128_Read_EVENT_HIGH_FLAG();
        uint8_t ADS7128_Read_EVENT_LOW_FLAG();

        //Config
        void ADS7128_ALERT_CH_SEL(unsigned char _Config);
        void ADS7128_AUTO_SEQ_CH_SEL(unsigned char _Config);    
        void ADS7128_Configure_TH_CHx(uint8_t Channel, uint16_t HIGH_TH, uint16_t LOW_TH, uint8_t EVENT_COUNT, uint8_t HYSTERESIS);

        // CRC
        void ADS7128_Clear_CRC_ERR_IN(void);
        bool ADS7128_Read_CRC_ERR_IN(void);
        // Other
        void ScanI2C(void);
        float convertADCtoVoltage(uint16_t adcValue, bool is16Bit);
        void Test(void);

    private:
        uint8_t _addr; // adresse I2C du capteur

        //FONCTION WRITE
        bool ADS7128_Write_REG_NoCRC(uint8_t _REG, uint8_t Data);
        void ADS7128_Write_REG(uint8_t _REG, uint8_t Data);
        uint8_t ADS7128_Read_REG(uint8_t _REG);

        //FONCTION READ
        uint8_t calculateCRC8_CCITT(const uint8_t data);

        //REGISTERS CONFIGURATION 
        void ActivateCRC(void);
        void ADS7128_OPMODE_CFG(bool CONV_ON_ERR, unsigned char CONV_MODE, bool OSC_SEL, unsigned char CLK_DIV);
        void ADS7128_SEQUENCE_CFG(bool SEQ_START, bool SEQ_MODE);
  };

  extern ADS7128 ADS;


#endif