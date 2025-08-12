#include "xMain.h"

SENSOR Angle_Tilt(0);
SENSOR Extension(1);
SENSOR Shear(2);
SENSOR StrokeOut(3);

unsigned int AMessageRF = 0;
unsigned long Time = 0;
unsigned long TimeAuto = 0;
unsigned long Time_Sensor = 0;

unsigned long Timeout_Sensor = 0;

unsigned long Time_Measure = 0;

STATEFUNCTION StateFunction;

void Set_Mosfet(void)
{
    for(unsigned char i = 0; i < 14; i++)
    {
        if(AValueCh[i] != ValueCh[i])
        {
            
            if(i == 5 || i ==6) 
            {
                Serial.println("");
                Serial.print("State function delay :"); 
                Serial.print(StateFunction.Delay); 
                Serial.println("ms");
                Serial.print("Time passed :"); Serial.print(millis()-Time_Measure); Serial.println("ms");
                Serial.println("");
            }
            Time_Measure = millis();
            AM_PLC_14M_SetMosfet(i + 1, ValueCh[i]), delayMicroseconds(10);
            AValueCh[i] = ValueCh[i];
        }
    }
}

void Init_MachineState(void)
{   
    Set_Mosfet();
    //Initialisation des SENSOR
    Angle_Tilt.SENSOR_Init();
    Extension.SENSOR_Init();
    Shear.SENSOR_Init();
    StrokeOut.SENSOR_Init();
    delay(100);

    Time_Sensor = millis();
    Time = millis();
    TimeAuto = millis();

    serviceBaseDeTemps_execute[SENSOR_PHASE] = Read_Sensor;
    serviceBaseDeTemps_execute[SEQUENCE_PHASE] = Wait_Zero;
}

void Wait_Zero(void)
{
    
    if(MessageRF != 0x0000) 
    {
        return;
    }

    AM_PLC_14M_SetRelaisUrgence(true);
    AM_PLC_14M_Activate_PCA9685_1(true);
    AM_PLC_14M_Activate_PCA9685_2(true);

    Time_Sensor = millis();
    Time = millis();
    TimeAuto = millis();
    serviceBaseDeTemps_execute[ERROR_PHASE] = AM_PLC_14M_Verify_Error;
    serviceBaseDeTemps_execute[SEQUENCE_PHASE] = Execute_Sequence;
    serviceBaseDeTemps_execute[MACHINE_PHASE] = SetMachineCoil;
}

