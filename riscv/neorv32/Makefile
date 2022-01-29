neorv32firmwareelf=riscv/neorv32/neorv32_blink_led.elf riscv/neorv32/neorv32_char_loopback.elf
neorv32firmwarebin=$(neorv32firmwareelf:.elf=.bin)
neorv32firmwareuploadable=$(neorv32firmwareelf:.elf=_uploadable.bin)
neorv32firmwareimage=$(neorv32firmwareelf:.elf=_image.vhd)

ifneq (,$(findstring rv32i,$(platform)))
  riscvfirmware+=$(neorv32firmwareelf) $(neorv32firmwarebin) $(neorv32firmwareuploadable) $(neorv32firmwareimage)
  outriscvfirmware+=$(addprefix $(builddir),$(notdir $(neorv32firmwareelf) $(neorv32firmwarebin) $(neorv32firmwareuploadable) $(neorv32firmwareimage)))
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

## generates image for neorv32 (compiled native)
riscv/neorv32/image_gen.c: riscv/neorv32/neorv32/sw/image_gen/image_gen.c
	cp $^ $@
riscv/neorv32/image_gen: riscv/neorv32/image_gen.c
	g++ -Wall -O -g $^ -o $@
$(neorv32firmwareuploadable): %_uploadable.bin: %.bin riscv/neorv32/image_gen
	@set -e
	riscv/neorv32/image_gen -app_bin $< $@ $(shell basename $(CURDIR))
$(neorv32firmwareimage): %_image.vhd: %.bin riscv/neorv32/image_gen
	@set -e
	riscv/neorv32/image_gen -app_img $< $@ $(shell basename $(CURDIR))
