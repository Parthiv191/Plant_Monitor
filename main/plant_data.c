#include "plant_data.h"
#include <freertos/FreeRTOS.h>
#include <freertos/semphr.h>
#include <string.h>

static SemaphoreHandle_t s_mutex;
static plant_readings_t s_data;

static void strip_copy(char *dst, size_t dst_size, const char *src)
{
    size_t j = 0;
    for (size_t i = 0; src[i] && j < dst_size - 1; ++i) {
        if (src[i] != '\n' && src[i] != '\r') {
            dst[j++] = src[i];
        }
    }
    dst[j] = '\0';
}

void plant_data_init(void)
{
    s_mutex = xSemaphoreCreateMutex();
    memset(&s_data, 0, sizeof(s_data));
}

void plant_data_update(float temp_c, float humidity, float light_pct, float soil_pct,
                       const char *tstat, const char *hstat, const char *lstat, const char *sstat)
{
    if (s_mutex == NULL) {
        return;
    }
    if (xSemaphoreTake(s_mutex, portMAX_DELAY) != pdTRUE) {
        return;
    }
    s_data.temp_c = temp_c;
    s_data.humidity = humidity;
    s_data.light_pct = light_pct;
    s_data.soil_pct = soil_pct;
    strip_copy(s_data.temp_status, sizeof(s_data.temp_status), tstat);
    strip_copy(s_data.humidity_status, sizeof(s_data.humidity_status), hstat);
    strip_copy(s_data.light_status, sizeof(s_data.light_status), lstat);
    strip_copy(s_data.soil_status, sizeof(s_data.soil_status), sstat);
    xSemaphoreGive(s_mutex);
}

void plant_data_get(plant_readings_t *out)
{
    if (out == NULL || s_mutex == NULL) {
        return;
    }
    if (xSemaphoreTake(s_mutex, portMAX_DELAY) != pdTRUE) {
        return;
    }
    *out = s_data;
    xSemaphoreGive(s_mutex);
}
