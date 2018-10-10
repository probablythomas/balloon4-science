#include <Arduino.h>

#include <Wire.h>
#include <SPI.h>
#include <SD.h>

#include <Adafruit_Sensor.h>
#include <Adafruit_LSM9DS1.h>
#include <Adafruit_BME280.h>

#include <OneWire.h>
#include <DallasTemperature.h>

/* libraries to be included:
Little motor control
*/

#define ONE_WIRE_BUS 2 // pin number for one-wire temperature sensor

// temperature sensor
OneWire oneWire(ONE_WIRE_BUS); // Create One Wire object
DallasTemperature temperatureSensor(&oneWire);

// motion sensor (I2C)
Adafruit_LSM9DS1 motionSensor = Adafruit_LSM9DS1();

// environmental sensor (I2C)
Adafruit_BME280 enviroSensor;

// sd card (SPI)
const int selectSDCard = 8; // pin number for SPI select pin
char logFilename[] = "LOG0000.CSV";
char motionFilename[] = "LSM0000.CSV";
char enviroFilename[] = "BME0000.CSV";
char extTempFilename[] = "EXTT0000.CSV";
File logFile, motionFile, enviroFile, extTempFile;

void setupHardware();
void startHardware();
void initializeSD();

void setup() {
    // put your setup code here, to run once:
    Serial.begin(9600);

    Wire.begin();
    setupHardware();
    startHardware();


}

void loop() {
    // put your main code here, to run repeatedly:
    unsigned long now = millis();
    // temperature sensor
    Serial.print("Requesting temperatures...");
    temperatureSensor.requestTemperatures(); // this takes ~600 ms
    Serial.println((millis()-now));
    
    // After we got the temperatures, we can print them here.
    // We use the function ByIndex, and as an example get the temperature from the first sensor only.
    Serial.print("Temperature for the device 1 (index 0) is: ");
    Serial.println(temperatureSensor.getTempCByIndex(0));

    delay(1000);
}

void setupHardware() {
    //// temperature sensor
    // none required

    //// motion sensor
    // 1.) Set the accelerometer range
    motionSensor.setupAccel(motionSensor.LSM9DS1_ACCELRANGE_2G);
    // 2.) Set the magnetometer sensitivity
    motionSensor.setupMag(motionSensor.LSM9DS1_MAGGAIN_4GAUSS);
    // 3.) Setup the gyroscope
    motionSensor.setupGyro(motionSensor.LSM9DS1_GYROSCALE_245DPS);

    //// environmental sensor
    // none required
}

void startHardware() {
    //// sd card
    // see if the card is present and can be initialized:
    if (!SD.begin(selectSDCard)) {
        Serial.println(F("Ruh roh... unable to initialize the SD Card"));
        // don't do anything more:
        while (1);
    }
    Serial.println(F("SD Card initialized!"));

    //// temperature sensor
    // start
    temperatureSensor.begin();

    //// motion sensor
    // start
    // Try to initialise and warn if we couldn't detect the chip
    if (!motionSensor.begin()) {
        Serial.println(F("Ruh roh... unable to initialize the LSM9DS1"));
        while (1);
    }
    Serial.println(F("LSM9DS1 9DOF sensor initialized!"));

    //// environmental sensor
    // start
    if (!enviroSensor.begin()) {
        Serial.println(F("Ruh roh... unable to initialize the BME280"));
        while (1);
    }
    Serial.println(F("BME280 environmental sensor initialized!"));
    //// sd card
    // start

}

void initializeSD() {
    // create a new file
    Serial.print(F("Creating log and telemetry files: "));
    for (uint8_t i = 0; i < 256; i++) {
        //logFilename[4] = (i/1000) % 10 + '0';
        logFilename[5] = (i/100) % 10 + '0';
        logFilename[6] = (i/10) % 10 + '0';
        logFilename[7] = i%10 + '0';
        if (!SD.exists(logFilename)) {
            logFile = SD.open(logFilename, FILE_WRITE); 
            if (logFile) {
                Serial.print(logFilename);
                break;  // leave the loop!
            }
        }
        if (i == 255) {
            Serial.println(F("Error: failed to find an available file index"));
            while(1);
        }
    }

    logFile.print(F("Time(ms),SonicTime(ms),SonicRange(mm),LightRange(mm)"));
    logFile.print(F(","));
    logFile.print(F("a_x,a_y,a_z,w_x,w_y,w_z,b_x,b_y,b_z"));  
    logFile.print(F(","));
    logFile.println(F("temp,rh,press,alt,tempExt"));

    logFile.close();
}
