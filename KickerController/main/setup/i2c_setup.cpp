

#include "../../include/setup/i2c_setup.h"

void i2c_setup() {
    i2c_mst_config.clk_source = I2C_CLK_SRC_DEFAULT;
    i2c_mst_config.i2c_port = I2C_NUM_0;
    i2c_mst_config.sda_io_num = sda_pin;
    i2c_mst_config.scl_io_num = scl_pin;
    i2c_mst_config.glitch_ignore_cnt = 0;
    i2c_mst_config.flags.enable_internal_pullup = true;
    i2c_new_master_bus(&i2c_mst_config, &bus_handle);
}