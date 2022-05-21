// #include "lpc_peripherals.h"
// #include "periodic_scheduler.h"
// #include "semphr.h"
// #include "sj2_cli.h"
// #include "common_macros.h"
#include "FreeRTOS.h"
#include "adc.h"
#include "board_io.h"
#include "gpio.h"
#include "lpc40xx.h"
#include "pwm1.h"
#include "task.h"
#include <stdio.h>
 
// typedef struct {
//   uint8_t pin;
// } port_pin_s;
 
// P2.0 PWM1
// P1.31 ADC[5]
void pwm_task(void *p);
void adc_task(void *p);
 
int main(void) {
 
  puts("Starting RTOS");
 
  /*** ADC + PWM LAB Part 0 ***/
  xTaskCreate(pwm_task, "pwm task", 4096 / sizeof(void *), NULL, PRIORITY_LOW, NULL);
  vTaskStartScheduler(); // This function never returns unless RTOS scheduler runs out of memory and fails
  return 0;
}
 
/*** ADC + PWM LAB Part 0 ***/
void pwm_task(void *p) {
  pwm1__init_single_edge(1000);
 
  gpio__construct_with_function(2, 0, 1); // FUNC = 0b001
  pwm1__set_duty_cycle(PWM1__2_0, 50);
  uint8_t percent = 0;
  while (1) {
    pwm1__set_duty_cycle(PWM1__2_0, percent);
 
    if (++percent > 100) {
      percent = 0;
    }
 
    vTaskDelay(100);
  }
}
