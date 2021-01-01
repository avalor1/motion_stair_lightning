#include <FastLED.h>
#if defined(FASTLED_VERSION) && (FASTLED_VERSION < 3001000)
#warning "Requires FastLED 3.1 or later; check github for latest code."
#endif

// ---------- Constants (wont change) ----------------------
#define DATA_PIN    7   //LED data pin
#define PIR1_PIN    5   //pin for PIR sensor 1
#define PIR2_PIN    6   //pin for PIR sensor 1
#define LED_TYPE    WS2812B
#define COLOR_ORDER GRB
#define NUM_LEDS    289     //number of leds
CRGB leds[NUM_LEDS];
#define BRIGHTNESS          25
#define FRAMES_PER_SECOND  120

// set some colors
// http://fastled.io/docs/3.1/struct_c_r_g_b.html
#define Magenta CRGB(0xff00ff)
#define Black CRGB(0x000000)
#define Gold CRGB(0xffd700)
#define Tomatoe CRGB(0xff6347)

//timeout after segments have been turned on
const unsigned long OFF_TIMEOUT = 1500;
//delay in ms between segment turn on
const unsigned long SEGMENTS_SWITCH_ON_DELAY = 400;
//delay in ms between segment turn off
const unsigned long SEGMENTS_SWITCH_OFF_DELAY = 300;
const int LED_SEGMENT_NUMBER = 17;
const int SEGMENTS_UP[17] = { 0, 17, 34, 51, 68, 85, 102, 119, 136, 153, 170, 187, 204, 221, 238, 255, 272 };
const int SEGMENTS_DOWN[17] = {289, 272, 255, 238, 221, 204, 187, 170, 153, 136, 119, 102, 85, 68, 51, 34, 17};



// ---------- VARIABLES (will change) -------------------
unsigned long currentTime = 0;
unsigned long startTimeUp = 0;
unsigned long startTimeDown = 0;
int pirState = 0;

// downstairs
int pirDownstairs = 0;
bool pirDownstairsTriggered = false;
int pirDownstairsTriggeredCount = 0;
int downstairsUpSegmentIndexNumber = 0;
int downstairsUpLedCounter = 0;
int downstairsUpLedCounterExport = 0;
bool downstairsUpOnAnimationComplete = false;
unsigned long downstairsUpOnAnimationCompleteTime = 0;
bool downstairsUpOffAnimationTriggered = false;

// upstairs
int pirUpstairs = 0;
bool pirUpstairsTriggered = false;
int pirUpstairsTriggeredCount = 0;
int upstairsDownSegmentIndexNumber = 0;
int upstairsDownLedCounter = 0;
int upstairsDownLedCounterExport = 0;
bool upstairsDownOnAnimationComplete = false;
unsigned long upstairsDownOnAnimationCompleteTime = 0;
bool upstairsDownOffAnimationTriggered = false;



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


  // clear everything before
  FastLED.clear();
  FastLED.show();
  
  startTimeUp = currentTime;             // set initial startTime to time which was already used for initialisation
  startTimeDown = currentTime;           // set initial startTime to time which was already used for initialisation
  delay(1500);                           // wait a short time to get PIR sensors ready
}

