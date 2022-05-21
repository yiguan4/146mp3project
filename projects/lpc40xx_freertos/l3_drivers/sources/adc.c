#include <stdint.h>

#include "adc.h"

#include "clock.h"
#include "lpc40xx.h"
#include "lpc_peripherals.h"

/*******************************************************************************
 *
 *                      P U B L I C    F U N C T I O N S
 *
 ******************************************************************************/

void adc__initialize(void) {
  lpc_peripheral__turn_on_power_to(LPC_PERIPHERAL__ADC);

  const uint32_t enable_adc_mask = (1 << 21);
  LPC_ADC->CR = enable_adc_mask;

  const uint32_t max_adc_clock =
      (12 * 1000UL *
       1000UL); // 12.4Mhz : max ADC clock in datasheet for lpc40xx
  const uint32_t adc_clock = clock__get_peripheral_clock_hz();

  // APB clock divicer to support max ADC clock
  for (uint32_t divider = 2; divider < 255; divider += 2) {
    if ((adc_clock / divider) < max_adc_clock) {
      LPC_ADC->CR |= (divider << 8);
      break;
    }
  }
}

uint16_t adc__get_adc_value(adc_channel_e channel_num) {
  uint16_t result = 0;
  const uint16_t twelve_bits = 0x0FFF;
  const uint32_t channel_masks = 0xFF;
  const uint32_t start_conversion = (1 << 24);
  const uint32_t start_conversion_mask = (7 << 24); // 3bits - B26:B25:B24
  const uint32_t adc_conversion_complete = (1 << 31);

  if ((ADC__CHANNEL_2 == channel_num) || (ADC__CHANNEL_4 == channel_num) ||
      (ADC__CHANNEL_5 == channel_num)) {
    LPC_ADC->CR &= ~(channel_masks | start_conversion_mask);
    // Set the channel number and start the conversion now
    LPC_ADC->CR |= (1 << channel_num) | start_conversion;

    while (!(LPC_ADC->GDR &
             adc_conversion_complete)) { // Wait till conversion is complete
      ;
    }
    result = (LPC_ADC->GDR >> 4) & twelve_bits; // 12bits - B15:B4
  }

  return result;
}

void adc__enable_burst_mode(void) {
  LPC_ADC->CR |= (1 << 5);   // selects AD0[5] sets bit 5 to 1
  LPC_ADC->CR |= (1 << 16);  // enables burst mode table 678 sets bit 16 to 1
  LPC_ADC->CR &= ~(7 << 24); // enables START of the burst mode by clearing the
                             // pins 0b111 table 678
}

uint16_t adc__get_channel_reading_with_burst_mode(uint8_t channel_number) {
  // LPC_ADC->CR |= (1 << 5); // selects AD0[5]
  // shiift right by 4 because in DR result starts on bit 4
  // 0xFFF is to check if the voltage being read is >= the reference voltage
  // of 3.3V
  uint16_t result;
  result = ((LPC_ADC->DR[channel_number] >> 4)) & 0XFFF;
  return result;
}
