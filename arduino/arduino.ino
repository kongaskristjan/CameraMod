
enum class Mode { normal, withReset };
const Mode mode = Mode::withReset;

constexpr int delayLength = 50; // ms
constexpr int totalIters = 15 /* seconds */ * (1000 /* ms per s */ / delayLength);
constexpr int stabIters1 = 5, stabIters2 = 2;
constexpr int minMeasurementTimeDiff = 15;
constexpr int minDelta = 14;

const int voltagePin = A5, cameraPin = A3;

short sensorValues[totalIters];
int iter = 0;

// Sensor value range: 0-1023 (0-5V)

struct Step {
  int iter = 0;
  int delta = 0;
};

void reset() {
  Serial.println("Resetting camera:");
  Serial.println("Switching camera off");
  digitalWrite(cameraPin, HIGH);
  delay(10000);
  Serial.println("Switching camera on");
  digitalWrite(cameraPin, LOW);

  iter = 0;
}

Step findMaxStep(int startIter, int stabIters) {
  Step best;
  for(int i = startIter; i < totalIters - stabIters; ++i) {
    int delta = sensorValues[i + stabIters] - sensorValues[i];
    if(delta < best.delta) {
      best.iter = i;
      best.delta = delta;
    }
  }
  return best;
}

bool detectCameraSuccess() {
  Step s1 = findMaxStep(0, stabIters1);
  Serial.println(String("1. step: iter: ") + s1.iter + "\t delta: " + s1.delta);
  Step s2 = findMaxStep(s1.iter + minMeasurementTimeDiff, stabIters2);
  Serial.println(String("2. step: iter: ") + s2.iter + "\t delta: " + s2.delta);
  if(s2.delta < -minDelta) {
    Serial.println("Camera success");
    return true;
  }
    Serial.println("Camera fail");
  return false;
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
    bool cameraSuccess = detectCameraSuccess();
    if(cameraSuccess) {
      Serial.println("Looping forever");
      while(true) delay(10000);
    } else {
      Serial.println("Resetting");
      reset();
    }
  } else {
    delay(delayLength);
    ++iter;
  }
}