void loop() {
  pirDownstairs = digitalRead(PIR1_PIN); // read PIR sensor downstairs
  pirUpstairs = digitalRead(PIR2_PIN);   // read PIR sensor upstairs
  currentTime = millis();                // set current time variable to current millis for further usage


  //Serial.println(pirDownstairs);
  //Serial.println(pirUpstairs);

  /* let me know if there is no motion and nothing has been triggered */
  /* if ( pirState == LOW && pirDownstairsTriggered == false && pirUpstairsTriggered == false) {
    Serial.println("No motion!");
    } */

  /* IF sequence for bottom to top */
  if ( pirDownstairs == LOW && pirDownstairsTriggered == false ) {
    pirDownstairsTriggered = true;
    Serial.println("pirDownstairs motion detected!");
    if ( currentTime - startTimeUp >= SEGMENTS_SWITCH_ON_DELAY ) {
      downstairsUpOn(Gold);
      FastLED.show();
      startTimeUp = currentTime;
      //digitalWrite(DATA_PIN, !digitalRead(DATA_PIN));
    }
  } else if ( pirDownstairsTriggered == true ) {
    if ( currentTime - startTimeUp >= SEGMENTS_SWITCH_ON_DELAY ) {
      downstairsUpOn(Gold);
      FastLED.show();
      startTimeUp = currentTime;
      //digitalWrite(DATA_PIN, !digitalRead(DATA_PIN));
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
      //digitalWrite(DATA_PIN, !digitalRead(DATA_PIN));
    }
  } else if ( pirUpstairsTriggered == true ) {
    if ( currentTime - startTimeDown >= SEGMENTS_SWITCH_ON_DELAY ) {
      upstairsDownOn(Magenta);
      FastLED.show();
      startTimeDown = currentTime;
      //digitalWrite(DATA_PIN, !digitalRead(DATA_PIN));
    }
  }

  /* IF routine for turning off the LEDs */
  //Serial.println((String)"downstairsUpOnAnimationComplete: " + downstairsUpOnAnimationComplete);
  if ( downstairsUpOnAnimationComplete == true || upstairsDownOnAnimationComplete == true) {  // check if on animation is complete before trying to turn LEDs off
    //Serial.println((String)"downstairsUpOffAnimationTriggered: " + downstairsUpOffAnimationTriggered);
    if ( downstairsUpOffAnimationTriggered == false ) { // check if off animation was not triggered
      //Serial.println((String)"upStairsDownOff - Timeout " + OFF_TIMEOUT + "ms");
      //Serial.println((String)"upStairsDownOff - AnimationCompleteTime " + downstairsUpOnAnimationCompleteTime);
      //Serial.println((String)"upStairsDownOff - current time " + currentTime);
      if ( currentTime - downstairsUpOnAnimationCompleteTime >= OFF_TIMEOUT ) { // check current time against ON animation time to start turning LEDs off
        downstairsUpOff(Black);
        FastLED.show();
      }
    } else if (upstairsDownOffAnimationTriggered == false ) { // check if off animation was not triggered)
      //Serial.println((String)"upStairsDownOff - Timeout " + OFF_TIMEOUT + "ms");
      //Serial.println((String)"upStairsDownOff - AnimationCompleteTime " + upstairsDownOnAnimationCompleteTime);
      //Serial.println((String)"upStairsDownOff - current time " + currentTime);
      if ( currentTime - upstairsDownOnAnimationCompleteTime >= OFF_TIMEOUT ) { // check current time against ON animation time to start turning LEDs off
        upstairsDownOff(Black);
        FastLED.show();
      }
    }
  }
}

// function for LED ON animation of bottom to top
void downstairsUpOn( CRGB ledColour ) {
  // debug stuff
  Serial.println((String)"downstairsUpSegmentIndexNumber: " + downstairsUpSegmentIndexNumber);

  // execute the for loop only as much as we have Segments
  if ( downstairsUpSegmentIndexNumber < LED_SEGMENT_NUMBER ) {
    // loop from beginning of array number to beginning of next segment (0 to 16 > 17 to 33 > 34 to 50 and so on)
    for (int downstairsUpLedCounter = SEGMENTS_UP[downstairsUpSegmentIndexNumber]; downstairsUpLedCounter < SEGMENTS_UP[downstairsUpSegmentIndexNumber + 1]; downstairsUpLedCounter++ ) {
      leds[downstairsUpLedCounter] = ledColour;  // write color to LED

      // debug stuff
      //Serial.println((String)"LED Counter: " + downstairsUpLedCounter);
      downstairsUpLedCounterExport++; // increase counter export variable to print it outside of for loop
    }
    downstairsUpSegmentIndexNumber++; // increase index number of up index to get the next element in array for next loop

    // debug stuff
    Serial.println((String)"SEGMENTS_UP current segment value: " + SEGMENTS_UP[downstairsUpSegmentIndexNumber]);
    //Serial.println((String) "downstairsUpSegmentIndexNumber: " + downstairsUpSegmentIndexNumber);
    Serial.println((String) "downstairsUpLedCounterExport: " + downstairsUpLedCounterExport);

    // add else if for state change when animation is complete and to start timing for turning LEDs off
  } else if ( downstairsUpSegmentIndexNumber > LED_SEGMENT_NUMBER) {
    // set state for animation to complete
    downstairsUpOnAnimationComplete = true;
    // only record Animation complete time once
    // set state for off animation to false to overwrite a set off trigger and allow off triggering again
    if ( downstairsUpOnAnimationCompleteTime == 0 ) {
      downstairsUpOnAnimationCompleteTime = millis();
      downstairsUpOffAnimationTriggered = false;
    }
  }
}

/* function for LED OFF animation of bottom to top */
void downstairsUpOff(CRGB ledColour) {
  if ( downstairsUpSegmentIndexNumber < LED_SEGMENT_NUMBER ) {
    // loop from beginning of array number to beginning of next segment (0 to 16 > 17 to 33 > 34 to 50 and so on)
    for (int downstairsUpLedCounter = SEGMENTS_UP[downstairsUpSegmentIndexNumber]; downstairsUpLedCounter < SEGMENTS_UP[downstairsUpSegmentIndexNumber + 1]; downstairsUpLedCounter++ ) {
      leds[downstairsUpLedCounter] = ledColour;  // write color to LED

      // debug stuff
      //Serial.println((String)"LED Counter: " + downstairsUpLedCounter);
      downstairsUpLedCounterExport++; // increase counter export variable to print it outside of for loop
    }
    downstairsUpSegmentIndexNumber++; // increase index number of up index to get the next element in array for next loop

    // debug stuff
    Serial.println((String)"Segment array element value " + SEGMENTS_UP[downstairsUpSegmentIndexNumber]);
    //Serial.println((String) "downstairsUpSegmentIndexNumber: " + downstairsUpSegmentIndexNumber);
    //Serial.println((String) "downstairsUpLedCounterExport: " + downstairsUpLedCounterExport);

    // set animation trigger state to true to inform loop about already started turning off routine
    downstairsUpOffAnimationTriggered = true;

    } else if ( downstairsUpSegmentIndexNumber > LED_SEGMENT_NUMBER ) {
    // reset variables to allow toggling of ON routine again
    downstairsUpOnAnimationCompleteTime = 0;
    downstairsUpOnAnimationComplete = false;
    downstairsUpSegmentIndexNumber = 0;
    //pirDownstairs = LOW;
    pirDownstairsTriggered = false;
    }
}

