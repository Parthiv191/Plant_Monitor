idf_component_register(
    SRCS "main.c" "plant_data.c" "wifi_sta.c" "http_server.c"
    INCLUDE_DIRS "."
    REQUIRES analysis dht display_oled driver esp_adc esp_event esp_http_server esp_netif esp_wifi espressif__mdns nvs_flash
)
