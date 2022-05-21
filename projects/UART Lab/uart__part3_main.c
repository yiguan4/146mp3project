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
void uart__interrupt_read_task(void *p);
void uart__interrupt_write_task(void *p);
void board_1_sender_task(void *p);
void board_2_receiver_task(void *p);
/*NOTE: had to change the baud rate in this function static void 
peripherals_init__uart0_init(void)
  back to 115200
*/
int main(void) {
  puts("Starting RTOS\n");
  const uint32_t uart__clock_mhz = 96 * 1000 * 1000;
  const uint32_t uart__baud_rate = 9600;
  uart_lab__init(UART_2, uart__clock_mhz, uart__baud_rate);
  LPC_IOCON->P2_8 &= ~0b111;
  LPC_IOCON->P2_9 &= ~0b111;
  LPC_IOCON->P2_8 |= 0b010; // U2_TXD
  LPC_IOCON->P2_9 |= 0b010; // U2_RXD
  uart__enable_receive_interrupt(UART_2);

  /****************************UART LAB PART 3****************************/
  xTaskCreate(board_1_sender_task, "sender task", 4096 / sizeof(void *), "data sent", PRIORITY_HIGH, NULL);
  xTaskCreate(board_2_receiver_task, "reader task", 4096 / sizeof(void *), "data read", PRIORITY_LOW, NULL);
  vTaskStartScheduler();
  return 0;
}

/****************************UART LAB PART 3****************************/
void board_1_sender_task(void *p) {
  char number_as_string[16] = {0};
  while (true) {
    const int number = rand();
    sprintf(number_as_string, "%i", number);
    for (int i = 0; i <= strlen(number_as_string); i++) {
      uart_lab__polled_put(UART_2, number_as_string[i]);
      printf("Sent: %c\n", number_as_string[i]);
    }
    printf("Sent: %i over UART to the other board\n", number);
    vTaskDelay(3000);
  }
}
void board_2_receiver_task(void *p) {
  char number_as_string[16] = {0};
  int counter = 0;
  while (true) {
    char byte = 0;
    uart_lab__get_char_from_queue(&byte, portMAX_DELAY);
    printf("Received: %c\n", byte);
    if ('\0' == byte) {
      number_as_string[counter] = '\0';
      counter = 0;
      printf("Received this number from the other board: %s\n", number_as_string);
    }
    else {
      for (counter = 1; counter <= 16; counter++) {
        number_as_string[counter];
      }
    }
  }
}