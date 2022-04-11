/**
 * Author: Xing Wu
 */
#include "op_exec.h"
#include <fstream>
#include <iostream>

#define RECORDED_MPI_REQUEST_NULL -1

extern int my_rank, my_size;
map<int, Fileopen> file_open;
multimap<int, readwriteend> r_w_end;
vector<int> rank_sort;
/*#define MAXSIZE 536870912
void *buf = (void*)malloc(MAXSIZE);*/
extern void* buf;


ptr_handler_t replay_ptr = NULL;
req_handler_t replay_req = NULL;

Timer *replay_timer;


void r_w_end_insert(int indexfh, MPI_File fh, int op_code){
	readwriteend temp;
	temp.fh = fh;
	temp.op_code = op_code;

	pair<int, readwriteend> m(indexfh, temp);
	r_w_end.insert(m);
}
/* collectives */
void replay_Allgather(Event *e, Timer *timer){
	MPI_Comm comm;
	MPI_Datatype datatype, recvtype;
	int datatype_size, recvtype_size, count, recvcount, ret, comm_size;
	void *sendbuf, *recvbuf;

	comm = (MPI_Comm)index_to_comm (replay_ptr, e->getParamValueForRank(COMM, my_rank));
	MPI_Comm_size(comm, &comm_size);

	datatype = index_to_type (replay_ptr, e->getParamValueForRank(DATATYPE, my_rank));
	if(MPI_Type_size(datatype, &datatype_size) != MPI_SUCCESS) {
		cerr << "MPI_Allgather error: MPI_Type_size failed - probably invalid type: " 
				<< e->getParamValueForRank(DATATYPE, my_rank) << endl;
		exit(0);
	}
	recvtype = index_to_type (replay_ptr, e->getParamValueForRank(RECVTYPE, my_rank));
	if(MPI_Type_size(recvtype, &recvtype_size) != MPI_SUCCESS) {
		cerr << "MPI_Allgather error: MPI_Type_size failed - probably invalid type: " 
				<< e->getParamValueForRank(RECVTYPE, my_rank) << endl;
		exit(0);
	}

	count = e->getParamValueForRank(COUNT, my_rank);
	sendbuf = malloc(count * datatype_size);

	recvcount = e->getParamValueForRank(RECVCOUNT, my_rank);
	recvbuf = malloc(recvcount * recvtype_size * comm_size);

	if(!sendbuf || !recvbuf) {
		cerr << "MPI_Allgather error: cannot malloc buffers" << endl;
		exit(0);
	}

	/* MPI call */
	if(timer)
		timer->simulateComputeTime();
	ret = MPI_Allgather(sendbuf, count, datatype, recvbuf, recvcount, recvtype, comm);
	if(timer)
		timer->setTime();
	if(ret != MPI_SUCCESS) {
		cerr << "error: MPI_Allgather did not return MPI_SUCCESS" << endl;
		exit(0);
	}

	if(sendbuf)
		free(sendbuf);
	if(recvbuf)
		free(recvbuf);
}

void replay_Allreduce(Event *e, Timer *timer){
	MPI_Comm comm;
	MPI_Datatype datatype;
	MPI_Op mpi_op;
	int datatype_size, count, ret;
	void *sendbuf, *recvbuf;

	comm = (MPI_Comm)index_to_comm (replay_ptr, e->getParamValueForRank(COMM, my_rank));

	datatype = index_to_type (replay_ptr, e->getParamValueForRank(DATATYPE, my_rank));
	if(MPI_Type_size(datatype, &datatype_size) != MPI_SUCCESS) {
		cerr << "MPI_Allreduce error: MPI_Type_size failed - probably invalid type: " 
				<< e->getParamValueForRank(DATATYPE, my_rank) << endl;
		exit(0);
	}

	count = e->getParamValueForRank(COUNT, my_rank);

	recvbuf = malloc(count * datatype_size);
	sendbuf = malloc(count * datatype_size);
	if(!sendbuf || !recvbuf) {
		cerr << "MPI_Allreduce error: cannot malloc buffers" << endl;
		exit(0);
	}

	mpi_op = index_to_op (replay_ptr, e->getParamValueForRank(MPI_OP, my_rank) );

	if(timer)
		timer->simulateComputeTime();
	ret = MPI_Allreduce(sendbuf, recvbuf, count, datatype, mpi_op, comm);
	if(timer)
		timer->setTime();

	if(ret != MPI_SUCCESS) {
		cerr << "error: MPI_Allreduce did not return MPI_SUCCESS" << endl;
		exit(0);
	}

	if(sendbuf)
	    free(sendbuf);
	if(recvbuf)
	    free(recvbuf);
}

void replay_Alltoall(Event *e, Timer *timer){
	MPI_Comm comm;
	MPI_Datatype datatype, recvtype;
	int datatype_size, recvtype_size, count, recvcount, ret, comm_size;
	void *sendbuf, *recvbuf;

	comm = (MPI_Comm)index_to_comm (replay_ptr, e->getParamValueForRank(COMM, my_rank));
	MPI_Comm_size(comm, &comm_size);

	datatype = index_to_type (replay_ptr, e->getParamValueForRank(DATATYPE, my_rank));
	if(MPI_Type_size(datatype, &datatype_size) != MPI_SUCCESS) {
		cerr << "MPI_Alltoall error: MPI_Type_size failed - probably invalid type: " 
				<< e->getParamValueForRank(DATATYPE, my_rank) << endl;
		exit(0);
	}
	recvtype = index_to_type (replay_ptr, e->getParamValueForRank(RECVTYPE, my_rank));
	if(MPI_Type_size(recvtype, &recvtype_size) != MPI_SUCCESS) {
		cerr << "MPI_Alltoall error: MPI_Type_size failed - probably invalid type: " 
				<< e->getParamValueForRank(RECVTYPE, my_rank) << endl;
		exit(0);
	}

	count = e->getParamValueForRank(COUNT, my_rank);
	sendbuf = malloc(count * datatype_size * comm_size);

	recvcount = e->getParamValueForRank(RECVCOUNT, my_rank);
	recvbuf = malloc(recvcount * recvtype_size * comm_size);

	if(!sendbuf || !recvbuf) {
		cerr << "MPI_Alltoall error: cannot malloc buffers" << endl;
		exit(0);
	}

	/* MPI call */
	if(timer)
		timer->simulateComputeTime();
	ret = MPI_Alltoall(sendbuf, count, datatype, recvbuf, recvcount, recvtype, comm);
	if(timer)
		timer->setTime();
	if(ret != MPI_SUCCESS) {
		cerr << "error: MPI_Alltoall did not return MPI_SUCCESS" << endl;
		exit(0);
	}

	if(sendbuf)
		free(sendbuf);
	if(recvbuf)
		free(recvbuf);
}

void replay_Alltoallv(Event *e, Timer *timer){
}

void replay_Barrier(Event *e, Timer *timer){
	MPI_Comm comm;
	int ret;

	comm = (MPI_Comm)index_to_comm (replay_ptr, e->getParamValueForRank(COMM, my_rank));
	ret = MPI_Barrier(comm);
	if(timer)
		timer->simulateComputeTime(true, NULL);
	//ret = MPI_Barrier(comm);
	if(timer)
		timer->setTime();

	if(ret != MPI_SUCCESS) {
		cerr << "error: MPI_Barrier did not return MPI_SUCCESS" << endl;
		exit(0);
	}
}

void replay_Bcast(Event *e, Timer *timer){
	MPI_Comm comm;
	MPI_Datatype datatype;
	int datatype_size, count, ret, root;
	void *sendbuf;

	comm = (MPI_Comm)index_to_comm (replay_ptr, e->getParamValueForRank(COMM, my_rank));

	datatype = index_to_type (replay_ptr, e->getParamValueForRank(DATATYPE, my_rank));
	if(MPI_Type_size(datatype, &datatype_size) != MPI_SUCCESS) {
		cerr << "MPI_Bcast error: MPI_Type_size failed - probably invalid type: " 
				<< e->getParamValueForRank(DATATYPE, my_rank) << endl;
		exit(0);
	}

	count = e->getParamValueForRank(COUNT, my_rank);
	root = e->getParamValueForRank(RT, my_rank);

	sendbuf = malloc(count * datatype_size);
	if(!sendbuf) {
		cerr << "MPI_Bcast error: cannot malloc buffer" << endl;
		exit(0);
	}

	if(timer)
		timer->simulateComputeTime();
	ret = MPI_Bcast(sendbuf, count, datatype, root, comm);
	if(timer)
		timer->setTime();

	if(ret != MPI_SUCCESS) {
		cerr << "error: MPI_Bcast did not return MPI_SUCCESS" << endl;
		exit(0);
	}

	if(sendbuf)
		free(sendbuf);
}

void replay_Reduce(Event *e, Timer *timer){
	MPI_Comm comm;
	MPI_Datatype datatype;
	MPI_Op mpi_op;
	int datatype_size, count, ret, root;
	void *sendbuf, *recvbuf;

	comm = (MPI_Comm)index_to_comm (replay_ptr, e->getParamValueForRank(COMM, my_rank));

	datatype = index_to_type (replay_ptr, e->getParamValueForRank(DATATYPE, my_rank));
	if(MPI_Type_size(datatype, &datatype_size) != MPI_SUCCESS) {
		cerr << "MPI_Reduce error: MPI_Type_size failed - probably invalid type: " 
				<< e->getParamValueForRank(DATATYPE, my_rank) << endl;
		exit(0);
	}

	count = e->getParamValueForRank(COUNT, my_rank);
	sendbuf = malloc(count * datatype_size);
	if(!sendbuf) {
		cerr << "MPI_Reduce error: cannot malloc send buffer" << endl;
		exit(0);
	}
	
	root = e->getParamValueForRank(RT, my_rank);
	if(my_rank == root){
		recvbuf = malloc(count * datatype_size);
		if(!recvbuf) {
			cerr << "MPI_Reduce error: cannot malloc recv buffer" << endl;
			exit(0);
		}
	} else {
		recvbuf = NULL;
	}
	
	mpi_op = index_to_op (replay_ptr, e->getParamValueForRank(MPI_OP, my_rank) );
	ret = MPI_Reduce(sendbuf, recvbuf, count, datatype, mpi_op, root, comm);
	if(timer)
		timer->simulateComputeTime();
	//ret = MPI_Reduce(sendbuf, recvbuf, count, datatype, mpi_op, root, comm);
	if(timer)
		timer->setTime();

	if(ret != MPI_SUCCESS) {
		cerr << "error: MPI_Reduce did not return MPI_SUCCESS" << endl;
		exit(0);
	}

	if(sendbuf)
	    free(sendbuf);
	if(recvbuf)
	    free(recvbuf);
}


