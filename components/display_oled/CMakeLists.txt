idf_component_register(
    SRCS "display_oled.c"
    INCLUDE_DIRS "."
    REQUIRES i2cdev ssd1306
)
