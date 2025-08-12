#include "xMain.h"

ADS7128 ADS(0x13);

void InitMain(void)
{
  InitSaveID();

  serviceTaskServer_initialise();
  serviceBaseDeTemps_initialise();

  AM_PLC_14M_Init(), delay(100);
  AM_PLC_14M_Init_PCA9685(), delay(100);
  ADS.ADS7128_Init_Autonomous(0b1, 0b0, 0b0000, 0b00001111, 0b00001111), delay(100);
  bool RN_Return = InitRN2903("Receive");
  SYSTEM_PRINTLN(RN_Return);

  serviceBaseDeTemps_execute[RF_PHASE] = Loop_RN2903_Receiving;

  flushSerial1Input(), delay(100);

  pinMode(A0, OUTPUT);
  
  analogWrite(A0, 255);

  
  Init_MachineState();

}
void setup() 
{
  Serial.begin(9600);
  Serial1.begin(57600);
  delay(8000);
  InitMain();

  serviceTaskServer_DemarreLesTachesALaTouteFinDeSetup();
} 

void loop() //Pas touche au main loop si tu veux changer le code voir switch
{
  serviceTaskServer_gestion.execute();
  serviceBaseDeTemps_gereDansLoop();
}
