#include "analysis.h"
#include <esp_log.h>
#include <stdint.h>
#include "esp_mac.h"

static const char *TAG = "";

char temp_status[6];
char light_status[6];
char humidity_status[6];
char soil_status[6];

void analyse_data(float *temperature, float *humidity, float *light, float *soil) {
    if (*temperature > 24) {
        snprintf(temp_status, sizeof(temp_status), "%s\n", "High");
    } else if (*temperature < 18) {
        snprintf(temp_status, sizeof(temp_status), "%s\n", "Low");
    } else {
        snprintf(temp_status, sizeof(temp_status), "%s\n", "Good");
    }

    if (*humidity > 60) {
        snprintf(humidity_status, sizeof(humidity_status), "%s\n", "High");
    } else if (*humidity < 40) {
        snprintf(humidity_status, sizeof(humidity_status), "%s\n", "Low");
    } else {
        snprintf(humidity_status, sizeof(humidity_status), "%s\n", "Good");
    }

    if (*soil < 20) {
        snprintf(soil_status, sizeof(soil_status), "%s\n", "Dry");
    } else if (*soil > 60) {
        snprintf(soil_status, sizeof(soil_status), "%s\n", "Wet");
    } else {
        snprintf(soil_status, sizeof(soil_status), "%s\n", "Good");
    }

    if (*light < 20) {
        snprintf(light_status, sizeof(light_status), "%s\n", "Low");
    } else if (*light > 50) {
        snprintf(light_status, sizeof(light_status), "%s\n", "High");
    } else {
        snprintf(light_status, sizeof(light_status), "%s\n", "Good");
    }

    print_analysis(temperature, humidity, light, soil);
}


void print_analysis(float *temperature, float *humidity, float *light, float *soil) {
    //convert the temperature from celsius to fahrenheit
    float temp_fahrenheit = (*temperature * 9.0f / 5.0f) + 32.0f;
    ESP_LOGI(TAG, "ANALYSIS:");
    ESP_LOGI(TAG, "Temperature: %.2f °C (%.2f °F) Status: %s", *temperature, temp_fahrenheit, temp_status);
    ESP_LOGI(TAG, "Humidity: %.2f %% Status: %s", *humidity, humidity_status);
    ESP_LOGI(TAG, "Light: %.2f Status: %s", *light, light_status);
    ESP_LOGI(TAG, "Soil: %.2f Status: %s", *soil, soil_status);
}