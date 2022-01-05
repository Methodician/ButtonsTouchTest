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

#define COMBO_OPPORTUNITY_TIME 800

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

class ComboCatch {
  public:
    ComboCatch(uint32_t opportunityDelay)
      : _opportunityDelay(opportunityDelay){}


    void begin() {
      _lastActiveTime = millis();
      _hasChanged = false;
      for(uint8_t i = 0; i < 4; i++) {
        _pinCatchers[i].begin();
      }
    }

    void catchPins() {
      for(uint8_t i = 0; i < 4; i++) {
        _pinCatchers[i].read();
      }
    }

    void recordActivation() {
      for(uint8_t i = 0; i < 4; i++) {
        if(_pinCatchers[i].isActive()) {
          _lastActiveTime = millis();
          break; // opposite is "continue"?
        }
      }
    }

    void reassignState(bool targetState [4], bool sourceState [4]) {
      for(uint8_t i = 0; i < 4; i++) {
        targetState[i] = sourceState[i];
      }
    }

    bool isStateEqual(bool state1 [4], bool state2 [4]) {
      for(uint8_t i = 0; i < 4; i++) {
        if(state1[i] != state2[i]) {
          return false;
        }
      }
      return true;
    }

    void read() {
      uint32_t ms = millis();

      for(uint8_t i = 0; i < 4; i++) {
        if(_pinCatchers[i].wasActive()) {
          _state[i] = true;
        }
      }
      
      if(ms - _lastActiveTime < _opportunityDelay) {
        _hasChanged = false;
      } else {
        _hasChanged = !isStateEqual(_lastState, _state );
        if(_hasChanged) {
          reassignState(_lastState, _state);
          _lastChangeTime = ms;
        }
      }
    }

    bool isActive(bool sampleState [4]) {
      return isStateEqual(_state, sampleState);
    }

    bool wasActive(bool sampleState [4]) {
      return _hasChanged && isStateEqual(_state, sampleState);
    }

    void resetState() {
      for(uint8_t i = 0; i < 4; i++) {
        _state[i] = false;
      }
      for(uint8_t i = 0; i < 4; i++) {
        _lastState[i] = false;
      }
    }

    void outputState() {
      // This might be more performant because it only loops after delay
      // if(_hasChanged) {
      //   for(uint8_t i = 0; i < 6; i++) {
      //     if(isActive(_keyStates[i])) {
      //       Serial.println(_keyStrokes[i]);
      //     }
      //   }
      // }
      for(uint8_t i = 0; i < sizeof(_keyStrokes)/sizeof(char); i++) {
        if(wasActive(_keyStates[i])) {
          Serial.println(_keyStrokes[i]);
        }
      }

      // Maybe only do this if one of the states above is true?
      if(millis() - _lastActiveTime > _opportunityDelay){
        resetState();
      }

    }


  private:
    // Search me with hash functions?
    bool _keyStates [15][4] = {
      {true, false, false, false},     // Yellow
      {false, true, false, false},     // Blue
      {false, false, true, false},     // White
      {false, false, false, true},     // Green
      {true, true, false, false},      // Yellow + Blue
      {true, false, true, false},      // Yellow + White
      {true, false, false, true},      // Yellow + Green
      {false, true, true, false},      // Blue + White
      {false, true, false, true},      // Blue + Green
      {false, false, true, true},      // White + Green
      {true, true, true, false},       // Yellow + Blue + White  
      {true, true, false, true},       // Yellow + Blue + Green
      {true, false, true, true},       // Yellow + White + Green
      {false, true, true, true},       // Blue + White + Green
      {true, true, true, true},        // Yellow + Blue + White + Green

    };
    const char _keyStrokes [15] = {'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o'};

    uint32_t _opportunityDelay;
    uint32_t _lastActiveTime;
    uint32_t _lastChangeTime;
    bool _state [4] = {false, false, false, false};
    bool _lastState [4] = {false, false, false, false};
    bool _hasChanged = false;
    PinCatch _pinCatchers[4] = {
      PinCatch(YELLOW_PIN),
      PinCatch(BLUE_PIN),
      PinCatch(WHITE_PIN),
      PinCatch(GREEN_PIN)
    };
};

ComboCatch comboCatch(COMBO_OPPORTUNITY_TIME);


void showLights() {
  // if(greenPin.isActive()) {
  //   CircuitPlayground.setPixelColor(GREEN_PIXEL, 0, 71, 2);
  // } 
  // if(greenPin.isInactive()) {
  //   CircuitPlayground.setPixelColor(GREEN_PIXEL, 0, 0, 0);
  // }

  // if(greenPin.isActive() && bluePin.isActive()) {
  //   CircuitPlayground.setPixelColor(BLUE_PIXEL, 0,  57, 59);
  //   CircuitPlayground.setPixelColor(GREEN_PIXEL, 0, 57, 59);

  // }
  // if(greenPin.isInactive() && bluePin.isInactive()) {
  //   CircuitPlayground.setPixelColor(BLUE_PIXEL, 0, 0, 0);
  //   CircuitPlayground.setPixelColor(GREEN_PIXEL, 0, 0, 0);
  // }

  // if(whitePin.isActive()) {
  //   CircuitPlayground.setPixelColor(WHITE_PIXEL, 48, 48, 48);
  // }
  // if(whitePin.isInactive()) {
  //   CircuitPlayground.setPixelColor(WHITE_PIXEL, 0, 0, 0);
  // }

  // if(yellowPin.isActive()) {
  //   CircuitPlayground.setPixelColor(YELLOW_PIXEL, 71, 69, 0);
  // }
  // if(yellowPin.isInactive()) {
  //   CircuitPlayground.setPixelColor(YELLOW_PIXEL, 0, 0, 0);
  // }

  // if(bluePin.isActive()) {
  //   CircuitPlayground.setPixelColor(BLUE_PIXEL, 0, 0, 71);
  // }
  // if(bluePin.isInactive()) {
  //   CircuitPlayground.setPixelColor(BLUE_PIXEL, 0, 0, 0);
  // }
}

void setup() {
  Serial.begin(9600);
  CircuitPlayground.begin();

  comboCatch.begin();
}

// Try interrupts some day
void loop() {
  comboCatch.catchPins();
  comboCatch.recordActivation();
  comboCatch.read();

  comboCatch.outputState();
}
