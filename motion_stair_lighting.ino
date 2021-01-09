/*
  motion stair lightning

  LEDs that light up step by step in direction of movement when entering a staircase.
  Also turns off in direction of movement.

  Uses:
  - one photo resistor to evaluate if it is dark enough to enable pir sensor evaluation
  - two pir sensors for movement detection after it is dark enough
  - one LED strip which gets logicaly seperated into multiple areas and
    lights up step by step after movement has been detected

  segments are set via an array with all start LEDs of the corresponding "logical" segment

*/

#include <FastLED.h>
#if defined(FASTLED_VERSION) && (FASTLED_VERSION < 3001000)
#warning "Requires FastLED 3.1 or later; check github for latest code."
#endif

// ---------- Constants (wont change) ----------------------
#define DATA_PIN    7   //LED data pin
#define PIR1_PIN    5   //pin for PIR sensor 1
#define PIR2_PIN    6   //pin for PIR sensor 1
#define PHO1_PIN    0   //pin for photo sensor (analog pin)
#define LED_TYPE    WS2812B //type of LED strip
#define COLOR_ORDER GRB // order of colors
#define NUM_LEDS    289     //number of leds
#define BRIGHTNESS          25  // default brightness
//#define FRAMES_PER_SECOND  120
CRGB leds[NUM_LEDS];  //set CRGB color struct to all leds

// set some colors
// http://fastled.io/docs/3.1/struct_c_r_g_b.html
#define Magenta CRGB(0xff00ff)
#define Black CRGB(0x000000)
#define Gold CRGB(0xffd700)
#define Tomatoe CRGB(0xff6347)

// at which analog value we consider it to be dark
const int ldrDark = 825;
// at which analog value we consider it to be bright
const int ldrBright = 300;

// timeout after segments have been turned on
const unsigned long OFF_TIMEOUT = 3000;
// delay in ms between segment turn on
const unsigned long SEGMENTS_SWITCH_ON_DELAY = 400;
// delay in ms between segment turn off
const unsigned long SEGMENTS_SWITCH_OFF_DELAY = 200;
const int LED_SEGMENT_NUMBER = 17;
// set segments in steps by LED_SEGMENT_NUMBER plus the first and last LED at the strip which is used for definition of the for-loop range
// set start numbers of LED segments for up animation
const int SEGMENTS_UP[18] = { 0, 17, 34, 51, 68, 85, 102, 119, 136, 153, 170, 187, 204, 221, 238, 255, 272, 289 };
// set start numbers of LED segments for down animation
const int SEGMENTS_DOWN[18] = { 289, 272, 255, 238, 221, 204, 187, 170, 153, 136, 119, 102, 85, 68, 51, 34, 17, 0 };

// ---------- VARIABLES (will change) -------------------
unsigned long currentTime = 0;
unsigned long startTimeUp = 0;
unsigned long startTimeDown = 0;
int pirState = 0;
int ldrValue = 0;

// downstairs
int pirDownstairs = 0;
bool pirDownstairsTriggered = false;
int pirDownstairsTriggeredCount = 0;
int downstairsUpOnSegmentIndexNumber = 0;
int downstairsUpOffSegmentIndexNumber = 0;
int downstairsUpLedCounter = 0;
int downstairsUpLedCounterExport = 0;
bool downstairsUpOnAnimationComplete = false;
unsigned long downstairsUpOnAnimationCompleteTime = 0;
bool downstairsUpOffAnimationTriggered = false;
bool downstairsUpOffAnimationTrigger = false;

// upstairs
int pirUpstairs = 0;
bool pirUpstairsTriggered = false;
int pirUpstairsTriggeredCount = 0;
int upstairsDownOnSegmentIndexNumber = 0;
int upstairsDownOffSegmentIndexNumber = 0;
int upstairsDownLedCounter = 0;
int upstairsDownLedCounterExport = 0;
bool upstairsDownOnAnimationComplete = false;
unsigned long upstairsDownOnAnimationCompleteTime = 0;
bool upstairsDownOffAnimationTriggered = false;
bool upstairsDownOffAnimationTrigger = false;


