#include "xMain.h"

SENSOR::SENSOR(uint8_t _Ch)
{
    _Channel = _Ch;
}

void SENSOR::SENSOR_Init(void)
{
    ADS.ADS7128_Configure_TH_CHx(_Channel, 0b001111110000, 0b0, 0x01, 0x00);
}
bool SENSOR::SENSOR_Read(void)
{
    uint16_t Lecture = ADS.ADS72128_Read_CHx_Autonomous(_Channel);
    SYSTEM_PRINTLN(Lecture);
    if(Lecture > 300) return 1;
    else return 0;
}
void SENSOR::SENSOR_Clear_flag(void)
{
    ADS.ADS7128_Clear_EVENT_HIGH_FLAG(_Channel);
}