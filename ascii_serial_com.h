#ifndef ASCII_SERIAL_COM_H
#define ASCII_SERIAL_COM_H

/** \file */

#include <inttypes.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "asc_exception.h"
#include "circular_buffer.h"

#define MAXMESSAGELEN 64
#define MAXDATALEN 54
#define MAXSPAYLOADEN (MAXDATALEN - 3)
#define NCHARCHECKSUM 4

////////////////////////////////////////////////////
////////////////////////////////////////////////////
//////////////// Public Interface //////////////////
////////////////////////////////////////////////////
////////////////////////////////////////////////////

/** \brief ASCII Serial Com Interface State struct
 *
 *  Keeps track of the state of the ASCII Serial Com interface
 *
 *  Normally, for a device, usage would go something like:
 *
 *  1) allocate the ascii_serial_com
 *
 *  2) initialize it with ascii_serial_com_init
 *
 *  3) Get the input buffer with ascii_serial_com_get_input_buffer and output
 * buffer with ascii_serial_com_get_output_buffer
 *
 *  4) poll on the input stream/file/peripheral (and output
 * stream/file/peripheral if the output buffer is not empty)
 *
 *  5) read from the input stream/file/peripheral
 *
 *  6) push_back or push_back_block what is read to the input_buffer
 *
 *  7) run ascii_serial_com_get_message_from_input_buffer, if a message is
 * received (unpacked) then act on it (possibly reply with
 * ascii_serial_com_put_message_in_output_buffer)
 *
 *  8) pop from output_buffer and write to output stream/file/peripheral
 *
 *  9) go back to 4)
 *
 */
typedef struct __ascii_serial_com_struct {
  circular_buffer_uint8 in_buf;  /**< Input buffer */
  circular_buffer_uint8 out_buf; /**< Output buffer */
  uint8_t
      raw_buffer[2 * MAXMESSAGELEN]; /**< Raw buffer used by circular buffers */
  uint8_t
      send_stream_frame_counter; /**< counter put in sent streaming messages */
  uint8_t receive_stream_frame_counter; /**< counter streaming messages for
                                           checking received messages */
  bool receive_stream_frame_counter_initialized; /**< true after receiving first
                                                    s message */
  bool ignoreCRCMismatch; /**< if true, ignore CRC errors. default false */
} ascii_serial_com;

/** \brief ASCII Serial Com Interface init method
 *
 *  Initialize interface
 *
 *  \param asc is a pointer to an uninitialized ascii_serial_com struct
 *
 */
void ascii_serial_com_init(ascii_serial_com *asc);

/** \brief ASCII Serial Com Pack and put message in output buffer
 *
 *  Packs the message into the output format and push it onto the output buffer
 *  USER'S RESPONSIBILITY TO MAKE SURE MESSAGE CAN FIT IN OUTPUT CIRCULAR
 * BUFFER. Message length is dataLen + (MAXMESSAGELEN-MAXDATALEN)
 *
 *  \param asc is a pointer to an initialized ascii_serial_com struct
 *
 *  \param ascVersion: the single char ASCII Serial Com version (probably '0')
 *
 *  \param appVersion: the single char application version (user application
 * info)
 *
 *  \param command: the single char command will be written to this byte
 *
 *  \param data: The message data
 *
 *  \param dataLen: The length of the data
 *
 *  May raise ASC_ERROR_DATA_TOO_LONG or the errors
 *  ascii_serial_com_compute_checksum raises
 *
 */
void ascii_serial_com_put_message_in_output_buffer(
    ascii_serial_com *asc, char ascVersion, char appVersion, char command,
    const char *data, size_t dataLen);

/** \brief ASCII Serial Com pop message from input buffer and unpack
 *
 *  Unpacks the first message found in the input buffer
 *
 *  \param asc is a pointer to an initialized ascii_serial_com struct
 *
 *  All other parameters are outputs. Command will be set to \0 if no message
 * found in buffer
 *
 * \param ascVersion: the single char ASCII-Serial-Com version
 * in the message will be written here
 *
 * \param appVersion: the single char application version in the message will be
 * written here
 *
 * \param command: the single char command will be written to this byte
 *
 * \param data: The message data will be put here. Should point to a MAXDATALEN
 * long buffer
 *
 * \param dataLen: The length of the data put in data
 *
 * May raise ASC_ERROR_INVALID_FRAME or ASC_ERROR_INVALID_FRAME_PERIOD
 */