/* the following functions make the assumption that the input trace
 * is node-level, so these functions use Event::getCurrentValue for
 * a better performance */

/* point-to-point */
void replay_Irecv(Event* e, Timer *timer){
	MPI_Comm comm;
	MPI_Datatype datatype = 0;
	MPI_Request request;
	int datatype_size, count, src, ret, tag;
	void *recvbuf = 0;

	comm = (MPI_Comm)index_to_comm (replay_ptr, e->getCurrentValue(COMM));
	datatype = index_to_type (replay_ptr, e->getCurrentValue(DATATYPE));
	if(MPI_Type_size(datatype, &datatype_size) != MPI_SUCCESS) {
		cerr << "MPI_Irecv error: MPI_Type_size failed - probably invalid type: " 
				<< e->getCurrentValue(DATATYPE) << endl;
		exit(0);
	}
	count = e->getCurrentValue(COUNT);
	recvbuf = malloc(count * datatype_size);
	if(!recvbuf) {
		cerr << "malloc MPI_Irecv buffer" << endl;
		exit(0);
	}
	add_mem_entry(recvbuf);
	src = e->getCurrentValue(SOURCE);
	src = DECODE_SOURCE (src);
	tag = e->getCurrentValue(TAG);

	if(timer)
		timer->simulateComputeTime();
	ret = MPI_Irecv(recvbuf, count, datatype, src, tag, comm, &request);
	if(timer)
		timer->setTime();

	add_request_entry(replay_req, request);
	if(ret != MPI_SUCCESS) {
		cerr << "error: MPI_Irecv did not return MPI_SUCCESS" << endl;
		exit(0);
	}

}

void replay_Recv(Event* e, Timer *timer){
	MPI_Comm comm;
	MPI_Datatype datatype = 0;
	MPI_Status status;
	int datatype_size, count, src, ret, tag;
	void *recvbuf = 0;

	comm = (MPI_Comm)index_to_comm (replay_ptr, e->getCurrentValue(COMM));
	datatype = index_to_type (replay_ptr, e->getCurrentValue(DATATYPE));
	if(MPI_Type_size(datatype, &datatype_size) != MPI_SUCCESS) {
		cerr << "MPI_Recv error: MPI_Type_size failed - probably invalid type: " 
				<< e->getCurrentValue(DATATYPE) << endl;
		exit(0);
	}
	count = e->getCurrentValue(COUNT);
	recvbuf = malloc(count * datatype_size);
	if(!recvbuf) {
		cerr << "malloc MPI_Recv buffer" << endl;
		exit(0);
	}
	src = e->getCurrentValue(SOURCE);
	src = DECODE_SOURCE (src);
	tag = e->getCurrentValue(TAG);

	if(timer)
		timer->simulateComputeTime();
	ret = MPI_Recv(recvbuf, count, datatype, src, tag, comm, &status);
	if(timer)
		timer->setTime();

	if(ret != MPI_SUCCESS) {
		cerr << "error: MPI_Recv did not return MPI_SUCCESS" << endl;
		exit(0);
	}

	if(recvbuf)
		free(recvbuf);

}

void replay_Isend(Event *e, Timer *timer){
	MPI_Comm comm;
	MPI_Datatype datatype = 0;
	MPI_Request request;
	int datatype_size, count, dest, ret, tag;
	void *sendbuf = 0;

	comm = (MPI_Comm)index_to_comm (replay_ptr, e->getCurrentValue(COMM));
	datatype = index_to_type (replay_ptr, e->getCurrentValue(DATATYPE));
	if(MPI_Type_size(datatype, &datatype_size) != MPI_SUCCESS) {
		cerr << "MPI_Isend error: MPI_Type_size failed - probably invalid type: " 
				<< e->getCurrentValue(DATATYPE) << endl;
		exit(0);
	}
	count = e->getCurrentValue(COUNT);
	sendbuf = malloc(count * datatype_size);
	if(!sendbuf) {
		cerr << "malloc MPI_Isend buffer" << endl;
		exit(0);
	}
	add_mem_entry(sendbuf);
	dest = e->getCurrentValue(DEST);
	dest = DECODE_DEST (dest);
	tag = e->getCurrentValue(TAG);

	if(timer)
		timer->simulateComputeTime();
	ret = MPI_Isend(sendbuf, count, datatype, dest, tag, comm, &request);
	if(timer)
		timer->setTime();

	add_request_entry(replay_req, request);
	if(ret != MPI_SUCCESS) {
		cerr << "error: MPI_Isend did not return MPI_SUCCESS" << endl;
		exit(0);
	}

}

void replay_Send(Event *e, Timer *timer){
	MPI_Comm comm;
	MPI_Datatype datatype = 0;
	int datatype_size, count, dest, ret, tag;
	void *sendbuf = 0;

	comm = (MPI_Comm)index_to_comm (replay_ptr, e->getCurrentValue(COMM));
	datatype = index_to_type (replay_ptr, e->getCurrentValue(DATATYPE));
	if(MPI_Type_size(datatype, &datatype_size) != MPI_SUCCESS) {
		cerr << "MPI_Isend error: MPI_Type_size failed - probably invalid type: " 
				<< e->getCurrentValue(DATATYPE) << endl;
		exit(0);
	}
	count = e->getCurrentValue(COUNT);
	sendbuf = malloc(count * datatype_size);
	if(!sendbuf) {
		cerr << "malloc MPI_Send buffer" << endl;
		exit(0);
	}
	dest = e->getCurrentValue(DEST);
	dest = DECODE_DEST (dest);
	tag = e->getCurrentValue(TAG);

	if(timer)
		timer->simulateComputeTime();
	ret = MPI_Send(sendbuf, count, datatype, dest, tag, comm);
	if(timer)
		timer->setTime();

	if(ret != MPI_SUCCESS) {
		cerr << "error: MPI_Send did not return MPI_SUCCESS" << endl;
		exit(0);
	}
	if(sendbuf)
		free(sendbuf);
}

/* Communicator ops */

MPI_Comm replay_Comm_create(Event *e, Timer *timer){
	MPI_Comm comm, comm_out;
	MPI_Group group;
	int comm_id, group_id, ret;

	comm_id = e->getParamValueForRank(COMM, my_rank);
	comm = (MPI_Comm)index_to_comm(replay_ptr, comm_id);

	group_id = e->getParamValueForRank(GROUP, my_rank);
	group = (MPI_Group)index_to_group (replay_ptr, group_id);

	if(timer)
		timer->simulateComputeTime();
	ret = MPI_Comm_create(comm, group, &comm_out);
	if(timer)
		timer->setTime();

	if(ret != MPI_SUCCESS) {
		cerr << "error: MPI_Comm_create did not return MPI_SUCCESS" << endl;
		exit(0);
	}

	add_comm_entry(replay_ptr, comm_out);
	
	return comm_out;
}

MPI_Comm replay_Comm_dup(Event *e, Timer* timer){
	MPI_Comm comm, comm_out;
	int comm_id, ret;

	comm_id = e->getParamValueForRank(COMM, my_rank);
	comm = (MPI_Comm)index_to_comm(replay_ptr, comm_id);

	if(timer)
		timer->simulateComputeTime();
	ret = MPI_Comm_dup(comm, &comm_out);
	if(timer)
		timer->setTime();

	if(ret != MPI_SUCCESS){
		cerr << "error: MPI_Comm_dup did not return MPI_SUCCESS" << endl;
		exit(0);
	}
	add_comm_entry(replay_ptr, comm_out);

	return comm_out;
}

void replay_Comm_free(Event *e, Timer *timer){
	MPI_Comm comm;
	int comm_id, ret;

	comm_id = e->getParamValueForRank(COMM, my_rank);
	comm = (MPI_Comm)index_to_comm(replay_ptr, comm_id);

	remove_comm_entry(replay_ptr, comm);

	if(timer)
		timer->simulateComputeTime();
	ret = MPI_Comm_free(&comm);
	if(timer)
		timer->setTime();

	if(ret != MPI_SUCCESS){
		cerr << "error: MPI_Comm_free did not return MPI_SUCCESS" << endl;
		exit(0);
	}
}


MPI_Comm replay_Comm_split(Event *e, Timer *timer){
	MPI_Comm comm, comm_out;
	int comm_id, ret, color, key;

	comm_id = e->getParamValueForRank(COMM, my_rank);
	comm = (MPI_Comm)index_to_comm(replay_ptr, comm_id);
	
	color = e->getParamValueForRank(COLOR, my_rank);
	key = e->getParamValueForRank(KEY, my_rank);
	key = DECODE_KEY(key);
	
	if(timer)
		timer->simulateComputeTime();
	ret = MPI_Comm_split(comm, color, key, &comm_out);
	if(timer)
		timer->setTime();

	if(ret != MPI_SUCCESS){
		cerr << "error: MPI_Comm_split did not return MPI_SUCCESS" << endl;
		exit(0);
	}
	add_comm_entry(replay_ptr, comm_out);

	return comm_out;
}



