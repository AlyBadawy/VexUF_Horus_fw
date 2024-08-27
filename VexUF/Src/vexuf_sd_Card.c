#include "vexuf_sd_card.h"

#include "fatfs.h"
#include "sdio.h"

extern VexufStatus vexufStatus;
extern SD_HandleTypeDef hsd;

FATFS FatFs;
FIL Fil;

FATFS *FS_Ptr;
UINT RWC, WWC;
DWORD FreeClusters;

SDCARD_STATUS SDCard_MountFS() {
  char cardLabel[12];
  // Add a delay to ensure the SD card is ready
  HAL_Delay(10);
  FRESULT rs = f_mount(NULL, "", 0);  // Unmount the filesystem
  HAL_Delay(30);                      // Short delay
  HAL_SD_DeInit(&hsd);
  __HAL_RCC_SDIO_FORCE_RESET();
  HAL_Delay(10);  // Short delay
  __HAL_RCC_SDIO_RELEASE_RESET();
  MX_SDIO_SD_Init();  // Reinitialize SDIO peripheral
  HAL_Delay(10);      // Short delay
  rs = f_mount(&FatFs, SDPath, 1);
  if (rs != FR_OK) return SDCARD_ERROR;

  if (f_getlabel("", cardLabel, NULL) != FR_OK) return SDCARD_ERROR;

  if (strncmp("VEXUF_HORUS", cardLabel, 12) != 0) {
    if (f_setlabel("VEXUF_HORUS") != FR_OK) return SDCARD_ERROR;
  }

  return SDCARD_OK;
}
SDCARD_STATUS SDCard_GetCardSize(float *totalSize, float *freeSize) {
  uint64_t total_sectors;
  uint64_t total_space;
  uint64_t free_sectors;
  uint64_t free_space;

  if (f_getfree("", &FreeClusters, &FS_Ptr) != FR_OK) return SDCARD_ERROR;

  total_sectors = (uint64_t)(FS_Ptr->n_fatent - 2) * FS_Ptr->csize;
  total_space = total_sectors * 512;  // Convert to bytes
  *totalSize =
      total_space / (1024 * 1024 * 1024);  // Convert to gigabytes (use float
                                           // for GB to handle large values)

  free_sectors =
      (uint64_t)FreeClusters * FS_Ptr->csize;  // Calculate free sectors
  free_space = free_sectors * 512;             // Convert to bytes

  *freeSize = (float)free_space /
              (1024 * 1024 * 1024);  // Convert to gigabytes (use float for GB
                                     // to handle large values)

  return SDCARD_OK;
}
SDCARD_STATUS SDCard_checkCard(void) {
  GPIO_PinState sdio_det = HAL_GPIO_ReadPin(SDIO_DET_GPIO_Port, SDIO_DET_Pin);
  if (vexufStatus.sdCardEjected && sdio_det == GPIO_PIN_SET) {
    // The card is marked to be ejected and is still ejected.
    return SDCARD_EJECTED;
  }

  if (!vexufStatus.sdCardEjected && sdio_det == GPIO_PIN_RESET) {
    // The card is marked to be present and is still present.
    if (vexufStatus.sdCardMounted == 1) return SDCARD_hasEnoughSpace();
  }

  if (!vexufStatus.sdCardEjected && sdio_det == GPIO_PIN_SET) {
    // The card is marked present, but it's no longer present
    vexufStatus.sdCardEjected = 1;
    f_mount(NULL, (TCHAR const *)"", 1);  // Unmount the filesystem
    return SDCARD_EJECTED;
  }

  // The card is marked not to be present, but now is present.
  // Attempt to mount the SD card
  if (vexufStatus.sdCardMounted == 0) {
    if (SDCard_MountFS() != SDCARD_OK) {
      vexufStatus.sdCardError = 1;
      return SDCARD_ERROR;
    }
  }
  vexufStatus.sdCardMounted = 1;
  vexufStatus.sdCardEjected = 0;

  SDCARD_STATUS status = SDCARD_hasEnoughSpace();
  if (status != SDCARD_OK) return status;

  vexufStatus.sdCardError = 0;
  return SDCARD_OK;
}
SDCARD_STATUS SDCARD_hasEnoughSpace(void) {
  float totalSize, freeSize;
  if (SDCard_GetCardSize(&totalSize, &freeSize) != SDCARD_OK) {
    vexufStatus.sdCardError = 1;
    return SDCARD_ERROR;
  }
  if (freeSize < 0.05) {  // Less than 50MB
    vexufStatus.sdCardError = 0;
    vexufStatus.sdCardFull = 1;
    return SDCARD_FULL;
  }
  vexufStatus.sdCardFull = 0;
  vexufStatus.sdCardError = 0;
  return SDCARD_OK;
}