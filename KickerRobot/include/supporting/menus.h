#ifndef __menus__
#define __menus__

#include "../drivers/ssd1306.h"
#include <string>


template <typename T>
class menu{
    uint8_t lines;
    std::string words[SSD1306Pages/2];
    bool have_values[SSD1306Pages/2];
    T **values[SSD1306Pages/2];
    ssd1306 *display;
    int8_t currently_selected;
    const uint8_t font_rows[4] = {0,15,31,47};
    bool has_selectable_values = false;
    bool selectable_values[SSD1306Pages/2];
    
    public:
        menu(uint8_t lines, bool *have_values, std::string *words, T **values, ssd1306 *display, bool *selectable_values);
        void change_selected(int8_t direction);
        void draw_to_display();
        void modify_selected_value(T amount);
};
#endif