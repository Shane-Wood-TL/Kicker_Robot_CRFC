#include "../../include/setup/led_strip_setup.h"
void led_setup(){
    /// RMT backend specific configuration
    led_strip_rmt_config_t rmt_config = {
        .clk_src = RMT_CLK_SRC_DEFAULT,    // different clock source can lead to different power consumption
        .resolution_hz = 10 * 1000 * 1000, // RMT counter clock frequency: 10MHz
        .mem_block_symbols = 64,           // the memory size of each RMT channel, in words (4 bytes)
        .flags = {
            .with_dma = false, // DMA feature is available on chips like ESP32-S3/P4
        }
    };

    /// LED strip common configuration
    led_strip_config_t led_A_strip_config = {
        .strip_gpio_num = led_A_pin,  // The GPIO that connected to the LED strip's data line
        .max_leds = number_of_leds,                 // The number of LEDs in the strip,
        .led_model = LED_MODEL_WS2812, // LED strip model, it determines the bit timing
        .color_component_format = LED_STRIP_COLOR_COMPONENT_FMT_GRB, // The color component format is G-R-B
        .flags = {
            .invert_out = false, // don't invert the output signal
        }
    };

    /// Create the LED strip object
    led_strip_handle_t led_A_strip;
    ESP_ERROR_CHECK(led_strip_new_rmt_device(&led_A_strip_config, &rmt_config, &led_A_strip));

    /// LED strip common configuration
    led_strip_config_t led_B_strip_config = {
        .strip_gpio_num = led_B_pin,  // The GPIO that connected to the LED strip's data line
        .max_leds = number_of_leds,                 // The number of LEDs in the strip,
        .led_model = LED_MODEL_WS2812, // LED strip model, it determines the bit timing
        .color_component_format = LED_STRIP_COLOR_COMPONENT_FMT_GRB, // The color component format is G-R-B
        .flags = {
            .invert_out = false, // don't invert the output signal
        }
    };

    /// Create the LED strip object
    led_strip_handle_t led_B_strip;
    ESP_ERROR_CHECK(led_strip_new_rmt_device(&led_B_strip_config, &rmt_config, &led_B_strip));

    for(uint8_t i =0; i < number_of_leds; i++){
        led_strip_set_pixel(led_A_strip, i, led_color_blue);
        led_strip_set_pixel(led_B_strip, i, led_color_blue);
    }

    led_strip_refresh(led_A_strip);
    led_strip_refresh(led_B_strip);    
}