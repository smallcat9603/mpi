CC:=gcc
CPPFLAGS:=-I. -g
C_SRCS:=stack_sig.c


ifneq ($(shell which blrts_xlc 2> /dev/null),)
  CPPFLAGS:=-D__blrts__
  endif

ifneq ($(shell which mpixlc 2> /dev/null),)
  CPPFLAGS:=-Dmpixlc
  CPPFLAGS+=-Iunwind/libunwind-1.1/include
  CPPFLAGS+= unwind/libunwind-1.1/src/.libs/libunwind.a
  endif


OBJS:=$(C_SRCS:.c=.o)

libstackwalk.a: $(OBJS)
	$(AR) -rv $@ $(OBJS)

clean:
	$(RM) $(OBJS) libstackwalk.a *.d

DEPCC:= gcc -MM
%.d: %.c 
	@$(SHELL) -ec '$(DEPCC) $(CPPFLAGS) $< \
    | sed '\''s/\($*\)\.o[ :]*/\1.o $@ : /g'\'' > $@; \
    [ -s $@ ] || rm -f $@'

ifneq ($(MAKECMDGOALS),clean)
  include $(C_SRCS:.c=.d) $(CXX_SRCS:.C=.D)
endif
