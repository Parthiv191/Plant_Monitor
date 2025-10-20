idf_component_register(
    SRCS "display_oled.c" "ssd1306_min.c"
    INCLUDE_DIRS "."
    REQUIRES driver
)