//
//    FILE: unit_test_001.cpp
//  AUTHOR: Rob Tillaart
// VERSION: 0.1.0
//    DATE: 2021-08-20
// PURPOSE: unit tests for the MTP40C CO2 sensor
//          https://github.com/RobTillaart/MTP40C
//          https://github.com/Arduino-CI/arduino_ci/blob/master/REFERENCE.md
//

// supported assertions
// ----------------------------
// assertEqual(expected, actual)
// assertNotEqual(expected, actual)
// assertLess(expected, actual)
// assertMore(expected, actual)
// assertLessOrEqual(expected, actual)
// assertMoreOrEqual(expected, actual)
// assertTrue(actual)
// assertFalse(actual)
// assertNull(actual)


#include <ArduinoUnitTests.h>

#include "Arduino.h"
#include "MTP40C.h"


unittest_setup()
{
}


unittest_teardown()
{
}


unittest(test_begin)
{
  fprintf(stderr, "MTP40C_LIB_VERSION:\t%s", MTP40C_LIB_VERSION);
  MTP40C sensor = MTP40C();
  sensor.begin();
}


unittest_main()

// --------