/* group ops */

MPI_Group replay_Comm_group(Event *e, Timer *timer){
	MPI_Comm comm;
	MPI_Group group;
	int comm_id, ret;

	comm_id = e->getParamValueForRank(COMM, my_rank);
	comm = (MPI_Comm)index_to_comm(replay_ptr, comm_id);

	if(timer)
		timer->simulateComputeTime();
	ret = MPI_Comm_group(comm, &group);
	if(timer)
		timer->setTime();

	if(ret != MPI_SUCCESS){
		cerr << "error: MPI_Comm_group did not return MPI_SUCCESS" << endl;
		exit(0);
	}

	add_group_entry(replay_ptr, group);

	return group;
}

MPI_Group replay_Group_difference(Event *e, Timer *timer){
	MPI_Group group1, group2, group_out;
	int ret, group_id1, group_id2;

	group_id1 = e->getParamValueForRank(GROUP1, my_rank);
	group1 = (MPI_Group)index_to_group (replay_ptr, group_id1);
	group_id2 = e->getParamValueForRank(GROUP2, my_rank);
	group2 = (MPI_Group)index_to_group (replay_ptr, group_id2);

	if(timer)
		timer->simulateComputeTime();
	ret = MPI_Group_difference(group1, group2, &group_out);
	if(timer)
		timer->setTime();

	if(ret != MPI_SUCCESS){
		cerr << "error: MPI_Group_difference did not return MPI_SUCCESS" << endl;
		exit(0);
	}

	add_group_entry(replay_ptr, group_out);

	return group_out;
}

MPI_Group replay_Group_excl(Event *e, Timer *timer){
	MPI_Group group, group_out;
	int ret, group_id, count, *ranks;

	group_id = e->getParamValueForRank(GROUP, my_rank);
	group = (MPI_Group)index_to_group (replay_ptr, group_id);
	
	count = e->getParamValueForRank(COUNT, my_rank);

	ranks = e->getParamValuesForRank(ARRAY_OF_RANKS, count, my_rank);
	if(ranks == NULL){
		cerr << "error: MPI_Group_excl cannot find ranks" << endl;
		exit(0);
	}

	if(timer)
		timer->simulateComputeTime();
	ret = MPI_Group_excl(group, count, ranks, &group_out);
	if(timer)
		timer->setTime();

	if(ret != MPI_SUCCESS) {
		cerr << "error: MPI_Group_excl did not return MPI_SUCCESS" << endl;
		exit(0);
	}

	add_group_entry(replay_ptr, group_out);

	if(ranks)
		free(ranks);

	return group_out;

}

void replay_Group_free(Event *e, Timer* timer){
	MPI_Group group;
	int group_id, ret;

	group_id = e->getParamValueForRank(GROUP, my_rank);
	group = (MPI_Group)index_to_group(replay_ptr, group_id);

	remove_group_entry(replay_ptr, group);

	if(timer)
		timer->simulateComputeTime();
	ret = MPI_Group_free(&group);
	if(timer)
		timer->setTime();

	if(ret != MPI_SUCCESS) {
		cerr << "error: MPI_Group_free did not return MPI_SUCCESS" << endl;
		exit(0);
	}

}


MPI_Group replay_Group_incl(Event *e, Timer *timer){
	MPI_Group group, group_out;
	int ret, group_id, count, *ranks;

	group_id = e->getParamValueForRank(GROUP, my_rank);
	group = (MPI_Group)index_to_group (replay_ptr, group_id);
	
	count = e->getParamValueForRank(COUNT, my_rank);

	ranks = e->getParamValuesForRank(ARRAY_OF_RANKS, count, my_rank);
	if(ranks == NULL){
		cerr << "error: MPI_Group_incl cannot find ranks" << endl;
		exit(0);
	}

	if(timer)
		timer->simulateComputeTime();
	ret = MPI_Group_incl(group, count, ranks, &group_out);
	if(timer)
		timer->setTime();

	if(ret != MPI_SUCCESS) {
		cerr << "error: MPI_Group_incl did not return MPI_SUCCESS" << endl;
		exit(0);
	}

	add_group_entry(replay_ptr, group_out);

	if(ranks)
		free(ranks);

	return group_out;

}

MPI_Group replay_Group_intersection(Event *e, Timer *timer){
	MPI_Group group1, group2, group_out;
	int ret, group_id1, group_id2;

	group_id1 = e->getParamValueForRank(GROUP1, my_rank);
	group1 = (MPI_Group)index_to_group (replay_ptr, group_id1);
	group_id2 = e->getParamValueForRank(GROUP2, my_rank);
	group2 = (MPI_Group)index_to_group (replay_ptr, group_id2);

	if(timer)
		timer->simulateComputeTime();
	ret = MPI_Group_intersection(group1, group2, &group_out);
	if(timer)
		timer->setTime();

	if(ret != MPI_SUCCESS){
		cerr << "error: MPI_Group_intersection did not return MPI_SUCCESS" << endl;
		exit(0);
	}

	add_group_entry(replay_ptr, group_out);

	return group_out;
}

MPI_Group replay_Group_range_excl(Event *e, Timer *timer){
	typedef int triplet_t[3];
	MPI_Group group, group_out;
	int ret, group_id, count, *ranks;
	triplet_t *triplets;

	group_id = e->getParamValueForRank(GROUP, my_rank);
	group = (MPI_Group)index_to_group (replay_ptr, group_id);
	
	count = e->getParamValueForRank(COUNT, my_rank);

	ranks = e->getParamValuesForRank(RANGES, count * 3, my_rank);
	if(ranks == NULL){
		cerr << "error: MPI_Group_range_excl cannot find ranks" << endl;
		exit(0);
	}

	triplets = (triplet_t*)malloc(count * sizeof(triplet_t));
	if(triplets == NULL){
		cerr << "error: MPI_Group_range_excl cannot allocate buffer for triplets" << endl;
		exit(0);
	}
	for(int i=0; i<count; i++){
		for(int j=0; j<3; j++)
			triplets[i][j] = ranks[i*3 + j];
	}

	if(ranks)
		free(ranks);

	if(timer)
		timer->simulateComputeTime();
	ret = MPI_Group_range_excl(group, count, triplets, &group_out);
	if(timer)
		timer->setTime();

	if(ret != MPI_SUCCESS) {
		cerr << "error: MPI_Group_range_excl did not return MPI_SUCCESS" << endl;
		exit(0);
	}

	add_group_entry(replay_ptr, group_out);

	if(triplets)
		free(triplets);

	return group_out;
}

MPI_Group replay_Group_range_incl(Event *e, Timer *timer){
	typedef int triplet_t[3];
	MPI_Group group, group_out;
	int ret, group_id, count, *ranks;
	triplet_t *triplets;

	group_id = e->getParamValueForRank(GROUP, my_rank);
	group = (MPI_Group)index_to_group (replay_ptr, group_id);
	
	count = e->getParamValueForRank(COUNT, my_rank);

	ranks = e->getParamValuesForRank(RANGES, count * 3, my_rank);
	if(ranks == NULL){
		cerr << "error: MPI_Group_range_incl cannot find ranks" << endl;
		exit(0);
	}

	triplets = (triplet_t*)malloc(count * sizeof(triplet_t));
	if(triplets == NULL){
		cerr << "error: MPI_Group_range_incl cannot allocate buffer for triplets" << endl;
		exit(0);
	}
	for(int i=0; i<count; i++){
		for(int j=0; j<3; j++)
			triplets[i][j] = ranks[i*3 + j];
	}

	if(ranks)
		free(ranks);

	if(timer)
		timer->simulateComputeTime();
	ret = MPI_Group_range_incl(group, count, triplets, &group_out);
	if(timer)
		timer->setTime();

	if(ret != MPI_SUCCESS) {
		cerr << "error: MPI_Group_range_incl did not return MPI_SUCCESS" << endl;
		exit(0);
	}

	add_group_entry(replay_ptr, group_out);

	if(triplets)
		free(triplets);

	return group_out;
}

MPI_Group replay_Group_union(Event *e, Timer *timer){
	MPI_Group group1, group2, group_out;
	int ret, group_id1, group_id2;

	group_id1 = e->getParamValueForRank(GROUP1, my_rank);
	group1 = (MPI_Group)index_to_group (replay_ptr, group_id1);
	group_id2 = e->getParamValueForRank(GROUP2, my_rank);
	group2 = (MPI_Group)index_to_group (replay_ptr, group_id2);

	if(timer)
		timer->simulateComputeTime();
	ret = MPI_Group_union(group1, group2, &group_out);
	if(timer)
		timer->setTime();

	if(ret != MPI_SUCCESS){
		cerr << "error: MPI_Group_union did not return MPI_SUCCESS" << endl;
		exit(0);
	}

	add_group_entry(replay_ptr, group_out);

	return group_out;
}


/* Wait */
void replay_Wait(Event *e, Timer *timer){
	int offset, ret;
	MPI_Request request;
	MPI_Status status;

	offset = e->getCurrentValue(REQUEST);
	lookup_request( replay_req, offset, RECORDED_MPI_REQUEST_NULL, &request);

	if(timer)
		timer->simulateComputeTime();
	ret = MPI_Wait(&request, &status);
	if(timer)
		timer->setTime();

	if(ret != MPI_SUCCESS) {
		cerr << "error: MPI_Wait did not return MPI_SUCCESS" << endl;
		exit(0);
	}
	if (request == MPI_REQUEST_NULL)
		free_mem_entry(offset);
}

