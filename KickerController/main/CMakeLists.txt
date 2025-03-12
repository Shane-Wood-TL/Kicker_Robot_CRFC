idf_component_register(SRCS "./drivers/ps4.cpp" "./drivers/ssd1306.cpp"
                            "./supporting/edge_dector.cpp" "./supporting/font.cpp" "./supporting/menu_handler.cpp" "./supporting/menus.cpp"
                            "./setup/esp_now_callback_setup.cpp" "./setup/esp_now_setup.cpp" "./setup/i2c_setup.cpp"
                            "./tasks/display_task.cpp" "./tasks/ps4_data_processor_task.cpp" "./tasks/ps4_task.cpp" "./tasks/send_data_task.cpp"
                            "main.cpp"
                            INCLUDE_DIRS "." "./drivers" "./setup" "./supporting" "./tasks")