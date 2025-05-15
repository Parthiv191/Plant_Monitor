#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include <dht.h>  // Include DHT11 driver header

// Define which GPIO the DHT11 data line is connected to
#define DHT_GPIO GPIO_NUM_4

static const char *TAG = "DHT11";

// This task runs repeatedly to read data from the DHT11 sensor
void dht_task(void *pvParameter)
{
    int16_t temperature = 0;
    int16_t humidity = 0;

    while (1) {
        // Read data from DHT11 sensor
        esp_err_t result = dht_read_data(DHT_TYPE_DHT11, DHT_GPIO, &humidity, &temperature);

        if (result == ESP_OK) {
            ESP_LOGI(TAG, "Humidity: %d%%  Temperature: %d°C", humidity, temperature);
        } else {
            ESP_LOGE(TAG, "Failed to read from sensor: %s", esp_err_to_name(result));
        }

        // Wait 2 seconds between reads (DHT11 has a 1Hz limit)
        vTaskDelay(pdMS_TO_TICKS(2000));
    }
}

void app_main(void)
{
    ESP_LOGI(TAG, "Starting DHT11 Sensor Reading Task");

    // Create the DHT11 reading task on core 1
    xTaskCreate(&dht_task, "dht_task", 2048, NULL, 5, NULL);
}