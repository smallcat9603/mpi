ALL:= replay
RECORDLIB=glob
DEBUGGING=n
IO_TIMER=y

#####################################
# Options
#####################################
HISTO_REPLAY=n

RAND_DEST=6

PREALLOC_MEM=y



include ../record/config/Makefile.config

top_srcdir=$(shell pwd)/..

VPATH+=${top_srcdir}/record/common/src

GCC=$(CC)
GXX=$(CXX)
MPI_CC = $(MPICC)
MPI_CXX = $(MPICXX)

SRCS:= mpi_util.c \
 
CXX_SRCS:= \
	Event.C \
	Ranklist.C \
	Param.C \
	ParamValue.C \
	Trace.C \
	StackSig.C \
	Loop.C \
	PtrHandler.C \
	ReqHandler.C \
	HistoBin.C \
	Histogram.C \
	ValueSet.C \
	Stat.C \
	StatTime.C \
	replay.C \
	op_exec.C \
	mem_handler.C \
	context.C \
	histo_exec.C \
	handle_queue.C \
	RandMnger.C \
	Timer.C \
	Cluster.C



SW_HOME=${top_srcdir}/stackwalk/ver0
RECORD_HOME=${top_srcdir}/record/lib



#CFLAGS+=-g -I$(MPICH_DIR)/include -L$(MPICH_DIR)/lib
CFLAGS+=-DMPICH_IGNORE_CXX_SEEK
#CFLAGS+=-DFEATURE_REPLAY -DFEATURE_FILE_IO -DFEATURE_CODE_GENERATOR
CFLAGS+=-I.
CFLAGS+=-I${top_srcdir}/stackwalk/ver0
CFLAGS+=-I${top_srcdir}/record/common/inc 
CFLAGS+=-I${top_srcdir}/record/inc 

LDFLAGS += -g -L$(SW_HOME)

LDLIBS += -lm
LDLIBS += -lstackwalk

ifeq ($(ALL),record)
LDFLAGS += -L$(RECORD_HOME)
LDLIBS += -l$(RECORDLIB)
ifeq ($(RECORDLIB),node)
CFLAGS+=-DCOMPRESS_INTRA_NODE
endif
ifeq ($(RECORDLIB),glob)
CFLAGS+=-DCOMPRESS_CROSS_NODE -DCOMPRESS_INTRA_NODE
endif
endif

ifeq ($(HISTO_REPLAY),y)
CFLAGS+=-DFEATURE_HISTO_REPLAY
endif

ifeq ($(DEBUGGING),y)
CFLAGS+=-DDEBUG
endif

#define whether use I/O timer 
ifeq ($(IO_TIMER), y)
CFLAGS+=-DIOTIMER
endif

ifeq ($(PREALLOC_MEM),y)
CFLAGS+=-DFEATURE_PREALLOC_MEM
endif

CFLAGS+=-DFEATURE_RAND_DEST=$(RAND_DEST)

MPICFLAGS+=-Wall -std=gnu99 -O0 -g 
MPICFLAGS+=$(CFLAGS)

MPICPPFLAGS+=-Wall -O0 -g 
MPICPPFLAGS+=$(CFLAGS)

OBJS = $(SRCS:.c=.o)
CXX_OBJS = $(CXX_SRCS:.C=.o)


all: $(ALL)

replay: $(OBJS) $(CXX_OBJS)
	$(MPI_CXX) $(LDFLAGS) -o $@ $(OBJS) $(CXX_OBJS) $(LDLIBS)

record: $(OBJS) $(CXX_OBJS)
	$(MPI_CXX) $(LDFLAGS) $(LDLIBS) -o $@ $(OBJS) $(CXX_OBJS) $(LDLIBS)

%.o: %.c
	$(MPI_CC) $(MPICFLAGS) -c -o $@ $<

%.o: %.C
	$(MPI_CXX) $(MPICPPFLAGS) -c -o $@ $<

%.d: %.c
	@$(SHELL) -ec '$(GCC) -MM $(CFLAGS) $< \
    | sed '\''s/\($*\)\.o[ :]*/\1.o $@ : /g'\'' > $@; \
    [ -s $@ ] || rm -f $@'


%.D: %.C
	@$(SHELL) -ec '$(GXX) -MM $(CFLAGS) $< \
    | sed '\''s/\($*\)\.o[ :]*/\1.o $@ : /g'\'' > $@; \
    [ -s $@ ] || rm -f $@'
ifneq ($(MAKECMDGOALS),clean)
-include $(SRCS:.c=.d)
-include $(CXX_SRCS:.C=.D)
endif

CXX_OBJCS:= \
	Event.o \
	Ranklist.o \
	Param.o \
	ParamValue.o \
	Trace.o \
	StackSig.o \
	Loop.o \
	PtrHandler.o \
	ReqHandler.o \
	HistoBin.o \
	Histogram.o \
	ValueSet.o \
	Stat.o \
	StatTime.o \
	op_exec.o \
	histo_exec.o \
	mem_handler.o \
	context.o \
	handle_queue.o \
	RandMnger.o \
	Timer.o \
	Cluster.o

make install:
	ar -rv libscalatrace.a $(OBJS) $(CXX_OBJCS)

run:
	mpirun -np 8 ./$(ALL)

clean:
	rm -f record replay *.o *~ *.d *.D *.a

veryclean: clean
	rm -rf trace_*

.PHONY: clean
