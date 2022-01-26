extcfiles := externals/CException.c
ifeq ($(platform),native)
  extcfiles := $(extcfiles) externals/cmock.c externals/unity.c
endif

extobjs := $(addsuffix .o,$(basename $(extcfiles)))
allobjs := $(allobjs) $(extobjs)
alllibs := $(alllibs) externals/libthrowtheswitch.a

externals/libthrowtheswitch.a: $(extobjs)
	$(AR) rcs $@ $^
