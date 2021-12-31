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

class PinCatch {

  public:
    PinCatch(uint8_t pin, uint32_t dbTime = 25)
      : _pin(pin), _dbTime(dbTime) {}
    
    void begin() {
      pinMode(_pin, INPUT_PULLUP);
      bool pinVal = digitalRead(_pin);
      _state = !pinVal;                   // Inverting because pullup
      _lastState = _state;
      _lastReadTime = millis();
      _lastChangeTime = _lastReadTime;
      _hasChanged = false;
    }

    bool read() {
      uint32_t ms = millis();
      bool pinVal = digitalRead(_pin);
      if(ms - _lastChangeTime < _dbTime) {
        _hasChanged = false;
      } else {
        _lastState = _state;
        _state = !pinVal;               // Inverting because pullup
        _hasChanged = _state != _lastState;
        if(_hasChanged) {
          _lastChangeTime = ms;
        }
      }
      _lastReadTime = ms;
      return _state;
    }

    bool isActive() {
      return _state;
    }

    bool isInactive() {
      return !_state;
    }

    bool wasActive() {
      return _hasChanged && _state;
    }

    bool wasInactive() {
      return _hasChanged && !_state;
    }

    bool lastChangeTime() {
      return _lastChangeTime;
    }

  private:
    uint8_t _pin;               // pin to catch
    uint32_t _dbTime;           // debounce time
    bool _state;                // current state; true = connected
    bool _lastState;            // previous state; true = connected
    bool _hasChanged;           // Whether state changed since last read
    uint32_t _lastReadTime;     // time of last read
    uint32_t _lastChangeTime;   // time of last change
};
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

PinCatch greenPin(GREEN_WIRE);
PinCatch whitePin(WHITE_WIRE);
PinCatch yellowPin(YELLOW_WIRE);
PinCatch bluePin(BLUE_WIRE);

struct RecentStates {
  bool green;
  bool white;
  bool yellow;
  bool blue;
};

RecentStates recentStates = {green: false, white: false, yellow: false, blue: false};

unsigned long lastActiveTime = 0;

void beginButtons() {
  greenBtn.begin();
  whiteBtn.begin();
  yellowBtn.begin();
  blueBtn.begin();
}

void startPins() {
  greenPin.begin();
  whitePin.begin();
  yellowPin.begin();
  bluePin.begin();
}

void readButtons() {
  greenBtn.read();
  whiteBtn.read();
  yellowBtn.read();
  blueBtn.read();
}

void readPins() {
  greenPin.read();
  whitePin.read();
  yellowPin.read();
  bluePin.read();
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

// Could be replaced with a function that returns the key stroke based on combos
void printComboState() {
  if(recentStates.green && recentStates.white && recentStates.blue) {
    Serial.println("Green, White, and Blue");
  }
  if(recentStates.green && recentStates.blue){
    Serial.println("Green and Blue");
  } else if(recentStates.green && recentStates.white){
    Serial.println("Green and White");
  } else if(recentStates.green){
    Serial.println("Green");
  } else if(recentStates.white){
    Serial.println("White");
  } else if(recentStates.blue){
    Serial.println("Blue");
  }

  recentStates = { false, false, false, false };
}

void showLights() {
  if(greenPin.isActive()) {
    CircuitPlayground.setPixelColor(GREEN_PIXEL, 0, 71, 2);
  } 
  if(greenPin.isInactive()) {
    CircuitPlayground.setPixelColor(GREEN_PIXEL, 0, 0, 0);
  }

  if(greenPin.isActive() && bluePin.isActive()) {
    CircuitPlayground.setPixelColor(BLUE_PIXEL, 0,  57, 59);
    CircuitPlayground.setPixelColor(GREEN_PIXEL, 0, 57, 59);

  }
  if(greenPin.isInactive() && bluePin.isInactive()) {
    CircuitPlayground.setPixelColor(BLUE_PIXEL, 0, 0, 0);
    CircuitPlayground.setPixelColor(GREEN_PIXEL, 0, 0, 0);
  }

  if(whitePin.isActive()) {
    CircuitPlayground.setPixelColor(WHITE_PIXEL, 48, 48, 48);
  }
  if(whitePin.isInactive()) {
    CircuitPlayground.setPixelColor(WHITE_PIXEL, 0, 0, 0);
  }

  if(yellowPin.isActive()) {
    CircuitPlayground.setPixelColor(YELLOW_PIXEL, 71, 69, 0);
  }
  if(yellowPin.isInactive()) {
    CircuitPlayground.setPixelColor(YELLOW_PIXEL, 0, 0, 0);
  }

  if(bluePin.isActive()) {
    CircuitPlayground.setPixelColor(BLUE_PIXEL, 0, 0, 71);
  }
  if(bluePin.isInactive()) {
    CircuitPlayground.setPixelColor(BLUE_PIXEL, 0, 0, 0);
  }
}

void updateLastActiveTime() {
  if(
    greenPin.isActive() ||
    bluePin.isActive() ||
    whitePin.isActive() ||
    yellowPin.isActive()
    ) {
    lastActiveTime = millis();
  }
}

void updateRecentStates() {
  if(greenPin.wasActive()){
    recentStates.green = true;
  }
  if(bluePin.wasActive()){
    recentStates.blue = true;
  }
  if(whitePin.wasActive()){
    recentStates.white = true;
  }
  if(yellowPin.wasActive()){
    recentStates.yellow = true;
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
  lastActiveTime = millis(); // try deleting this line
  Serial.begin(9600);
  CircuitPlayground.begin();
  startPins();
}

void loop() {
  readPins();
  updateRecentStates();
  updateLastActiveTime();
  showLights();
  unsigned long timeSinceLastPress = millis() - lastActiveTime;
  if(timeSinceLastPress > COMBO_OPPORTUNITY_TIME) {
    printComboState();
  }
}
