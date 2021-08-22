
[![Arduino CI](https://github.com/RobTillaart/MTP40C/workflows/Arduino%20CI/badge.svg)](https://github.com/marketplace/actions/arduino_ci)
[![License: MIT](https://img.shields.io/badge/license-MIT-green.svg)](https://github.com/RobTillaart/MTP40C/blob/master/LICENSE)
[![GitHub release](https://img.shields.io/github/release/RobTillaart/MTP40C.svg?maxAge=3600)](https://github.com/RobTillaart/MTP40C/releases)

# MTP40C

Arduino library for MTP40C CO2 + air pressure sensor

(include image)

## Description

The library for the MTP40C CO2 sensor is experimental as not all functionality is tested.


```
               // TOPVIEW
              +-------------+---+
              |             | O |
    Vin   1 --|             +---+
    GND   2 --|             |
    TX    3 --|             |
    RX    4 --|             |
    NC    5 --|             +---+
              |             | O |
              +-------------+---+
```


## Interface


### Constructors

- **MTP40C(Stream \* str)** constructor. should get a Serial port as parameter e.g. \&Serial
- **bool begin(uint8_t address = 0x64)** initialize the device, sets the address to communicate to it.
Uses the factory default value of 0x64 as default. Values allowed 0..247
- **bool isConnected()** returns true if the address as set by **begin()** can be found on the 'bus'


### Configuration

- **uint8_t getAddress()** request the address from the device.
Expect a value from 0 .. 247.
- **bool setAddress(uint8_t address)** set a new address for the device. 
Returns false if not successful.

These address functions are only needed if handling multiple devices. (under test)
- **void setGenericAddress()** uses the broadcast address 0xFE in all requests.
- **void setSpecificAddress()** uses the address specified in **begin()** or **setAddress()**
to be used in all requests.
- **bool useSpecificAddress()** returns true if the specific address is used.
Returns false if broadcast address is used.

The MTP40C library can set a maximum timeout in the communication with the sensor.
Normally this is not needed to set as the default of 1 second is more as long enough.
- **void setTimeOut(uint32_t to)** set the timeout.
- **uint32_t getTimeOut()** get the value set above (Or the default. Time in milliseconds.


### Measurements

- **float getAirPressure()** request air pressure from the device.
- **bool setAirPressureReference(float apr)** to calibrate the air pressure one can calibrate 
the sensor with an external device. 
- **uint16_t getGasConcentration()** retuns the CO2 concentration in PPM (parts per million).


### Calibration

Please read datasheet before using these functions to understand the process of calibration.


#### SPC calibration

This takes a relative short time (few minutes) to calibrate the sensor in a known 
gas concentration. 

- **bool setSinglePointCorrection(float spc)** takes several minutes. see datasheet.
- **bool getSinglePointCorrectionReady()** To see if SPC is finished or not.

As far as known the SPC point can not be retrieved from the sensor.


#### Self calibration

Self calibration is a process in which the sensor takes the minimum values over a longer period
between 24 - 720 hours as the reference for minimum outdoor values. (CHECK).
Note that 720 hours is 30 days / 1 month.

- **bool openSelfCalibration()** start the self calibration cycle. CHECK
- **bool closeSelfCalibration()** stop the self calibration cycle. CHECK
- **uint8_t getSelfCalibrationStatus()** CHECK.
- **bool setSelfCalibrationHours(uint16_t hrs)** Sets the number of hours between self calibration
moments. Valid values are 24 - 720 .
- **uint16_t getSelfCalibrationHours()** returns the value set above.


## TODO

- examples
  - setSinglePointCorrection + Pressure
  - self calibration
  - 2nd hardware Serial (MEGA)
- scanner to get the address of a device
- ESP32 test.


## Future

- test test test test
- CRC in PROGMEM
- optimize performance
- optimize memory usage  (buffer)
- serial bus with multiple devices? => diodes


## Operations

See examples.


## Sponsor 

The development of this MTP40C library is sponsored by [TinyTronics, Netherlands](https://www.tinytronics.nl/shop/en).
