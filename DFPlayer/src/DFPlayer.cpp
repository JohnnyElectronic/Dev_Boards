/*!
 * @file DFPlayer.h
 * @brief DFPlayer - An MP3 Player for FN-M16P Embedded MP3 Audio Module
 *
 * @version  V1.1
 * @date  2024-10-3
 * - Added additional comments
 */

#include "DFPlayer.h"

byte lastRxCmd = 0;
byte dfpBusyPin = -1;

void dfpReset()
{
  dfpExecuteCmd(DFP_RESET,0,0);
// delay(500); /* Moved to dfpExecuteCmd */
}

void dfpStop()
{
  dfpExecuteCmd(DFP_STOP,0,0);
 // delay(500); /* Moved to dfpExecuteCmd */
}

void dfpPause()
{
  dfpExecuteCmd(DFP_PAUSE,0,0);
 // delay(500); /* Moved to dfpExecuteCmd */
}

void dfpPlay()
{
  dfpExecuteCmd(DFP_PLAY,0,1); 
 // delay(500); /* Moved to dfpExecuteCmd */
}

void dfpPlayNext()
{
  dfpExecuteCmd(DFP_PLAYNEXT,0,1);
  //delay(500); /* Moved to dfpExecuteCmd */
}

void dfpPlayPrevious()
{
  dfpExecuteCmd(DFP_PLAYPREVIOUS,0,1);
  //delay(500); /* Moved to dfpExecuteCmd */
}

void dfpPlayRandom()
{
  dfpExecuteCmd(DFP_RANDOM,0,0);
  //delay(500); /* Moved to dfpExecuteCmd */
}


// Enable or disable repeat play
// Mode 1 is start repeat play and 0 is stop
void dfpPlayRepeat(int mode)
{
  dfpExecuteCmd(DFP_REPEAT_PLAY,0,mode);
  //delay(500); /* Moved to dfpExecuteCmd */
}

// Enable or disable repeat play of a track
// Track must be playing already for repeat to work
// Mode 0 is start repeat play of track and 1 is stop repeat
void dfpRepeatTrack(int mode)
{
  dfpExecuteCmd(DFP_REPEAT_TRACK,0,mode);
  //delay(500); /* Moved to dfpExecuteCmd */
}

void dfpSetVolume(int volume)
{
  dfpExecuteCmd(DFP_SETVOLUME, 0, volume); // Set the volume (0-30)
 // delay(2000); /* Moved to dfpExecuteCmd */
}

void dfpUpVolume()
{
  dfpExecuteCmd(DFP_UPVOLUME, 0, 0); // Increase volume
  //delay(500); /* Moved to dfpExecuteCmd */
}

void dfpDownVolume()
{
  dfpExecuteCmd(DFP_DOWNVOLUME, 0, 0); // Decrease volume
 // delay(500); /* Moved to dfpExecuteCmd */
}

void dfpSetEq(int eq)
{
  dfpExecuteCmd(DFP_SETEQ, 0, eq); // Set the EQ mode (0-5, norm|pop|rock|jazz|classic|base)
 // delay(2000); /* Moved to dfpExecuteCmd */
}

void dfpPlayTrack(int track)
{
  dfpExecuteCmd(DFP_PLAYTRACK, 0, track); // Play the track number (0-2999)
  //delay(2000); /* Moved to dfpExecuteCmd */
}

void dfpPlayTrackMP3(int track)
{
  dfpExecuteCmd(DFP_PLAYTRACK_MP3, 0, track); // Play the track number (0-2999)
  //delay(2000); /* Moved to dfpExecuteCmd */
}

// Query, Get DFP Status
// Sends a DFP_GETSTATUS command to the DFP andreturns the result
// MSB                           LSB
// 0x01 USB flash drive       | 0x00 Stopped
// 0x02 SD card               | 0x01 Playing
// 0x10 Module in sleep mode  | 0x02 Paused
// -1 command failed
int dfpGetStatus()
{
  int result = -1;
  dfpExecuteCmd(DFP_GETSTATUS, 0, 0); 
  result = dfpReadQuery(DFP_GETSTATUS);

  return result;
}

int dfpGetVolume()
{
  int result = -1;
  dfpExecuteCmd(DFP_GETVOLUME, 0, 0);
  result = dfpReadQuery(DFP_GETVOLUME) & 0xFF;

  return result;
}

