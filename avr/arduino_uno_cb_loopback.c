#include "asc_exception.h"
#include "asc_helpers.h"
#include "avr/avr_uart.h"
#include "circular_buffer.h"
#include <avr/io.h>

#define FOSC 16000000L
#define BAUD 9600
#define UART_NO 0
#define MYUBRR (FOSC / 16 / BAUD - 1)

#define bufCap 64

uint8_t byteBuffer;
uint8_t rawBuffer[bufCap];
circular_buffer_uint8 cb;
CEXCEPTION_T e;

int main(void) {

  Try { circular_buffer_init_uint8(&cb, bufCap, rawBuffer); }
  Catch(e) { return e; }

  USART_Init(0, MYUBRR, 0);

  while (1) {
    Try {
      uart_rx_to_circ_buf(0, &cb);
      uart_tx_from_circ_buf(0, &cb);
    }
    Catch(e) { return e; }
  }
  return 0;
}
