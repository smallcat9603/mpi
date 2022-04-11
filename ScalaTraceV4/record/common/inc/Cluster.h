#include <vector>
#include <stdlib.h>
#include <map>
#include <iostream>
#include <fstream>
#include <string>
#include "Ranklist.h"
#include <bitset>
#include <math.h>
#include <float.h>
#define CALL_PATH 0
#define AVG_COUNT 1
#define AVG_COMM_TIME 2
#define AVG_COMP_TIME 3
#define D_SRC 4
#define D_DEST 5
#define D_TAG 6
#define D_COLOR 7
#define D_KEY 8
#define D_DATA_OP 9
#define AVG_LOOP_ITER 10
#define NUM_CLUSTERING 11

using namespace std;

class Cluster{

private:
	Ranklist ranklist;

	short int SIG_FORMAT;	
	int K;
	int L;
	int MergeScore;
	double weight_CallPath;
	double weight_Count;
	double weight_AvgCommTime;
	double weight_AvgCompTime;
	double weight_SRC;
	double weight_DEST;
	double weight_TAG;
	double weight_COLOR;
	double weight_KEY;
	double weight_DATA_OP;
	double weight_AvgLoopIter;

	int Group1_Weight;
	int Group2_Weight;
	int Group3_Weight;
	int Group4_Weight;

	double SD_AvgCount;
	double SD_AvgCommTime;
	double SD_AvgCompTime;
	double SD_AvgLoopIter;
	long long globalAvgCount;
	long long globalAvgCommTime;
	long long globalAvgCompTime;
	long long globalFreqCallPath;
	long long globalFreqSRC;
	long long globalFreqDEST;
	long long globalFreqTAG;
	long long globalFreqCOLOR;
	long long globalFreqKEY;
	long long globalFreqDATA_OP;
	long long globalAvgLoopIter;

	long long freqCallPath;
	long long freqSRC;
	long long freqDEST;
	long long freqTAG;
	long long freqCOLOR;
	long long freqKEY;
	long long freqDATA_OP;
		
	long long CallPath;
	long long AvgCount;
	long long AvgSRC;
	long long AvgDEST;
	long long AvgTAG;
	long long AvgCOLOR;
	long long AvgKEY;
	long long DATA_OP;
	long long AvgCompTime;
	long long AvgCommTime;
	long long AvgLoopIter;

	short int cmp_Format_Signature;

	map<long long , Ranklist> Call_Path_Clustering;
	map<long long , Ranklist> AvgCount_Clustering;
	map<long long , Ranklist> AvgCompTime_Clustering;
	map<long long , Ranklist> AvgCommTime_Clustering;
	map<long long , Ranklist> AvgSRC_Clustering;
	map<long long , Ranklist> AvgDEST_Clustering;
	map<long long , Ranklist> AvgTAG_Clustering;
	map<long long , Ranklist> AvgCOLOR_Clustering;
	map<long long , Ranklist> AvgKEY_Clustering;
	map<long long , Ranklist> DATA_OP_Clustering;
	map<long long , Ranklist> AvgLoopIter_Clustering;


	map<long long , Ranklist> recv_Call_Path_Clustering;
	map<long long , Ranklist> recv_AvgCount_Clustering;
	map<long long , Ranklist> recv_AvgCompTime_Clustering;
	map<long long , Ranklist> recv_AvgCommTime_Clustering;
	map<long long , Ranklist> recv_AvgSRC_Clustering;
	map<long long , Ranklist> recv_AvgDEST_Clustering;
	map<long long , Ranklist> recv_AvgTAG_Clustering;
	map<long long , Ranklist> recv_AvgCOLOR_Clustering;
	map<long long , Ranklist> recv_AvgKEY_Clustering;
	map<long long , Ranklist> recv_DATA_OP_Clustering;
	map<long long , Ranklist> recv_AvgLoopIter_Clustering;

	map<int, Ranklist> recvClusterHeads;
	map<int, Ranklist> recvClusterHeads_Left;
	map<int, Ranklist> recvClusterHeads_Right;
	map<int, Ranklist> ClusterHeads;
	map<int, Ranklist> CallPathClusterHeads;
	
 	void deleteMap(map<long long, Ranklist> &Cluster);
	int * unrolled_rank;
	int unrolled_rank_size;
	int clusterID;


public:
	Cluster() {}
	//Cluster(int rank){SIG_FORMAT=0; cmp_Format_Signature=0; Ranklist ranklist_temp(rank); ranklist=ranklist_temp; ranklist.toString(); }
	~Cluster();
	
