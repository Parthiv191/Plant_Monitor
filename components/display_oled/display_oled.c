#include "display_oled.h"
#include "ssd1306_min.h"
#include <stdio.h>

void oled_init(void)
{
    ssd1306_init();
    ssd1306_clear(true);
    ssd1306_set_cursor(0, 0);
    ssd1306_print("Plant Monitor\n");
    ssd1306_print("OLED Ready\n");
    ssd1306_refresh();
}

void oled_clear(void)
{
    ssd1306_clear(true);
}

void oled_show_readings(float t, float h, float l, float s,
                        const char* tstat, const char* hstat,
                        const char* lstat, const char* sstat)
{
    ssd1306_clear(false);

    ssd1306_set_cursor(0, 0);
    ssd1306_printf("T: %.1fC (%s)", t, tstat);

    ssd1306_set_cursor(0, 2);
    ssd1306_printf("H: %.1f%% (%s)", h, hstat);

    ssd1306_set_cursor(0, 4);
    ssd1306_printf("L: %.1f%% (%s)", l, lstat);

    ssd1306_set_cursor(0, 6);
    ssd1306_printf("S: %.1f%% (%s)", s, sstat);

    ssd1306_refresh();
}