void StopMachine(void)
{
    AM_PLC_14M_SetRelaisUrgence(false);
    AM_PLC_14M_Activate_PCA9685_1(false);
    AM_PLC_14M_Activate_PCA9685_2(false);
}
void SetMachineCoil(void)
{
    Set_Mosfet();

    //SYSTEM_PRINTLN(MessageRF,HEX);
    if(AMessageRF == MessageRF ) return;
    if(StateFunction.Sequence_Auto == 0)
    {
        if(((MessageRF & 0x0001) == 0x0001) && ((MessageRF & 0x0010) != 0x0010))
        {
            StateFunction.Rotation_CCW = 1;
            ValueCh[Mosfet_Rotation_CCW] = MOSFET_ON;
            Set_Mosfet();
            //SYSTEM_PRINT("Rotation_CCW : "), SYSTEM_PRINTLN(StateFunction.Rotation_CCW);
        } else {
            StateFunction.Rotation_CCW = 0;
            ValueCh[Mosfet_Rotation_CCW] = MOSFET_OFF;
            Set_Mosfet();
            //SYSTEM_PRINT("Rotation_CCW : "), SYSTEM_PRINTLN(StateFunction.Rotation_CCW);
        }
    
        if(((MessageRF & 0x0002) == 0x0002) && ((MessageRF & 0x0008) != 0x0008))
        {
            StateFunction.Shear_Open = 1;
            ValueCh[Mosfet_Shear_Open] = MOSFET_ON;
            //SYSTEM_PRINT("Shear Open : "), SYSTEM_PRINTLN(StateFunction.Shear_Open);
        } else {
            StateFunction.Shear_Open = 0;
            ValueCh[Mosfet_Shear_Open] = MOSFET_OFF;
            //SYSTEM_PRINT("Shear Open : "), SYSTEM_PRINTLN(StateFunction.Shear_Open);
        }
    
        if(((MessageRF & 0x0004) == 0x0004) && ((MessageRF & 0x0400) != 0x0400))
        {
            if(Angle_Tilt.SENSOR_State == SENSOR_ON)
            {
                StateFunction.Extension_Up = 0;
            }
            else StateFunction.Extension_Up = 1;
            //SYSTEM_PRINT("Extension Up : "), SYSTEM_PRINTLN(StateFunction.Extension_Up);
        } else {
            StateFunction.Extension_Up = 0;
            //SYSTEM_PRINT("Extension Up : "), SYSTEM_PRINTLN(StateFunction.Extension_Up);
        }    
    
        if(((MessageRF & 0x0008) == 0x0008) && ((MessageRF & 0x0002) != 0x0002))
        {
            StateFunction.Shear_Close = 1;
            ValueCh[Mosfet_Shear_Close] = MOSFET_ON;
            //SYSTEM_PRINT("Shear Close : "), SYSTEM_PRINTLN(StateFunction.Shear_Close);
        } else {
            StateFunction.Shear_Close = 0;
            ValueCh[Mosfet_Shear_Close] = MOSFET_OFF;
            //SYSTEM_PRINT("Shear Close : "), SYSTEM_PRINTLN(StateFunction.Shear_Close);
        }   
    
        if(((MessageRF & 0x0010) == 0x0010) && ((MessageRF & 0x0001) != 0x0001))
        {
            StateFunction.Rotation_Clockwise = 1;
            ValueCh[Mosfet_Rotation_Clockwise] = MOSFET_ON;
            //SYSTEM_PRINT("Rotation Clockwise : "), SYSTEM_PRINTLN(StateFunction.Rotation_Clockwise);
        } else {
            StateFunction.Rotation_Clockwise = 0;
            ValueCh[Mosfet_Rotation_Clockwise] = MOSFET_OFF;
            //SYSTEM_PRINT("Rotation Clockwise : "), SYSTEM_PRINTLN(StateFunction.Rotation_Clockwise);
        }   
    
        if(((MessageRF & 0x0020) == 0x0020) && ((MessageRF & 0x0100) != 0x0100))
        {
            StateFunction.Ebrancheur_Open = 1;
            ValueCh[Mosfet_Ebrancheur_Open] = MOSFET_ON;
            //SYSTEM_PRINT("Ebrancheur Open : "), SYSTEM_PRINTLN(StateFunction.Ebrancheur_Open);
        }
        else {
            StateFunction.Ebrancheur_Open = 0;
            ValueCh[Mosfet_Ebrancheur_Open] = MOSFET_OFF;
            //SYSTEM_PRINT("Ebrancheur Open : "), SYSTEM_PRINTLN(StateFunction.Ebrancheur_Open);
        }   
    
        if(((MessageRF & 0x0040) == 0x0040) && ((MessageRF & 0x0080) != 0x0080))
        {
            StateFunction.Grappin_Open = 1;
            ValueCh[Mosfet_Grappin_Open] = MOSFET_ON;
            //SYSTEM_PRINT("Grappin Open : "), SYSTEM_PRINTLN(StateFunction.Grappin_Open);
        } else {
            StateFunction.Grappin_Open = 0;
            ValueCh[Mosfet_Grappin_Open] = MOSFET_OFF;
            //SYSTEM_PRINT("Grappin Open : "), SYSTEM_PRINTLN(StateFunction.Grappin_Open);
        }   
    
        if(((MessageRF & 0x0080) == 0x0080) && ((MessageRF & 0x0040) != 0x0040))
        {
            StateFunction.Grappin_Close = 1;
            ValueCh[Mosfet_Grappin_Close] = MOSFET_ON;
            //SYSTEM_PRINT("Grappin Close : "), SYSTEM_PRINTLN(StateFunction.Grappin_Close);
        } else {
            StateFunction.Grappin_Close = 0;
            ValueCh[Mosfet_Grappin_Close] = MOSFET_OFF;
            //SYSTEM_PRINT("Grappin Close : "), SYSTEM_PRINTLN(StateFunction.Grappin_Close);
        }   
    
        if(((MessageRF & 0x0100) == 0x0100) && ((MessageRF & 0x0020) != 0x0020))
        {
            StateFunction.Ebrancheur_Close = 1;
            ValueCh[Mosfet_Ebrancheur_Close] = MOSFET_ON;
            //SYSTEM_PRINT("Grappin Close : "), SYSTEM_PRINTLN(StateFunction.Ebrancheur_Close);
        } else {
            StateFunction.Ebrancheur_Close = 0;
            ValueCh[Mosfet_Ebrancheur_Close] = MOSFET_OFF;
            //SYSTEM_PRINT("Grappin Close : "), SYSTEM_PRINTLN(StateFunction.Ebrancheur_Close);
        }   
     
        if(((MessageRF & 0x0200) == 0x0200) && ((MessageRF & 0x0800) != 0x0800))
        {
            if(Extension.SENSOR_State == SENSOR_OFF && Angle_Tilt.SENSOR_State == SENSOR_ON)
            {
                StateFunction.Tilt_Retract = 0;
            }
            else 
            {
                StateFunction.Tilt_Retract = 1;
                ValueCh[Mosfet_Tilt_Retract] = MOSFET_ON;
            }
            //SYSTEM_PRINT("Grappin Close : "), SYSTEM_PRINTLN(StateFunction.Grappin_Close);
        } else {
            StateFunction.Tilt_Retract = 0;
            ValueCh[Mosfet_Tilt_Retract] = MOSFET_OFF;
            //SYSTEM_PRINT("Grappin Close : "), SYSTEM_PRINTLN(StateFunction.Grappin_Close);
        }     
        
        if(((MessageRF & 0x0400) == 0x0400) && ((MessageRF & 0x0004) != 0x0004))
        {
            if(Shear.SENSOR_State == SENSOR_ON)
            {
                StateFunction.Extension_Down = 0;
            }
            else StateFunction.Extension_Down = 1;
            //SYSTEM_PRINT("Extension Down : "), SYSTEM_PRINTLN(StateFunction.Extension_Down);
        } else {
            StateFunction.Extension_Down = 0;
            //SYSTEM_PRINT("Extension Down : "), SYSTEM_PRINTLN(StateFunction.Extension_Down);
        }     
    
        if(((MessageRF & 0x0800) == 0x0800) && ((MessageRF & 0x0200) != 0x0200))
        {
            StateFunction.Tilt = 1;
            ValueCh[Mosfet_Tilt] = MOSFET_ON;
            //SYSTEM_PRINT("Tilt : "), SYSTEM_PRINTLN(StateFunction.Tilt);
        } else {
            StateFunction.Tilt = 0;
            ValueCh[Mosfet_Tilt] = MOSFET_OFF;
            //SYSTEM_PRINT("Tilt : "), SYSTEM_PRINTLN(StateFunction.Tilt);
        }     
    }
    
    if((MessageRF & 0x1000) == 0x1000)
    {
        StateFunction.Automatic = 1;
    } 
    else 
    {
        StateFunction.Automatic = 0;
    }   
    
    if((MessageRF & 0x8000) == 0x8000)
    {
        StateFunction.Automatic_Reversed = 1;
    } 
    else 
    {
        StateFunction.Automatic_Reversed = 0;
    }
    
    if((MessageRF & 0x10000) == 0x10000)
    {
        StateFunction.Sequence_Auto = 1;
    } 
    else 
    {
        StateFunction.Sequence_Auto = 0;
    }

    if((MessageRF & 0x20000) == 0x20000)
    {
        StateFunction.Reset_Time = 1;
    }

    if((MessageRF & 0x40000) == 0x40000) 
    {
        StateFunction.Delay = Vitesse_1;
    }
    else if((MessageRF & 0x80000) == 0x80000) 
    {
        StateFunction.Delay = Vitesse_2;
    }
    else if((MessageRF & 0x100000) == 0x100000)
    {
        StateFunction.Delay = Vitesse_3;
    }
    else if((MessageRF & 0x200000) == 0x200000)
    {
        StateFunction.Delay = Vitesse_4;
    }
    else
    {
        StateFunction.Delay = NORMAL;
    }
    
    AMessageRF = MessageRF;  
}

