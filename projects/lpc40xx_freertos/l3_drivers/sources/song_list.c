#include "song_list.h"
#include "ff.h"
#include "lcd.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static song_memory_t list_of_songs[32];
static size_t number_of_songs;

static void song_list__handle_filename(const char *filename) {
  // This will not work for cases like "file.mp3.zip"
  if (NULL != strstr(filename, ".mp3")) {
    // printf("Filename: %s\n", filename);

    // Dangerous function: If filename is > 128 chars, then it will copy extra
    // bytes leading to memory corruption strcpy(list_of_songs[number_of_songs],
    // filename);

    // Better: But strncpy() does not guarantee to copy null char if max length
    // encountered So we can manually subtract 1 to reserve as NULL char
    // strncpy(list_of_songs[number_of_songs], filename, sizeof(song_memory_t) -
    // 1);

    // Best: Compensates for the null, so if 128 char filename, then it copies
    // 127 chars, AND the NULL char
    snprintf(list_of_songs[number_of_songs], sizeof(song_memory_t), "%.149s",
             filename);

    ++number_of_songs;
    // or
    // number_of_songs++;
  }
}

void song_list__populate(void) {
  FRESULT res;
  static FILINFO file_info;
  const char *root_path = "/";

  DIR dir;
  res = f_opendir(&dir, root_path);

  if (res == FR_OK) {
    for (;;) {
      res = f_readdir(&dir, &file_info); /* Read a directory item */
      if (res != FR_OK || file_info.fname[0] == 0) {
        break; /* Break on error or end of dir */
      }

      if (file_info.fattrib & AM_DIR) {
        /* Skip nested directories, only focus on MP3 songs at the root */
      } else { /* It is a file. */
        song_list__handle_filename(file_info.fname);
      }
    }
    f_closedir(&dir);
  }

  else {
    check_sd_error();
    exit(0);
  }
}

size_t songlist_count(void) { return number_of_songs; }

const char *songlist_name(size_t item_number) {
  const char *return_pointer = "";

  if (item_number >= number_of_songs) {
    return_pointer = "";
  } else {
    return_pointer = list_of_songs[item_number];
  }

  return return_pointer;
}

void check_sd_error(void) {
  char line1[] = "=== Error";
  char line2[] = "No SD Mounted.";

  lcd_print_line(line1, 1);
  lcd_print_line(line2, 2);
}