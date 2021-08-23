//
//    FILE: MTP40C.h
//  AUTHOR: Rob Tillaart
//    DATE: 2021-08-20
// VERSION: 0.1.1
// PURPOSE: Arduino library for MTP40C CO2 sensor
//     URL: https://github.com/RobTillaart/MTP40C
//
// HISTORY:
//  0.1.0   2021-08-20  initial version
//  0.1.1   2021-08-23  added examples, minor fixes


#include "MTP40C.h"

// debug flag, development.
// #define MTP40C_DEBUG    1



MTP40C::MTP40C(Stream * str)
{
  _ser = str;
  _buffer[0] = '\0';
}


bool MTP40C::begin(uint8_t address)
{
  if (address > 247) return false;

  _address = address;
  return isConnected();
}


bool MTP40C::isConnected()
{
  uint8_t addr = getAddress();
  return (addr == _address);
}


uint8_t MTP40C::getAddress()
{
  uint8_t cmd[8] = { 0xFE, 0x03, 0x14, 0x00, 0x01, 0x00, 0x55, 0xA5 };
  if (request(cmd, 8, 7) )
  {
   return _buffer[3];
  }
  return 0xFF;
}


bool MTP40C::setAddress(uint8_t address)
{
  if (address > 247) return false;

  uint8_t cmd[11] = { 0xFE, 0x10, 0x04, 0x00, 0x01, 0x00, 0x01, 0x60, 0x00, 0x42, 0x58 };
  cmd[0] = _address;
  cmd[7] = address;
  if (request(cmd, 11, 8) )
  {
    _address = address;
    return true;
  }
  return false;
}


float MTP40C::getAirPressure()
{
  union
  {
    float value;
    uint8_t b[4];
  } convert;

  // max read freq 1x per 4 seconds
  if (millis() - _lastRead < 4000) return _airPressure;  // last value
  _lastRead = millis();

  uint8_t cmd[5] = { 0xFE, 0x68, 0x01, 0xFE, 0x30 };
  if (request(cmd, 5, 10) )
  {
    for (uint8_t i = 0; i < 3; i++)
    {
      convert.b[i] = _buffer[4 + i];
    }
    _airPressure = convert.value;
    return _airPressure;
  }
  return -999;  // error
}


bool MTP40C::setAirPressureReference(float apr)
{
  if ((apr < 700) || (apr > 1100)) return false;

  union
  {
    float value;
    uint8_t b[4];
  } convert;

  uint8_t cmd[10] = { 0xFE, 0x67, 0x01, 0x01, 0x00, 0x40, 0x7D, 0x44, 0xC4, 0xA3 };
  convert.value = apr;
  for (uint8_t i = 0; i < 3; i++)
  {
    cmd[4 + i] = convert.b[i];
  }
  if (request(cmd, 10, 10) )
  {
    return true;
  }
  return false;
}


uint16_t MTP40C::getGasConcentration()
{
  // max read freq 1x per 4 seconds
  if (millis() - _lastRead < 4000) return _gasLevel;  // last value
  _lastRead = millis();

  uint8_t cmd[5] = { 0xFE, 0x69, 0x03, 0x7E, 0x61 };
  if (request(cmd, 5, 14) )
  {
    // check valid
    for (uint8_t i = 8; i < 12; i++)
    {
      if (_buffer[i] != 0) return 0;
    }
    _gasLevel = _buffer[5] *256 + _buffer[4];
    return _gasLevel;
  }
  return 0;  // out of range to indicate error
}


bool MTP40C::setSinglePointCorrection(float spc)
{
  if ((spc < 400) || (spc > 5000)) return false;

  union
  {
    float value;
    uint8_t b[4];
  } convert;

  uint8_t cmd[9] = { 0xFE, 0x28, 0x80, 0x00, 0x80, 0x40, 0x44, 0x33, 0x22 };
  convert.value = spc;
  for (uint8_t i = 0; i < 3; i++)
  {
    cmd[3 + i] = convert.b[i];
  }
  if (request(cmd, 9, 10) )
  {
    if (_buffer[7] ) return true;
  }
  return false;
}