void Execute_Sequence(void)
{
    if(StateFunction.Extension_Down == 0 && StateFunction.Extension_Up == 0 && StateFunction.Automatic == 0 && ((MessageRF & 0x0002) != 0x0002))
    {
        ValueCh[Mosfet_Shear_Open] = MOSFET_OFF;
    }

    if(StateFunction.Extension_Down == 1)
    {
        Time = millis(), Timeout_Sensor = millis();
        serviceBaseDeTemps_execute[SEQUENCE_PHASE] = Sequence_Manual_Down_Stroke_Out;
        return;
    }

    if(StateFunction.Extension_Up == 1)
    {
        /*if(Shear.SENSOR_State == SENSOR_OFF) 
        {
            Timeout_Sensor = millis();
            ValueCh[Mosfet_Shear_Open] = MOSFET_ON;
            return;
        }
        else ValueCh[Mosfet_Shear_Open] = MOSFET_OFF;

        if((millis() - Timeout_Sensor) < SENSOR_DEBOUNCE_TIME_MS) return;*/

        Time = millis(), Timeout_Sensor = millis();
        serviceBaseDeTemps_execute[SEQUENCE_PHASE] = Sequence_Manual_Up_Stroke_Out;
        return;
    }

    if(StateFunction.Automatic == 1)
    {
        if(Shear.SENSOR_State == SENSOR_OFF) 
        {
            Timeout_Sensor = millis();
            ValueCh[Mosfet_Shear_Open] = MOSFET_ON;
            return;
        }
        else ValueCh[Mosfet_Shear_Open] = MOSFET_OFF;

        if((millis() - Timeout_Sensor) < SENSOR_DEBOUNCE_TIME_MS) return;

        Time = millis(), TimeAuto = millis(), Timeout_Sensor = millis();
        serviceBaseDeTemps_execute[SEQUENCE_PHASE] = Sequence_Automatic_Up_Stroke_Out; 
        return;
    }
}
void Read_Sensor(void)
{
    if((millis() - Time_Sensor) < 10)    return;
    Time_Sensor = millis();

    uint8_t MAX_REBOUNCE = 5;
    delayMicroseconds(10);
    uint8_t EVENT_HIGH_FLAG = ADS.ADS7128_Read_EVENT_HIGH_FLAG();
    delayMicroseconds(10);

    if(EVENT_HIGH_FLAG == 0x00)
    {
        if(Angle_Tilt.SENSOR_Rebounce == 0) Angle_Tilt.SENSOR_State = 0;
        else if(Angle_Tilt.SENSOR_Rebounce > 0) Angle_Tilt.SENSOR_Rebounce--;

        if(Extension.SENSOR_Rebounce == 0) Extension.SENSOR_State = 0;
        else if(Extension.SENSOR_Rebounce > 0) Extension.SENSOR_Rebounce--;

        if(Shear.SENSOR_Rebounce == 0) Shear.SENSOR_State = 0;
        else if(Shear.SENSOR_Rebounce > 0) Shear.SENSOR_Rebounce--;

        if(StrokeOut.SENSOR_Rebounce == 0) StrokeOut.SENSOR_State = 0;
        else if(StrokeOut.SENSOR_Rebounce > 0) StrokeOut.SENSOR_Rebounce--;
        return;
    }

    //SYSTEM_PRINT("Alert: "), SYSTEM_PRINTLN(EVENT_HIGH_FLAG, HEX);

    if((Angle_Tilt.SENSOR_Rebounce < MAX_REBOUNCE) && (EVENT_HIGH_FLAG & 0x01))
    {
        //SYSTEM_PRINTLN(Angle_Tilt.SENSOR_Rebounce);
        Angle_Tilt.SENSOR_Rebounce += 1;
        Angle_Tilt.SENSOR_Clear_flag(), delayMicroseconds(10);
    }
    if((Extension.SENSOR_Rebounce < MAX_REBOUNCE) && (EVENT_HIGH_FLAG & 0x02))
    {
        //SYSTEM_PRINTLN(Extension.SENSOR_Rebounce);
        Extension.SENSOR_Rebounce += 1;
        Extension.SENSOR_Clear_flag(), delayMicroseconds(10);
    }
    if((Shear.SENSOR_Rebounce < MAX_REBOUNCE) && (EVENT_HIGH_FLAG & 0x04))
    {
        //SYSTEM_PRINTLN(Shear.SENSOR_Rebounce);
        Shear.SENSOR_Rebounce += 1;
        Shear.SENSOR_Clear_flag(), delayMicroseconds(10);
    }
    if((StrokeOut.SENSOR_Rebounce < MAX_REBOUNCE) && (EVENT_HIGH_FLAG & 0x08))
    {
        //SYSTEM_PRINTLN(StrokeOut.SENSOR_Rebounce);
        StrokeOut.SENSOR_Rebounce += 1;
        StrokeOut.SENSOR_Clear_flag(), delayMicroseconds(10);
    }


    if(Angle_Tilt.SENSOR_Rebounce == MAX_REBOUNCE)
    {
        Angle_Tilt.SENSOR_State = 1;
        Angle_Tilt.SENSOR_Clear_flag(), delayMicroseconds(10);
        //SYSTEM_PRINTLN("ANGLE TILT BIPPPPPPPPPP");
    }
    if(Extension.SENSOR_Rebounce == MAX_REBOUNCE)
    {
        Extension.SENSOR_State = 1;
        Extension.SENSOR_Clear_flag(), delayMicroseconds(10);
        //SYSTEM_PRINTLN("EXTENSION BIPPPPPPPPPP");
    }
    if(Shear.SENSOR_Rebounce == MAX_REBOUNCE)
    {
        Shear.SENSOR_State = 1;
        Shear.SENSOR_Clear_flag(), delayMicroseconds(10);
        //SYSTEM_PRINTLN("SHEAR BIPPPPPPPPPP");
    }  
    if(StrokeOut.SENSOR_Rebounce == MAX_REBOUNCE)
    {
        StrokeOut.SENSOR_State = 1;
        StrokeOut.SENSOR_Clear_flag(), delayMicroseconds(10);
        //SYSTEM_PRINTLN("StrokeOut BIPPPPPPPPPP");
    }  
}

