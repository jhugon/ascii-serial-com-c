/** \file
 *
 * \brief Test that ASC can communicate with GPIOs
 *
 * Register map is documented at \ref register_map
 *
 */

#include <stdbool.h>

#include <neorv32.h>

#include "asc_exception.h"
#include "ascii_serial_com.h"
#include "ascii_serial_com_device.h"
#include "ascii_serial_com_register_pointers.h"
#include "circular_buffer.h"
#include "millisec_timer.h"

#ifndef NEORV32
#error ERROR: NEORV32 preprocessor macro is not defined!
#endif

// Lower 16 bits of GPIO.OUTPUT_LO are writable to the leds
// GPIO.INPUT_HI is readable for the switches (bits 15 down to 1)

#define BAUD_RATE 19200
#define UART1_TX_BUFFER_FULL                                                   \
  ((NEORV32_UART1.CTRL & (1 << UART_CTRL_TX_FULL)) != 0)

CEXCEPTION_T e;
uint16_t nExceptions;

#define nRegs 6

/** \brief Register Map
 *
 * ## Register Map
 *
 * |Register Number | Description | r/w |
 * | -------------- |------------ | --- |
 * | 0 | GPIO output (lower 32 bits) | r/w |
 * | 1 | GPIO output (upper 32 bits) | r/w |
 * | 2 | GPIO input (lower 32 bits) | r |
 * | 3 | GPIO input (upper 32 bits) | r |
 * | 4 | Pulse counter count | r |
 * | 5 | Pulse counter CSR* | r/w |
 *
 * Pulse counter CSR:
 *
 * Bit 0: enable
 * Bit 1: reset
 *
 * @see register_write_masks
 *
 */
volatile REGTYPE *register_map[nRegs] = {
    &NEORV32_GPIO.OUTPUT_LO,
    &NEORV32_GPIO.OUTPUT_HI,
    &NEORV32_GPIO.INPUT_LO,
    &NEORV32_GPIO.INPUT_HI,
    0x71000000,
    0x71000004,
};

/** \brief Write masks for \ref register_map
 *
 * These define whether the given register in register_map is writable or not
 *
 */
REGTYPE register_write_masks[nRegs] = {
    0xFFFFFFFF, 0xFFFFFFFF, 0, 0, 0, 0x3,
};

DECLARE_ASC_DEVICE_W_REGISTER_POINTERS()

uint8_t tmp_byte;
int rx_status;

int main() {

  neorv32_uart0_setup(BAUD_RATE, PARITY_NONE, FLOW_CONTROL_NONE);

  if (neorv32_uart1_available() == 0) {
    neorv32_uart0_print("Error! UART1 not synthesized!\n");
    return 1;
  }

  if (neorv32_gpio_available() == 0) {
    neorv32_uart0_print("Error! GPIO not synthesized!\n");
    return 1;
  }

  // capture all exceptions and give debug info via UART
  // this is not required, but keeps us safe
  neorv32_rte_setup();

  neorv32_uart0_print("Starting ASCII-Serial-Com WB Pulse Counter Program\n");

  Try {
    neorv32_uart1_setup(9600, PARITY_NONE, FLOW_CONTROL_NONE);

    SETUP_ASC_DEVICE_W_REGISTER_POINTERS(register_map, register_write_masks,
                                         nRegs);
  }
  Catch(e) { return e; }

  while (1) {
    Try {
      HANDLE_ASC_COMM_IN_POLLING_LOOP(ASC_USART);
      // Receive bytes into extraInputBuffer
      rx_status = neorv32_uart1_getc_safe((char *)&tmp_byte);
      if (rx_status == 0) { // successfully rx a char
        circular_buffer_push_back_uint8(&extraInputBuffer, tmp_byte);
      }
    }
    Catch(e) {
      nExceptions++;
      neorv32_uart0_printf("Found: %d exceptions\n", nExceptions);
    }
  }

  return 0;
}