	void setCallPathHeads(map<int, Ranklist> &Heads){
		map<int, Ranklist>::iterator it;
		for(it=Heads.begin(); it!=Heads.end(); it++)
			CallPathClusterHeads[it->first] = it->second;
	}
	void initClusterRanklist(int rank);
	void setCallPath(long long callPathSig)
		{CallPath=callPathSig;}
	void setAvgCount(long long avgCountSig)
		{AvgCount=avgCountSig;}
	void setAvgCommTime(long long avgCommTimeSig)
		{AvgCommTime=avgCommTimeSig;}
	void setAvgCompTime(long long avgCompTimeSig)
		{AvgCompTime=avgCompTimeSig;}
	void setAvgSRC(long long AvgSRC_Sig)
		{AvgSRC=AvgSRC_Sig;}
	void setAvgDEST(long long AvgDEST_Sig)
		{AvgDEST=AvgDEST_Sig;}
	void setAvgTAG(long long AvgTAG_Sig)
		{AvgTAG=AvgTAG_Sig;}
	void setAvgCOLOR(long long AvgCOLOR_Sig)
		{AvgCOLOR=AvgCOLOR_Sig;}
	void setAvgKEY(long long AvgKEY_Sig)
		{AvgKEY=AvgKEY_Sig;}
	void setDATA_OP(long long DATA_OP_Sig)
		{DATA_OP=DATA_OP_Sig;}
	void setAvgLoopIter(long long avgLoopIterSig)
		{AvgLoopIter=avgLoopIterSig;}

	int getNumCallPath(){
		return Call_Path_Clustering.size();
	}

	long long getCallPath()
		{return CallPath;}
	long long getAvgCount()
		{return AvgCount;}
	long long getAvgCommTime()
		{return AvgCommTime;}
	long long getAvgCompTime()
		{return AvgCompTime;}
	long long getAvgSRC()
		{return AvgSRC;}
	long long getAvgDEST()
		{return AvgDEST;}
	long long getAvgTAG()
		{return AvgTAG;}
	long long getAvgCOLOR()
		{return AvgCOLOR;}
	long long getAvgKEY()
		{return AvgKEY;}
	long long getDATA_OP()
		{return DATA_OP;}
	long long getAvgLoopIter()
		{return AvgLoopIter;}

	void setRanklist(Ranklist& rl){ 
		ranklist = rl;		
	}

	void mergeSigFormat(short int recv_SIG_FORMAT){
		SIG_FORMAT |= recv_SIG_FORMAT;	
	}

	void setSigFormat(short int temp_SIG_FORMAT){
		SIG_FORMAT = temp_SIG_FORMAT;	
	}

	short int getSigFormat(){
		return SIG_FORMAT;	
	}

	Ranklist getRanklist(){
		return ranklist;
	}
	
	void sendSigFormat(int dest);
	void recvSigFormat(int recv);

	void sendSignature(int dest);
	void recvSignature(int recv, int my_rank);

	void sendClusterFormat(int dest);
	void recvClusterFormat(int src, int my_rank);
	void mergeClusterFormat();
		
	void sendSigClusters(int dest);
	void recvSigClusters(int src);

	void initSigClusters();

	void pack(void *buf, int bufsize, int *position, MPI_Comm comm, int clusterType);
	void unpack(void *buf, int bufsize, int *position, MPI_Comm comm, int ClusterType);

	void outputSigClusters(const char* filename);
	
	void mergeSigClusters();
		
	int* getClusterPackedSize();
	int getKClusterPackedSize();
	
	void addCluster();
	Ranklist getClusterRanklist(int rank);
	int onlyHeadCluster(int rank);

	void BcastClusterFormat(int rank);
	void BcastKCluster(int rank);
	void BcastCallPathCluster(int rank);
	void initSigClusters_L2(int my_rank, int status);
	void calcGlobalAvg(int rank);	
	void setK(int tempK){
		K=tempK;
	}
	
	void printSignatures(); 
	void sendClusterHeadstoRoot(int my_rank);
	int convertUnrolledRank(int position){ return unrolled_rank[position];}
	int findUnrolledRank(int rank);
	int findUnrolledSize() { return unrolled_rank_size; }
	void setUnrolledRank(int * temp, int size){
		unrolled_rank= new int [size];
		memcpy(unrolled_rank, temp, size * sizeof(int));
		unrolled_rank_size=size;
	}
	void copyUnrolledRank(int * temp){
		for(int index=0; index < unrolled_rank_size; index++)	
			temp[index] = unrolled_rank[index]; 	
	}

	void setClusterID(int CID){
		clusterID=CID;
	}
	void setLevel(int tempL){
		L=tempL;
	}
	void recvKClusters(int src, int child);
	void sendKClusters(int dest);
	void mergeKClusters(int myrank);
	void merge(map<int, Ranklist> &LHeads, map<int, Ranklist> &RHeads, int myrank);
	bool checkRelative(map<long long, Ranklist>::iterator it, int type);	
	void findAndReduce(Ranklist rankSet, int val);
	void findMinScore(int &tempKey, int &tempVal);
	void findMySig(map<long long, Ranklist> &myMap, long long &sig_i, int rank );
	double findDist(int i, int j);
	double calcDist(long long sig_i, long long sig_j, int weight);
	vector<int> getArrayClusterHeads();
 	//void mergeCluster(map<long long int, Ranklist> *clusters );
	void findWeight(int &w1, int &w2, int &w3, int &w4);
	long long findVariation(map<long long, Ranklist> &myMap);
	void formClusters(map<int, Ranklist> &LHeads,  map<long long , Ranklist>::iterator it, int myrank, unsigned int little_k);
	void findTopK_SRC_DEST(map<int, Ranklist> &Heads);


	int getunrolled_rank_size(){
		return unrolled_rank_size;
	}

};
