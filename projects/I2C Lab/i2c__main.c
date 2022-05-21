#include "FreeRTOS.h"
#include "acceleration.h"
#include "ff.h"
#include "gpio.h"
#include "lpc40xx.h"
#include "queue.h"
#include "sl_string.h"
#include "task.h"
#include "i2c.h"
#include "board_io.h"
#include "event_groups.h"
#include "i2c_slave_init.h"
#include "lpc_peripherals.h"
#include "periodic_scheduler.h"
#include "sj2_cli.h"
#include <stdio.h>
#include <string.h>
 
static volatile uint8_t slave_memory[256];
 
bool i2c_slave_receive__write_memory(uint8_t register_index, uint8_t memory_value) {
 if (register_index <= 256) {
   slave_memory[register_index] = memory_value;
   return true;
 } else {
   return false;
 }
}
 
bool i2c_slave_transmit__read_memory(uint8_t register_index, uint8_t *memory_ptr) {
 if (register_index <= 256) {
   *memory_ptr = slave_memory[register_index];
   return true;
 } else {
   return false;
 }
}

int main(void) {
  i2c__init_slave(0x86);
  
  while (1) {
    if (slave_memory[0] == 0) {
      LPC_GPIO2->PIN |= (1 << 3);
    } else {
      LPC_GPIO2->PIN &= ~(1 << 3);
    }
  }

  return -1;
}
