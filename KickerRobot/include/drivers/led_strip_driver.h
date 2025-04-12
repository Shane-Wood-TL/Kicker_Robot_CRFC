/**
 * @file led_strip_drive.h
 * @brief This is the header for the led strip class
 * @author Shane Wood
 * @date 14/12/2024
*/

#ifndef __led_strip_driver__
#define __led_strip_driver__
#include "../all_includes.h"

#define rmt_counter_clock_frequency (10000000)  ///< 10MHz for the rmt counter clock
#define rmt_channel_memory_size  (64)  ///< the memory size of each RMT channel, in words (4 bytes)

/**
 * @class led_strip_driver
 * @brief This is a class to control ws2812b led strips
 */
class led_strip_driver {
   private:
    led_strip_rmt_config_t rmt_config = {
        .clk_src = RMT_CLK_SRC_DEFAULT,  ///< different clock source can lead to different power consumption
        .resolution_hz = rmt_counter_clock_frequency,  ///< RMT counter clock frequency: 10MHz
        .mem_block_symbols = rmt_channel_memory_size,  ///< the memory size of each RMT channel, in words (4 bytes)
        .flags = {
            .with_dma = false,  ///< DMA feature is available on chips like ESP32-S3/P4
        }};  ///< rmt setup (Remote Control Transceiver)

    led_strip_config_t led_strip_config;  ///< the config for the led strip
    led_strip_handle_t led_strip;         ///< the handle for the led strip

   public:
    /**
     * @brief Constructor for the led_strip_driver class
     *
     * @param strip_gpio_pin : uint8_t, the GPIO pin where the led strip is connected
     * @param number_of_leds_in_strip : uint8_t the number of leds in the strip, max addressable is n-1, 16 leds can address 0-15
     */
    led_strip_driver(uint8_t strip_gpio_pin, uint8_t number_of_leds_in_strip);

    /**
     * @brief set the color for a set of leds in the strip
     *
     * @param r : uint8_t set the red value for the range of leds
     * @param g : uint8_t set the green value for the range of leds
     * @param b : uint8_t set the blue value for the range of leds
     * @param led_start : uint8_t the index of the led to start this color change (starts at 0)
     * @param led_end : uint8_t the index of the led to stop this color change (max value of number_of_leds_in_strip-1)
     */
    void set_color(uint8_t r, uint8_t g, uint8_t b, uint8_t led_start,
                   uint8_t led_end);
};


/*
```plantuml
@startuml
skinparam classAttributeIconSize 0
hide circle

class led_strip_driver {
    - rmt_config : led_strip_rmt_config_t
    - led_strip_config : led_strip_config_t
    - led_strip : led_strip_handle_t
    + led_strip_driver(strip_gpio_pin : uint8_t, number_of_leds_in_strip : uint8_t)
    + set_color(r : uint8_t, g : uint8_t, b : uint8_t, led_start : uint8_t, led_end : uint8_t) : void
}

@enduml
```
*/
#endif