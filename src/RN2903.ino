/*
###############################################################################
# File:          RN2903.ino
# Author:        Mikael Lavoie-Fortin / Amérifor
# Created:       2025-02-26
# Last Updated:  2025-02-26
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

#include "xMain.h"


#ifdef DEBUG_RN2903
  #define RN_PRINT(msg)     Serial.print(msg)
  #define RN_PRINTLN(msg)   Serial.println(msg)
#else
  #define RN_PRINT(msg)
  #define RN_PRINTLN(msg)
#endif


unsigned int LastMessageMillis = 0;
int MaxTimeoutDelay = 500;

unsigned long WaitConfirmMillis = 0;

bool LostRemote = true;

//bool PairingStarted = false;
unsigned long PairingMillis = 0;

byte NewUniqueIDtoSend;

int Restart_Counter = 0;

#define BUFFER_SIZE 512
char buffer[BUFFER_SIZE];
int head = 0;
int tail = 0;

unsigned long TimePairing= 0;
bool PairingStarted = false;

/*
###############################################################################
# Function:      CHANGE_CHANNEL
# Description:   Changes the Frequency of the RN2903.
# Parameters:    0 to 99 or 0x00 to 0x64
###############################################################################
*/
void CHANGE_CHANNEL(int Channel)
{
  Serial1.print("radio set freq ");
  Serial1.println(ConvertChannelToFrequency(Channel));
  waitForResponseInit();
  RN_PRINT("Frequency changed to : ");
  Serial1.println("radio get freq");
  waitForResponseInit();
}

/*
###############################################################################
# Function:      CHANGE_SPREADING_FACTOR
# Description:   Changes the Spreading factor of the RN2903.
# Parameters:    0x07 TO 0x12
###############################################################################
*/
void CHANGE_SPREADING_FACTOR(byte SpreadingFactor)
{
  Serial1.print("radio set sf sf");
  if(SpreadingFactor == 0x07)
  {
    Serial1.println("7");
  }
  else if(SpreadingFactor == 0x08)
  {
    Serial1.println("8");
  }
  else if(SpreadingFactor == 0x09)
  {
    Serial1.println("9");
  }
  else if(SpreadingFactor == 0x10)
  {
    Serial1.println("10");
  }
  else if(SpreadingFactor == 0x11)
  {
    Serial1.println("11");
  }
  else if(SpreadingFactor == 0x12)
  {
    Serial1.println("12");
  }
  else
  {
    Serial1.println("7");
  }
  waitForResponseInit();
  RN_PRINT("SpreadingFactor changed to : ");
  Serial1.println("radio get sf");
  waitForResponseInit();
}


/*
###############################################################################
# Function:      CHANGE_CODING_RATE
# Description:   Changes the Coding Rate of the RN2903.
# Parameters:    0x45 TO 0x48
###############################################################################
*/
void CHANGE_CODING_RATE(byte CodingRate)
{
  Serial1.print("radio set cr 4/");
  if(CodingRate == 0x45)
  {
    Serial1.println("5");
  }
  else if(CodingRate == 0x46)
  {
    Serial1.println("6");
  }
  else if(CodingRate == 0x47)
  {
    Serial1.println("7");
  }
  else if(CodingRate == 0x48)
  {
    Serial1.println("8");
  }
  else
  {
    Serial1.println("5");
  }
  waitForResponseInit();
  RN_PRINT("CodingRate changed to : ");
  Serial1.println("radio get cr");
  waitForResponseInit();
}