bool MTP40C::getSinglePointCorrectionReady()
{
  uint8_t cmd[5] = { 0xFE, 0x28, 0x81, 0xCE, 0x50 };
  if (request(cmd, 5, 6) )
  {
    if (_buffer[3] == 0) return true;
  }
  return false;
}


bool MTP40C::openSelfCalibration()
{
  uint8_t cmd[6] = { 0xFE, 0x28, 0x66, 0xFF, 0xDA, 0x24 };
  if (request(cmd, 6, 6) )
  {
    return true;
  }
  return false;
}


bool MTP40C::closeSelfCalibration()
{
  uint8_t cmd[6] = { 0xFE, 0x28, 0x66, 0x00, 0x9A, 0x64 };
  if (request(cmd, 6, 6) )
  {
    return true;
  }
  return false;
}


uint8_t MTP40C::getSelfCalibrationStatus()
{
  uint8_t cmd[5] = { 0xFE, 0x28, 0x67, 0x4F, 0xDA };
  if (request(cmd, 5, 6) )
  {
    return _buffer[3];
  }
  return 0x02;
}


bool MTP40C::setSelfCalibrationHours(uint16_t hrs)
{
  if ((hrs < 24) || (hrs > 720)) return false;
  uint8_t cmd[7] = { 0xFE, 0x28, 0x6A, 0x64, 0x00, 0x0E, 0xA8 };
  cmd[4] = hrs & 0xFF;
  cmd[5] = hrs / 256;
  if (request(cmd, 7, 6) )
  {
    return (_buffer[3] == 0x00);
  }
  return false;
}


uint16_t MTP40C::getSelfCalibrationHours()
{
  uint8_t cmd[5] = { 0xFE, 0x28, 0x69, 0xCE, 0x1E };
  if (request(cmd, 5, 9) )
  {
    return _buffer[4] * 256 + _buffer[3];
  }
  return 0xFFFF;  // to indicate error.
}


//////////////////////////////////////////////////////////////////////
//
// PRIVATE
//
bool MTP40C::request(uint8_t *data, uint8_t commandLength, uint8_t answerLength)
{
  // generic or specific address
  if (_useAddress) 
  {
    data[0] = _address;
  }
  else
  {
    data[0] = 0xFE;  // broadcast 
  }
  // calculate CRC of command
  uint16_t crc = CRC(data, commandLength - 2);
  data[commandLength - 1] = crc / 256;
  data[commandLength - 2] = crc & 0xFF;
  while (commandLength--)
  {
#ifdef MTP40C_DEBUG
    if (*data < 0x10) _ser->print(0);
    _ser->print(*data++, HEX);
    _ser->print(" ");
#else
    _ser->write(*data++);
#endif
     yield();  // because baud rate is low!
  }

  uint32_t start = millis();
  uint8_t i = 0;
  while (answerLength)
  {
    if (millis() - start > _timeout) return false;
    if (_ser->available())
    {
      _buffer[i] = _ser->read();
      i++;
      answerLength--;
    }
    yield();  // because baud rate is low!
  }
  return true;
}


