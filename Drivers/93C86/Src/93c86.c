/**
 ******************************************************************************
 * @file          : 93c86.c
 * @brief         : 93C86 EEPROM Implementation
 ******************************************************************************
 * @attention
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 *
 ******************************************************************************
 * @copyright     : Aly Badawy
 * @author website: https://alybadawy.com
 ******************************************************************************
 */

/* Includes ------------------------------------------------------------------*/
#include "93c86.h"

/* TypeDef -------------------------------------------------------------------*/
static SPI_HandleTypeDef hspi;
static GPIO_TypeDef* eeprom_cs_port;
/* Defines -------------------------------------------------------------------*/

/* Macros --------------------------------------------------------------------*/
// Define macro functions here

/* Extern Variables ----------------------------------------------------------*/
// Declare external variables here

/* Variables -----------------------------------------------------------------*/
static uint16_t eeprom_cs_pin;
int16_t MEMrData[1];
int16_t MEMsData[1];

/* Prototypes ----------------------------------------------------------------*/
void EEPROM_93C86_CS_UNSELECT(void);
void EEPROM_93C86_CS_SELECT(void);
UF_STATUS EEPROM_93C86_getStatus(void);
UF_STATUS EEPROM_93C86_SendCommand(uint16_t command);
UF_STATUS EEPROM_93C86_WriteEnable(void);
UF_STATUS EEPROM_93C86_WriteDisable(void);
UF_STATUS EEPROM_93C86_TransmitReceive(uint8_t tx, uint8_t* rx);

/* Code ----------------------------------------------------------------------*/
void EEPROM_93C86_init(SPI_HandleTypeDef* spi, GPIO_TypeDef* cs_port,
                       uint16_t cs_pin) {
  hspi = *spi;
  eeprom_cs_port = cs_port;
  eeprom_cs_pin = cs_pin;
}

UF_STATUS EEPROM_93C86_Read(uint16_t address, uint16_t* data) {
  UF_STATUS status = UF_OK;

  while (EEPROM_93C86_getStatus() == UF_BUSY);

  uint16_t command =
      EEPROM_CMD_READ | (address & 0x03FF);  // Ensure address is 10 bits
  uint8_t temp = 0;
  *data = 0;  // Initialize the data buffer

  EEPROM_93C86_CS_SELECT();
  status = EEPROM_93C86_SendCommand(command);
  if (status != UF_OK) {
    EEPROM_93C86_CS_UNSELECT();
    return status;
  }

  // Receive the first byte which contains the dummy bit and the high part of
  // the data
  status = EEPROM_93C86_TransmitReceive(0x00, &temp);
  if (status != UF_OK) {
    EEPROM_93C86_CS_UNSELECT();
    return status;
  }
  *data |= (temp & 0x7F) << 9;  // Mask out the dummy bit and shift left to
                                // align with the 16-bit data

  // Receive the second byte and combine it with the first byte's data
  status = EEPROM_93C86_TransmitReceive(0x00, &temp);
  if (status != UF_OK) {
    EEPROM_93C86_CS_UNSELECT();
    return status;
  }
  *data |= temp << 1;

  // Receive the remaining 7 bits of data
  status = EEPROM_93C86_TransmitReceive(0x00, &temp);
  if (status != UF_OK) {
    EEPROM_93C86_CS_UNSELECT();
    return status;
  }
  *data |= (temp >> 7);

  HAL_Delay(1);  // Ensure some delay as per the datasheet
  EEPROM_93C86_CS_UNSELECT();

  return status;
}
UF_STATUS EEPROM_93C86_ReadMultipleWords(uint16_t startAddress,
                                         uint16_t* buffer, uint16_t length) {
  UF_STATUS status = UF_OK;
  for (uint16_t i = 0; i < length; i++) {
    status = EEPROM_93C86_Read(startAddress + i, &buffer[i]);
    if (status != UF_OK)
      return status;  // Return immediately if any read operation fails
  }

  return status;  // Return OK if all read operations were successful
}

