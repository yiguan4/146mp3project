#include "decoder.h"
#include "FreeRTOS.h"
#include "delay.h"
#include "ff.h"
#include "gpio.h"
#include "lpc40xx.h"
#include "queue.h"
#include "semphr.h"
#include "ssp2.h"
#include <stdbool.h>
#include <stdio.h>

void pin_setup(void) {
  sck = gpio__construct_with_function(1, 0, 4);
  miso = gpio__construct_with_function(1, 4, 4);
  mosi = gpio__construct_with_function(1, 1, 4);

  gpio__set_as_output(sck);
  gpio__set_as_output(mosi);
  gpio__set_as_input(miso);

  gpio__construct_with_function(0, 22, GPIO__FUNCITON_0_IO_PIN); // xcs
  gpio__construct_with_function(0, 18, GPIO__FUNCITON_0_IO_PIN); // xdcs
  gpio__construct_with_function(
      0, 17, GPIO__FUNCITON_0_IO_PIN); // reset (pin 3 in decoder)

  DREQ = gpio__construct_with_function(0, 11, GPIO__FUNCITON_0_IO_PIN); // dreq
  gpio__set_as_input(DREQ);

  LPC_GPIO0->DIR |= (1 << 22); // setting xcs as output
  LPC_GPIO0->DIR |= (1 << 18); // setting xdcs as output
  LPC_GPIO0->DIR |= (1 << 17); // setting reset as output
}

void mp3_pin_initialization(void) {
  decoder_set_reset();
  decoder_set_xcs();
  decoder_set_xdcs();
  pin_setup();
}

void decoder__init() {
  mp3_pin_initialization();
  reset();
  decoder_write(SCI_VOL, 0x40, 0x40);
  const uint16_t spi_clock_khz = 10;
  ssp2__initialize(spi_clock_khz);
  decoder_write(SCI_MODE, 0x48, 0x00);
  decoder_write(SCI_CLOCKF, 0x60, 0x00);
  decoder_write(SCI_AUDATA, 0xAC, 0x45);
}

void decoder_set_reset(void) { LPC_GPIO0->SET = (1 << 17); }
void decoder_clear_reset(void) { LPC_GPIO0->CLR = (1 << 17); }
void decoder_set_xcs(void) { LPC_GPIO0->SET = (1 << 22); }
void decoder_clear_xcs(void) { LPC_GPIO0->CLR = (1 << 22); }
void decoder_set_xdcs(void) { LPC_GPIO0->SET = (1 << 18); }
void decoder_clear_xdcs(void) { LPC_GPIO0->CLR = (1 << 18); }

void reset(void) {
  decoder_clear_reset(); // Reset Pin Is Active Low
  delay__ms(200);
  decoder_set_reset();
}
// decoder data request dreq
bool mp3_decoder_needs_data(void) {
  return gpio__get(DREQ);
} // double check this line

void spi_send_to_mp3_decoder(uint8_t byte) {
  decoder_clear_xdcs();
  { ssp2__exchange_byte(byte); }
  decoder_set_xdcs();
}

// write data to decoder
void decoder_write(uint8_t address, uint8_t high_byte, uint8_t low_byte) {
  while (!mp3_decoder_needs_data()) {
  }
  decoder_clear_xcs();
  ssp2__exchange_byte(WRITE_OPCODE);
  ssp2__exchange_byte(address);
  ssp2__exchange_byte(high_byte);
  ssp2__exchange_byte(low_byte);

  while (!mp3_decoder_needs_data()) {
    ;
  }
  decoder_set_xcs();
}

uint16_t decoder_read(uint8_t address) {
  uint8_t data = 0;
  while (!mp3_decoder_needs_data()) {
  }
  decoder_clear_xcs();

  // Send opcode and read bytes
  ssp2__exchange_byte(READ_OPCODE);
  ssp2__exchange_byte(address);
  data |= (ssp2__exchange_byte(0xFF) << 8);
  data |= (ssp2__exchange_byte(0xFF) << 0);

  decoder_set_xcs();
  while (!mp3_decoder_needs_data()) {
  }
  return data;
}
