#include "uart_lab.h"

void uart_lab_pin_configure(uart_number_e uart) {
  if (uart == UART_2) {
    gpio__construct_with_function(GPIO__PORT_2, 8, GPIO__FUNCTION_2);
    gpio__construct_with_function(GPIO__PORT_2, 9, GPIO__FUNCTION_2);
  } else if (uart == UART_3) {
    gpio__construct_with_function(GPIO__PORT_0, 0, GPIO__FUNCTION_2);
    gpio__construct_with_function(GPIO__PORT_0, 1, GPIO__FUNCTION_2);
  }
}

void uart_lab__init(uart_number_e uart, uint32_t peripheral_clock,
                    uint32_t baud_rate) {
  if (uart == UART_2) {
    LPC_SC->PCONP |= (1 << 24); // power

    LPC_UART2->LCR |= (1 << 7); // DLAB = 1
    LPC_UART2->LCR |= (3 << 0); // 8 bit char

    const uint16_t divider_16_bit = peripheral_clock / (16 * baud_rate);
    LPC_UART2->DLM = (divider_16_bit >> 8) & 0xFF;
    LPC_UART2->DLL = (divider_16_bit >> 0) & 0xFF;

    // 4. UART FIFO - Enabled FIFO, reset TX* RX buffers*
    LPC_UART2->FCR |= (1 << 0);
    LPC_UART2->FCR |= (1 << 1);
    LPC_UART2->FCR |= (1 << 2);

    LPC_UART2->LCR &= ~(1 << 7);

  } else if (uart == UART_3) {
    LPC_SC->PCONP |= (1 << 25);
    LPC_UART3->LCR |= (1 << 7);
    LPC_UART3->LCR |= (3 << 0);

    const uint16_t divider_16_bit = peripheral_clock / (16 * baud_rate);
    LPC_UART3->DLM = (divider_16_bit >> 8) & 0xFF;
    LPC_UART3->DLL = (divider_16_bit >> 0) & 0xFF;
    LPC_UART3->LCR &= ~(1 << 7);
  }
}

// Read the byte from RBR and actually save it to the pointer - RX
bool uart_lab__polled_get(uart_number_e uart, char *input_byte) {

  if (uart == UART_2) {
    const uint32_t recevie_data_bit_ready = (1 << 0);
    while (!(LPC_UART2->LSR & recevie_data_bit_ready)) {
      ;
    }
    *input_byte = LPC_UART2->RBR; // Copy data

  } else if (uart == UART_3) {
    const uint32_t recevie_data_bit_ready = (1 << 0);
    while (!(LPC_UART3->LSR & recevie_data_bit_ready)) {
      ;
    }
    *input_byte = LPC_UART3->RBR;
  }
}

// TX
bool uart_lab__polled_put(uart_number_e uart, char output_byte) {
  if (uart == UART_2) {
    const uint32_t transmit_hold_bit_empty = (1 << 5);
    while (!(LPC_UART2->LSR & transmit_hold_bit_empty)) {
      ;
    }
    LPC_UART2->THR = output_byte; // Copy output

  } else if (uart == UART_3) {

    const uint32_t transmit_hold_bit_empty = (1 << 5);
    while (!(LPC_UART3->LSR & transmit_hold_bit_empty)) {
      ;
    }
    LPC_UART3->THR = output_byte;
  }
}
