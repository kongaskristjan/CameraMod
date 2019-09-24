
void plotDelay(int ms, int step=50) {
  for(int t = 0; t < ms; t += step) {
    delay(step);
    
    int sensorValue = analogRead(A1);
    Serial.println(sensorValue);
  }
}



void setup() {
  Serial.begin(9600);
}

void loop() {
  pinMode(A5, OUTPUT);
  digitalWrite(A5, LOW);
  plotDelay(15000);
  
  pinMode(A5, INPUT);
  plotDelay(15000);
}
