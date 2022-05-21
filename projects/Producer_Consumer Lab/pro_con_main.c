#include "FreeRTOS.h"
#include "gpio.h"
#include "lpc40xx.h"
#include "queue.h"
#include "task.h"
#include "board_io.h"
#include "common_macros.h"
#include "lpc_peripherals.h"
#include "periodic_scheduler.h"
#include "sj2_cli.h"
#include <stdio.h>

static QueueHandle_t switch_queue;

void producer(void *p);
void consumer(void *p);
int get_switch_input_from_switch0(void);

typedef enum { switch__off, switch__on } switch_e;

int main(void) {
  puts("Starting RTOS");
  xTaskCreate(producer, "producer task", 4096 / sizeof(void *), "producer", PRIORITY_HIGH, NULL);
  xTaskCreate(consumer, "consumer task", 4096 / sizeof(void *), "consumer", PRIORITY_LOW, NULL);
  switch_queue = xQueueCreate(1, sizeof(switch_e));
  vTaskStartScheduler();
  return 0;
}

// tx
void producer(void *p) {
  while (1) {
    // This xQueueSend() will internally switch context to "consumer" task because it is higher priority than this
    // "producer" task Then, when the consumer task sleeps, we will resume out of xQueueSend()and go over to the next
    // line
    const switch_e switch_value = get_switch_input_from_switch0();
    printf("\nSending data = %d!\n", switch_value);
    xQueueSend(switch_queue, &switch_value, 0);
    printf("\n%d = Data successfully sent to queue!\n", switch_value);
    vTaskDelay(1000); // gives time for the consumer task to run
  }
}

// rx
void consumer(void *p) {
  switch_e switch_value;
  while (1) {
    printf("\nWaiting to receive data!\n");
    xQueueReceive(switch_queue, &switch_value, portMAX_DELAY);
    printf("\n%d = Data successfully received from queue!\n", switch_value);
  }
}

int get_switch_input_from_switch0(void) {
  const uint32_t switch3_init = (1 << 29); // initializes SW3 as an input
  LPC_GPIO0->DIR &= ~(switch3_init);
  if (LPC_GPIO0->PIN & switch3_init) {
    return 1;
  } else {
    return 0;
  }
}