#include "asc_helpers.h"
#include "avr/avr_uart.h"
#include <avr/io.h>

#define FOSC 16000000L
#define BAUD 9600
#define MYUBRR (FOSC / 16 / BAUD - 1)
#define UART_NO 0

#define bufCap 64

static const char message[] = ">00s0 0 0 0.DE10\n";
static const uint8_t message_len = 17;
uint8_t iChar = 0;

int main(void) {

  UART_Init(UART_NO, MYUBRR, 0);

  while (1) {
    uart_tx_blocking(UART_NO, message[iChar]);
    iChar++;
    if (iChar == message_len) {
      iChar = 0;
    }
  }
  return 0;
}
