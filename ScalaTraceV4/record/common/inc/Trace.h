/**
 *	Author: Xing Wu
 *	
 *	A Trace is a doubly linked list of Events
 */

#ifndef __TRACE_H__
#define __TRACE_H__

#ifdef __cplusplus

#include "Event.h"
#include "Stat.h"
#include <fstream>
#include <string>
#include <iostream>
#include <mpi.h>
#include <vector>
#include <map>
#include <list>
#include <climits>
#include <algorithm>
#include <time.h>
#include <cstdlib>
#include "Cluster.h"
#define WINDOW_SIZE 5000
#define MAX_LENGTH_DIFF 0.2
#define CALL_PATH_PARAMETER_CLUSTERING 1
#define REFERENCE_CLUSTERING 2
typedef int (event_handler)(Event *e, int depth, int iter);


using namespace std;

class Event;

enum{
	DIRECTION_UP,
	DIRECTION_LEFT,
	DIRECTION_UL
};

typedef struct matching_pair {
	int index1; /* merge */
	int index2; /* target or slave */
} matching_pair_t;

typedef struct lphd{
	Event *event;
	int pos;
} loophead_t;

typedef struct iteration{
	Event *merge_head;
	Event *merge_tail;
	int merge_length;
	Event *target_head;
	Event *target_tail;
	int target_length;
} iteration_t;

typedef multimap<Event*, iteration_t*> iterationmap_t;
typedef multimap<Event*, iteration_t*>::iterator iterationmap_iterator_t;

class PendingIterations{
	private:
		iterationmap_t mh_key;
		iterationmap_t mt_key;
		iterationmap_t th_key;
		iterationmap_t tt_key;

	public:
		PendingIterations();
		~PendingIterations();

		void insert(iteration_t* iteration);
		void create(Event* merge_head, Event* merge_tail, int merge_length,
						Event* target_head, Event* target_tail, int target_length);

		vector<iteration_t *> getIterationsByMergeHead(Event* merge_head);
		vector<iteration_t *> getIterationsByMergeTail(Event* merge_tail);
		vector<iteration_t *> getIterationsByTargetHead(Event* target_head);
		vector<iteration_t *> getIterationsByTargetTail(Event* target_tail);

		void updateIterationMergeHead(iteration_t* iteration, Event* new_merge_head);
		void updateIterationMergeTail(iteration_t* iteration, Event* new_merge_tail);
		void updateIterationTargetHead(iteration_t* iteration, Event* new_target_head);
		void updateIterationTargetTail(iteration_t* iteration, Event* new_target_tail);

		void deleteIteration(iteration_t* iteration);

		void print();

};

class Trace{
	private:
		Event* head;
		Event* tail;
		Event* lastTail; // Clustering: tail event of the last iteration

		int cnt_AT;
		int cnt_L;
		int cnt_C;
		
		int MarkerCommValue;

		int outerLoopHead_ID;
		int PRSD_flag_cluster;
		bool ReClusteringFlag;
		bool NoClustering;
		bool InterCompression;
		long int NumEvents;		
		long int OldNumEvents;		
		long int NumIters;		

		long long DeltaTime;
		long long AvgCountSignature;
		long long CallPathSignature;
		long long ParameterSignature;
		map<int, map<int, long long int> > ReferenceSignature;
		Ranklist ranklist;
		int my_rank;
		map<long long int, Ranklist> clusters;
		map<long long int, Ranklist> recv_clusters;
		map<int, map<int, map<int, long long int> > >refClusters;
		map<int, map<int, map<int, long long int> > >recvRefClusters;
		list <Ranklist> RLRefClustering;
		list <Ranklist> recvRLRefClustering;

		Cluster * myCluster, * tempCluster;
		long long OldCallPath;
		long long OldCount;
		long long OldDeltaTime;

