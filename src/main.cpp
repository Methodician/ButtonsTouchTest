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

void setup() {
  // put your setup code here, to run once:
  // pinMode(1, INPUT_PULLUP);
  // pinMode(2, INPUT_PULLUP);

  Serial.begin(9600);
  CircuitPlayground.begin();

  pinMode(GREEN_WIRE, INPUT_PULLUP);
  pinMode(WHITE_WIRE, INPUT_PULLDOWN);
  pinMode(YELLOW_WIRE, INPUT_PULLUP);
  pinMode(BLUE_WIRE, INPUT_PULLDOWN);

}

void loop() {
  // put your main code here, to run repeatedly:
  // Serial.println(digitalRead(1));
  // Serial.println(digitalRead(2));
  // Serial.println(digitalRead(3));

// int reading = digitalRead(3);

// Serial.println(reading);

Serial.print("green: ");
Serial.print(digitalRead(GREEN_WIRE));
Serial.print(" white: ");
Serial.print(digitalRead(WHITE_WIRE));
Serial.print(" yellow: ");
Serial.print(digitalRead(YELLOW_WIRE));
Serial.print(" blue: ");
Serial.println(digitalRead(BLUE_WIRE));

delay(500);
}