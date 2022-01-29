#include <neorv32.h>
#include <stdbool.h>

// Lower 16 bits of GPIO.OUTPUT_LO are writable to the leds
// GPIO.INPUT_HI is readable for the switches (bits 15 down to 1)

#define BAUD_RATE 19200
#define UART1_TX_BUFFER_FULL ((NEORV32_UART1.CTRL & (1<<UART_CTRL_TX_FULL)) != 0)

char tmpChar;
char loopbackChar;
bool charReceived=false;
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

  neorv32_uart1_setup(9600,PARITY_NONE,FLOW_CONTROL_NONE);

  neorv32_uart0_print("Starting UART1 loopback demo program\n");

  // Assumes uart has a larger than default fifo
  while(1) {
    if(!charReceived) {
        rx_status = neorv32_uart1_getc_safe(&tmpChar);
        if (rx_status == 0) { // successfully rx a char
            loopbackChar = tmpChar;
            charReceived = true;
            neorv32_uart0_printf("NEORV32_GPIO.INPUT_HI: %x\n",NEORV32_GPIO.INPUT_HI);
        }
    }
    if(charReceived && !UART1_TX_BUFFER_FULL) {
        neorv32_uart1_putc(loopbackChar);
        charReceived = false;
        NEORV32_GPIO.OUTPUT_LO = ~NEORV32_GPIO.OUTPUT_LO;
    }
  }

  return 0;
}
