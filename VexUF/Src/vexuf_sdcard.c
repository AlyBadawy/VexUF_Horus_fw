/**
 ******************************************************************************
 * @file          : vexuf_sd_card.c
 * @brief        : Brief description
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

/* Includes -----------------------------------------------------------------*/
#include "vexuf_sdcard.h"

#include "fatfs.h"
#include "sdio.h"

/* TypeDef ------------------------------------------------------------------*/

/* Defines -------------------------------------------------------------------*/

/* Macros -------------------------------------------------------------------*/

/* Extern Variables ---------------------------------------------------------*/
extern VexufStatus vexufStatus;
extern SD_HandleTypeDef hsd;

/* Variables ----------------------------------------------------------------*/
static FATFS FatFs;
static FATFS *FS_Ptr;
static DWORD FreeClusters;

/* Prototypes ---------------------------------------------------------------*/
UF_STATUS SDCard_prepareFS();
UF_STATUS SDCARD_createFolderIfNotExist(char *folderName);
void resetSDCard(void);

/* Code ---------------------------------------------------------------------*/
UF_STATUS SDCard_MountFS() {
  char cardLabel[12];
  // resetSDCard();

  if (f_mount(&FatFs, SDPath, 1) != FR_OK) {
    printf("Mount failed\n");
  }

  if (f_getlabel(SDPath, cardLabel, NULL) != FR_OK) return UF_ERROR;
  if (strncmp("VEXUF_HORUS", cardLabel, 12) != 0 &&
      f_setlabel("VEXUF_HORUS") != FR_OK)
    return UF_ERROR;

  if (SDCard_prepareFS() != UF_OK) return UF_ERROR;

  vexufStatus.sdCardMounted = 1;

  return UF_OK;
}

UF_STATUS SDCard_GetCardSize(float *totalSize, float *freeSize) {
  if (f_getfree("", &FreeClusters, &FS_Ptr) != FR_OK) return UF_ERROR;

  uint64_t total_sectors = (uint64_t)(FS_Ptr->n_fatent - 2) * FS_Ptr->csize;
  uint64_t free_sectors = (uint64_t)FreeClusters * FS_Ptr->csize;

  *totalSize = (float)(total_sectors * 512) /
               (1024 * 1024 * 1024);  // Convert to gigabytes
  *freeSize = (float)(free_sectors * 512) /
              (1024 * 1024 * 1024);  // Convert to gigabytes

  return UF_OK;
}

UF_STATUS SDCard_checkCard(void) {
  GPIO_PinState sdio_det = HAL_GPIO_ReadPin(SDIO_DET_GPIO_Port, SDIO_DET_Pin);

  if (sdio_det == GPIO_PIN_SET) {
    if (vexufStatus.sdCardEjected) {
      // The card is marked to be ejected and is still ejected.
      return UF_CARD_EJECTED;
    } else {
      // The card is marked present, but it's no longer present
      vexufStatus.sdCardEjected = 1;
      f_mount(NULL, (TCHAR const *)"", 1);  // Unmount the filesystem
      return UF_CARD_EJECTED;
    }
  } else {
    if (!vexufStatus.sdCardEjected) {
      // The card is marked to be present and is still present.
      if (vexufStatus.sdCardMounted == 1) return SDCARD_hasEnoughSpace();
      if (SDCard_MountFS() != UF_OK) {
        vexufStatus.sdCardError = 1;
        return UF_ERROR;
      }
    } else {
      // The card is marked not to be present, but now is present.
      // Attempt to mount the SD card
      if (vexufStatus.sdCardMounted == 0) {
        if (SDCard_MountFS() != UF_OK) {
          vexufStatus.sdCardError = 1;
          return UF_ERROR;
        }
      }
      vexufStatus.sdCardMounted = 1;
      vexufStatus.sdCardEjected = 0;
      UF_STATUS status = SDCARD_hasEnoughSpace();
      if (status != UF_OK) return status;
      vexufStatus.sdCardError = 0;
    }
  }
  return UF_OK;
}
UF_STATUS SDCARD_hasEnoughSpace(void) {
  float totalSize, freeSize;
  if (SDCard_GetCardSize(&totalSize, &freeSize) != UF_OK) {
    vexufStatus.sdCardError = 1;
    return UF_ERROR;
  }
  if (freeSize < 0.05) {  // Less than 50MB
    vexufStatus.sdCardError = 0;
    vexufStatus.sdCardFull = 1;
    return UF_CARD_FULL;
  }
  vexufStatus.sdCardFull = 0;
  vexufStatus.sdCardError = 0;
  return UF_OK;
}

/* Private Methods -----------------------------------------------------------*/
UF_STATUS SDCard_prepareFS() {
  char *startupFolder = "/startup";
  char *avFolder = "/av";
  char *logFolder = "/log";
  char *configFolder = "/config";
  char *dataFolder = "/data";

  if (SDCARD_createFolderIfNotExist(startupFolder) != UF_OK) return UF_ERROR;
  if (SDCARD_createFolderIfNotExist(avFolder) != UF_OK) return UF_ERROR;
  if (SDCARD_createFolderIfNotExist(logFolder) != UF_OK) return UF_ERROR;
  if (SDCARD_createFolderIfNotExist(configFolder) != UF_OK) return UF_ERROR;
  if (SDCARD_createFolderIfNotExist(dataFolder) != UF_OK) return UF_ERROR;

  return UF_OK;
}

UF_STATUS SDCARD_createFolderIfNotExist(char *folderName) {
  FRESULT res;
  DIR dir;

  res = f_opendir(&dir, folderName);
  if (res == FR_OK) {
    // Folder exists
    f_closedir(&dir);
  } else if (res == FR_NO_PATH) {
    if (f_mkdir(folderName) != FR_OK) return UF_ERROR;
  } else {
    return UF_ERROR;
  }
  return UF_OK;
}

void resetSDCard(void) {
  f_mount(NULL, (TCHAR const *)"", 1);  // Unmount the filesystem
  vexufStatus.sdCardMounted = 0;
  vexufStatus.sdCardEjected = 0;
  vexufStatus.sdCardError = 0;
  vexufStatus.sdCardFull = 0;
  f_mount(NULL, SDPath, 1);
  HAL_SD_DeInit(&hsd);
  __HAL_RCC_SDIO_CLK_DISABLE();
  HAL_Delay(10);
  __HAL_RCC_SDIO_FORCE_RESET();
  HAL_Delay(10);  // Small delay
  __HAL_RCC_SDIO_RELEASE_RESET();
  __HAL_RCC_SDIO_CLK_ENABLE();
  HAL_Delay(10);
  if (HAL_SD_Init(&hsd) != HAL_OK) {
    printf("SDIO Init failed\n");
  }
  if (HAL_SD_ConfigWideBusOperation(&hsd, SDIO_BUS_WIDE_4B) != HAL_OK) {
    printf("SDIO Config failed\n");
  }
}