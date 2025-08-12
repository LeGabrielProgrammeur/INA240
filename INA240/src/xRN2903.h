#ifndef XRN2903
#define XRN2903

#define BUFFER_SIZE 512 // Define a reasonable buffer size


#include "xMain.h"

unsigned long Delay_Reception = 0;

bool InitRN2903FSK(String InitState);
bool InitRN2903(String InitState);

void Loop_RN2903_Receiving(void);
void Loop_RN2903_Pairing(void);
void Loop_RN2903_Paused(void);

void flushSerial1Input(void);

String ConvertChannelToFrequency(int HexValue);

void SleepRN2903(void);
void WakeupRN2903(void);

void CHANGE_CHANNEL(int Channel);
void CHANGE_SPREADING_FACTOR(byte SpreadingFactor);
void CHANGE_CODING_RATE(byte CodingRate);

//void SendAliveMessageRN2903(void);
//void SendEMOMessageRN2903(void);
void SendCustomMessageRN2903(byte* Message, int MessageLength, int NumberOfMessagesToSend, bool GoToReceiveModeAfter);

long MessageRF;
unsigned long Delay_timeout;
//bool processPairingMessage(String response);

//void waitForResponseInit(void);
#endif