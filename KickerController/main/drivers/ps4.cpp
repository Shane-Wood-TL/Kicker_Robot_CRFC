/**
 * @file ps4.cpp
 * @brief This is the file that contains the ps4 controller class (and internal tasks for usb host)
 * @author Shane Wood
 */
#include "../../include/drivers/ps4.h"

QueueHandle_t ps4::app_event_queue = nullptr;

extern std::atomic<uint8_t> controller_byte_2;
extern std::atomic<uint8_t> controller_byte_4;
extern std::atomic<uint8_t> controller_byte_5;
extern std::atomic<uint8_t> controller_byte_6;

extern std::atomic<bool> ps4_controller_connected;


extern SemaphoreHandle_t new_controller_data;

void ps4::hid_host_generic_report_callback(const uint8_t *const data, const int length)
{
    controller_byte_2 = data[values_2];
    controller_byte_4 = data[values_4];
    controller_byte_5 = data[values_5];
    controller_byte_6 = data[values_6];
    xSemaphoreGive(new_controller_data);
}

    //https://www.psdevwiki.com/ps4/DS4-USB
    /*
    byte 1, left stick x
    byte 2, left stick y
    byte 3, right sick x
    byte 4, right stick y

    byte 5 [7] = triangle
    byte 5 [6] = circle
    byte 5 [5] = x
    byte 5 [4] = square

    byte 5 [3:0] = directional buttons
    0111 = left + up
    0110 = left
    0101 = left + down
    0100 = down
    0011 = right + down
    0010 = right
    0001 = right + up
    0000 = up
    1111 = nothing????

    byte 6 [7] = r3 0x80
    byte 6 [6] = l3  0x40
    btye 6 [5] = options 0x20
    byte 6 [4] = share 0x10

    byte 6 [3] = r2 0x08
    byte 6 [2] = l2 0x04
    byte 6 [1] = r1 0x02
    byte 6 [0] = l1 0x01

    byte 7[0] = ps button

    byte 8 = l2 trigger
    byte 9 = r2 trigger

    byte 12 = battery level
    */


    
void ps4::hid_host_interface_callback(hid_host_device_handle_t hid_device_handle,
                                 const hid_host_interface_event_t event,
                                 void *arg)
{
    uint8_t data[usb_data_read_size] = {0};
    size_t data_length = 0;
    hid_host_dev_params_t dev_params;
    ESP_ERROR_CHECK(hid_host_device_get_params(hid_device_handle, &dev_params));

    switch (event) {
    case HID_HOST_INTERFACE_EVENT_INPUT_REPORT:
        ESP_ERROR_CHECK(hid_host_device_get_raw_input_report_data(hid_device_handle,
                                                                  data,
                                                                  usb_data_read_size,
                                                                  &data_length));

        if (HID_SUBCLASS_BOOT_INTERFACE == dev_params.sub_class) {

        } else {
            hid_host_generic_report_callback(data, data_length);
        }

        break;
    case HID_HOST_INTERFACE_EVENT_DISCONNECTED:
        ESP_LOGI("USB", "HID Device, protocol DISCONNECTED");
        ESP_ERROR_CHECK(hid_host_device_close(hid_device_handle));
        if(xSemaphoreTake(main_menu_values_mutex, portMAX_DELAY)){
            contoller_connected = "DisCon";
            ps4_controller_connected = false;
            xSemaphoreGive(main_menu_values_mutex);
            if(xSemaphoreTake(update_main_display_mutex, portMAX_DELAY)){
                update_main_display = true;
                xSemaphoreGive(update_main_display_mutex);
            }
        }
        break;
    case HID_HOST_INTERFACE_EVENT_TRANSFER_ERROR:
        ESP_LOGI("USB", "HID Device, protocol TRANSFER_ERROR");
        break;
    default:
        ESP_LOGE("USB", "HID Device, protocol Unhandled event");
        break;
    }
}