void replay_Waitall(Event *e, Timer *timer){
	int *offsets, count;
	MPI_Request *requests;
	MPI_Status *statuses;
	int ret;

	count = e->getCurrentValue(COUNT);
	offsets = e->getCurrentValues(ARRAY_OF_REQUESTS, count);
	if(offsets == NULL){
		cerr << "error: MPI_Waitall could not find the offsets" << endl;
		exit(0);
	}
	requests = (MPI_Request *) malloc(sizeof(MPI_Request) * count);
	statuses = (MPI_Status *) malloc(sizeof(MPI_Status) * count);

	lookup_requestlist( replay_req, offsets, count, RECORDED_MPI_REQUEST_NULL, requests);

	if(timer)
		timer->simulateComputeTime();
	ret = MPI_Waitall(count, requests, statuses);
	if(timer)
		timer->setTime();

	if(ret != MPI_SUCCESS) {
		cerr << "error: MPI_Waitall did not return MPI_SUCCESS" << endl;
		int x, length;
		char err[MPI_MAX_ERROR_STRING];
		for(x=0; x<count; x++){
			if(statuses[x].MPI_ERROR!=MPI_SUCCESS)
				MPI_Error_string(statuses[x].MPI_ERROR, err, &length);
			cerr <<  statuses[x].MPI_SOURCE << " " << err << endl;
		}
		exit(0);
	}

	for (int i = 0; i < count; i++) {
		if (requests[i] == MPI_REQUEST_NULL)
			free_mem_entry (offsets[i]);
	}
	free(requests);
	free(statuses);

	if(offsets != NULL)
		free(offsets);
}

int replay_File_open(Event *e, Timer *timer){
	MPI_Comm comm;
	string temp;
	vector<string> *tem;
	int amode,indexfh,ret = 0;
	MPI_File fh;
	MPI_Info info =  MPI_INFO_NULL;
	comm = (MPI_Comm)index_to_comm(replay_ptr, e->getCurrentValue(COMM));
	temp = e -> getCurrentCharValue(FILENAME);
	char ch[temp.length()];
	temp.copy(ch, temp.length(), 0);
	char *p=&ch[0];
	p += temp.length();
	*p = '\0';

	amode = e -> getCurrentValue(AMODE);
	indexfh = e -> getCurrentValue(FH);
	fh = (MPI_File)index_to_file_handle(replay_ptr, e->getCurrentValue(FH));
	
	
	Fileopen fileopen;
	fileopen.comm = comm;
	fileopen.amode = amode;
	fileopen.info = info;

	for(unsigned i=0; i<temp.length(); i++)
		fileopen.ch[i] = *(temp.begin()+i);
	fileopen.ch[temp.length()] = '\0';

	pair<int, Fileopen> m(indexfh,fileopen);
	file_open.insert(m);

	return ret;
}

int replay_File_close(Event *e, Timer *timer){
	int ret=0,indexfh;
	MPI_File fh;
	indexfh = e->getCurrentValue(FH);
	map<int, Fileopen>::iterator it;
	it = file_open.find(indexfh);	

	if(it != file_open.end()){
		fh = (it->second).fh;
		if(timer)
			timer->simulateComputeTime();
		ret = MPI_File_close(&fh);
		if(timer)
			timer->setTime();
		if(ret != MPI_SUCCESS){
			cerr << "error: MPI_File_close did not return MPI_SUCCESS"<<endl;
			exit(0);
		}
		else{
			file_open.erase(it);
		}
	}
	else{
		cerr << "error: no such files opened" <<endl;
		exit(0);
	}
	return ret;
	
}

int replay_File_delete(Event *e, Timer *timer){
	int ret;
	string temp;
	vector<string> *tem;
	MPI_Info info = MPI_INFO_NULL;
	temp = e -> getCurrentCharValue(FILENAME);
	char ch[temp.length()];
	temp.copy(ch, temp.length(), 0);
	char *p = &ch[0];
	p += temp.length();
	*p = '\0';
	
	if(timer)
		timer->simulateComputeTime();
	ret = MPI_File_delete(&ch[0], info);
	if(timer)
		timer->setTime();

	if(ret != MPI_SUCCESS){
		cerr << "error: MPI_File_delete did not return MPI_SUCCESS"<<endl;
		exit(0);
	}

	return ret;
}

int replay_File_seek(Event *e, Timer *timer){
	int ret = 0,whence,indexfh;
	MPI_Offset offset;

	whence = e -> getCurrentValue(WHENCE);
	offset = e -> getCurrentValue(OFFSET);
	indexfh = e -> getCurrentValue(FH);

	map<int, Fileopen>::iterator it;
	it = file_open.find(indexfh);
	if(it != file_open.end()){
		if(timer)
			timer ->simulateComputeTime();
		ret = MPI_File_seek((it->second).fh, offset, whence);
		if(timer)
			timer->setTime();
		if(ret != MPI_SUCCESS){
			cerr << "error: MPI_File_seek did not return MPI_SUCCESS"<<endl;
			exit(0);
		}
	}
	else{
		cerr << "error: no such files opened"<<endl;
		exit(0);
	}

	return ret;
}

int replay_File_set_view(Event *e, Timer *timer){
	int ret = 0,indexfh;
	vector<string> *tem;
	string temp;
	MPI_Offset disp;
	MPI_Datatype datatype, filetype;
	MPI_Info info = MPI_INFO_NULL;

	disp = e->getCurrentValue(OFFSET);
	datatype = index_to_type(replay_ptr, e->getCurrentValue(ETYPE));
	filetype = index_to_type(replay_ptr, e->getParamValueForRank(FILETYPE,my_rank));
	indexfh = e->getCurrentValue(FH);
	temp = e -> getCurrentCharValue(DATAREP);

	char ch[temp.length()+1];	
	temp.copy(ch, temp.length(), 0);
	char *p = &ch[0];
	p += temp.length();
	*p = '\0';

	map<int, Fileopen>::iterator it;
	it = file_open.find(indexfh);
	if(it != file_open.end()){
		if(timer)
			timer->simulateComputeTime();
		ret = MPI_File_set_view((it->second).fh, disp, datatype, filetype, &ch[0], info);
		if(timer)
			timer->setTime();
		if(ret != MPI_SUCCESS){
			cerr << "error: MPI_File_set_view did not return MPI_SUCCESS"<<endl;
			exit(0);
		}
	}
	else{
		cerr << "error: no such files opened"<<endl;
		exit(0);
	}
	return ret;
}


int replay_File_seek_shared(Event *e, Timer *timer){
	int ret = 0,whence,indexfh;
	MPI_Offset offset;

	whence = e -> getCurrentValue(WHENCE);
	offset = e -> getCurrentValue(OFFSET);
	indexfh = e -> getCurrentValue(FH);

	map<int, Fileopen>::iterator it;
	it = file_open.find(indexfh);
	if(it != file_open.end()){
		if(timer)
			timer ->simulateComputeTime();
		ret = MPI_File_seek_shared((it->second).fh, offset, whence);
		if(timer)
			timer->setTime();
		if(ret != MPI_SUCCESS){
			cerr << "error: MPI_File_seek_shared did not return MPI_SUCCESS"<<endl;
			exit(0);
		}
	}
	else{
		cerr << "error: no such files opened"<<endl;
		exit(0);
	}

	return ret;
}

int replay_File_set_size(Event *e, Timer *timer){
	int ret, indexfh;
	MPI_Offset size;

	indexfh = e->getCurrentValue(FH);
	size = e->getCurrentValue(OFFSET);

	map<int, Fileopen>::iterator it;
	it = file_open.find(indexfh);
	if(it != file_open.end()){
		if(timer)
			timer->simulateComputeTime();
		ret = MPI_File_set_size((it->second).fh, size);
		if(timer)
			timer->setTime();
		if(ret != MPI_SUCCESS){
			cerr << "error: MPI_File_set_size did not return MPI_SUCCESS"<<endl;
			exit(0);
		}
	}
	else{
		cerr << "error: no such files opened"<<endl;
		exit(0);
	}
	return ret;
}


int replay_File_preallocate(Event *e, Timer *timer){
	int ret, indexfh;
	MPI_Offset size;

	indexfh = e->getCurrentValue(FH);
	size = e->getCurrentValue(OFFSET);

	map<int, Fileopen>::iterator it;
	it = file_open.find(indexfh);
	if(it != file_open.end()){
		if(timer)
			timer->simulateComputeTime();
		ret = MPI_File_preallocate((it->second).fh, size);
		if(timer)
			timer->setTime();
		if(ret != MPI_SUCCESS){
			cerr << "error: MPI_File_preallocate did not return MPI_SUCCESS"<<endl;
			exit(0);
		}
	}
	else{
		cerr << "error: no such files opened"<<endl;
		exit(0);
	}
	return ret;
}


int replay_File_sync(Event *e, Timer *timer){
	int ret, indexfh;
	
	indexfh = e->getCurrentValue(FH);

	map<int, Fileopen>::iterator it;
	it = file_open.find(indexfh);
	if(it != file_open.end()){
		if(timer)
			timer->simulateComputeTime();
		ret = MPI_File_sync((it->second).fh);
		if(timer)
			timer->setTime();
		if(ret != MPI_SUCCESS){
			cerr << "error: MPI_File_sync did not return MPI_SUCCESS"<<endl;
			exit(0);
		}
	}
	else{
		cerr << "error: no such files opened"<<endl;
		exit(0);
	}
	return ret;
}


int replay_File_set_atomicity(Event *e, Timer *timer){
	int ret = 0,flag,indexfh;

	indexfh = e->getCurrentValue(FH);
	flag = e->getCurrentValue(FLAG);

	map<int, Fileopen>::iterator it;
	it = file_open.find(indexfh);
	if(it != file_open.end()){
		if(timer)
			timer->simulateComputeTime();
		ret = MPI_File_set_atomicity((it->second).fh, flag);
		if(timer)
			timer->setTime();
		if(ret != MPI_SUCCESS){
			cerr << "error: MPI_File_set_atomicity did not return MPI_SUCCESS"<<endl;
			exit(0);
		}
	}
	else{
		cerr << "error: no such files opened"<<endl;
		exit(0);
	}
	return ret;
}

