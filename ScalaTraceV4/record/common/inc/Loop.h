/**
 * Author: Xing Wu
 */

#ifndef __LOOP_H__
#define __LOOP_H__

#include <vector>
#include <string>
#include <sstream>
#include "Param.h"
#include "paramtype.h"
#include <cassert>
#include <mpi.h>
#include "Ranklist.h"
using namespace std;

typedef struct {
	Param *mem;
	Param *iter;
} loop_t;

class Loop{

private:
	vector<loop_t *> loops;

public:
	Loop();
	~Loop();

	void insertLoop(int mem, int iter);
	void insertLoop(loop_t *loop);
	loop_t* popOuterLoop();
	vector<loop_t *> *getLoops();
	loop_t *getLoopAt(int depth);
	int getDepth();

	void mergeLoopStack(Loop* other);
	void interMergeLoopStack(Loop* other);

	void incLoopIter(int depth, int inc = 1);

	bool incLoopMem(int boundary);

	void cleanup();

	string toString();
	void input(string& buf, int mode, int rank);

	int getPackedSize();
	void pack(void *buf, int bufsize, int *position, MPI_Comm comm);
	void unpack(void *buf, int bufsize, int *position, MPI_Comm comm);

	int getMemForRank(int depth, int rank);
	int getIterForRank(int depth, int rank);

	void resetTraversalIterators();

	void setRanklist(Ranklist& rl){
	        for(unsigned i=0; i<loops.size(); i++){
        	  	loops[i]->iter->setRanklist(rl);
			loops[i]->mem->setRanklist(rl);
		}
	}

	void onlineMergeLoopStack(Loop* other);
	//void onlineMergeLoopStack2(Loop* other);
	void onlineInsertLoop(int mem, int iter, Ranklist rl);
};

#endif /*__LOOP_H__*/
