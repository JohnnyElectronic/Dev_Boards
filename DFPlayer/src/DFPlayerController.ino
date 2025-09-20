#include "SoftwareSerial.h"
#include <DFPlayer.h>

// Uses a ATTINY85 to control a DF Player module
// Uses SoftSerail to communicate with the DFPLayer and also has the option
// to use the Serial Monitor for debug.

// ATtiny85 Pinouts
// ---------------------------------------------------------
// ATTny85: AVR_ATtiny85
//                          _____
// (Reset)  (A0) PB5  5   1|*    |8  VCC
// (Xtal)   (A3) PB3  3   2|     |7  2   PB2 (A1)(INT0)(SCK)
// (Xtal)   (A2) PB4  4   3|     |6  1~  PB1     (MISO)
//                   GND  4|_____|5  0~  PB0     (MOSI)
// 
// ---------------------------------------------------------
// ---------------------------------------------------------
// Pin Assignments:
//   DFPRx          - D0, Pin 5, serial RX, Can be connected to DFP Tx or Busy pin   
//   DFPTx          - D4, Pin 3, serial TX, Can be connected to DFP Rx
//
//   Reserved INT0  - D2, pin 7, Reserved for INT0 or other use.
//   Reserved PWM   - D1, pin 6, Reserved for PWM or other use.
//   Reserved A3    - D3, pin 2, Reserved for Analog or other use.

#define LED_PIN     1       // Output pin (6) used for LED (Blink and Busy state) 
 
//#define DFPLAYER_DEBUG    // Enable to use Serial Monitor for debug. Will use SerRx/Tx pins.
#define USE_DFP_BUSY        // When enabled will map DFPTx for Busy line instead of DFP Tx. (Active Low)

#define DFP_AUDIO_LEVEL 25  // Used to set intial volume level in setup
#define MAX_TRACKS  6       // Max expected tracks when DFPLAYER_DEBUG is enabled

#ifdef DFPLAYER_DEBUG
    const int SerRx = 2;    // Serial RX, this is physical pin 7, connects to Serial Mon TX
    const int SerTx = 1;    // Serial TX, this is physical pin 6, connects to Serial Mon RX
    SoftwareSerial mySerial(SerRx, SerTx); // RX, TX
#endif 

int query;                        // variable to store status queries
int track;                        // variable to track current mp3 track
int maxTracks = MAX_TRACKS;       // variable to track total mp3 tracks
const int blinkLEDPin = LED_PIN;
unsigned long timer;              // Used when issues with Busy pin to break while loop

const int DFPTx = 4;              // Serial TX, this is physical pin 3, connects to DFP RX
#ifdef USE_DFP_BUSY
    SoftwareSerial dfpSerial (-1, DFPTx);    // RX, TX, Rx is not used, set as -1.
    const int DFPBusy = 0;                   // variable for using DFP busy pin, Active Low
#else
    const int DFPRx = 0;   // Serial RX, this is physical pin 5, connects to DFP TX
    SoftwareSerial dfpSerial (DFPRx, DFPTx); // RX, TX
#endif

// DFP Status Decode
// MSB                           LSB
// 0x01 USB flash drive       | 0x00 Stopped
// 0x02 SD card               | 0x01 Playing
// 0x10 Module in sleep mode  | 0x02 Paused

#ifdef DFPLAYER_DEBUG
void dfpStatusDecode (int status)
{
    mySerial.print(F("DFP Status:"));
    mySerial.print(status, HEX);
    mySerial.print(F(" | "));

    switch ((status >> 8) & 0x13) {
      case 1:
          mySerial.print(F("USB flash drive"));
          break;
      case 2:
          mySerial.print(F("SD card"));
          break;
      case 0x10:
          mySerial.print(F("Module in sleep mode"));
          break;
      default:
          mySerial.print(F("Unknown Module"));
          break;
    }
    
    mySerial.print(F(" - "));

    switch (status & 0x3) {
      case 0:
          mySerial.println(F("Stopped"));
          break;
      case 1:
          mySerial.println(F("Playing"));
          break;
      case 2:
          mySerial.println(F("Paused"));
          break;
      default:
          mySerial.println(F("Unknown Status"));
          break;
    }
}
#endif