/*
###############################################################################
# Function:      InitRN2903
# Description:   Initializes the RN2903 RF module with required settings.
# Returns:       true if initialization is successful, false otherwise.
# Parameters:    State, "Transmit" or "Receive"
###############################################################################
*/
bool InitRN2903(String InitState)
{
  //delay(1000); //I have no idea why, but it won't call Startup() unless I have this delay here. 
  
  RN_PRINTLN("** Startup Commands Sent **");
  Serial1.println("sys reset");
  String Result = waitForResponseInit();
  RN_PRINTLN(Result);
  if(Result == "ERROR")
  {
    RN_PRINTLN("RF MODULE ERROR!");
    return false;
  }
  Serial1.println("sys get ver");waitForResponseInit();
  Serial1.println("mac pause");waitForResponseInit();
  Serial1.println("radio set wdt 500");waitForResponseInit();
  Serial1.println("radio set bw 500");waitForResponseInit();// Set Bandwidth to 500 kHz for higher data rate
  Serial1.println("radio set pwr 15");waitForResponseInit(); // Sets the Db of the transmitter (no effect on receiver)

  CHANGE_CHANNEL(RF_Channel); // Changes the frequency of the RN2903 to the RF_CHANNEL value.
  CHANGE_SPREADING_FACTOR(RF_SF); // Changes the SpeadingFactor of the RN2903 to the RF_SF value. 
  CHANGE_CODING_RATE(RF_CR); // Changes the coding rate of the rn2903 to the RF_CR value.

  if(InitState == "Receive")
  {
    Serial1.println("radio rx 0");waitForResponseInit();// COMMENT THIS LINE IF SET AS TRANSMITTER
    RN_PRINTLN("RN2903 Ready for Receiving data!"); 
  }
  else
  {
    RN_PRINTLN("RN2903 Ready for Transmitting data!"); 
  }
  return true;
}
bool InitRN2903FSK(String InitState)
{
  //delay(1000); //I have no idea why, but it won't call Startup() unless I have this delay here. 
  
  Serial.println("** Startup Commands Sent **");
  Serial1.println("sys reset");
  String Result = waitForResponseInit();
  Serial.println(Result);
  if(Result == "ERROR")
  {
    Serial.println("RF MODULE ERROR!");
    return false;
  }
  Serial1.println("mac pause"); waitForResponseInit();
  Serial1.println("radio set mod fsk"); waitForResponseInit();       // Set modulation
  //Serial1.println("radio set freq 915000000"); waitForResponseInit(); // Match your region
  CHANGE_CHANNEL(RF_Channel); // Changes the frequency of the RN2903 to the RF_CHANNEL value.
  Serial1.println("radio set pwr 15"); waitForResponseInit();        // TX Power (2–20 dBm)
  Serial1.println("radio set bitrate 50000"); waitForResponseInit(); // Bitrate in bps
  Serial1.println("radio set fdev 25000"); waitForResponseInit();    // Frequency deviation (Hz)
  Serial1.println("radio set prlen 8"); waitForResponseInit();       // Preamble length (bytes)
  Serial1.println("radio set crc on"); waitForResponseInit();        // CRC enabled (recommended)
  Serial1.println("radio set sync 2D"); waitForResponseInit();       // Sync word (must match RX side)
  Serial1.println("radio set bt 0.5"); waitForResponseInit();        // GFSK filter shaping
  Serial1.println("radio set rxbw 125"); waitForResponseInit();      // Receiver bandwidth (kHz)
  Serial1.println("radio set wdt 1000"); waitForResponseInit();      // Watchdog (optional)

  Serial1.println("radio get mod"); waitForResponseInit();
  Serial1.println("radio get bitrate"); waitForResponseInit();
  Serial1.println("radio get fdev"); waitForResponseInit();
  Serial1.println("radio get rxbw"); waitForResponseInit();
  Serial1.println("radio get sync"); waitForResponseInit();

  if(InitState == "Receive")
  {
    Serial1.println("radio rx 0");waitForResponseInit();// COMMENT THIS LINE IF SET AS TRANSMITTER
    Serial.println("RN2903 Ready for Receiving data!"); 
  }
  else
  {
    Serial.println("RN2903 Ready for Transmitting data!"); 
  }
  return true;
}

/*
###############################################################################
# Function:      flushSerial1Input
# Description:   Must be called after wakingup the RN2903 to flush anything left in the buffer.
###############################################################################
*/
void flushSerial1Input(void) {
  while (Serial1.available()) {
      Serial1.read();
      RN_PRINTLN("Flushing");
  }
}


/*
###############################################################################
# Function:      READ_RN2903
# Description:   Reads the RN2903 buffer and handles what it read by calling the appropriate process function
# Parameters:    ProcessNumber, int value that can be used if you want to handle multiple process functions 
###############################################################################
*/

