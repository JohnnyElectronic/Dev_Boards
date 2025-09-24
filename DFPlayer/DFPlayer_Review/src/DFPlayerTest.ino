/***************************************************
DFPlayer Tester 

 GNU Lesser General Public License.
 See <http://www.gnu.org/licenses/> for details.
 All above must be included in any redistribution
 ****************************************************/

/***********Notice and Trouble shooting***************
 1.Connection and Diagram can be found here
 <https://www.dfrobot.com/wiki/index.php/DFPlayer_Mini_SKU:DFR0299#Connection_Diagram>
 2.This code is tested on Arduino Nano
 ****************************************************/

#include "DFPlayer.h"

// Running on Arduino NANO

//#define DFP_DEBUG                 /* Enables display of received data for query requests to Serial */
//#define BUSY_PIN_TEST               /* Runs test for busy pin */
//#define LOW_PWR_TEST                /* Enable low power mode, CMD: 0x0A */
//#define CONT_PLAY_TEST              /* Runs continuious play test based on busy pin */
//#define LOOP_PLAY_TEST              /* Runs continuious play test every 1300ms */
//#define REPEAT_TRACK_TEST           /* Enables repeat current track test */
#define REPEAT_PLAY_TEST            /* Repeat all tracks test */

//#define DFP_VER_3_0                 /* Adds additional delays for V 3.0 */
#define DFP_VER_3_0_DELAY  120      /* Additional delay used for V 3.0 after call to play track - 120 or greater/ */

// DFPlayer.h settings
// DFP_RX_PIN               12     /* Pin Assigned for serial RX, set to -1 if not used - D12 Uno/Nano */
// DFP_TX_PIN                8     /* Pin Assigned for serial TX  - D8 Uno/Nano */
// DFP_BUSY_PIN              0     /* Pin Assigned for busy detection, set to 0 if not used */

#define DFP_RX_PIN             3     /* Pin Assigned for serial RX, set to -1 if not used - D3 Uno/Nano */
#define DFP_TX_PIN             4     /* Pin Assigned for serial TX  - D4 Uno/Nano */
#define DFP_BUSY_PIN          12     /* Busy status, active low */

SoftwareSerial dfpSerial (DFP_RX_PIN, DFP_TX_PIN); // RX, TX

#define BLINK_LED_OUT     13 /* Blink LED, D13 */ 

#define DFP_AUDIO_LEVEL 25
#define DFP_EXPECTED_TRACKS   9

/* MP3 Folder Audio Tracks (/mp3):
1- 0001_emf start                       0.238s
2- 0002_emf low short                   0.282s
3- 0003_emf steady short (High Tone)    0.568s
4- 0004_emf steady long (High Tone)     1.435s
5- 0005_emf end                         0.282s
6- 0006_emf power up                    copy of emf start, 0.238s
*/
#define MP3_TRACK1   1    
#define MP3_TRACK2   2    
#define MP3_TRACK3   3    

const int dfpBusyPin = DFP_BUSY_PIN;
const int blinkLEDPin = BLINK_LED_OUT;

int queryVal;
int errorCnt = 0;
int loopCnt = 0;
unsigned long timer;
unsigned long timer2;
unsigned long result;

