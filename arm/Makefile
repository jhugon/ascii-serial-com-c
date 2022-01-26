stm32f091rcfirmware=arm/stm32f091nucleo64_blink.elf arm/stm32f091nucleo64_write_pattern_to_serial.elf arm/stm32f091nucleo64_char_loopback.elf arm/stm32f091nucleo64_asc_loopback.elf arm/stm32f091nucleo64_adc_streaming.elf arm/stm32f091nucleo64_pulser.elf
stm32f091rcldscript=arm/generated.stm32f091rc.ld

CFLAGSstm32f091rc=$(CFLAGS) -DSTM32F0 -DSTM32F091RC -msoft-float

ifeq ($(platform),cortex-m0)
  armfirmware+=$(stm32f091rcfirmware)
  outarmfirmware+=$(addprefix $(builddir),$(notdir $(armfirmware)))
endif

$(stm32f091rcfirmware): %.elf: %.c libasciiserialcom.a externals/libthrowtheswitch.a $(stm32f091rcldscript)
	arm-none-eabi-gcc $(CFLAGSstm32f091rc) $(LDFLAGS) -T $(stm32f091rcldscript) -o $@ $< libasciiserialcom.a externals/libthrowtheswitch.a -lopencm3_stm32f0

$(stm32f091rcldscript): $(OPENCM3_DIR)/ld/linker.ld.S
	arm-none-eabi-gcc $(CFLAGSstm32f091rc) $(shell  $(OPENCM3_DIR)/scripts/genlink.py $(OPENCM3_DEVICES_DATA) stm32f091rc DEFS) -P -E $< -o $@

$(outarmfirmware): $(builddir)%: arm/% | $(builddir)
	cp $^ $@
