#include "../../include/supporting/menu_handler.h"

enum rolling_menu_states{STATUS_SCREEN_STATE, MOTOR_SPEEDS_STATE, RAMPED_VELOCITY_STATE};



menu_handler::menu_handler(ssd1306 *display, menu<std::string> *status_screen, menu<changeable_values<uint8_t>> *motor_speed_menu, menu<changeable_values<float>> *ramped_velocity_menu, 
    menu<uint8_t> *servo_latched_menu, menu<uint8_t> *servo_released_menu, menu<uint8_t> *motors_calibrating_menu,
    menu<uint8_t> *motors_enable_menu, menu<uint8_t> *motors_disable_menu, menu<uint8_t> *errors_clear_menu) : option_state(false), r2_state(false), 
    l2_state(false), square_state(false), triangle_state(false), circle_state(false), dpad_up_state(false), dpad_down_state(false), dpad_left_state(false),
    dpad_right_state(false){
        this->display = display;
        this->status_screen = status_screen;
        this->motor_speed_menu = motor_speed_menu;
        this->ramped_velocity_menu = ramped_velocity_menu;
        this->servo_latched_menu = servo_latched_menu;
        this->servo_released_menu = servo_released_menu;
        this->motors_calibrating_menu = motors_calibrating_menu;
        this->motors_enable_menu = motors_enable_menu;
        this->motors_disable_menu = motors_disable_menu;
        this->errors_clear_menu = errors_clear_menu;
}

void menu_handler::draw_status_display(){
    current_state = 0;
    display->clear_buffer();
    status_screen->draw_to_display();
    display->write_buffer_SSD1306();
}


void menu_handler::triangle_pressed(){
    static uint8_t motors_enabled = DISABLED;
    if (xSemaphoreTake(motor_status_mutex, portMAX_DELAY)){
        if(motors_enabled==ENABLED){
            motor_status = DISABLED;
            xSemaphoreGive(motor_status_mutex);
            motors_enabled = DISABLED;
            display->clear_buffer();
            motors_disable_menu->draw_to_display();
        }else{
            motor_status = ENABLED;
            xSemaphoreGive(motor_status_mutex);
            motors_enabled = ENABLED;
            display->clear_buffer();
            motors_enable_menu->draw_to_display();
        }
        display->write_buffer_SSD1306();
        vTaskDelay(pdMS_TO_TICKS(motor_status_changed_display_time));
        if (xSemaphoreTake(motor_status_mutex, portMAX_DELAY)){
            motor_status = IDLE;
            xSemaphoreGive(motor_status_mutex);
        }
        draw_status_display();
    }
}


void menu_handler::circle_pressed(){
    if (xSemaphoreTake(motor_status_mutex, portMAX_DELAY)){
        motor_status = CALIBRATING;
        xSemaphoreGive(motor_status_mutex);
        display->clear_buffer();
        motors_calibrating_menu->draw_to_display();
        display->write_buffer_SSD1306();
        vTaskDelay(pdMS_TO_TICKS(motor_calibrating_display_time));
        if (xSemaphoreTake(motor_status_mutex, portMAX_DELAY)){
            motor_status = DISABLED;
            xSemaphoreGive(motor_status_mutex);
        }
        draw_status_display();
    }
}


void menu_handler::square_pressed(){
    if (xSemaphoreTake(motor_status_mutex, portMAX_DELAY)){
        motor_status = ERRORLESS;
        xSemaphoreGive(motor_status_mutex);
        display->clear_buffer();
        errors_clear_menu->draw_to_display();
        display->write_buffer_SSD1306();
        vTaskDelay(pdMS_TO_TICKS(motor_status_changed_display_time));
        if (xSemaphoreTake(motor_status_mutex, portMAX_DELAY)){
            motor_status = IDLE;
            xSemaphoreGive(motor_status_mutex);
        }
        draw_status_display();
    }
}


void menu_handler::r2_pressed(){
    if(xSemaphoreTake(servo_status_mutex, portMAX_DELAY)){
        servo_status = LATCHED;
        xSemaphoreGive(servo_status_mutex);

        display->clear_buffer();
        servo_released_menu->draw_to_display();
        display->write_buffer_SSD1306();

        vTaskDelay(pdMS_TO_TICKS(servo_status_changed_display_time));
        draw_status_display();
    }

}


void menu_handler::l2_pressed(){
    if(xSemaphoreTake(servo_status_mutex, portMAX_DELAY)){
        servo_status = RELEASED;
        xSemaphoreGive(servo_status_mutex);
        display->clear_buffer();
        servo_latched_menu->draw_to_display();
        display->write_buffer_SSD1306();

        vTaskDelay(pdMS_TO_TICKS(servo_status_changed_display_time));

        draw_status_display();
    }
}


void menu_handler::options_pressed(){
    current_state++;
    if(current_state > cyclic_menu_count){
        current_state = STATUS_SCREEN_STATE;
    }
    switch (current_state)
    {
    case(STATUS_SCREEN_STATE):{
        display->clear_buffer();
        if(xSemaphoreTake(main_menu_values_mutex, portMAX_DELAY)){
            status_screen->draw_to_display();
            xSemaphoreGive(main_menu_values_mutex);
        }
        display->write_buffer_SSD1306();
        break;
    }
    case(MOTOR_SPEEDS_STATE):{
        display->clear_buffer();
        if(xSemaphoreTake(motor_speeds_mutex, portMAX_DELAY)){
            motor_speed_menu->draw_to_display();
            xSemaphoreGive(motor_speeds_mutex);
        }
        display->write_buffer_SSD1306();
        break;
    }
    case(RAMPED_VELOCITY_STATE):{
        display->clear_buffer();
        if(xSemaphoreTake(ramped_velocity_mutex, portMAX_DELAY)){
            ramped_velocity_menu->draw_to_display();
            xSemaphoreGive(ramped_velocity_mutex);
        }
        display->write_buffer_SSD1306();
        break;
    }
    default:{
        current_state = STATUS_SCREEN_STATE;
        break;
    }
}
}


