#include "../../include/tasks/odrive_driver_task.h"

void oDriveDriver(void *pv)
{
    static const twai_timing_config_t t_config = TWAI_TIMING_CONFIG_100KBITS();
    static const twai_filter_config_t f_config = TWAI_FILTER_CONFIG_ACCEPT_ALL();
    
    static const twai_general_config_t g_config = {
        .controller_id = 0,
        .mode = TWAI_MODE_NORMAL,
        .tx_io = canTXPin,
        .rx_io = canRXPin,
        .clkout_io = TWAI_IO_UNUSED,
        .bus_off_io = TWAI_IO_UNUSED,
        .tx_queue_len = 1,
        .rx_queue_len = 1,
        .alerts_enabled = TWAI_ALERT_NONE,
        .clkout_divider = 0,
        .intr_flags = 0
    };

    if (twai_driver_install(&g_config, &t_config, &f_config) != ESP_OK)
    {
        assert(false);
    }

    while (twai_start() != ESP_OK)
    {
        assert(false);
    }
    oDrive left_drive(oDrive0ID);
    oDrive right_drive(oDrive1ID);
    kicker_drive_train drive_train(&left_drive, &right_drive, battery_voltage_refresh_time);
    for (;;)
    {
        drive_train.update();
        vTaskDelay(pdMS_TO_TICKS(odrive_driver_wait_time));
    }
}