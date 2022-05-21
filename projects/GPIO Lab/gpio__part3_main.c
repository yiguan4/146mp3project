#include <stdio.h>
#include "FreeRTOS.h"
#include "task.h"
#include "board_io.h"
#include "common_macros.h"
#include "gpio_lab.h"
#include "periodic_scheduler.h"
#include "semphr.h"
#include "sj2_cli.h"

static SemaphoreHandle_t switch_press_indication; // GPIO Part 3 Lab
static void switch_task(void *task_parameter); // GPIO Part 3 Lab
static void led_task(void *task_parameter); // GPIO Part 3 Lab

int main(void) {
  puts("Starting RTOS");
  /***** GPIO PART 3 LAB *****/
  switch_press_indication = xSemaphoreCreateBinary();
  static port_pin_s SW = {15};
  static port_pin_s led = {26};
 
  xTaskCreate(switch_task, "switch", 2048 / sizeof(void *), (void *)&SW, 1, NULL);
  xTaskCreate(led_task, "led", 2048 / sizeof(void *), (void *)&led, 1, NULL);
  vTaskStartScheduler(); // This function never returns unless RTOS scheduler runs out of memory and fails
  return 0;
}
 
/***** GPIO PART 3 LAB *****/
void led_task(void *task_parameter) {
  port_pin_s *led = (port_pin_s *)task_parameter;
  gpio0__set_as_output(led->pin);
  while (true) {
   if (xSemaphoreTake(switch_press_indication, 1000)) {
      puts("START BLINKING!");
      gpio0__set_high(led->pin);
      vTaskDelay(100);
      gpio0__set_low(led->pin);
      vTaskDelay(100);
    } else {
      puts("Timeout: No switch press indication for 1000ms");
    }
  }
}
 
void switch_task(void *task_parameter) {
  port_pin_s *SW = (port_pin_s *)task_parameter;
  LPC_IOCON->P1_15 = 0b01000; // Pull-down resistor enabled in order to use switch 1
  gpio0__set_as_input(SW->pin);
  while (true) {
    if (gpio0__get_level(SW->pin)) {
      puts("BUTTON PRESSED!");
      xSemaphoreGive(switch_press_indication);
    }
    vTaskDelay(100);
  }
}
