#pragma once

#include "ff.h"
#include "gpio.h"
#include <stdint.h>

gpio_s DREQ;
// gpio_s UP_BUTTON;
// gpio_s DOWN_BUTTON;
gpio_s sck;
gpio_s miso;
gpio_s mosi;

typedef enum { READ_OPCODE = 0x3, WRITE_OPCODE = 0x2 } OPCODE;

typedef enum {
  SCI_MODE = 0x0,
  SCI_STATUS = 0x1,
  SCI_BASS = 0x2,
  SCI_CLOCKF = 0x3,
  SCI_DECODE_TIME = 0x4,
  SCI_AUDATA = 0x5,
  SCI_WRAM = 0x6,
  SCI_WRAMADDR = 0x7,
  SCI_HDAT0 = 0x8,
  SCI_HDAT1 = 0x9,
  SCI_AIADDR = 0xA,
  SCI_VOL = 0xB,
  SCI_AICTRL0 = 0xC,
  SCI_AICTRL1 = 0xD,
  SCI_AICTRL2 = 0xE,
  SCI_AICTRL3 = 0xF
} SCI_REGISTERS;

void decoder__init(void);
void pin_setup(void);
void reset(void);
void pin_initialization(void);

void spi_send_to_mp3_decoder(uint8_t byte);
bool mp3_decoder_needs_data(void);

void decoder_set_reset(void);
void decoder_clear_reset(void);
void decoder_clear_xcs(void);
void decoder_set_xcs(void);
void decoder_clear_xdcs(void);
void decoder_set_xdcs(void);

uint16_t decoder_read(uint8_t address); // read data from decoder
void decoder_write(uint8_t address, uint8_t high_byte,
                   uint8_t low_byte); // write data to decoder

bool decoder_raise_volume(void);
bool decoder_lower_volume(void);

void decoder_raise_bass(void);
void decoder_lower_bass(void);
void decoder_raise_treble(void);
void decoder_lower_treble(void);