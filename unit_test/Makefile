testcfiles := $(wildcard unit_test/*.c)
testobjs := $(addsuffix .o,$(basename $(testcfiles)))
testexes := $(basename $(testcfiles))

ifeq ($(platform),native)
  allobjs := $(allobjs) $(testobjs)
endif

$(testexes): %: %.o libasciiserialcom.a externals/libthrowtheswitch.a
	$(CC) $(CFLAGS) -o $@ $^