int replay_File_c2f(Event *e, Timer *timer){
	int ret = 0, indexfh;
	indexfh = e->getCurrentValue(FH);
	map<int, Fileopen>::iterator it;
	it = file_open.find(indexfh);
	if(it != file_open.end()){
		if(timer)
			timer->simulateComputeTime();
		ret = MPI_File_c2f((it->second).fh);
		if(timer)
			timer->setTime();
		if(ret != MPI_SUCCESS){
			cerr << "error: MPI_Fle_c2f did not return MPI_SUCCESS"<<endl;
			exit(0);
		}
	}
	else{
		cerr << "error: no such files opened"<<endl;
		exit(0);
	}
	return ret;
}

int replay_File_read_write(Event *e, Timer *timer, int op_code){
	int ret = 0,count,indexfh,datatype_size;
	MPI_File fh;
	MPI_Datatype datatype;
	MPI_Status status;
	MPI_Request request;

	count = e->getCurrentValue(COUNT);
	datatype = index_to_type(replay_ptr, e->getCurrentValue(DATATYPE));
	indexfh = e->getCurrentValue(FH);
	if(MPI_Type_size(datatype, &datatype_size) !=MPI_SUCCESS){
		cerr << "MPI_File_read_write error: MPI_Type_size failed - probably invalid type: "
			<< e->getCurrentValue(DATATYPE) <<endl;
		exit(0);
	}
	if(!buf){
		cerr << "malloc MPI_File_write_shared buffer"<<endl;
		exit(0);
	}
	
	map<int, Fileopen>::iterator it;
	it = file_open.find(indexfh);
	if(it != file_open.end()){
		fh = (it->second).fh;
		switch(op_code){
			case umpi_MPI_File_read:
				{
					if(timer)
						timer->simulateComputeTime();
					ret = MPI_File_read(fh, buf, count, datatype, &status);
					if(timer)
						timer->setTime();
					if(ret != MPI_SUCCESS){
						cerr << "error: MPI_File_read did not return MPI_SUCCESS"<<endl;
						exit(0);
					}
				}
				break;
			case umpi_MPI_File_iread:
				{		
					if(timer)
						timer->simulateComputeTime();
					ret = MPI_File_iread(fh, buf, count, datatype, &request);
					if(timer)
						timer->setTime();
					add_request_entry(replay_req, request);
					if(ret != MPI_SUCCESS){
						cerr << "error: MPI_File_iread did not return MPI_SUCCESS"<<endl;
						exit(0);
					}
				}
				break;
			case umpi_MPI_File_read_shared:
				{
					if(timer)
						timer->simulateComputeTime();
					ret = MPI_File_read_shared(fh, buf, count, datatype, &status);
					if(timer)
						timer->setTime();
					if(ret != MPI_SUCCESS){
						cerr << "error: MPI_File_read_shared did not return MPI_SUCCESS"<<endl;
						exit(0);
					}
				}
				break;
			case umpi_MPI_File_iread_shared:
				{		
					if(timer)
						timer->simulateComputeTime();
					ret = MPI_File_iread_shared(fh, buf, count, datatype, &request);
					if(timer)
						timer->setTime();
					add_request_entry(replay_req, request);
					if(ret != MPI_SUCCESS){
						cerr << "error: MPI_File_iread_shared did not return MPI_SUCCESS"<<endl;
						exit(0);
					}
				}
				break;
			case umpi_MPI_File_write:
				{
					if(timer)
						timer->simulateComputeTime();
					ret = MPI_File_write(fh, buf, count, datatype, &status);
					if(timer)
						timer->setTime();
					if(ret != MPI_SUCCESS){
						cerr << "error: MPI_File_write did not return MPI_SUCCESS"<<endl;
						exit(0);
					}
				}
				break;
			case umpi_MPI_File_iwrite:
				{		
					if(timer)
						timer->simulateComputeTime();
					ret = MPI_File_iwrite(fh, buf, count, datatype, &request);
					if(timer)
						timer->setTime();

					add_request_entry(replay_req, request);
					if(ret != MPI_SUCCESS){
						cerr << "error: MPI_File_iwrite did not return MPI_SUCCESS"<<endl;
						exit(0);
					}
				}
				break;
			case umpi_MPI_File_write_shared:
				{
					if(timer)
						timer->simulateComputeTime();
					ret = MPI_File_write_shared(fh, buf, count, datatype, &status);
					if(timer)
						timer->setTime();
					if(ret != MPI_SUCCESS){
						cerr << "error: MPI_File_write_shared did not return MPI_SUCCESS"<<endl;
						exit(0);
					}
				}
				break;
			case umpi_MPI_File_iwrite_shared:
				{		
					if(timer)
						timer->simulateComputeTime();
					ret = MPI_File_iwrite_shared(fh, buf, count, datatype, &request);
					if(timer)
						timer->setTime();
					add_request_entry(replay_req, request);
					if(ret != MPI_SUCCESS){
						cerr << "error: MPI_File_iwrite_shared did not return MPI_SUCCESS"<<endl;
						exit(0);
					}
				}
				break;			
			case umpi_MPI_File_read_all:
				{
					if(timer)
						timer->simulateComputeTime();
					ret = MPI_File_read_all(fh, buf, count, datatype, &status);
					if(timer)
						timer->setTime();
					if(ret != MPI_SUCCESS){
						cerr << "error: MPI_File_read_all did not return MPI_SUCCESS"<<endl;
						exit(0);
					}
				}
				break;
			case umpi_MPI_File_write_all:
				{
					if(timer)
						timer->simulateComputeTime();
					ret = MPI_File_write_all(fh, buf, count, datatype, &status);
					if(timer)
						timer->setTime();
					if(ret != MPI_SUCCESS){
						cerr << "error: MPI_File_write_all did not return MPI_SUCCESS"<<endl;
						exit(0);
					}
				}
				break;
			case umpi_MPI_File_read_ordered:
				{
					if(timer)
						timer->simulateComputeTime();
					ret = MPI_File_read_ordered(fh, buf, count, datatype, &status);
					if(timer)
						timer->setTime();
					if(ret != MPI_SUCCESS){
						cerr << "error: MPI_File_read_ordered did not return MPI_SUCCESS"<<endl;
						exit(0);
					}
				}
				break;
			case umpi_MPI_File_write_ordered:
				{
					if(timer)
						timer->simulateComputeTime();
					ret = MPI_File_write_ordered(fh, buf, count, datatype, &status);
					if(timer)
						timer->setTime();
					if(ret != MPI_SUCCESS){
						cerr << "error: MPI_File_write_ordered did not return MPI_SUCCESS"<<endl;
						exit(0);
					}
				}
				break;
			case umpi_MPI_File_read_all_begin:
				{
					if(timer)
						timer->simulateComputeTime();
					ret = MPI_File_read_all_begin(fh, buf, count, datatype);
					if(timer)
						timer->setTime();
					if(ret != MPI_SUCCESS){
						cerr << "error: MPI_File_read_all_begin did not return MPI_SUCCESS"<<endl;
						exit(0);
					}
				}
				break;
			case umpi_MPI_File_write_all_begin:
				{
					if(timer)
						timer->simulateComputeTime();
					ret = MPI_File_write_all_begin(fh, buf, count, datatype);
					if(timer)
						timer->setTime();
					if(ret != MPI_SUCCESS){
						cerr << "error: MPI_File_write_all_begin did not return MPI_SUCCESS"<<endl;
						exit(0);
					}
				}
				break;
			case umpi_MPI_File_read_ordered_begin:
				{
					if(timer)
						timer->simulateComputeTime();
					ret = MPI_File_read_ordered_begin(fh, buf, count, datatype);
					if(timer)
						timer->setTime();
					if(ret != MPI_SUCCESS){
						cerr << "error: MPI_File_read_ordered_begin did not return MPI_SUCCESS"<<endl;
						exit(0);
					}
				}
				break;
			case umpi_MPI_File_write_ordered_begin:
				{
					if(timer)
						timer->simulateComputeTime();
					ret = MPI_File_write_ordered_begin(fh, buf, count, datatype);
					if(timer)
						timer->setTime();
					if(ret != MPI_SUCCESS){
						cerr << "error: MPI_File_write_ordered_begin did not return MPI_SUCCESS"<<endl;
						exit(0);
					}
				}
				break;
		}
	}
	else{
		cerr <<"error: no such files opend"<<endl;
		exit(0);
	}
	return ret;

}

