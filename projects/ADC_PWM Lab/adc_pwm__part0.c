#include "FreeRTOS.h"
#include "adc.h"
#include "board_io.h"
#include "common_macros.h"
#include "gpio.h"
#include "lpc40xx.h"
#include "lpc_peripherals.h"
#include "periodic_scheduler.h"
#include "pwm1.h"
#include "sj2_cli.h"
#include "task.h"
#include <stdio.h>

// P2.0 PWM1
// P1.31 ADC[5]

void pwm_task(void *p);

int main(void) {

  puts("Starting RTOS");

  /*** ADC + PWM LAB Part 0 ***/
  xTaskCreate(pwm_task, "pwm task", 4096 / sizeof(void *), NULL, PRIORITY_LOW, NULL);
  vTaskStartScheduler(); 
  return 0;
}

/*** ADC + PWM LAB Part 0 ***/
void pwm_task(void *p) {
  pwm1__init_single_edge(1000);

  // Locate a GPIO pin that a PWM channel will control
  gpio__construct_with_function(2, 0, 1); // FUNC = 0b001

  // We only need to set PWM configuration once, and the HW will drive
  // the GPIO at 1000Hz, and control set its duty cycle to 50%
  pwm1__set_duty_cycle(PWM1__2_0, 50);

  // Continue to vary the duty cycle in the loop
  uint8_t percent = 0;
  while (1) {
    pwm1__set_duty_cycle(PWM1__2_0, percent);
    if (++percent > 100) {
      percent = 0;
    }
    vTaskDelay(100);
  }
}
