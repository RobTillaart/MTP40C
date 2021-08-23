//
//    FILE: MTP40C_getAddress.ino
//  AUTHOR: Rob Tillaart
// VERSION: 0.1.0
// PURPOSE: demo of MTP40C library
//    DATE: 2021-08-21
//     URL: https://github.com/RobTillaart/MTP40C
//

#include "MTP40C.h"

MTP40C mtp(&Serial);

void setup()
{
  Serial.begin(19200);
  Serial.println(__FILE__);

  Serial.print("MTP40C_LIB_VERSION:\t");
  Serial.println(MTP40C_LIB_VERSION);

  if (mtp.begin(248) == false)   // must fail!
  {
    Serial.println("OK 248");
  }
  mtp.begin();    // default address 0x64

  Serial.println();
  uint8_t addr = mtp.getAddress();
  Serial.print("ADDR: \t");
  Serial.println(addr);
  Serial.println();

  Serial.println();
  mtp.setAddress(0x10);
  Serial.println();
  mtp.setAddress(0x20);
  Serial.println();
  mtp.setAddress(0x30);

  Serial.println("\ndone");
}


void loop()
{
}


// -- END OF FILE --