void ps4::hid_host_device_event(hid_host_device_handle_t hid_device_handle,
                           const hid_host_driver_event_t event,
                           void *arg)
{
    hid_host_dev_params_t dev_params;
    ESP_ERROR_CHECK(hid_host_device_get_params(hid_device_handle, &dev_params));

    switch (event) {
        case HID_HOST_DRIVER_EVENT_CONNECTED:{
            ESP_LOGI("USB", "HID Device, protocol CONNECTED");
            if(xSemaphoreTake(main_menu_values_mutex, portMAX_DELAY)){
                contoller_connected = "Conn";
                ps4_controller_connected = true;
                xSemaphoreGive(main_menu_values_mutex); 
                if(xSemaphoreTake(update_main_display_mutex, portMAX_DELAY)){
                    update_main_display = true;
                xSemaphoreGive(update_main_display_mutex);
                }
            }
            const hid_host_device_config_t dev_config = {
                .callback = hid_host_interface_callback,
                .callback_arg = NULL
            };

            ESP_ERROR_CHECK(hid_host_device_open(hid_device_handle, &dev_config));
            if (HID_SUBCLASS_BOOT_INTERFACE == dev_params.sub_class) {
                printf("b\n");
                ESP_ERROR_CHECK(hid_class_request_set_protocol(hid_device_handle, HID_REPORT_PROTOCOL_BOOT));
                if (HID_PROTOCOL_KEYBOARD == dev_params.proto) {
                    printf("c\n");
                    ESP_ERROR_CHECK(hid_class_request_set_idle(hid_device_handle, 0, 0));
                }
            }
            ESP_ERROR_CHECK(hid_host_device_start(hid_device_handle));
            break;
        }
        default:{
            printf("a\n");
            break;
        }
    }
}

void ps4::usb_lib_task(void *arg){
        const usb_host_config_t host_config = {
        .skip_phy_setup = false,
        .intr_flags = ESP_INTR_FLAG_LEVEL1,
        .enum_filter_cb = NULL
    };

    ESP_ERROR_CHECK(usb_host_install(&host_config));
    xTaskNotifyGive((TaskHandle_t)arg);

    while (true) {
        uint32_t event_flags;
        usb_host_lib_handle_events(portMAX_DELAY, &event_flags);
        // In this example, there is only one client registered
        // So, once we deregister the client, this call must succeed with ESP_OK
        if (event_flags & USB_HOST_LIB_EVENT_FLAGS_NO_CLIENTS) {
            ESP_ERROR_CHECK(usb_host_device_free_all());
            break;
        }
    }

    ESP_LOGI("USB", "USB shutdown");
    // Clean up USB Host
    vTaskDelay(usb_lib_disconnect_wait_time); // Short delay to allow clients clean-up
    ESP_ERROR_CHECK(usb_host_uninstall());
    vTaskDelete(NULL);
}

void ps4::hid_host_device_callback(hid_host_device_handle_t hid_device_handle,
                                const hid_host_driver_event_t event,
                                void *arg){
        app_event_queue_t evt_queue = {
        .event_group = APP_EVENT_HID_HOST,
        // HID Host Device related info
        .hid_host_device = {
            .handle = hid_device_handle,
            .event = event,
            .arg = arg
        }
    };

    if (app_event_queue) {
        xQueueSend(app_event_queue, &evt_queue, 0);
    }
}

ps4::ps4(){
    BaseType_t task_created;
    app_event_queue_t evt_queue;
    task_created = xTaskCreatePinnedToCore(usb_lib_task,
                                           "usb_events",
                                           usb_events_stack_size,
                                           xTaskGetCurrentTaskHandle(),
                                           usb_events_priority, NULL, 0);
    assert(task_created == pdTRUE);


    // Wait for notification from usb_lib_task to proceed
    ulTaskNotifyTake(false, usb_lib_task_wait_time);

    /*
    * HID host driver configuration
    * - create background task for handling low level event inside the HID driver
    * - provide the device callback to get new HID Device connection event
    */
    const hid_host_driver_config_t hid_host_driver_config = {
        .create_background_task = true,
        .task_priority = hid_host_drvier_priority,
        .stack_size = usb_events_stack_size,
        .core_id = 0,
        .callback = hid_host_device_callback,
        .callback_arg = NULL
    };

    ESP_ERROR_CHECK(hid_host_install(&hid_host_driver_config));

    // Create queue
    app_event_queue = xQueueCreate(usb_events_queue_size, sizeof(app_event_queue_t));

    ESP_LOGI("USB", "Waiting for HID Device to be connected");
    for(;;){
        if (xQueueReceive(app_event_queue, &evt_queue, portMAX_DELAY)) {
            if (APP_EVENT == evt_queue.event_group) {
                // User pressed button
                usb_host_lib_info_t lib_info;
                ESP_ERROR_CHECK(usb_host_lib_info(&lib_info));
                if (lib_info.num_devices == 0) {
                    // End while cycle
                    break;
                } else {
                    ESP_LOGW("USB", "To shutdown example, remove all USB devices and press button again.");
                    // Keep polling
                }
            }

            if (APP_EVENT_HID_HOST ==  evt_queue.event_group) {
                hid_host_device_event(evt_queue.hid_host_device.handle,
                                      evt_queue.hid_host_device.event,
                                      evt_queue.hid_host_device.arg);
            }
        }
    }
}