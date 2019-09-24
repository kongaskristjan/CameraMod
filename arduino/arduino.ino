
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
}

void loop() {
  delay(18000);
  
  int sensorValue = analogRead(A1); // The lower the value the brighter the LED
  if(sensorValue > 700) { // LED is red
    Serial.println("Led not detected --- ");
    Serial.println(sensorValue);
    
    switchCamera(false);
    delay(18000);
    switchCamera(true);
  } else {
    Serial.println("Led detected +++ ");
    Serial.println(sensorValue);

    if(sensorValue < 200) {
      Serial.println("WARNING: Something is probably wrong, sensorValue should be >= 200 (most likely there is no 5V from main usb power)");
    }

    Serial.println("Loop forever");
    while(true) { delay(1000); }
  }
}
