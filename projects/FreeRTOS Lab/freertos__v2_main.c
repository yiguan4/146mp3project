#include <stdio.h>
#include "FreeRTOS.h"
#include "task.h"
#include "board_io.h"
#include "common_macros.h"
#include "periodic_scheduler.h"
#include "sj2_cli.h"

static void task_one(void *task_parameter);
static void task_two(void *task_parameter);

int main(void) {
  puts("Starting RTOS");
  vTaskStartScheduler(); // This function never returns unless RTOS scheduler runs out of memory and fails
  xTaskCreate(task_one, "one", 4096 / sizeof(void *), NULL, 2, NULL);
  xTaskCreate(task_two, "two", 4096 / sizeof(void *), NULL, 1, NULL);
  return 0;
}

static void task_one(void *task_parameter) {
  while (1) {
    // Read existing main.c regarding when we should use fprintf(stderr...) in place of printf()
    // For this lab, we will use fprintf(stderr, ...)
    fprintf(stderr, "AAAAAAAAAAAA");
    vTaskDelay(100); // tick rate
  }
}
 
static void task_two(void *task_parameter) {
  while (1) {
    fprintf(stderr, "bbbbbbbbbbbb");
    vTaskDelay(100);
  }
}