#!/bin/sh
#------ pjsub option --------#
#PJM -N "test1-mpi"
#PJM -L rscgrp=coupler-lec-a
#PJM -L node=1
#PJM --mpi proc=2
#PJM -L elapse=00:05:00
#PJM -g gt00
#PJM -j
#------- Program execution -------#

module purge
module load intel
module load impi
module load waitio

export WAITIO_MASTER_PORT=7100
export WAITIO_PBID=1
export WAITIO_NPB=2
export WAITIO_MASTER_HOST=`waitio-serv -c`

mpiexec -n 2  ./test-mpi-intel

exit
