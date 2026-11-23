#include <stdio.h>
#include <Arduino.h>

#include "sd_card_bsp.h"

#include "esp_err.h"
#include "esp_vfs_fat.h"
#include "sdmmc_cmd.h"
#include "driver/sdmmc_host.h"
#include "driver/gpio.h"

#define SDMMC_CMD_PIN   ((gpio_num_t)3)
#define SDMMC_D0_PIN    ((gpio_num_t)5)
#define SDMMC_D1_PIN    ((gpio_num_t)6)
#define SDMMC_D2_PIN    ((gpio_num_t)42)
#define SDMMC_D3_PIN    ((gpio_num_t)2)
#define SDMMC_CLK_PIN   ((gpio_num_t)4)

static const char *SD_MOUNT_POINT = "/sdcard";
static sdmmc_card_t *sdCard = nullptr;

bool sd_card_Init(void) {
  if (sdCard != nullptr) {
    Serial.println("SD card is already mounted.");
    return true;
  }

  esp_vfs_fat_sdmmc_mount_config_t mountConfig = {};
  mountConfig.format_if_mount_failed = false;
  mountConfig.max_files = 8;
  mountConfig.allocation_unit_size = 512;

  sdmmc_host_t host = SDMMC_HOST_DEFAULT();
  host.max_freq_khz = SDMMC_FREQ_HIGHSPEED;

  sdmmc_slot_config_t slotConfig = SDMMC_SLOT_CONFIG_DEFAULT();
  slotConfig.width = 4;
  slotConfig.clk = SDMMC_CLK_PIN;
  slotConfig.cmd = SDMMC_CMD_PIN;
  slotConfig.d0 = SDMMC_D0_PIN;
  slotConfig.d1 = SDMMC_D1_PIN;
  slotConfig.d2 = SDMMC_D2_PIN;
  slotConfig.d3 = SDMMC_D3_PIN;

  Serial.println("Mounting SD card with 4-bit SDMMC...");
  esp_err_t err = esp_vfs_fat_sdmmc_mount(
      SD_MOUNT_POINT, &host, &slotConfig, &mountConfig, &sdCard);

  if (err != ESP_OK || sdCard == nullptr) {
    Serial.printf("SDMMC mount error: %s (0x%x)\n",
                  esp_err_to_name(err), static_cast<unsigned int>(err));
    sdCard = nullptr;
    return false;
  }

  Serial.println("SD card mounted successfully.");
  sdmmc_card_print_info(stdout, sdCard);

  const uint64_t cardBytes =
      static_cast<uint64_t>(sdCard->csd.capacity) * 512ULL;
  Serial.printf("Card capacity: %llu MB\n",
                static_cast<unsigned long long>(cardBytes / (1024ULL * 1024ULL)));
  return true;
}
