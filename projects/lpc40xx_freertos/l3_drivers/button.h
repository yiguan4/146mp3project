#pragma once
#include "gpio.h"
#include "lpc40xx.h"
#include "lpc_peripherals.h"

#include <stdio.h>

static gpio_s select_button;
static gpio_s up_button;
static gpio_s down_button;
static gpio_s left_button;
static gpio_s right_button;
static gpio_s playpause_button;
void button_init(void);
void button_set_as_input(void);