ALL:= extrap
RECORDLIB=glob

#####################################
# Options
#####################################
HISTO_REPLAY=y

RAND_DEST=6

PREALLOC_MEM=y

DROPEVENT=n




top_srcdir=$(shell pwd)/..

VPATH+=${top_srcdir}/record/common/src

GCC=gcc
GXX=g++
MPI_CC = mpicc
MPI_CXX = mpicxx

 
CXX_SRCS:= extrap.C \
	   expnode.C \
	   rsd_node.C \
	   vmaster.C \
	   eutil.C \
	   exparam.C \
	   rsd_hlink.C 


SW_HOME=${top_srcdir}/stackwalk/ver0
RECORD_HOME=${top_srcdir}/record/lib



CFLAGS+=-g
CFLAGS+=-DMPICH_IGNORE_CXX_SEEK
CFLAGS+=-I.
CFLAGS+=-I${top_srcdir}/stackwalk/ver0
CFLAGS+=-I${top_srcdir}/record/common/inc 
CFLAGS+=-I${top_srcdir}/replay
CFLAGS+=-I${top_srcdir}/record/libsrc/glob

LDFLAGS += -g -L$(SW_HOME)
LDFLAGS += -L${RECORD_HOME}
LDFLAGS += -L${top_srcdir}/replay


LDLIBS =
LDLIBS += -lm -L${top_srcdir}/stackwalk/ver0 -lglob -lstackwalk
LDLIBS += -lscalatrace


ifeq ($(HISTO_REPLAY),y)
CFLAGS+=-DFEATURE_HISTO_REPLAY
endif

ifeq ($(DEBUGGING),y)
CFLAGS+=-DDEBUG
endif


ifeq ($(PREALLOC_MEM),y)
CFLAGS+=-DFEATURE_PREALLOC_MEM
endif

ifeq ($(DROPEVENT),y)
CFLAGS+=-DDROPEVENT
endif

CFLAGS+=-DFEATURE_RAND_DEST=$(RAND_DEST)

MPICFLAGS+=-Wall -std=gnu99 -O0 -g 
MPICFLAGS+=$(CFLAGS)

MPICPPFLAGS+=-Wall -O0 -g 
MPICPPFLAGS+=$(CFLAGS)

CXX_OBJS = $(CXX_SRCS:.C=.o)


all: $(ALL)

extrap: $(OBJS) $(CXX_OBJS)
	$(MPI_CXX) $(LDFLAGS) -o $@ $(CXX_OBJS) $(LDLIBS)



%.o: %.C
	$(MPI_CXX) $(MPICPPFLAGS) -c -o $@ $<


ifneq ($(MAKECMDGOALS),clean)
-include $(SRCS:.c=.d)
-include $(CXX_SRCS:.C=.D)
endif


run:
	mpirun -np 8 ./$(ALL)

clean:
	rm -f extrap *.o *~ 


.PHONY: clean