bool READ_RN2903(int ProcessNumber = 0)
{

  
  while (Serial1.available() > 0)
  {
    Delay_timeout = millis();
    char c = Serial1.read();         // Read one character at a time
    buffer[head] = c;                // Store the character in the buffer
    head = (head + 1) % BUFFER_SIZE; // Advance the head pointer in a circular fashion

    // Prevent buffer overflow by overwriting the oldest data
    if (head == tail)
    {
      tail = (tail + 1) % BUFFER_SIZE; // Move the tail pointer forward
    }
  }

  // Process complete messages from the buffer
  while (tail != head)
  {
    char c = buffer[tail];
    tail = (tail + 1) % BUFFER_SIZE;

    static String response = ""; // Temporary response accumulator
    if (c == '\n' || c == '\r')
    {
      response.trim(); // Remove leading/trailing whitespace
      if (response.length() > 0)
      {
        bool Answer = false;
        if(ProcessNumber == 0 or ProcessNumber == 1)
        {
          processMessage(response);
        } 
        else if(ProcessNumber == 2)
        {
          Answer = processPairingMessage(response); // Process the complete message
        }
        response = "";
        return(Answer);
      }
    }
    else
    {
      response += c; // Add character to the response string
    }
  }
  return false;
}

/*
###############################################################################
# Function:      Loop_RN2903_Pairing
# Description:   The loop that will be used once the pair button on the display will be activated
###############################################################################
*/
void Loop_RN2903_Pairing(void)
{
  Error_State = Error_State | 0x04;

  if(PairingStarted == false)
  {
    NewUniqueIDtoSend = EEPROM.read(EEPROM_ADDR_UniqueID);
    PairingStarted = true;
    PairingMillis = millis();
    RN_PRINTLN("Loop_RN2903_Pairing");
    byte PairingMessage[] = {0x11, ID1, ID2, ID3, NewUniqueIDtoSend, RF_Channel, RF_SF, RF_CR, 0xAB, 0xCD};
    
    SendCustomMessageRN2903(PairingMessage, sizeof(PairingMessage), 5, true);
  }
  if((millis() - PairingMillis) < 800) AM_PLC_14M_SetError(false);
  else if((millis() - PairingMillis) < 1000) AM_PLC_14M_SetError(true);
  else if((millis() - PairingMillis) < 1200) AM_PLC_14M_SetError(false);
  else if((millis() - PairingMillis) < 1400) AM_PLC_14M_SetError(true);
  else if((millis() - PairingMillis) < 1600) AM_PLC_14M_SetError(false);
  else if((millis() - PairingMillis) < 1800) AM_PLC_14M_SetError(true);
  else if((millis() - PairingMillis) < 2000) AM_PLC_14M_SetError(false);

  if ((millis() - PairingMillis) <= 2000)
  {
    if(PairingStarted == true)
    {
      bool PairingSuccess = READ_RN2903(2);
      if(PairingSuccess)
      {
        PairingStarted = false;
        //ChangePage(0x03);
        serviceBaseDeTemps_execute[RF_PHASE] = Loop_RN2903_Receiving;
        RN_PRINT("UniqueID sent to transmitter : ");
        RN_PRINTLN(NewUniqueIDtoSend);
        if (NewUniqueIDtoSend == 255)
        {
          NewUniqueIDtoSend = 1;
        }
        else
        {
          NewUniqueIDtoSend = NewUniqueIDtoSend + 1;
        }
        EEPROM.write(EEPROM_ADDR_UniqueID, NewUniqueIDtoSend);
        RN_PRINT("The next UniqueID to send will be : ");
        RN_PRINTLN(NewUniqueIDtoSend);
        UniqueID = 0x00;
        PAIRING_MODE = false;
        CHANGE_CHANNEL(RF_Channel);
        CHANGE_SPREADING_FACTOR(RF_SF);
        CHANGE_CODING_RATE(RF_CR);

        Error_State = Error_State & 0xFB;

        serviceBaseDeTemps_execute[MACHINE_PHASE] = SetMachineCoil;

      }
    }
    else
    {
      RN_PRINTLN("ParingStarted == false");
    }
  }
  else
  {
    RN_PRINTLN("1 second has passed. resend message.");
    PairingStarted = false;
  }
}