UF_STATUS EEPROM_93C86_Write(uint16_t address, uint16_t data) {
  UF_STATUS status = UF_OK;

  while (EEPROM_93C86_getStatus() == UF_BUSY);

  uint16_t command =
      EEPROM_CMD_WRITE | (address & 0x03FF);  // Ensure address is 10 bits
  uint8_t temp = 0;

  status = EEPROM_93C86_WriteEnable();
  if (status != UF_OK) return status;

  EEPROM_93C86_CS_SELECT();
  status = EEPROM_93C86_SendCommand(command);
  if (status != UF_OK) {
    EEPROM_93C86_CS_UNSELECT();
    return status;
  }

  // Send the high byte of the 16-bit data
  temp = (data >> 8) & 0xFF;
  status = EEPROM_93C86_TransmitReceive(temp, &temp);
  if (status != UF_OK) {
    EEPROM_93C86_CS_UNSELECT();
    return status;
  }
  // Send the low byte of the 16-bit data
  temp = data & 0xFF;
  status = EEPROM_93C86_TransmitReceive(temp, &temp);
  if (status != UF_OK) {
    EEPROM_93C86_CS_UNSELECT();
    return status;
  }

  EEPROM_93C86_CS_UNSELECT();
  HAL_Delay(2);  // Wait for write cycle to complete
  status = EEPROM_93C86_WriteDisable();

  return status;
}
UF_STATUS EEPROM_93C86_WriteMultipleWords(uint16_t startAddress,
                                          uint16_t* buffer, uint16_t length) {
  UF_STATUS status = UF_OK;
  for (uint16_t i = 0; i < length; i++) {
    status = EEPROM_93C86_Write(startAddress + i, buffer[i]);
    if (status != UF_OK)
      return status;  // Return immediately if any read operation fails
  }

  return status;
}
UF_STATUS EEPROM_93C86_WriteAll(uint16_t data) {
  UF_STATUS status = UF_OK;

  while (EEPROM_93C86_getStatus() == UF_BUSY);

  uint8_t temp = 0;

  status = EEPROM_93C86_WriteEnable();
  if (status != UF_OK) return status;

  EEPROM_93C86_CS_SELECT();
  status = EEPROM_93C86_SendCommand(EEPROM_CMD_WRAL);
  if (status != UF_OK) {
    EEPROM_93C86_CS_UNSELECT();
    return status;
  }

  // Send the high byte of the 16-bit data
  temp = (data >> 8) & 0xFF;
  status = EEPROM_93C86_TransmitReceive(temp, &temp);
  if (status != UF_OK) {
    EEPROM_93C86_CS_UNSELECT();
    return status;
  }
  // Send the low byte of the 16-bit data
  temp = data & 0xFF;
  status = EEPROM_93C86_TransmitReceive(temp, &temp);
  if (status != UF_OK) {
    EEPROM_93C86_CS_UNSELECT();
    return status;
  }

  EEPROM_93C86_CS_UNSELECT();
  HAL_Delay(2);  // Wait for write cycle to complete
  status = EEPROM_93C86_WriteDisable();

  return status;
}
UF_STATUS EEPROM_93C86_Erase(uint16_t address) {
  UF_STATUS status = UF_OK;

  // Wait until EEPROM is not busy
  while (EEPROM_93C86_getStatus() == UF_BUSY);

  uint16_t command =
      EEPROM_CMD_ERASE | (address & 0x03FF);  // Ensure address is 10 bits
  status = EEPROM_93C86_WriteEnable();
  if (status != UF_OK) return UF_ERROR;

  EEPROM_93C86_CS_SELECT();
  status = EEPROM_93C86_SendCommand(command);
  EEPROM_93C86_CS_UNSELECT();
  if (status != UF_OK) return UF_ERROR;

  HAL_Delay(2);  // Wait for erase cycle to complete
  status = EEPROM_93C86_WriteDisable();
  if (status != UF_OK) return UF_ERROR;

  return UF_OK;
}
UF_STATUS EEPROM_93C86_EraseAll(void) {
  UF_STATUS status = UF_OK;

  // Wait until EEPROM is not busy
  while (EEPROM_93C86_getStatus() == UF_BUSY);

  status = EEPROM_93C86_WriteEnable();
  if (status != UF_OK) return status;
  EEPROM_93C86_CS_SELECT();
  status = EEPROM_93C86_SendCommand(EEPROM_CMD_ERASE_ALL);
  EEPROM_93C86_CS_UNSELECT();
  if (status != UF_OK) return status;
  HAL_Delay(2);  // Wait for erase cycle to complete
  status = EEPROM_93C86_WriteDisable();
  return status;
}

