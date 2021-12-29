#include <Arduino.h>
#include <Adafruit_CircuitPlayground.h>
#include <JC_Button.h>

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

#define DEBOUNCE_TIME  90

struct Buttons {
  Button green;
  Button white;
  Button yellow;
  Button blue;
};

struct WerePressed {
  bool green;
  bool white;
  bool yellow;
  bool blue;
};

struct ArePressed {
  bool green;
  bool white;
  bool yellow;
  bool blue;
};

struct AreReleased {
  bool green;
  bool white;
  bool yellow;
  bool blue;
};

Button greenBtn(GREEN_WIRE, DEBOUNCE_TIME);
Button whiteBtn(WHITE_WIRE, DEBOUNCE_TIME);
Button yellowBtn(YELLOW_WIRE, DEBOUNCE_TIME);
Button blueBtn(BLUE_WIRE, DEBOUNCE_TIME);
Buttons buttons = {
  greenBtn,
  whiteBtn,
  yellowBtn,
  blueBtn
};

WerePressed werePressed = {false, false, false, false};
ArePressed arePressed = {false, false, false, false};
AreReleased areReleased = {false, false, false, false};

void beginButtons() {
  greenBtn.begin();
  whiteBtn.begin();
  yellowBtn.begin();
  blueBtn.begin();
}

void readButtons() {
  greenBtn.read();
  whiteBtn.read();
  yellowBtn.read();
  blueBtn.read();
}

void updateStates() {
  werePressed = {
    greenBtn.wasPressed(),
    whiteBtn.wasPressed(),
    yellowBtn.wasPressed(),
    blueBtn.wasPressed()
  };

  arePressed = {
    greenBtn.isPressed(),
    whiteBtn.isPressed(),
    yellowBtn.isPressed(),
    blueBtn.isPressed()
  };

  areReleased = {
    greenBtn.wasReleased(),
    whiteBtn.wasReleased(),
    yellowBtn.wasReleased(),
    blueBtn.wasReleased()
  };
}

void showLights() {
  if(arePressed.green) {
    CircuitPlayground.setPixelColor(GREEN_PIXEL, 0, 71, 2);
  } 
  if(areReleased.green) {
    CircuitPlayground.setPixelColor(GREEN_PIXEL, 0, 0, 0);
  }
  if(werePressed.green) {
    Serial.println("green");
  }

  if(arePressed.white) {
    CircuitPlayground.setPixelColor(WHITE_PIXEL, 48, 48, 48);
  }
  if(areReleased.white) {
    CircuitPlayground.setPixelColor(WHITE_PIXEL, 0, 0, 0);
  }
  if(werePressed.white) {
    Serial.println("white");
  }

  if(arePressed.yellow) {
    CircuitPlayground.setPixelColor(YELLOW_PIXEL, 71, 69, 0);
  }
  if(areReleased.yellow) {
    CircuitPlayground.setPixelColor(YELLOW_PIXEL, 0, 0, 0);
  }
  if(werePressed.yellow) {
    Serial.println("yellow");
  }

  if(arePressed.blue) {
    CircuitPlayground.setPixelColor(BLUE_PIXEL, 0, 0, 71);
  }
  if(areReleased.blue) {
    CircuitPlayground.setPixelColor(BLUE_PIXEL, 0, 0, 0);
  }
  if(werePressed.blue) {
    Serial.println("blue");
  }
}

void setup() {
  Serial.begin(9600);
  CircuitPlayground.begin();
  beginButtons();
}

void loop() {

  readButtons();
  updateStates();
  showLights();

}