int replay_File_read_write_at(Event *e, Timer *timer, int op_code){
	int ret = 0,count,indexfh,datatype_size;
	MPI_File fh;
	MPI_Offset offset;
	MPI_Datatype datatype;
	MPI_Status status;
	MPI_Request request;

	count = e->getCurrentValue(COUNT);
	datatype = index_to_type(replay_ptr, e->getCurrentValue(DATATYPE));
	offset = e->getCurrentValue(OFFSET);
	indexfh = e->getCurrentValue(FH);
	if(MPI_Type_size(datatype, &datatype_size) !=MPI_SUCCESS){
		cerr << "MPI_File_read_at error: MPI_Type_size failed - probably invalid type: "
			<< e->getCurrentValue(DATATYPE) <<endl;
		exit(0);
	}
	if(!buf){
		cerr << "malloc MPI_File_read_at buffer"<<endl;
		exit(0);
	}
	
	map<int, Fileopen>::iterator it;
	it = file_open.find(indexfh);
	if(it != file_open.end()){
		fh = (it->second).fh;
		switch(op_code){
			case umpi_MPI_File_read_at:
				{
					if(timer)
						timer->simulateComputeTime();
					ret = MPI_File_read_at(fh, offset, buf, count, datatype, &status);
					if(timer)
						timer->setTime();
					if(ret != MPI_SUCCESS){
						cerr << "error: MPI_File_read_at did not return MPI_SUCCESS"<<endl;
						exit(0);
					}					
				}
				break;			
			case umpi_MPI_File_iread_at:
				{	
					if(timer)
						timer->simulateComputeTime();
					ret = MPI_File_iread_at(fh, offset, buf, count, datatype, &request);
					if(timer)
						timer->setTime();
					add_request_entry(replay_req, request);
					if(ret != MPI_SUCCESS){
						cerr << "error: MPI_File_iread_at did not return MPI_SUCCESS"<<endl;
						exit(0);
					}
				}
				break;
			case umpi_MPI_File_write_at:
				{
					if(timer)
						timer->simulateComputeTime();
					ret = MPI_File_write_at(fh, offset, buf, count, datatype, &status);
					if(timer)
						timer->setTime();
					if(ret != MPI_SUCCESS){
						cerr << "error: MPI_File_write_at did not return MPI_SUCCESS"<<endl;
						exit(0);
					}					       
				}
				break;			
			case umpi_MPI_File_iwrite_at:
				{	
					if(timer)
						timer->simulateComputeTime();
					ret = MPI_File_iwrite_at(fh, offset, buf, count, datatype, &request);
					if(timer)
						timer->setTime();
					add_request_entry(replay_req, request);
					if(ret != MPI_SUCCESS){
						cerr << "error: MPI_File_iwrite_at did not return MPI_SUCCESS"<<endl;
						exit(0);
					}
				}
				break;
			case umpi_MPI_File_read_at_all:
				{
					if(timer)
						timer->simulateComputeTime();
					ret = MPI_File_read_at_all(fh, offset, buf, count, datatype, &status);
					if(timer)
						timer->setTime();
					if(ret != MPI_SUCCESS){
						cerr << "error: MPI_File_read_at_all did not return MPI_SUCCESS"<<endl;
						exit(0);
					}					       
				}
				break;			
			case umpi_MPI_File_write_at_all:
				{	
					if(timer)
						timer->simulateComputeTime();
					ret = MPI_File_write_at_all(fh, offset, buf, count, datatype, &status);
					if(timer)
						timer->setTime();
					if(ret != MPI_SUCCESS){
						cerr << "error: MPI_File_write_at_all did not return MPI_SUCCESS"<<endl;
						exit(0);
					}
				}
				break;
			case umpi_MPI_File_read_at_all_begin:
				{
					if(timer)
						timer->simulateComputeTime();
					ret = MPI_File_read_at_all_begin(fh, offset, buf, count, datatype);
					if(timer)
						timer->setTime();
					if(ret != MPI_SUCCESS){
						cerr << "error: MPI_File_read_at_all_begin did not return MPI_SUCCESS"<<endl;
						exit(0);
					}
				}
				break;			
			case umpi_MPI_File_write_at_all_begin:
				{	
					if(timer)
						timer->simulateComputeTime();
					ret = MPI_File_write_at_all_begin(fh, offset, buf, count, datatype);
					if(timer)
						timer->setTime();
					if(ret != MPI_SUCCESS){
						cerr << "error: MPI_Fil_write_at_all_begin did not return MPI_SUCCESS"<<endl;
						exit(0);
					}
				}
				break;
		}
	}
	else{
		cerr <<"error: no such files opend"<<endl;
		exit(0);
	}
	return ret;

}

int replay_File_read_at_all_end(Event *e, Timer *timer){
	int ret = 0, indexfh;
	MPI_Status status;
	int opcode = umpi_MPI_File_read_at_all_begin;

	indexfh = e->getCurrentValue(FH);
	multimap<int, readwriteend>::iterator it=r_w_end.find(indexfh);
	if(it != r_w_end.end() && (it->second).op_code == opcode){
		if(timer)
			timer->simulateComputeTime();
		ret = MPI_File_read_at_all_end((it->second).fh, buf,&status);
		if(timer)
			timer->setTime();
		if(ret != MPI_SUCCESS){
			cerr << "error: MPI_File_read_at_all_end did not return MPI_SUCCESS"<<endl;
			exit(0);
		}
		r_w_end.erase(it);			
	}		
	else{
		cerr << "error: no corresponded MPI_File_read_at_all_begin for it"<<endl;
		exit(0);
	}
	return ret;
}

int replay_File_write_at_all_end(Event *e, Timer *timer){
	int ret = 0, indexfh;
	MPI_Status status;
	int opcode = umpi_MPI_File_write_at_all_begin;

	indexfh = e->getCurrentValue(FH);
	multimap<int, readwriteend>::iterator it=r_w_end.find(indexfh);
	if(it != r_w_end.end() && (it->second).op_code == opcode){
		if(timer)
			timer->simulateComputeTime();
		ret = MPI_File_write_at_all_end((it->second).fh, buf,&status);
		if(timer)
			timer->setTime();
		if(ret != MPI_SUCCESS){
			cerr << "error: MPI_File_write_at_all_end did not return MPI_SUCCESS"<<endl;
			exit(0);
		}
		r_w_end.erase(it);
	}
	else{
		cerr << "error: no corresponded MPI_File_write_at_all_begin for it"<<endl;
		exit(0);
	}
	return ret;
}

int replay_File_read_all_end(Event *e, Timer *timer){
	int ret = 0, indexfh;
	MPI_Status status;
	int opcode = umpi_MPI_File_read_all_begin;

	indexfh = e->getCurrentValue(FH);
	multimap<int, readwriteend>::iterator it=r_w_end.find(indexfh);
	if(it!=r_w_end.end() && (it->second).op_code == opcode){
		if(timer)
			timer->simulateComputeTime();
		ret = MPI_File_read_all_end((it->second).fh, buf,&status);
		if(timer)
			timer->setTime();
		if(ret != MPI_SUCCESS){
			cerr << "error: MPI_File_read_at_all_end did not return MPI_SUCCESS"<<endl;
			exit(0);
		}
		r_w_end.erase(it);
	}
	else{
		cerr << "error: no corresponded MPI_File_read_all_begin for it"<<endl;
		exit(0);
	}
	return ret;
}

int replay_File_write_all_end(Event *e, Timer *timer){
	int ret = 0, indexfh;
	MPI_Status status;
	int opcode = umpi_MPI_File_write_all_begin;

	indexfh = e->getCurrentValue(FH);
	multimap<int, readwriteend>::iterator it=r_w_end.find(indexfh);
	if(it != r_w_end.end() && (it->second).op_code == opcode){
		if(timer)
			timer->simulateComputeTime();
		ret = MPI_File_write_all_end((it->second).fh, buf,&status);
		if(timer)
			timer->setTime();
		if(ret != MPI_SUCCESS){
			cerr << "error: MPI_File_write_all_end did not return MPI_SUCCESS"<<endl;
			exit(0);
		}
		r_w_end.erase(it);
	}
	else{
		cerr << "error: no corresponded MPI_File_write_all_begin for it"<<endl;
		exit(0);
	}
	return ret;
}

int replay_File_read_ordered_end(Event *e, Timer *timer){
	int ret = 0, indexfh;
	MPI_Status status;
	int opcode = umpi_MPI_File_read_ordered_begin;

	indexfh = e->getCurrentValue(FH);
	multimap<int, readwriteend>::iterator it=r_w_end.find(indexfh);
	if(it != r_w_end.end() && (it->second).op_code == opcode){
		if(timer)
			timer->simulateComputeTime();
		ret = MPI_File_read_ordered_end((it->second).fh, buf,&status);
		if(timer)
			timer->setTime();
		if(ret != MPI_SUCCESS){
			cerr << "error: MPI_File_read_ordered_end did not return MPI_SUCCESS"<<endl;
			exit(0);
		}
		r_w_end.erase(it);
	}
	else{
		cerr << "error: no corresponded MPI_File_read_ordered_begin for it"<<endl;
		exit(0);
	}
	return ret;
}

int replay_File_write_ordered_end(Event *e, Timer *timer){
	int ret = 0, indexfh;
	MPI_Status status;
	int opcode = umpi_MPI_File_write_ordered_begin;

	indexfh = e->getCurrentValue(FH);
	multimap<int, readwriteend>::iterator it=r_w_end.find(indexfh);
	if(it != r_w_end.end() && (it->second).op_code == opcode){
		if(timer)
			timer->simulateComputeTime();
		ret = MPI_File_write_ordered_end((it->second).fh, buf,&status);
		if(timer)
			timer->setTime();
		if(ret != MPI_SUCCESS){
			cerr << "error: MPI_File_write_ordered_end did not return MPI_SUCCESS"<<endl;
			exit(0);
		}
		r_w_end.erase(it);
	}
	else{
		cerr << "error: no corresponded MPI_File_write_ordered_begin for it"<<endl;
		exit(0);
	}
	return ret;
}

/*posix io op*/
char*  replay_open(Event *e, int PTYPE){
	string temp = e->getCurrentCharValue(PTYPE);
	char ch[temp.length()];
	temp.copy(ch, temp.length(), 0);
	char *p = &ch[0];
	p += temp.length();
	*p = '\0';
	return ch;	
}

ssize_t replay_read_write(Event *e, Timer *timer, int op_code){
	ssize_t ret;
	void *buf;
	int count, fdindex, fd = -1;

	count = e->getCurrentValue(COUNT);
	fdindex = e->getCurrentValue(POSIX_FD);
	void *posixbuf = malloc(count);
	fd = index_to_fds_handle(replay_ptr, fdindex);
	if(fd < 0){
		cerr << "error: no such file opened for read and write"<<endl;
		exit(0);
	}
	if(op_code == umpi_read)
		ret = read(fd, posixbuf, count);
	else
		ret = write(fd, posixbuf, count);
	if(timer){
		timer->simulateComputeTime();
		timer->setTime();
	}	
	if(ret < 0){
		cerr << "error: read_write did not return success"<<endl;
		exit(0);
	}
	free(posixbuf);
	return ret;
}

