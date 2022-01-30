# Instructions for NeoRV32 Soft-Core

In the VHDL project:

Check that:
- `MEM_INT_IMEM_EN => true`
- `MEM_INT_IMEM_SIZE => ` >= the size of the text section of the program
- `MEM_INT_DMEM_SIZE => ` >= the size needed for data+bss+stack
- `CPU_EXTENSION_RISCV_C => true` if using compressed instructions
- `CPU_EXTENSION_RISCV_M => true` if you need muliply/divide

You can have the fw present as ROM in the FPGA design and/or upload it using NeoRV32's serial bootloader.

## Write FW into FPGA design ROM

Copy the firmware to `rtl/core/neorv32_application_image.vhd` in the neorv32
sub-module. For example:

    cp build/rv32ic_gcc_opt/neorv32_blink_led_image.vhd riscv/neorv32/neorv32/rtl/core/neorv32_application_image.vhd

## Upload FW with NeoRV32's serial bootloader

**GTKTerm only works for < 4KB, Cutecom works for larger firmware files**

- Open GTKTerm
- Click Configuration -> Port
- Set the port to the relevant (USB) one in /dev/tty\*. Baud rate to 19200, 8 bit, 1 stop bit, no parity, and no flow control.
- Click Okay
- Click Configuration -> CR LF Auto (make sure it's checked)

Now that the terminal is setup you can upload the firmware:

- Press the button that resets the NeoRV32 core on the board
- You should see some stuff on the terminal, press any key
- Press `u` at the command prompt
- Click File -> Send Raw File (or just Shift+Ctrl+R)
- Select your firmware file (e.g. `build/rv32ic_gcc_opt/neorv32_blink_led_uploadable.bin`)
- OK should appear in your terminal
- Press `e` to execute your program
