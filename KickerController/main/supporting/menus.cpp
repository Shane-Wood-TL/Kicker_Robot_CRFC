/**
 * @file menus.cpp
 * @brief source file for the class that handles the creation of menus
 * @author Shane Wood
 * @date 10/02/2025
 */
#include "../../include/supporting/menus.h"



template<typename T>
menu<T>::menu(uint8_t lines, bool *have_values, std::string *words, T **values, ssd1306 *display, bool *selectable_values) {
  if (lines > max_line_count) {
    printf("Too many lines for menu\n");
    esp_restart();
  }
  if (have_values == nullptr) {
    printf("have_values is null\n");
    esp_restart();
  }
  if (words == nullptr) {
    printf("words is null\n");
    esp_restart();
  }
  if (display == nullptr) {
    printf("display is null\n");
    esp_restart();
  }
  if (selectable_values == nullptr) {
    printf("selectable_values is null\n");
    esp_restart();
  }
  bool first_run = true;
  this->lines = lines;
  currently_selected = max_line_count + 1;
  for (uint8_t i = 0; i < lines; ++i) {
    this->have_values[i] = have_values[i];
    this->words[i] = words[i];
    this->values[i] = &values[i];
    this->selectable_values[i] = selectable_values[i];
    if (have_values[i]) {
      if (first_run) {
        currently_selected = i;
        first_run = false;
      }
    }
    if (selectable_values[i]) {
      has_selectable_values = true;
    }
  }
  this->display = display;
}



template<>
void menu<changeable_values<float>>::draw_to_display() {
  //find the center of each word
  for (uint8_t i = 0; i < lines; i++) {
    uint8_t x_positions = 0;
    if (have_values[i] == true) {
      uint8_t current_length = 0;
      std::string value_as_string = "";
      char temp_buffer[float_display_length];
      sprintf(temp_buffer, two_decimal_places, (*values[i])->get_value());
      value_as_string = temp_buffer;
      current_length = words[i].length() + value_as_string.length() + 1;
      current_length *= font_width;
      x_positions = (SSD1306_horizontal_resolution - current_length) divide_by_two;
      display->write_string_SSD1306(words[i], x_positions, font_rows[i]);
      display->write_string_SSD1306(value_as_string, x_positions + (words[i].length() * font_width) + font_width, font_rows[i]);
    } else {
      uint8_t current_length = 0;
      current_length = words[i].length();
      current_length *= font_width;
      x_positions = (SSD1306_horizontal_resolution - current_length) divide_by_two;
      display->write_string_SSD1306(words[i], x_positions, font_rows[i]);
    }
  }
  change_selected(0);
}



template<>
void menu<changeable_values<uint8_t>>::draw_to_display() {
  //find the center of each word
  for (uint8_t i = 0; i < lines; i++) {
    uint8_t x_positions = 0;
    if (have_values[i] == true) {
      uint8_t current_length = 0;
      std::string value_as_string = "";
      value_as_string = std::to_string((*values[i])->get_value());
      current_length = words[i].length() + value_as_string.length() + 1;
      current_length *= font_width;
      x_positions = (SSD1306_horizontal_resolution - current_length) divide_by_two;
      display->write_string_SSD1306(words[i], x_positions, font_rows[i]);
      display->write_string_SSD1306(value_as_string, x_positions + (words[i].length() * font_width) + font_width, font_rows[i]);
    } else {
      uint8_t current_length = 0;
      current_length = words[i].length();
      current_length *= font_width;
      x_positions = (SSD1306_horizontal_resolution - current_length) divide_by_two;
      display->write_string_SSD1306(words[i], x_positions, font_rows[i]);
    }
  }
  change_selected(0);
}



template<>
void menu<uint8_t>::draw_to_display() {
  //find the center of each word
  for (uint8_t i = 0; i < lines; i++) {
    uint8_t x_positions = 0;
    if (have_values[i] == true) {
      uint8_t current_length = 0;
      std::string value_as_string = "";
      value_as_string = std::to_string(**values[i]);
      current_length = words[i].length() + value_as_string.length() + 1;
      current_length *= font_width;
      x_positions = (SSD1306_horizontal_resolution - current_length) divide_by_two;
      display->write_string_SSD1306(words[i], x_positions, font_rows[i]);
      display->write_string_SSD1306(value_as_string, x_positions + (words[i].length() * font_width) + font_width, font_rows[i]);
    } else {
      uint8_t current_length = 0;
      current_length = words[i].length();
      current_length *= font_width;
      x_positions = (SSD1306_horizontal_resolution - current_length) divide_by_two;
      display->write_string_SSD1306(words[i], x_positions, font_rows[i]);
    }
  }
  change_selected(0);
}



template<>
void menu<std::string>::draw_to_display() {
  //find the center of each word
  for (uint8_t i = 0; i < lines; i++) {
    uint8_t x_positions = 0;
    if (have_values[i] == true) {
      uint8_t current_length = 0;
      std::string value_as_string = **values[i];
      current_length = words[i].length() + value_as_string.length() + 1;
      current_length *= font_width;
      x_positions = (SSD1306_horizontal_resolution - current_length) divide_by_two;
      display->write_string_SSD1306(words[i], x_positions, font_rows[i]);
      display->write_string_SSD1306(value_as_string, x_positions + (words[i].length() * font_width) + font_width, font_rows[i]);
    } else {
      uint8_t current_length = 0;
      current_length = words[i].length();
      current_length *= font_width;
      x_positions = (SSD1306_horizontal_resolution - current_length) divide_by_two;
      display->write_string_SSD1306(words[i], x_positions, font_rows[i]);
    }
  }
  change_selected(0);
}



template<typename T>
void menu<T>::change_selected(int8_t direction) {
  if (direction != 1 && direction != -1 && direction != 0) {
    return;
  }

  if (!has_selectable_values) {
    return;
  }
  currently_selected -= direction;
  if (currently_selected >= lines) {
    currently_selected = 0;
  } else if (currently_selected < 0) {
    currently_selected = lines - 1;
  }
  while (selectable_values[currently_selected] == false) {
    currently_selected -= direction;
    if (currently_selected >= lines) {
      currently_selected = 0;
    } else if (currently_selected < 0) {
      currently_selected = lines - 1;
    }
  }
  //draw an X denoting currently selected
  display->write_string_SSD1306("X", 0, font_rows[currently_selected]);
}



template<>
void menu<changeable_values<float>>::increase_selected_value() {
  if (selectable_values[currently_selected]) {
    (*values[currently_selected])->increase_value();
  }
}



template<>
void menu<changeable_values<float>>::decrease_selected_value() {
  (*values[currently_selected])->decrease_value();
}



template<>
void menu<changeable_values<uint8_t>>::increase_selected_value() {
  if (selectable_values[currently_selected]) {
    (*values[currently_selected])->increase_value();
  }
}



template<>
void menu<changeable_values<uint8_t>>::decrease_selected_value() {
  if (selectable_values[currently_selected]) {
    (*values[currently_selected])->decrease_value();
  }
}

template class menu<float>;
template class menu<uint8_t>;
template class menu<std::string>;
template class menu<changeable_values<float>>;
template class menu<changeable_values<uint8_t>>;
