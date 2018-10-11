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
const int selectSDCard = 15; // pin number for SPI select pin
char logFilename[] = "LOG0000.CSV";
char motionFilename[] = "MOT0000.CSV";
char enviroFilename[] = "ENV0000.CSV";
char extTempFilename[] = "EXT0000.CSV";
File logFile, motionFile, enviroFile, extTempFile;

// generic
unsigned long now, lastFastCycle = 0, lastSlowCycle = 0;

// measurements
float extTemperature, temperature, humidity, pressure;


void setupHardware();
void startHardware();
void initializeSD();

void setup() {
    // put your setup code here, to run once:
    Serial.begin(9600);
    Serial.println();
    Serial.println(F("System startup..."));

    Wire.begin();
    setupHardware();
    startHardware();
    initializeSD();

    Serial.println(F("Startup complete!"));
}

void loop() {
    // put your main code here, to run repeatedly:

    // begin by getting a start time for this run
    now = millis();

    // fast loop, once a second?
    if ((now - lastFastCycle) >= 100) {
        Serial.print("fast:\t");
        Serial.print(millis());
        // record some stuff!
        // motion sensor
        motionSensor.read();
        sensors_event_t a, m, g, temp;
        motionSensor.getEvent(&a, &m, &g, &temp); 

        // Write to SD Card
        motionFile = SD.open(motionFilename, FILE_WRITE);
        if (motionFile) {
            motionFile.print(now);
            motionFile.print(",");
            motionFile.print(a.acceleration.x);
            motionFile.print(",");
            motionFile.print(a.acceleration.y);
            motionFile.print(",");
            motionFile.print(a.acceleration.z);
            motionFile.print(",");
            motionFile.print(g.gyro.x);
            motionFile.print(",");
            motionFile.print(g.gyro.y);
            motionFile.print(",");
            motionFile.print(g.gyro.z);
            motionFile.print(",");
            motionFile.print(m.magnetic.x);
            motionFile.print(",");
            motionFile.print(m.magnetic.y);
            motionFile.print(",");
            motionFile.print(m.magnetic.z);
            // motionFile.print(",");
            // motionFile.print(temp.temperature);
            motionFile.println();
            motionFile.close();
        }
        else {
            logFile = SD.open(logFilename, FILE_WRITE);
            if (logFile) {
                logFile.print("Error writing fast cycle at ");
                logFile.println(now);
                logFile.close();
            }
        }

        // finally, update the last fast read time
        lastFastCycle = now;
        Serial.print("\t");
        Serial.print(millis());
        Serial.println();
    }

    // slow loop, once a minute?
    if ((now - lastSlowCycle) >= 60000) {
        // record some stuff!
        // External temperature
        temperatureSensor.requestTemperatures(); // this takes ~600 ms
        extTemperature = temperatureSensor.getTempCByIndex(0);

        // BME280 measurements
        temperature = enviroSensor.readTemperature();
        humidity = enviroSensor.readHumidity();
        pressure = enviroSensor.readPressure();

        // Write to SD Card
        enviroFile = SD.open(enviroFilename, FILE_WRITE);
        if (enviroFile) {
            enviroFile.print(now);
            enviroFile.print(",");
            enviroFile.print(extTemperature);
            enviroFile.print(",");
            enviroFile.print(temperature);
            enviroFile.print(",");
            enviroFile.print(humidity);
            enviroFile.print(",");
            enviroFile.print(pressure);
            enviroFile.println();
            enviroFile.close();
        }
        else {
            logFile = SD.open(logFilename, FILE_WRITE);
            if (logFile) {
                logFile.print("Error writing slow cycle at ");
                logFile.println(now);
                logFile.close();
            }
        }

        // finally, update the last fast read time
        lastSlowCycle = now;
    }
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
}

void initializeSD() {
    // create a new file
    Serial.print(F("Creating log and telemetry files: "));
    for (uint8_t i = 0; i < 256; i++) {
        //logFilename[4] = (i/1000) % 10 + '0';
        logFilename[4] = (i/100) % 10 + '0';
        logFilename[5] = (i/10) % 10 + '0';
        logFilename[6] = (i) % 10 + '0';
        if (!SD.exists(logFilename)) {
            logFile = SD.open(logFilename, FILE_WRITE); 
            if (logFile) {
                Serial.println(logFilename);
                break;  // leave the loop!
            }
        }
        if (i == 255) {
            Serial.println(F("Error: failed to find an available file index"));
            while(1);
        }
    }

    for (uint8_t j = 4; j < 7; j++) {
        motionFilename[j] = logFilename[j];
        enviroFilename[j] = logFilename[j];
        // extTempFilename[j] = logFilename[j];
    }

    motionFile = SD.open(motionFilename, FILE_WRITE);
    motionFile.println(F("Time(ms),AccelX,AccelY,AccelZ,OmegaX,OmegaY,OmegaZ,MagX,MagY,MagZ"));  
    motionFile.close();

    enviroFile = SD.open(enviroFilename, FILE_WRITE);
    enviroFile.println(F("Time(ms),ExternalTemperature(C),InternalTemperature(C),Humidity(%),Pressure(Pa)"));
    enviroFile.close();

    // extTempFile = SD.open(extTempFilename, FILE_WRITE);
    // extTempFile.println(F("Time(ms),ExtTemperature"));
    // extTempFile.close();

    logFile.println(F("Created telemetry files:"));
    logFile.println(motionFilename);
    logFile.println(enviroFilename);
    // logFile.println(extTempFilename);
    logFile.close();
}
