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

#define DEBOUNCE_TIME  90

class Btn {
  public:
    Btn(uint8_t pin, uint32_t dbTime=25, uint8_t puEnable=true, uint8_t invert=true)
    : _pin(pin), _dbTime(dbTime), _puEnable(puEnable), _invert(invert) {}

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

Btn greenBtn(GREEN_WIRE, DEBOUNCE_TIME);
Btn whiteBtn(WHITE_WIRE, DEBOUNCE_TIME);
Btn yellowBtn(YELLOW_WIRE, DEBOUNCE_TIME);
Btn blueBtn(BLUE_WIRE, DEBOUNCE_TIME);
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
    greenBtn.isReleased(),
    whiteBtn.isReleased(),
    yellowBtn.isReleased(),
    blueBtn.isReleased()
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

  if(arePressed.green && arePressed.blue) {
    CircuitPlayground.setPixelColor(GREEN_PIXEL, 0, 57, 59);
    CircuitPlayground.setPixelColor(BLUE_PIXEL, 0, 57, 59);
  }

  if(areReleased.green && areReleased.blue) {
    CircuitPlayground.setPixelColor(GREEN_PIXEL, 0, 0, 0);
    CircuitPlayground.setPixelColor(BLUE_PIXEL, 0, 0, 0);
  }

  if(werePressed.green && werePressed.blue) {
    Serial.println("cyan");
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
  }

  // showLights();

}
