
#include <EEPROM.h>

enum class Mode { normal=1, test=2, printLog=3 }; // WARNING: CHANGING VALUES CORRUPT LOGS
const Mode mode = Mode::normal;

const int unlogged = -100;

// Sensor value ranges: 0-1023 (0-5V)
// 0 - no 5V from main power supply
// ~450 - white LED detected
// ~900 - red LED detected
// ~1023 - no LED detected

class Log {
private:
  struct EepromObject {
    int value;
    byte label;
    byte reserved;
  };

  static int messageCapacity() {
    return (EEPROM.length() - 8) / 4;
  }

  static int computeAddr(int totalMsg) {
    return 4 * (totalMsg % messageCapacity()) + 4;
  }

public:
  enum class Label { init=1, camera=2, sensorValue=3, led=4, sleep=5, loopForever=6 }; // WARNING: CHANGING VALUES CORRUPT LOGS
  
  static int getTotal() {
    int totalMsg;
    EEPROM.get(0, totalMsg);
    return totalMsg;
  }

  static int getMin() {
    int totalMsg = getTotal();
    int minMsg = max(0, totalMsg - messageCapacity());
    return minMsg;
  }

  static void get(int count, Label &label, int &value) {
    int addr = computeAddr(count);
    EepromObject eeprom;
    EEPROM.get(addr, eeprom);
    label = (Label) eeprom.label;
    value = eeprom.value;
  }

  static void printLogInformation(int count, Label label, int value) {
    if(count == unlogged) Serial.print("Unlogged: ");
    else { Serial.print(count); Serial.print(": "); }
    
    if((Log::Label) label == Label::init) {
      if((Mode) value == Mode::normal) Serial.println("Init normal mode");
      if((Mode) value == Mode::test) Serial.println("Init test mode");
      if((Mode) value == Mode::printLog) Serial.println("Init print log mode");
    }
    if((Log::Label) label == Label::camera) { Serial.print("Camera switched "); Serial.println(value ? "on" : "off"); }
    if((Log::Label) label == Label::sensorValue) { Serial.print("LED sensor: "); Serial.println(value); }
    if((Log::Label) label == Label::led) { Serial.print("LED detected: "); Serial.println(value ? "+++++" : "-----"); }
    if((Log::Label) label == Label::sleep) { Serial.print("Sleep for "); Serial.print(value); Serial.println(" ms"); }
    if((Log::Label) label == Label::loopForever) { Serial.println("Looping forever"); }
  }

  static void printLogEntry(int count) {
    Label label;
    int value;
    get(count, label, value);
    printLogInformation(count, label, value);
  }

  static void printLog() {
    int minMsg = getMin();
    int totalMsg = getTotal();
    Serial.print("printing "); Serial.print(minMsg); Serial.print(" - "); Serial.println(totalMsg);
    for(int i = minMsg; i < totalMsg; ++i) {
      printLogEntry(i);
    }
  }
  
  static void put(Label label, int value=-1) {
    if(mode != Mode::normal) {
      printLogInformation(unlogged, label, value);
      return;
    }
    
    int totalMsg = getTotal();
    EEPROM.get(0, totalMsg);
    int addr = computeAddr(totalMsg);
    
    EEPROM.put(addr, EepromObject{value, (byte) label, 0});
    EEPROM.put(0, totalMsg + 1);

    printLogEntry(totalMsg);
  }
};

void sleep(int ms) {
  Log::put(Log::Label::sleep, ms);
  delay(ms);
}

void loopForever() {
  Log::put(Log::Label::loopForever);
  
  if(mode == Mode::normal || mode == Mode::printLog) {
    while(true) {
    }
  } else if(mode == Mode::test) {
    // Switch camera off
    switchCamera(false);
    Serial.println("Resetting soon");
    sleep(12000);

    // Reset Arduino
    void(* resetFunc) (void) = 0;
    resetFunc();
  }
}

void switchCamera(bool state) {
  if(state) {
    pinMode(A5, INPUT);
  } else {
    pinMode(A5, OUTPUT);
    digitalWrite(A5, LOW);
  }

  Log::put(Log::Label::camera, (int) state);
}

void resetLog() { // WARNING: ERASES ALL DATA
  EEPROM.put(0, 0);
  Serial.println("All logs reset");
  while (true) {}
}

void setup() {
  Serial.begin(9600);
  Log::put(Log::Label::init, (int) mode);

  if(mode == Mode::printLog) {
    Log::printLog();
    loopForever();
  }
  
  switchCamera(true);
}

void loop() {
  sleep(18000);
  
  int sensorValue = analogRead(A1); // The lower the value the brighter the LED
  bool detected = sensorValue < 700;
  Log::put(Log::Label::sensorValue, sensorValue);
  Log::put(Log::Label::led, detected);
  if(detected) { // LED is red
    loopForever();
  } else {
    switchCamera(false);
    sleep(12000);
    switchCamera(true);
  }
}
