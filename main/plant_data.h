#pragma once

#include <stddef.h>

typedef struct {
    float temp_c;
    float humidity;
    float light_pct;
    float soil_pct;
    char temp_status[16];
    char humidity_status[16];
    char light_status[16];
    char soil_status[16];
} plant_readings_t;

void plant_data_init(void);
void plant_data_update(float temp_c, float humidity, float light_pct, float soil_pct,
                       const char *tstat, const char *hstat, const char *lstat, const char *sstat);
void plant_data_get(plant_readings_t *out);
