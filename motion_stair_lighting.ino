#include <FastLED.h>
#if defined(FASTLED_VERSION) && (FASTLED_VERSION < 3001000)
#warning "Requires FastLED 3.1 or later; check github for latest code."
#endif

// ---------- Constants (wont change) ----------------------
#define DATA_PIN    7   //LED data pin
#define PIR1_PIN    5   //pin for PIR sensor 1
#define PIR2_PIN    6   //pin for PIR sensor 2
#define PHO1_PIN    0   //pin for photo sensor
#define LED_TYPE    WS2812B
#define COLOR_ORDER GRB
#define NUM_LEDS    289     //number of leds 
#define LED_SEGMENT_COUNT  17   //number of segment into which the strip should be divided. result needs to be a natural number
CRGB leds[NUM_LEDS];
#define BRIGHTNESS          25
#define FRAMES_PER_SECOND  120

const int ldrDark = 780;    // at which analog value we consider it to be dark
const int ldrBright = 300;  // at which analog value we consider it to be bright
const unsigned long SEGMENT_SWITCH_ON_DELAY = 400; //delay in ms between segment turn on
const unsigned long SEGMENT_SWITCH_OFF_DELAY = 300; //delay in ms between segment turn off
const unsigned long TIMEOUT = 10000;  //timeout after segments have been turned on

//const int segments[] = {0, 30, 60, 90, 120, 150, 180, 210, 240, 270};
//const int segments[] = {0, 60, 120, 180, 240};
const int segments[] = {0, 17, 34, 51, 68, 85, 102, 119, 136, 153, 170, 187, 204, 221, 238, 255, 272};

// ---------- VARIABLES (will change) -------------------
#define LED_SEGMENT_SIZE_CALC   NUM_LEDS/LED_SEGMENT_COUNT  //calculate led number per segment via segment and led count (number needs to be natural)
int LED_SEGMENT_SIZE = LED_SEGMENT_SIZE_CALC;
int PIR_DOWN = 0;
int PIR_UP = 0;
int pirState = LOW;
uint8_t gHue = 0; // rotating "base color"
int ldrValue = 0;

// ------- setup ----------
void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  pinMode(DATA_PIN, OUTPUT);
  pinMode(PIR1_PIN, INPUT);
  pinMode(PIR2_PIN, INPUT);

  //FastLED stuff
  delay(3000); // 3 second delay for recover
  //FastLED.delay(1000 / FRAMES_PER_SECOND);

  // tell FastLED about the LED strip configuration
  FastLED.addLeds<LED_TYPE, DATA_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);

  // set master brightness control
  FastLED.setBrightness(BRIGHTNESS);

  //initialisation
  randomColors();
  rainbow();
  delay(3000);
  FastLED.clear();
  FastLED.show();

}

void loop() {
  // put your main code here, to run repeatedly:
  ldrValue = analogRead(PHO1_PIN);
  //Serial.println((String)"LDR Value: " + ldrValue);
  PIR_DOWN = digitalRead(PIR1_PIN);
  PIR_UP = digitalRead(PIR2_PIN);



  if (ldrValue > ldrDark) {
    
    if (PIR_DOWN == HIGH) {
      Serial.println("Sensor Unten ausgeloest");
      downUp();
      if (pirState == LOW) {
        Serial.println("PIR_DOWN Motion detected!");
        pirState = HIGH;
      }
    } else if (PIR_UP == HIGH) {
      Serial.println("Sensor oben ausgeloest");
      upDown();
      if (pirState == LOW) {
        Serial.println("PIR_UP Motion detected!");
        pirState = HIGH;
      }
    } else {
      if (pirState == HIGH) {
        Serial.println("Motion ended!");
        // We only want to print on the output change, not state
        pirState = LOW;
      }
    }
  }

  //upDown();

}

void downUp() {
  // iterate through all segments, turning led from start to end of segment on
  for (int i = 0; i < LED_SEGMENT_COUNT; i++ ) {
    //Serial.println((String)"segment " + i + "; start: " + segments[i] + "; index i: " + i);
    for (int l = segments[i]; l < (segments[i] + LED_SEGMENT_SIZE); l++) {
      //Serial.println((String)"turning on led " + l + " of segment " + i + " with index l " + l);
      leds[l] = CRGB::Magenta;
    }
    delay(SEGMENT_SWITCH_ON_DELAY);
    FastLED.show();
  }

  //keep lights on for x and print message about it
  //Serial.println((String)"Wait " + TIMEOUT + " for turning off");
  delay(TIMEOUT);

  for (int i = 0; i < LED_SEGMENT_COUNT; i++ ) {
    //Serial.println((String)"segment " + i + "; start: " + segments[i] + "; index i: " + i);
    for (int l = segments[i]; l < (segments[i] + LED_SEGMENT_SIZE); l++) {
      //Serial.println((String)"turning off led " + l + " of segment " + i + " with index l " + l);
      leds[l] = CRGB::Black;
    }
    delay(SEGMENT_SWITCH_OFF_DELAY);
    FastLED.show();
  }
}

void upDown() {
  // iterate through all segments, turning led from end to start of segment on
  for (int i = (LED_SEGMENT_COUNT - 1); i >= 0 ; i-- ) {
    //Serial.println((String)"segment " + i + "; start: " + segments[i] + "; index i: " + i);
    for (int l = (segments[i] + LED_SEGMENT_SIZE); l >= segments[i]; l--) {
      //Serial.println((String)"turning on led " + l + " of segment " + i + " with index l " + l);
      leds[l] = CRGB::Magenta;
    }
    delay(SEGMENT_SWITCH_ON_DELAY);
    FastLED.show();
  }

  //keep lights on for x and print message about it
  Serial.println((String)"Wait " + TIMEOUT + " for turning off");
  delay(TIMEOUT);

  for (int i = (LED_SEGMENT_COUNT - 1); i >= 0; i-- ) {
    //Serial.println((String)"segment " + i + "; start: " + segments[i] + "; index i: " + i);
    for (int l = (segments[i] + LED_SEGMENT_SIZE); l >= segments[i] ; l--) {
      //Serial.println((String)"turning off led " + l + " of segment " + i + " with index l " + l);
      leds[l] = CRGB::Black;
    }
    delay(SEGMENT_SWITCH_OFF_DELAY);
    FastLED.show();
  }
}

//  //Turn all LEDs on
void allOn() {
  Serial.println((String)"Turn all LEDs on");
  for (int i = 0; i < NUM_LEDS; i++) {
    leds[i] = CRGB::Magenta;
  }
  FastLED.show();
}

void allOff() {
  //Turn all LEDs off
  Serial.println((String)"Turn all LEDs off");
  for (int i = 0; i < NUM_LEDS; i++) {
    leds[i] = CRGB::Black;
  }
  FastLED.show();
}

void randomColors() {
  for (int i = 0; i <= 600; i++) {
    leds[random(0, NUM_LEDS)] = CRGB( random(0, 255), random(0, 255), random(0, 255));
    leds[random(0, NUM_LEDS)] = CRGB( random(0, 255), random(0, 255), random(0, 255));
    leds[random(0, NUM_LEDS)] = CRGB( random(0, 255), random(0, 255), random(0, 255));
    FastLED.show();
  }
}

void rainbow()
{
  // FastLED's built-in rainbow generator
  fill_rainbow( leds, NUM_LEDS, gHue, 7);
  FastLED.show();
}