/*
###############################################################################
# Function:      Loop_RN2903_WaitConfirm
# Description:   The loop that will be used to wait for the confimr button to be clicked once paired.
###############################################################################
*/
void Loop_RN2903_WaitConfirm(void)
{
  if((millis() - WaitConfirmMillis) >= 3000)
  {
    serviceBaseDeTemps_execute[RF_PHASE] = Loop_RN2903_Paused;
  }
}


/*
###############################################################################
# Function:      Loop_RN2903_Paused
# Description:   The loop that will be used when the machine is set to OFF
###############################################################################
*/
void Loop_RN2903_Paused(void)
{

}


/*
###############################################################################
# Function:      Loop_RN2903_Receiving
# Description:   The loop that will be used to read the RN2903 when in OFF or AUTO Mode
###############################################################################
*/
void Loop_RN2903_Receiving(void)
{
  READ_RN2903(); // Read the RN2903 and use the ProcessMessage(2) function.

  if(AM_PLC_14M_ReadI1() == true)
  {
    if((millis() - TimePairing) < 1000) return;
    TimePairing = millis();

    RN_PRINTLN("PAIRING");
    serviceBaseDeTemps_execute[RF_PHASE] = Loop_RN2903_Pairing;
    serviceBaseDeTemps_execute[MACHINE_PHASE] = StopMachine, StopMachine(); 
    CHANGE_CHANNEL(0x68); // fREQUENCE PAIRING 928 mHZ
    CHANGE_SPREADING_FACTOR(0x07);
    CHANGE_CODING_RATE(0x45);
    return;
  }
  TimePairing = millis();
}


/*
###############################################################################
# Function:      isIDMatching
# Description:   Compares the receivedID with the ID1 ID2 and ID3.
# Returns:       True if ID received is same as ID in the tube.
###############################################################################
*/
bool isIDMatching(const char* receivedID) {
  byte receivedIDBytes[3] = {0}; // To store extracted byte values

  // Convert the received hex string to bytes
  for (int i = 0; i < 3; i++) {
      char hexByte[3] = {receivedID[i * 2], receivedID[i * 2 + 1], '\0'}; // Extract two hex characters
      receivedIDBytes[i] = strtol(hexByte, NULL, 16); // Convert hex string to byte
  }

  // Compare with defined ID
  return (receivedIDBytes[0] == ID1 && receivedIDBytes[1] == ID2 && receivedIDBytes[2] == ID3);
}

/*
###############################################################################
# Function:      isUniqueIDMatching
# Description:   Compares the receivedUniqueID with the UniqueID.
# Returns:       True if UniqueID received is same as UniqueID.
###############################################################################
*/
bool isUniqueIDMatching(const char* receivedUniqueID) {
  byte receivedUniqueIDBytes[1] = {0}; // To store extracted byte values

  // Convert the received hex string to bytes
  for (int i = 0; i < 1; i++) {
      char hexByte[3] = {receivedUniqueID[i * 2], receivedUniqueID[i * 2 + 1], '\0'}; // Extract two hex characters
      receivedUniqueIDBytes[i] = strtol(hexByte, NULL, 16); // Convert hex string to byte
  }

  if(PAIRING_MODE == true) // We must compared the uniqueID received to the one in the EEPROM.
  {
    return (receivedUniqueIDBytes[0] == EEPROM.read(EEPROM_ADDR_UniqueID));
  }
  else if(UniqueID == 0x00)
  {
    RN_PRINT("First controller detected :");
  
    RN_PRINTLN(receivedUniqueIDBytes[0]);
    UniqueID = receivedUniqueIDBytes[0];
    RN_PRINT("New uniqueID Changed : ");
    RN_PRINTLN(UniqueID);
    return true;
  }
  else
  {
    // Compare with defined ID
    return (receivedUniqueIDBytes[0] == UniqueID);
  }
}


