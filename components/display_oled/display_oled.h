#pragma once
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

void oled_init(void);  // init I2C + SSD1306
void oled_clear(void);

// show one page of text (auto-clears & writes 4 lines)
void oled_show_readings(float temp_c, float humidity,
                        float light_pct, float soil_pct,
                        const char* temp_status,
                        const char* hum_status,
                        const char* light_status,
                        const char* soil_status);

#ifdef __cplusplus
}
#endif
