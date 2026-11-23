#ifndef LCD_BSP_H
#define LCD_BSP_H

#include "Arduino.h"
#include <stdbool.h>
#include "lvgl.h"

#ifdef __cplusplus
extern "C" {
#endif

void lcd_lvgl_Init(void);
bool lcd_lvgl_lock(int timeout_ms);
void lcd_lvgl_unlock(void);

#ifdef __cplusplus
}
#endif

#endif
