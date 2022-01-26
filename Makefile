
ifndef platform
  platform = native
  export platform
endif

ifndef build_type
  build_type = debug
  export build_type
endif

ifndef CC
  CC = gcc
  export CC
endif
ifeq ($(CC),cc)
  CC = gcc
  export CC
endif

target_type := $(platform)_$(CC)_$(build_type)
$(info target_type is ${target_type})

builddir := build/$(target_type)/
$(info builddir is ${builddir})

.PHONY: all
all: all_later

include flags.mk

srcobjs := circular_buffer.o ascii_serial_com.o ascii_serial_com_device.o ascii_serial_com_register_block.o ascii_serial_com_register_pointers.o \
			 crc_16_dnp_crcmod.o crc_16_dnp_bbb.o crc_16_dnp_bbf.o crc_16_dnp_tbl4bit.o millisec_timer.o
ifeq ($(platform),native)
  srcobjs := $(srcobjs) circular_buffer_io_fd_poll.o
endif
allobjs := $(allobjs) $(srcobjs)
#exes := $(exes)
alllibs := $(alllibs) libasciiserialcom.a

libasciiserialcom.a: $(srcobjs)
	$(AR) rcs $@ $^

include externals/Makefile
ifeq ($(platform),native)
  include native/Makefile
  include unit_test/Makefile
endif
include avr/Makefile
include arm/Makefile

$(info alllibs is ${alllibs})
$(info exes is ${exes})
$(info allobjs is ${allobjs})
$(info testexes is ${testexes})
$(info avrfirmware is ${avrfirmware})
$(info armfirmware is ${armfirmware})

.PHONY: all_later
all_later: $(alllibs) $(exes) $(testexes) $(avrfirmware) $(armfirmware)

$(allobjs): %.o: %.c

$(exes): %: %.o $(alllibs)

######################################

outexes := $(addprefix $(builddir),$(notdir $(exes)))
outtestexes := $(addprefix $(builddir),$(notdir $(testexes)))

$(builddir):
	mkdir -p $(builddir)

$(outexes): $(builddir)%: native/% | $(builddir)
	cp $^ $(builddir)

$(outtestexes): $(builddir)%: unit_test/% | $(builddir)
	cp $^ $(builddir)

$(builddir)/libasciiserialcom.a: libasciiserialcom.a | $(builddir)
	cp $^ $@

$(builddir)/libthrowtheswitch.a: externals/libthrowtheswitch.a | $(builddir)
	cp $^ $@

.PHONY: install
install: $(builddir)/libthrowtheswitch.a $(builddir)/libasciiserialcom.a $(outtestexes) $(outexes) $(outavrfirmware) $(outarmfirmware) all

######################################

runouttestexes := $(addsuffix _runTest,$(outtestexes))

.PHONY: test
test: $(runouttestexes)

.PHONY: $(runouttestexes)
$(runouttestexes): %_runTest: %
	$(abspath $*)

######################################

allgcno := $(addsuffix .gcno, $(basename $(allobjs)))
allgcda := $(addsuffix .gcda, $(basename $(allobjs)))
#allgcov := $(addsuffix .gcov, $(basename $(allobjs)))

.PHONY: clean
clean:
	rm -rf $(allobjs)
	rm -rf $(alllibs)
	rm -rf $(exes)
	rm -rf $(testexes)
	rm -rf $(avrfirmware)
	rm -rf $(armfirmware)
	rm -rf $(allgcno)
	rm -rf $(allgcda)
	#rm -rf $(allgcov)
	rm -rf *.d
	rm -rf arm/*.d
	rm -rf avr/*.d
	rm -rf arm/generated.*