void setup()
{
  bool timeOut = false;
  bool busyPinDetected = false;
  Serial.begin(115200);
  Serial.println();
  Serial.println(F("UART @ 115200"));
  Serial.println();
  Serial.println(F("DFPlayer Test"));
  Serial.print(F("DFPlayer Cmd Delay (ms): "));
  Serial.println(DFP_CMD_DELAY);

#ifdef DFP_VER_3_0
  Serial.print(F("DFPlayer 3.0 Delay Enabled (ms): "));
  Serial.println(DFP_VER_3_0_DELAY);
#endif

  Serial.println(F("Initializing DFPlayer ... (May take 3~5 seconds)"));

    pinMode(blinkLEDPin, OUTPUT); 
    pinMode(dfpBusyPin, INPUT); 
    
    dfpSerial.begin (9600);
    dfpSerial.listen();

    timer = millis();
    dfpSetup(); /* Sets up Busy pin if used, Resets the device and provides a 1 sec delay. */
    queryVal = dfpAvailWait();
    timer2 = millis();

    if (queryVal) {
        Serial.print(F("DFP Initialization Time (ms):"));
        Serial.println(timer2 - timer);

        queryVal = dfpReadQuery(0);

        Serial.print(F("DFP Initialization Response: 0x"));
        Serial.print(queryVal, HEX);
        Serial.print(F(", CMD: 0x"));
        Serial.println(dfpLastRxCmd(), HEX);
    } else {
        Serial.println(F("DFP Initialization Response: TIMEOUT"));
        Serial.println(F("ERROR"));
        errorCnt++;
    }

    dfpSetVolume(DFP_AUDIO_LEVEL);
    dfpSetEq(DFP_EQ_CLASSIC);

    queryVal = dfpGetSwVer();
    Serial.print(F("Params SW Ver: "));
    Serial.print(queryVal);
    Serial.print(F(", 0x"));
    Serial.println(queryVal, HEX);

    /* Configure settings and check results */
    queryVal = dfpGetVolume();
    Serial.print(F("Params Vol: "));
    Serial.print(queryVal);
    if (queryVal > 9) {
      Serial.print(F(", 0x"));
      Serial.println(queryVal, HEX);
    } else {
      Serial.println();
    }
    if (queryVal != DFP_AUDIO_LEVEL) {
        Serial.println(F("ERROR"));
        errorCnt++;
    }

    queryVal = dfpGetEq();
    Serial.print(F("Params Eq:"));
    Serial.println(queryVal);
    
    if (queryVal != DFP_EQ_CLASSIC) {
        Serial.println(F("ERROR"));
        errorCnt++;
    }

    queryVal = dfpGetSDTracks();
    Serial.print(F("Params SD Tracks:"));
    Serial.print(queryVal);
    if (queryVal > 9) {
      Serial.print(F(", 0x"));
      Serial.println(queryVal, HEX);
    } else {
      Serial.println();
    }

    if (queryVal != DFP_EXPECTED_TRACKS) {
        Serial.println(F("ERROR"));
        errorCnt++;
    }

#ifdef BUSY_PIN_TEST
    Serial.print(F("DFP Busy Pin Test: "));
    if (digitalRead(dfpBusyPin)) {
        dfpPlayTrackMP3(MP3_TRACK1);

        timer = millis();
        while (digitalRead(dfpBusyPin)) {
          delay(1);
          timer2 = millis();
          if ((timer2 - timer) > 1000) {
              timeOut = true;
              break;
          }
        }
        timer2 = millis();

        if (timeOut) {
            Serial.println(F("TIMEOUT"));
            Serial.println(F("ERROR"));
            errorCnt++;
        } else {
            if (digitalRead(dfpBusyPin)) {
                Serial.print(F(", Expected active low, pin D"));
                Serial.println(DFP_BUSY_PIN);
                Serial.println(F("ERROR"));
                errorCnt++;
            } else {
                Serial.println(F("OK"));

                Serial.print(F("DFP Busy Active (ms): "));
                Serial.println(timer2 - timer);
                busyPinDetected = true;
            }

            /* Wait for busy line to clear */
            while (!digitalRead(dfpBusyPin)) {
              delay(10);
            }
        }
    } else {
        Serial.print(F(", Expected idle high, pin D"));
        Serial.println(DFP_BUSY_PIN);
        Serial.println(F("ERROR"));
        errorCnt++;
    }

    delay (1000);

    while (dfpSerialAvail()) {
        queryVal = dfpReadQuery(0);

        Serial.print(F("DFP Response: 0x"));
        Serial.print(queryVal, HEX);
        Serial.print(F(", CMD: 0x"));
        Serial.println(dfpLastRxCmd(), HEX);
    }

#else
    busyPinDetected = true;
#endif

/******************************************************/
#ifdef CONT_PLAY_TEST

    Serial.println();
    if (busyPinDetected) {
        Serial.println(F("DFP Continuous Play Test:"));
        /* Busy pin is active low */
    
        timer = millis();
        dfpPlayTrackMP3(MP3_TRACK3);
    
        /* Wait for busy line to go low */
        while (digitalRead(dfpBusyPin)) {
          delay(1);
        }
        timer2 = millis();

        Serial.print(F(" - DFP CMD To Play Start:"));
        Serial.println(timer2 - timer);

#ifdef DFP_VER_3_0
        delay (DFP_VER_3_0_DELAY);
#endif
    
        for (loopCnt = 0; loopCnt < 3; loopCnt++) {
            /* Wait for file to finish, active low */
            while (!digitalRead(dfpBusyPin)) {
                delay(1);
            }

            timer = millis();
            dfpPlayTrackMP3(MP3_TRACK3);

            while (digitalRead(dfpBusyPin)) {
                delay(1);
                timer2 = millis();
            }
            timer2 = millis();
            
            Serial.println ();
            Serial.print(F(" - DFP Gap Time:"));
            Serial.println(timer2 - timer);

#ifdef DFP_VER_3_0
            delay (DFP_VER_3_0_DELAY);
#endif
        }

        delay (2000);

        if (dfpSerial.overflow()) {
            Serial.println(F("Serial overflow error!"));
        }

        dfpSerialPurge();
        delay (200);

        while (dfpSerialAvail()) {
            queryVal = dfpReadQuery(0);

            Serial.print(F("DFP Post Response: 0x"));
            Serial.print(queryVal, HEX);
            Serial.print(F(", CMD: 0x"));
            Serial.println(dfpLastRxCmd(), HEX);
        }

        queryVal = dfpGetStatus();
        Serial.print(F("DFP Status: Dev 0x"));
        Serial.print(queryVal >> 8, HEX);
        Serial.print(F(", State 0x"));
        Serial.println(queryVal & 0xFF, HEX);

        // Check for stopped status 
        if ((queryVal & 0xFF) != 0) {
            Serial.println(F("Not Stopped, ERROR"));
            errorCnt++;
        }

    } else {
        Serial.println(F("Skipped Continuous Play Test due to error"));
    }

#endif

/******************************************************/
#ifdef LOOP_PLAY_TEST
    Serial.println();
    Serial.println(F("DFP Loop Play Test:"));
    for (loopCnt = 0; loopCnt < 3; loopCnt++) {
        dfpPlayTrackMP3(MP3_TRACK3);   // 1.435 seconds */

        timer = millis();

        while ((millis() - timer) < 1300) {
            delay (20);
            while (dfpSerialAvail()) {
                queryVal = dfpReadQuery(0);

                Serial.print(F("DFP Response: 0x"));
                Serial.print(queryVal, HEX);
                Serial.print(F(", CMD: 0x"));
                Serial.println(dfpLastRxCmd(), HEX);
            }
        }
    }

    delay (1500);
    while (dfpSerialAvail()) {
        queryVal = dfpReadQuery(0);

        Serial.print(F("DFP Post Response: 0x"));
        Serial.print(queryVal, HEX);
        Serial.print(F(", CMD: 0x"));
        Serial.println(dfpLastRxCmd(), HEX);
    }

    queryVal = dfpGetStatus();
    Serial.print(F("DFP Status: Dev 0x"));
    Serial.print(queryVal >> 8, HEX);
    Serial.print(F(", State 0x"));
    Serial.println(queryVal & 0xFF, HEX);

    // Check for stopped status 
    if ((queryVal & 0xFF) != 0) {
        Serial.println(F("Not Stopped, ERROR"));
        errorCnt++;
    }
#endif

/******************************************************/
#ifdef REPEAT_TRACK_TEST

    Serial.println();
    Serial.println(F("DFP Repeat Play Track Test:"));

    dfpPlayTrackMP3(MP3_TRACK3);
    delay (10);
    dfpRepeatTrack(0);  // Enable repeat track

    timer = millis();
    while ((millis() - timer) < 5000) {
        while (dfpSerialAvail()) {
            delay (20);
            queryVal = dfpReadQuery(0);

            Serial.print(F("DFP Response: 0x"));
            Serial.print(queryVal, HEX);
            Serial.print(F(", CMD: 0x"));
            Serial.println(dfpLastRxCmd(), HEX);
        }
    }

    dfpRepeatTrack(1);  // Disable repeat track
    delay (1500);

    while (dfpSerialAvail()) {
        delay (20);
        queryVal = dfpReadQuery(0);

        Serial.print(F("DFP Post Response: 0x"));
        Serial.print(queryVal, HEX);
        Serial.print(F(", CMD: 0x"));
        Serial.println(dfpLastRxCmd(), HEX);
    }

    queryVal = dfpGetStatus();
    Serial.print(F("DFP Status: Dev 0x"));
    Serial.print(queryVal >> 8, HEX);
    Serial.print(F(", State 0x"));
    Serial.println(queryVal & 0xFF, HEX);

    // Check for stopped status 
    if ((queryVal & 0xFF) != 0) {
        Serial.println(F("Not Stopped, ERROR"));
        errorCnt++;
    }
#endif
/******************************************************/
#ifdef REPEAT_PLAY_TEST

    Serial.println();
    Serial.println(F("DFP Repeat Play All Test:"));

    Serial.println(F(" - Enable Repeat"));
    dfpPlayRepeat(1);

    timer = millis();

    while ((millis() - timer) < 60000) {
        while (dfpSerialAvail()) {
            delay (20);
            queryVal = dfpReadQuery(0);

            Serial.print(F("DFP Response: 0x"));
            Serial.print(queryVal, HEX);
            Serial.print(F(", CMD: 0x"));
            Serial.println(dfpLastRxCmd(), HEX);
        }
    }

    delay (1000);
    Serial.println(F(" - Disable Repeat"));
    dfpStop();
    dfpPlayRepeat(0);
    delay (500);
    dfpPlayTrackMP3(MP3_TRACK1);
    delay (100);

    while (dfpSerialAvail()) {
        delay (20);
        queryVal = dfpReadQuery(0);

        Serial.print(F("DFP Post Response: 0x"));
        Serial.print(queryVal, HEX);
        Serial.print(F(", CMD: 0x"));
        Serial.println(dfpLastRxCmd(), HEX);
    }

    queryVal = dfpGetStatus();
    Serial.print(F("DFP Status: Dev 0x"));
    Serial.print(queryVal >> 8, HEX);
    Serial.print(F(", State 0x"));
    Serial.println(queryVal & 0xFF, HEX);

    // Check for stopped status 
    if ((queryVal & 0xFF) != 0) {
        Serial.println(F("Not Stopped, ERROR"));
        errorCnt++;
    }
#endif
/******************************************************/

#ifdef LOW_PWR_TEST

    Serial.println();
    Serial.println(F("DFP Low Power Test:"));

    Serial.println(F("- Normal DFP Power"));

    queryVal = dfpGetStatus();
    Serial.print(F("DFP Status: Dev 0x"));
    Serial.println(queryVal, HEX);

    Serial.println(F("- DFP Sleep Mode"));

    dfpExecuteCmd(0x09, 0, 6);    // Set sleep mode
    delay (200);

    queryVal = dfpGetStatus();
    Serial.print(F("DFP Status: Dev 0x"));
    Serial.println(queryVal, HEX);

    // Check for stopped status 
    if (queryVal != -1) {
        Serial.println(F("Not in Sleep mode, ERROR"));
        errorCnt++;
    }

    dfpPlayTrackMP3(MP3_TRACK1);  // Track should not play

    delay (2000);

    Serial.println(F("- DFP End Sleep Mode"));

    dfpExecuteCmd(0x09, 0, 2);    // End sleep mode

    queryVal = dfpGetStatus();
    Serial.print(F("DFP Status: Dev 0x"));
    Serial.println(queryVal, HEX);

    dfpPlayTrackMP3(MP3_TRACK1); // Track should play
    delay (300);

    while (dfpSerialAvail()) {
        delay (20);
        queryVal = dfpReadQuery(0);

        Serial.print(F("DFP Post Response: 0x"));
        Serial.print(queryVal, HEX);
        Serial.print(F(", CMD: 0x"));
        Serial.println(dfpLastRxCmd(), HEX);
    }

    queryVal = dfpGetStatus();
    Serial.print(F("DFP Status: Dev 0x"));
    Serial.print(queryVal >> 8, HEX);
    Serial.print(F(", State 0x"));
    Serial.println(queryVal & 0xFF, HEX);

    // Check for stopped status 
    if ((queryVal & 0xFF) != 0) {
        Serial.println(F("Not Stopped, ERROR"));
        errorCnt++;
    }

#endif

    Serial.println();
    Serial.print(F("Errors:"));
    Serial.println(errorCnt);

    Serial.println(F("END OF TEST"));
}

void loop()
{

  dfpPlayNext();

  while (!digitalRead(dfpBusyPin)) {
      digitalWrite(blinkLEDPin, HIGH);  // turn the LED on (HIGH is the voltage level)
      delay(700);                      // wait for a second
      digitalWrite(blinkLEDPin, LOW);   // turn the LED off by making the voltage LOW
      delay(700);                      // wait for a second
  }

}