void Sequence_Manual_Up_Stroke_Out(void)
{
    
    if(StateFunction.Extension_Up == 0) 
    {
        ValueCh[Mosfet_Grappin_Close] = MOSFET_OFF;
        ValueCh[Mosfet_LowPressure] = MOSFET_OFF;
        ValueCh[Mosfet_Extension_Up] = MOSFET_OFF;

        Time = millis();

        serviceBaseDeTemps_execute[SEQUENCE_PHASE] = Execute_Sequence;
        return;
    }

    if(StrokeOut.SENSOR_State == SENSOR_ON)
    {
        ValueCh[Mosfet_Grappin_Close] = MOSFET_OFF;
        ValueCh[Mosfet_LowPressure] = MOSFET_OFF;
        ValueCh[Mosfet_Extension_Up] = MOSFET_OFF;

        if((millis() - Timeout_Sensor) < SENSOR_DEBOUNCE_TIME_MS) return;

        Time = millis();

        serviceBaseDeTemps_execute[SEQUENCE_PHASE] = Sequence_Manual_Up_Stroke_In;
        return;
    }
    Timeout_Sensor = millis();

    if((millis() - Time) < PRE_ACTION_DELAY_MS) return;
    if((millis() - Time) < (PRE_ACTION_DELAY_MS + INTER_ACTION_DELAY_MS))
    {
        ValueCh[Mosfet_Grappin_Close] = MOSFET_ON;
        return;
    }
    else if((millis() - Time) < (PRE_ACTION_DELAY_MS + (2*INTER_ACTION_DELAY_MS)))
    {
        ValueCh[Mosfet_Grappin_Close] = MOSFET_ON;
        ValueCh[Mosfet_LowPressure] = MOSFET_ON;
        return;
    }
    else if((millis() - Time) > (PRE_ACTION_DELAY_MS + (2*INTER_ACTION_DELAY_MS)))
    {
        ValueCh[Mosfet_Grappin_Close] = MOSFET_ON;
        ValueCh[Mosfet_LowPressure] = MOSFET_ON;
        ValueCh[Mosfet_Extension_Up] = MOSFET_ON;
        return;
    }
}
void Sequence_Manual_Up_Stroke_In(void)
{
    
    if(StateFunction.Extension_Up == 0) 
    {
        ValueCh[Mosfet_Ebrancheur_Close] = MOSFET_OFF;
        ValueCh[Mosfet_Grappin_Open] = MOSFET_OFF;
        ValueCh[Mosfet_Extension_Down] = MOSFET_OFF;

        Time = millis();

        serviceBaseDeTemps_execute[SEQUENCE_PHASE] = Execute_Sequence;
        return;
    }

    if(Extension.SENSOR_State == SENSOR_ON)
    {
        ValueCh[Mosfet_Ebrancheur_Close] = MOSFET_OFF;
        ValueCh[Mosfet_Grappin_Open] = MOSFET_OFF;
        ValueCh[Mosfet_Extension_Down] = MOSFET_OFF;

        if((millis() - Timeout_Sensor) < SENSOR_DEBOUNCE_TIME_MS) return;

        Time = millis();

        serviceBaseDeTemps_execute[SEQUENCE_PHASE] = Sequence_Manual_Up_Stroke_Out;
        return;
    }
    Timeout_Sensor = millis();

    if((millis() - Time) < PRE_ACTION_DELAY_MS) return;
    if((millis() - Time) < (PRE_ACTION_DELAY_MS + INTER_ACTION_DELAY_MS))
    {
        ValueCh[Mosfet_Ebrancheur_Close] = MOSFET_ON;
        return;
    }
    else if((millis() - Time) < (PRE_ACTION_DELAY_MS + (2*INTER_ACTION_DELAY_MS)))
    {
        ValueCh[Mosfet_Ebrancheur_Close] = MOSFET_ON;
        ValueCh[Mosfet_Grappin_Open] = MOSFET_ON;
        return;
    }
    else if((millis() - Time) > (PRE_ACTION_DELAY_MS + (2*INTER_ACTION_DELAY_MS)))
    {
        ValueCh[Mosfet_Ebrancheur_Close] = MOSFET_ON;
        ValueCh[Mosfet_Grappin_Open] = MOSFET_OFF;
        ValueCh[Mosfet_Extension_Down] = MOSFET_ON;
        return;
    }
}
void Sequence_Manual_Down_Stroke_Out(void)
{
    
    if(StateFunction.Extension_Down == 0) 
    {
        ValueCh[Mosfet_Ebrancheur_Close] = MOSFET_OFF;
        ValueCh[Mosfet_Grappin_Open] = MOSFET_OFF;
        ValueCh[Mosfet_Extension_Up] = MOSFET_OFF;

        Time = millis();

        serviceBaseDeTemps_execute[SEQUENCE_PHASE] = Execute_Sequence;
        return;
    }
    
    if(StrokeOut.SENSOR_State == SENSOR_ON)
    {
        ValueCh[Mosfet_Ebrancheur_Close] = MOSFET_OFF;
        ValueCh[Mosfet_Grappin_Open] = MOSFET_OFF;
        ValueCh[Mosfet_Extension_Up] = MOSFET_OFF;

        if((millis() - Timeout_Sensor) < SENSOR_DEBOUNCE_TIME_MS) return;
        
        Time = millis();

        serviceBaseDeTemps_execute[SEQUENCE_PHASE] = Sequence_Manual_Down_Stroke_In;
        return;
    }
    Timeout_Sensor = millis();

    if((millis() - Time) < PRE_ACTION_DELAY_MS) return;
    if((millis() - Time) < (PRE_ACTION_DELAY_MS + INTER_ACTION_DELAY_MS))
    {
        ValueCh[Mosfet_Ebrancheur_Close] = MOSFET_ON;
        return;
    }
    else if((millis() - Time) < (PRE_ACTION_DELAY_MS + (2*INTER_ACTION_DELAY_MS)))
    {
        ValueCh[Mosfet_Ebrancheur_Close] = MOSFET_ON;
        ValueCh[Mosfet_Grappin_Open] = MOSFET_ON;
    }
    else if((millis() - Time) > (PRE_ACTION_DELAY_MS + (2*INTER_ACTION_DELAY_MS)))
    {
        ValueCh[Mosfet_Ebrancheur_Close] = MOSFET_ON;
        ValueCh[Mosfet_Grappin_Open] = MOSFET_OFF;
        ValueCh[Mosfet_Extension_Up] = MOSFET_ON;
        return;
    } 
}
void Sequence_Manual_Down_Stroke_In(void)
{
    if(StateFunction.Extension_Down == 0) 
    {
        ValueCh[Mosfet_Grappin_Close] = MOSFET_OFF;
        ValueCh[Mosfet_LowPressure] = MOSFET_OFF;
        ValueCh[Mosfet_Extension_Down] = MOSFET_OFF;

        Time = millis();

        serviceBaseDeTemps_execute[SEQUENCE_PHASE] = Execute_Sequence;
        return;
    }
    
    if(Extension.SENSOR_State == SENSOR_ON)
    {
        ValueCh[Mosfet_Grappin_Close] = MOSFET_OFF;
        ValueCh[Mosfet_LowPressure] = MOSFET_OFF;
        ValueCh[Mosfet_Extension_Down] = MOSFET_OFF;

        if((millis() - Timeout_Sensor) < SENSOR_DEBOUNCE_TIME_MS) return;

        Time = millis();

        serviceBaseDeTemps_execute[SEQUENCE_PHASE] = Sequence_Manual_Down_Stroke_Out;
        return;
    }  
    Timeout_Sensor = millis();

    if((millis() - Time) < PRE_ACTION_DELAY_MS) return;
    if((millis() - Time) < (PRE_ACTION_DELAY_MS + INTER_ACTION_DELAY_MS))
    {
        ValueCh[Mosfet_Grappin_Close] = MOSFET_ON;
        return;
    }
    else if((millis() - Time) < (PRE_ACTION_DELAY_MS + (2*INTER_ACTION_DELAY_MS)))
    {
        ValueCh[Mosfet_Grappin_Close] = MOSFET_ON;
        ValueCh[Mosfet_Ebrancheur_Open] = MOSFET_ON;
    }
    else if((millis() - Time) > (PRE_ACTION_DELAY_MS + (2*INTER_ACTION_DELAY_MS)))
    {
        ValueCh[Mosfet_Grappin_Close] = MOSFET_ON;
        ValueCh[Mosfet_Ebrancheur_Open] = MOSFET_OFF;
        ValueCh[Mosfet_Extension_Down] = MOSFET_ON;
        return;
    }   
}

