

Page
1
of 3
#include "FreeRTOS.h"
#include "gpio.h"
#include "lpc40xx.h"
#include "semphr.h"
#include "ssp2_lab.h"
#include "task.h"
#include "board_io.h"
#include "common_macros.h"
#include "lpc_peripherals.h"
#include "periodic_scheduler.h"
#include "sj2_cli.h"
#include <stdio.h>

void spi_task(void *p);

typedef struct {
  uint8_t manufacturer_id;
  uint8_t device_id_1;
  uint8_t device_id_2;
  uint8_t extended_device_id;
} adesto_flash_id_s;

int main(void) {
  puts("Starting RTOS");
  /****************************SPI LAB PART 1****************************/
  xTaskCreate(spi_task, "spi task", 4096 / sizeof(void *), NULL, PRIORITY_LOW, NULL);
  vTaskStartScheduler();
  return 0;
}
/****************************SPI LAB PART 1****************************/
void spi_task(void *p) {
  const uint32_t spi_clock_mhz = 24;
  ssp2__init(spi_clock_mhz);
  gpio__construct_with_function(1, 0, GPIO__FUNCTION_4); // SCK2
  gpio__construct_with_function(1, 1, GPIO__FUNCTION_4); // MOSI2
  gpio__construct_with_function(1, 4, GPIO__FUNCTION_4); // MISO2
  LPC_IOCON->P1_10 &= ~0b111;                            // CS
  const uint32_t ssp2__enable_gpio_output = (1 << 10);
  LPC_GPIO1->DIR |= ssp2__enable_gpio_output;
  while (1) {
    adesto_flash_id_s id = ssp2__adesto_read_signature();
    printf("Manufacture ID: %x\nDevice 1 ID: %x\nDevice 2 ID: %x\n\n", id.manufacturer_id, id.device_id_1, id.device_id_2);
    vTaskDelay(500);
  }
}