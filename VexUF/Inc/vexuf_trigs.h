#ifndef VEXUF_TRIGS_H_
#define VEXUF_TRIGS_H_

#include "vexuf.h"
#include "vexuf_actuators.h"
#include "vexuf_outputs.h"
#include "vexuf_pwm.h"

#define NUMBER_OF_TRIGGERS 25

typedef enum {
  noTest = 0,
  lessThan = 1,
  lessThanOrEqual = 2,
  Equal = 3,
  GreaterThanOrEqual = 4,
  GreaterThan = 5,
  NotEqual = 6,
  inRange = 7,
  outtaRange = 8
} TrigCompareTest;

typedef enum {
  trigDisabled = 0,
  Av1ToAv2 = 1,
  Av2ToAv3 = 2,
  Av1ToAv3 = 3,
  IntTempToExternal = 4,
  Av1ToTest = 5,
  Av2ToTest = 6,
  Av3ToTest = 7,
  IntTempToTest = 8,
  ExtTempToTest = 9
} TrigComparison;

typedef struct {
  TrigComparison status : 4;  // 4 bits, enough to hold 10 choices
  TrigCompareTest test : 4;   // 4 bits, enough to hold 9 choices
  uint16_t reserved : 8;      // 8 bits (padding to make 16 bits total)
  uint16_t fromValue;         // 16 bits
  uint16_t toValue;           // 16 bits
  ActuatorsValues actuators;
  AlarmOrTrigOutput output;
  uint16_t pwm1;  // 16 bits, 0xffff = no change
  uint16_t pwm2;  // 16 bits, 0xffff = no change
  uint16_t tnc1Enabled : 1;
  uint16_t tnc1Path : 3;
  uint16_t tnc1Message : 4;
  uint16_t tnc2Enabled : 1;
  uint16_t tnc2Path : 3;
  uint16_t tnc2Message : 4;
} TriggerConfiguration;

UF_STATUS TRIGS_Init(void);
UF_STATUS TRIGS_runAll(void);
UF_STATUS TRIGS_runTrigger(uint8_t idx);

#endif  // VEXUF_TRIGS_H_