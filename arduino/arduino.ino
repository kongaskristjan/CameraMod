
enum class Mode { normal, test };

const Mode mode = Mode::test;

void loopForever() {
  Serial.println("Loop forever");
  if(mode == Mode::normal) {
    while(true) {
      delay(1000);
    }
  } else if(mode == Mode::test) {
    // Switch camera off
    switchCamera(false);
    Serial.println("Resetting soon");
    delay(12000);

    // Reset Arduino
    void(* resetFunc) (void) = 0;
    resetFunc();
  }
}

void switchCamera(bool state) {
  if(state) {
    pinMode(A5, INPUT);
    Serial.println("Camera switched on");
  } else {
    pinMode(A5, OUTPUT);
    digitalWrite(A5, LOW);
    Serial.println("Camera switched off");
  }
}

void setup() {
  Serial.begin(9600);

  // Camera is on by default
  if(mode == Mode::normal) {
    Serial.println("Initializing Arduino (normal mode)");
  } else if(mode == Mode::test) {
    Serial.println("Initializing Arduino (test mode)");
  }
  switchCamera(true);
}

void loop() {
  delay(18000);
  
  int sensorValue = analogRead(A1); // The lower the value the brighter the LED
  if(sensorValue > 700) { // LED is red
    Serial.println("Led not detected --- ");
    Serial.println(sensorValue);
    
    switchCamera(false);
    delay(12000);
    switchCamera(true);
  } else {
    Serial.println("Led detected +++ ");
    Serial.println(sensorValue);

    if(sensorValue < 200) {
      Serial.println("WARNING: Something is probably wrong, sensorValue should be >= 200 (most likely there is no 5V from main usb power)");
    }

    loopForever();
  }
}