void ascii_serial_com_get_message_from_input_buffer(ascii_serial_com *asc,
                                                    char *ascVersion,
                                                    char *appVersion,
                                                    char *command, char *data,
                                                    size_t *dataLen);

/** \brief ASCII Serial Com get input buffer
 *
 *  Get a pointer to the input buffer.
 *
 *  \param asc is a pointer to an initialized ascii_serial_com struct
 *
 *  \return a pointer to the input buffer.
 */
circular_buffer_uint8 *ascii_serial_com_get_input_buffer(ascii_serial_com *asc);

/** \brief ASCII Serial Com get output buffer
 *
 *  Get a pointer to the output buffer.
 *
 *  \param asc is a pointer to an initialized ascii_serial_com struct
 *
 *  \return a pointer to the output buffer.
 */
circular_buffer_uint8 *
ascii_serial_com_get_output_buffer(ascii_serial_com *asc);

/** \brief ASCII Serial Com Pack and put 's' message in output buffer
 *
 *  Packs the message into the output format and push it onto the output buffer
 *  USER'S RESPONSIBILITY TO MAKE SURE MESSAGE CAN FIT IN OUTPUT CIRCULAR
 * BUFFER. Message length is dataLen + (MAXMESSAGELEN-MAXDATALEN)
 *
 *  This sends streaming data. If this is a device, you should only do this
 *  once you've received an 'n' message and stop after receiving an 'f' message.
 *
 *  \param asc is a pointer to an initialized ascii_serial_com struct
 *
 *  \param ascVersion: the single char ASCII Serial Com version (probably '0')
 *
 *  \param appVersion: the single char application version (user application
 * info)
 *
 *  \param data: The message data
 *
 *  \param dataLen: The length of the data, must be <= MAXSPAYLOADEN
 *
 *  May raise ASC_ERROR_DATA_TOO_LONG or the errors
 *  ascii_serial_com_compute_checksum raises
 *
 */
void ascii_serial_com_put_s_message_in_output_buffer(ascii_serial_com *asc,
                                                     char ascVersion,
                                                     char appVersion,
                                                     const char *data,
                                                     size_t dataLen);

/** \brief ASCII Serial Com put error message in out buffer
 *
 * Called when you want to return an error message related to some input
 * message
 *
 * The same parameters as ascii_serial_com_put_message_in_output_buffer, except
 * data isn't const (should it be const?) and errorCode
 *
 */
void ascii_serial_com_put_error_in_output_buffer(ascii_serial_com *asc,
                                                 char ascVersion,
                                                 char appVersion, char command,
                                                 char *data, size_t dataLen,
                                                 enum asc_exception errorCode);

void ascii_serial_com_set_ignore_CRC_mismatch(ascii_serial_com *asc);

void ascii_serial_com_unset_ignore_CRC_mismatch(ascii_serial_com *asc);

////////////////////////////////////////////////////
////////////////////////////////////////////////////
//////////////// Private Methods ///////////////////
////////////////////////////////////////////////////
////////////////////////////////////////////////////

/** \brief ASCII Serial Com compute checksum of message
 *
 *  Computes the checksum of the last message in the input or output buffer.
 * Specifically finds the last substring starting with the last '>' and ending
 * in the last '.'.
 *
 *  \param asc is a pointer to an initialized ascii_serial_com struct
 *
 *  \param checksumOut: pointer to already initialized buffer NCHARCHECKSUM long
 *
 *  \param outputBuffer: if true, use output buffer, if false use input buffer
 *
 *  Raises ASC_ERROR_INVALID_FRAME or ASC_ERROR_INVALID_FRAME_PERIOD if frame
 *  invalid
 *
 */
void ascii_serial_com_compute_checksum(ascii_serial_com *asc, char *checksumOut,
                                       bool outputBuffer);

/** \brief convert uint8 to hex string
 *
 *  Converts uint8 to capital hex string, 2-bytes long, zero padded
 *
 *  \param num: the number to convert to hex
 *
 *  \param outstr: a pointer to a 2-byte long string that will hold the result
 *
 *  \param caps: hex letters are caps A-F if true, and lowercase a-f if false
 *
 */
void convert_uint8_to_hex(uint8_t num, char *outstr, bool caps);