// ------- setup ----------
void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  pinMode(DATA_PIN, OUTPUT);
  pinMode(PIR1_PIN, INPUT);
  pinMode(PIR2_PIN, INPUT);


  //FastLED stuff
  delay(1500); // 3 second delay for recover

  // tell FastLED about the LED strip configuration
  FastLED.addLeds<LED_TYPE, DATA_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);

  // set master brightness control
  FastLED.setBrightness(BRIGHTNESS);

  // clear everything before we go into action
  FastLED.clear();
  FastLED.show();

  // wait a short time to get PIR sensors ready
  delay(1500);

  // set initial startTime Upstairs to time which was already used for initialisation
  startTimeUp = currentTime;
  // set initial startTime Downstairs to time which was already used for initialisation
  startTimeDown = currentTime;


  Serial.println((String)"upstairsDownOn - AnimationCompleteTime " + upstairsDownOnAnimationCompleteTime);
}

void loop() {
  pirDownstairs = digitalRead(PIR1_PIN); // read PIR sensor downstairs
  pirUpstairs = digitalRead(PIR2_PIN);   // read PIR sensor upstairs
  ldrValue = analogRead(PHO1_PIN);
  currentTime = millis();                // set current time variable to current millis for further usage


  //Serial.println(pirDownstairs);
  //Serial.println(pirUpstairs);


  /* let me know if there is no motion and nothing has been triggered */
  /* if ( pirState == LOW && pirDownstairsTriggered == false && pirUpstairsTriggered == false) {
    //Serial.println("No motion!");
    } */

  /* check light before doing anything */
  //Serial.println(ldrValue);
  //if (ldrValue > ldrDark) {
  //Serial.println(pirDownstairsTriggered);
  //Serial.println(pirUpstairsTriggered);
    /* IF sequence for bottom to top */
    if ( pirDownstairs == HIGH && pirDownstairsTriggered == false ) {
      pirDownstairsTriggered = true;
      Serial.println("pirDownstairs motion detected!");
      if ( currentTime - startTimeUp >= SEGMENTS_SWITCH_ON_DELAY ) {
        downstairsUpOn(Magenta);
        FastLED.show();
        startTimeUp = currentTime;
      }
    } 
    if ( pirDownstairsTriggered == true ) {
      if ( currentTime - startTimeUp >= SEGMENTS_SWITCH_ON_DELAY ) {
        downstairsUpOn(Magenta);
        FastLED.show();
        startTimeUp = currentTime;
      }
    }

    /* IF sequence for top to bottom */
    if ( pirUpstairs == HIGH && pirUpstairsTriggered == false ) {
      pirUpstairsTriggered = true;
      Serial.println("pirUpstairs motion detected!");
      if ( currentTime - startTimeDown >= SEGMENTS_SWITCH_ON_DELAY ) {
        upstairsDownOn(Magenta);
        FastLED.show();
        startTimeDown = currentTime;
      }
    }
    if ( pirUpstairsTriggered == true ) {
      if ( currentTime - startTimeDown >= SEGMENTS_SWITCH_ON_DELAY ) {
        upstairsDownOn(Magenta);
        FastLED.show();
        startTimeDown = currentTime;
        //delay(10000);
      }
    }
  //}

  /*
    IF routine for turning off the LEDs
      does not check light as turning it off should also work if it gets bright during
      turning it on or while LEDs are on
  */

  //Serial.println((String)"downstairsUpOnAnimationComplete: " + downstairsUpOnAnimationComplete);
  if ( downstairsUpOnAnimationComplete == true ) {  // check if on animation is complete before trying to turn LEDs off
    //////Serial.println((String)"downstairsUpOffAnimationTriggered: " + downstairsUpOffAnimationTriggered);

    //////Serial.println((String)"upStairsDownOff - Timeout " + OFF_TIMEOUT + "ms");
    //Serial.println((String)"downstairsUpOn - AnimationCompleteTime " + downstairsUpOnAnimationCompleteTime);
    //////Serial.println((String)"upStairsDownOff - current time " + currentTime);
    if ( currentTime - downstairsUpOnAnimationCompleteTime >= OFF_TIMEOUT ) { // check current time against ON animation time to start turning LEDs off
      if ( currentTime - startTimeUp >= SEGMENTS_SWITCH_OFF_DELAY ) {
        downstairsUpOff(Black);
        FastLED.show();
        startTimeUp = currentTime;
      }
    }
  }
  Serial.println((String)"upstairsDownOnAnimationComplete: " + upstairsDownOnAnimationComplete);
  Serial.println((String)"upstairsDownOn - AnimationCompleteTime " + upstairsDownOnAnimationCompleteTime);
  if ( upstairsDownOnAnimationComplete == true ) {
    //////Serial.println((String)"upstairsDownOffAnimationTriggered: " + upstairsDownOffAnimationTriggered);

    //////Serial.println((String)"upStairsDownOff - Timeout " + OFF_TIMEOUT + "ms");
    Serial.println((String)"upstairsDownOn - AnimationCompleteTime " + upstairsDownOnAnimationCompleteTime);
    //Serial.println((String)"upStairsDownOff - current time " + currentTime);
    if ( currentTime - upstairsDownOnAnimationCompleteTime >= OFF_TIMEOUT ) { // check current time against ON animation time to start turning LEDs off
      if ( currentTime - startTimeDown >= SEGMENTS_SWITCH_OFF_DELAY ) {
        upstairsDownOff(Black);
        FastLED.show();
        startTimeDown = currentTime;
      }
    }
  }
}

