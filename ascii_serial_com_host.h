#ifndef ASCII_SERIAL_COM_HOST_H
#define ASCII_SERIAL_COM_HOST_H

/** \file ascii_serial_com_host.h
 * \brief ASCII Serial Com Host
 *
 * The host is meant to be used as followed:
 *
 * Declare:
 *
 *     ascii_serial_com_host host;
 *     circular_buffer_uint8 * in_buf;
 *     circular_buffer_uint8 * out_buf;
 *
 * Before the polling starts:
 *
 *     ascii_serial_com_host_init(&host,'0');
 *     in_buf = ascii_serial_com_host_get_input_buffer(&host);
 *     out_buf = ascii_serial_com_host_get_output_buffer(&host);
 *
 * In the polling loop:
 *
 *     uart_rx_to_circ_buf(UART_NO,in_buf);
 *
 *     ascii_serial_com_host_poll_input_buffer(&host);
 *
 *     // to just read once
 *     if (host.status == NOT_AWAITING_RESPONSE) {
 *       ascii_serial_com_host_read_register(&host,REGNUM);
 *     } elif (host.status == VALID_RESPONSE {
 *       uint32_t readval = host.registerVal;
 *       // ... do stuff with readval ....
 *     })
 *
 *     uart_tx_from_circ_buf(UART_NO,out_buf);
 *
 */

#include "ascii_serial_com.h"

/** \brief ASCII Serial Com Host Status Enum
 *
 * Member of ascii_serial_com_host struct
 *
 */
enum ascii_serial_com_host_status {
  NOT_AWAITING_RESPONSE, /**< No messages yet sent so not awaiting response */
  INVALID_RESPONSE,      /**< Invalid response received */
  AWAITING_RESPONSE,     /**< Message sent, awaiting response message */
  VALID_RESPONSE /**< Response recieved, valid, and matching sent message */
};

/** \brief ASCII Serial Com Host State struct
 *
 *  Keeps track of the state of the ASCII Serial Com host
 *
 * The functions take the stuff from
 * ascii_serial_com_get_message_from_input_buffer plus a void pointer to
 * possible state/configuration info. If the functions are null ptrs, an error
 * message returned to host. This class owns the data buffer passed around.
 */
typedef struct __ascii_serial_com_host {
  ascii_serial_com asc; /**< used to send and recieve messages */
  char ascVersion; /**< This and next 4 variables just hold results, putting
                      them here lets them be statically allocated */
  char appVersion;
  char command;
  char dataBuffer[MAXDATALEN]; /**< data part of message received here */
  size_t dataLen;
  ascii_serial_com_host_status
      status;           /**< status/state of a read/write message */
  uint16_t registerNum; /**< register number a read/write message was sent
                           about. Used to verify received messages */
  uint32_t registerVal; /**< register value recived back for a read message */
} ascii_serial_com_host;

/** \brief ASCII Serial Com Host init
 *
 * Initialize ASCII Serial Com host
 *
 */
void ascii_serial_com_host_init(
    ascii_serial_com_host *asch,
    const char appVersion /**< app version used for sent messages */
);

/** \brief ASCII Serial Com Host poll input buffer
 *         for received messages
 *
 * Attempts to read response messages from input
 * buffer looking for confirmation of register
 * writes or data from register reads.
 *
 * /param asch: initialized ascii_serial_com_host
 *
 */
void ascii_serial_com_host_poll_input_buffer(ascii_serial_com_host *asch);

/** \brief ASCII Serial Com Host get input buffer
 *
 * MAKE SURE asch IS ALREADY INITIALIZED!
 *
 * /param asch: initialized ascii_serial_com_host
 */
circular_buffer_uint8 *
ascii_serial_com_host_get_input_buffer(ascii_serial_com_host *asch);

/** \brief ASCII Serial Com Host get output buffer
 *
 * MAKE SURE asch IS ALREADY INITIALIZED!
 *
 * /param asch: initialized ascii_serial_com_host
 */
circular_buffer_uint8 *
ascii_serial_com_host_get_output_buffer(ascii_serial_com_host *asch);

/** \brief ASCII Serial Com Host start a register read
 *
 * Reads into uint32_t, but can read from devices
 * with either 8 bit or 32 bit register widths
 *
 * MAKE SURE asch IS ALREADY INITIALIZED!
 *
 */
void ascii_serial_com_host_read_register(
    ascii_serial_com_host *asch, /**< initialized host state */
    const uint16_t regnum        /**< register number to read */
);

/** \brief ASCII Serial Com Host start a register write
 *
 * MAKE SURE THE DEVICE HAS 8 BIT REGISTERS!
 *
 * MAKE SURE asch IS ALREADY INITIALIZED!
 *
 */
void ascii_serial_com_host_write_register_8bit(
    ascii_serial_com_host *asch, /**< initialized host state */
    const uint16_t regnum,       /**< register number to write */
    const uint8_t regval         /**< register value to write */
);

/** \brief ASCII Serial Com Host start a register write
 *
 * MAKE SURE THE DEVICE HAS 32 BIT REGISTERS!
 *
 * MAKE SURE asch IS ALREADY INITIALIZED!
 *
 */
void ascii_serial_com_host_write_register_32bit(
    ascii_serial_com_host *asch, /**< initialized host state */
    const uint16_t regnum,       /**< register number to write */
    const uint32_t regval        /**< register value to write */
);

#endif
