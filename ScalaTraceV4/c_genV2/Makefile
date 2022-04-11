top_srcdir		= $(shell pwd)

GPROF:=

USE_HISTO:=y
USE_BITS:=n

USE_REC_MERGE_TRACE:=y

GCC=gcc
GXX=g++
ifeq ($(shell uname), AIX)
  MPI_CC  = mpcc
  MPI_CXX = mpCC
  MPIP_CC = mpcc
  MPICFLAGS   = -qlanglvl=extc99 -O0 -g -DAIX
  CFLAGS = -I/usr/lpp/ppe.poe/include
  else
ifneq ($(shell which blrts_xlc 2> /dev/null),)
  platform=BGL
  MPI_CC=		mpixlc
  MPI_CXX = mpixlcxx
  MPIP_CC=	mpixlc
  MPICFLAGS   = -qlanglvl=extc99 -O0 -DHAVE_RTS_GETTIMEBASE
  LDFLAGS=-lrts.rts

  else
  MPI_CC = mpicc
  MPI_CXX = mpicxx
  MPIP_CC = cc

  MPICFLAGS		= -std=gnu99 -O0 #-Wall
  MPICPPFLAGS		= -O0 #-Wall
  LDLIBS = -lrt
endif
endif

CFLAGS+=-DMPICH_IGNORE_CXX_SEEK
MPICPPFLAGS+=-DMPICH_IGNORE_CXX_SEEK

#CFLAGS+= 	-g -DFEATURE_REPLAY -DFEATURE_FILE_IO -DFEATURE_CODE_GENERATOR
CFLAGS+= 	-g -DFEATURE_REPLAY -DFEATURE_CODE_GENERATOR
ifeq ($(USE_HISTO),y)
CFLAGS+= -DFEATURE_TIMING_HISTO 
endif
ifeq ($(USE_BITS),y)
CFLAGS+= -DFEATURE_TIMING_BITS
endif

ifeq ($(USE_REC_MERGE_TRACE),y)
CFLAGS+= -DREC_MERGE_TRACE
endif

CFLAGS+= -DLINK_WITH_RECORD

MPIPLDFLAGS=   -L/${HOME}/opt/lib
#MPIPLDFLAGS=   -L/${HOME}/tools/mpiP-3.1.0  
#LDFLAGS+=  -L/${HOME}/opt/efence

MPIPLDFLAGS += -g
LDFLAGS += -g -L../record/lib -L../stackwalk/ver0 -L../replay

#MPIPLDLIBS =     -lmpiP
#MPIPLDLIBS =     -lmpiP-mpich
#MPIPLDLIBS +=    -lunwind
LDLIBS +=     -lm
#LDLIBS +=    -lefence

RECLIB=glob
RECLDFLAGS += -L../record/lib
RECLDLIBS = -l${RECLIB}

LDFLAGS+=$(GPROF)

ALL:= generator

all: $(ALL)

#TODO: Use VPATH instead of absolute path
RECORD_SRC=../record/common/src
REPLAY_SRC=../replay/
GEN_SRCS:= code_generator.C \
	code_generator_handler.C \
	generate_utils.C \
	op_generator.C \
	$(RECORD_SRC)/Event.C \
	$(RECORD_SRC)/Ranklist.C \
	$(RECORD_SRC)/Param.C \
	$(RECORD_SRC)/ParamValue.C \
	$(RECORD_SRC)/Trace.C \
	$(RECORD_SRC)/StackSig.C \
	$(RECORD_SRC)/Loop.C \
	$(RECORD_SRC)/Histogram.C \
	$(RECORD_SRC)/HistoBin.C \
	$(RECORD_SRC)/ValueSet.C \
	$(RECORD_SRC)/Stat.C \
 	$(RECORD_SRC)/StatTime.C \
	$(RECORD_SRC)/RandMnger.C \
	$(RECORD_SRC)/PtrHandler.C \
	$(RECORD_SRC)/ReqHandler.C \
	$(REPLAY_SRC)/Timer.C \


CFLAGS+=-I.
 
