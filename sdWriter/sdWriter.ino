
#include <SPI.h>
#include <SD.h>

const int switchPin = 2;
const int chipSelect = 10;

File dataFile;

unsigned long startTime;
unsigned long previousTime = 0;

const unsigned long interval = 50;   // 0.05 seconds
const unsigned long duration = 5000; // 5 seconds

void setup() {
  Serial.begin(9600);

  pinMode(switchPin, INPUT_PULLUP);

  Serial.println("Initializing SD card...");

  if (!SD.begin(chipSelect)) {
    Serial.println("SD card initialization failed!");
    return;
  }

  Serial.println("SD card initialized!");

  // Open data.txt for writing
  dataFile = SD.open("data.txt", FILE_WRITE);

  if (!dataFile) {
    Serial.println("Failed to open data.txt!");
    return;
  }

  Serial.println("Recording started!");

  startTime = millis();

  // Record switch state every 0.05 seconds
  while (millis() - startTime < duration) {

    unsigned long currentTime = millis() - startTime;

    if (currentTime - previousTime >= interval) {

      previousTime += interval;

      int switchState = digitalRead(switchPin);

      dataFile.print(currentTime / 1000.0, 2);
      dataFile.print(" seconds: ");

      if (switchState == LOW) {
        dataFile.println("ON");
      } else {
        dataFile.println("OFF");
      }
    }
  }

  dataFile.close();

  Serial.println("Recording finished!");
}

void loop() {
}
