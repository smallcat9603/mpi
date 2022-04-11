
include ../../record/config/Makefile.config

SCALATRACE_HOME=../../

MPI_CC:=$(MPICXX)

MPI_CC:=CC
CFLAGS:=-Wall -g -O0

LDFLAGS:=-L${SCALATRACE_HOME}/record/lib -L${SCALATRACE_HOME}/stackwalk/ver0
LIBS:=-lglob -lstackwalk -Wl,-wrap=open -Wl,-wrap=write -Wl,-wrap=read -Wl,-wrap=open64 \
 -Wl,-wrap=creat -Wl,-wrap=close -Wl,-wrap=lseek -Wl,-wrap=lseek64 -Wl,-wrap=remove \
  -Wl,-wrap=fopen -Wl,-wrap=fclose -Wl,-wrap=fread -Wl,-wrap=fwrite -Wl,-wrap=fseek

#LDFLAGS:= -I/opt/openmpi/include 
#LIBS:=/opt/openmpi/lib/libmpi.a /opt/openmpi/lib/libmpi_cxx.a 
#LIBS+= -Wl,-wrap,open -Wl,-wrap,open64 -static -lm


TARGET=io-sample
SOURCE=$(addsuffix .c, $(basename $(TARGET)))

all: ${TARGET}

${TARGET} : ${SOURCE}
	$(MPI_CC) $(CFLAGS) $(LDFLAGS) ${SOURCE} -o ${TARGET} $(LIBS)

clean: 
	rm -rf *.o *~ ${TARGET} 

veryclean: clean
	rm -rf trace_*

