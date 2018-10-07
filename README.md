# balloon4-science
 
 This code is for the 4th flight by the Astro Hackers. Learn more about the @astroHackers on [Twitter](https://twitter.com/astroHackers) and on [Instagram](https://www.instagram.com/astrohackers/)
 
 
## System overview 
### Sensors
Multiple sensors are used to collect flight data. Measurements include:

- Linear acceleration
- Rotational velocity
- Magnetic field
- Temperature (internal and external)
- Pressure
- Humidity

Data is collected on an SD card as a formatted text file for simple retrieval after flight.

### Actuators
None at this time.

## Changes from 3rd flight
A few changes have occured in the hardware of the sensor system:

- Swapping out the Arduino UNO for the Adafruit Huzzah ESP8266. Advantages include wifi connectivity, onboard LiPo battery charging and usage, and smaller footprint and mass.
- Reduction of sensors. Removal of the UV sensor and ranging payloads as they both accomplished their objectives.
- (As of 07/10/18) Planned addition of stabilized stage for future flights using a DFR micro DC motor with encoder.
- (As of 07/10/18) Planned addition of hot-wire cutting test system for altitude-based balloon release in future flights.

## Contributors
Hardware and software by Thomas Sears.
See individual libraries for their authors.
