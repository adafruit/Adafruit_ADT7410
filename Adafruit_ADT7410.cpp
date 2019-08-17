/*!
 *  @file Adafruit_ADT7410.cpp
 *
 *  @mainpage Adafruit ADT7410 I2C Temp Sensor
 *
 *  @section intro_sec Introduction
 *
 * 	I2C Driver for Microchip's ADT7410 I2C Temp sensor
 *
 * 	This is a library for the Adafruit ADT7410 breakout:
 * 	http://www.adafruit.com/products/4089
 *
 * 	Adafruit invests time and resources providing this open source code,
 *  please support Adafruit and open-source hardware by purchasing products from
 * 	Adafruit!
 *
 *  @section author Author
 *
 *  K.Townsend (Adafruit Industries)
 *
 * 	@section license License
 *
 * 	BSD (see license.txt)
 *
 * 	@section  HISTORY
 *
 *     v1.0 - First release
 */

#include "Arduino.h"
#include <Wire.h>

#include "Adafruit_ADT7410.h"

/*!
 *    @brief  Instantiates a new ADT7410 class
 */
Adafruit_ADT7410::Adafruit_ADT7410() {}

/*!
 *    @brief  Setups the HW with default address and Wire object
 *    @return True if initialization was successful, otherwise false.
 */
bool Adafruit_ADT7410::begin() {
  _i2caddr = ADT7410_I2CADDR_DEFAULT;
  _wire = &Wire;
  return init();
}

/*!
 *    @brief  Setups the HW with default address
 *    @param  *wire
 *    @return True if initialization was successful, otherwise false.
 */
bool Adafruit_ADT7410::begin(TwoWire *wire) {
  _i2caddr = ADT7410_I2CADDR_DEFAULT;
  _wire = wire;
  return init();
}

/*!
 *    @brief  Setups the HW with default Wire object
 *    @param  addr
 *    @return True if initialization was successful, otherwise false.
 */
bool Adafruit_ADT7410::begin(uint8_t addr) {
  _i2caddr = addr;
  _wire = &Wire;
  return init();
}

/*!
 *    @brief  Setups the HW
 *    @param  addr
 *    @param  *wire
 *    @return True if initialization was successful, otherwise false.
 */
bool Adafruit_ADT7410::begin(uint8_t addr, TwoWire *wire) {
  _i2caddr = addr;
  _wire = wire;
  return init();
}

/*!
 *    @brief  init function
 *    @return True if initialization was successful, otherwise false.
 */
bool Adafruit_ADT7410::init() {
  
  _wire->begin();

  uint8_t id = read8(ADT7410_REG__ADT7410_ID) & 0xF8;
  if (id != 0xC8) {
    return false;
  }

  // soft reset
  _wire->beginTransmission(_i2caddr);
  _wire->write(ADT7410_REG__ADT7410_SWRST);
  _wire->endTransmission();

  // ADT7410 is 13-bit resolution by default, ensure we enable the 16-bit 
  // temperature ADC conversion
  write8(ADT7410_REG__ADT7410_CONFIG, 0x80);

  delay(10);

  return true;
}

/*!
 *   @brief  Reads the 16-bit temperature register and returns the Centigrade
 *           temperature as a float.
 *   @return Temperature in Centigrade.
 */
float Adafruit_ADT7410::readTempC() {
  uint16_t t = read16(ADT7410_REG__ADT7410_TEMPMSB);

  float temp = (int16_t)t;
  temp /= 128.0;

  return temp;
}

/*!
 *   @brief  Reads the 16-bit temperature register and returns the Fahrenheit
 *           temperature as a float.
 *   @return Temperature in Fahrenheit.
 */
float Adafruit_ADT7410::readTempF() {
  uint16_t t = read16(ADT7410_REG__ADT7410_TEMPMSB);

  float temp = (int16_t)t;
  temp = temp * 0.0140625 /*( 1.0/128.0 * 9.0/5.0 )*/ + 32.0;

  return temp;
}

/*!
 *    @brief  Low level 8 bit read procedure
 *    @param  reg
 *    @return value
 */
uint8_t Adafruit_ADT7410::read8(uint8_t reg) {
  uint8_t val = 0xFF;
  uint8_t state;

  _wire->beginTransmission(_i2caddr);
  _wire->write(reg);
  state = _wire->endTransmission(false);

  if (0 == state) {
    _wire->requestFrom(_i2caddr, (uint8_t)2);
    val = _wire->read();
  }

  return val;
}

/*!
 *    @brief  Low level 8 bit write procedures
 *    @param  reg
 *    @param  value
 */
void Adafruit_ADT7410::write8(uint8_t reg, uint8_t value) {
  _wire->beginTransmission(_i2caddr);
  _wire->write(reg);
  _wire->write(value);
  _wire->endTransmission();
}

/*!
 *    @brief  Low level 16 bit read procedure
 *    @param  reg
 *    @return value
 */
uint16_t Adafruit_ADT7410::read16(uint8_t reg) {
  uint16_t val = 0xFFFF;
  uint8_t state;

  _wire->beginTransmission(_i2caddr);
  _wire->write(reg);
  state = _wire->endTransmission(false);

  if (0 == state) {
    _wire->requestFrom(_i2caddr, (uint8_t)2);
    val = _wire->read();
    val <<= 8;
    val |= _wire->read();
  }
  return val;
}

/*!
 *    @brief  Low level 16 bit write procedures
 *    @param  reg
 *    @param  value
 */
void Adafruit_ADT7410::write16(uint8_t reg, uint16_t value) {
  _wire->beginTransmission(_i2caddr);
  _wire->write((uint8_t)reg);
  _wire->write(value >> 8);
  _wire->write(value & 0xFF);
  _wire->endTransmission();
}
