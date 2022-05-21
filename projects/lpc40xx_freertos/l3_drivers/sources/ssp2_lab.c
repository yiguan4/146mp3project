#include "ssp2_lab.h"
#include "lpc40xx.h"

/*
    Pins for SSP from schematic
    P1_10 CS (Chip Select)
    P1_4 MISO2 (Master In Slave Out <--)
    P1_0 SCK2 (CLK)
    P1_1 MOSI2 (Master Out Slave In -->)

*/

void ssp2__init(uint32_t max_clock_mhz) {
  const uint32_t ssp2__power_up = (1 << 20);
  LPC_SC->PCONP |= ssp2__power_up;

  const uint32_t ssp2__scr_value = 0;
  const uint32_t ssp2__cr0_setup = (0b111 << 0) | (ssp2__scr_value << 8);
  LPC_SSP2->CR0 |= ssp2__cr0_setup; // see section 21.5.3 in user manual and
                                    // table 483 and adesto fig 5-1

  const uint32_t ssp2__cr1_enable = (1 << 1);
  LPC_SSP2->CR1 |= ssp2__cr1_enable;

  LPC_SSP2->CPSR = 4; // 4 * 12MHz = 96MHz
}

uint8_t ssp2__lab_exchange_byte(uint8_t data_out) {
  LPC_SSP2->DR = data_out;

  // check if data is being transferred
  const uint32_t ssp2__enable_bsy_bit = (1 << 4);
  while (LPC_SSP2->SR & ssp2__enable_bsy_bit) {
    ;
  }

  return LPC_SSP2->DR;
}