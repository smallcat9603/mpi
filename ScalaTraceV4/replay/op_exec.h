/**
 * Author: Xing Wu
 */

#ifndef __OP_EXEC_H__
#define __OP_EXEC_H__

#include "PtrHandler.h"
#include "Timer.h"
#include "replay.h"
#include "Event.h"
#include "umpi_mpi_ops.h"
#include "ReqHandler.h"
#include "mem_handler.h"
#include "ParamValue.h"
#include <sys/time.h>
#include <map>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

using namespace std;

class Event;

struct Fileopen{
	MPI_Comm comm;
	char ch[256];
	int amode;
	MPI_Info info;
	MPI_Offset offset;
	MPI_File fh;
};

struct readwriteend{
	MPI_File fh;
	int op_code;
};
extern ptr_handler_t replay_ptr;

void r_w_end_insert(int indexfh, MPI_File fh, int op_code);

void switch_event(int op_code, Event *e, Timer *timer);

int replay_event (Event *e, int depth, int iteration);

/* replay functions for individual MPI functions */
/* collectives */
void replay_Allgather(Event *e, Timer *timer);
void replay_Allgatherv(Event *e, Timer *timer);
void replay_Allreduce(Event *e, Timer *timer);
void replay_Alltoall(Event *e, Timer *timer);
void replay_Alltoallv(Event *e, Timer *timer);
void replay_Barrier(Event *e, Timer *timer);
void replay_Bcast(Event *e, Timer *timer);
void replay_Gather(Event *e, Timer *timer);
void replay_Gatherv(Event *e, Timer *timer);
void replay_Reduce(Event *e, Timer *timer);
void replay_Reduce_scatter(Event *e, Timer *timer);
void replay_Scatter(Event *e, Timer *timer);
void replay_Scatterv(Event *e, Timer *timer);

/* communicator ops */
MPI_Comm replay_Comm_create(Event *e, Timer *timer);
MPI_Comm replay_Comm_dup(Event *e, Timer *timer);
void replay_Comm_free(Event *e, Timer *timer);
MPI_Comm replay_Comm_split(Event *e, Timer *timer);
int replay_cart_create(Event *e, Timer *timer);
int replay_type_contiguous(Event *e, Timer *timer);
int replay_type_create_subarray(Event *e, Timer *timer);
int replay_type_create_darray(Event *e, Timer *timer);
int replay_type_free(Event *e, Timer *timer);

/* group ops*/
MPI_Group replay_Comm_group(Event *e, Timer *timer);
MPI_Group replay_Group_difference(Event *e, Timer *timer);
MPI_Group replay_Group_excl(Event *e, Timer *timer);
void replay_Group_free(Event *e, Timer *timer);
MPI_Group replay_Group_incl(Event *e, Timer *timer);
MPI_Group replay_Group_intersection(Event *e, Timer *timer);
MPI_Group replay_Group_range_excl(Event *e, Timer *timer);
MPI_Group replay_Group_range_incl(Event *e, Timer *timer);
MPI_Group replay_Group_union(Event *e, Timer *timer);

/*io ops*/
int replay_File_open(Event *e, Timer *timer);
int replay_File_close(Event *e, Timer *timer);
int replay_File_delete(Event *e, Timer *timer);
int replay_File_seek(Event *e, Timer *timer);
int replay_File_set_view(Event *e, Timer *timer);
int replay_File_seek_shared(Event *e, Timer *timer);
int replay_File_set_size(Event *e, Timer *timer);
int replay_File_preallocate(Event *e, Timer *timer);
int replay_File_sync(Event *e, Timer *timer);
int replay_File_set_atomicity(Event *e, Timer *timer);
int replay_File_c2f(Event *e, Timer *timer);
int replay_File_read_write(Event *e, Timer *timer, int op_code);
int replay_File_read_write_at(Event *e, Timer *timer, int op_code);
int replay_File_read_write_end(Event *e, Timer *timer, int op_code);
int replay_File_read_at_all_end(Event *e, Timer *timer);
int replay_File_write_at_all_end(Event *e, Timer *timer);
int replay_File_read_all_end(Event *e, Timer *timer);
int replay_File_write_all_end(Event *e, Timer *timer);
int replay_File_read_ordered_end(Event *e, Timer *timer);
int replay_File_write_ordered_end(Event *e, Timer *timer);
char* replay_open(Event *e, int PTYPE);
ssize_t replay_read_write(Event *e, Timer *timer, int op_code);
int replay_close(Event *e, Timer *timer);
off_t replay_lseek(Event *e, Timer *timer, int op_code);
int replay_creat(Event *e, Timer *timer);
int replay_dup(Event *e, Timer *timer, int op_code);
int replay_fcntl(Event *e, Timer *timer);
int replay_fclose(Event *e, Timer *timer);
int replay_fread_fwrite(Event *e, Timer *timer);
#endif /*__OP_EXEC_H__*/