/*
###############################################################################
# Function:      isChannelMatching
# Description:   Compares the receivedChannel with RF_Channel.
# Returns:       True if Channel received is same as RF_Channel.
###############################################################################
*/
bool isChannelMatching(const char* receivedChannel) {
  byte receivedChannelBytes[1] = {0}; // To store extracted byte values

  // Convert the received hex string to bytes
  for (int i = 0; i < 1; i++) {
      char hexByte[3] = {receivedChannel[i * 2], receivedChannel[i * 2 + 1], '\0'}; // Extract two hex characters
      receivedChannelBytes[i] = strtol(hexByte, NULL, 16); // Convert hex string to byte
  }
  return (receivedChannelBytes[0] == RF_Channel);
}


/*
###############################################################################
# Function:      waitForResponseInit
# Description:   Waits for a response from the RF module within a timeout period.
# Returns:       String response from the module, or "ERROR" on timeout.
###############################################################################
*/
String waitForResponseInit(void) {
  unsigned long startTime = millis();
  while ((millis() - startTime) < 1000) { // 500ms timeout
    if (Serial1.available()) {
      String response = Serial1.readStringUntil('\n');
      response.trim();  // Remove any trailing whitespace

      if (!response.isEmpty()) {
        if(response != "ok")
        {
          RN_PRINTLN("Init Response: " + response);
        }
        
        if (response == "ok" || response.startsWith("radio_rx")) {

          return response;
        }
        return response;
      }
    }
  }
  return "ERROR"; // Ensure we return something
}


/*
###############################################################################
# Function:      waitForResponseWakeup
# Description:   Waits for a response from the RF module within a timeout period.
# Returns:       String response from the module, or "ERROR" on timeout.
###############################################################################
*/
String waitForResponseWakeup() {
  unsigned long startTime = millis();
  while ((millis() - startTime) < 100) { // 500ms timeout
    if (Serial1.available()) {
      String response = Serial1.readStringUntil('\n');
      response.trim();  // Remove any trailing whitespace

      if (!response.isEmpty()) {
        RN_PRINTLN("Init Response: " + response);
        
        if (response == "ok" || response.startsWith("radio_rx")) {
          return response;
        }
        return response;
      }
    }
  }
  return "ERROR"; // Ensure we return something
}


/*
###############################################################################
# Function:      waitForResponseTX
# Description:   Waits for a response from the RF module within a timeout period.
###############################################################################
*/
void waitForResponseTX() {
  unsigned long startTime = millis();
  while ((millis() - startTime) < 500) { // 2 seconds timeout
    if (Serial1.available()) {
      String response = Serial1.readStringUntil('\n');
      response.trim();  // Remove any trailing whitespace
      RN_PRINTLN("waitForResponseTX Response: " + response);
      if (response == "radio_tx_ok") {
        
        break;
      }
    }
  }
  //Serial1.println("Sys reset");
}


/*
###############################################################################
# Function:      ConvertChannelToFrequency
# Description:   Convert a hexadecimal 0x00 to 0x64 (100 different possibilities) 00 to 99
# Returns:       902000000MHz to 928000000MHz 
###############################################################################
*/
String ConvertChannelToFrequency(int HexValue)
{
  unsigned int ConvertedFrequency = ( (HexValue * 250000) + 902000000);
  return String(ConvertedFrequency);
}

/*
###############################################################################
# Function:      SleepRN2903
# Description:   Puts the RN2903 module into sleep mode.
###############################################################################
*/
void SleepRN2903(void)
{
  while (Serial1.available()) 
  {
    Serial1.read();  // Read and discard any previous response
  }
  Serial1.println("sys sleep 999999");
}

/*
###############################################################################
# Function:      WakeupRN2903
# Description:   Wakes up the RN2903 module from sleep mode.
###############################################################################
*/
void WakeupRN2903(void) {
  RN_PRINT("Waking up RN2903...");

  // Step 1: Send a valid break condition (Hold TX LOW for at least 938µs)
  pinMode(1, OUTPUT);  // TX pin (adjust if needed)
  digitalWrite(1, LOW);
  delayMicroseconds(2000);  // Hold LOW for 2ms to ensure RN2903 detects break
  digitalWrite(1, HIGH);

  // Step 2: Small delay for RN2903 to process the wake-up condition
  delay(5);

  // Step 3: Restart UART at the correct baud rate
  Serial1.begin(57600);
  delay(10);  // Allow some stabilization time

  // Step 4: Send the auto-baud detection byte (0x55)
  Serial1.write(0x55);
  delay(10);  // Give RN2903 time to process auto-baud
  //Serial1.println("sys get vdd");  
  Serial1.println("");
  waitForResponseInit();
  waitForResponseInit();
  RN_PRINTLN("RN2903 AWAKE.");
}


