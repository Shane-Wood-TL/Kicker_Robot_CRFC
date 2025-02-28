#include "../../include/drivers/led_strip_driver.h"
led_strip_driver::led_strip_driver(uint8_t strip_gpio_pin, uint8_t number_of_leds_in_strip){
    led_strip_config.strip_gpio_num = strip_gpio_pin;
    led_strip_config.max_leds=number_of_leds_in_strip;
    led_strip_config.led_model = LED_MODEL_WS2812;
    led_strip_config.color_component_format = LED_STRIP_COLOR_COMPONENT_FMT_GRB;
    led_strip_config.flags = {
        .invert_out = false, // don't invert the output signal
    };

    (void)led_strip_new_rmt_device(&led_strip_config, &rmt_config, &led_strip);
    set_color(0,0,0,0,number_of_leds_in_strip-1);
}

void led_strip_driver::set_color(uint8_t r, uint8_t g, uint8_t b, uint8_t led_start, uint8_t led_end){
    for(uint8_t led_to_change = led_start; led_to_change <= led_end; led_to_change++){
        (void)led_strip_set_pixel(led_strip, led_to_change, r,g,b);
    }
    (void)led_strip_refresh(led_strip);
}