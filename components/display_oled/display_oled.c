#include "display_oled.h"
#include <stdio.h>
#include <string.h>
#include <i2cdev.h>
#include <ssd1306.h>
#include <esp_log.h>

#define I2C_PORT   I2C_NUM_0
#define OLED_ADDR  SSD1306_I2C_ADDR_0 // 0x3C
#define PIN_SDA    21
#define PIN_SCL    22

static const char *TAG = "OLED";
static ssd1306_t dev;

void oled_init(void)
{
    ESP_ERROR_CHECK(i2cdev_init());

    memset(&dev, 0, sizeof(ssd1306_t));
    dev.protocol = SSD1306_PROTO_I2C;
    dev.i2c_dev.port = I2C_PORT;
    dev.i2c_dev.addr = OLED_ADDR;
    dev.i2c_dev.cfg.sda = PIN_SDA;
    dev.i2c_dev.cfg.scl = PIN_SCL;
    dev.i2c_dev.cfg.clock_speed = 400000;

    ESP_ERROR_CHECK(ssd1306_init(&dev, 128, 64));
    ssd1306_set_whole_display_lighting(&dev, false);
    ssd1306_set_contrast(&dev, 0x7F);
    ssd1306_clear_screen(&dev, false);
    ssd1306_draw_string(&dev, 0, 0, "Plant Monitor", 12, true);
    ssd1306_refresh(&dev, true);
    ESP_LOGI(TAG, "SSD1306 ready");
}

void oled_clear(void)
{
    ssd1306_clear_screen(&dev, false);
    ssd1306_refresh(&dev, true);
}

void oled_show_readings(float t, float h, float l, float s,
                        const char* tstat, const char* hstat,
                        const char* lstat, const char* sstat)
{
    char line1[32], line2[32], line3[32], line4[32];

    // 6×8 font fits ~21 chars/line at 128 px width
    snprintf(line1, sizeof(line1), "T: %.1fC (%s)", t, tstat);
    snprintf(line2, sizeof(line2), "H: %.1f%% (%s)", h, hstat);
    snprintf(line3, sizeof(line3), "L: %.1f%% (%s)", l, lstat);
    snprintf(line4, sizeof(line4), "S: %.1f%% (%s)", s, sstat);

    ssd1306_clear_screen(&dev, false);
    ssd1306_draw_string(&dev, 0, 0,  line1, 12, true);
    ssd1306_draw_string(&dev, 0, 16, line2, 12, true);
    ssd1306_draw_string(&dev, 0, 32, line3, 12, true);
    ssd1306_draw_string(&dev, 0, 48, line4, 12, true);
    ssd1306_refresh(&dev, true);
}