# include histogram src and header
ifeq ($(USE_HISTO),y)
#COMMON_CXX_SRCS += Histogram.C
#COMMON_CXX_SRCS += Histogram.C BitVector.C TimeList.C Timing.C
#SRCS+=normal_dist.c
endif

ifeq ($(USE_BITS),y)
  COMMON_CXX_SRCS+=Intseq.C
  endif

#COMMON_CXX_SRCS+=VParam.C RList.C RankSet.C stack_sig.C cvector.C

# This is used to select between the wisconsin and the older stackwalk libs
USE_WISC=n

ifeq ($(USE_WISC),y)
SW_HOME=`pwd`/../record/core
SW_PLATFORM=x86_64-unknown-linux2.4

SW_INCLUDE = -I"$(SW_HOME)"
SW_LIBS = -L$(SW_HOME)/$(SW_PLATFORM)/lib -lstackwalk -lcommon -lsymtabAPI -liberty

CFLAGS += -DWISCONSIN_WALKER  $(SW_INCLUDE)

else

# Not using wisconsin walker
CFLAGS += -I../stackwalk/ver0

SW_HOME=${top_srcdir}/../stackwalk/ver0
SW_LIBS = -L$(SW_HOME) -lstackwalk -lglob 
SW_INCLUDE = -I$(SW_HOME)

endif
LDLIBS+=$(SW_LIBS)

VPATH+=../record/common/src ../record/libsrc
CFLAGS+=-I../record/common/inc -I../record/inc -I../record/libsrc -I../replay -I../record/libsrc/glob
MPICFLAGS+=$(CFLAGS)
MPICFLAGS+=$(GPROF)
MPICPPFLAGS+=$(CFLAGS)

OBJS = $(SRCS:.c=.o)

COMMON_OBJS = $(COMMON_SRCS:.c=.o) $(COMMON_CXX_SRCS:.C=.o)

GEN_OBJS = $(GEN_SRCS:.c=.o)

# these lines are adapted from the GNU make manual :-
# 4.14 Generating Prerequisites Automatically
%.d: %.c
	@$(SHELL) -ec '$(GCC) -MM $(CFLAGS) $< \
    | sed '\''s/\($*\)\.o[ :]*/\1.o $@ : /g'\'' > $@; \
    [ -s $@ ] || rm -f $@'

%.D: %.C
	@$(SHELL) -ec '$(GXX) -MM $(CFLAGS) $< \
    | sed '\''s/\($*\)\.o[ :]*/\1.o $@ : /g'\'' > $@; \
    [ -s $@ ] || rm -f $@'

#%.o: %.c
#	$(GCC) $(CFLAGS) -c -o $@ $<

#%.o: %.C
#	$(GXX) $(CFLAGS) -c -o $@ $<
%.o: %.c
	$(MPI_CC) $(MPICFLAGS) -c -o $@ $<

%.o: %.C
	$(MPI_CXX) $(MPICPPFLAGS) -c -o $@ $<

generator: $(GEN_OBJS) $(COMMON_OBJS)
	$(MPI_CXX) $(MPICPPFLAGS) $(LDFLAGS) -o $@ $(GEN_OBJS) $(COMMON_OBJS) $(LDLIBS)

#replay: $(OBJS) $(COMMON_OBJS)
#	$(MPI_CXX) $(LDFLAGS) -o $@ $(OBJS) $(COMMON_OBJS) $(LDLIBS)

# build to replay with mpip
#mpip_replay: $(OBJS) $(COMMON_OBJS)
#	$(MPI_CXX) $(MPIPLDFLAGS) $(LDFLAGS) -o $@ $(OBJS) $(COMMON_OBJS) $(MPIPLDLIBS) $(LDLIBS)

# build to replay with umpire itself!
#ump_replay: $(OBJS) ../../record/lib/lib${RECLIB}.a
#	$(MPI_CXX) $(RECLDFLAGS) $(LDFLAGS) -o $@ $(OBJS) $(RECLDLIBS) $(LDLIBS)

ifneq ($(MAKECMDGOALS),clean)
-include $(SRCS:.c=.d)
-include $(COMMON_CXX_SRCS:.C=.D)
endif

# utils

clean:
	rm -f $(ALL) *.o *~ *.mpiP *.d *.D

.PHONY: clean
