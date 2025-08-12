#ifndef XMACHINE_STATE_H
#define XMACHINE_STATE_H

    #define Vitesse_4       25
    #define Vitesse_3       50
    #define Vitesse_2       100
    #define Vitesse_1       150
    
    #define NORMAL          0

    #define ACTION_TIMEOUT_MS        600
    #define INTER_ACTION_DELAY_MS    350
    #define PRE_ACTION_DELAY_MS      50

    #define SENSOR_DEBOUNCE_TIME_MS 100

    #define MOSFET_ON   4096
    #define MOSFET_OFF  0


    #define Mosfet_Rotation_CCW 0
    #define Mosfet_Shear_Open 2
    #define Mosfet_Extension_Up 4
    #define Mosfet_Shear_Close 3
    #define Mosfet_Rotation_Clockwise 1
    #define Mosfet_Ebrancheur_Open 6
    #define Mosfet_Grappin_Open 8
    #define Mosfet_Grappin_Close 9
    #define Mosfet_Ebrancheur_Close 7
    #define Mosfet_Tilt_Retract 10
    #define Mosfet_Extension_Down 5
    #define Mosfet_Tilt 11
    #define Mosfet_LowPressure 12

    struct STATEFUNCTION
    {
        bool Rotation_CCW = 0;
        bool Shear_Open = 0;
        bool Extension_Up = 0;
        bool Shear_Close = 0;
        bool Rotation_Clockwise = 0;
        bool Ebrancheur_Open = 0;
        bool Grappin_Open = 0;
        bool Grappin_Close = 0;
        bool Ebrancheur_Close;
        bool Tilt_Retract = 0;
        bool Extension_Down = 0;
        bool Tilt = 0;
        bool Automatic = 0;
        bool Automatic_Reversed = 0;
        bool Sequence_Auto = 0;
        bool Reset_Time = 0;
        unsigned int Delay = 0;

        bool Trigger_Up = 0;
        bool Trigger_Down = 0;
    };

    extern STATEFUNCTION StateFunction;

    void Init_MachineState(void);
    void Wait_Zero(void);

    void StopMachine(void);
    void SetMachineCoil(void);

    void Execute_Sequence(void);
    void Read_Sensor(void);

    void Sequence_Manual_Up_Stroke_Out(void);
    void Sequence_Manual_Up_Stroke_In(void);
    void Sequence_Manual_Down_Stroke_Out(void);
    void Sequence_Manual_Down_Stroke_In(void);

    void Sequence_Automatic_Up_Stroke_Out(void);
    void Sequence_Automatic_Up_Stroke_In(void);
    void Sequence_Automatic_Down_Stroke_Out(void);
    void Sequence_Automatic_Down_Stroke_In(void);

    void Sequence_RetournToZero(void);
    void Sequence_RetournToCenter(void);
    void Sequence_Adjust_Up(void);

    void Sequence_Adjust_Down(void);
    void Sequence_Abort_Auto(void);




#endif