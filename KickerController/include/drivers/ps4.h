/**
 * @file pinout.h
 * @brief This is the file that contains the ps4 controller class (and internal tasks for usb host)
 * @author Shane Wood
 * @date 10/2/2025
*/

#ifndef __ps4__
#define __ps4__ 
#include "../all_includes.h"

extern SemaphoreHandle_t main_menu_values_mutex;
extern std::string contoller_connected;


extern SemaphoreHandle_t update_main_display_mutex;
extern bool update_main_display;

/**
 * @class ps4
 * @brief This class is for creating a ps4 controller object and handling all tasks related to the ps4 controller
 * @details A modified version of the example code from the esp-idf examples, with the addition of ds4 for decoding the ps4 /
 * controller data exported data is sent to a set of queues, hotswaping of controllers is supported
 * @ref https://github.com/espressif/esp-idf/blob/v5.3.2/examples/peripherals/usb/host/hid/main/hid_host_example.c
 * @ref https://www.psdevwiki.com/ps4/DS4-USB
 */
class ps4{
    static QueueHandle_t app_event_queue;

    /**
     * @brief APP event group
     *
     * Application logic can be different. There is a one among other ways to distingiush the
     * event by application event group.
     * In this example we have two event groups:
     * APP_EVENT            - General event, which is APP_QUIT_PIN press event (Generally, it is IO0).
     * APP_EVENT_HID_HOST   - HID Host Driver event, such as device connection/disconnection or input report.
     */
    typedef enum {
        APP_EVENT = 0,
        APP_EVENT_HID_HOST
    } app_event_group_t;

    /**
     * @brief APP event queue
     *
     * This event is used for delivering the HID Host event from callback to a task.
     */
    typedef struct {
        app_event_group_t event_group;
        /* HID Host - Device related info */
        struct {
            hid_host_device_handle_t handle;
            hid_host_driver_event_t event;
            void *arg;
        } hid_host_device;
    } app_event_queue_t;


    /**
     * @brief USB HID Host Generic Interface report callback handler
     *
     * 'generic' means anything else than mouse or keyboard
     *
     * @param[in] data    Pointer to input report data buffer
     * @param[in] length  Length of input report data buffer
     */
    static void hid_host_generic_report_callback(const uint8_t *const data, const int length);


    /**
     * @brief USB HID Host interface callback
     *
     * @param[in] hid_device_handle  HID Device handle
     * @param[in] event              HID Host interface event
     * @param[in] arg                Pointer to arguments, does not used
     */
    static void hid_host_interface_callback(hid_host_device_handle_t hid_device_handle,
                                    const hid_host_interface_event_t event,
                                    void *arg);


    /**
     * @brief USB HID Host Device event
     *
     * @param[in] hid_device_handle  HID Device handle
     * @param[in] event              HID Host Device event
     * @param[in] arg                Pointer to arguments, does not used
     */
    void hid_host_device_event(hid_host_device_handle_t hid_device_handle,
                            const hid_host_driver_event_t event,
                            void *arg);

    
    /**
     * @brief Start USB Host install and handle common USB host library events while app pin not low
     *
     * @param[in] arg  Not used
     */
    static void usb_lib_task(void *arg);


    /**
     * @brief HID Host Device callback
     *
     * Puts new HID Device event to the queue
     *
     * @param[in] hid_device_handle HID Device handle
     * @param[in] event             HID Device event
     * @param[in] arg               Not used
     */
    static void hid_host_device_callback(hid_host_device_handle_t hid_device_handle,
                                const hid_host_driver_event_t event,
                                void *arg);
    public:
    /**
     * @brief Construct the ps4 object, which creates the required tasks for the usb host, and processing the usb datafv
     */
        ps4();

};


#endif