byte nibble(char c)
{
  if (c >= '0' && c <= '9')
    return c - '0';

  if (c >= 'a' && c <= 'f')
    return c - 'a' + 10;

  if (c >= 'A' && c <= 'F')
    return c - 'A' + 10;

  return 0;  // Not a valid hexadecimal character
}

void hexCharacterStringToBytes(byte *byteArray, const char *hexString)
{
  bool oddLength = strlen(hexString) & 1;

  byte currentByte = 0;
  byte byteIndex = 0;

  for (byte charIndex = 0; charIndex < strlen(hexString); charIndex++)
  {
    bool oddCharIndex = charIndex & 1;

    if (oddLength)
    {
      // If the length is odd
      if (oddCharIndex)
      {
        // odd characters go in high nibble
        currentByte = nibble(hexString[charIndex]) << 4;
      }
      else
      {
        // Even characters go into low nibble
        currentByte |= nibble(hexString[charIndex]);
        byteArray[byteIndex++] = currentByte;
        currentByte = 0;
      }
    }
    else
    {
      // If the length is even
      if (!oddCharIndex)
      {
        // Odd characters go into the high nibble
        currentByte = nibble(hexString[charIndex]) << 4;
      }
      else
      {
        // Odd characters go into low nibble
        currentByte |= nibble(hexString[charIndex]);
        byteArray[byteIndex++] = currentByte;
        currentByte = 0;
      }
    }
  }
}


unsigned int calculateCRC16(byte *data, int length) {
  unsigned int crc = 0xFFFF; // Initial value
  for (int i = 0; i < length; i++) {
    crc ^= data[i];
    for (int j = 0; j < 8; j++) {
      if (crc & 0x0001) {
        crc = (crc >> 1) ^ 0xA001; // Polynomial: 0xA001 (CRC-16-IBM)
      } else {
        crc >>= 1;
      }
    }
  }
  return crc;
}

void dumpByteArray(const byte * byteArray, const byte arraySize)
{
  for (int i = 0; i < arraySize; i++)
  {
   // RN_PRINT("0x");
    if (byteArray[i] < 0x10){}
    //  RN_PRINT("");
  //  RN_PRINT(byteArray[i], HEX);
  //  RN_PRINT(", ");
  }
  //RN_PRINTLN();
}


/*
###############################################################################
# Function:      SendCustomMessageRN2903
# Description:   Sends a custom message via the RN2903 module.
# Parameters:    Message - Pointer to byte array containing the message.
#                MessageLength - Length of the message array.
#                NumberOfMessagesToSend - Number of times to send the message.
#                GoToReceiveModeAfter - Whether to set the module to RX mode.
###############################################################################
*/
void SendCustomMessageRN2903(byte* Message, int MessageLength, int NumberOfMessagesToSend, bool GoToReceiveModeAfter)
{
  RN_PRINTLN("Sending CUSTOM MESSAGE");

  // Calculate CRC-16
  unsigned int crc = calculateCRC16(Message, MessageLength);
  byte crcLow = crc & 0xFF;       // Low byte
  byte crcHigh = (crc >> 8) & 0xFF; // High byte

  // Append CRC to the message
  byte messageWithCRC[MessageLength + 2]; // Original message + 2 bytes for CRC
  memcpy(messageWithCRC, Message, MessageLength); // Copy original message
  messageWithCRC[MessageLength] = crcLow;        // Append CRC low byte
  messageWithCRC[MessageLength + 1] = crcHigh;   // Append CRC high byte

  // Start the "radio tx" command
  for (int j = 0; j < NumberOfMessagesToSend; j++) 
  {  
    RN_PRINT("Sending message # ");
    RN_PRINTLN(j);
    Serial1.print("radio tx ");
    
    // Convert and send the message as a hexadecimal string
    for (int i = 0; i < sizeof(messageWithCRC); i++) {
      if (messageWithCRC[i] < 0x10) {
        Serial1.print("0"); // Add leading zero for single hex digits
      }
      Serial1.print(messageWithCRC[i], HEX);
    }

    Serial1.println();  // End the command
    waitForResponseTX(); // Wait for TX confirmation
  }

  RN_PRINTLN("Messages sent.");
  if(GoToReceiveModeAfter == true)
  {
    RN_PRINTLN("Returning to receive mode.");
    Serial1.println("radio rx 0");
    waitForResponseInit();
  }
}


