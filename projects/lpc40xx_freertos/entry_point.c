#include <stdio.h>

#include "FreeRTOS.h"
#include "trcRecorder.h"

#include "clock.h"
#include "crash.h"
#include "peripherals_init.h"
#include "sensors.h"
#include "startup.h"
#include "sys_time.h"

extern void main(void);
static void entry_point__halt(void);
static void entry_point__handle_crash_report(void);
static void entry_point__rtos_trace_init_after_mounting_sd_card(void);

void entry_point(void) {
  /* startup__initialize_ram(); - startup.c
    This function calls two other functions -
    startup__init_data_sram() creates a copy of the initialized variables
      in flash and puts it into RAM
    startup__init_bss_sram() takes the uninitialized variables and sets
     up two pointers, start BSS and end BSS, which are initialized to 0
     when the CPU starts up.
  */
  startup__initialize_ram();
  /* startup__initialize_fpu(); - startup.c
    This function initializes the FPU, floating point unit, which performs
    mathmatical functions as well as other special functions.
  */
  startup__initialize_fpu();

  /* clock__initialize_system_clock_96mhz(); - clock.c
    This function initializes the system clock to be 96Mhz
    - The multiplier can be any value as long as it does not exceed 96Mhz,
      otherwise the data might not be processed correctly possibly causing
      an error.
 */
  clock__initialize_system_clock_96mhz();

  /* RTOS trace is an optional component configured by FreeRTOSConfig.h
   * We need to initialize the trace early before using ANY RTOS API
   *
   *  Notes:
   *  - We cannot use TRC_START here as the SD card is not initialized yet
   *  - This can be used even if the trace library is not enabled
   *  - Initialize trace here so that the trace library APIs are accessible
   * early
   */
  /* vTraceEnable(TRC_INIT);
    This initializes the trace recorder before it starts tracing.
  */
  vTraceEnable(TRC_INIT);

  /* startup__initialize_interrupts(); - startup.c
    This function initializes and enables the interrupts to ensure that an
    interrupt is called when needed.
  */
  startup__initialize_interrupts();
  /* sys_time__init(clock__get_peripheral_clock_hz()); - sys_time.c /
    FreeRTOSConfig.h This function sets up the system time cycle divisions and
    enables a timer considering an interrupt. It does this by taking the
    frequency for 1us to a lower frequency to better see the clock signals for
    the timer.
  */
  sys_time__init(clock__get_peripheral_clock_hz());

  // Peripherals init initializes UART and then we can print the crash report if
  // applicable
  /* peripherals_init(); - peripherals_init.c
    This function calls 6 other functions within it  -
    board_io__initialize() initializes all the pins on the board as either
    inputs or outputs peripherals_init__startup_sequence() initializes and
    presents the current outputs of leds 0 through 3 on
    peripherals_init__uart0_init() initializes the UART speed and sets up the
    receiver and transceiver queues and handlers
    ssp2__initialize(spi_sd_max_speed_khz) turns on the serial port with the
    given spi sd speed peripherals_init__i2c_init() initializes the I2C speed
    and the required binary semaphores and mutex. It also identifies where the
    slave (device the microcontroller is communicating with) is located This
    function also sets up the SPI SD speed and if the SD card is attached
  */
  peripherals_init();
  /* entry_point__handle_crash_report(); - crash.c
    This function prints out a crash report - if a crash occurs, it calls a
    seperate function that describes where the crash occurred by displaying the
    current program counter, link register, processor state register, and R0
    through R3.
  */
  entry_point__handle_crash_report();

  /* sensors__init() - sensors.c
    If the sensors' status is not true, then a warning statement is printed
    considering the current function.
   */
  if (!sensors__init()) {
    printf("\n%s(): WARNING: Sensor errors on this board\n", __FUNCTION__);
  }

  /* entry_point__rtos_trace_init_after_mounting_sd_card(); - entry_point.c
    This function enables the ability to trace the SD card immediately if it
    is being used, otherwise SD card is not traced but is set up for tracing
    without starting the actual trace.
   */
  entry_point__rtos_trace_init_after_mounting_sd_card();

  /* A statement informing us that we are now going to enter the main function
    and begin executing the main.
 */
  printf("\n%s(): Entering main()\n", __FUNCTION__);
  main();

  entry_point__halt();
}

static void entry_point__halt(void) {
  // main() should never return: CPU will now halt forever at this point.
  fprintf(stderr, "ERROR: main() should never return, program has been halted");
  while (1) {
  }
}

static void entry_point__handle_crash_report(void) {
  if (crash__report_if_occurred()) {
    while (sys_time__get_uptime_ms() < 5000) {
      ; // Deliberately delay the startup and let the user carefully read the
        // information
    }
  }
}

static void entry_point__rtos_trace_init_after_mounting_sd_card(void) {
#if !defined(configENABLE_TRACE_ON_SD_CARD) ||                                 \
    !defined(configUSE_TRACE_FACILITY)
#error                                                                         \
    "configENABLE_TRACE_ON_SD_CARD and configUSE_TRACE_FACILITY must be defined; try including FreeRTOSConfig.h"
// If instructed to trace on the SD card, then start the trace immediately
#elif (configENABLE_TRACE_ON_SD_CARD)
  vTraceEnable(TRC_START);
/* If we are not tracing to SD card, initialize the trace but do not start it.
 * However, since the vTraceEnable(TRC_INIT) occurs early, we are good here.
 */
#elif (configUSE_TRACE_FACILITY)
  // vTraceEnable(TRC_INIT);
#endif
}
