attiny817firmware=avr/attiny817_xplained_blink

attiny3217firmware=avr/attiny3217_cnano_blink

atmega328pfirmware=avr/arduino_uno_blink avr/arduino_uno_blink_interrupt avr/arduino_uno_char_loopback avr/arduino_uno_cb_loopback avr/arduino_uno_asc_loopback avr/arduino_uno_dummy_register_block avr/arduino_uno_register_pointers avr/arduino_uno_write_pattern_to_serial avr/arduino_uno_write_message_to_serial avr/arduino_uno_asc_write_pattern_to_serial avr/arduino_uno_test_device avr/arduino_uno_adc_streaming

atmega2560firmware=avr/arduino_mega2560_blink

atmega4809firmware=avr/atmega4809_cnano_blink

CFLAGSattiny817=$(subst -mmcu=avrxmega3,-mmcu=attiny817,$(CFLAGS))
CFLAGSattiny3217=$(subst -mmcu=avrxmega3,-mmcu=attiny3217,$(CFLAGS))
CFLAGSatmega328p=$(subst -mmcu=avr5,-mmcu=atmega328p,$(CFLAGS))
CFLAGSatmega2560=$(subst -mmcu=avr6,-mmcu=atmega2560,$(CFLAGS))
CFLAGSatmega4809=$(subst -mmcu=avrxmega3,-mmcu=atmega4809,$(CFLAGS))

ifeq ($(platform),avr5) # atmega328 and 328p
  avrfirmware+=$(atmega328pfirmware)
  outavrfirmware+=$(addprefix $(builddir),$(notdir $(avrfirmware)))
endif
ifeq ($(platform),avr6) # atmega2560
  avrfirmware+=$(atmega2560firmware)
  outavrfirmware+=$(addprefix $(builddir),$(notdir $(avrfirmware)))
endif
ifeq ($(platform),avrxmega3) # atmega4809 attiny817 attiny3217
  avrfirmware+=$(attiny817firmware)
  avrfirmware+=$(attiny3217firmware)
  avrfirmware+=$(atmega4809firmware)
  outavrfirmware+=$(addprefix $(builddir),$(notdir $(avrfirmware)))
endif

$(attiny817firmware): %: %.c libasciiserialcom.a externals/libthrowtheswitch.a
	avr-gcc $(CFLAGSattiny817) -o $@ $^

$(attiny3217firmware): %: %.c libasciiserialcom.a externals/libthrowtheswitch.a
	avr-gcc $(CFLAGSattiny3217) -o $@ $^

$(atmega328pfirmware): %: %.c libasciiserialcom.a externals/libthrowtheswitch.a
	avr-gcc $(CFLAGSatmega328p) -o $@ $^

$(atmega2560firmware): %: %.c libasciiserialcom.a externals/libthrowtheswitch.a
	avr-gcc $(CFLAGSatmega2560) -o $@ $^

$(atmega4809firmware): %: %.c libasciiserialcom.a externals/libthrowtheswitch.a
	avr-gcc $(CFLAGSatmega4809) -o $@ $^

$(outavrfirmware): $(builddir)%: avr/% | $(builddir)
	cp $^ $@
