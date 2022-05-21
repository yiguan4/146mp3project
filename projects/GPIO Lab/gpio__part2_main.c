#include <stdio.h>
#include "FreeRTOS.h"
#include "task.h"
#include "board_io.h"
#include "common_macros.h"
#include "gpio_lab.h"
#include "periodic_scheduler.h"
#include "semphr.h"
#include "sj2_cli.h"

static void led_task(void *task_parameter); // GPIO Part 1 & 2 Lab
 
int main(void) {
  puts("Starting RTOS");
  /***** GPIO PARTS 1 & 2 LAB *****/
  static port_pin_s led0 = {24};
  static port_pin_s led1 = {26};
 
  xTaskCreate(led_task, "led0", 2048 / sizeof(void *), (void *)&led0, 1,
              NULL); /* &led0 is a task parameter going to led_task */
  xTaskCreate(led_task, "led1", 2048 / sizeof(void *), (void *)&led1, 1, NULL);
  vTaskStartScheduler(); // This function never returns unless RTOS scheduler runs out of memory and fails
  return 0;
}
 
/***** GPIO PARTS 1 & 2 LAB *****/
typedef struct {
  /* First get gpio0 driver to work only, and if you finish it
   * you can do the extra credit to also make it work for other Ports
   */
  // uint8_t port;
 
  uint8_t pin;
} port_pin_s;
 
void led_task(void *task_parameter) {
  const port_pin_s *led = (const)(port_pin_s *)(task_parameter);
  while (true) {
    gpio0__set_high(led->pin);
    vTaskDelay(100);
 
    gpio0__set_low(led->pin);
    vTaskDelay(100);
  }
}
