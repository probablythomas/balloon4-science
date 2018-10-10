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
const int chipSelect = 8; // pin number for SPI select pin
char filename[] = "DATA0000.CSV";
File dataFile;

void setupHardware();
void startHardware();
void initializeSD();

void setup() {
    // put your setup code here, to run once:
    Wire.begin();
    setupHardware();
    startHardware();

}

void loop() {
    // put your main code here, to run repeatedly:


    // temperature sensor
    temperatureSensor.requestTemperatures();
    temperatureSensor.getTempCByIndex(0);
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
    initializeSD();

    //// temperature sensor
    // start
    temperatureSensor.begin();

    //// motion sensor
    // start
    motionSensor.begin();

    //// environmental sensor
    // start
    enviroSensor.begin();

    //// sd card
    // start

}

void initializeSD()
{
    Serial.print(F("Initialize SD card: "));

    // see if the card is present and can be initialized:
    if (!SD.begin(chipSelect)) {
        Serial.println(F("FAILED (or not present)"));
        // don't do anything more:
        while (1);
    }
    Serial.println(F("SUCCESS"));

    // create a new file
    Serial.print(F("Create log file: "));
    for (uint8_t i = 0; i < 256; i++) {
    //filename[4] = (i/1000) % 10 + '0';
    filename[5] = (i/100) % 10 + '0';
    filename[6] = (i/10) % 10 + '0';
    filename[7] = i%10 + '0';
    if (!SD.exists(filename)) {
        dataFile = SD.open(filename, FILE_WRITE); 
        if (dataFile) {
            break;  // leave the loop!
        }
    }
    if (i == 255) {
            Serial.println(F("FAILED"));
            while(1);
        }
    }

    Serial.print(F("SUCCESS -- "));
    Serial.println(filename);

    dataFile.print(F("Time(ms),SonicTime(ms),SonicRange(mm),LightRange(mm)"));
    dataFile.print(F(","));
    dataFile.print(F("a_x,a_y,a_z,w_x,w_y,w_z,b_x,b_y,b_z"));  
    dataFile.print(F(","));
    dataFile.println(F("temp,rh,press,alt,tempExt"));

    dataFile.close();
}