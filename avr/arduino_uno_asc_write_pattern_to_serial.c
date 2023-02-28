#include "asc_exception.h"
#include "asc_helpers.h"
#include "ascii_serial_com.h"
#include "avr/avr_uart.h"
#include <avr/io.h>
#define UART_NO 0

#define F_CPU 16000000L
#include <util/delay.h>
#define FOSC 16000000L
#define BAUD 9600
#define MYUBRR (FOSC / 16 / BAUD - 1)

CEXCEPTION_T e;
uint16_t nExceptions;

ascii_serial_com asc;
circular_buffer_uint8 *asc_out_buf;
const char ascVersion = '0';
const char appVersion = '0';
char dataBuffer[2];
const size_t dataLen = 2;

int main(void) {

  Try {
    ascii_serial_com_init(&asc);
    ascii_serial_com_set_ignore_CRC_mismatch(&asc);
    asc_out_buf = ascii_serial_com_get_output_buffer(&asc);
    dataBuffer[0] = '0';
    dataBuffer[1] = '0';
  }
  Catch(e) { return e; }

  UART_Init(UART_NO, MYUBRR, 0);

  while (true) {
    Try {
      if (circular_buffer_is_empty_uint8(asc_out_buf)) {
        ascii_serial_com_put_s_message_in_output_buffer(
            &asc, ascVersion, appVersion, dataBuffer, dataLen);
        if (dataBuffer[0] == '9') {
          dataBuffer[0] = '0';
        } else {
          dataBuffer[0]++;
        }
      }
      uart_tx_from_circ_buf(UART_NO, asc_out_buf);
    }
    Catch(e) { nExceptions++; }
  }

  return 0;
}