void menu_handler::dpad_up_pressed(){
    if(current_state == MOTOR_SPEEDS_STATE){
        display->clear_buffer();
        motor_speed_menu->change_selected(1);
        motor_speed_menu->draw_to_display();
        display->write_buffer_SSD1306();
    }else if(current_state == RAMPED_VELOCITY_STATE){
        display->clear_buffer();
        ramped_velocity_menu->change_selected(1);
        ramped_velocity_menu->draw_to_display();
        display->write_buffer_SSD1306();
    }
}


void menu_handler::dpad_down_pressed(){
    if(current_state == MOTOR_SPEEDS_STATE){
        display->clear_buffer();
        motor_speed_menu->change_selected(-1);
        motor_speed_menu->draw_to_display();
        display->write_buffer_SSD1306();
    }else if(current_state == RAMPED_VELOCITY_STATE){
        display->clear_buffer();
        ramped_velocity_menu->change_selected(-1);
        ramped_velocity_menu->draw_to_display();
        display->write_buffer_SSD1306();
    }
}


void menu_handler::dpad_left_pressed(){
    if(current_state == MOTOR_SPEEDS_STATE){
        display->clear_buffer();
        if(xSemaphoreTake(motor_speeds_mutex, portMAX_DELAY)){
            motor_speed_menu->decrease_selected_value();
            motor_speed_menu->draw_to_display();
            xSemaphoreGive(motor_speeds_mutex);
        }
        display->write_buffer_SSD1306();
    }else if(current_state == RAMPED_VELOCITY_STATE){
        display->clear_buffer();
        if(xSemaphoreTake(ramped_velocity_mutex, portMAX_DELAY)){
            ramped_velocity_menu->decrease_selected_value();
            ramped_velocity_menu->draw_to_display();
            xSemaphoreGive(ramped_velocity_mutex);
        }
        display->write_buffer_SSD1306();
    }
}


void menu_handler::dpad_right_pressed(){
    if(current_state == MOTOR_SPEEDS_STATE){
        display->clear_buffer();
        if(xSemaphoreTake(motor_speeds_mutex, portMAX_DELAY)){
            motor_speed_menu->increase_selected_value();
            motor_speed_menu->draw_to_display();
            xSemaphoreGive(motor_speeds_mutex);
        }
        display->write_buffer_SSD1306();
    }else if(current_state == RAMPED_VELOCITY_STATE){
        display->clear_buffer();
        if(xSemaphoreTake(ramped_velocity_mutex, portMAX_DELAY)){
            ramped_velocity_menu->increase_selected_value();
            ramped_velocity_menu->draw_to_display();
            xSemaphoreGive(ramped_velocity_mutex);
        }
        display->write_buffer_SSD1306();
    }
}



void menu_handler::update(){
    if(xSemaphoreTake(update_main_display_mutex, portMAX_DELAY)){
        if(update_main_display==true && current_state ==STATUS_SCREEN_STATE){
            update_main_display = false;
            xSemaphoreGive(update_main_display_mutex);
            if(xSemaphoreTake(main_menu_values_mutex, portMAX_DELAY)){
                draw_status_display();
                xSemaphoreGive(main_menu_values_mutex);
            }
            
        }else{
            xSemaphoreGive(update_main_display_mutex);
        }
    }
    if((xQueueReceive(other_controller_data_queue,&current_data,portMAX_DELAY) == pdTRUE)){
        xQueueReset(other_controller_data_queue); //prevents multiple inputs
        bool current_options_state = current_data.options_triggers & (1 <<processed_options_bit);

        bool current_r2_state = current_data.options_triggers & (1 <<processed_r2_bit);
        bool current_l2_state = current_data.options_triggers & (1 <<processed_l2_bit);

        bool current_square_state = (current_data.buttons & (1 << processed_square_bit));
        bool current_triangle_state = (current_data.buttons & (1 << processed_triangle_bit));
        bool current_circle_state = (current_data.buttons & (1 << processed_circle_bit));

        bool current_dpad_up_state = current_data.buttons & (1 << processed_dpad_up_bit);
        bool current_dpad_down_state = current_data.buttons & (1 << processed_dpad_down_bit);
        bool current_dpad_left_state = current_data.buttons & (1 << processed_dpad_left_bit) ;
        bool current_dpad_right_state = current_data.buttons & (1 << processed_dpad_right_bit);


        if(triangle_state.update(current_triangle_state)){
            triangle_pressed();
        }else if(square_state.update(current_square_state)){
            square_pressed();
        }else if(circle_state.update(current_circle_state)){
            circle_pressed();
        }else if(r2_state.update(current_r2_state)){
            r2_pressed();
        }else if(l2_state.update(current_l2_state)){
            l2_pressed();
        }else if(option_state.update(current_options_state)){
            options_pressed();
        }else if(dpad_up_state.update(current_dpad_up_state)){
            dpad_up_pressed();
        }else if(dpad_down_state.update(current_dpad_down_state)){
            dpad_down_pressed();
        }else if(dpad_right_state.update(current_dpad_right_state)){
            dpad_right_pressed();
        }else if(dpad_left_state.update(current_dpad_left_state)){
            dpad_left_pressed();
        }
    }
}