void Sequence_Automatic_Up_Stroke_Out(void)
{
    if(StateFunction.Sequence_Auto == 0) 
    {
        Time = millis(), Sequence_Abort_Auto();

        serviceBaseDeTemps_execute[SEQUENCE_PHASE] = Execute_Sequence;
        return;
    }

    if(StateFunction.Automatic_Reversed == 1)
    {
        ValueCh[Mosfet_Grappin_Close] = MOSFET_ON;
        ValueCh[Mosfet_LowPressure] = MOSFET_OFF;
        ValueCh[Mosfet_Extension_Up] = MOSFET_OFF;

        Time = millis();

        serviceBaseDeTemps_execute[SEQUENCE_PHASE] = Sequence_ReturnToCenter;
        return;        
    }

    if(StrokeOut.SENSOR_State == SENSOR_ON)
    {
        ValueCh[Mosfet_Grappin_Close] = MOSFET_ON;
        ValueCh[Mosfet_LowPressure] = MOSFET_OFF;
        ValueCh[Mosfet_Extension_Up] = MOSFET_OFF;
        
        if((millis() - Timeout_Sensor) < SENSOR_DEBOUNCE_TIME_MS) return;

        Time = millis();
        
        serviceBaseDeTemps_execute[SEQUENCE_PHASE] = Sequence_Automatic_Up_Stroke_In;
        return;
    }
    Timeout_Sensor = millis();

    if((millis() - Time) < PRE_ACTION_DELAY_MS) return;
    else if((millis() - Time) < (PRE_ACTION_DELAY_MS + INTER_ACTION_DELAY_MS))
    {
        ValueCh[Mosfet_Grappin_Close] = MOSFET_ON;
        ValueCh[Mosfet_Extension_Up] = MOSFET_OFF;
        return;
    }
    else if((millis() - Time) < (PRE_ACTION_DELAY_MS + (2*INTER_ACTION_DELAY_MS)))
    {
        ValueCh[Mosfet_Grappin_Close] = MOSFET_ON;
        ValueCh[Mosfet_LowPressure] = MOSFET_ON;
        ValueCh[Mosfet_Ebrancheur_Close] = MOSFET_OFF;
        return;
    }
    else if((millis() - Time) > (PRE_ACTION_DELAY_MS + (2*INTER_ACTION_DELAY_MS)))
    {
        ValueCh[Mosfet_Grappin_Close] = MOSFET_ON;
        ValueCh[Mosfet_LowPressure] = MOSFET_ON;
        ValueCh[Mosfet_Extension_Up] = MOSFET_ON;
        return;
    }
}
void Sequence_Automatic_Up_Stroke_In(void)
{
    if(StateFunction.Sequence_Auto == 0) 
    {
        Time = millis(), Sequence_Abort_Auto();

        serviceBaseDeTemps_execute[SEQUENCE_PHASE] = Execute_Sequence;
        return;
    }

    if(StateFunction.Automatic_Reversed == 1)
    {
        ValueCh[Mosfet_Ebrancheur_Close] = MOSFET_ON;
        ValueCh[Mosfet_Grappin_Open] = MOSFET_OFF;
        ValueCh[Mosfet_Extension_Down] = MOSFET_OFF;

        Time = millis();

        serviceBaseDeTemps_execute[SEQUENCE_PHASE] = Sequence_ReturnToCenter;
        return;        
    }

    if(Extension.SENSOR_State == SENSOR_ON)
    {
        ValueCh[Mosfet_Ebrancheur_Close] = MOSFET_ON;
        ValueCh[Mosfet_Grappin_Open] = MOSFET_OFF;
        ValueCh[Mosfet_Extension_Down] = MOSFET_OFF;

        if((millis() - Timeout_Sensor) < SENSOR_DEBOUNCE_TIME_MS) return;

        Time = millis();

        serviceBaseDeTemps_execute[SEQUENCE_PHASE] = Sequence_Automatic_Up_Stroke_Out;
        return;
    }
    Timeout_Sensor = millis();

    if((millis() - Time) < PRE_ACTION_DELAY_MS) return;
    else if((millis() - Time) < (PRE_ACTION_DELAY_MS + INTER_ACTION_DELAY_MS))
    {
        ValueCh[Mosfet_Ebrancheur_Close] = MOSFET_ON;
        ValueCh[Mosfet_Extension_Down] = MOSFET_OFF;
        return;
    }
    else if((millis() - Time) < (PRE_ACTION_DELAY_MS + (2*INTER_ACTION_DELAY_MS)))
    {
        ValueCh[Mosfet_Ebrancheur_Close] = MOSFET_ON;
        ValueCh[Mosfet_Grappin_Close] = MOSFET_OFF;
        ValueCh[Mosfet_Grappin_Open] = MOSFET_ON;
        return;
    }
    else if((millis() - Time) > (PRE_ACTION_DELAY_MS + (2*INTER_ACTION_DELAY_MS)))
    {
        ValueCh[Mosfet_Ebrancheur_Close] = MOSFET_ON;
        ValueCh[Mosfet_Grappin_Open] = MOSFET_OFF;
        ValueCh[Mosfet_Extension_Down] = MOSFET_ON;
        return;
    }
}

