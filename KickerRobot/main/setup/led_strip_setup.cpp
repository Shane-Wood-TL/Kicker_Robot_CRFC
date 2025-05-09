/**
 * @file led_strip_setup.cpp
 * @brief source file that sets up the led strips
 * @author Shane Wood
 * @date 12/02/2025
 */

#include "../../include/setup/led_strip_setup.h"
#include "../../include/drivers/led_strip_driver.h"



void led_setup(){
    led_strip_driver a_led_strip(led_A_pin,number_of_leds);
    led_strip_driver b_led_strip(led_B_pin,number_of_leds);

    a_led_strip.set_color(led_color_blue, 0,(number_of_leds-1));
    b_led_strip.set_color(led_color_blue, 0,(number_of_leds-1));
}