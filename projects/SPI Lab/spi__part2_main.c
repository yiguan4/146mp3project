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

void adesto_cs(void);
void adesto_ds(void);
void spi_id_verification_task(void *task);
SemaphoreHandle_t ssp2__device_mutex;

typedef struct {
  uint8_t manufacturer_id;
  uint8_t device_id_1;
  uint8_t device_id_2;
  uint8_t extended_device_id;
} adesto_flash_id_s;

int main(void) {
  puts("Starting RTOS");
  
  /****************************SPI LAB PART 2****************************/
  ssp2__device_mutex = xSemaphoreCreateMutex();
  const uint32_t spi_clock_mhz = 24;
  ssp2__init(spi_clock_mhz);
  gpio__construct_with_function(1, 0, GPIO__FUNCTION_4); // SCK2
  gpio__construct_with_function(1, 1, GPIO__FUNCTION_4); // MOSI2
  gpio__construct_with_function(1, 4, GPIO__FUNCTION_4); // MISO2
  LPC_IOCON->P1_10 &= ~0b111;                            // CS
  const uint32_t ssp2__enable_gpio_output = (1 << 10);
  LPC_GPIO1->DIR |= ssp2__enable_gpio_output;
  
  xTaskCreate(spi_id_verification_task, "spi task1", 4096 / sizeof(void *), NULL, PRIORITY_LOW, NULL);
  xTaskCreate(spi_id_verification_task, "spi task2", 4096 / sizeof(void *), NULL, PRIORITY_HIGH, NULL);
  vTaskStartScheduler(); 
  return 0;
}
adesto_flash_id_s ssp2__adesto_read_signature(void) {
  adesto_flash_id_s data = {0};
  adesto_cs();
  {
    ssp2__lab_exchange_byte(0x9F);
    data.manufacturer_id = ssp2__lab_exchange_byte(0x00);
    data.device_id_1 = ssp2__lab_exchange_byte(0x00);
    data.device_id_2 = ssp2__lab_exchange_byte(0x00);
  }
  adesto_ds();
  return data;
}
/****************************SPI LAB PART 2****************************/
void spi_id_verification_task(void *p) {
  adesto_flash_id_s id = {0};
  while (1) {
    if (xSemaphoreTake(ssp2__device_mutex, portMAX_DELAY)) {
      // Use Guarded Resource
      id = ssp2__adesto_read_signature();
      fprintf(stderr, "Manufacture ID: %x\nDevice 1 ID: %x\nDevice 2 ID: %x\n\n", id.manufacturer_id, id.device_id_1, id.device_id_2);
      // Give Semaphore back:
      xSemaphoreGive(ssp2__device_mutex);
    }
    // When we read a manufacturer ID we do not expect, we will kill this task
    if (0x1F != id.manufacturer_id) {
      fprintf(stderr, "Manufacturer ID read failure\n");
      vTaskSuspend(NULL); // Kill this task
    }
    vTaskDelay(1000);
  }
}

void adesto_cs(void) {
  const uint32_t adesto__enable_cs = ~(1 << 10);
  LPC_GPIO1->PIN &= adesto__enable_cs;
}
void adesto_ds(void) {
  const uint32_t adesto__enable_ds = (1 << 10);
  LPC_GPIO1->PIN |= adesto__enable_ds;
}