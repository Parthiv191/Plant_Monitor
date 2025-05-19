#include <stdio.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <dht.h>

#define SENSOR_TYPE DHT_TYPE_DHT11
#define DHT_GPIO_PIN GPIO_NUM_26

void dht_test(void *pvParameters)
{
    float temperature, humidity;

    while (1)
    {
        if (dht_read_float_data(SENSOR_TYPE, DHT_GPIO_PIN, &humidity, &temperature) == ESP_OK) 
        {
            float temp = (temperature * 9/5) + 32; // Convert to Fahrenheit
            printf("Humidity: %.1f%% Temp: %.1fF\n", humidity, temp);
        } 
        else 
        {
            printf("Could not read data from sensor\n");
        }

        vTaskDelay(pdMS_TO_TICKS(2000));
    }
}

void app_main(void)
{
    xTaskCreate(dht_test, "dht_test", configMINIMAL_STACK_SIZE * 3, NULL, 5, NULL);
}