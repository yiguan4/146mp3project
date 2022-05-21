#include "lcd.h"
#include "delay.h"
#include "gpio.h"
#include "lpc40xx.h"
#include <ctype.h>

static gpio_s lcd_rs_pin;
static gpio_s lcd_rw_pin;
static gpio_s lcd_enable_pin;
static gpio_s lcdpin_d7, lcdpin_d6, lcdpin_d5, lcdpin_d4, lcdpin_d3, lcdpin_d2,
    lcdpin_d1, lcdpin_d0;

void enable_lcd(void) {
  gpio__set(lcd_enable_pin);
  delay__ms(1);
  gpio__reset(lcd_enable_pin);
  delay__ms(1);
}

void lcd_char(uint8_t char_byte) {

  rs_pin(0);
  rw_pin(0);
  set_d7(((1 << 7) & char_byte));
  set_d6(((1 << 6) & char_byte));
  set_d5(((1 << 5) & char_byte));
  set_d4(((1 << 4) & char_byte));
  set_d3(((1 << 3) & char_byte));
  set_d2(((1 << 2) & char_byte));
  set_d1(((1 << 1) & char_byte));
  set_d0(((1 << 0) & char_byte));

  enable_lcd();
}

void lcd_reset(void) {
  uint8_t x_reset = 0;
  uint8_t y_reset = 0;
  uint8_t clear_char = 0x01;
  lcd_char(clear_char);
  lcd_set_xy(x_reset, y_reset);
}

void lcd_print(uint8_t char_byte) {

  rs_pin(1);
  rw_pin(0);

  set_d7(((1 << 7) & char_byte));
  set_d6(((1 << 6) & char_byte));
  set_d5(((1 << 5) & char_byte));
  set_d4(((1 << 4) & char_byte));
  set_d3(((1 << 3) & char_byte));
  set_d2(((1 << 2) & char_byte));
  set_d1(((1 << 1) & char_byte));
  set_d0(((1 << 0) & char_byte));

  enable_lcd();
}

void lcd_print_line(const char *song_name, int line) {
  char arr[] = "                    ";
  lcd_set_xy(0, line);
  lcd_print_remove(arr);

  lcd_set_xy(0, line);
  lcd_print_remove(song_name);
}

void lcd_print_remove(const char *song_name) {
  for (int i = 0; i < 20; i++) {

    if (song_name[i] == '.' || song_name[i] == '\0')
      break;
    lcd_print(song_name[i]);
  }
}

void lcd_set_xy(uint8_t x, uint8_t y) {
  switch (y) {
  case 2:
    x += 0x40;
    break;
  default:
    x += 0x00;
  }

  lcd_char(0x80 | x);
}

void lcd_select(int line) {
  switch (line) {
  case 1:
    lcd_set_xy(12, 1);
    break;
  case 2:
    lcd_set_xy(12, 2);
    break;
  default:
    lcd_set_xy(12, 1);
    break;
  }

  lcd_print_remove("<--");
}

void rs_pin(bool high) {
  if (high)
    gpio__set(lcd_rs_pin);
  else
    gpio__reset(lcd_rs_pin);
}

void rw_pin(bool read_high) {
  if (read_high)
    gpio__set(lcd_rw_pin);
  else
    gpio__reset(lcd_rw_pin);
}

void set_d7(bool high) {
  if (high)
    gpio__set(lcdpin_d7);
  else
    gpio__reset(lcdpin_d7);
}

void set_d6(bool high) {
  if (high)
    gpio__set(lcdpin_d6);
  else
    gpio__reset(lcdpin_d6);
}

void set_d5(bool high) {
  if (high)
    gpio__set(lcdpin_d5);
  else
    gpio__reset(lcdpin_d5);
}

void set_d4(bool high) {
  if (high)
    gpio__set(lcdpin_d4);
  else
    gpio__reset(lcdpin_d4);
}

void set_d3(bool high) {
  if (high)
    gpio__set(lcdpin_d3);
  else
    gpio__reset(lcdpin_d3);
}

void set_d2(bool high) {
  if (high)
    gpio__set(lcdpin_d2);
  else
    gpio__reset(lcdpin_d2);
}

void set_d1(bool high) {
  if (high)
    gpio__set(lcdpin_d1);
  else
    gpio__reset(lcdpin_d1);
}

void set_d0(bool high) {
  if (high)
    gpio__set(lcdpin_d0);
  else
    gpio__reset(lcdpin_d0);
}

void lcd_init(void) {
  uint8_t config_eight_bit = 0x30;
  uint8_t two_line = 0x08;
  uint8_t lcd_font = 0x04;
  uint8_t display_on = 0x0C;
  uint8_t display_off = 0x08;
  uint8_t display_clear = 0x01;
  uint8_t enter = 0x6;
  lcd_rs_pin =
      gpio__construct_with_function(0, 8, GPIO__FUNCITON_0_IO_PIN);
  lcd_rw_pin =
      gpio__construct_with_function(0, 26, GPIO__FUNCITON_0_IO_PIN);
  lcd_enable_pin =
      gpio__construct_with_function(2, 6, GPIO__FUNCITON_0_IO_PIN);

  gpio__set_as_output(lcd_rw_pin);
  gpio__set_as_output(lcd_rs_pin);
  gpio__set_as_output(lcd_enable_pin);

  gpio__reset(lcd_rs_pin);
  gpio__reset(lcd_rw_pin);
  gpio__reset(lcd_enable_pin);

  lcdpin_d0 =
      gpio__construct_with_function(1, 23, GPIO__FUNCITON_0_IO_PIN);
  lcdpin_d1 =
      gpio__construct_with_function(1, 28, GPIO__FUNCITON_0_IO_PIN);
  lcdpin_d2 =
      gpio__construct_with_function(1, 29, GPIO__FUNCITON_0_IO_PIN);
  lcdpin_d3 =
      gpio__construct_with_function(2, 0, GPIO__FUNCITON_0_IO_PIN);
  lcdpin_d4 =
      gpio__construct_with_function(2, 1, GPIO__FUNCITON_0_IO_PIN);
  lcdpin_d5 =
      gpio__construct_with_function(2, 2, GPIO__FUNCITON_0_IO_PIN);
  lcdpin_d6 =
      gpio__construct_with_function(2, 4, GPIO__FUNCITON_0_IO_PIN);
  lcdpin_d7 =
      gpio__construct_with_function(2, 5, GPIO__FUNCITON_0_IO_PIN);

  gpio__set_as_output(lcdpin_d0);
  gpio__set_as_output(lcdpin_d1);
  gpio__set_as_output(lcdpin_d2);
  gpio__set_as_output(lcdpin_d3);
  gpio__set_as_output(lcdpin_d4);
  gpio__set_as_output(lcdpin_d5);
  gpio__set_as_output(lcdpin_d6);
  gpio__set_as_output(lcdpin_d7);

  delay__ms(50);

  lcd_char(config_eight_bit);
  lcd_char(config_eight_bit);
  lcd_char(config_eight_bit);

  lcd_char(config_eight_bit | two_line | lcd_font); // 0x3c

  lcd_char(display_off);
  lcd_char(display_clear);
  lcd_char(enter);
  lcd_char(display_on);
}
