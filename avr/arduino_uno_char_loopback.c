#include "asc_helpers.h"
#include "avr/avr_uart.h"
#include <avr/io.h>

#define F_CPU 16000000L
#include <util/delay.h>
#define FOSC 16000000L
//#define BAUD 9600
#define BAUD 19200
#define MYUBRR (FOSC / 16 / BAUD - 1)
#define UART_NO 0

uint8_t byteBuffer;

int main(void) {

  UART_Init(UART_NO, MYUBRR, 0);

  while (1) {
    while (1) {
      if (is_uart_rx_data_waiting(UART_NO)) {
        uart_rx(UART_NO, byteBuffer);
        break;
      }
    }
    //_delay_ms(100);
    while (1) {
      if (is_uart_ready_to_tx(UART_NO)) {
        uart_tx(UART_NO, byteBuffer);
        break;
      }
    }
  }
  return 0;
}
