#ifndef ASC_HELPERS_H
#define ASC_HELPERS_H

/** \file
 *
 * Helper macros to help with UARTs, etc.
 *
 * */

#include <inttypes.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "ascii_serial_com.h"
#include "circular_buffer.h"

uint8_t ASC_HELPERS_BYTE;

/**
 *
 * Helper to check if UART has a received a byte and is waiting on the user to
 * read it
 *
 * Use uart_no as 1,2,3, etc.
 *
 */
#define is_uart_rx_data_waiting(uart_no) _is_uart_rx_data_waiting(uart_no)

/**
 * Second level of macro makes sure parameters are expanded before subbing into
 * string concatenation
 */
#if defined(__ARM_ARCH)
#define _is_uart_rx_data_waiting(uart_no)                                      \
  ((USART_ISR(USART##uart_no) & USART_ISR_RXNE))
#elif defined(__AVR)
#define _is_uart_rx_data_waiting(uart_no) (UCSR##uart_no##A & (1 << RXC0))
#elif defined(NEORV32)
#define _is_uart_rx_data_waiting(uart_no)                                      \
  (neorv32_uart##uart_no##_char_received())
#else
#define _is_uart_rx_data_waiting(uart_no) __builtin_unreachable()
#endif

/**
 *
 * Helper to check if UART tx buffer is ready for the user to write to it
 *
 * Use uart_no as 1,2,3, etc.
 *
 */
#define is_uart_ready_to_tx(uart_no) _is_uart_ready_to_tx(uart_no)

/**
 * Second level of macro makes sure parameters are expanded before subbing into
 * string concatenation
 */
#if defined(__ARM_ARCH)
#define _is_uart_ready_to_tx(uart_no)                                          \
  ((USART_ISR(USART##uart_no) & USART_ISR_TXE))
#elif defined(__AVR)
#define _is_uart_ready_to_tx(uart_no) (UCSR##uart_no##A & (1 << UDRE0))
#elif defined(NEORV32)
#define _is_uart_ready_to_tx(uart_no)                                          \
  ((NEORV32_UART##uart_no##.CTRL & (1 << UART_CTRL_TX_FULL)) == 0)
#else
#define _is_uart_ready_to_tx(uart_no) __builtin_unreachable()
#endif

/**
 *
 * Reads from the UART rx buffer, non-blocking, without checking if data is
 * there
 *
 */
#define uart_rx(uart_no, _tmp_byte) _uart_rx(uart_no, _tmp_byte)

/**
 * Second level of macro makes sure parameters are expanded before subbing into
 * string concatenation
 */
#if defined(__ARM_ARCH)
#define _uart_rx(uart_no, _tmp_byte)                                           \
  do {                                                                         \
    _tmp_byte = usart_recv(USART##uart_no);                                    \
  } while (0)
#elif defined(__AVR)
#define _uart_rx(uart_no, _tmp_byte)                                           \
  do {                                                                         \
    _tmp_byte = UDR##uart_no;                                                  \
  } while (0)
#elif defined(NEORV32)
#define _uart_rx(uart_no, _tmp_byte)                                           \
  do {                                                                         \
    _tmp_byte = neorv32_uart##uart_no##_getc();                                \
  } while (0)
#else
#define _uart_rx(uart, _tmp_byte) __builtin_unreachable()
#endif

/**
 *
 * Reads from the UART rx buffer, blocking until data is there
 *
 */
#define uart_rx_blocking(uart_no, _tmp_byte)                                   \
  do {                                                                         \
    while (1) {                                                                \
      if (is_uart_rx_data_waiting(uart_no)) {                                  \
        uart_rx(uart_no, _tmp_byte);                                           \
        break;                                                                 \
      }                                                                        \
    }                                                                          \
  } while (0)

/**
 *
 * Writes to the UART tx buffer, non-blocking, without checking if it's ready to
 * receive data
 *
 */
#define uart_tx(uart_no, data) _uart_tx(uart_no, data)

/**
 * Second level of macro makes sure parameters are expanded before subbing into
 * string concatenation
 */
#if defined(__ARM_ARCH)
#define _uart_tx(uart_no, data) usart_send(USART##uart_no, data)
#elif defined(__AVR)
#define _uart_tx(uart_no, _tmp_byte)                                           \
  do {                                                                         \
    UDR##uart_no = _tmp_byte;                                                  \
  } while (0)
#elif defined(NEORV32)
#define _uart_tx(uart_no, _tmp_byte) (neorv32_uart##uart_no##_putc(_tmp_byte))
#else
#define _uart_tx(uart, _tmp_byte) __builtin_unreachable()
#endif

/**
 *
 * Writes to the UART tx buffer, blocking until the
 * buffer is ready for writing
 *
 */
#define uart_tx_blocking(uart_no, _tmp_byte)                                   \
  do {                                                                         \
    while (1) {                                                                \
      if (is_uart_ready_to_tx(uart_no)) {                                      \
        uart_tx(uart_no, _tmp_byte);                                           \
        break;                                                                 \
      }                                                                        \
    }                                                                          \
  } while (0)

/**
 *
 * Cross-platform atomic block
 *
 */
#if defined(__ARM_ARCH)
#define _ATOMIC CM_ATOMIC_BLOCK()
#elif defined(__AVR)
#define _ATOMIC ATOMIC_BLOCK(ATOMIC_FORCEON)
#elif defined(NEORV32)
#define _ATOMIC
#else
#define _ATOMIC __builtin_unreachable()
#endif

/** \brief Receive a byte from UART into circular buffer
 *
 * Checks if uart has a received byte waiting, and if so,
 * pushes it onto the back of the circular buffer.
 *
 */
#define uart_rx_to_circ_buf(uart_no, circ_buf_ptr)                             \
  do {                                                                         \
    if (is_uart_rx_data_waiting(uart_no)) {                                    \
      uart_rx(uart_no, ASC_HELPERS_BYTE);                                      \
      circular_buffer_push_back_uint8(circ_buf_ptr, ASC_HELPERS_BYTE);         \
    }                                                                          \
  } while (0)

/** \brief Transmit a byte from the circular buffer
 *
 * Checks if uart can accept a byte and the circular
 * buffer isn't empty, and if so, pops a byte off of
 * the circular buffer and transmits it.
 *
 */
#define uart_tx_from_circ_buf(uart_no, circ_buf_ptr)                           \
  do {                                                                         \
    if (is_uart_ready_to_tx(uart_no) &&                                        \
        !circular_buffer_is_empty_uint8(circ_buf_ptr)) {                       \
      uart_tx(uart_no, circular_buffer_pop_front_uint8(circ_buf_ptr));         \
    }                                                                          \
  } while (0)

///////////////////////////////////////////////////
////// For ASC Device with Register Pointers //////
///////////////////////////////////////////////////

#define _extraInputBuffer_size_ 64

/** \brief Declarations for ascii_serial_com_device and
 * ascii_serial_com_register_pointers
 *
 * Declarations that should be outside of (and before) main() to ease use of
 * ascii_serial_com_register_pointers with ascii_serial_com_device.
 *
 * The only "public" variables are:
 *
 *      bool streaming_is_on;
 *      circular_buffer_uint8 extraInputBuffer;
 *
 * ## Notes
 *
 * **Don't follow this with a semicolon**
 *
 * ## Parameters
 *
 * None
 *
 */
#define DECLARE_ASC_DEVICE_W_REGISTER_POINTERS()                               \
  void _handle_nf_messages(ascii_serial_com *asc, char ascVersion,             \
                           char appVersion, char command, char *data,          \
                           size_t dataLen, void *state_vp);                    \
  bool streaming_is_on = false;                                                \
  circular_buffer_uint8 extraInputBuffer;                                      \
                                                                               \
  uint8_t _tmp_byte;                                                           \
  uint8_t _extraInputBuffer_raw[_extraInputBuffer_size_];                      \
  ascii_serial_com_device _ascd;                                               \
  ascii_serial_com_register_pointers _reg_pointers_state;                      \
  ascii_serial_com_device_config _ascd_config = {                              \
      .func_rw = ascii_serial_com_register_pointers_handle_message,            \
      .state_rw = &_reg_pointers_state,                                        \
      .func_nf = _handle_nf_messages,                                          \
      .state_nf = &streaming_is_on};                                           \
  void _handle_nf_messages(__attribute__((unused)) ascii_serial_com *asc,      \
                           __attribute__((unused)) char ascVersion,            \
                           __attribute__((unused)) char appVersion,            \
                           char command, __attribute__((unused)) char *data,   \
                           __attribute__((unused)) size_t dataLen,             \
                           void *state_vp) {                                   \
    bool *state = (bool *)state_vp;                                            \
    if (command == 'n') {                                                      \
      *state = true;                                                           \
    } else if (command == 'f') {                                               \
      *state = false;                                                          \
    }                                                                          \
  }

/** \brief Setup for ascii_serial_com_device and
 * ascii_serial_com_register_pointers
 *
 * Setup that should be inside main() before the polling loop. This is part of
 * a group of macros to ease use of ascii_serial_com_register_pointers with
 * ascii_serial_com_device.
 *
 * ## Notes
 *
 * **Make sure this is inside a Try/Catch block**
 *
 * **Follow this with a semicolon**
 *
 * ## Parameters
 *
 * register_map: the register map, type: REGTYPE * array
 *
 * register_write_masks: the register write masks, type: REGTYPE array
 *
 * nRegs: the length of the two arrays above
 *
 */
#define SETUP_ASC_DEVICE_W_REGISTER_POINTERS(register_map,                     \
                                             register_write_masks, nRegs)      \
  do {                                                                         \
    ascii_serial_com_register_pointers_init(                                   \
        &_reg_pointers_state, register_map, register_write_masks, nRegs);      \
    ascii_serial_com_device_init(&_ascd, &_ascd_config);                       \
                                                                               \
    circular_buffer_init_uint8(&extraInputBuffer, _extraInputBuffer_size_,     \
                               _extraInputBuffer_raw);                         \
  } while (0)

/** \brief Polling for ascii_serial_com_device and
 * ascii_serial_com_register_pointers
 *
 * Within the polling loop, handles transmitting bytes, processing received
 * bytes, and handling received messages. This is part of a group of macros to
 * ease use of ascii_serial_com_register_pointers with ascii_serial_com_device.
 *
 * ## Notes
 *
 * **Assumes that something else receives bytes and puts them in**
 * `extraInputBuffer`
 *
 * **Make sure this is inside a Try/Catch block**
 *
 * **Follow this with a semicolon**
 *
 * ## Parameters
 *
 * uart: the address of the USART used for transmitting bytes. For STM32:
 * USART1, USART2, .... For AVR: UDR0, ....
 *
 */
#define HANDLE_ASC_COMM_IN_POLLING_LOOP(uart_no)                               \
  do {                                                                         \
    uart_tx_from_circ_buf(uart_no,                                             \
                          ascii_serial_com_device_get_output_buffer(&_ascd));  \
    if (!circular_buffer_is_empty_uint8(&extraInputBuffer)) {                  \
      _ATOMIC {                                                                \
        _tmp_byte = circular_buffer_pop_front_uint8(&extraInputBuffer);        \
      }                                                                        \
      circular_buffer_push_back_uint8(                                         \
          ascii_serial_com_device_get_input_buffer(&_ascd), _tmp_byte);        \
    }                                                                          \
    ascii_serial_com_device_receive(&_ascd);                                   \
  } while (0)

/** \brief Check if you should stream a message to the host
 *
 * This is a boolean value macro that checks if streaming is enabled and the
 * transmit buffer is empty. If true, the user may stream a message to the host
 * now with STREAM_TO_HOST_ASC_DEVICE_W_REGISTER_POINTERS
 *
 * This is part of a group of macros to ease use of
 * ascii_serial_com_register_pointers with ascii_serial_com_device.
 *
 * ## Notes
 *
 * **Make sure this is inside a Try/Catch block**
 *
 */
#define READY_TO_STREAM_ASC_DEVICE_W_REGISTER_POINTERS                         \
  (streaming_is_on &&                                                          \
   circular_buffer_get_size_uint8(                                             \
       ascii_serial_com_device_get_output_buffer(&_ascd)) == 0)

/** \brief Stream data to the host
 *
 * This macro puts a message in the output buffer
 *
 * This is part of a group of macros to ease use of
 * ascii_serial_com_register_pointers with ascii_serial_com_device.
 *
 * ## Notes
 *
 * **Make sure this is inside a Try/Catch block**
 *
 * ## Parameters
 *
 * data: a pointer to data or array of data to be sent
 *
 * data_size: a uint describing the data size in bytes
 *
 */
#define STREAM_TO_HOST_ASC_DEVICE_W_REGISTER_POINTERS(data, data_size)         \
  ascii_serial_com_device_put_s_message_in_output_buffer(&_ascd, '0', '0',     \
                                                         data, data_size)

#endif
