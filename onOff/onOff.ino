
const int switchPin = A2;

void setup() {
  Serial.begin(9600);

  pinMode(switchPin, INPUT);
}

void loop() {
  int state = analogRead(switchPin);

  Serial.println(state);

  delay(100);
}
