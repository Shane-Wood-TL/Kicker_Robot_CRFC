/**
 * @file menus.h
 * @brief This is the file that contains the class for menus
 * @author Shane Wood
 * @date 9/2/2025
*/
#ifndef __menus__
#define __menus__

#include "../drivers/ssd1306.h"
#include "changeable_values.h"
#include <string>

/**
 * @class menu
 * @brief This class is for creating menus
 * @details This class is for creating menus that can be displayed on the ssd1306 display, multiple lines are supported, and multiple data types
 * if int or float with 2 decimal places are not what is needed, then consider using the string version of this class
 * @tparam T The type of the values that can be modified in the menu
 */
template <typename T>
class menu{
    uint8_t lines; ///< The amount of lines for this menu (max 4 for 128x64 display, or 2 for 128x32 display)
    std::string words[SSD1306Pages/2]; ///< The (constant) words that are displayed on the menu
    bool have_values[SSD1306Pages/2]; ///< If the words have an associated value to be displayed
    T **values[SSD1306Pages/2]; ///< The values that are displayed on the menu
    ssd1306 *display;  ///< The display that the menu is displayed on
    int8_t currently_selected; ///< The currently selected value (shown as an X on the display) 
    const uint8_t font_rows[4] = {0,15,31,47}; ///< The rows that the text is displayed on 0-63 split evenly 
    bool has_selectable_values = false; ///< If the menu has selectable values
    bool selectable_values[SSD1306Pages/2]; ///< If the values can be modified
    
    public:
        /**
         * @brief This is the constructor for the menu class
         * @param lines The amount of lines for the menu
         * @param have_values If the lines have values
         * @param words The words that are displayed on the menu
         * @param values The values that are displayed on the menu
         * @param display The display that the menu is displayed on
         * @param selectable_values If the values can be selected
         */
        menu(uint8_t lines, bool *have_values, std::string *words, T **values, ssd1306 *display, bool *selectable_values);

        /**
         * @brief This function changes what value is selected in the menu (if modifiable)
         * @param direction The direction to move the selection
         */
        void change_selected(int8_t direction);

        /**
         * @brief This function draws the menu to the display
         */
        void draw_to_display();

        /**
         * @brief This function modifies the selected value in the menu (if modifiable)
         */
        void increase_selected_value();

        void decrease_selected_value();
};
#endif