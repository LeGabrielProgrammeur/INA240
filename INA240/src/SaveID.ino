#include "xMain.h"


/*
###############################################################################
# Function:      InitInterrupts
# Description:   Initializes the interrupt handling mechanism.
###############################################################################
*/
void InitSaveID()
{
   byte FirstBoot = EEPROM.read(EEPROM_ADDR_FirstBoot);
   if(FirstBoot == 0xFF)
   {
      SYSTEM_PRINTLN("It's the first time running this program, let's save the default values.");
      EEPROM.write(EEPROM_ADDR_FirstBoot, 0x00);
      EEPROM.write(EEPROM_ADDR_UniqueID, 0x01);
   }

}