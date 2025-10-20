#pragma once
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

void oled_init(void);  // I2C + SSD1306 init (I2C0, SDA=21, SCL=22, addr=0x3C)
void oled_clear(void);

// Draw four lines with current readings + statuses
void oled_show_readings(float temp_c, float humidity,
                        float light_pct, float soil_pct,
                        const char* temp_status,
                        const char* hum_status,
                        const char* light_status,
                        const char* soil_status);

#ifdef __cplusplus
}
#endif