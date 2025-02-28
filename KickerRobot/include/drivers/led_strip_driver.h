#ifndef __led_strip_driver__
#define __led_strip_driver__
#include "../all_includes.h"
class led_strip_driver{
private:
    /// RMT backend specific configuration
    led_strip_rmt_config_t rmt_config = {
        .clk_src = RMT_CLK_SRC_DEFAULT,    // different clock source can lead to different power consumption
        .resolution_hz = 10 * 1000 * 1000, // RMT counter clock frequency: 10MHz
        .mem_block_symbols = 64,           // the memory size of each RMT channel, in words (4 bytes)
        .flags = {
            .with_dma = false, // DMA feature is available on chips like ESP32-S3/P4
        }
    };
    led_strip_config_t led_strip_config;
    led_strip_handle_t led_strip;

    public:
    led_strip_driver(uint8_t strip_gpio_pin, uint8_t number_of_leds_in_strip);
    void set_color(uint8_t r, uint8_t g, uint8_t b, uint8_t led_start, uint8_t led_end);
};
#endif