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
void adc_task(void *p);

int main(void) {

  puts("Starting RTOS");

  /*** ADC + PWM LAB Part 1 ***/
  xTaskCreate(adc_task, "adc task", 4096 / sizeof(void *), NULL, PRIORITY_LOW, NULL);
  vTaskStartScheduler();
  return 0;
}


/*** ADC + PWM LAB Part 1 ***/
void adc_task(void *p) {
  adc__initialize();
  adc__enable_burst_mode();

  gpio__construct_with_function(1, 31, 3); // FUNC = 0b011 and can be GPIO__FUNCTION_3
  // enables analog mode needs to be inactive (00) for it to be in analog
  LPC_IOCON->P1_31 &= ~(1 << 7);

  while (1) {
    const uint16_t adc_value = adc__get_channel_reading_with_burst_mode(ADC__CHANNEL_5);
    printf("%u\n", adc_value);
    vTaskDelay(500);
  }
}
