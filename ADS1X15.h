#pragma once
//
//        FILE: ADS1X15.cpp
//      AUTHOR: DeeEmm
//     VERSION: See changelog
//        DATE: 2025-01-14
//     PURPOSE: Arduino library for ADS_1115 
//         URL: https://github.com/DeeEmm/ADS1115_lite
// FORKED FROM: https://github.com/RobTillaart/ADS1X15
//         FOR: DIYFB Project - https://github.com/DeeEmm/DIY-Flow-Bench

#include "Arduino.h"
#include "Wire.h"

#ifndef ADS_1115_ADDRESS
#define ADS_1115_ADDRESS                   0x48
#endif


#define ADS1X15_OK                        0
#define ADS1X15_INVALID_VOLTAGE           -100
#define ADS1X15_ERROR_TIMEOUT             -101
#define ADS1X15_ERROR_I2C                 -102
#define ADS1X15_INVALID_GAIN              0xFF
#define ADS1X15_INVALID_MODE              0xFE


//  PARAMETER CONSTANTS NOT USED IN CODE YET
//  enum ?
#define ADS1X15_GAIN_6144MV               0x00
#define ADS1X15_GAIN_4096MV               0x01
#define ADS1X15_GAIN_2048MV               0x02
#define ADS1X15_GAIN_1024MV               0x04
#define ADS1X15_GAIN_0512MV               0x08
#define ADS1X15_GAIN_0256MV               0x10

#define ADS1x15_COMP_MODE_TRADITIONAL     0x00
#define ADS1x15_COMP_MODE_WINDOW          0x01

// #define ADS1x15_COMP_POL_FALLING_EDGE     0x00
// #define ADS1x15_COMP_POL_RISING_EDGE      0x01

// #define ADS1x15_COMP_POL_LATCH            0x00
// #define ADS1x15_COMP_POL_NOLATCH          0x01



class ADS1X15
{
public:
  void     reset();

  bool     begin();
  bool     isConnected();

  //           GAIN
  //  0  =  +- 6.144V  default
  //  1  =  +- 4.096V
  //  2  =  +- 2.048V
  //  4  =  +- 1.024V
  //  8  =  +- 0.512V
  //  16 =  +- 0.256V
  void     setGain(uint8_t gain = 0);    //  invalid values are mapped to 0 (default).
  uint8_t  getGain();                    //  0xFF == invalid gain error.


  //  0  =  CONTINUOUS
  //  1  =  SINGLE       default
  void     setMode(uint8_t mode = 1);    //  invalid values are mapped to 1 (default)
  // uint8_t  getMode();                    //  0xFE == invalid mode error.


  //  0  =  slowest
  //  7  =  fastest
  //  4  =  default
  void     setDataRate(uint8_t dataRate = 4);  //  invalid values are mapped on 4 (default)
  // uint8_t  getDataRate();                      //  actual speed depends on device


  int16_t  readADC(uint8_t pin = 0);
  // int16_t  readADC_Differential_0_1();

  //  used by continuous mode and async mode.
  //  [[deprecated("Use getValue() instead")]]
  //  int16_t  getLastValue() { return getValue(); };  //  will be obsolete in the future 0.4.0
  int16_t  getValue();


  //  ASYNC INTERFACE
  //  requestADC(pin) -> isBusy() or isReady() -> getValue();
  //  see examples
  void     requestADC(uint8_t pin = 0);
  // void     requestADC_Differential_0_1();
  bool     isBusy();
  bool     isReady();


  //  COMPARATOR
  //  0    = TRADITIONAL   > high          => on      < low   => off
  //  else = WINDOW        > high or < low => on      between => off
  // void     setComparatorMode(uint8_t mode);
  // uint8_t  getComparatorMode();

  //  0    = LOW (default)
  //  else = HIGH
  // void     setComparatorPolarity(uint8_t pol);
  // uint8_t  getComparatorPolarity();

  //  0    = NON LATCH
  //  else = LATCH
  // void     setComparatorLatch(uint8_t latch);
  // uint8_t  getComparatorLatch();

  //  0   = trigger alert after 1 conversion
  //  1   = trigger alert after 2 conversions
  //  2   = trigger alert after 4 conversions
  //  3   = Disable comparator =  default, also for all other values.
  // void     setComparatorQueConvert(uint8_t mode);
  // uint8_t  getComparatorQueConvert();

  // void     setComparatorThresholdLow(int16_t lo);
  // int16_t  getComparatorThresholdLow();
  // void     setComparatorThresholdHigh(int16_t hi);
  // int16_t  getComparatorThresholdHigh();


  int8_t   getError();


  // //  EXPERIMENTAL
  // //  see https://github.com/RobTillaart/ADS1X15/issues/22
  // void     setWireClock(uint32_t clockSpeed = 100000);
  // //  prototype
  // //  - getWireClock returns the value set by setWireClock
  // //    not necessary the actual value
  // uint32_t getWireClock();


protected:
  ADS1X15();

  //  CONFIGURATION
  //  BIT   DESCRIPTION
  //  0     # channels        0 == 1    1 == 4;
  //  1     0
  //  2     # resolution      0 == 12   1 == 16
  //  3     0
  //  4     has gain          0 = NO    1 = YES
  //  5     has comparator    0 = NO    1 = YES
  //  6     0
  //  7     0
  uint8_t  _config;
  uint8_t  _maxPorts;
  uint8_t  _address;
  uint8_t  _conversionDelay;
  uint8_t  _bitShift;
  uint16_t _gain;
  uint16_t _mode;
  uint16_t _datarate;

  //  COMPARATOR variables
  //  TODO merge these into one COMPARATOR MASK?  (low priority)
  //       would speed up code in _requestADC() and save 3 bytes RAM.
  //  TODO boolean flags for first three, or make it mask value that
  //       can be or-ed.   (low priority)
  uint8_t  _compMode;
  uint8_t  _compPol;
  uint8_t  _compLatch;
  uint8_t  _compQueConvert;

  //  variable to track the last pin requested,
  //  to allow for round robin query of
  //  pins based on this state == if no last request then == 0xFFFF.
  uint16_t  _lastRequest;

  int16_t  _readADC(uint16_t readmode);
  void     _requestADC(uint16_t readmode);
  bool     _writeRegister(uint8_t address, uint8_t reg, uint16_t value);
  uint16_t _readRegister(uint8_t address, uint8_t reg);
  int8_t   _error = ADS1X15_OK;

  TwoWire*  _wire;
  uint32_t  _clockSpeed = 0;
};


class ADS_1115 : public ADS1X15
{
public:
  ADS_1115(uint8_t address = ADS_1115_ADDRESS, TwoWire *wire = &Wire);
  int16_t  readADC_Differential_0_3();
  int16_t  readADC_Differential_1_3();
  int16_t  readADC_Differential_2_3();
  int16_t  readADC_Differential_0_2();   //  not possible in async
  int16_t  readADC_Differential_1_2();   //  not possible in async
  void     requestADC_Differential_0_3();
  void     requestADC_Differential_1_3();
  void     requestADC_Differential_2_3();
};


//  -- END OF FILE --