void Sequence_ReturnToCenter(void)
{
    if(StateFunction.Sequence_Auto == 0) 
    {
        Time = millis(), Sequence_Abort_Auto();

        serviceBaseDeTemps_execute[SEQUENCE_PHASE] = Execute_Sequence;
        return;
    }

    if((millis() - Time) < PRE_ACTION_DELAY_MS) return;
    else if((millis() - Time) < (PRE_ACTION_DELAY_MS + INTER_ACTION_DELAY_MS)) 
    {
        ValueCh[Mosfet_Ebrancheur_Open] = MOSFET_OFF;
        ValueCh[Mosfet_Ebrancheur_Close] = MOSFET_ON;
        return;
    }
    else if((millis() - Time) < (PRE_ACTION_DELAY_MS + (2*INTER_ACTION_DELAY_MS)))
    {
        ValueCh[Mosfet_Grappin_Close] = MOSFET_OFF;
        ValueCh[Mosfet_Grappin_Open] = MOSFET_ON;
        return;
    }
    else if((StateFunction.Automatic_Reversed == 1) && (StateFunction.Reset_Time == 1)) 
    {
        StateFunction.Reset_Time = 0;
        ValueCh[Mosfet_Grappin_Open] = MOSFET_OFF;
        serviceBaseDeTemps_execute[SEQUENCE_PHASE] = Sequence_Adjust_Up;
        return;
    }
    else if((StateFunction.Automatic_Reversed == 0) && (StateFunction.Reset_Time == 1))
    {
        StateFunction.Reset_Time = 0;
        ValueCh[Mosfet_Grappin_Open] = MOSFET_OFF;
        serviceBaseDeTemps_execute[SEQUENCE_PHASE] = Sequence_Adjust_Down;
        return;
    }
}

