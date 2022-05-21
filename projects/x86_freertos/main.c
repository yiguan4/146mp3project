#include <stdio.h>

#include "FreeRTOS.h"
#include "queue.h"
#include "task.h"

QueueHandle_t q;
int x, y;

void producer(void *p) {
  while (1) {
    xQueueSend(q, &x, 0);
    vTaskDelay(100);
  }
}

void consumer(void *p) {
  while (1) {
    xQueueReceive(q, &y, 50);
    printf("Received %d\n", y);
  }
}

int main(void) {
  xTaskCreate(producer, "producer", 1024, NULL, PRIORITY_HIGH, NULL);
  xTaskCreate(consumer, "consumer", 1024, NULL, PRIORITY_LOW, NULL);
  q = xQueueCreate(1, sizeof(int));
  vTaskStartScheduler();
  return 0;
}

// //#include "cpu_utilization_task.h"
// static void task_one(void *task_parameter);
// static void task_two(void *task_parameter);

/**
 * This POSIX based FreeRTOS simulator is based on:
 * https://github.com/linvis/FreeRTOS-Sim
 *
 * Do not use for production!
 * There may be issues that need full validation of this project to make it production intent.
 * This is a great teaching tool though :)
 */
// int main(void) {
//   // xTaskCreate(cpu_utilization_print_task, "cpu", 1, NULL, PRIORITY_LOW, NULL);

//   // puts("Starting FreeRTOS Scheduler ..... \r\n");

//   xTaskCreate(task_one, "one", 4096 / sizeof(void *), NULL, 1, NULL);
//   xTaskCreate(task_two, "two", 4096 / sizeof(void *), NULL, 1, NULL);

//   vTaskStartScheduler();

//   return 0;
// }

// static void task_one(void *task_parameter) {
//   while (1) {
//     // Read existing main.c regarding when we should use fprintf(stderr...) in place of printf()
//     // For this lab, we will use fprintf(stderr, ...)
//     fprintf(stderr, "AAAAAAAAAAAA");
//     vTaskDelay(1); // tick rate
//   }
// }

// static void task_two(void *task_parameter) {
//   while (1) {
//     fprintf(stderr, "bbbbbbbbbbbb");
//     vTaskDelay(1);
//   }
// }