idf_component_register(SRCS "./drivers/oDrive.cpp" "./drivers/servo.cpp" "./drivers/ssd1306.cpp" "./drivers/led_strip_driver.cpp"
                            "./setup/esp_now_callback_setup.cpp" "./setup/esp_now_setup.cpp" "./setup/i2c_setup.cpp" "./setup/led_strip_setup.cpp"
                            "./supporting/font.cpp" "./supporting/menus.cpp" "./supporting/kicker_drive_train.cpp"
                            "./tasks/display_driver_task.cpp" "./tasks/esp_now_task.cpp" "./tasks/kicker_drive_train_task.cpp" "./tasks/servo_driver_task.cpp"
                            "main.cpp"
                            INCLUDE_DIRS "." "./drivers" "./setup" "./supporting")