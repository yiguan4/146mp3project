#include "FreeRTOS.h"
#include "gpio.h"
#include "lpc40xx.h"
#include "task.h"
#include "uart_lab.h"
#include "board_io.h"
#include "common_macros.h"
#include "lpc_peripherals.h"
#include "periodic_scheduler.h"
#include "sj2_cli.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void uart_read_task(void *p);
void uart_write_task(void *p);

/*NOTE: had to change the baud rate in this function static void peripherals_init__uart0_init(void) back to 115200*/

int main(void) {
  puts("Starting RTOS\n");
  /****************************UART LAB PART 1****************************/
  const uint32_t uart__clock_mhz = 96 * 1000 * 1000;
  const uint32_t uart__baud_rate = 9600;
  uart_lab__init(UART_2, uart__clock_mhz, uart__baud_rate);
  LPC_IOCON->P2_8 &= ~0b111;
  LPC_IOCON->P2_9 &= ~0b111;
  LPC_IOCON->P2_8 |= 0b010; // U2_TXD
  LPC_IOCON->P2_9 |= 0b010; // U2_RXD
  uart__enable_receive_interrupt(UART_2);
  xTaskCreate(uart_read_task, "uart read task", 4096 / sizeof(void *), "data read",PRIORITY_LOW, NULL);
  xTaskCreate(uart_write_task, "uart write task", 4096 / sizeof(void *), "data sent", PRIORITY_HIGH, NULL);
  vTaskStartScheduler();
  return 0;
}
/****************************UART LAB PART 1****************************/
void uart_read_task(void *p) {
  while (1) {
    char input_byte;
    // fprintf(stderr, "Receiving Data!\n");
    uart_lab__polled_get(UART_2, &input_byte);
    printf("Data read: %c\n\n", input_byte);
    vTaskDelay(500);
  }
}
void uart_write_task(void *p) {
  while (1) {
    // fprintf(stderr, "Trasmitting Data!\n");
    uart_lab__polled_put(UART_2, 'A');
    vTaskDelay(500);
  }
}