#pragma once
#include <stdint.h>
#include <stdbool.h>
#include "driver/i2c.h"

#define SSD1306_ADDR        0x3C
#define SSD1306_WIDTH       128
#define SSD1306_HEIGHT      64

// Pins (change if you wired differently)
#define OLED_I2C_PORT       I2C_NUM_0
#define OLED_I2C_SDA        21
#define OLED_I2C_SCL        22
#define OLED_I2C_FREQ       400000

void ssd1306_init(void);
void ssd1306_clear(bool refresh);
void ssd1306_refresh(void);

// Basic text drawing (5x7 font, one extra column spacing)
void ssd1306_set_cursor(uint8_t x, uint8_t y); // x in pixels, y in rows of 8px (0..7)
void ssd1306_print(const char *s);             // prints at current cursor
void ssd1306_printf(const char *fmt, ...);     // tiny printf -> print()

// Low-level (rarely needed)
void ssd1306_cmd(uint8_t cmd);
void ssd1306_cmd2(uint8_t c1, uint8_t c2);
void ssd1306_data(const uint8_t *data, size_t len);