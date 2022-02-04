#include "MIDIUSB.h"
#include <avr/power.h>
#include <Adafruit_NeoPixel.h>
#define BUTTONPIN 8
#define PIN 2
#define BRIGHTNESS 100
#define myCHANNEL 0
#define myID 87
uint16_t NUMPIXELS = 5;
Adafruit_NeoPixel strip = Adafruit_NeoPixel(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);

void setleds(int r, int g, int b) {
  for (int led_index = 0; led_index < NUMPIXELS;  led_index++)
  {
    strip.setPixelColor(led_index, r, g, b);

  }
  strip.show();
}


void setup() {
  //  Serial.begin(38400);
  //  Serial.write("Starting...\n");
  //  Serial.end();

  pinMode(BUTTONPIN, INPUT_PULLUP);
  strip.setBrightness(BRIGHTNESS);
  strip.begin();
  strip.show(); // Initialize all pixels to 'off'
  setleds(255,255,0);
}

void controlChange(byte channel, byte control, byte value) {
  midiEventPacket_t event = {0x0B, 0xB0 | channel, control, value};
  MidiUSB.sendMIDI(event);
  MidiUSB.flush();
}

// Variables will change:
int ledState = LOW;         // the current state of the output pin
int buttonState;             // the current reading from the input pin
int lastButtonState = HIGH;   // the previous reading from the input pin
// the following variables are unsigned longs because the time, measured in
// milliseconds, will quickly become a bigger number than can be stored in an int.
unsigned long lastDebounceTime = 0;  // the last time the output pin was toggled
unsigned long debounceDelay = 100;    // the debounce time; increase if the output flickers
midiEventPacket_t rx;
void loop() {
  // read the state of the switch into a local variable:
  int reading = digitalRead(BUTTONPIN);

  // check to see if you just pressed the button
  // (i.e. the input went from LOW to HIGH), and you've waited long enough
  // since the last press to ignore any noise:

  // If the switch changed, due to noise or pressing:
  if (reading != lastButtonState) {
    // reset the debouncing timer
    lastDebounceTime = millis();
  }

  if ((millis() - lastDebounceTime) > debounceDelay) {
    // whatever the reading is at, it's been there for longer than the debounce
    // delay, so take it as the actual current state:

    // if the button state has changed:
    if (reading != buttonState) {
      buttonState = reading;

      // only toggle the LED if the new button state is HIGH
      if (buttonState == LOW) {
        controlChange(myCHANNEL, myID, 127);
      }
      else {
        controlChange(myCHANNEL, myID, 0);
      }

    }
  }

  // save the reading. Next time through the loop, it'll be the lastButtonState:
  lastButtonState = reading;

  rx = MidiUSB.read();
  // &&(rx.byte1 & 0xF == myCHANNEL))/
  if ((rx.header == 0xB) && (rx.byte2 == myID))//controlChange(rx.byte2, rx.byte3);
  {
    if (rx.byte3<64) {setleds(0,255,0);}
    else {setleds(255,0,0);}
    }
  delay(50);  
}