int dfpGetEq()
{
  int result = -1;
  dfpExecuteCmd(DFP_GETEQ, 0, 0);
  result = dfpReadQuery(DFP_GETEQ) & 0xFF;

  return result;
}

int dfpGetSDTracks()
{
  int result = -1;
  dfpExecuteCmd(DFP_SD_TRACKS, 0, 0);
  result = dfpReadQuery(DFP_SD_TRACKS);

  return result;
}

int dfpGetCurTrack()
{
  int result = -1;
  dfpExecuteCmd(DFP_CUR_TRACK, 0, 0);
  result = dfpReadQuery(DFP_CUR_TRACK);

  return result;
}

int dfpGetSwVer()
{
  int result = -1;
  dfpExecuteCmd(DFP_SW_VER, 0, 0);
  result = dfpReadQuery(0);

  return result;
}

byte dfpLastRxCmd()
{
  return lastRxCmd;
}

// Initial setup for DFPlayer. Add to the setup() section.
// Set busyPin to -1 if not used else greater than zero for use by the dfpBusyWait() procedure, dfpDelay should be at 
// least 1000 or higher if there is a large number of tracks.
// 
// busyPin is configured with input pull ups enabled.
// 
// Resets the device and provides a delay set by dfpDelay.
void dfpSetup(byte busyPin, int dfpDelay)
// Setup the busy pin if used and reset
{
    if (busyPin != -1) {
        dfpBusyPin = busyPin;
        pinMode(dfpBusyPin, INPUT_PULLUP);
    }

    /* Init/Reset DFP */
    dfpReset();
    delay (dfpDelay);
}

// Empties input serial buffer of any unused data
void dfpSerialPurge()
{
#ifdef DFP_USE_SERIAL
    while (Serial.available() > 0) {
        inByte = Serial.read();
    }
#else
    while (dfpSerial.available() > 0) {
        dfpSerial.read();
    }
#endif
}

// Checks and waits while busy line is active.
// Returns 0 if no busy pin set or 1 for busy duration completed
int dfpBusyWait(int waitDelay)
{

#ifdef DFP_DEBUG
  Serial.print(F("Wait delay:"));
  Serial.println(waitDelay);
#endif

    if (dfpBusyPin != -1) {
        delay (waitDelay);  /* Short wait for Busy signal to activate - active low */    
        while (!digitalRead(dfpBusyPin)) {
          delay (waitDelay);
        }
        return 1;
    } else {
        return 0;
    }
}

// Checks if serial port has data.
// Returns 1 for data available else 0.
int dfpSerialAvail()
{
#ifdef DFP_USE_SERIAL
    if (Serial.available()){
        return 1;
    }
#else
    if (dfpSerial.available()){
        return 1;
    }
#endif

    return 0;
}
  
// Checks if serial port has data and wait if not.
// Returns 0 if timeout or 1 for data available
int dfpAvailWait()
{
    unsigned long timer = millis();

#ifdef DFP_USE_SERIAL
    while (!Serial.available()){
      if (millis() - timer > DFP_WAIT_TIMEOUT) {
         return 0;
      }
      delay(10);
    } /* end while loop */
#else
    while (!dfpSerial.available()){
      if (millis() - timer > DFP_WAIT_TIMEOUT) {
#ifdef DFP_DEBUG
  Serial.println(F("AW TIMEOUT"));
#endif
         return 0;
      }
      delay(10);
    } /* end while loop */
#endif

    return 1;
}

// Send a command to the DFP board.
// CMD is one of the available valid commands or querys
// Par1 (MSB) and Par2 (LSB) are the command parameters. 
// A delay of DFP_CMD_DELAY is provided before exiting.
void dfpExecuteCmd(byte CMD, byte Par1, byte Par2)
// Excecute the command and parameters
{
    // Calculate the checksum (2 bytes)
    word checksum = -(DFP_VERSION_BYTE + DFP_COMMAND_LENGTH + CMD + DFP_ACKNOWLEDGE + Par1 + Par2);
    // Build the command line
    byte Command_line[10] = { DFP_START_BYTE, DFP_VERSION_BYTE, DFP_COMMAND_LENGTH, CMD, DFP_ACKNOWLEDGE,
    Par1, Par2, highByte(checksum), lowByte(checksum), DFP_END_BYTE};
    //Send the command line to the module
    for (byte k=0; k<10; k++)
    {
#ifdef DFP_USE_SERIAL
        Serial.write( Command_line[k]);
#else
        dfpSerial.write( Command_line[k]);
#endif
    }

    delay(DFP_CMD_DELAY);
}