/* function for LED ON animation of top to bottom */
void upstairsDownOn(CRGB ledColour) {
  // debug stuff
  Serial.println("-------------------------------");
  Serial.println((String)"upstairsDownSegmentIndexNumber: " + upstairsDownSegmentIndexNumber);

  // execute the for loop only as much as we have Segments
  if ( upstairsDownSegmentIndexNumber < LED_SEGMENT_NUMBER ) {
    // loop from beginning of array number to beginning of next segment (289 > 272 to 271 > 255 and so on)
    for (upstairsDownLedCounter = SEGMENTS_DOWN[upstairsDownSegmentIndexNumber]; upstairsDownLedCounter <= SEGMENTS_DOWN[upstairsDownSegmentIndexNumber + 1]; upstairsDownLedCounter-- ) {
      leds[upstairsDownLedCounter] = ledColour;  // write color to LED

      // debug stuff
      Serial.println((String)"LED Counter: " + upstairsDownLedCounter);
    }
    upstairsDownSegmentIndexNumber++; // increase index number of up index to get the next element in array for next loop

    // debug stuff
    Serial.println((String)"SEGMENTS_DOWN current segment value: " + SEGMENTS_DOWN[upstairsDownSegmentIndexNumber]);
    Serial.println((String) "upstairsDownSegmentIndexNumber: " + upstairsDownSegmentIndexNumber);
    //Serial.println((String) "upstairsDownLedCounterExport: " + upstairsDownLedCounterExport);
    Serial.println("-------------------------------");

    // add else if for state change when animation is complete and to start timing for turning LEDs off
  } else if ( upstairsDownSegmentIndexNumber > LED_SEGMENT_NUMBER) {
    // set state for animation to complete
    upstairsDownOnAnimationComplete = true;
    // only record Animation complete time once
    // set state for off animation to false to overwrite a set off trigger and allow off triggering again
    if ( upstairsDownOnAnimationCompleteTime == 0 ) {
      upstairsDownOnAnimationCompleteTime = millis();
      upstairsDownOffAnimationTriggered = false;
    }
  }
}

// function for LED OFF animation of to to bottom
void upstairsDownOff(CRGB ledColour) {
  Serial.println("### off from top to bottom ###");
  if ( upstairsDownSegmentIndexNumber < LED_SEGMENT_NUMBER ) {
    // loop from beginning of array number to beginning of next segment (0 to 16 > 17 to 33 > 34 to 50 and so on)
    for (int upstairsDownLedCounter = SEGMENTS_UP[upstairsDownSegmentIndexNumber]; upstairsDownLedCounter < SEGMENTS_UP[upstairsDownSegmentIndexNumber + 1]; upstairsDownLedCounter-- ) {
      leds[upstairsDownLedCounter] = ledColour;  // write color to LED

      // debug stuff
      //Serial.println((String)"LED Counter: " + upstairsDownLedCounter);
      upstairsDownLedCounterExport++; // increase counter export variable to print it outside of for loop
    }
    upstairsDownSegmentIndexNumber++; // increase index number of up index to get the next element in array for next loop

    // debug stuff
    Serial.println((String)"Segment array element value " + SEGMENTS_UP[upstairsDownSegmentIndexNumber]);
    //Serial.println((String) "upstairsDownSegmentIndexNumber: " + upstairsDownSegmentIndexNumber);
    //Serial.println((String) "upstairsDownLedCounterExport: " + upstairsDownLedCounterExport);

    // set animation trigger state to true to inform loop about already started turning off routine
    upstairsDownOffAnimationTriggered = true;

  } else if ( upstairsDownSegmentIndexNumber > LED_SEGMENT_NUMBER ) {
    // reset variables to allow toggling of ON routine again
    upstairsDownOnAnimationCompleteTime = 0;
    upstairsDownOnAnimationComplete = false;
    upstairsDownSegmentIndexNumber = 0;
    //pirUpstairs = LOW;
    pirUpstairsTriggered = false;
  }
}
