
#include <SPI.h>
#include <SD.h>

#include "i2c.h"
#include <Wire.h>

#include "i2c_BMP280.h"
BMP280 bmp280;

const int switchPin = 2;
const int chipSelect = 10;

File dataRaw;
File dataInterpreted;

unsigned long startTime;
unsigned long previousTime = 0;

float initAltitude = 0;
float initPascal = 0;
float initTemp = 0;

int signalCount = 0;

struct BMPValues {
    float meters;
    float pascal;
    float temperature;
    bool signal;
};

float meters;
bool executedBaseline = false;
float baseline = 0;
float totalMeters = 0;
int totalMetersCount = 0;
unsigned long timeInterval = 50;
float currentTime = 0;

void setup() {
    Serial.begin(115200);

    initBMP();

    pinMode(switchPin, INPUT_PULLUP);

    Serial.println("Initializing SD card...");

    if (!SD.begin(chipSelect)) {
        Serial.println("SD card initialization failed!");
        return;
    }

    BMPValues vals = getVals();

    initAltitude = vals.meters;
    initPascal = vals.pascal;
    initTemp = vals.temperature;
}

void loop() {
    BMPValues vals = getVals();

    // Open data files for writing
    dataRaw = SD.open("dataRaw.txt", FILE_WRITE);

    if (!dataRaw) {
        Serial.println("Failed to open dataRaw.txt!");
        return;
    }

    dataRaw.print("Seconds: ");
    dataRaw.print(currentTime / 1000.0, 2);

    dataRaw.print(", Altitude: ");
    dataRaw.print(vals.meters);

    dataRaw.print(", Pressure: ");
    dataRaw.print(vals.pascal / 1000.0, 2);

    dataRaw.print(", Temperature: ");
    dataRaw.print(vals.temperature);

    dataRaw.print(", Signal: ");
    dataRaw.println(vals.signal);

    dataRaw.close();

    if (currentTime + timeInterval > millis()) {

        if (vals.signal) {
            signalCount += 1;
        }

    }
    else {

        dataInterpreted = SD.open("dataIntr.txt", FILE_WRITE);

        if (!dataInterpreted) {
            Serial.println("Failed to open dataInterpreted.txt!");
            return;
        }

        dataInterpreted.print("Seconds: ");
        dataInterpreted.print(currentTime);
        dataInterpreted.print("-");

        dataInterpreted.print(currentTime + timeInterval);

        dataInterpreted.print(", Altitude: ");
        dataInterpreted.print((initAltitude + vals.meters) / 2, 2);

        dataInterpreted.print(", Pressure: ");
        dataInterpreted.print(((initPascal + vals.pascal) / 2) / 1000.0, 2);

        dataInterpreted.print(", Temperature: ");
        dataInterpreted.print((initTemp + vals.temperature) / 2, 2);

        dataInterpreted.print(", Signal count: ");
        dataInterpreted.println(signalCount);

        currentTime += timeInterval;
        signalCount = 0;

        initAltitude = vals.meters;
        initPascal = vals.pascal;
        initTemp = vals.temperature;

        dataInterpreted.close();
    }
}

void initBMP() {
    Serial.print("Probe BMP280: ");

    if (bmp280.initialize()) {
        Serial.println("Sensor found");
    }
    else {
        Serial.println("Sensor missing");
        while (1) {}
    }
}

BMPValues getVals() {

    bmp280.awaitMeasurement();

    float temperature;
    bmp280.getTemperature(temperature);

    float pascal;
    bmp280.getPressure(pascal);

    bmp280.getAltitude(meters);

    if (millis() > 3000) {
        totalMeters += meters;
        totalMetersCount += 1;

        if (totalMetersCount == 10 && !executedBaseline) {
            baseline = totalMeters / totalMetersCount;
            executedBaseline = true;

            Serial.print("Baseline: ");
            Serial.println(baseline);
        }
    }

    Serial.print("Baseline: ");
    Serial.println(baseline);

    bmp280.triggerMeasurement();

    meters -= baseline;

    int switchState = digitalRead(switchPin);

    bool signal = false;

    if (switchState == LOW) {
        signal = true;
    }

    return {meters, pascal, temperature, signal};
}
