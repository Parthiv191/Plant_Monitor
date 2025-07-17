#ifndef ANALYSIS_H
#define ANALYSIS_H

#include <esp_log.h>

void analyse_data(float *temperature, float *humidity, float *light);

void print_analysis(float *temperature, float *humidity, float *light);

#endif // ANALYSIS_H