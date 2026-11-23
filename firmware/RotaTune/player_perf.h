#pragma once
// v0.16.7 diagnostics: timing of the LVGL task and touch polling.
// Kept in its own header (included by lcd_bsp.c and the sketch) so the
// sketch does not depend on which copy of lcd_bsp.h the compiler finds.
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
  uint32_t max_handler_us;     // longest single lv_timer_handler() run
  uint32_t max_touch_gap_us;   // longest gap between two touch reads
  uint32_t max_lock_wait_us;   // longest wait in lcd_lvgl_lock() (callers outside LVGL)
  uint32_t handler_runs;       // lv_timer_handler() runs in the interval
  uint32_t touch_errors;       // CST816 I2C read errors since boot
} lcd_perf_t;

// Copies the figures and resets the interval maxima. Defined in lcd_bsp.c.
void lcd_perf_take(lcd_perf_t *out);

#ifdef __cplusplus
}
#endif
