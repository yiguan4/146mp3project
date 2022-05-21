#include "i2c_slave_functions.h"
#include "gpio.h"
#include "i2c.h"
#include "lpc40xx.h"
#include "peripherals_init.h"

static volatile uint8_t slave_memory[256];

// Read the data from slave_memory[memory_index] to *memory pointer
// return true if all is well (memory index is within bounds)
bool i2c_slave_receive__write_memory(uint8_t register_index,
                                     uint8_t memory_value) {
  if (register_index <= 256) {
    slave_memory[register_index] = memory_value;
    return true;
  } else {
    return false;
  }
}

// Write the memory_value at slave_memory[memory_index]
// return true if memory_index is within bounds

bool i2c_slave_transmit__read_memory(uint8_t register_index,
                                     uint8_t *memory_ptr) {
  if (register_index <= 256) {
    *memory_ptr = slave_memory[register_index];
    return true;
  } else {
    return false;
  }
}