// from datasheet
uint16_t MTP40C::CRC(uint8_t *data, uint16_t len)
{
const uint8_t auchCRCHi[] = {
  0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81,
  0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
  0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01,
  0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,
  0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81,
  0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0,
  0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01,
  0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
  0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81,
  0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
  0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01,
  0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
  0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81,
  0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
  0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01,
  0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
  0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81,
  0x40
};


const uint8_t auchCRCLo[] = {
  0x00, 0xC0, 0xC1, 0x01, 0xC3, 0x03, 0x02, 0xC2, 0xC6, 0x06, 0x07, 0xC7, 0x05, 0xC5, 0xC4,
  0x04, 0xCC, 0x0C, 0x0D, 0xCD, 0x0F, 0xCF, 0xCE, 0x0E, 0x0A, 0xCA, 0xCB, 0x0B, 0xC9, 0x09,
  0x08, 0xC8, 0xD8, 0x18, 0x19, 0xD9, 0x1B, 0xDB, 0xDA, 0x1A, 0x1E, 0xDE, 0xDF, 0x1F, 0xDD,
  0x1D, 0x1C, 0xDC, 0x14, 0xD4, 0xD5, 0x15, 0xD7, 0x17, 0x16, 0xD6, 0xD2, 0x12, 0x13, 0xD3,
  0x11, 0xD1, 0xD0, 0x10, 0xF0, 0x30, 0x31, 0xF1, 0x33, 0xF3, 0xF2, 0x32, 0x36, 0xF6, 0xF7,
  0x37, 0xF5, 0x35, 0x34, 0xF4, 0x3C, 0xFC, 0xFD, 0x3D, 0xFF, 0x3F, 0x3E, 0xFE, 0xFA, 0x3A,
  0x3B, 0xFB, 0x39, 0xF9, 0xF8, 0x38, 0x28, 0xE8, 0xE9, 0x29, 0xEB, 0x2B, 0x2A, 0xEA, 0xEE,
  0x2E, 0x2F, 0xEF, 0x2D, 0xED, 0xEC, 0x2C, 0xE4, 0x24, 0x25, 0xE5, 0x27, 0xE7, 0xE6, 0x26,
  0x22, 0xE2, 0xE3, 0x23, 0xE1, 0x21, 0x20, 0xE0, 0xA0, 0x60, 0x61, 0xA1, 0x63, 0xA3, 0xA2,
  0x62, 0x66, 0xA6, 0xA7, 0x67, 0xA5, 0x65, 0x64, 0xA4, 0x6C, 0xAC, 0xAD, 0x6D, 0xAF, 0x6F,
  0x6E, 0xAE, 0xAA, 0x6A, 0x6B, 0xAB, 0x69, 0xA9, 0xA8, 0x68, 0x78, 0xB8, 0xB9, 0x79, 0xBB,
  0x7B, 0x7A, 0xBA, 0xBE, 0x7E, 0x7F, 0xBF, 0x7D, 0xBD, 0xBC, 0x7C, 0xB4, 0x74, 0x75, 0xB5,
  0x77, 0xB7, 0xB6, 0x76, 0x72, 0xB2, 0xB3, 0x73, 0xB1, 0x71, 0x70, 0xB0, 0x50, 0x90, 0x91,
  0x51, 0x93, 0x53, 0x52, 0x92, 0x96, 0x56, 0x57, 0x97, 0x55, 0x95, 0x94, 0x54, 0x9C, 0x5C,
  0x5D, 0x9D, 0x5F, 0x9F, 0x9E, 0x5E, 0x5A, 0x9A, 0x9B, 0x5B, 0x99, 0x59, 0x58, 0x98, 0x88,
  0x48, 0x49, 0x89, 0x4B, 0x8B, 0x8A, 0x4A, 0x4E, 0x8E, 0x8F, 0x4F, 0x8D, 0x4D, 0x4C, 0x8C,
  0x44, 0x84, 0x85, 0x45, 0x87, 0x47, 0x46, 0x86, 0x82, 0x42, 0x43, 0x83, 0x41, 0x81, 0x80,
  0x40
};

  uint8_t  uchCRCHi = 0xFF ; // High byte initialization of the CRC
  uint8_t  uchCRCLo = 0xFF ; // The low byte of the CRC is initialized
  uint16_t uIndex;           // Query the CRC table index
  uint16_t crc;

  while (len--) /* Complete the entire message buffer*/
  {
    uIndex   = uchCRCLo ^ *data++;            /* Calculate CRC */;
    uchCRCLo = uchCRCHi ^ auchCRCHi[uIndex];
    uchCRCHi = auchCRCLo[uIndex];
  }
  crc = (uint16_t)uchCRCHi * 256;
  crc += (uint16_t)uchCRCLo;
  return crc;
}


// -- END OF FILE --
