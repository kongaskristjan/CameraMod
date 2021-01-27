
enum class Mode { operate, testHardware }; // WARNING: CHANGING VALUES CORRUPT LOGS
const Mode mode = Mode::testHardware;

constexpr int delayLength = 100; // ms
constexpr int totalIters = 10 /* seconds */ * (1000 /* ms per s */ / delayLength);

const int voltagePin = A5, cameraPin = A3;

short sensorValues[totalIters];
int iter = 0;

// Sensor value ranges: 0-1023 (0-5V)

void setup() {
  Serial.begin(9600);
  if(mode == Mode::operate) Serial.println("Starting arduino: operate normally");
  if(mode == Mode::testHardware) Serial.println("Starting arduino: testing hardware");
  pinMode(cameraPin, OUTPUT);
}

void loop() {
  int sensorValue = analogRead(voltagePin);
  Serial.println(sensorValue);
  if(iter < totalIters) {
    sensorValues[iter] = sensorValue;
  }

  // Hardware testing mode
  if(mode == Mode::testHardware && iter == totalIters) {
    digitalWrite(cameraPin, HIGH);
  }

  delay(delayLength);
  ++iter;
}
