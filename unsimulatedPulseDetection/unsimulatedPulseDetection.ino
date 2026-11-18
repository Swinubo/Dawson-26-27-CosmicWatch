const int signalPin = 3;
const int receivePin = A0;
int previousReading = 0;
int reading;
const float referenceVoltage = 1.1;
float expectedVoltageInput = 0.6;

float threshold = 1023*expectedVoltageInput/referenceVoltage;

unsigned long initTime;

void setup() {
  Serial.begin(115200);

  analogReference(INTERNAL);
  
  pinMode(signalPin, OUTPUT);
  previousReading = analogRead(receivePin);
}

void loop() {
    detectSignal();
}

void detectSignal() {

  reading = analogRead(receivePin);

  if (reading > threshold && previousReading < threshold) {
    Serial.print("SIGNAL/PULSE DETECTED! ");
    Serial.println(reading);
    delay(10);
  }
  else {
    Serial.println(reading);
  }

  previousReading = reading;
}

//The program purely detects a pulse. The voltage of the pulse is defined with the float expectedVoltageInput
//which appears right before setup().

//Currently on 1.1v reference. If a switch to 5v reference is necessary, comment out analog INTERNAL line
//in the set up and redefine reference as 5v manually.

//Threshold is expectedVoltageInput, so if more voltage is fed to arduino it will still detect,
//unless voltage is too high. Never feed more than 5v.