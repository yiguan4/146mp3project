#include <stdio.h>
#include "FreeRTOS.h"
#include "task.h"
#include "board_io.h"
#include "common_macros.h"
#include "gpio_lab.h"
#include "periodic_scheduler.h"
#include "semphr.h"
#include "sj2_cli.h"
 
static void led_task(void *pvParameters); //GPIO Part 0 Lab
 
int main(void) {
  puts("Starting RTOS");
  
  /***** GPIO PART 0 LAB *****/
  // Create FreeRTOS LED task
  xTaskCreate(led_task, "led", 2048 / sizeof(void *), NULL, 1, NULL); // GPIO P0
  vTaskStartScheduler(); // This function never returns unless RTOS scheduler runs out of memory and fails
  return 0;
}
 
 
/***** GPIO PART 0 LAB *****/
static void led_task(void *pvParameters) {
  LPC_GPIO1->DIR |= (1 << 18);
  while (true) {
    LPC_GPIO1->PIN &= ~(1 << 18);
    vTaskDelay(500);
    LPC_GPIO1->PIN |= (1 << 18);
    vTaskDelay(500);
  }
}
