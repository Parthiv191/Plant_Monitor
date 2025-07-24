#ifndef ANALYSIS_H
#define ANALYSIS_H

#include <esp_log.h>

void analyse_data(float *temperature, float *humidity, float *light, float *soil);

void print_analysis(float *temperature, float *humidity, float *light, float *soil);

#endif // ANALYSIS_H