int replay_close(Event *e, Timer *timer){
	int ret = -1, fdindex, fd = -1;
	fdindex = e->getCurrentValue(POSIX_FD);
	fd = index_to_fds_handle(replay_ptr, fdindex);
	if(fd < 0){
		cerr << "error: no such file opened for close"<<endl;
		exit(0);
	}
	ret = close(fd);
	if(timer){
		timer->simulateComputeTime();
		timer->setTime();
	}
	if(ret < 0){
		cerr << "error: close did not return success"<<endl;
		exit(0);
	}
	remove_fds_handle_entry(replay_ptr, fd);
	return ret;
}

int replay_fclose(Event *e, Timer *timer){
	int ret, fpindex;
	FILE *fp = NULL;
	fpindex = e -> getCurrentValue(POSIX_FD);
	fp = index_to_ffiles_handle(replay_ptr, fpindex);
	if(fp == NULL){
		cerr << "error: no such file fopened for fclose"<<endl;
		exit(0);	
	}
	ret = fclose(fp);
	if(timer){
		timer->simulateComputeTime();
		timer->setTime();
	}
	if(ret < 0){
		cerr << "error: fclose did not return success"<<endl;
		exit(0);
	}
	remove_ffiles_handle_entry(replay_ptr, fp);
	return ret;

}

off_t replay_lseek(Event *e, Timer *timer, int op_code){
	off_t ret = -1, offset=0;
	int fdindex, fd = -1, whence;
	offset = e->getCurrentValue(OFFSET);
	whence = e->getCurrentValue(WHENCE);
	fdindex = e->getCurrentValue(POSIX_FD);
	fd = index_to_fds_handle(replay_ptr, fdindex);
	if(fd < 0){
		cerr << "error: no such file opened for lseek"<<endl;
		exit(0);
	}

	if(timer)
		timer->simulateComputeTime();
	if(op_code == umpi_lseek)
		ret = lseek(fd, offset, whence);
	else
		ret = lseek64(fd, offset, whence);
	if(timer)
		timer->setTime();
	if(ret < 0){
		cerr <<"error: lseek did not return success"<<endl;
		exit(0);
	}
	return ret;
}

int replay_creat(Event *e, Timer *timer){
	int ret = -1;
	mode_t mode;
	char *ch;
	
	ch = replay_open(e, FILENAME);
	mode = e->getCurrentValue(POSIX_MODE);
	if(timer)
		timer->simulateComputeTime();
	ret = creat(ch, mode);
	if(timer)
		timer->setTime();
	if(ret < 0){
		cerr << "error: creat did not return success"<<endl;
		exit(0);
	}
	return ret;
}

int replay_dup(Event *e, Timer *timer, int op_code){
	/*int ret, fd, nowfd, newfd;
	fd = e->getCurrentValue(POSIX_OLDFD);
	if(it != posixfd.end())
		nowfd = it->second;
	else{
		cerr << "error: no such files opened for dup"<<endl;
		exit(0);
	}
	if(op_code == umpi_dup){
		if(timer)
			timer->simulateComputeTime();
		ret = dup(nowfd);
		if(timer)
			timer->setTime();
	}
	else{
		newfd = e->getCurrentValue(POSIX_NEWFD);
		if(timer)
			timer->simulateComputeTime();
		ret = dup2(nowfd, newfd);
		if(timer)
			timer->setTime();		
	}
	if(ret<0){
		cerr << "error: dup did not return success"<<endl;
		exit(0);
	}
	return ret;*/
}

int replay_fcntl(Event *e, Timer *timer){
	int ret, fdindex, fd = -1, cmd;
	fdindex = e->getCurrentValue(POSIX_FD);
	cmd = e->getCurrentValue(POSIX_CMD);
	fd = index_to_fds_handle(replay_ptr, fdindex);
	if(fd < 0){
		cerr << "cerror: no such files opened for fcntl"<<endl;
		exit(0);
	}
	if(timer)
		timer->simulateComputeTime();
	ret = fcntl(fd, cmd);
	if(timer)
		timer->setTime();
	if(ret < 0){
		cerr << "error: fcntl did not return success"<<endl;
		exit(0);
	}
	return ret;
}

int replay_cart_create(Event *e, Timer *timer){
	int ret,count,commout,reorder;
	MPI_Comm comm, comm_out;
	count = e->getCurrentValue(COUNT);
	int *array_of_dims = e->getCurrentValues(ARRAY_OF_DIMS, count);
	int *array_of_periods = e->getCurrentValues(ARRAY_OF_PERIODS, count);
	reorder = e->getCurrentValue(REORDER);
	comm = (MPI_Comm)index_to_comm(replay_ptr, e->getCurrentValue(COMM));
	commout = e->getCurrentValue(COMM_OUT);
	if(timer)
		timer->simulateComputeTime();
	ret = MPI_Cart_create(comm, count, array_of_dims, array_of_periods, reorder, &comm_out);
	if(timer)
		timer->setTime();
	if(ret < 0){
		cerr <<"error: MPI_Cart_create did not return success"<<endl;
		exit(0);
	}
	add_comm_entry(replay_ptr, comm_out);
	assert(comm_to_index(replay_ptr, comm_out) == commout);
	return ret;

}

int replay_type_contiguous(Event *e, Timer *timer){
	int ret, count, new_type;
	MPI_Datatype oldtype, newtype;
	count = e->getCurrentValue(COUNT);
	oldtype = (MPI_Datatype)index_to_type(replay_ptr, e->getCurrentValue(DATATYPE));
	new_type = e->getParamValueForRank(NEWTYPE, my_rank);
	if(timer)
		timer->simulateComputeTime();
	ret = MPI_Type_contiguous(count, oldtype, &newtype);
	MPI_Type_commit(&newtype);
	if(timer)
		timer->setTime();
	if(ret != MPI_SUCCESS){
		cerr <<"error: MPI_Type_contiguous did not return success"<<endl;
		exit(0);
	}
	add_type_entry(replay_ptr, newtype);
	assert(type_to_index(replay_ptr, newtype) == new_type);
	return ret;
}

int replay_type_create_subarray(Event *e, Timer *timer){
	int ret, count, order, new_type;
	MPI_Datatype oldtype, newtype;
	count = e->getCurrentValue(COUNT);
	order = e->getCurrentValue(ORDER);
	new_type = e->getCurrentValue(NEWTYPE);
	oldtype = (MPI_Datatype)index_to_type(replay_ptr, e->getCurrentValue(DATATYPE));
	int *array_of_sizes = e->getParamValuesForRank(ARRAY_OF_SIZES, count, my_rank);
	int *array_of_subsizes = array_of_subsizes = e->getParamValuesForRank(ARRAY_OF_SUBSIZES, count, my_rank);
	int *array_of_starts = e->getParamValuesForRank(ARRAY_OF_STARTS, count, my_rank);
	if(timer)
		timer->simulateComputeTime();
	ret = MPI_Type_create_subarray(count, array_of_sizes, array_of_subsizes, array_of_starts, order, oldtype, &newtype);
	MPI_Type_commit(&newtype);
	if(timer)
		timer->setTime();
	if(ret != MPI_SUCCESS){
		cerr <<"error: MPI_Type_create_subarray did not return success"<<endl;
		exit(0);
	}
	add_type_entry(replay_ptr, newtype);
	assert(type_to_index(replay_ptr, newtype) == new_type);
	return ret;
}
int replay_type_free(Event *e, Timer *timer){
	int ret, new_type;
	MPI_Datatype newtype;
	new_type = e->getCurrentValue(NEWTYPE);
	newtype = (MPI_Datatype)index_to_type(replay_ptr, new_type);
	remove_type_entry(replay_ptr, newtype);
	if(timer)
		timer->simulateComputeTime();
	ret = MPI_Type_free(&newtype);
	if(timer)
		timer->setTime();
	if(ret != MPI_SUCCESS){
		cerr <<"error: MPI_Type_free did not return success"<<endl;
		exit(0);
	}
	return ret;
}


int replay_type_create_darray(Event *e, Timer *timer){
	int ret, count, order, size, rank;
	MPI_Datatype datatype, newtype;
	size = e->getCurrentValue(SIZE);
	rank = e->getParamValueForRank(RANK,my_rank);
	count = e->getCurrentValue(COUNT);
	order = e->getCurrentValue(ORDER);
	int new_type = e->getParamValueForRank(NEWTYPE,my_rank);
	datatype = (MPI_Datatype)index_to_type(replay_ptr, e->getCurrentValue(DATATYPE));
	int *array_of_gsizes = e->getParamValuesForRank(ARRAY_OF_GSIZES, count, my_rank);
	int *array_of_distribs = e->getParamValuesForRank(ARRAY_OF_DISTRIBS, count, my_rank);
	int *array_of_dargs = e->getParamValuesForRank(ARRAY_OF_DARGS, count, my_rank);
	int *array_of_psizes = e->getParamValuesForRank(ARRAY_OF_PSIZES, count, my_rank);
	if(timer)
		timer->simulateComputeTime();
	ret = MPI_Type_create_darray(size, rank, count, array_of_gsizes, array_of_distribs, array_of_dargs, array_of_psizes, order, datatype, &newtype);
	if(timer)
		timer->setTime();
	if(ret != MPI_SUCCESS){
		cerr <<"error: MPI_Type_create_darray did not return MPI_SUCCESS"<<endl;
		exit(0);
	}
	MPI_Type_commit(&newtype);
	add_type_entry(replay_ptr, newtype);
	assert(type_to_index(replay_ptr, newtype) == new_type);
	return ret;

}

