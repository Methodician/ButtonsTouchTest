#include <Arduino.h>
#include <Adafruit_CircuitPlayground.h>

//  A0 p?
//  A1 p6
//  A2 p9
//  A3 p10
//  A4 p3
//  A5 p2
//  A6 p0
//  A7 p1



#define GREEN_WIRE     3 // A4
#define GREEN_PIXEL    0
#define WHITE_WIRE     2 // A5
#define WHITE_PIXEL    1
#define YELLOW_WIRE    0 // A6
#define YELLOW_PIXEL   3
#define BLUE_WIRE      1 // A7
#define BLUE_PIXEL     4

uint32_t currentPinMode = INPUT_PULLUP;
uint8_t pads[] = {GREEN_WIRE, YELLOW_WIRE, WHITE_WIRE, BLUE_WIRE};
uint8_t numberOfPads = sizeof(pads) / sizeof(uint8_t);

// questionable state management
// pulldown active = 0
// not activated   = 1
// pullup active   = 2
struct WireStates {
  unsigned int greenState;
  unsigned int whiteState;
  unsigned int yellowState;
  unsigned int blueState;
};

WireStates wireStates;

void printState(unsigned int state) {
  switch (state) {
    case 0:
      Serial.println("pulldown");
      break;
    case 1:
      Serial.println("not activated");
      break;
    case 2:
      Serial.println("pullup");
      break;
  }
}

void updateStates() {
  for (int i = 0; i < numberOfPads; i++) {
    int pin = pads[i];
    int state = digitalRead(pin);
    if (currentPinMode == INPUT_PULLUP) {
      if(state == LOW) {
        state = 2;
      } else {
        state = 1;
      }
    }
    if(currentPinMode == INPUT_PULLDOWN) {
      if(state == HIGH) {
        state = 0;
      } else {
        state = 1;
      }
    }
    switch (pin) {
      case GREEN_WIRE:
        wireStates.greenState = state;
        break;
      case WHITE_WIRE:
        wireStates.whiteState = state;
        break;
      case YELLOW_WIRE:
        wireStates.yellowState = state;
        break;
      case BLUE_WIRE:
        wireStates.blueState = state;
        break;
    }
  }
}

void printAllStates() {
  Serial.print("green: ");
  printState(wireStates.greenState);
  // Serial.println(digitalRead(GREEN_WIRE));
  Serial.print("white: ");
  printState(wireStates.whiteState);
  // Serial.println(digitalRead(WHITE_WIRE));
  Serial.print("yellow: ");
  printState(wireStates.yellowState);
  // Serial.println(digitalRead(YELLOW_WIRE));
  Serial.print("blue: ");
  printState(wireStates.blueState);
  // Serial.println(digitalRead(BLUE_WIRE));
}

// void setAllPullup() {
//     // state will be 1 and ground makes it 0
//     for (int i=0; i < numberOfPads; i++) {
//       pinMode(i, INPUT_PULLUP);
//     }
// }

// void setAllPulldown() {
//     // state will be 0 and hot makes it 1
//     for (int i=0; i < numberOfPads; i++) {
//       pinMode(i, INPUT_PULLDOWN);
//     }
// }

void setAllPinModes() {
  for (int i=0; i < numberOfPads; i++) {
    pinMode(i, currentPinMode);
  }
}

void setup() {

  Serial.begin(9600);
  CircuitPlayground.begin();

  // set all to not activated
  wireStates.greenState = 1;
  wireStates.whiteState = 1;
  wireStates.yellowState = 1;
  wireStates.blueState = 1;

  // CircuitPlayground.setPixelColor(GREEN_PIXEL, 0, 0, 255);
  // CircuitPlayground.setPixelColor(WHITE_PIXEL, 255, 0, 0);
  // CircuitPlayground.setPixelColor(YELLOW_PIXEL, 255, 155, 0);
  // CircuitPlayground.setPixelColor(BLUE_PIXEL, 255, 255, 0);

}

void loop() {

  currentPinMode = INPUT_PULLUP; // Goes from 1 to 0
  setAllPinModes();
  updateStates();
  Serial.println("PULLUP:");
  printAllStates();
  currentPinMode = INPUT_PULLDOWN; // Goes from 0 to 1
  setAllPinModes();
  updateStates();
  Serial.println("PULLDOWN:");
  printAllStates();

  delay(500);
}
