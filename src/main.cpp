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

#define COMBO_OPPORTUNITY_TIME 300

#define NUMBER_OF_COMBOS 15
#define NUMBER_OF_PINS 4

volatile bool yellowState = false;
volatile bool blueState = false;
volatile bool whiteState = false;
volatile bool greenState = false;

uint32_t lastKeyoutMs = 0;

void yellowIsr() {
  if(!digitalRead(YELLOW_PIN)) {
    yellowState = true;
  }
}
void blueIsr() {
  if(!digitalRead(BLUE_PIN)) {
    blueState = true;
  }
}
void whiteIsr() {
  if(!digitalRead(WHITE_PIN)) {
    whiteState = true;
  }
}
void greenIsr() {
  if(!digitalRead(GREEN_PIN)) {
    greenState = true;
  }
}

void resetStates() {
  yellowState = false;
  blueState = false;
  whiteState = false;
  greenState = false;
}


const bool keyStates [NUMBER_OF_COMBOS][NUMBER_OF_PINS] = {
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

bool isStateEqual(bool sampleState [NUMBER_OF_PINS], const bool testState [NUMBER_OF_PINS]) {
  for(uint8_t i = 0; i < NUMBER_OF_PINS; i++) {
    if(sampleState[i] != testState[i]) {
      return false;
    }
  }
  return true;
}

char printKey() {
  const char keyStrokes [NUMBER_OF_COMBOS] = 
    {'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o'}; 
  bool sampleState [NUMBER_OF_PINS] = {yellowState, blueState, whiteState, greenState};

  for(uint8_t i = 0; i < NUMBER_OF_COMBOS; i++) {
    if(isStateEqual(sampleState, keyStates[i])) {
      Serial.println(keyStrokes[i]);
    }
  }
}

void setup() {
  Serial.begin(9600);
  CircuitPlayground.begin();
  lastKeyoutMs = millis();
  pinMode(YELLOW_PIN, INPUT_PULLUP);
  pinMode(BLUE_PIN, INPUT_PULLUP);
  pinMode(WHITE_PIN, INPUT_PULLUP);
  pinMode(GREEN_PIN, INPUT_PULLUP);
  attachInterrupt(YELLOW_PIN, yellowIsr, CHANGE);
  attachInterrupt(BLUE_PIN, blueIsr, CHANGE);
  attachInterrupt(WHITE_PIN, whiteIsr, CHANGE);
  attachInterrupt(GREEN_PIN, greenIsr, CHANGE);
}

void loop() {
   uint32_t ms = millis();
  //  Serial.println(lastKeyoutMs);
  if(ms - lastKeyoutMs > COMBO_OPPORTUNITY_TIME) {
   Serial.println(ms - lastKeyoutMs);
    Serial.print(yellowState);
    Serial.print(blueState);
    Serial.print(whiteState);
    Serial.println(greenState);
  //   printKey();
    resetStates();
    lastKeyoutMs = ms;
  }
}

// class PinCatch {

//   public:
//     PinCatch(uint8_t pin, uint32_t dbTime = 25)
//       : _pin(pin), _dbTime(dbTime) {}
    
//     void begin() {
//       pinMode(_pin, INPUT_PULLUP);
//       bool pinVal = digitalRead(_pin);
//       _state = !pinVal;                   // Inverting because pullup
//       _lastState = _state;
//       _lastReadTime = millis();
//       _lastChangeTime = _lastReadTime;
//       _hasChanged = false;
//     }

//     bool read() {
//       uint32_t ms = millis();
//       bool pinVal = digitalRead(_pin);
//       if(ms - _lastChangeTime < _dbTime) {
//         _hasChanged = false;
//       } else {
//         _lastState = _state;
//         _state = !pinVal;               // Inverting because pullup
//         _hasChanged = _state != _lastState;
//         if(_hasChanged) {
//           _lastChangeTime = ms;
//         }
//       }
//       _lastReadTime = ms;
//       return _state;
//     }

//     bool isActive() {
//       return _state;
//     }

//     bool isInactive() {
//       return !_state;
//     }

//     bool wasActive() {
//       return _hasChanged && _state;
//     }

//     bool wasInactive() {
//       return _hasChanged && !_state;
//     }

//     bool lastChangeTime() {
//       return _lastChangeTime;
//     }

//   private:
//     uint8_t _pin;               // pin to catch
//     uint32_t _dbTime;           // debounce time
//     bool _state;                // current state; true = connected
//     bool _lastState;            // previous state; true = connected
//     bool _hasChanged;           // Whether state changed since last read
//     uint32_t _lastReadTime;     // time of last read
//     uint32_t _lastChangeTime;   // time of last change
// };

// class ComboCatch {
//   public:
//     ComboCatch(uint32_t opportunityDelay)
//       : _opportunityDelay(opportunityDelay){}


//     void begin() {
//       _lastActiveTime = millis();
//       _hasChanged = false;
//       for(uint8_t i = 0; i < NUMBER_OF_PINS; i++) {
//         _pinCatchers[i].begin();
//       }
//     }

//     void reassignState(bool targetState [NUMBER_OF_PINS], bool sourceState [NUMBER_OF_PINS]) {
//       for(uint8_t i = 0; i < NUMBER_OF_PINS; i++) {
//         targetState[i] = sourceState[i];
//       }
//     }

    
//     void resetState() {
//       for(uint8_t i = 0; i < NUMBER_OF_PINS; i++) {
//         _state[i] = false;
//         _lastState[i] = false;
//       }
//     }

//     bool isStateEqual(bool sampleState [NUMBER_OF_PINS], const bool testState [NUMBER_OF_PINS]) {
//       for(uint8_t i = 0; i < NUMBER_OF_PINS; i++) {
//         if(sampleState[i] != testState[i]) {
//           return false;
//         }
//       }
//       return true;
//     }

//     void read() {
//       uint32_t ms = millis();

//       for(uint8_t i = 0; i < NUMBER_OF_PINS; i++) {
//         _pinCatchers[i].read();
//         if(_pinCatchers[i].isActive()) {
//           _lastActiveTime = ms;
//         }
//         if(_pinCatchers[i].wasActive()) {
//           _state[i] = true;
//         }
//       }
      
//       // If opportunity time passed last loop, reset.
//       if(ms - _lastActiveTime > _opportunityDelay + 10) {
//         resetState();
//       }

//       // If within opportunity time, do nothing.
//       if(ms - _lastActiveTime < _opportunityDelay) {
//         _hasChanged = false;
//       } else {
//         // If opportunity time has passed, check for changes
//         _hasChanged = !isStateEqual(_lastState, _state );
//         if(_hasChanged) {
//           // Is this really necessary?
//           reassignState(_lastState, _state);
//         }
//       }
//     }

//     bool isActive(const bool testState [NUMBER_OF_PINS]) {
//       return isStateEqual(_state, testState);
//     }

//     bool wasActive(const bool testState [NUMBER_OF_PINS]) {
//       return _hasChanged && isActive(testState);
//     }

//   private:
//     uint32_t _opportunityDelay;
//     uint32_t _lastActiveTime;
//     bool _state [NUMBER_OF_PINS] = {false, false, false, false};
//     bool _lastState [NUMBER_OF_PINS] = {false, false, false, false};
//     bool _hasChanged = false;
//     PinCatch _pinCatchers[NUMBER_OF_PINS] = {
//       PinCatch(YELLOW_PIN),
//       PinCatch(BLUE_PIN),
//       PinCatch(WHITE_PIN),
//       PinCatch(GREEN_PIN)
//     };
// };

// ComboCatch comboCatch(COMBO_OPPORTUNITY_TIME);

// const bool keyStates [NUMBER_OF_COMBOS][NUMBER_OF_PINS] = {
//   {true, false, false, false},     // Yellow
//   {false, true, false, false},     // Blue
//   {false, false, true, false},     // White
//   {false, false, false, true},     // Green
//   {true, true, false, false},      // Yellow + Blue
//   {true, false, true, false},      // Yellow + White
//   {true, false, false, true},      // Yellow + Green
//   {false, true, true, false},      // Blue + White
//   {false, true, false, true},      // Blue + Green
//   {false, false, true, true},      // White + Green
//   {true, true, true, false},       // Yellow + Blue + White  
//   {true, true, false, true},       // Yellow + Blue + Green
//   {true, false, true, true},       // Yellow + White + Green
//   {false, true, true, true},       // Blue + White + Green
//   {true, true, true, true},        // Yellow + Blue + White + Green
// };

// char activeKey() {
//   // Search me with hash functions?

//   const char keyStrokes [NUMBER_OF_COMBOS] = 
//     {'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o'}; 

//   for(uint32_t i = 0; i < NUMBER_OF_COMBOS; i++) {
//     if(comboCatch.wasActive(keyStates[i])) {
//       return keyStrokes[i];
//     }
//   }
//   return NULL;
// }

// void showLights() {
//   if(comboCatch.isActive(keyStates[0])) {
//     CircuitPlayground.setPixelColor(YELLOW_PIXEL, 60, 60, 0);
//   } else {
//     CircuitPlayground.setPixelColor(YELLOW_PIXEL, 0, 0, 0);
//   }
//   if(comboCatch.isActive(keyStates[1])) {
//     CircuitPlayground.setPixelColor(BLUE_PIXEL, 0, 3, 87);
//   } else {
//     CircuitPlayground.setPixelColor(BLUE_PIXEL, 0, 0, 0);
//   }
//   if(comboCatch.isActive(keyStates[2])) {
//     CircuitPlayground.setPixelColor(WHITE_PIXEL, 40, 40, 40);
//   } else {
//     CircuitPlayground.setPixelColor(WHITE_PIXEL, 0, 0, 0);
//   }
//   if(comboCatch.isActive(keyStates[3])) {
//     CircuitPlayground.setPixelColor(GREEN_PIXEL, 0, 87, 3);
//   } else {
//     CircuitPlayground.setPixelColor(GREEN_PIXEL, 0, 0, 0);
//   }
//   if(comboCatch.isActive(keyStates[4])) {
//     CircuitPlayground.setPixelColor(YELLOW_PIXEL, 60, 60, 0);
//     CircuitPlayground.setPixelColor(BLUE_PIXEL, 0, 3, 87);
//   } else {
//     CircuitPlayground.setPixelColor(YELLOW_PIXEL, 0, 0, 0);
//     CircuitPlayground.setPixelColor(BLUE_PIXEL, 0, 0, 0);
//   }
//   if(comboCatch.isActive(keyStates[5])) {
//     CircuitPlayground.setPixelColor(YELLOW_PIXEL, 60, 60, 0);
//     CircuitPlayground.setPixelColor(WHITE_PIXEL, 40, 40, 40);
//   } else {
//     CircuitPlayground.setPixelColor(YELLOW_PIXEL, 0, 0, 0);
//     CircuitPlayground.setPixelColor(WHITE_PIXEL, 0, 0, 0);
//   }
//   if(comboCatch.isActive(keyStates[6])) {
//     CircuitPlayground.setPixelColor(YELLOW_PIXEL, 60, 60, 0);
//     CircuitPlayground.setPixelColor(GREEN_PIXEL, 0, 87, 3);
//   } else {
//     CircuitPlayground.setPixelColor(YELLOW_PIXEL, 0, 0, 0);
//     CircuitPlayground.setPixelColor(GREEN_PIXEL, 0, 0, 0);
//   }
//   if(comboCatch.isActive(keyStates[7])) {
//     CircuitPlayground.setPixelColor(BLUE_PIXEL, 0, 3, 87);
//     CircuitPlayground.setPixelColor(WHITE_PIXEL, 40, 40, 40);
//   } else {
//     CircuitPlayground.setPixelColor(BLUE_PIXEL, 0, 0, 0);
//     CircuitPlayground.setPixelColor(WHITE_PIXEL, 0, 0, 0);
//   }
//   if(comboCatch.isActive(keyStates[8])) {
//     CircuitPlayground.setPixelColor(BLUE_PIXEL, 0, 3, 87);
//     CircuitPlayground.setPixelColor(GREEN_PIXEL, 0, 87, 3);
//   } else {
//     CircuitPlayground.setPixelColor(BLUE_PIXEL, 0, 0, 0);
//     CircuitPlayground.setPixelColor(GREEN_PIXEL, 0, 0, 0);
//   }
//   if(comboCatch.isActive(keyStates[9])) {
//     CircuitPlayground.setPixelColor(WHITE_PIXEL, 40, 40, 40);
//     CircuitPlayground.setPixelColor(GREEN_PIXEL, 0, 87, 3);
//   } else {
//     CircuitPlayground.setPixelColor(WHITE_PIXEL, 0, 0, 0);
//     CircuitPlayground.setPixelColor(GREEN_PIXEL, 0, 0, 0);
//   }
//   if(comboCatch.isActive(keyStates[10])) {
//     CircuitPlayground.setPixelColor(YELLOW_PIXEL, 60, 60, 0);
//     CircuitPlayground.setPixelColor(BLUE_PIXEL, 0, 3, 87);
//     CircuitPlayground.setPixelColor(WHITE_PIXEL, 40, 40, 40);
//   } else {
//     CircuitPlayground.setPixelColor(YELLOW_PIXEL, 0, 0, 0);
//     CircuitPlayground.setPixelColor(BLUE_PIXEL, 0, 0, 0);
//     CircuitPlayground.setPixelColor(WHITE_PIXEL, 0, 0, 0);
//   }
//   if(comboCatch.isActive(keyStates[11])) {
//     CircuitPlayground.setPixelColor(YELLOW_PIXEL, 60, 60, 0);
//     CircuitPlayground.setPixelColor(BLUE_PIXEL, 0, 3, 87);
//     CircuitPlayground.setPixelColor(GREEN_PIXEL, 0, 87, 3);
//   } else {
//     CircuitPlayground.setPixelColor(YELLOW_PIXEL, 0, 0, 0);
//     CircuitPlayground.setPixelColor(BLUE_PIXEL, 0, 0, 0);
//     CircuitPlayground.setPixelColor(GREEN_PIXEL, 0, 0, 0);
//   }
//   if(comboCatch.isActive(keyStates[12])) {
//     CircuitPlayground.setPixelColor(YELLOW_PIXEL, 60, 60, 0);
//     CircuitPlayground.setPixelColor(BLUE_PIXEL, 0, 3, 87);
//     CircuitPlayground.setPixelColor(WHITE_PIXEL, 40, 40, 40);
//     CircuitPlayground.setPixelColor(GREEN_PIXEL, 0, 87, 3);
//   } else {
//     CircuitPlayground.setPixelColor(YELLOW_PIXEL, 0, 0, 0);
//     CircuitPlayground.setPixelColor(BLUE_PIXEL, 0, 0, 0);
//     CircuitPlayground.setPixelColor(WHITE_PIXEL, 0, 0, 0);
//     CircuitPlayground.setPixelColor(GREEN_PIXEL, 0, 0, 0);
//   }
//   if(comboCatch.isActive(keyStates[13])) {
//     CircuitPlayground.setPixelColor(YELLOW_PIXEL, 60, 60, 0);
//     CircuitPlayground.setPixelColor(WHITE_PIXEL, 40, 40, 40);
//     CircuitPlayground.setPixelColor(GREEN_PIXEL, 0, 87, 3);
//   } else {
//     CircuitPlayground.setPixelColor(YELLOW_PIXEL, 0, 0, 0);
//     CircuitPlayground.setPixelColor(WHITE_PIXEL, 0, 0, 0);
//     CircuitPlayground.setPixelColor(GREEN_PIXEL, 0, 0, 0);
//   }
//   if(comboCatch.isActive(keyStates[14])) {
//     CircuitPlayground.setPixelColor(YELLOW_PIXEL, 60, 60, 0);
//     CircuitPlayground.setPixelColor(WHITE_PIXEL, 40, 40, 40);
//     CircuitPlayground.setPixelColor(BLUE_PIXEL, 0, 3, 87);
//   } else {
//     CircuitPlayground.setPixelColor(YELLOW_PIXEL, 0, 0, 0);
//     CircuitPlayground.setPixelColor(WHITE_PIXEL, 0, 0, 0);
//     CircuitPlayground.setPixelColor(BLUE_PIXEL, 0, 0, 0);
//   }
// }

// void setup() {
//   Serial.begin(9600);
//   CircuitPlayground.begin();

//   comboCatch.begin();
// }

// // Try interrupts some day
// void loop() {
//   comboCatch.read();
//   showLights();
//   const char keyStroke = activeKey();
//   if(keyStroke != NULL) {
//     Serial.println(keyStroke);
//   }
// }
