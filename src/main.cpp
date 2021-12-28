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
#define WHITE_WIRE     2 // A5
#define YELLOW_WIRE    0 // A6
#define BLUE_WIRE      1 // A7

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
    for (int i=0; i < numberOfPads; i++) {
      pinMode(i, INPUT_PULLUP);
    }
}

void setAllPulldown() {
    for (int i=0; i < numberOfPads; i++) {
      pinMode(i, INPUT_PULLDOWN);
    }
}

void setup() {
  // put your setup code here, to run once:
  // pinMode(1, INPUT_PULLUP);
  // pinMode(2, INPUT_PULLUP);

  Serial.begin(9600);
  CircuitPlayground.begin();

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
