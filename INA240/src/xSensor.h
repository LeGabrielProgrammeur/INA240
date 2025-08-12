#ifndef XSENSOR_H
#define XSENSOR_H

#include <Arduino.h>

class SENSOR {
    public:
        SENSOR(uint8_t _Ch); //Constructeur

        bool SENSOR_State = 0;
        uint8_t SENSOR_Rebounce = 0;

        void SENSOR_Init(void);
        bool SENSOR_Read(void);
        void SENSOR_Clear_flag(void);

    private:
        uint8_t _Channel;
  };

#endif