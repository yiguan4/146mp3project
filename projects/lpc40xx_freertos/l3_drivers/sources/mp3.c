#include "mp3.h"
#include "decoder.h"
#include "delay.h"
#include "lpc40xx.h"
#include "stdlib.h"

QueueHandle_t mp3__songname_queue;
QueueHandle_t mp3_songdata_queue;

volatile bool pause;

volatile bool playing;
volatile size_t song_index;

void mp3__init(void) {
  decoder__init();
  lcd_init();

  mp3__songname_queue = xQueueCreate(1, sizeof(songname_t));
  mp3_songdata_queue = xQueueCreate(1, sizeof(songdata_t));

  pause = false;
  playing = false;
  song_index = songlist_count();
  song_list__populate();
  display_select_song();
}

bool open_file(FIL *file_handler, char *song_name) {
  if (f_open(file_handler, song_name, FA_READ) == FR_OK)
    return true;

  return false;
}

void close_file(FIL *file_handler) { f_close(file_handler); }

void read_from_file(FIL *file_handler, char *buffer, UINT *Bytes_Read) {

  while (1) {
    f_read(file_handler, buffer, sizeof(songdata_t), Bytes_Read);

    if (*Bytes_Read == 0 || (uxQueueMessagesWaiting(mp3__songname_queue) == 1))
      break;

    xQueueSend(mp3_songdata_queue, buffer, portMAX_DELAY);
  }
}

void songindex_decrease(void) {
  if (song_index == 0) {
    song_index = songlist_count();
  }
  song_index--;
}

void songindex_increase(void) {
  song_index++;
  if (song_index >= songlist_count()) {
    song_index = 0;
  }
}

void display_select_song(void) {
  lcd_print_line("=== Menu", 1);
  lcd_print_line(songlist_name(song_index), 2);
  lcd_select(2);
}

void display_current_song(void) {
  lcd_reset();
  lcd_print_line("Current playing: ", 1);
  lcd_print_line(songlist_name(song_index), 2);
}
