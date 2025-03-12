/**
 * @file menu_handler.h
 * @brief This is the file that contains the class for switching between menus
 * @author Shane Wood
 * @date 9/2/2025
*/

#ifndef __menu_handler__
#define __menu_handler__
#include "edge_dector.h"
#include "menus.h"
#include "../all_includes.h"

#define motor_status_changed_display_time 500 ///< how long to show that motor the status has changed for
#define servo_status_changed_display_time 250 ///< how long to show that servo the status has changed for
#define motor_calibrating_display_time 14000 ///< how long to show that the motors are calibrating for

#define cyclic_menu_count 2 ///< number of cyclic menus (main menu, motor speeds, ramped velocity)

extern QueueHandle_t other_controller_data_queue;

extern SemaphoreHandle_t ramped_velocity_mutex;
extern float velocity_ramp_limit;
extern float velocity_gain;  
extern float velocity_integrator_gain;  

extern SemaphoreHandle_t main_menu_values_mutex;
extern std::string contoller_connected; 
extern std::string robot_connected;
extern std::string battery_voltage_string;
extern std::string status;

extern SemaphoreHandle_t motor_status_mutex;
extern uint8_t motor_status;


extern SemaphoreHandle_t motor_speeds_mutex;
extern uint8_t left_motor_speed; //values set in settings
extern uint8_t right_motor_speed;

extern SemaphoreHandle_t servo_status_mutex;
extern uint8_t servo_status;

extern SemaphoreHandle_t update_main_display_mutex;
extern bool update_main_display;

/**
 * @class menu_handler
 * @brief This class is for switching between menus
 * @details This class will is specific to the current hardware setup of the controller, as
 * button presses are used to switch between specific menus (such as the right trigger releasing the servo)
 * and modifications will need to be done to utlize this class in other setups.
 */
class menu_handler{
    private:
        uint8_t current_state; ///< The current menu that is being displayed

        display_controller_data current_data={0,false}; ///< The data for the status display

        edge_dector option_state; ///< The edge dector for the options button

        edge_dector r2_state; ///< The edge dector for the right trigger
        edge_dector l2_state; ///< The edge dector for the left trigger

        edge_dector square_state; ///< The edge dector for the square button
        edge_dector triangle_state; ///< The edge dector for the triangle button
        edge_dector circle_state; ///< The edge dector for the circle button

        edge_dector dpad_up_state; ///< The edge dector for the dpad up button
        edge_dector dpad_down_state; ///< The edge dector for the dpad down button
        edge_dector dpad_left_state; ///< The edge dector for the dpad left button
        edge_dector dpad_right_state; ///< The edge dector for the dpad right button

        ssd1306 *display; ///< The display that the menus are being displayed on

        //list of all menu pointers
        menu<std::string> *status_screen; ///< The status screen menu
        menu<changeable_values<uint8_t>> *motor_speed_menu; ///< The motor speed menu
        menu<changeable_values<float>> *ramped_velocity_menu; ///< The ramped velocity menu menu
        menu<uint8_t> *servo_latched_menu;  ///< The servo latched menu
        menu<uint8_t> *servo_released_menu; ///< The servo released menu
        menu<uint8_t> *motors_calibrating_menu; ///< The motors calibrating menu
        menu<uint8_t> *motors_enable_menu; ///< The motors enable menu
        menu<uint8_t> *motors_disable_menu; ///< The motors disable menu
        menu<uint8_t> *errors_clear_menu; ///< The errors clear menu


        uint8_t motors_enabled; ///< The current state of the motors (only enabled / disabled)
        /**
         * @brief This function draws the status menu to the display
         */
        void draw_status_display();

        /**
         * @brief This function draws the motors en/dis abled menu to the display 
         */
        void triangle_pressed();

        /**
         * @brief This function draws the motor calibration menu to the display
         */
        void circle_pressed();

        /**
         * @brief This function draws the errors clear menu to the display
         */
        void square_pressed();

        /**
         * @brief This function draws the servo released menu to the display
         */
        void r2_pressed();

        /**
         * @brief This function draws the servo latched menu to the display
         */
        void l2_pressed();

        /**
         * @brief This function cycles throught the status, motor speed, and ramped velocity menu menus
         */
        void options_pressed();

        /**
         * @brief This function will switch which variable can be modified in the menus (if modifiable)
         * @details attempts to move the cursor one value down, will roll over if at the bottom of the menu
         */
        void dpad_up_pressed();

        /**
         * @brief This function will switch which variable can be modified in the menus (if modifiable)
         * @details attempts to move the cursor one value up, will roll over if at the top of the menu
         */
        void dpad_down_pressed();

        /**
         * @brief This function will decrease the value of the selected variable if modifiable)
         * @details this value that it will be changed by is set in the menu class
         */
        void dpad_left_pressed();

        /**
         * @brief This function will increase the value of the selected variable if modifiable)
         * @details this value that it will be changed by is set in the menu class
         */
        void dpad_right_pressed();


    public:
        /**
         * @brief This is the constructor for the menu handler
         * @param display : The display that the menus will be displayed on
         * @param status_screen : The status screen menu
         * @param motor_speed_menu : The motor speed menu
         * @param ramped_velocity_menu : The ramped velocity menu menu
         * @param servo_latched_menu The servo latched menu
         * @param servo_released_menu The servo released menu
         * @param motors_calibrating_menu The motors calibrating menu
         * @param motors_enable_menu The motors enable menu
         * @param motors_disable_menu The motors disable menu
         * @param errors_clear_menu The errors clear menu
         */
        menu_handler(ssd1306 *display, menu<std::string> *status_screen, menu<changeable_values<uint8_t>> *motor_speed_menu, menu<changeable_values<float>> *ramped_velocity_menu, 
            menu<uint8_t> *servo_latched_menu, menu<uint8_t> *servo_released_menu, menu<uint8_t> *motors_calibrating_menu,
            menu<uint8_t> *motors_enable_menu, menu<uint8_t> *motors_disable_menu, menu<uint8_t> *errors_clear_menu);
        /**
         * @brief This function will update the menu handler
         * @details This function will check the state of the inputs and update the display
         */
        void update();
};

#endif