// function for LED ON animation of bottom to top
void downstairsUpOn( CRGB ledColour ) {
  // debug stuff
  ////Serial.println("-------------------------------");
  ////Serial.println((String)"downstairsUpOnSegmentIndexNumber: " + downstairsUpOnSegmentIndexNumber);

  // execute the for loop only as much as we have Segments minus 1 as we start at 0 in the segment array
  if ( downstairsUpOnSegmentIndexNumber < LED_SEGMENT_NUMBER ) {
    // loop from beginning of array number to beginning of next segment (0 to 16 > 17 to 33 > 34 to 50 and so on)
    for (int downstairsUpLedCounter = SEGMENTS_UP[downstairsUpOnSegmentIndexNumber]; downstairsUpLedCounter < SEGMENTS_UP[downstairsUpOnSegmentIndexNumber + 1]; downstairsUpLedCounter++ ) {
      leds[downstairsUpLedCounter] = ledColour;  // write color to LED

      // debug stuff
      //////Serial.println((String)"LED Counter: " + downstairsUpLedCounter);
      downstairsUpLedCounterExport++; // increase counter export variable to print it outside of for loop
    }
    downstairsUpOnSegmentIndexNumber++; // increase index number of up index to get the next element in array for next loop
    
    // debug stuff
    ////Serial.println((String)"UP ON: SEGMENTS_UP current segment value: " + SEGMENTS_UP[downstairsUpOnSegmentIndexNumber]);
    ////Serial.println((String) "UP ON: downstairsUpOnSegmentIndexNumber: " + downstairsUpOnSegmentIndexNumber);
    //////Serial.println((String) "UP ON: downstairsUpLedCounterExport: " + downstairsUpLedCounterExport);
    ////Serial.println("-------------------------------");

    // add else if for state change when animation is complete and to start timing for turning LEDs off
  }
  
  if ( downstairsUpOnSegmentIndexNumber == LED_SEGMENT_NUMBER) {
    pirDownstairsTriggered = false;
    if ( downstairsUpOnAnimationComplete == false ) {
      // set state for animation to complete but only if it is not already set
      downstairsUpOffAnimationTrigger = true;
      downstairsUpOnAnimationComplete = false; 
    }
    // only record Animation complete time once
    // set state for off animation to false to overwrite a set off trigger and allow off triggering again
    if ( downstairsUpOnAnimationCompleteTime == 0 ) {
      downstairsUpOnAnimationCompleteTime = millis();
      downstairsUpOffAnimationTriggered = false;
      downstairsUpOnSegmentIndexNumber = 0;
    }
  }
}

