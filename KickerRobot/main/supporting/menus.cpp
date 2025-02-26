
#include "../../include/supporting/menus.h"

template <typename T>
menu<T>::menu(uint8_t lines, bool *have_values, std::string *words, T **values, ssd1306 *display, bool *selectable_values){
    bool first_run = true;
    this->lines = lines;
    currently_selected = 5;
    for (uint8_t i = 0; i < lines; ++i) {
        this->have_values[i] = have_values[i];
        this->words[i] = words[i];
        this->values[i] = &values[i];
        this->selectable_values[i] = selectable_values[i];
        if(have_values[i]){
            if(first_run){
                currently_selected = i;
                first_run = false;
            } 
        }
        if(selectable_values[i]){
            has_selectable_values  = true;
        }
    }
    this->display = display;
}

template <>
void menu<float>::draw_to_display(){
    //find the center of each word
    for(uint8_t i=0; i < lines; i++){
        uint8_t x_positions = 0;
        if(have_values[i]==true){
            uint8_t current_length=0;
            std::string value_as_string = "";
            char temp_buffer[5];
            sprintf(temp_buffer,"%.2f",**values[i]);
            value_as_string = temp_buffer;
            current_length = words[i].length() + value_as_string.length()+1;
            current_length *=12;
            x_positions = (SSD1306HorizontalRes-current_length)/2;
            display->write_string_SSD1306(words[i],x_positions,font_rows[i]);
            display->write_string_SSD1306(value_as_string,(x_positions+(words[i].length()*12)+12),font_rows[i]);
        }else{
            uint8_t current_length=0;
            current_length = words[i].length();
            current_length *=12;
            x_positions = (SSD1306HorizontalRes-current_length)/2; 
            display->write_string_SSD1306(words[i],x_positions,font_rows[i]);
        }
    }
    change_selected(0);
    
}

template <>
void menu<uint8_t>::draw_to_display(){
    //find the center of each word
    for(uint8_t i=0; i < lines; i++){
        uint8_t x_positions = 0;
        if(have_values[i]==true){
            uint8_t current_length=0;
            std::string value_as_string = "";
            value_as_string = std::to_string(**values[i]);                
            current_length = words[i].length() + value_as_string.length()+1;
            current_length *=12;
            x_positions = (SSD1306HorizontalRes-current_length)/2;
            display->write_string_SSD1306(words[i],x_positions,font_rows[i]);
            display->write_string_SSD1306(value_as_string,(x_positions+(words[i].length()*12)+12),font_rows[i]);
        }else{
            uint8_t current_length=0;
            current_length = words[i].length();
            current_length *=12;
            x_positions = (SSD1306HorizontalRes-current_length)/2; 
            display->write_string_SSD1306(words[i],x_positions,font_rows[i]);
        }
    }
    change_selected(0);
    
    
}

template <>
void menu<std::string>::draw_to_display(){
    //find the center of each word
    for(uint8_t i=0; i < lines; i++){
        uint8_t x_positions = 0;
        if(have_values[i]==true){
            uint8_t current_length=0;
            std::string value_as_string = **values[i];                
            current_length = words[i].length() + value_as_string.length()+1;
            current_length *=12;
            x_positions = (SSD1306HorizontalRes-current_length)/2;
            display->write_string_SSD1306(words[i],x_positions,font_rows[i]);
            display->write_string_SSD1306(value_as_string,(x_positions+(words[i].length()*12)+12),font_rows[i]);
        }else{
            uint8_t current_length=0;
            current_length = words[i].length();
            current_length *=12;
            x_positions = (SSD1306HorizontalRes-current_length)/2; 
            display->write_string_SSD1306(words[i],x_positions,font_rows[i]);
        }
    }
    change_selected(0);
    
}

template <typename T>
void menu<T>::change_selected(int8_t direction){
    if(!has_selectable_values){
        return;
    }
    currently_selected -= direction;
    if(currently_selected >= lines){
        currently_selected = 0;
    }else if(currently_selected < 0){
        currently_selected = lines-1;
    }
    while(selectable_values[currently_selected] == false){
        currently_selected -= direction;
        if(currently_selected >= lines){
            currently_selected = 0;
        }else if(currently_selected < 0){
            currently_selected = lines-1;
        }
    }
    //draw an X denoting currently selected
    display->write_string_SSD1306("X",0,font_rows[currently_selected]);
}

template <>
void menu<uint8_t>::modify_selected_value(uint8_t amount){
    if(selectable_values[currently_selected]){
        **(values[currently_selected]) += amount;
    }
}

template <>
void menu<float>::modify_selected_value(float amount){
    if(selectable_values[currently_selected]){
        **(values[currently_selected]) += amount;
    }
}


template class menu<unsigned char>;
template class menu<float>;
template class menu<std::string>;