void Sequence_Max_Reached(void)
{
    if(StateFunction.Sequence_Auto == 0) 
    {
        Time = millis(), Sequence_Abort_Auto();

        serviceBaseDeTemps_execute[SEQUENCE_PHASE] = Execute_Sequence;
        return;
    }

    if((millis() - Time) < PRE_ACTION_DELAY_MS)  return;
    else if((millis() - Time) < (PRE_ACTION_DELAY_MS + INTER_ACTION_DELAY_MS))
    {
        ValueCh[Mosfet_Grappin_Open] = MOSFET_OFF;
        ValueCh[Mosfet_Grappin_Close] = MOSFET_ON;
        return;
    }
    else if((millis() - Time) < (PRE_ACTION_DELAY_MS + (2*INTER_ACTION_DELAY_MS)))
    {
        ValueCh[Mosfet_Ebrancheur_Close] = MOSFET_OFF;
        ValueCh[Mosfet_Ebrancheur_Open] = MOSFET_ON;
        return;
    }
    else if((millis() - Time) < (PRE_ACTION_DELAY_MS + (2*INTER_ACTION_DELAY_MS)) + ACTION_TIMEOUT_MS)
    {
        ValueCh[Mosfet_Ebrancheur_Open] = MOSFET_OFF;
        ValueCh[Mosfet_Extension_Up] = MOSFET_OFF;
        ValueCh[Mosfet_Extension_Down] = MOSFET_ON;
        return;
    }
    else if((millis() - Time) > (PRE_ACTION_DELAY_MS + (2*INTER_ACTION_DELAY_MS)) + ACTION_TIMEOUT_MS)
    {
        ValueCh[Mosfet_Extension_Down] = MOSFET_OFF;
        Time = millis();
        serviceBaseDeTemps_execute[SEQUENCE_PHASE] = Sequence_ReturnToCenter;
        return;
    }
}
void Sequence_Min_Reached(void)
{
    if(StateFunction.Sequence_Auto == 0) 
    {
        Time = millis(), Sequence_Abort_Auto();

        serviceBaseDeTemps_execute[SEQUENCE_PHASE] = Execute_Sequence;
        return;
    }

    if((millis() - Time) < PRE_ACTION_DELAY_MS)  return;
    else if((millis() - Time) < (PRE_ACTION_DELAY_MS + INTER_ACTION_DELAY_MS))
    {
        ValueCh[Mosfet_Grappin_Open] = MOSFET_OFF;
        ValueCh[Mosfet_Grappin_Close] = MOSFET_ON;
        return;
    }
    else if((millis() - Time) < (PRE_ACTION_DELAY_MS + (2*INTER_ACTION_DELAY_MS)))
    {
        ValueCh[Mosfet_Ebrancheur_Close] = MOSFET_OFF;
        ValueCh[Mosfet_Ebrancheur_Open] = MOSFET_ON;
        return;
    }
    else if((millis() - Time) < (PRE_ACTION_DELAY_MS + (2*INTER_ACTION_DELAY_MS) + ACTION_TIMEOUT_MS))
    {
        ValueCh[Mosfet_Ebrancheur_Open] = MOSFET_OFF;
        ValueCh[Mosfet_Extension_Down] = MOSFET_OFF;
        ValueCh[Mosfet_Extension_Up] = MOSFET_ON;
        return;
    }
    else if((millis() - Time) > (PRE_ACTION_DELAY_MS + (2*INTER_ACTION_DELAY_MS) + ACTION_TIMEOUT_MS))
    {
        ValueCh[Mosfet_Extension_Up] = MOSFET_OFF;
        Time = millis();
        serviceBaseDeTemps_execute[SEQUENCE_PHASE] = Sequence_ReturnToCenter;
        return;
    }
    
    
}

