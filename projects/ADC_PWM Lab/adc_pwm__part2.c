#include "FreeRTOS.h"
#include "adc.h"
#include "board_io.h"
#include "common_macros.h"
#include "gpio.h"
#include "lpc40xx.h"
#include "lpc_peripherals.h"
#include "periodic_scheduler.h"
#include "pwm1.h"
#include "queue.h"
#include "sj2_cli.h"
#include "task.h"
#include <stdio.h>

// P2.0 PWM1
// P1.31 ADC[5]

void pwm_task(void *p);
void adc_task(void *p);

// This is the queue handle we will need for the xQueue Send/Receive API
static QueueHandle_t adc_to_pwm_task_queue;

int main(void) {

  puts("Starting RTOS");
  /*** ADC + PWM LAB Part 2 ***/
  // Queue will only hold 1 integer
  adc_to_pwm_task_queue = xQueueCreate(1, sizeof(int));
  xTaskCreate(adc_task, "adc task", 4096 / sizeof(void *), NULL, PRIORITY_LOW, NULL);
  xTaskCreate(pwm_task, "pwm task", 4096 / sizeof(void *), NULL, PRIORITY_LOW, NULL);
  vTaskStartScheduler(); // This function never returns unless RTOS scheduler runs out of memory and fails
  return 0;
}

/*** ADC + PWM LAB Part 2 ***/
void adc_task(void *p) {
  // NOTE: Reuse the code from Part 1
  adc__initialize();
  adc__enable_burst_mode();
  gpio__construct_with_function(1, 31, 3); // FUNC = 0b011 and can be GPIO__FUNCTION_3
  // enables analog mode needs to be inactive (00) for it to be in analog
  LPC_IOCON->P1_31 &= ~(1 << 7);

  int adc_reading = 0; 

  while (1) {
    adc_reading = (int)adc__get_channel_reading_with_burst_mode(ADC__CHANNEL_5);
    // adc_reading = 100;
    xQueueSend(adc_to_pwm_task_queue, &adc_reading, 0);
    fprintf(stderr, "%i\n", adc_reading);
    vTaskDelay(100);
  }
}

void pwm_task(void *p) {
  // NOTE: Reuse the code from Part 0
  int adc_reading = 0;
  pwm1__init_single_edge(1000);
  gpio__construct_with_function(2, 0, 1); // FUNC = 0b001
  pwm1__set_duty_cycle(PWM1__2_0, 50);
  uint8_t percent = 0;
  while (1) {
    if (xQueueReceive(adc_to_pwm_task_queue, &adc_reading, 100)) {
      fprintf(stderr, "received %i", adc_reading);
    }
  }
}
