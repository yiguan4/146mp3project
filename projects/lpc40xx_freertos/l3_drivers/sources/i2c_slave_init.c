#include "i2c_slave_init.h"
#include "gpio.h"
#include "i2c.h"
#include "lpc40xx.h"
#include "peripherals_init.h"

void i2c2__slave_init(uint8_t slave_address_to_respond_to) {
  peripherals_init();

  gpio__construct_with_function(0, 10, 2); // SDA2
  gpio__construct_with_function(0, 11, 2); // SCL2

  LPC_I2C2->ADR0 = (slave_address_to_respond_to << 0);

  LPC_I2C2->CONSET = 0x44; // enable slave function
}