void Sequence_Adjust_Up(void)
{
    if(StateFunction.Sequence_Auto == 0) 
    {
        Time = millis(), Sequence_Abort_Auto();

        serviceBaseDeTemps_execute[SEQUENCE_PHASE] = Execute_Sequence;
        return;
    }
    
    if(StrokeOut.SENSOR_State == SENSOR_ON)
    {
        ValueCh[Mosfet_Extension_Up] = MOSFET_OFF;

        if((millis() - Timeout_Sensor) < SENSOR_DEBOUNCE_TIME_MS) return;

        Time = millis();
        serviceBaseDeTemps_execute[SEQUENCE_PHASE] = Sequence_Max_Reached;
        return;
    }
    Timeout_Sensor = millis();

    if(StateFunction.Automatic_Reversed == 0)
    {
        ValueCh[Mosfet_Extension_Up] = MOSFET_OFF;

        serviceBaseDeTemps_execute[SEQUENCE_PHASE] = Sequence_Adjust_Down;
        return;
    }

    if(StateFunction.Reset_Time == 1)   Time = millis(), StateFunction.Reset_Time = 0;
    

    if(((millis() - Time) < StateFunction.Delay))
    {
        ValueCh[Mosfet_Extension_Up] = MOSFET_ON;
        ValueCh[Mosfet_Extension_Down] = MOSFET_OFF;
        return;
    }
    else
    {
        ValueCh[Mosfet_Extension_Up] = MOSFET_OFF;
        ValueCh[Mosfet_Extension_Down] = MOSFET_OFF;
    }
}
void Sequence_Adjust_Down(void)
{
    if(StateFunction.Sequence_Auto == 0) 
    {
        Time = millis(), Sequence_Abort_Auto();

        serviceBaseDeTemps_execute[SEQUENCE_PHASE] = Execute_Sequence;
        return;
    }
    if(Extension.SENSOR_State == SENSOR_ON)
    {
        ValueCh[Mosfet_Extension_Down] = MOSFET_OFF;

        if((millis() - Timeout_Sensor) < SENSOR_DEBOUNCE_TIME_MS) return;

        Time = millis();
        serviceBaseDeTemps_execute[SEQUENCE_PHASE] = Sequence_Min_Reached;
        return;
    }
    Timeout_Sensor = millis();

    if(StateFunction.Automatic_Reversed == 1)
    {
        ValueCh[Mosfet_Extension_Down] = MOSFET_OFF;

        serviceBaseDeTemps_execute[SEQUENCE_PHASE] = Sequence_Adjust_Up;
        return;
    }

    if(StateFunction.Reset_Time == 1)   Time = millis(), StateFunction.Reset_Time = 0;
    
    if(((millis() - Time) < StateFunction.Delay))
    {
        ValueCh[Mosfet_Extension_Up] = MOSFET_OFF;
        ValueCh[Mosfet_Extension_Down] = MOSFET_ON;
        return;
    }
    else
    {
        ValueCh[Mosfet_Extension_Up] = MOSFET_OFF;
        ValueCh[Mosfet_Extension_Down] = MOSFET_OFF;
    }
}
void Sequence_Abort_Auto(void)
{
    ValueCh[Mosfet_Grappin_Close] = MOSFET_OFF;
    ValueCh[Mosfet_Grappin_Open] = MOSFET_OFF;

    ValueCh[Mosfet_Ebrancheur_Open] = MOSFET_OFF;
    ValueCh[Mosfet_Ebrancheur_Close] = MOSFET_OFF;
    ValueCh[Mosfet_LowPressure] = MOSFET_OFF;

    ValueCh[Mosfet_Extension_Up] = MOSFET_OFF;
    ValueCh[Mosfet_Extension_Down] = MOSFET_OFF;

    SYSTEM_PRINTLN("Abort!");
    if((millis() - Time) > 100)
    {
        Time = millis();
        serviceBaseDeTemps_execute[SEQUENCE_PHASE] = Execute_Sequence;
        return;
    }
}