/*
###############################################################################
# Function:      processMessage
# Description:   Processes received message, check if ID & CRC matches then saves the buttons values.
# Parameters:    response - The received response string.
###############################################################################
*/
unsigned long MessagePerSecondMillis = 0;
int MessagePerSecond = 0;
int OldMessagePerSecond = 0;

void processMessage(String response)
{
  static String lastResponse = "";
  // Check if the response contains a received message
  if (response.startsWith("radio_rx"))
  {
    const char *receivedData = response.c_str() + 10; // Skip "radio_rx "

    if(response.length() != 28)
    {
      RN_PRINTLN("Trop Long");
      Serial1.println("radio rx 0");
      waitForResponseInit();
      return;
    }
    char FirstByte[3] = {0};
    strncpy(FirstByte, receivedData + 4, 2);
    byte receivedIDBytes[1] = {0};                        // To store extracted byte values
    char hexByte[3] = {FirstByte[0], FirstByte[1], '\0'}; // Extract two hex characters
    if (ID3 != strtol(hexByte, NULL, 16))
    {
      RN_PRINTLN("Pas le bon ID");
      Serial1.println("radio rx 0");
      waitForResponseInit();
      return;      
    }
    if(response == lastResponse)
    {
      Delay_Reception = millis();
      Serial1.println("radio rx 0");
      waitForResponseInit();
      return;
    }
    lastResponse = response;

    //RN_PRINT("Raw Received Data: ");RN_PRINTLN(receivedData);

    char ReceivedCRC[5] = {0};strncpy(ReceivedCRC, receivedData + 14, 4);

    char MessageWithoutCRC[15] = {0};
    strncpy(MessageWithoutCRC, receivedData, 14);//RN_PRINTLN(MessageWithoutCRC);
    byte byteArray[7] = {0};
    hexCharacterStringToBytes(byteArray, MessageWithoutCRC);
    dumpByteArray(byteArray, 7);

    unsigned int crc = calculateCRC16(byteArray, sizeof(byteArray));
    byte crcLow = crc & 0xFF;         // Low byte
    byte crcHigh = (crc >> 8) & 0xFF; // High byte
    unsigned int calculatedCRCBigEndian = (crcLow << 8) | crcHigh;
    unsigned int receivedCRC = strtol(ReceivedCRC, NULL, 16);

    char ReceivedID[7] = {0};strncpy(ReceivedID, receivedData, 6);
    char ReceivedUniqueID[3] = {0};strncpy(ReceivedUniqueID, receivedData + 6, 2);

    if (calculatedCRCBigEndian == receivedCRC)
    {
      //RN_PRINTLN("CRC IS MATCHING");
      if (isIDMatching(ReceivedID))
      {
        //RN_PRINTLN("ID IS MATCHING");
        if(isUniqueIDMatching(ReceivedUniqueID))
        {
          //RN_PRINT("TimeLeft : "); RN_PRINTLN(100 - (millis() - Delay_timeout));    
          RF_Error_Count = 0;
                
          String Value_String = response.substring(18, 24);
          MessageRF = strtoul(Value_String.c_str(), NULL, 16);

  //        RN_PRINTLN(MessageRF, HEX);
          MessagePerSecond = MessagePerSecond + 1;
          //RN_PRINT("Message per second : ");
          //RN_PRINTLN(OldMessagePerSecond);
          if((millis()- MessagePerSecondMillis) >= 1000)
          {
            OldMessagePerSecond = MessagePerSecond;
            MessagePerSecond = 0;
            MessagePerSecondMillis = millis();
          }
          Delay_Reception = millis();
          Serial1.println("radio rx 0");
          waitForResponseInit();
        }
        else
        {
          RN_PRINT("Unique ID Not MAtching");
          Serial1.println("radio rx 0");
          waitForResponseInit();
        }
      }
      else
      {
        RN_PRINTLN("ID NOT MATCHING");
        Serial1.println("radio rx 0");
        waitForResponseInit();
      }
    }
    else
    {
      RN_PRINTLN("CRC ERROR NOT THE SAME RN2903");
      RN_PRINT("Raw Received Data: ");RN_PRINTLN(receivedData);
      Serial1.println("radio rx 0");
      waitForResponseInit();
    }
  }
  else
  {
    // For any other response, reset RN2903 to receive mode
    Serial1.println("radio rx 0");
    waitForResponseInit();
  }
}