		int RefSize;
		int clusteringType;
		int * unrolled_rank;
		int unrolled_rank_size;
		int * cluster_ranks;
		int cluster_size;
		int * interClusterRL;
		int interClusterRLSize;
		int clusterID;
		int numClusters;
		clock_t t;	
		clock_t SlackTime;	
		int f;


		PendingIterations pendingIterations;

		/* ***********************************************************************
		 * the following functions are used for intra-node compression 
		 * ***********************************************************************/

		/* the following three functions are used for regular loop compression which
		 * requires loop iterations to be exactly matching */
		void fastCompressLoop(int window);
		bool matchLoop(Event* merge_head, Event* merge_tail, int merge_length,
		Event *target_head, Event* target_tail, int target_length);
		void mergeLoopEvents(Event* merge_head, Event* merge_tail, Event *target_head, Event* target_tail);
		void updateLoopInfo(Event* merge_head, Event* merge_tail, Event *target_head, Event *target_tail, int merge_length);
		void deleteTail(Event* merge_tail);

		/* the following functions are used for advanced loop compression which 
		 * performs longest common subsequence analysis for consecutive loop
		 * iterations */
		void slowCompressLoop(int window);
		int realLength(Event* head, Event* tail);
		int realLengthHelper(iteration_t* iteration, int bound);
		void updateIds(Event* from);
		iteration_t* compressLoopLCS(iteration_t* iteration);
		vector<matching_pair_t *>* matchLoopLCS(Event* merge_head, Event* merge_tail, int merge_length,
				Event *target_head, Event* target_tail, int target_length);
		void insertEvent(Event* e, Event *position);
		int updateLoopLCS(Event* merge_head, Event* merge_tail, int merge_length, Event* target_head, Event* target_tail, vector<matching_pair_t *>* pairs);
		void deleteIteration(Event *target_head, Event* target_tail);


		/* ***********************************************************************
		 * the following functions are used for inter-node compression 
		 * ***********************************************************************/
		void get_lcs_helper(int *b, int i, int j, int width, vector<matching_pair_t *> *pairs);
		vector<matching_pair_t *> * getLCS(int *b, int mlen, int slen);
		void LCSDoMerge(vector<matching_pair_t *> *pairs, Trace* slave);
		bool traverse_recursive(Event* loophead, int depth, event_handler handler, int *len);

#ifdef FEATURE_CODE_GENERATOR
		bool code_gen_traverse_recursive(Event* loophead, int depth, event_handler handler, int *len, int loop_depth);
#endif

	public:
		Trace();
		~Trace();
			
		void incNumIters(){
			NumIters++;		
		}
		long int getNumIters(){
			return NumIters;
		}

//		static void deleteTrace(Trace *trace);
		Event* getHead(){
			return head;
		}
		void setHead(Event *e){
			head = e;
		}
		Event* getTail(){
			return tail;
		}
		void incNumEvents(){
			NumEvents++;		
		}

		void resetNumEvents(){
			NumEvents=0;		
		}
		
		long int getNumEvents(){
			return NumEvents;
		}

		void setOldNumEvents(int long input){
			OldNumEvents=input;		
		}

		long int getOldNumEvents(){
			return OldNumEvents;
		}

		void setMarkerCommValue(int input){
			if(MarkerCommValue==-1)
				MarkerCommValue=input;		
		}

		int getMarkerCommValue(){
			return MarkerCommValue;
		}



		void setRefSize(int size){RefSize=size;}
		int getRefSize() {return RefSize;}
		void deleteTrace(Trace *trace);

		void setTail(Event *e){
			tail = e;
		}

		void setLastTail(Event *e){
			//ut << "LAST TAIL: " << e->toString() << endl;
			if(e){ //==0
				lastTail = e;
				//cout << "saved the tail!!!: " << e->toString() << endl;	
			}
		
		} 
		Event* getLastTail(){
			return lastTail;
		}

		void setOuterLoopHead(int _ID){
			outerLoopHead_ID = _ID;
		} 
		int getOuterLoopHead(){
			return outerLoopHead_ID;
		}