void setup()
{
#ifdef DFPLAYER_DEBUG
    pinMode(SerRx, INPUT);
    pinMode(SerTx, OUTPUT);
    mySerial.begin(115200); // send serial data at 9600 bits/sec

    delay (1000);
    mySerial.println();
    mySerial.println(F("UART @ 115200"));

    mySerial.println();
    mySerial.println(F("DFPlayer Test"));
    mySerial.println(F("Initializing DFPlayer ... (May take 3~5 seconds)"));
#endif

#ifdef USE_DFP_BUSY
    pinMode(DFPBusy, INPUT);
#endif

    pinMode(blinkLEDPin, OUTPUT);

    digitalWrite(blinkLEDPin, HIGH);  // turn the LED on
    delay(500);                       // wait
    digitalWrite(blinkLEDPin, LOW);   // turn the LED off
    delay(500);                       // wait

    dfpSerial.begin (9600);
    dfpSerial.listen();

  // Initial setup for DFPlayer.
  // Sets up DFP_BUSY_PIN if used to use the busy state/pin of the player. 
  // Resets the device and provides a 1 sec delay.
    dfpSetup();
    dfpSetVolume(DFP_AUDIO_LEVEL);
    dfpSetEq(DFP_EQ_CLASSIC);

#if defined(DFPLAYER_DEBUG) && !defined(USE_DFP_BUSY)
    query = dfpReadQuery(0);
    mySerial.print(F("Params Rst:"));
    mySerial.println(query, HEX);

    query = dfpGetVolume();
    mySerial.print(F("Params Vol:"));
    mySerial.println(query);

    query = dfpGetEq();
    mySerial.print(F("Params Eq:"));
    mySerial.println(query);

    query = dfpGetSDTracks();
    mySerial.print(F("SD Tracks:"));
    mySerial.println(query);

    maxTracks = query;

    query = dfpGetCurTrack();
    mySerial.print(F("Cur Track:"));
    mySerial.println(query);
#endif

    digitalWrite(blinkLEDPin, HIGH);  // turn the LED on
    delay(500);                       // wait
    digitalWrite(blinkLEDPin, LOW);   // turn the LED off
    delay(500);                       // wait

 //   dfpPlayRepeat(1);                 // Start playing all tracks
}

void loop()
{
  // Sample code, enter your own here.
  // dfpPlayTrackMP3(track);

    dfpPlayNext();

    timer = millis();

    delay(50);

#ifdef USE_DFP_BUSY
    if (digitalRead(DFPBusy)) {
        digitalWrite(blinkLEDPin, LOW);
    } else {
        digitalWrite(blinkLEDPin, HIGH);
    }

    /* Wait for busy line to go high, end of track */
    while (!digitalRead(DFPBusy) && (millis() - timer < 70000)) {
      delay(10);

      if (millis() - timer > 30000) {
        digitalWrite(blinkLEDPin, LOW);
        delay(200);
        digitalWrite(blinkLEDPin, HIGH);
      } 
    }

    delay(50);

    if (digitalRead(DFPBusy)) {
        digitalWrite(blinkLEDPin, LOW);
    } else {
        digitalWrite(blinkLEDPin, HIGH);
    }
#endif

#ifdef DFPLAYER_DEBUG
    mySerial.print(F("Playing Track:"));
    mySerial.println(track);

    #ifdef USE_DFP_BUSY
        query = digitalRead(DFPBusy);
        mySerial.print(F("Busy Pin:"));
        mySerial.println(query);
    #else
        query = dfpGetStatus ();
        dfpStatusDecode (query);
    
        query = dfpGetCurTrack();
        mySerial.print(F("Cur Track:"));
        mySerial.println(query);
    #endif
#endif
}
