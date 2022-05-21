#include "FreeRTOS.h"
#include "acceleration.h"
#include "ff.h"
#include "gpio.h"
#include "lpc40xx.h"
#include "queue.h"
#include "sl_string.h"
#include "task.h"
#include "board_io.h"
#include "event_groups.h"
#include "lpc_peripherals.h"
#include "periodic_scheduler.h"
#include "sj2_cli.h"
#include <stdio.h>
#include <string.h>

static QueueHandle_t sensor_queue;
static EventGroupHandle_t watchdog_event;
static TickType_t time;
static TickType_t counter, task_counter;

uint8_t producer_eventbits = 0x01;
uint8_t consumer_eventbits = 0x02;
uint8_t waitbits = 0x03;

typedef struct {
  acceleration__axis_data_s sample_data[100];
  float avg;
  float sum;
} data;

static void producer_task(void *P) {
  acceleration__init();
  data data;
  while (1) {
    printf("\nSending data");
    for (int i = 0; i < 100; i++) {
      data.sample_data[i] = acceleration__get_data();
      data.sum = data.sum + data.sample_data[i].x + data.sample_data[i].y + data.sample_data[i].z;
    }
    data.avg = data.sum / 100;
    xQueueSend(sensor_queue, &data.avg, 0);
    printf("Data successfully sent to queue!\n");
    data.sum = 0.000;
    xEventGroupSetBits(watchdog_event, producer_eventbits);
    vTaskDelay(1000);
  }
}

int get_avg_from_samples(void) {
  float sum = 0;
  float average = 0;
  for (int i = 0; i <= 100; i++) {
    acceleration__axis_data_s data = acceleration__get_data();
    sum += sqrt((data.x * data.x) + (data.y * data.y) + (data.z * data.z));
  }
  average = sum / 100;
  return average;
}
static void consumer_task(void *P) {
  static char string[64];
  const char *filename = "sensor.txt";
  FIL file;
  UINT bytes_written = 0;
  FRESULT result = f_open(&file, filename, (FA_WRITE | FA_CREATE_ALWAYS));
  float avg_value;
  while (1) {
    printf("\nWaiting to receive data!\n");
    if (xQueueReceive(sensor_queue, &avg_value, portMAX_DELAY)) {
      if (FR_OK == result) {
        time = xTaskGetTickCount();
        // sprintf("%li, %d\n", time, acceleration__get_data());
        sprintf(string, "Time: %i mS, Sample Data: %.2f\n", time, avg_value);
        if (FR_OK == f_write(&file, string, strlen(string), &bytes_written)) {
          f_sync(&file);
        } else {
          printf("ERROR: Failed to write data to file\n");
        }
        // f_close(&file);
      } else {
        printf("ERROR: Failed to open: %s\n", filename);
      }
    }
    xEventGroupSetBits(watchdog_event, consumer_eventbits);
    printf("Data is received at time = %imS, Sample Data = %.2f\n", time, avg_value);
  }
}
void Watchdog_task(void *P) {
  const char *filename = "watchdog.txt";
  FIL file;
  UINT bytes_written = 0;
  FRESULT result = f_open(&file, filename, (FA_WRITE | FA_CREATE_ALWAYS));
  while (1) {
    uint8_t uxBits = xEventGroupWaitBits(watchdog_event, waitbits, pdTRUE, pdTRUE, 2000);
    if ((uxBits == waitbits)) {
      if (FR_OK == result) {
        static char string[64];
        counter = xTaskGetTickCount();
        sprintf(string, "Verified at time: %ld mS\n", counter);
        if (FR_OK == f_write(&file, string, strlen(string), &bytes_written)) {
          f_sync(&file);
        }
      }
    } else if (uxBits == 0x01) {
      printf("Consumer task suspend\n");
      if (FR_OK == result) {
        static char string[64];
        task_counter = xTaskGetTickCount();  average = sum / 100;
  return average;
}
static void consumer_task(void *P) {
  static char string[64];
  const char *filename = "sensor.txt";
  FIL file;
  UINT bytes_written = 0;
  FRESULT result = f_open(&file, filename, (FA_WRITE | FA_CREATE_ALWAYS));
  float avg_value;
  while (1) {
    printf("\nWaiting to receive data!\n");
    if (xQueueReceive(sensor_queue, &avg_value, portMAX_DELAY)) {
      if (FR_OK == result) {
        time = xTaskGetTickCount();
        // sprintf("%li, %d\n", time, acceleration__get_data());
        sprintf(string, "Time: %i mS, Sample Data: %.2f\n", time, avg_value);
        if (FR_OK == f_write(&file, string, strlen(string), &bytes_written)) {
          f_sync(&file);
        } else {
          printf("ERROR: Failed to write data to file\n");
        }
        // f_close(&file);
      } else {
        printf("ERROR: Failed to open: %s\n", filename);
      }
    }
    xEventGroupSetBits(watchdog_event, consumer_eventbits);
    printf("Data is received at time = %imS, Sample Data = %.2f\n", time, avg_value);
  }
}
void Watchdog_task(void *P) {
  const char *filename = "watchdog.txt";
  FIL file;
  UINT bytes_written = 0;
  FRESULT result = f_open(&file, filename, (FA_WRITE | FA_CREATE_ALWAYS));
  while (1) {
    uint8_t uxBits = xEventGroupWaitBits(watchdog_event, waitbits, pdTRUE, pdTRUE, 2000);
    if ((uxBits == waitbits)) {
      if (FR_OK == result) {
        static char string[64];
        counter = xTaskGetTickCount();
        sprintf(string, "Verified at time: %ld mS\n", counter);
        if (FR_OK == f_write(&file, string, strlen(string), &bytes_written)) {
          f_sync(&file);
        }
      }
    } else if (uxBits == 0x01) {
      printf("Consumer task suspend\n");
      if (FR_OK == result) {
        static char string[64];
        task_counter = xTaskGetTickCount();
        sprintf(string, "Consumer task error at time: %ld mS\n ", task_counter);
        if (FR_OK == f_write(&file, string, strlen(string), &bytes_written)) {
          f_sync(&file);
        }
      }
    } else {
      printf("Both Tasks Failed\n");
      if (FR_OK == result) {
        static char string[64];
        task_counter = xTaskGetTickCount();
        sprintf(string, "Producer task error at time: %ld mS\n ", task_counter);
        if (FR_OK == f_write(&file, string, strlen(string), &bytes_written)) {
          f_sync(&file);
        } else {
          printf("ERROR: Failed to write data to file\n");
        }
      } else {
        printf("ERROR: Failed to open: %s\n", filename);
      }
    }
    vTaskDelay(1000);
  }
}
int main(void) {
  puts("Starting RTOS\n");
  sensor_queue = xQueueCreate(100, sizeof(float));
  watchdog_event = xEventGroupCreate();
  xTaskCreate(producer_task, "producer", 2048 / sizeof(void *), NULL, PRIORITY_MEDIUM, NULL);
  xTaskCreate(consumer_task, "consumer", 2048 / sizeof(void *), NULL, PRIORITY_MEDIUM, NULL);
  xTaskCreate(Watchdog_task, "watchdog", 2048 / sizeof(void *), NULL, PRIORITY_HIGH, NULL);
  vTaskStartScheduler();
  return 0;
}