#include "FreeRTOS.h"
#include "decoder.h"
#include "delay.h"
#include "gpio.h"
#include "gpio0_lab.h"
#include "gpio1_lab.h"
#include "mp3.h"
#include "queue.h"
#include "sj2_cli.h"
#include "song_list.h"
#include "ssp2.h"
#include "ssp2_mutex.h"
#include "task.h"
#include <stdbool.h>
#include <stdio.h>

static QueueHandle_t button_press_queue;

gpio_s up = {1, 15};
gpio_s down = {1, 19};
gpio_s select = {1, 30};
gpio_s prev = {0, 29};
gpio_s next = {0, 30};
gpio_s play_and_pause = {0, 6};

TaskHandle_t mp3_player_handle;

static void mp3_reader_task(void *p);
static void mp3_player_task(void *p);

static void mp3_reader_task(void *p) {
  FIL file_handler;
  songname_t name;
  songdata_t buffer;
  UINT Bytes_Read;

  while (1) {
    xQueueReceive(mp3__songname_queue, &name.song_name, portMAX_DELAY);
    if (open_file(&file_handler, &name.song_name)) {
      read_from_file(&file_handler, buffer.song_data, &Bytes_Read);
      close_file(&file_handler);
    }
  }
}

void mp3_player_task(void *p) {
  char bytes_512[512];
  while (1) {
    xQueueReceive(mp3_songdata_queue, &bytes_512[0], portMAX_DELAY);
    // vTaskDelay(2);
    for (int i = 0; i < sizeof(bytes_512);) {
      while (!mp3_decoder_needs_data()) {
        vTaskDelay(1);
      }
      // spi_send_to_mp3_decoder(bytes_512[i]);
      spi2_mutex__acquire();
      decoder_clear_xdcs();

      ssp2__exchange_byte(bytes_512[i]);
      i++;
      decoder_set_xdcs();
      spi2_mutex__release();
    }
  }
}

void button_check_task(void) {

  while (1) {
    if (gpio__get(select)) {
      int action = 0;
      xQueueSend(button_press_queue, &action, 0);
    }
    if (gpio__get(up)) {
      int action = 1;
      xQueueSend(button_press_queue, &action, 0);
    }
    if (gpio__get(down)) {
      int action = 2;
      xQueueSend(button_press_queue, &action, 0);
    }
    if (gpio__get(prev)) {
      int action = 3;
      pause = true;
      xQueueSend(button_press_queue, &action, 0);
    }
    if (gpio__get(next)) {
      int action = 4;
      pause = true;
      xQueueSend(button_press_queue, &action, 0);
    }
    if (gpio__get(play_and_pause)) {
      int action = 5;
      pause = !pause;
      xQueueSend(button_press_queue, &action, 0);
    }
    vTaskDelay(1);
  }
}

void button_task(void *p) {
  int action;
  while (1) {
    if (xQueueReceive(button_press_queue, &action, portMAX_DELAY)) {
      if (action == 0) {
        pause = false;
        playing = true;
        lcd_reset();
        xQueueSend(mp3__songname_queue, songlist_name(song_index),
                   portMAX_DELAY);
        display_current_song();
      }
      if (action == 1) {
        playing = false;
        lcd_reset();
        songindex_decrease();
        display_select_song();
      }
      if (action == 2) {
        playing = false;
        lcd_reset();
        songindex_increase();
        display_select_song();
      }
      if (action == 3) {
        playing = true;
        if (pause) {
          vTaskSuspend(mp3_player_handle);
          lcd_reset();
          songindex_decrease();
          xQueueSend(mp3__songname_queue, songlist_name(song_index),
                     portMAX_DELAY);
          vTaskResume(mp3_player_handle);
          display_current_song();
        }
      }
      if (action == 4) {
        playing = true;
        if (pause) {
          vTaskSuspend(mp3_player_handle);
          lcd_reset();
          songindex_increase();
          xQueueSend(mp3__songname_queue, songlist_name(song_index),
                     portMAX_DELAY);
          vTaskResume(mp3_player_handle);
          display_current_song();
        }
      }
      if (action == 5) {
        if (pause) {
          lcd_print_line("Paused", 1);
          vTaskSuspend(mp3_player_handle);
          pause = !pause;
          vTaskDelay(1);
        }
        if (!pause) {
          lcd_print_line("Playing", 1);
          vTaskResume(mp3_player_handle);
          vTaskDelay(1);
        }
      }
    }
  }
}

int main(void) {
  sj2_cli__init();
  mp3__init();

  gpio__enable_pull_down_resistors(prev);
  gpio__enable_pull_down_resistors(next);

  button_press_queue = xQueueCreate(1, sizeof(int));

  xTaskCreate(mp3_reader_task, "Mp3_Reader", 4096 / sizeof(void), NULL,
              PRIORITY_LOW, NULL);
  xTaskCreate(mp3_player_task, "Mp3_Player", 4096 / sizeof(void), NULL,
              PRIORITY_HIGH, &mp3_player_handle);
  xTaskCreate(button_task, "button_task", 4096 / sizeof(void), NULL,
              PRIORITY_HIGH, NULL);
  xTaskCreate(button_check_task, "button_check_task", 4096 / sizeof(void), NULL,
              PRIORITY_HIGH, NULL);

  vTaskStartScheduler();

  return 0;
}
