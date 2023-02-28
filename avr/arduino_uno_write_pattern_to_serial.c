#include "asc_helpers.h"
#include "avr/avr_uart.h"
#include <avr/io.h>

#define F_CPU 16000000L
#include <util/delay.h>
#define FOSC 16000000L
#define BAUD 9600
#define MYUBRR (FOSC / 16 / BAUD - 1)
#define UART_NO 0

uint8_t byteBuffer;

int main(void) {

  UART_Init(UART_NO, MYUBRR, 0);

  byteBuffer = 48;
  while (1) {
    uart_tx_blocking(UART_NO, byteBuffer);
    byteBuffer++;
    if (byteBuffer > 57) {
      byteBuffer = 48;
      //_delay_ms(100);
    }
  }
  return 0;
}
