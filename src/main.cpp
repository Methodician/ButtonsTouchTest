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

uint8_t pads[] = {GREEN_WIRE, YELLOW_WIRE, WHITE_WIRE, BLUE_WIRE};
uint8_t numberOfPads = sizeof(pads) / sizeof(uint8_t);

void printAllStates() {
  Serial.print("green: ");
  Serial.print(digitalRead(GREEN_WIRE));
  Serial.print(" white: ");
  Serial.print(digitalRead(WHITE_WIRE));
  Serial.print(" yellow: ");
  Serial.print(digitalRead(YELLOW_WIRE));
  Serial.print(" blue: ");
  Serial.println(digitalRead(BLUE_WIRE));
}

void setAllPullup() {
    // state will be 1 and ground makes it 0
    for (int i=0; i < numberOfPads; i++) {
      pinMode(i, INPUT_PULLUP);
    }
}

void setAllPulldown() {
    // state will be 0 and hot makes it 1
    for (int i=0; i < numberOfPads; i++) {
      pinMode(i, INPUT_PULLDOWN);
    }
}

void setup() {

  Serial.begin(9600);
  CircuitPlayground.begin();

  // CircuitPlayground.setPixelColor(0, 0, 0, 255);
  // CircuitPlayground.setPixelColor(4, 255, 0, 0);
  // CircuitPlayground.setPixelColor(5, 255, 155, 0);
  // CircuitPlayground.setPixelColor(6, 255, 255, 0);

}

void loop() {

  setAllPullup();
  Serial.println("on pullup:");
  printAllStates();
  setAllPulldown();
  Serial.println("on pulldown:");
  printAllStates();

  delay(500);
}