/* function for LED OFF animation of bottom to top */
void downstairsUpOff(CRGB ledColour) {
  // debug stuff
  ////Serial.println("-------------------------------");
  ////Serial.println((String)"downstairsUpOffSegmentIndexNumber: " + downstairsUpOffSegmentIndexNumber);

  // execute the for loop only as much as we have segments minus 1 as we start at 0 in the segment array
  if ( downstairsUpOffSegmentIndexNumber < LED_SEGMENT_NUMBER ) {
    // loop from beginning of array number to beginning of next segment (0 to 16 > 17 to 33 > 34 to 50 and so on)
    for (int downstairsUpLedCounter = SEGMENTS_UP[downstairsUpOffSegmentIndexNumber]; downstairsUpLedCounter < SEGMENTS_UP[downstairsUpOffSegmentIndexNumber + 1]; downstairsUpLedCounter++ ) {
      leds[downstairsUpLedCounter] = ledColour;  // write color to LED

      // debug stuff
      //////Serial.println((String)"LED Counter: " + downstairsUpLedCounter);
      downstairsUpLedCounterExport++; // increase counter export variable to print it outside of for loop
    }
    downstairsUpOffSegmentIndexNumber++; // increase index number of up index to get the next element in array for next loop

    // debug stuff
    //Serial.println((String)"UP OFF: SEGMENTS_UP current segment value: " + SEGMENTS_UP[downstairsUpOffSegmentIndexNumber]);
    //Serial.println((String) "UP OFF: downstairsUpOffSegmentIndexNumber: " + downstairsUpOffSegmentIndexNumber);
    ////Serial.println((String) "UP OFF: downstairsUpLedCounterExport: " + downstairsUpLedCounterExport);
    //Serial.println("-------------------------------");

    // set animation trigger state to true to inform loop about already started turning off routine
    downstairsUpOffAnimationTriggered = true;

  }
  if ( downstairsUpOffSegmentIndexNumber == LED_SEGMENT_NUMBER && downstairsUpOffAnimationTrigger == true ) {
    // reset variables to allow toggling of ON routine again
    downstairsUpOnAnimationCompleteTime = 0;
    downstairsUpOffAnimationTrigger = false;
    downstairsUpOnAnimationComplete = false;
    downstairsUpOffSegmentIndexNumber = 0;
    pirDownstairsTriggered = false;
  }
}

