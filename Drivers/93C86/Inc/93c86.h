/*
 * 93C86.h
 *
 *  Created on: Jul 27, 2024
 *      Author: Aly Badawy
 */

#ifndef INC_93C86_H_
#define INC_93C86_H_

#include "main.h"

#define EEPROM_CMD_READ 0x1800
#define EEPROM_CMD_WRITE 0x1400
#define EEPROM_CMD_ERASE 0x1C00
#define EEPROM_CMD_WREN 0x1300
#define EEPROM_CMD_WRDI 0x1000
#define EEPROM_CMD_ERASE_ALL 0x1200

#define MEM_DelayNUM 2
#define MEM_DeLay() \
  for (uint16_t i = 0; i < MEM_DelayNUM; i++) asm("NOP")
#define MEM_Timeout 1000

typedef enum { EEPROM_OK = 0, EEPROM_BUSY, EEPROM_ERROR } EEPROM_STATUS;

void EEPROM_93C86_init(GPIO_TypeDef* cs_port, uint16_t cs_pin);

EEPROM_STATUS EEPROM_93C86_Read(uint16_t address, uint16_t* data);
EEPROM_STATUS EEPROM_93C86_ReadMultipleWords(uint16_t startAddress,
                                             uint16_t* buffer, uint16_t length);

EEPROM_STATUS EEPROM_93C86_Write(uint16_t address, uint16_t* data);
EEPROM_STATUS EEPROM_93C86_WriteMultipleWords(uint16_t startAddress,
                                              uint16_t* buffer,
                                              uint16_t length);

EEPROM_STATUS EEPROM_93C86_Erase(uint16_t address);
EEPROM_STATUS EEPROM_93C86_EraseAll(void);

void EEPROM_Test(void);

#endif /* INC_93C86_H_ */