		int getPRSDClusterFlag();
		
		void setPRSDClusterFlag(){
			PRSD_flag_cluster = 1;
		}

		void resetPRSDClusterFlag(){
			PRSD_flag_cluster = 0;
		}
		
		void setReClusteringFlag(bool recl){
			ReClusteringFlag = recl;
		}

		void setInterCompressionFlag(bool flag){
			InterCompression = flag;
		}

		bool getInterCompressionFlag(){
			return InterCompression;
		}

		void setNoClusteringFlag(bool stat){
			NoClustering = stat;
		}

		bool getNoClusteringFlag(){
			return NoClustering;
		}


		bool getReClusteringFlag(){
			return ReClusteringFlag; 
		}

		int traceLen(){
			int i = 0;
			Event *iter = head;
			while(iter){
				i++;
				iter = iter->next;
			}
			return i;
		}
		void resetTraversalIterators();

		void appendEvent(Event* event, bool compress = true);
		void finalizePendingIterations();
		void cleanupLoops();

		static Trace* inputTrace(const char* filename, int mode, int rank = -1);
		void outputTrace(const char* filename);
		void printTrace();

		int getPackedSize();
		void pack(void *buf, int bufsize, int *postion, MPI_Comm comm);
		void unpack(void *buf, int bufsize, int *postion, MPI_Comm comm);

		void sendTrace(int dest);
		void recvTrace(int src);
		void LCSMergeTrace(Trace *slave);
		bool traverse(event_handler handler);


		/* Calc Average - Clustering */		
		template<typename T> 
			T findUniqueSig(vector<T> inputSet);
		template<typename T>
			T calcAverage(vector<T> input){
				int n = input.size();
				T avg=0;
				if (n==0)
					return avg;

				typename vector<T>::iterator it;
				int cnt =1;
				int Old_Sum=0;
				it=input.begin();
				avg=Old_Sum = (*it);
				if (n==1)
					return avg;

				++it;
				for (; it!=input.end(); ++it){
					avg= Old_Sum - ((Old_Sum - (*it))/++cnt);
					Old_Sum =avg;
				}
				return avg;
			}

		void setDeltaTime(long long S_Sig)
		{DeltaTime=S_Sig;}

                void addDeltaTime(long long t)
                {DeltaTime+=t;}

                void resetDeltaTime()
                {DeltaTime=0;}


		void setAvgCountSignature(long long S_Sig)
		{AvgCountSignature=S_Sig;}

		void setCallPathSignature(long long S_Sig)
		{CallPathSignature=S_Sig;}
		void setParameterSignature(long long C_Sig)
		{ParameterSignature=C_Sig;}
		void setReferenceSignature(map<int, map<int, long long int> > R_Sig)
		{ReferenceSignature=R_Sig;}

		long long  getDeltaTime()
		{return DeltaTime;}

		long long  getAvgCountSignature()
		{return AvgCountSignature;}

		long long  getCallPathSignature()
		{return CallPathSignature;}
		long long  getParameterSignature()
		{return ParameterSignature;}
		map<int, map<int, long long int> > getReferenceSignature()
		{return ReferenceSignature;}
		void outputSignature(const char* filename);
		void createSignature(int rank, int comm_size );

		void expandClusterRanklist(Ranklist rl);
		void recvClusterRanklist();
		void sendClusterRanklist(int myrank);

		void callSignatures(const char *filename, int rank, int comm_size);
		Ranklist getRanklist(){
			return ranklist;
		}

		void setRanklist(Ranklist rl){ 
			ranklist=rl;			
		}

		void setUnrolledRank(int * temp, int size){
			unrolled_rank= new int [size];
			memcpy(unrolled_rank, temp, size * sizeof(int));
			unrolled_rank_size=size;
		}

