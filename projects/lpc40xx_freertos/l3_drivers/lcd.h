#pragma once

#include "stdbool.h"
#include <stdio.h>

void lcd_init(void);

void lcd_char(uint8_t command);
void lcd_reset(void);
void lcd_print(uint8_t character);
void lcd_print_line(const char *song_name, int line);
void lcd_print_remove(const char *song_name);
void lcd_set_xy(uint8_t x, uint8_t y);
void lcd_select(int line);

/* Pin Config */

void rs_pin(bool high);
void rw_pin(bool read_high);
void set_d7(bool high);
void set_d6(bool high);
void set_d5(bool high);
void set_d4(bool high);
void set_d3(bool high);
void set_d2(bool high);
void set_d1(bool high);
void set_d0(bool high);
