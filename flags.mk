
$(info CC is ${CC})
$(info build_type is ${build_type})
$(info platform is ${platform} )

GCCFLAGS=-Werror -Wall -Wextra
GCCFLAGS+=-Wdouble-promotion -Wformat=2 -Wformat-signedness -Winit-self -Wmissing-include-dirs -Wswitch-default -Wswitch-enum -Wuninitialized -Wtrampolines -Wfloat-equal -Wshadow -Wundef -Wbad-function-cast

CLANGFLAGS=-Werror -Wall -Wextra
CLANGFLAGS+=-Wdouble-promotion -Winit-self -Wmissing-include-dirs -Wswitch-default -Wswitch-enum -Wuninitialized -Wfloat-equal -Wshadow -Wundef -Wbad-function-cast

ifeq ($(platform),native)
  ifneq (,$(findstring gcc,$(CC)))
    CFLAGS=$(GCCFLAGS)
    CFLAGS+= -std=gnu18 -Wformat-overflow=2 -Wformat-truncation=2 -Wnull-dereference -Walloc-zero -Wduplicated-branches -Wduplicated-cond
    CXXFLAGS=$(GCCFLAGS) -std=gnu++17 -Wsuggest-override -Wplacement-new=2

    ifeq ($(build_type),debug)
      CFLAGS+=-g -Og
      CFLAGS+=-fsanitize=address -fsanitize=leak -fsanitize=undefined -fsanitize=float-divide-by-zero -fsanitize=float-cast-overflow -fsanitize-address-use-after-scope -fstack-protector-all #-fsanitize=pointer-compare -fsanitize=pointer-subtract
  	  ifdef coverage
  	    CFLAGS+=--coverage
  	  endif
    else
      CFLAGS+=-O2 -flto -Wstrict-aliasing -fstrict-aliasing
    endif

    ifeq ($(build_type),profile)
      CFLAGS+=-g
    endif
  endif
  ifneq (,$(findstring clang,$(CC)))
    CFLAGS=$(CLANGFLAGS) -std=gnu18
    CXXFLAGS=$(CLANGFLAGS) -std=gnu++14 -Wsuggest-override -Wplacement-new=2

    ifeq ($(build_type),debug)
      CFLAGS+=-g -Og -fsanitize=address -fsanitize=leak -fsanitize=undefined -fsanitize=float-divide-by-zero -fsanitize=float-cast-overflow -fsanitize-address-use-after-scope -fstack-protector-all #-fsanitize=pointer-compare -fsanitize=pointer-subtract
  	  ifdef coverage
  	    CFLAGS+=--coverage
  	  endif
    else
      CFLAGS+=-O2
    endif

    ifeq ($(build_type),profile)
      CFLAGS+=-g
    endif
  endif
endif

###############################################

ifneq (,$(findstring avr,$(platform)))
  CC=avr-gcc
  CFLAGS=$(GCCFLAGS) -std=c18 -mmcu=$(platform)
  CXXFLAGS=$(GCCFLAGS) -std=c++17 -Wsuggest-override -Wplacement-new=2 -mmcu=$(platform)
  ifeq ($(build_type),debug)
    CFLAGS+=-g -Og
  else
    CFLAGS+=-Os -flto -Wstrict-aliasing -fstrict-aliasing
  endif
endif

ifneq (,$(findstring cortex,$(platform)))
  CC=arm-none-eabi-gcc
  CFLAGS=$(GCCFLAGS) -std=c18 -mcpu=$(platform) -mthumb -Itools/libopencm3/include -MD -static -nostartfiles -Wl,--start-group -lc -lgcc -lnosys -Wl,--end-group
  CXXFLAGS=$(GCCFLAGS) -std=c++17 -Wsuggest-override -Wplacement-new=2 -mcpu=$(platform) -mthumb -Itools/libopencm3/include
  ifeq ($(build_type),debug)
    CFLAGS+=-g -Og
  else
    CFLAGS+=-Os -flto -Wstrict-aliasing -fstrict-aliasing
  endif
  LDFLAGS+=-Ltools/libopencm3/lib
endif

ifneq (,$(findstring rv32i,$(platform)))
  CC=riscv32-unknown-elf-gcc
  CFLAGS=$(filter-out -Werror,$(GCCFLAGS)) -std=gnu18 -march=$(platform) -mabi=ilp32 -ffunction-sections -fdata-sections -nostartfiles -mno-fdiv -Wl,--gc-sections -lm -lc -lgcc -lc
  # only needed for c-extension
  CFLAGS+= -falign-functions=4 -falign-labels=4 -falign-loops=4 -falign-jumps=4
  # define NEORV32 flag for my header/library use
  CFLAGS+= -DNEORV32=1
  ifeq ($(build_type),debug)
    CFLAGS+=-g -Og
  else
    CFLAGS+=-Os -flto -Wstrict-aliasing -fstrict-aliasing
  endif
  LDFLAGS+=-Ltools/libopencm3/lib
endif

###############################################

LDFLAGS+=$(CFLAGS)
CFLAGS+=-I$(shell pwd)

OPENCM3_DIR=tools/libopencm3
OPENCM3_DEVICES_DATA=$(OPENCM3_DIR)/ld/devices.data
