const int signalPin = 3;
const int receivePin = A0;
int previousReading = 0;
int reading;

unsigned long initTime;

void setup() {
  Serial.begin(115200);
  pinMode(signalPin, OUTPUT);
  previousReading = analogRead(receivePin);
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

  reading = analogRead(receivePin);

  if (reading > 120 && previousReading < 120) {
    Serial.print("SIGNAL/PULSE DETECTED! ");
    Serial.println(reading);
    delay(10);
  }
  else {
    Serial.println(reading);
  }

  previousReading = reading;
}