# ASCII Serial Com for the C Language

See main project repo at: [ascii-serial-com](https://github.com/jhugon/ascii-serial-com)

You may need to download and compile libopencm3:

    git submodule init
    cd tools/libopencm3/
    make

and download and build AVR tools with:

    cd tools/
    ./make-avr.sh

## Standard Usage

Expose a register interface for a device.

This interface contains an array of pointers, so that each message address
points to an arbitrary address. Additionally, a write mask controls which
bits of the pointed to words are writable.

You probably want to create the array of pointers like:

     #include "ascii_serial_com_register_pointers.h"

     #define NREGS 6
     REGTYPE * reg_pointers[NREGS] = {
      &x,
      &y,
      &z,
      NULL,
      NULL,
      NULL
     };

     REGTYPE reg_write_masks[NREGS] = {
      0,
      0x3,
      0xFF,
      0,
      0,
      0
     }

     ascii_serial_com_register_pointers_init(&rps,reg_pointers,reg_write_masks,NREGS);

A set of convenience macros are provided to ease the common use of
ascii_serial_com_register_pointers with ascii_serial_com_device.

To use the macros:

1. Declare the register pointer and register write mask array as above
2. Before the main() function, put `DECLARE_ASC_DEVICE_W_REGISTER_POINTERS()`
on a line **with no semicolon**
3. In the setup portion of main(), before the polling loop, inside a
Try/Catch block, put:
`SETUP_ASC_DEVICE_W_REGISTER_POINTERS(reg_map,reg_write_masks, NREGS);`,
where the variables are declared as in the previous section.
4. Inside the polling loop and a Try/Catch block, put:
`HANDLE_ASC_COMM_IN_POLLING_LOOP(USART1);`, replacing USART1 with the
appropriate USART.

Number 4 assumes something is putting received bytes into a
circular_buffer_uint8 called `extraInputBuffer` (declared and setup by the
macros). This can be accomplished using, for STM32,
`def_usart_isr_push_rx_to_circ_buf(<usart_isr>,<usart>,&extraInputBuffer)`
(no semicolon)

### Streaming

The boolean valued macro `READY_TO_STREAM_ASC_DEVICE_W_REGISTER_POINTERS`
should be checked before actually streaming a message. and
`STREAM_TO_HOST_ASC_DEVICE_W_REGISTER_POINTERS(data,data_len);` should be
run to send the message.

The macros define a `bool streaming_is_on` that is initialized to false, set
to true when a "n" message is received, and set to false when a "f" message
is received. It should be used to decide when to start auxiliary routines
that prepare to stream messages.
