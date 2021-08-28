//
//    FILE: MTP40C_test.ino
//  AUTHOR: Rob Tillaart
// VERSION: 0.1.0
// PURPOSE: demo of MTP40C library
//    DATE: 2021-08-20
//     URL: https://github.com/RobTillaart/MTP40C
//

// CODE PART TEST


#include "MTP40C.h"

MTP40C mtp(&Serial);


void setup()
{
  Serial.begin(115200);
  Serial.println(__FILE__);

  Serial.print("MTP40_LIB_VERSION:\t");
  Serial.println(MTP40_LIB_VERSION);

  union
  {
    float value;
    uint8_t b[4];
  } convert;

  convert.b[0] = 0x00;
  convert.b[1] = 0x40;
  convert.b[2] = 0x7D;
  convert.b[3] = 0x44;
  Serial.println(convert.value, 1);

  convert.b[0] = 0x00;
  convert.b[1] = 0x80;
  convert.b[2] = 0x40;
  convert.b[3] = 0x44;
  Serial.println(convert.value, 1);

  uint8_t cmd[7] = { 0xFE, 0x28, 0x6A, 0x64, 0x00, 0x0E, 0xA8 };

  uint16_t x = mtp.CRC(cmd, 5);
  Serial.println(x, HEX);

}


void loop()
{
}


// -- END OF FILE --
