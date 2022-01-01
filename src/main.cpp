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



#define YELLOW_PIN     0 // A6
#define BLUE_PIN       1 // A7
#define WHITE_PIN      2 // A5
#define GREEN_PIN      3 // A4
#define GREEN_PIXEL     0
#define WHITE_PIXEL     1
#define YELLOW_PIXEL    3
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

PinCatch greenPin(GREEN_PIN);
PinCatch whitePin(WHITE_PIN);
PinCatch yellowPin(YELLOW_PIN);
PinCatch bluePin(BLUE_PIN);

struct RecentStates {
  bool green;
  bool white;
  bool yellow;
  bool blue;
};

RecentStates recentStates = {green: false, white: false, yellow: false, blue: false};


class ComboCatch {
  public:
    ComboCatch(uint32_t opportunityDelay)
      : _opportunityDelay(opportunityDelay){}

    class ComboState {
      public:
        ComboState() {
        }

        void reset() {
          for(int i = 0; i < 4; i++) {
            _states[i] = false;
          }
        }

        void setValue(const uint8_t index, const bool value) {
          _states[index] = value;
        }

        bool getValue(const uint8_t index) {
          return _states[index];
        }

      private:
        bool _states [4] = {false, false, false, false};
    };

    bool getValue(const uint8_t index) {
      return _state.getValue(index);
    }

    void begin() {
      _lastActiveTime = millis();
    }


    // Let's keep it to a single purpose
    void recordActivation() {
      if(
        greenPin.isActive() ||
        whitePin.isActive() ||
        yellowPin.isActive() ||
        bluePin.isActive()
        ){
        _lastActiveTime = millis();
      }
    }

    ComboState updateStates() {
      if(greenPin.wasActive()) {
        _state.setValue(GREEN_PIN, true);
      }
      if(whitePin.wasActive()) {
        _state.setValue(WHITE_PIN, true);
      }
      if(yellowPin.wasActive()) {
        _state.setValue(YELLOW_PIN, true);
      }
      if(bluePin.wasActive()) {
        _state.setValue(BLUE_PIN, true);
      }
      return _state;
    }

  private:
    uint32_t _opportunityDelay;
    uint32_t _lastActiveTime;
    ComboState _state;
    ComboState _lastState;
};

ComboCatch comboCatch(COMBO_OPPORTUNITY_TIME);

unsigned long lastActiveTime = 0;

void startPins() {
  greenPin.begin();
  whitePin.begin();
  yellowPin.begin();
  bluePin.begin();
}

void readPins() {
  greenPin.read();
  whitePin.read();
  yellowPin.read();
  bluePin.read();
}

// Could be replaced with a function that returns the key stroke based on combos
void printComboState() {
  if(comboCatch.getValue(GREEN_PIN) && comboCatch.getValue(WHITE_PIN) && comboCatch.getValue(BLUE_PIN)) {
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

void setup() {
  lastActiveTime = millis(); // try deleting this line
  Serial.begin(9600);
  CircuitPlayground.begin();
  startPins();
}

// Try interrupts some day
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
