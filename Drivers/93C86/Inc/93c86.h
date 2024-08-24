/*
 * 93C86.h
 *
 *  Created on: Jul 27, 2024
 *      Author: Aly Badawy
 */

#ifndef INC_93C86_H_
#define INC_93C86_H_

#include "vexuf_helpers.h"


#define EEPROM_CMD_READ  		0x1800
#define EEPROM_CMD_WRITE 		0x1400
#define EEPROM_CMD_ERASE 		0x1C00
#define EEPROM_CMD_WREN  		0x1300
#define EEPROM_CMD_WRDI  		0x1000
#define EEPROM_CMD_ERASE_ALL 	0x1200

#define MEM_DelayNUM			2
#define MEM_DeLay()				for(uint16_t i=0;i<MEM_DelayNUM;i++)asm("NOP")
#define MEM_Timeout				1000

typedef enum {
    EEPROM_OK = 0,
    EEPROM_BUSY,
    EEPROM_ERROR
} EEPROM_STATUS;

void EEPROM_93C86_init(GPIO_TypeDef * cs_port, uint16_t cs_pin);

EEPROM_STATUS EEPROM_93C86_Read(uint16_t address, uint16_t *data);
EEPROM_STATUS EEPROM_93C86_ReadMultipleWords(uint16_t startAddress, uint16_t* buffer, uint16_t length);

EEPROM_STATUS EEPROM_93C86_Write(uint16_t address, uint16_t data);
EEPROM_STATUS EEPROM_93C86_WriteMultipleWords(uint16_t startAddress, uint16_t* buffer, uint16_t length);

EEPROM_STATUS EEPROM_93C86_Erase(uint16_t address);
EEPROM_STATUS EEPROM_93C86_EraseAll(void);

void EEPROM_Test(void);




// Memory Map addresses
#define EEPROM_CONFIG_FLAG_ADDRESS                          0x0000
#define EEPROM_CONFIG_VERSION_ADDRESS                       0x0001
#define EEPROM_CONFIG_COUNT_ADDRESS							            0x0002
#define EEPROM_SERIAL_NUMBER_ADDRESS                        0x0003
#define EEPROM_SERIAL_NUMBER_LENGTH							            12          // 12 WORDS - 24 BYTES
#define EEPROM_VEXUF_SERIAL_ADDRESS							            0x000F

#define EEPROM_REGISTERATION_NUMBER_ADDRESS                 0x0010
#define EEPROM_REGISTERATION_NUMBER_LENGTH					        5           // 5 WORDS - 10 BYTES
#define EEPROM_CALLSIGN_ADDRESS		                          0x0015
#define EEPROM_CALLSIGN_LENGTH								              10          // 11 WORDS - 22 BYTES

// 0x0020 to 0x002F reserved

#define	EEPROM_PWM1_ENABLED_ADDRESS							            0x0030
#define	EEPROM_PWM1_DEFAULT_ADDRESS							            0x0031
#define	EEPROM_PWM2_ENABLED_ADDRESS							            0x0032
#define	EEPROM_PWM2_DEFAULT_ADDRESS							            0x0033
// 0x0034 to 0x0036 reserved
#define EEPROM_SERIAL_INTERFACE_ADDRESS						          0x0037
#define EEPROM_I2C_ADDRESS_ADDRESS                          0x0038
#define EEPROM_I2C_TYPE_ADDRESS                             0x0039
#define EEPROM_LCD_ADDRESS_ADDRESS                          0x003A
#define EEPROM_LCD_TYPE_ADDRESS                             0x003B
#define EEPROM_LCD_PWM_ADDRESS                              0x003C
#define EEPROM_SPI_TYPE_ADDRESS                             0x003D
#define EEPROM_OUTPUT_ADDRESS								                0x003E
#define EEPROM_ACTUATORS_DEFAULT_ADDRESS         			      0x003F

// 0x0040 to 0x009F reserved

#define EEPROM_ALARM_ACTUATORS_ADDRESS						          0x00A0
#define EEPROM_ALARM_OUTPUT_ADDRESS							            0x00A1
#define EEPROM_ALARM_PWM1_ADDRESS							              0x00A2
#define EEPROM_ALARM_PWM2_ADDRESS							              0x00A3
#define EEPROM_ALARM_TNC_ENABLED_ADDRESS					          0x00A4
#define EEPROM_ALARM_TNC_PATH_ADDRESS						            0x00A5
#define EEPROM_ALARM_TNC_MSG_ADDRESS						            0x00A6
// 0x00A7 reserved
#define EEPROM_ALARM_SHIFT									                8			// There are 2 alarms



#define EEPROM_AV_ENABLED_ADDRESS                        	  0x00B0
#define EEPROM_AV_LED_ADDRESS	                        	    0x00B1
// 0X00B2 TO 0X00B4 reserved
#define EEPROM_AV_SLOW_RULE_ENABLED_ADDRESS                 0x00B5
#define EEPROM_AV_SLOW_RULE_MIN_ADDRESS                     0x00B6
#define EEPROM_AV_SLOW_RULE_MAX_ADDRESS                     0x00B7
// 0x00B8 reserved
#define EEPROM_AV_FAST_RULE_ENABLED_ADDRESS                 0x00B9
#define EEPROM_AV_FAST_RULE_MIN_ADDRESS                     0x00BA
#define EEPROM_AV_FAST_RULE_MAX_ADDRESS                     0x00BB
// 0x00BC reserved
#define EEPROM_AV_ON_RULE_ENABLED_ADDRESS                   0x00BD
#define EEPROM_AV_ON_RULE_MIN_ADDRESS                       0x004E
#define EEPROM_AV_ON_RULE_MAX_ADDRESS                       0x004F
#define EEPROM_AV_SHIFT                                     16			// there are 3 AVs


#define EEPROM_TRIG_STATUS_ADDRESS                          0x00E0
#define EEPROM_TRIG_FROM_VALUE_ADDRESS                      0x00E1
#define EEPROM_TRIG_TO_VALUE_ADDRESS                        0x00E2
#define EEPROM_TRIG_TEST_ADDRESS                            0x00E3
#define EEPROM_TRIG_ACTUATORS_ADDRESS                       0x00E4
#define EEPROM_TRIG_OUTPUT_ADDRESS                          0x00E5
#define EEPROM_TRIG_PWM_ADDRESS                           	0x00E6		// 2 words (FOR PWM1 AND PWM 2)
#define EEPROM_TRIG_TNC1_ADDRESS                           	0x00E8		// 4 words (ENABLED, PATH, MESSAGE, RESERVED)
#define EEPROM_TRIG_TNC2_ADDRESS                           	0x00EC		// 4 words (ENABLED, PATH, MESSAGE, RESERVED)
#define EEPROM_TRIG_SHIFT                                   16			// There are 25 triggers


#define EEPROM_TNC_MESSAGE_ADDRESS							            0x0270
#define EEPROM_TNC_MESSAGE_LENGTH_SHIFT						          32
#define EEPROM_TNC_MESSAGE_COUNT							              10

#define EEPROM_TNC_PATH_ADDRESS								              0x03B0
#define EEPROM_TNC_PATH_LENGTH_SHIFT						            16
#define EEPROM_TNC_PATH_COUNT								                5





#endif /* INC_93C86_H_ */