/*
###############################################################################
# Function:      processMessage
# Description:   Processes received message, check if ID & CRC matches then saves the buttons values.
# Parameters:    response - The received response string.
###############################################################################
*/
bool processPairingMessage(String response)
{
  // Check if the response contains a received message
  if (response.startsWith("radio_rx"))
  {
    // Extract the received data (in hex) from the response
    const char *receivedData = response.c_str() + 10; // Skip "radio_rx "

    //RN_PRINT("Raw Received Data: ");RN_PRINTLN(receivedData);

    char ReceivedCRC[5] = {0};strncpy(ReceivedCRC, receivedData + 18, 4);

    char MessageWithoutCRC[21] = {0};strncpy(MessageWithoutCRC, receivedData, 18);
    byte byteArray[9] = {0};
    hexCharacterStringToBytes(byteArray, MessageWithoutCRC);
    dumpByteArray(byteArray, 9);

    unsigned int crc = calculateCRC16(byteArray, sizeof(byteArray));
    
    byte crcLow = crc & 0xFF;         // Low byte
    byte crcHigh = (crc >> 8) & 0xFF; // High byte
    unsigned int calculatedCRCBigEndian = (crcLow << 8) | crcHigh;
    unsigned int receivedCRC = strtol(ReceivedCRC, NULL, 16);

    char PairingCode[3] = {0};strncpy(PairingCode, receivedData, 2);
    char ReceivedID[7] = {0};strncpy(ReceivedID, receivedData + 2, 6);
    char ReceivedChannel[3] = {0};strncpy(ReceivedChannel, receivedData + 8, 2);
    char ReceivedSF[3] = {0};strncpy(ReceivedSF, receivedData + 10, 2);
    char ReceivedCR[3] = {0};strncpy(ReceivedCR, receivedData + 12, 2);
    char BUTTON_State[3] = {0};strncpy(BUTTON_State, receivedData + 14, 2);
    char BUTTON_State2[3] = {0};strncpy(BUTTON_State2, receivedData + 16, 2);

    if (calculatedCRCBigEndian == receivedCRC)
    {
      RN_PRINTLN("CRC IS MATCHING");
      if(isChannelMatching(ReceivedChannel))
      {
        if ( (strcmp(PairingCode, "22") == 0) && (strcmp(BUTTON_State, "CD") == 0) && (strcmp(BUTTON_State2, "EF") == 0))
        {
          Serial1.println("radio rx 0");
          waitForResponseInit();
          return true;
        }
        else
        {
          Serial1.println("radio rx 0");
          waitForResponseInit();
          return false;
        }
      }
      else
      {
        RN_PRINTLN("Channel not the same");
        Serial1.println("radio rx 0");
        waitForResponseInit();
        return false;
      }
    }
    else
    {
      RN_PRINTLN("CRC ERROR NOT THE SAME RN2903");
      RN_PRINT("Raw Received Data: ");RN_PRINTLN(receivedData);
      Serial1.println("radio rx 0");
      waitForResponseInit();
      return false;
    }
  }
  else
  {
    // For any other response, reset RN2903 to receive mode
    Serial1.println("radio rx 0");
    waitForResponseInit();
    return false;
  }
}