int replay_remove(Event *e, Timer *timer){
	int ret;
	string temp;
	vector<string> *tem;
	temp = e -> getCurrentCharValue(FILENAME);
	char ch[temp.length()];
	temp.copy(ch, temp.length(), 0);
	char *p = &ch[0];
	p += temp.length();
	*p = '\0';

	ret = remove(&ch[0]);
	if(timer){
		timer->simulateComputeTime();
		timer->setTime();
	}
	if(ret < 0){
		cerr << "error: remove did not return success"<<endl;
		//exit(0);
	}
	return ret;
}
int replay_fread_fwrite(Event *e, Timer *timer, int op_code){
	int count, size, ret, fpindex;
	FILE *fp;

	count = e->getCurrentValue(COUNT);
	size = e ->getCurrentValue(SIZE);
	fpindex = e->getParamValueForRank(POSIX_FD, my_rank);
	fp = index_to_ffiles_handle(replay_ptr, fpindex);
	void *posixbuf = malloc(count*size);
	if(fpindex < 0 || fp == NULL){
		cerr << "error: no such file opened for read and write"<<endl;
		//exit(0);
		return 0;
	}
	if(op_code == umpi_fread)
		ret = fread(posixbuf, size, count, fp);
	else
		ret = fwrite(posixbuf, size, count, fp);
	if(timer){
		timer->simulateComputeTime();
		timer->setTime();
	}	
	if(ret < 0){
		cerr << "error: fread_fwrite did not return success"<<endl;
		exit(0);
	}
	free(posixbuf);
	return ret;
}
int replay_fseek(Event *e, Timer *timer){
	long offset=0;
	int whence, fpindex, ret;
	FILE *fp;
	fpindex = e->getCurrentValue(POSIX_FD);
	fp = index_to_ffiles_handle(replay_ptr, fpindex);
	offset = e->getCurrentValue(OFFSET);
	ret = fseek(fp, offset, whence);
	if(timer){
		timer->simulateComputeTime();
		timer->setTime();
	}
	if(ret < 0){
		cerr << "error: fseek did not return success"<<endl;
		exit(0);
	}
	return ret;
}



void switch_event(int op_code, Event *e, Timer *timer){
	switch(op_code){
		/* collectives */
		case umpi_MPI_Allgather:
			replay_Allgather(e, timer);
			break;
		case umpi_MPI_Allreduce:
			replay_Allreduce(e, timer);
			break;
		case umpi_MPI_Alltoall:
			replay_Alltoall(e, timer);
			break;
		case umpi_MPI_Barrier:
			replay_Barrier(e, timer);
			break;
		case umpi_MPI_Bcast:
			replay_Bcast(e, timer);
			break;
		case umpi_MPI_Reduce:
			replay_Reduce(e, timer);
			break;
		
		/* communicator ops */
		case umpi_MPI_Comm_create:
			replay_Comm_create(e, timer);
			break;
		case umpi_MPI_Comm_dup:
			replay_Comm_dup(e, timer);
			break;
		case umpi_MPI_Comm_free:
			replay_Comm_free(e, timer);
			break;
		case umpi_MPI_Comm_split:
			replay_Comm_split(e, timer);
			break;
		case umpi_MPI_Cart_create:
			replay_cart_create(e, timer);
			break;
		case umpi_MPI_Type_contiguous:
			replay_type_contiguous(e, timer);
			break;
		case umpi_MPI_Type_create_subarray:
			replay_type_create_subarray(e, timer);
			break;
		case umpi_MPI_Type_create_darray:
			replay_type_create_darray(e, timer);
			break;
		case umpi_MPI_Type_free:
			replay_type_free(e, timer);
			break;
		case umpi_MPI_Type_commit:
			break;
		/* group ops */
		case umpi_MPI_Comm_group:
			replay_Comm_group(e, timer);
			break;
		case umpi_MPI_Group_difference:
			replay_Group_difference(e, timer);
			break;
		case umpi_MPI_Group_excl:
			replay_Group_excl(e, timer);
			break;
		case umpi_MPI_Group_free:
			replay_Group_free(e, timer);
			break;
		case umpi_MPI_Group_incl:
			replay_Group_incl(e, timer);
			break;
		case umpi_MPI_Group_intersection:
			replay_Group_intersection(e, timer);
			break;
		case umpi_MPI_Group_range_excl:
			replay_Group_range_excl(e, timer);
			break;
		case umpi_MPI_Group_range_incl:
			replay_Group_range_incl(e, timer);
			break;
		case umpi_MPI_Group_union:
			replay_Group_union(e, timer);
			break;
		

		/* io ops */
		case umpi_MPI_File_open:
			replay_File_open(e, timer);
			break;
		case umpi_MPI_File_close:
			replay_File_close(e, timer);
			break;
		case umpi_MPI_File_delete:
			replay_File_delete(e, timer);
			break;
		case umpi_MPI_File_seek:
			replay_File_seek(e, timer);
			break;
		case umpi_MPI_File_set_view:
			replay_File_set_view(e, timer);
			break;
		case umpi_MPI_File_seek_shared:
			replay_File_seek_shared(e, timer);
			break;
		case umpi_MPI_File_set_size:
			replay_File_set_size(e, timer);
			break;
		case umpi_MPI_File_preallocate:
			replay_File_preallocate(e, timer);
			break;
		case umpi_MPI_File_sync:
			replay_File_sync(e, timer);
			break;
		case umpi_MPI_File_set_atomicity:
			replay_File_set_atomicity(e, timer);
			break;
		case umpi_MPI_File_c2f:
			replay_File_c2f(e, timer);
			break;
		case umpi_MPI_File_read:
		case umpi_MPI_File_iread:
		case umpi_MPI_File_write:
		case umpi_MPI_File_iwrite:
		case umpi_MPI_File_read_shared:
		case umpi_MPI_File_iread_shared:
		case umpi_MPI_File_write_shared:
		case umpi_MPI_File_iwrite_shared:
		case umpi_MPI_File_read_all:
		case umpi_MPI_File_write_all:
		case umpi_MPI_File_read_all_begin:
		case umpi_MPI_File_write_all_begin:
		case umpi_MPI_File_read_ordered:
		case umpi_MPI_File_write_ordered:
		case umpi_MPI_File_read_ordered_begin:
		case umpi_MPI_File_write_ordered_begin:
			replay_File_read_write(e, timer, op_code);
			break;
		case umpi_MPI_File_write_at:
		case umpi_MPI_File_write_at_all:
		case umpi_MPI_File_read_at:
		case umpi_MPI_File_read_at_all:
		case umpi_MPI_File_iread_at:
		case umpi_MPI_File_iwrite_at:
		case umpi_MPI_File_read_at_all_begin:
		case umpi_MPI_File_write_at_all_begin:
			replay_File_read_write_at(e, timer, op_code);
			break;
		case umpi_MPI_File_read_at_all_end:
			replay_File_read_at_all_end(e, timer);
			break;
		case umpi_MPI_File_write_at_all_end:
			replay_File_write_at_all_end(e, timer);
			break;
		case umpi_MPI_File_read_all_end:
			replay_File_read_all_end(e, timer);
			break;
		case umpi_MPI_File_write_all_end:
			replay_File_write_all_end(e, timer);
			break;
		case umpi_MPI_File_read_ordered_end:
			replay_File_read_ordered_end(e, timer);
			break;
		case umpi_MPI_File_write_ordered_end:
			replay_File_write_ordered_end(e, timer);
			break;
		case umpi_read:
		case umpi_write:
			replay_read_write(e, timer, op_code);
			break;
		case umpi_close:
			replay_close(e, timer);
			break;
		case umpi_lseek:
		case umpi_lseek64:
			replay_lseek(e, timer, op_code);
			break;
		case umpi_creat:
			replay_creat(e, timer);
			break;
		case umpi_dup:
		case umpi_dup2:
			replay_dup(e, timer, op_code);
			break;
		case umpi_fcntl:
			replay_fcntl(e, timer);
			break;
		case umpi_remove:
			replay_remove(e, timer);
			break;
		case umpi_fclose:
			replay_fclose(e, timer);
			break;
		case umpi_fread:
		case umpi_fwrite:
			replay_fread_fwrite(e, timer, op_code);
			break;
		case umpi_fseek:
			replay_fseek(e, timer);
			break;
		default:
			cerr << "error: unsupported event " << op_code << " on rank " << my_rank << endl;
			cerr << e->toString() << endl;
			exit(0);
			break;
	}
	//free(buf);
}


int replay_event(Event *e, int depth, int iteration){
	int op_code;

	if(replay_timer)
		replay_timer->setCurrentEvent(e);
	op_code = e->getOp();
	switch(op_code){

		case umpi_MPI_Finalize:
			finalize_req(replay_req);
			finalize_mem();
			finalize_ptr (replay_ptr);
			delete replay_timer;
			break;

		case umpi_MPI_Init:
			init_req(&replay_req, REQ_BUF_SIZE);
			init_mem(REQ_BUF_SIZE);
			replay_ptr = init_ptr (PTR_HANDLER_SZ);
			replay_timer = new Timer();
			replay_timer->setCurrentEvent(e);
			replay_timer->setTime();
			break;

		case umpi_MPI_Irecv:
			replay_Irecv(e, replay_timer);
			break;

		case umpi_MPI_Recv:
			replay_Recv(e, replay_timer);
			break;

		case umpi_MPI_Isend:
			replay_Isend(e, replay_timer);
			break;

		case umpi_MPI_Send:
			replay_Send(e, replay_timer);
			break;

		case umpi_MPI_Wait:
			replay_Wait(e, replay_timer);
			break;

		case umpi_MPI_Waitall:
			replay_Waitall(e, replay_timer);
			break;

		default:
			switch_event(op_code, e, replay_timer);
			break;
	}

	return 0;
}
