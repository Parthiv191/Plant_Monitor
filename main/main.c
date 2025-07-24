#include <stdio.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <dht.h>
#include <analysis.h>
#include <esp_adc/adc_continuous.h>
#include <esp_log.h>

#define SENSOR_TYPE DHT_TYPE_DHT11
#define DHT_GPIO_PIN GPIO_NUM_26
#define LIGHT_SENSOR_CHANNEL ADC_CHANNEL_0 //GPIO_NUM_36  Pin 14
#define SOIL_SENSOR_CHANNEL ADC_CHANNEL_3 //GPIO_NUM_39 // Pin 15
#define ADC_UNIT ADC_UNIT_1
#define ADC_ATTEN ADC_ATTEN_DB_12

static const char *TAG = "PlantMonitor";

adc_continuous_handle_t handle = NULL;

static bool IRAM_ATTR s_conv_done_cb(adc_continuous_handle_t handle, const adc_continuous_evt_data_t *edata, void *user_data) {
    return false;
}

static void init_adc(void) {
    adc_continuous_handle_cfg_t adc_config = {
        .max_store_buf_size = 1024,
        .conv_frame_size = 256,
    };
    ESP_ERROR_CHECK(adc_continuous_new_handle(&adc_config, &handle));

    // Configure both channels
    adc_digi_pattern_config_t adc_patterns[2] = {
        {
            .atten = ADC_ATTEN,
            .channel = LIGHT_SENSOR_CHANNEL,
            .unit = ADC_UNIT,
            .bit_width = SOC_ADC_DIGI_MAX_BITWIDTH,
        },
        {
            .atten = ADC_ATTEN,
            .channel = SOIL_SENSOR_CHANNEL,
            .unit = ADC_UNIT,
            .bit_width = SOC_ADC_DIGI_MAX_BITWIDTH,
        }
    };

    adc_continuous_config_t dig_cfg = {
        .pattern_num = 2,
        .adc_pattern = adc_patterns,
        .sample_freq_hz = 20000,
        .conv_mode = ADC_CONV_SINGLE_UNIT_1,
        .format = ADC_DIGI_OUTPUT_FORMAT_TYPE1,
    };

    ESP_ERROR_CHECK(adc_continuous_config(handle, &dig_cfg));

    adc_continuous_evt_cbs_t cbs = {
        .on_conv_done = s_conv_done_cb,
        .on_pool_ovf = NULL,
    };
    ESP_ERROR_CHECK(adc_continuous_register_event_callbacks(handle, &cbs, NULL));
}

static void parse_adc_results(uint8_t *result, uint32_t length, float *light, float *soil) {
    uint32_t sum_light = 0, count_light = 0;
    uint32_t sum_soil = 0, count_soil = 0;

    for (int i = 0; i < length; i += SOC_ADC_DIGI_RESULT_BYTES) {
        adc_digi_output_data_t *p = (void*)&result[i];
        if (p->type1.channel == LIGHT_SENSOR_CHANNEL) {
            sum_light += p->type1.data;
            count_light++;
        } else if (p->type1.channel == SOIL_SENSOR_CHANNEL) {
            sum_soil += p->type1.data;
            count_soil++;
        }
    }

    *light = (count_light > 0) ? ((float)sum_light / count_light / 4095.0f * 100.0f) : 0;
    *soil  = (count_soil  > 0) ? (100.0f - ((float)sum_soil  / count_soil  / 4095.0f * 100.0f)) : 0;
}

void sensor_test(void *pvParameters) {
    float temperature, humidity, light_percentage, soil_percentage;
    uint8_t result[256] = {0};
    uint32_t o_length = 0;

    ESP_LOGI(TAG, "Starting sensors test...");
    ESP_ERROR_CHECK(adc_continuous_start(handle));

    while (1) {
        // Read DHT sensor
        if (dht_read_float_data(SENSOR_TYPE, DHT_GPIO_PIN, &humidity, &temperature) == ESP_OK) 
        {
        } 
        else 
        {
            ESP_LOGE(TAG, "Could not read DHT sensor");
        }

        // Read light and soil sensors
        if (adc_continuous_read(handle, result, 256, &o_length, 1000) == ESP_OK) {
            parse_adc_results(result, o_length, &light_percentage, &soil_percentage);
        } else {
            ESP_LOGE(TAG, "ADC read failed");
        }

        // Analyze data
        analyse_data(&temperature, &humidity, &light_percentage, &soil_percentage);

        vTaskDelay(pdMS_TO_TICKS(2000));
    }
}

void app_main(void)
{
    init_adc();

    xTaskCreate(sensor_test, "sensor_test", configMINIMAL_STACK_SIZE * 3, NULL, 5, NULL);
}