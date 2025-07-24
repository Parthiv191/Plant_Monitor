idf_component_register(
    SRCS "main.c"
    INCLUDE_DIRS "."
    REQUIRES dht
    REQUIRES esp_adc
    REQUIRES analysis
)