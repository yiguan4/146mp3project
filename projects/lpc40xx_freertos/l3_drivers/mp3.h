#pragma once
#include <stdbool.h>
#include <stdio.h>

#include "FreeRTOS.h"
#include "ff.h"
#include "gpio.h"
#include "queue.h"
#include "semphr.h"

#include "lcd.h"
#include "song_attributes.h"
#include "song_list.h"

typedef void (*function_pointer_t)(void);

extern QueueHandle_t mp3__songname_queue;
extern QueueHandle_t mp3_songdata_queue;

extern volatile bool pause;
extern volatile bool playing;
extern volatile size_t song_index;

void mp3__init(void);

bool open_file(FIL *file_handler, char *song_name);
void close_file(FIL *file_handler);
void read_from_file(FIL *file_handler, char *data, UINT *Bytes_Read);

void songindex_increase(void);
void songindex_decrease(void);
void display_select_song(void);
void display_current_song(void);
