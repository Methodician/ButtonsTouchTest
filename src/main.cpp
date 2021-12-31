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



#define GREEN_WIRE      3 // A4
#define GREEN_PIXEL     0
#define WHITE_WIRE      2 // A5
#define WHITE_PIXEL     1
#define YELLOW_WIRE     0 // A6
#define YELLOW_PIXEL    3
#define BLUE_WIRE       1 // A7
#define BLUE_PIXEL      4

#define DEBOUNCE_TIME  25

#define COMBO_OPPORTUNITY_TIME 200

class Btn {
  public:
    Btn(
      uint8_t pin,
      uint32_t dbTime=25,
      uint8_t puEnable=true,
      uint8_t invert=true)
    : _pin(pin),
      _dbTime(dbTime),
      _puEnable(puEnable),
      _invert(invert) {}

    void begin() {
      pinMode(_pin, _puEnable ? INPUT_PULLUP : INPUT_PULLDOWN);
      _state = digitalRead(_pin);
      if (_invert) _state = !_state;
      _time = millis();
      _lastState = _state;
      _changed = false;
      _lastChange = _time;
    }

    bool read() {
      uint32_t ms = millis();
      bool pinVal = digitalRead(_pin);
      if (_invert) pinVal = !pinVal;
      if (ms - _lastChange < _dbTime) {
        _changed = false;
      } else {
        _lastState = _state;
        _state = pinVal;
        _changed = (_state != _lastState);
        if (_changed) {
          _lastChange = ms;
        }
      }
      _time = ms;
      return _state;
    }

    bool isPressed() {
      return _state;
    }

    bool isReleased() {
      return !_state;
    }

    bool wasPressed() {
      return _changed && _state;
    }

    bool wasReleased() {
      return _changed && !_state;
    }

    bool pressedFor(uint32_t ms) {
      return _state && _time - _lastChange >= ms;
    }

    bool releasedFor(uint32_t ms) {
      return !_state && _time - _lastChange >= ms;
    }

    uint32_t lastChange() {
      return _lastChange;
    }

  private:
    uint8_t _pin;          // arduino pin number connected to button
    uint32_t _dbTime;      // debounce time (ms)
    bool _puEnable;        // internal pullup resistor enabled
    bool _invert;          // if true, interpret logic low as pressed, else interpret logic high as pressed
    bool _state;           // current button state, true=pressed
    bool _lastState;       // previous button state
    bool _changed;         // state changed since last read
    uint32_t _time;        // time of current state (ms from millis)
    uint32_t _lastChange; 
};

struct Buttons {
  Btn green;
  Btn white;
  Btn yellow;
  Btn blue;
};

Btn greenBtn(GREEN_WIRE, DEBOUNCE_TIME);
Btn whiteBtn(WHITE_WIRE, DEBOUNCE_TIME);
Btn yellowBtn(YELLOW_WIRE, DEBOUNCE_TIME);
Btn blueBtn(BLUE_WIRE, DEBOUNCE_TIME);


struct RecentClicks {
  boolean green;
  boolean white;
  boolean yellow;
  boolean blue;
};

RecentClicks recentClicks = {green: false, white: false, yellow: false, blue: false};

Buttons buttons = {
  greenBtn,
  whiteBtn,
  yellowBtn,
  blueBtn,
};

unsigned long lastPressMillis = 0;

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

void printMillis(){
  Serial.print(millis());
  Serial.print(" ");
}

void printClick() {
  if(recentClicks.green && recentClicks.blue) {
    Serial.println("Cyan");
  } else if(recentClicks.green && recentClicks.white) {
    Serial.println("Green and White");
  } else if(recentClicks.green) {
    Serial.println("Green");
  } else if(recentClicks.white) {
    Serial.println("White");
  } else if(recentClicks.blue) {
    Serial.println("Blue");
  }
  recentClicks = { false, false, false, false };
}

void showLights() {
  if(greenBtn.isPressed()) {
    CircuitPlayground.setPixelColor(GREEN_PIXEL, 0, 71, 2);
  } 
  if(greenBtn.isReleased()) {
    CircuitPlayground.setPixelColor(GREEN_PIXEL, 0, 0, 0);
  }

  if(greenBtn.isPressed() && blueBtn.isPressed()) {
    CircuitPlayground.setPixelColor(BLUE_PIXEL, 0,  57, 59);
    CircuitPlayground.setPixelColor(GREEN_PIXEL, 0, 57, 59);

  }
  if(greenBtn.isReleased() && blueBtn.isReleased()) {
    CircuitPlayground.setPixelColor(BLUE_PIXEL, 0, 0, 0);
    CircuitPlayground.setPixelColor(GREEN_PIXEL, 0, 0, 0);
  }

  if(whiteBtn.isPressed()) {
    CircuitPlayground.setPixelColor(WHITE_PIXEL, 48, 48, 48);
  }
  if(whiteBtn.isReleased()) {
    CircuitPlayground.setPixelColor(WHITE_PIXEL, 0, 0, 0);
  }

  if(yellowBtn.isPressed()) {
    CircuitPlayground.setPixelColor(YELLOW_PIXEL, 71, 69, 0);
  }
  if(yellowBtn.isReleased()) {
    CircuitPlayground.setPixelColor(YELLOW_PIXEL, 0, 0, 0);
  }

  if(blueBtn.isPressed()) {
    CircuitPlayground.setPixelColor(BLUE_PIXEL, 0, 0, 71);
  }
  if(blueBtn.isReleased()) {
    CircuitPlayground.setPixelColor(BLUE_PIXEL, 0, 0, 0);
  }
}

void updateLastPress() {
  if(
    greenBtn.isPressed() ||
    blueBtn.isPressed() ||
    whiteBtn.isPressed() ||
    yellowBtn.isPressed()
    ) {
    lastPressMillis = millis();
  }
}

void updateRecentClicks() {
  if(greenBtn.wasPressed()){
    recentClicks.green = true;
  }
  if(blueBtn.wasPressed()){
    recentClicks.blue = true;
  }
  if(whiteBtn.wasPressed()){
    recentClicks.white = true;
  }
  if(yellowBtn.wasPressed()){
    recentClicks.yellow = true;
  }
}

void plotButtons() {
  Serial.print(greenBtn.wasPressed());
  Serial.print(", ");
  Serial.print(whiteBtn.wasPressed());
  Serial.print(", ");
  Serial.print(yellowBtn.wasPressed());
  Serial.print(", ");
  Serial.print(blueBtn.wasPressed());
  Serial.println();
}

void setup() {
  lastPressMillis = millis();
  Serial.begin(9600);
  CircuitPlayground.begin();
  beginButtons();
}

void loop() {
  readButtons();
  updateRecentClicks();
  updateLastPress();
  showLights();
  unsigned long timeSinceLastPress = millis() - lastPressMillis;
  if(timeSinceLastPress > COMBO_OPPORTUNITY_TIME) {
    printClick();
  }
}