// Receive Query results from the DFP board. One result is processed per call.
// CMD is optional and if > 0 will compare with the returned results and validate. If the first result 
// CMD does not match a retry will occur and return that result.
// 
// Returns any parameters for Par1 (MSB) and Par2 (LSB). 
// If a CMD value was provided and it does not match the query cmd or no data was detected a -1 is returned
int dfpReadQuery(byte CMD)
// Read the query parameters
{
    bool dataVal = false;
    bool dataRetry = false;
    int loopIndex = 0;
    int params;
    char inByte;

    dfpAvailWait();

#ifdef DFP_USE_SERIAL
    while ((Serial.available() > 0) && (loopIndex <= 9)) {
      inByte = Serial.read();
      switch (loopIndex) {
          case DFP_DATA_START:
              if ((inByte & 0xFF) == DFP_START_BYTE) {
                  dataVal = true;
              } else {
                  /* Bad start - no match, try again */
                  continue;
              }
              break;
          case DFP_DATA_COMMAND:
              if ((CMD > 0) && (CMD != (inByte & 0xFF))) {
                  dataVal = false;
                  dataRetry = true;  // wrong command data, try again
              }
              break;
          case DFP_DATA_PARAMETER:
              params = (inByte & 0xFF) << 8;
              break;
          case DFP_DATA_PARAMETER+1:
              params = params | (inByte & 0xFF);
              break;
          case DFP_DATA_END:
              if ((inByte & 0xFF) != DFP_END_BYTE) {
                  dataVal = false;
              }

              if (dataRetry) {
                  loopIndex = -1;
              }
              break;
          default:
              break;
      } /* end switch */

      loopIndex++;
    } /* End while loop */

    if (dataVal) {
        return params;
    }
#else
#ifdef DFP_DEBUG
  Serial.print(F("Received: "));
#endif

    while ((dfpSerial.available() > 0) && (loopIndex <= 9)) {
      inByte = dfpSerial.read();

#ifdef DFP_DEBUG
  Serial.print((inByte & 0xFF), HEX);
#endif

      switch (loopIndex) {
          case DFP_DATA_START:
              if ((inByte & 0xFF) == DFP_START_BYTE) {
                  dataVal = true;
#ifdef DFP_DEBUG
  Serial.print(F("(S)"));
#endif
              } else {
                  /* Bad start - no match, try again */
#ifdef DFP_DEBUG
  Serial.println();
  Serial.print(F("Received: "));
#endif
                  continue;
              }
              break;
           case DFP_DATA_COMMAND:
              lastRxCmd = inByte & 0xFF;
              if ((CMD > 0) && (CMD != (inByte & 0xFF))) {
                  dataVal = false;
                  dataRetry = true;  // wrong command data, try again
              }
#ifdef DFP_DEBUG
  Serial.print(F("(C)"));
#endif
              break;
          case DFP_DATA_PARAMETER:
              params = (inByte & 0xFF) << 8;
#ifdef DFP_DEBUG
  Serial.print(F("(P)"));
#endif
              break;
          case DFP_DATA_PARAMETER+1:
              params = params | (inByte & 0xFF);
#ifdef DFP_DEBUG
  Serial.print(F("(P)"));
#endif
              break;
          case DFP_DATA_END:
              if ((inByte & 0xFF) != DFP_END_BYTE) {
                  dataVal = false;
              }

              if (dataRetry) {
                  loopIndex = -1;
              }
#ifdef DFP_DEBUG
  Serial.print(F("(E)"));
#endif
              break;
          default:
              break;
      } /* end switch */

#ifdef DFP_DEBUG
  Serial.print(F(" "));
#endif
      loopIndex++;
    } /* End while loop */

    if (dataVal) {
#ifdef DFP_DEBUG
  Serial.println();
#endif
        return params;
    }

#ifdef DFP_DEBUG
    if (loopIndex) {
        Serial.println(F("BAD_DATA"));
    } else {
        Serial.println(F("NO_DATA"));
    }
#endif

#endif

    return -1;
}

