#include <stdio.h>
#include "FreeRTOS.h"
#include "lpc40xx.h"
#include "task.h"
#include "board_io.h"
#include "common_macros.h"
#include "gpio0_lab.h"
#include "gpio1_lab.h"
#include "lpc_peripherals.h"
#include "periodic_scheduler.h"
#include "semphr.h"
#include "sj2_cli.h"

static SemaphoreHandle_t switch_pressed_signal;
void gpio_interrupt(void); // Interrupt Part 0 Lab

int main(void) {
  puts("Starting RTOS");
 
  /***** Interrupt PART 0 LAB *****/
  LPC_GPIO0->DIR &= ~(1 << 29);        // set P0.29 as input (SW)
  LPC_GPIOINT->IO0IntEnF |= (1 << 29); // enables falling edge interrupt for Port0

  function lpc_peripheral__enable_interrupt(GPIO_IRQn, gpio_interrupt, "GPIO INTERRUPT");
 
  NVIC_EnableIRQ(GPIO_IRQn);
 
  // static port_pin_s SW = {29};  // P0.29
  // static port_pin_s led = {18}; // P1.18
 
  // LPC_IOCON->P1_18 = 0b000;    // ensure that the pin is used as a GPIO
  // LPC_GPIO1->DIR |= (1 << 18); // set P1.18 as output
  while (1) {
    delay__ms(100);
    gpio0__set_high(led->pin); // turns on LED
    vTaskDelay(100);           // waits 100ms
    gpio0__set_low(led->pin);  // turns off LED
    vTaskDelay(100);           // waits 100ms
  }
 
//   xTaskCreate(sleep_on_sem_task, "sem", (512U * 4) / sizeof(void *), NULL, PRIORITY_LOW, NULL);
//   vTaskStartScheduler(); 
  return 0;
}
/***** Interrupt PART 0 LAB *****/
void gpio_interrupt(void) {
  LPC_GPIOINT->IO0IntClr |= (1 << 29);
  fprintf(stderr, "INSIDE ISR\n");
  if (LPC_GPIOINT->IO0IntStatF & ((1 << 29) == 1)) {
    fprintf(stderr, "Interrupt WORKED!");
  }
}
