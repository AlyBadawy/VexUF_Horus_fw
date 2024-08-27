#ifndef VEXUF_SD_CARD_H_
#define VEXUF_SD_CARD_H_

#include "vexuf_helpers.h"

typedef enum {
  SDCARD_OK,
  SDCARD_ERROR,
  SDCARD_EJECTED,
  SDCARD_FULL
} SDCARD_STATUS;

SDCARD_STATUS SDCard_MountFS();
SDCARD_STATUS SDCard_GetCardSize(float *totalSize, float *freeSize);
SDCARD_STATUS SDCard_checkCard(void);
SDCARD_STATUS SDCARD_hasEnoughSpace(void);

void SDCard_Test(void);

#endif  // VEXUF_SD_CARD_H_