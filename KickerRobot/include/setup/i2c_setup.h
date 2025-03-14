#ifndef __i2c_setup__
#define __i2c_setup__

#include "../all_includes.h"
#include "../pinout.h"

extern i2c_master_bus_config_t i2c_mst_config;
extern i2c_master_bus_handle_t bus_handle;

/**
 * @brief Sets up the I2C bus, with the set pins and port number
 */
void i2c_setup();
#endif