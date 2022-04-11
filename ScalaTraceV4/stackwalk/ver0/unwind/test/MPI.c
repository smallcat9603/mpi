/* First time run the result without filesopen() function
 * Second time run the result with filesopen() function
 * */
#define UNW_LOCAL_ONLY
#include "mpi.h"
#include <stdio.h>
#include <stdlib.h>
#include <walker.h>
#include <libunwind.h>

int MPI_File_open(MPI_Comm comm,
                  char *filename,
                  int amode,
                  MPI_Info info,
  MPI_File *mpi_fh){
        int ret = PMPI_File_open(comm, filename, amode, info, mpi_fh);
        unw_cursor_t cursor; unw_context_t uc;
        unw_word_t ip, sp;

        unw_getcontext(&uc);
        unw_init_local(&cursor, &uc);
        while (unw_step(&cursor) > 0) {
          unw_get_reg(&cursor, UNW_REG_IP, &ip);
          unw_get_reg(&cursor, UNW_REG_SP, &sp);
          printf ("ip = %lx, sp = %lx\n", (long) ip, (long) sp);
        }
    intptr_t * tmp = do_stack_unwind();
     int i = 0;
    while (tmp[i]) {
      printf ("%x ", tmp[i]);
      i++;
    }
    printf ("\n");

    return 0;
}

void filesopen(){
  MPI_File cFile;
  MPI_File_open( MPI_COMM_WORLD, "temp", MPI_MODE_RDWR | MPI_MODE_DELETE_ON_CLOSE | MPI_MODE_CREATE, MPI_INFO_NULL, &cFile );
  MPI_File_close(&cFile);
	
}

int main( int argc, char *argv[] )
{
  int rank;
  MPI_File cFile;

  MPI_Init( &argc, &argv );
  MPI_Comm_rank( MPI_COMM_WORLD, &rank );
  filesopen();
  printf("\n\n"); 
  MPI_File_open( MPI_COMM_WORLD, "temp1", MPI_MODE_RDWR | MPI_MODE_DELETE_ON_CLOSE | MPI_MODE_CREATE, MPI_INFO_NULL, &cFile );
  MPI_File_close(&cFile);

  MPI_Finalize();
  return 0;
}