		void setClusterRank(int * temp, int size){
			cluster_ranks= new int [size];
			memcpy(cluster_ranks, temp, size * sizeof(int));
			cluster_size= size;
		}

		void setInterClusterRL(int * temp, int size){
			interClusterRL= new int [size];
			memcpy(interClusterRL, temp, size * sizeof(int));
			interClusterRLSize= size;
		}

		void initInterClusterRL(){
			unrolled_rank=interClusterRL;
			unrolled_rank_size=interClusterRLSize;
			for (int index=0; index< unrolled_rank_size && unrolled_rank[0]!=0; index++){
				if (unrolled_rank[index]==0)
				{
					unrolled_rank[index]= unrolled_rank[0];
					unrolled_rank[0]=0;

				}

			}

		}

		/**/
		void printClusteringCounters(int rank){
			cout << "#### Chameleon: State AT: " << cnt_AT << " State C: " << cnt_C << " State L: " << cnt_L << endl; 
		} 
		void initMyCluster(int rank); 
		void onlineLCSDoMerge(vector<matching_pair_t *> *pairs, Trace *slave, int clusteringFlag);
		void onlineLCSMergeTrace(Trace *slave, int clusteringFlag);
		void onlineDeleteTail(Event* merge_tail, Event *slaveTail);
		void appendEventOnline(Event* event, bool compress); //, Event* slaveHead, Event* slaveTail);
		void onlineFastCompressLoop(int window); //, Event* slaveTail);
		void appendExistingEventsOnlineTrace(Trace *master, Trace *slave);
		int onlyHeadCluster(int rank){
				return (myCluster->onlyHeadCluster(rank));	
			}
			void BcastClusterFormat(int rank){
				myCluster->BcastClusterFormat(rank);	
			}	
			void recvClusterFormat(int src, int my_rank){
				myCluster->recvClusterFormat(src, my_rank);
			}
			void sendClusterFormat(int dest){
				myCluster->sendClusterFormat(dest);
			}
			void sendSigClusters(int dest){
				myCluster->sendSigClusters(dest);
			}
			void recvSigClusters(int src){
				myCluster->recvSigClusters(src);
			}
			void initSigClusters(){
				myCluster->initSigClusters();
			}
			void initSigClusters_L2(int my_rank, int status){
				myCluster->initSigClusters_L2(my_rank, status);
			}
			void mergeSigClusters(){
				myCluster->mergeSigClusters();
			}
			void outputSigClusters(char *filename){
				myCluster->outputSigClusters(filename);
			}
			void setK(int K){
				myCluster->setK(K);	
			}
			int getNumCallPath(){
				return (myCluster->getNumCallPath());
			}
			void setLevel(int Level){
				myCluster->setLevel(Level);
			}
			void recvKClusters(int src, int child){
				myCluster->recvKClusters(src, child);
			}
			void sendKClusters(int dest){
				myCluster->sendKClusters(dest);
			}
			void mergeKClusters( int myrank){
				myCluster->mergeKClusters(myrank);
			}
			void BcastKCluster(int rank){
				myCluster->BcastKCluster(rank);
			}
			void initIndirectRanklist(){
				vector<int> tmp = myCluster->getArrayClusterHeads();
				setUnrolledRank(&tmp[0], tmp.size());
			}

			void updateTraceRanklist(int my_rank){
				expandClusterRanklist(myCluster->getClusterRanklist(my_rank));
			}

			void bcastCluster(int rank); 

			void sendClusterHeadstoRoot(int my_rank){
				myCluster->sendClusterHeadstoRoot(my_rank);
			}

			/**/


			void updateOnlineLoopInfo(Event* merge_head, Event *merge_tail, Event *target_head, Event *target_tail, int merge_length);
			void packCluster(void *buf, int bufsize, int *position, MPI_Comm comm);
			void unpackCluster(void *buf, int bufsize, int *position, MPI_Comm comm);

