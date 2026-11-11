
#include <SPI.h>
#include <SD.h>

#include "i2c.h"
#include <Wire.h>

#include "i2c_BMP280.h"
BMP280 bmp280;

const int switchPin = 2;
const int chipSelect = 10;

File dataFile;

unsigned long startTime;
unsigned long previousTime = 0;

const unsigned long interval = 50;   // 0.05 seconds
const unsigned long duration = 5000; // 5 seconds

struct BMPValues {
    float meters;
    float pascal;
    float temperature;
};

float meters;
bool executedBaseline = false;
float baseline = 0;

void setup() {
  Serial.begin(115200);

  initBMP();

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

      dataFile.print(currentTime / 1000.0, 2);
      dataFile.print(" seconds: ");

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
  BMPValues vals = getVals();

  Serial.println(vals.meters);
}

void initBMP(){ //init serial before running ts
  

    Serial.print("Probe BMP280: ");
    if (bmp280.initialize()) Serial.println("Sensor found");
    else
    {
        Serial.println("Sensor missing");
        while (1) {}
    }

    /*// onetime-measure:
    bmp280.setEnabled(0);
    bmp280.triggerMeasurement();

    unsigned long startTime = millis();

    float meters;
    while (millis() - startTime < 5000) {
        bmp280.awaitMeasurement();        
        bmp280.getAltitude(meters);
        bmp280.triggerMeasurement();
        Serial.println(meters);
    }
    float initMeters = meters;
    Serial.print(initMeters);*/
}

BMPValues getVals() {

    bmp280.awaitMeasurement();

    float temperature;
    bmp280.getTemperature(temperature);

    float pascal;
    bmp280.getPressure(pascal);

    bmp280.getAltitude(meters);

    if (millis() < 7000 && !executedBaseline) {

        baseline = meters;
        executedBaseline = true;

        Serial.print("Baseline: ");
        Serial.println(baseline);
    }
    Serial.print("Baseline: ");
    Serial.println(baseline);

    bmp280.triggerMeasurement();

    meters -= baseline;

    if (meters < 0)
    {
      meters = 0;
    } 

    return {meters, pascal, temperature};
}

