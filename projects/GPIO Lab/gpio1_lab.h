// file gpio1_lab.h
#pragma once

#include <stdbool.h>
#include <stdint.h>

// include this file at gpio_lab.c file
// #include "lpc40xx.h"

// NOTE: The IOCON is not part of this driver

/// Should alter the hardware registers to set the pin as input
void gpio1__set_as_input(uint8_t pin_num);

/// Should alter the hardware registers to set the pin as output
void gpio1__set_as_output(uint8_t pin_num);

/// Should alter the hardware registers to set the pin as high
void gpio1__set_high(uint8_t pin_num);

/// Should alter the hardware registers to set the pin as low
void gpio1__set_low(uint8_t pin_num);

/**
 * Should alter the hardware registers to set the pin as low
 *
 * @param {bool} high - true => set pin high, false => set pin low
 */
void gpio1__set(uint8_t pin_num, bool high);

/**
 * Should return the state of the pin (input or output, doesn't matter)
 *
 * @return {bool} level of pin high => true, low => false
 */
bool gpio1__get_level(uint8_t pin_num);

// typedef struct {
//   /* First get gpio0 driver to work only, and if you finish it
//    * you can do the extra credit to also make it work for other Ports
//    */
//   // uint8_t port;

//   uint8_t pin;
// } port_pin_s;