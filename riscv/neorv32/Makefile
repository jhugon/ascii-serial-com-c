neorv32firmwareelf=riscv/neorv32/neorv32_blink_led.elf
neorv32firmwarebin=$(neorv32firmwareelf:.elf=.bin)

ifneq (,$(findstring rv32i,$(platform)))
  riscvfirmware+=$(neorv32firmwareelf) $(neorv32firmwarebin)
  outriscvfirmware+=$(addprefix $(builddir),$(notdir $(neorv32firmwareelf) $(neorv32firmwarebin)))
endif

neorv32ldscript=riscv/neorv32/neorv32/sw/common/neorv32.ld
neorv32coreinc=riscv/neorv32/neorv32/sw/lib/include
neorv32coresrc=$(wildcard riscv/neorv32/neorv32/sw/lib/source/*.c)
neorv32coresrc+=riscv/neorv32/neorv32/sw/common/crt0.S

$(neorv32firmwareelf): %.elf: %.c libasciiserialcom.a externals/libthrowtheswitch.a
	$(CC) $(CFLAGS) $(LDFLAGS) -T $(neorv32ldscript) -I $(neorv32coreinc) -o $@ $< libasciiserialcom.a externals/libthrowtheswitch.a $(neorv32coresrc)

$(neorv32firmwarebin): %.bin: %.elf
	riscv32-unknown-elf-objcopy -I elf32-little $< -j .text   -O binary text.bin
	riscv32-unknown-elf-objcopy -I elf32-little $< -j .rodata -O binary rodata.bin
	riscv32-unknown-elf-objcopy -I elf32-little $< -j .data   -O binary data.bin
	cat text.bin rodata.bin data.bin > $@
	rm -f text.bin rodata.bin data.bin

$(outriscvfirmware): $(builddir)%: riscv/neorv32/% | $(builddir)
	cp $^ $@
