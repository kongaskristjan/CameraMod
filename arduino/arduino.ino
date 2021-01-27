
enum class Mode { normal, withReset };
const Mode mode = Mode::withReset;

constexpr int delayLength = 50; // ms
constexpr int totalIters = 15 /* seconds */ * (1000 /* ms per s */ / delayLength);

const int voltagePin = A5, cameraPin = A3;

short sensorValues[totalIters];
int iter = 0;

// Sensor value range: 0-1023 (0-5V)

void reset() {
  Serial.println("Resetting camera:");
  Serial.println("Switching camera off");
  digitalWrite(cameraPin, HIGH);
  delay(10000);
  Serial.println("Switching camera on");
  digitalWrite(cameraPin, LOW);

  iter = 0;
}

bool detectCameraFailure() {
  return true;
}

void setup() {
  Serial.begin(9600);
  Serial.println("");
  if(mode == Mode::normal) Serial.println("Starting arduino: operate normally");
  if(mode == Mode::withReset) Serial.println("Starting arduino: operate normally, but reset (for testing)");

  pinMode(cameraPin, OUTPUT);
  
  if(mode == Mode::withReset) {
    reset();
  }
}

void loop() {
  if(iter < totalIters) {
    int sensorValue = analogRead(voltagePin);
    Serial.println(sensorValue);
    sensorValues[iter] = sensorValue;
  }

  if(iter == totalIters) {
    bool cameraFailed = detectCameraFailure();
    if(cameraFailed) {
      reset();
    } else {
      Serial.println("Camera successful: Looping forever");
      // Loop forever
      while(true) delay(10000);
    }
  } else {
    delay(delayLength);
    ++iter;
  }
}