			void outputOnlineTrace(const char* filename, Event * head);
			void outputCluster(const char* filename);
			void mergeCluster();
			int getClusterPackedSize();
			void setClusteringType(int type){clusteringType=type;}
			int getClusteringType(){return clusteringType;}
			void addCluster();
			void recvCluster (int src);
			void sendCluster (int dest);

			void sendCluster_P(int dest);
			void recvCluster_P(int dest);
			void mergeCluster_P();
			void initCluster(int rank);
			void findMyCluster(int rank);
			int findUnrolledRank(int rank)
			{
				return	myCluster->findUnrolledRank(rank);
			}
			int findUnrolledSize() { return myCluster->findUnrolledSize();}
			int convertUnrolledRank(int position){return myCluster->convertUnrolledRank(position);}
			void selectSamples(int rank);
			void sendSamples(int my_rank);
			void recvSamples();
			int checkSampleRanks(int my_rank);
			void setClusterID(int CID){
				clusterID=CID;
			}
			int getClusterID(){
				return clusterID;
			}
			void setNumberOfClusters(int num){
				numClusters=num;
			}


			int getNumberOfClusters(){
				return numClusters;
			}
			void recvClusterTrace(int CID);
			void sendClusterTrace();
			void checkInterClusterRL();
			void LinearMerge(Trace *slave);		
			void printParameters(const char *filename);

			void deleteCluster(){
				delete myCluster;
			};


			int matchSignatures(int my_rank, int comm_size, long int num_events);
//			int matchCallPathSignatures(int my_rank, int comm_size, long int num_events);
	
			void setOldDeltaTime(long long int temp){
				OldDeltaTime = temp;
			};

			void setOldCallPath(long long int temp){
				OldCallPath = temp;
			};
	
			void setOldCount(long long int temp){
				OldCount = temp;
			};
	
			long long int getOldCallPath(){
				return OldCallPath;
			};

			long long int getOldCount(){
				return OldCount;
			};

			long long int getOldDeltaTime(){
				return OldDeltaTime;
			};



			void startTime() {
				t = clock();
			};		
			clock_t endTime(){
				t = clock() - t;
				//printf ("Elapsed Time %d clicks (%f seconds).\n", (int)t,((float)t)/CLOCKS_PER_SEC);
				addTime();
				return t;
			};

			void addTime(){
				if ((int)t > 0){
					//printf ("Elapsed Time %d clicks (%f seconds).\n", (int)t,((float)t)/CLOCKS_PER_SEC);
					SlackTime += t;
					printf ("#### Chameleon: It took me %d clicks (%f seconds).\n", (int)SlackTime,((float)SlackTime)/CLOCKS_PER_SEC);
				}
			}
			

			void setOFFOnlineClusteringFlags(int rank);
			int getMyRank(){return my_rank; };
			void setMyRank(int rank){my_rank=rank; };


#ifdef FEATURE_CODE_GENERATOR
		Event* getLoopTail(Event* loophead, int depth);
		bool code_gen_traverse(event_handler handler);
#endif

};


