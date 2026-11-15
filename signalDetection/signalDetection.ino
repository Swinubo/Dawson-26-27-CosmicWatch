const int signalPin = 3;
const int receivePin = A0;

unsigned long initTime;

void setup() {
  Serial.begin(115200);
  pinMode(signalPin, OUTPUT);
}

void loop() {

  // HIGH for 5 ms
  digitalWrite(signalPin, HIGH);
  initTime = millis();

  while (millis() - initTime < 5) {
    detectSignal();
  }

  // LOW for 1000 ms
  digitalWrite(signalPin, LOW);
  initTime = millis();

  while (millis() - initTime < 1000) {
    detectSignal();
  }
}

void detectSignal() {

  int reading = analogRead(receivePin);

  if (reading > 120) {
    Serial.print("SIGNAL DETECTED! ");
    Serial.println(reading);
    delay(10);
  }
}