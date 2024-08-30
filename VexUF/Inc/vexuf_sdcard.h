#ifndef VEXUF_SD_CARD_H_
#define VEXUF_SD_CARD_H_

#include "vexuf.h"

UF_STATUS SDCard_MountFS();
UF_STATUS SDCard_GetCardSize(float *totalSize, float *freeSize);
UF_STATUS SDCard_checkCard(void);
UF_STATUS SDCARD_hasEnoughSpace(void);

void SDCard_Test(void);

#endif  // VEXUF_SD_CARD_H_