extern "C" {
#endif /* __cplusplus */

typedef struct Trace Trace;

extern Trace trace, left_trace, right_trace, left_cluster, right_cluster, cluster, root_trace, left ,right, OnlineTrace, K_trace;

void appendEvent(Trace *trace, Event *event);

void finalizePendingIterations(Trace *trace);
void cleanupLoops(Trace *trace);

Trace* inputTrace(char *filename, int mode );
void outputTrace(Trace *trace, char *filename);

void sendTrace(Trace *trace, int dest);
void recvTrace(Trace *trace, int src);
void mergeTrace(Trace *master, Trace *slave);

//void deleteTrace(Trace *trace);
/**/
	void updateOuterLoop(Trace *trace, Trace *OnlineTrace);
	void mergeOnlineTrace(Trace *master, Trace *slave, int clusteringFlag);
	void appendExistingEventsOnlineTrace(Trace *master, Trace *slave);
	void sendClusterFormat(Trace *trace, int dest);
	void recvClusterFormat(Trace *trace, int src, int my_rank);
	void BcastClusterFormat(Trace *trace, int rank);
	void BcastKCluster(Trace *trace, int rank);
	void initIndirectRanklist(Trace *trae);
	void updateTraceRanklist(Trace *trace, int rank);
	int onlyHeadCluster(Trace *trace, int rank);
	void sendSigClusters(Trace *trace, int dest);
	void recvSigClusters(Trace *trace, int src);
	void initSigClusters(Trace *trace);
	void initSigClusters_L2(Trace *trace, int my_rank, int status);
	void mergeSigClusters(Trace *trace);
	void outputSigClusters(Trace *trace, char *filename);
	void setK(Trace *trace, int K);
	int getNumCallPath(Trace *trace);
	void setLevel(Trace *trace, int L);
	void recvKClusters(Trace *trace, int src, int child);
	void sendKClusters(Trace *trace, int dest);
	void mergeKClusters(Trace *trace,int myrank);
	void sendClusterHeadstoRoot(Trace *trace, int my_rank);
	
	int matchSignatures(Trace *trace, int my_rank, int comm_size, long int num_events);
        void incOuterLoopLen(Trace *trace);
	void setOuterLoopHead(Trace *OnlineTrace, Trace *trace);

	void resetPRSDClusterFlag(Trace *trace);
	void setPRSDClusterFlag(Trace *trace);
	int getPRSDClusterFlag(Trace *trace);
	int getNoClusteringFlag(Trace *trace);


	void saveLastTail(Trace *trace);
	void outputOnlineTrace(Trace *trace, const char* filename);
	void initMyCluster(Trace *trace, int my_rank);
	void initCluster(Trace *trace,  int rank);
	void sendCluster(Trace *trace, int dest);
	void recvCluster(Trace *trace, int dest);
	void mergeCluster(Trace *trace);
	void outputCluster(Trace *trace, char *filename);
	void bcastCluster(Trace *trace, int rank);
	void findMyCluster(Trace *trace, int rank);
	void setClusteringType(Trace *trace, int temp_SIGTYPE);
	int findUnrolledRank(Trace *trace, int rank);
	int convertUnrolledRank(Trace *trace, int position);
	int findUnrolledSize(Trace *trace);
	void callSignatures(Trace *trace, char *filename, int rank, int comm_size);
	void selectSamples(Trace *trace, int rank);
	void sendSamples(Trace *trace, int my_rank);
	void recvSamples(Trace *trace);
	int checkSampleRanks(Trace *trace, int my_rank);
	void sendClusterTrace(Trace *trace);
	void recvClusterTrace(Trace *trace, int CID);
	int getClusterID(Trace *trace);
	int getNumberOfClusters(Trace *trace);
	void initInterClusterRL(Trace *trace);
	void recvClusterRanklist(Trace *trace);
	void sendClusterRanklist(Trace *trace, int myrank);
	void deleteCluster(Trace *trace);
	void deleteTrace(Trace *trace);
	void LinearMerge(Trace *trace, Trace *slave);
	void startTime(Trace *trace);
	void endTime(Trace *trace);
	void printParameters(Trace *trace, char *filename);
	Event* tailEvent(Trace *trace);
	Event* headEvent(Trace *trace);
	void copyTrace(Trace *master, Trace *slave);
	void setOFFOnlineClusteringFlags(Trace *trace, int rank);
	void offNoClusteringFlag(Trace *trace);
	void resetNumEvents(Trace *trace, int rank);
	long int getNumEvents(Trace *trace, int rank);
	void printClusteringCounters(Trace *trace, int rank); 
	void setMarkerCommValue(Trace *trace, int input);
	int getMarkerCommValue(Trace *trace);



Event* tailEvent(Trace *trace);
Event* headEvent(Trace *trace);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /*__TRACE_H__*/
