


#ifndef XMAIN
#define XMAIN

#include <Arduino.h>
#include "xAM_PLC_14M.h"
#include <EEPROM.h>

#include <Arduino_CAN.h>

#include "xWire_Amerifor.h"
#include "xPCA9685_Amerifor.h"
#include "xADS7128.h"

#include "xRN2903.h"

#include "xSaveID.h"
#include "xSensor.h"

#include "xMachineState.h"
// en-tête commun
#define DEBUG    // décommentez pour activer les traces
//#define DEBUG_ADS7128
//#define DEBUG_RN2903

#ifdef DEBUG
  #define SYSTEM_PRINT(msg)     Serial.print(msg)
  #define SYSTEM_PRINTLN(msg)   Serial.println(msg)
#else
  #define SYSTEM_PRINT(msg)
  #define SYSTEM_PRINTLN(msg)
#endif


#define ID1 0x25
#define ID2 0xA0
#define ID3 0x04

#define RF_Channel 0x40 // 0x00 TO 0x63 (ID00 TO ID99)
#define RF_SF 0x07 // SpreadingFactor : 0x07 to 0x12 (Smaller is faster but less range & reliability)
#define RF_CR 0x45 // Coding rate : 0x45 to 0x48 (Smaller is faster but less error correction)

#define EEPROM_ADDR_UniqueID 0
#define EEPROM_ADDR_FirstBoot 10    

byte UniqueID = 0x00;

bool PAIRING_MODE = false;

#define PILOTEI2C1_FREQUENCE_D_HORLOGE_EN_HZ 400000

#define SERVICETASKSERVER_PERIODE_EN_US (0L)
#define SERVICEBASEDETEMPS_FREQUENCE_EN_HZ ((1000000L)/SERVICETASKSERVER_PERIODE_EN_US)

#define SERVICEBASEDETEMPS_NOMBRE_DE_PHASES 5

#define SENSOR_PHASE 0
#define RF_PHASE 1
#define MACHINE_PHASE 2
#define SEQUENCE_PHASE 3
#define ERROR_PHASE 4


#define SERVICEBASEDETEMPS_NOMBRE_DE_PHASES_DANS_LOOP 1

#include "xserviceTaskServer.h"
#include "xserviceBaseDeTemps.h"

#define SENSOR_ON 1
#define SENSOR_OFF 0



//Variable
#define DELAY_ADJUSTEMENT 75

void InitMain(void);

#endif