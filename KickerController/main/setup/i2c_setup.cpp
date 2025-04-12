/**
 * @file i2c_setup.cpp
 * @brief source file that sets up the i2c bus
 * @author Shane Wood
 * @date 11/15/2024
 */

#include "../../include/setup/i2c_setup.h"



void i2c_setup() {
  //i2c bus clock source
  i2c_mst_config.clk_source = I2C_CLK_SRC_DEFAULT;

  //i2c port
  i2c_mst_config.i2c_port = I2C_NUM_0;

  //pins for the i2c bus
  i2c_mst_config.sda_io_num = sda_pin;
  i2c_mst_config.scl_io_num = scl_pin;

  i2c_mst_config.glitch_ignore_cnt = 0;

  //set internal pullup resistors
  i2c_mst_config.flags.enable_internal_pullup = true;

  //set the bus
  i2c_new_master_bus(&i2c_mst_config, &bus_handle);
}