void EEPROM_Test(void) {
  // EEPROM READ TEST
  // Example usage
  uint16_t data;
  EEPROM_93C86_Read(0xFFEE, &data);
  printf("EEprom says: %04X\r\n", data);
  data = 0xABCD;
  EEPROM_93C86_Write(0xFFEE, data);
  EEPROM_93C86_Read(0xFFEE, &data);
  printf("EEprom says: %04X\r\n", data);

  uint16_t buffer[0x3FF];
  for (uint16_t address = 0; address < 0x3FF; address++) {
    EEPROM_93C86_Read(address, &buffer[address]);
  }
  for (uint16_t address = 0; address < 0x3FF; address += 16) {
    printf("%10X: ", address);
    // Print first column (8 words)
    for (uint16_t i = 0; i < 8; i++) {
      printf("%04X ", buffer[address + i]);
    }

    // Print separator
    printf("    ");

    // Print second column (8 words)
    for (uint16_t i = 8; i < 16; i++) {
      printf("%04X ", buffer[address + i]);
    }

    // New line
    printf("\r\n");
  }

  char writeData[25] = "Hello, EEPROM! Testing";
  char readData[25];
  uint16_t writeBuffer[13];  // 25 characters will be stored in 13 words (16-bit
                             // each)
  uint16_t readBuffer[13];

  // Convert char array to uint16_t array
  for (int i = 0; i < 12; i++) {
    writeBuffer[i] = (writeData[2 * i] << 8) | writeData[2 * i + 1];
  }
  // Handle the last character (if array size is odd)
  writeBuffer[12] = writeData[24];

  // Write to EEPROM
  EEPROM_93C86_WriteMultipleWords(0x0000, writeBuffer, 13);

  // Read from EEPROM
  EEPROM_93C86_ReadMultipleWords(0x0000, readBuffer, 13);

  // Convert uint16_t array back to char array
  for (int i = 0; i < 12; i++) {
    readData[2 * i] = (readBuffer[i] >> 8) & 0xFF;
    readData[2 * i + 1] = readBuffer[i] & 0xFF;
  }
  // Handle the last character (if array size is odd)
  readData[24] = readBuffer[12] & 0xFF;

  // Print the read data
  printf("Read Data: %s\r\n", readData);
}

/* Private Methods -----------------------------------------------------------*/
void EEPROM_93C86_CS_UNSELECT(void) {
  HAL_GPIO_WritePin(eeprom_cs_port, eeprom_cs_pin, GPIO_PIN_RESET);
}
void EEPROM_93C86_CS_SELECT(void) {
  HAL_GPIO_WritePin(eeprom_cs_port, eeprom_cs_pin, GPIO_PIN_SET);
}
UF_STATUS EEPROM_93C86_getStatus(void) {
  EEPROM_93C86_CS_SELECT();
  UF_STATUS status =
      (HAL_GPIO_ReadPin(SPI_MISO_GPIO_Port, SPI_MISO_Pin) == GPIO_PIN_RESET)
          ? UF_BUSY
          : UF_OK;
  EEPROM_93C86_CS_UNSELECT();
  return status;
}
UF_STATUS EEPROM_93C86_SendCommand(uint16_t command) {
  uint8_t receivedData;
  UF_STATUS status = UF_OK;
  uint8_t cmdHigh = (command >> 8) & 0xFF;
  uint8_t cmdLow = command & 0xFF;
  status = EEPROM_93C86_TransmitReceive(cmdHigh, &receivedData);
  if (status != UF_OK) return status;
  status = EEPROM_93C86_TransmitReceive(cmdLow, &receivedData);

  return status;
}
UF_STATUS EEPROM_93C86_WriteEnable(void) {
  // Wait until EEPROM is not busy
  while (EEPROM_93C86_getStatus() == UF_BUSY);

  EEPROM_93C86_CS_SELECT();
  UF_STATUS status = EEPROM_93C86_SendCommand(EEPROM_CMD_WREN);
  HAL_Delay(1);  // Ensure some delay as per datasheet
  EEPROM_93C86_CS_UNSELECT();

  return status;
}
UF_STATUS EEPROM_93C86_WriteDisable(void) {
  while (EEPROM_93C86_getStatus() == UF_BUSY);

  EEPROM_93C86_CS_SELECT();
  UF_STATUS status = EEPROM_93C86_SendCommand(EEPROM_CMD_WRDI);
  HAL_Delay(1);
  EEPROM_93C86_CS_UNSELECT();

  return status;
}
UF_STATUS EEPROM_93C86_TransmitReceive(uint8_t tx, uint8_t* rx) {
  if (HAL_SPI_TransmitReceive(&hspi, &tx, rx, 1, 150) != HAL_OK) {
    return UF_ERROR;
  };
  return UF_OK;
}
