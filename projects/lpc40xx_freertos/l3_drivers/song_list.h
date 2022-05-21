#pragma once

#include <stddef.h> // size_t

typedef char song_memory_t[128];

void song_list__populate(void);
size_t songlist_count(void);
const char *songlist_name(size_t item_number);
void check_sd_error(void);