#ifndef SD_CARD_BSP_H
#define SD_CARD_BSP_H

#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

// Mount the onboard microSD card using Waveshare's ESP-IDF SDMMC/VFS method.
bool sd_card_Init(void);

#ifdef __cplusplus
}
#endif

#endif