/* function for LED ON animation of top to bottom */
void upstairsDownOn(CRGB ledColour) {
  // debug stuff
  Serial.println("-------------------------------");
  //Serial.println((String)"upstairsDownOnSegmentIndexNumber: " + upstairsDownOnSegmentIndexNumber);
  Serial.println((String) "DOWN ON: upstairsDownOnAnimationComplete: " + upstairsDownOnAnimationComplete);
  // execute the for loop only as much as we have segments minus 1 as we start at 0 in the segment array
  if ( upstairsDownOnSegmentIndexNumber < LED_SEGMENT_NUMBER ) {
    // loop from beginning of array number to beginning of next segment (289 > 272 to 271 > 255 and so on)
    for (upstairsDownLedCounter = SEGMENTS_DOWN[upstairsDownOnSegmentIndexNumber]; upstairsDownLedCounter > SEGMENTS_DOWN[upstairsDownOnSegmentIndexNumber + 1]; upstairsDownLedCounter-- ) {
      leds[upstairsDownLedCounter] = ledColour;  // write color to LED

      // debug stuff
      //Serial.println((String)"LED Counter: " + upstairsDownLedCounter);
    }
    upstairsDownOnSegmentIndexNumber++; // increase index number of up index to get the next element in array for next loop
    
    // debug stuff
    //Serial.println((String)"DOWN ON: SEGMENTS_DOWN current segment value: " + SEGMENTS_DOWN[upstairsDownOnSegmentIndexNumber]);
    Serial.println((String) "DOWN ON: upstairsDownOnSegmentIndexNumber: " + upstairsDownOnSegmentIndexNumber);
    ////Serial.println((String) "DOWN ON: upstairsDownLedCounterExport: " + upstairsDownLedCounterExport);
    //Serial.println("-------------------------------");

    // add else if for state change when animation is complete and to start timing for turning LEDs off
  }
  if ( upstairsDownOnSegmentIndexNumber == LED_SEGMENT_NUMBER) {
    pirUpstairsTriggered = false;
    // set state for animation to complete but only if it is not already set
    if (upstairsDownOnAnimationComplete == false) { 
      upstairsDownOffAnimationTrigger = true;
      upstairsDownOffAnimationTrigger = true;
    }
    // only record Animation complete time once
    // set state for off animation to false to overwrite a set off trigger and allow off triggering again
    if ( upstairsDownOnAnimationCompleteTime == 255 ) {
      Serial.println((String)"upstairsDownOn Function - AnimationCompleteTime " + upstairsDownOnAnimationCompleteTime);
      upstairsDownOnAnimationCompleteTime = millis();
      upstairsDownOffAnimationTriggered = false;
      upstairsDownOnSegmentIndexNumber = 0;
    }
  }
}

// function for LED OFF animation of to to bottom
void upstairsDownOff(CRGB ledColour) {
  // debug stuff
  //Serial.println("-------------------------------");
  //Serial.println((String)"upstairsDownOffSegmentIndexNumber: " + upstairsDownOffSegmentIndexNumber);

  // execute the for loop only as much as we have segments minus 1 as we start at 0 in the segment array
  if ( upstairsDownOffSegmentIndexNumber < LED_SEGMENT_NUMBER ) {
    // loop from beginning of array number to beginning of next segment (0 to 16 > 17 to 33 > 34 to 50 and so on)
    for (int upstairsDownLedCounter = SEGMENTS_DOWN[upstairsDownOffSegmentIndexNumber]; upstairsDownLedCounter > SEGMENTS_DOWN[upstairsDownOffSegmentIndexNumber + 1]; upstairsDownLedCounter-- ) {
      leds[upstairsDownLedCounter] = ledColour;  // write color to LED

      // debug stuff
      ////Serial.println((String)"LED Counter: " + upstairsDownLedCounter);
      upstairsDownLedCounterExport++; // increase counter export variable to print it outside of for loop
    }
    upstairsDownOffSegmentIndexNumber++; // increase index number of up index to get the next element in array for next loop

    // debug stuff
    //Serial.println((String)"DOWN OFF: SEGMENTS_DOWN current segment value: " + SEGMENTS_DOWN[upstairsDownOffSegmentIndexNumber]);
    Serial.println((String) "DOWN OFF: upstairsDownOffSegmentIndexNumber: " + upstairsDownOffSegmentIndexNumber);
    ////Serial.println((String) "DOWN OFF: upstairsDownLedCounterExport: " + upstairsDownLedCounterExport);
    //Serial.println("-------------------------------");

    // set animation trigger state to true to inform loop about already started turning off routine
    upstairsDownOffAnimationTriggered = true;

  }
  
  if ( upstairsDownOffSegmentIndexNumber == LED_SEGMENT_NUMBER && upstairsDownOffAnimationTrigger == true ) {
    // reset variables to allow toggling ON routine again
    Serial.println("################## ich werde jetzt gleich alles von upstairsDown für ein ON vorbereiten");
    upstairsDownOnAnimationCompleteTime = 0;
    upstairsDownOnAnimationComplete = false;
    upstairsDownOffAnimationTrigger = false;
    upstairsDownOffSegmentIndexNumber = 0;
    pirUpstairsTriggered = false;
  }
}