/** \brief convert uint16 to hex string
 *
 *  Converts uint16 to capital hex string, 4-bytes long, zero padded
 *
 *  \param num: the number to convert to hex
 *
 *  \param outstr: a pointer to a 4-byte long string that will hold the result
 *
 *  \param caps: hex letters are caps A-F if true, and lowercase a-f if false
 *
 */
void convert_uint16_to_hex(uint16_t num, char *outstr, bool caps);

/** \brief convert uint32 to hex string
 *
 *  Converts uint32 to capital hex string, 8-bytes long, zero padded
 *
 *  \param num: the number to convert to hex
 *
 *  \param outstr: a pointer to a 8-byte long string that will hold the result
 *
 *  \param caps: hex letters are caps A-F if true, and lowercase a-f if false
 *
 */
void convert_uint32_to_hex(uint32_t num, char *outstr, bool caps);

/** \brief convert hex string to uint8
 *
 *  Converts hex string to uint8
 *
 *  \param str: a pointer to a 2-byte long string that holds the hex input
 *
 *  \return the uint8_t
 *
 *  May throw ASC_ERROR_NOT_HEX_CHAR
 */
uint8_t convert_hex_to_uint8(const char *instr);

/** \brief convert hex string to uint16
 *
 *  Converts hex string to uint16
 *
 *  \param str: a pointer to a 4-byte long string that holds the hex input
 *
 *  \return the uint16_t
 *
 *  May throw ASC_ERROR_NOT_HEX_CHAR
 *
 */
uint16_t convert_hex_to_uint16(const char *instr);

/** \brief convert hex string to uint32
 *
 *  Converts hex string to uint32
 *
 *  \param str: a pointer to a 8-byte long string that holds the hex input
 *
 *  \return the uint32_t
 *
 *  May throw ASC_ERROR_NOT_HEX_CHAR
 *
 */
uint32_t convert_hex_to_uint32(const char *instr);

//////////////////////////////////////////////////
//////////////////////////////////////////////////
//////////////// Helper Macros ///////////////////
//////////////////////////////////////////////////
//////////////////////////////////////////////////

#if defined(__ARM_ARCH)
#define is_usart_recv_data_waiting(usart) ((USART_ISR(usart) & USART_ISR_RXNE))
#elif defined(__AVR)
#define is_usart_recv_data_waiting(usart) (UCSR0A & (1 << RXC0))
#elif defined(NEORV32)
#define is_usart_recv_data_waiting(usart) neorv32_uart1_char_received()
#else
#define is_usart_recv_data_waiting(usart) __builtin_unreachable()
#endif

#if defined(__ARM_ARCH)
#define is_usart_ready_to_send(usart) ((USART_ISR(usart) & USART_ISR_TXE))
#elif defined(__AVR)
#define is_usart_ready_to_send(usart) (UCSR0A & (1 << UDRE0))
#elif defined(NEORV32)
#define is_usart_ready_to_send(usart)                                          \
  ((NEORV32_UART1.CTRL & (1 << UART_CTRL_TX_FULL)) == 0)
#else
#define is_usart_ready_to_send(usart) __builtin_unreachable()
#endif

#if defined(__ARM_ARCH)
// Already defined by libopencm3
#elif defined(__AVR)
#define usart_recv(reg, _tmp_byte) _tmp_byte = reg
#elif defined(NEORV32)
#define usart_recv(reg, _tmp_byte) _tmp_byte = neorv32_uart1_getc()
#else
#define usart_recv(usart, _tmp_byte) __builtin_unreachable()
#endif

#if defined(__ARM_ARCH)
// Already defined by libopencm3
#elif defined(__AVR)
#define usart_send(reg, _tmp_byte) reg = _tmp_byte
#elif defined(NEORV32)
#define usart_send(reg, _tmp_byte) neorv32_uart1_putc(_tmp_byte)
#else
#define usart_send(usart, _tmp_byte) __builtin_unreachable()
#endif

#if defined(__ARM_ARCH)
#define _ATOMIC CM_ATOMIC_BLOCK()
#elif defined(__AVR)
#define _ATOMIC ATOMIC_BLOCK(ATOMIC_FORCEON)
#elif defined(NEORV32)
#define _ATOMIC
#else
#define _ATOMIC __builtin_unreachable()
#endif

#endif
