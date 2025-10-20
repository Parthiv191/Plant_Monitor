#include <stdio.h>
#include <string.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <dht.h>
#include <analysis.h>
#include <esp_adc/adc_continuous.h>
#include <esp_log.h>
#include <esp_timer.h>
#include <driver/gpio.h>
#include "display_oled.h"

#define SENSOR_TYPE DHT_TYPE_DHT11
#define DHT_GPIO_PIN GPIO_NUM_26

#define LIGHT_SENSOR_CHANNEL ADC_CHANNEL_0 // GPIO36
#define SOIL_SENSOR_CHANNEL  ADC_CHANNEL_3 // GPIO39
#define ADC_UNIT             ADC_UNIT_1
#define ADC_ATTEN            ADC_ATTEN_DB_12

#define LED_STATUS   GPIO_NUM_2
#define LED_ALERT    GPIO_NUM_4

static const char *TAG = "PlantMonitor";

// rolling light storage (every 30 min for 24h = 48 samples)
static float daily_light_data[48];
static int light_read_count = 0;

static adc_continuous_handle_t adc_handle = NULL;

static void init_adc(void)
{
    adc_continuous_handle_cfg_t cfg = {
        .max_store_buf_size = 1024,
        .conv_frame_size = 256,
    };
    ESP_ERROR_CHECK(adc_continuous_new_handle(&cfg, &adc_handle));

    adc_digi_pattern_config_t patterns[2] = {
        { .atten = ADC_ATTEN, .channel = LIGHT_SENSOR_CHANNEL, .unit = ADC_UNIT, .bit_width = SOC_ADC_DIGI_MAX_BITWIDTH },
        { .atten = ADC_ATTEN, .channel = SOIL_SENSOR_CHANNEL,  .unit = ADC_UNIT, .bit_width = SOC_ADC_DIGI_MAX_BITWIDTH }
    };

    adc_continuous_config_t dig_cfg = {
        .pattern_num = 2,
        .adc_pattern = patterns,
        .sample_freq_hz = 20000,
        .conv_mode = ADC_CONV_SINGLE_UNIT_1,
        .format = ADC_DIGI_OUTPUT_FORMAT_TYPE1,
    };
    ESP_ERROR_CHECK(adc_continuous_config(adc_handle, &dig_cfg));
    ESP_ERROR_CHECK(adc_continuous_start(adc_handle));
}

#include <driver/gpio.h>

static inline void leds_init(void)
{
    gpio_config_t io = {
        .mode = GPIO_MODE_OUTPUT,
        .pin_bit_mask = (1ULL<<LED_STATUS) | (1ULL<<LED_ALERT),
        .pull_down_en = 0,
        .pull_up_en = 0,
        .intr_type = GPIO_INTR_DISABLE
    };
    gpio_config(&io);
    gpio_set_level(LED_STATUS, 0);
    gpio_set_level(LED_ALERT, 0);
}

static inline void led_status_blink(void)
{
    gpio_set_level(LED_STATUS, 1);
    vTaskDelay(pdMS_TO_TICKS(80));
    gpio_set_level(LED_STATUS, 0);
}

static inline void led_alert_set(bool on)
{
    gpio_set_level(LED_ALERT, on ? 1 : 0);
}

static void parse_adc_results(uint8_t *result, uint32_t length, float *light, float *soil)
{
    uint32_t sum_light = 0, cnt_light = 0;
    uint32_t sum_soil  = 0, cnt_soil  = 0;

    for (int i = 0; i < length; i += SOC_ADC_DIGI_RESULT_BYTES) {
        adc_digi_output_data_t *p = (void*)&result[i];
        if (p->type1.channel == LIGHT_SENSOR_CHANNEL) { sum_light += p->type1.data; cnt_light++; }
        else if (p->type1.channel == SOIL_SENSOR_CHANNEL) { sum_soil += p->type1.data; cnt_soil++; }
    }

    *light = (cnt_light > 0) ? ((float)sum_light / cnt_light / 4095.0f * 100.0f) : 0;
    *soil  = (cnt_soil  > 0) ? (100.0f - ((float)sum_soil / cnt_soil / 4095.0f * 100.0f)) : 0;
}

/* externs from analysis.c so we can show statuses on OLED */
extern char temp_status[6];
extern char humidity_status[6];
extern char light_status[6];
extern char soil_status[6];

static void sensor_task(void *arg)
{
    uint8_t frame[256];
    uint32_t out_len = 0;

    while (1) {
        float temp_c = 0.0f, humidity = 0.0f;
        float light_pct = 0.0f, soil_pct = 0.0f;

        // DHT
        if (dht_read_float_data(SENSOR_TYPE, DHT_GPIO_PIN, &humidity, &temp_c) != ESP_OK) {
            ESP_LOGE(TAG, "DHT read failed");
        }

        // ADC batch
        if (adc_continuous_read(adc_handle, frame, sizeof(frame), &out_len, 1000) == ESP_OK) {
            parse_adc_results(frame, out_len, &light_pct, &soil_pct);
        } else {
            ESP_LOGE(TAG, "ADC read failed");
        }

        // store light sample
        if (light_read_count < 48) daily_light_data[light_read_count++] = light_pct;

        float avg_light = light_pct;
        if (light_read_count == 48) {
            float sum = 0;
            for (int i = 0; i < 48; ++i) sum += daily_light_data[i];
            avg_light = sum / 48.0f;
            light_read_count = 0;
        }

        // analysis -> fills *_status strings and logs
        analyse_data(&temp_c, &humidity, &avg_light, &soil_pct);

        // OLED
        oled_show_readings(temp_c, humidity, avg_light, soil_pct,
                           temp_status, humidity_status, light_status, soil_status);

        // LEDs
        led_status_blink();
        bool alert = (strcmp(temp_status,"Good\n")!=0) ||
                     (strcmp(humidity_status,"Good\n")!=0) ||
                     (strcmp(light_status,"Good\n")!=0) ||
                     (strcmp(soil_status,"Good\n")!=0);
        led_alert_set(alert);

        // interval (30 minutes). For bring-up/testing, use 5 seconds, then change to 30 min.
        // vTaskDelay(pdMS_TO_TICKS(30 * 60 * 1000));
        vTaskDelay(pdMS_TO_TICKS(5000));
    }
}

void app_main(void)
{
    init_adc();
    oled_init();
    leds_init();

    xTaskCreatePinnedToCore(sensor_task, "sensor_task", 4096, NULL, 5, NULL, tskNO_AFFINITY);
}
