#include "uart_lab.h"
#include "FreeRTOS.h"
#include "lpc40xx.h"
#include "lpc_peripherals.h"
#include "queue.h"
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

/*
    Pins for UART2 from schematic
    P2_8 U2_TXD
    P2_9 U2_RXD
*/

// Private queue handle of our uart_lab.c
static QueueHandle_t your_uart_rx_queue;

void uart_lab__init(uart_number_e uart, uint32_t peripheral_clock, uint32_t baud_rate) {
  const uint32_t uart2__power_up = (1 << 24);
  LPC_SC->PCONP |= uart2__power_up;

  const uint32_t uart2__div_16_bit = peripheral_clock / (16 * baud_rate);
  const uint32_t uart2__lower_8_bits_divisor_latch = (uart2__div_16_bit >> 0);
  const uint32_t uart2__upper_8_bits_divisor_latch = (uart2__div_16_bit >> 8);
  const uint32_t uart2__dlab_enable = (1 << 7);
  // const uint32_t uart2__fdr_enable = (1 << 4);
  LPC_UART2->LCR |= (3 << 0);
  LPC_UART2->LCR |= uart2__dlab_enable;
  LPC_UART2->DLM = uart2__upper_8_bits_divisor_latch;
  LPC_UART2->DLL = uart2__lower_8_bits_divisor_latch;
  // LPC_UART2->FDR = uart2__fdr_enable;
  LPC_UART2->LCR &= ~(uart2__dlab_enable);
}

// Read the byte from RBR and actually save it to the pointer
bool uart_lab__polled_get(uart_number_e uart, char *input_byte) {
  const uint32_t uart2__check_rdr_bit = (1 << 0);
  while (!(LPC_UART2->LSR & uart2__check_rdr_bit)) {
    ;
  }
  *input_byte = LPC_UART2->RBR;
}

bool uart_lab__polled_put(uart_number_e uart, char output_byte) {
  const uint32_t uart2__check_thre_bit = (1 << 5);
  while (!(LPC_UART2->LSR & uart2__check_thre_bit)) {
    ;
  }
  LPC_UART2->THR = output_byte;
}

/****************************UART LAB PART 2****************************/
// Private function of our uart_lab.c
static void your_receive_interrupt(void) {
  const uint32_t receive_data_available_occurred = (2 << 1);
  const uint32_t uart2__check_thre_bit = (1 << 5);
  char byte;

  if (LPC_UART2->IIR & receive_data_available_occurred) {
    // fprintf(stderr, "Interrupt Occurred - receive_data_available");
    if (LPC_UART2->LSR & uart2__check_thre_bit) {
      byte = LPC_UART2->RBR;
    }
  }
  xQueueSendFromISR(your_uart_rx_queue, &byte, NULL);
}

// Public function to enable UART interrupt
void uart__enable_receive_interrupt(uart_number_e uart_number) {
  LPC_UART2->LCR &= ~(1 << 7);
  lpc_peripheral__enable_interrupt(UART2_IRQn, your_receive_interrupt, "Receive Interrupt");
  NVIC_EnableIRQ(UART2_IRQn);
  LPC_UART2->IER |= (1 << 0);
  your_uart_rx_queue = xQueueCreate(10, sizeof(int));
}

// Public function to get a char from the queue (this function should work without modification)
bool uart_lab__get_char_from_queue(char *input_byte, uint32_t timeout) {
  return xQueueReceive(your_uart_rx_queue, input_byte, timeout);
}