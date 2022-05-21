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

void sleep_on_sem_task(void *p); // Interrupt Part 1 Lab
static SemaphoreHandle_t switch_pressed_signal;
 
int main(void) {
 
  puts("Starting RTOS");
 
  /***** Interrupt PART 1 LAB *****/
  switch_pressed_signal = xSemaphoreCreateBinary(); // Create your binary semaphore
  static port_pin_s SW = {15};
  static port_pin_s led = {26};
  LPC_IOCON->P1_26 = 0b000;            // ensure that the pin is used as a GPIO
  LPC_IOCON->P1_15 = 0b01000;          // Pull-down resistor enabled in order to use switch
  LPC_GPIOINT->IO0IntEnF |= (1 << 15); // enables falling edge interrupt for Port0
  lpc_peripheral__enable_interrupt(GPIO_IRQn, gpio_interrupt,
                                   "GPIO INTERRUPT"); // TODO: Setup interrupt by re-using code from Part 0
  NVIC_EnableIRQ(GPIO_IRQn);                          // Enable interrupt gate for the GPIO
 
  xTaskCreate(sleep_on_sem_task, "sem", (512U * 4) / sizeof(void *), NULL, PRIORITY_LOW, NULL);
  vTaskStartScheduler(); // This function never returns unless RTOS scheduler runs out of memory and fails
  return 0;
}
/***** Interrupt PART 1 LAB *****/
void gpio_interrupt(void) {
  fprintf(stderr, "ISR Entry\n");
  xSemaphoreGiveFromISR(switch_pressed_signal, NULL);
  LPC_GPIOINT->IO0IntClr |= (1 << 29);
}
 
void sleep_on_sem_task(void *p) {
  while (1) {
    port_pin_s *led = (port_pin_s *)p;
    gpio1__set_as_output(led->pin);
    if (xSemaphoreTake(switch_pressed_signal, portMAX_DELAY)) {
      puts("START BLINKING!\n");
      gpio1__set_high(led->pin); // turns off LED
      vTaskDelay(100);           // waits 100ms
      gpio1__set_low(led->pin);  // turns on LED
      vTaskDelay(100);           // waits 100ms
    }
  }
}
