#include "Cluster.h"


Cluster::~Cluster(){
	deleteMap(Call_Path_Clustering);
	deleteMap(AvgCount_Clustering);
	deleteMap(AvgCommTime_Clustering);
	deleteMap(AvgCompTime_Clustering);
	deleteMap(AvgSRC_Clustering);
	deleteMap(AvgDEST_Clustering);
	deleteMap(AvgTAG_Clustering);
	deleteMap(AvgCOLOR_Clustering);
	deleteMap(AvgKEY_Clustering);
	deleteMap(DATA_OP_Clustering);
	deleteMap(AvgLoopIter_Clustering);
}
void Cluster::deleteMap(map<long long int, Ranklist> &Cluster){

	while (!Cluster.empty())
		{
			 Cluster.erase( Cluster.begin());
		}

}
void Cluster::initClusterRanklist(int rank){
			Ranklist rl(rank); 
			SIG_FORMAT=0; cmp_Format_Signature=0; 
			ranklist=rl; 

			//Group1_Weight=20;
			//Group2_Weight=;
			//Group3_Weight=;		
			//Group4_Weight=;
	
			weight_CallPath=0;
			weight_Count=0;
			weight_AvgCommTime=0;
			weight_AvgCompTime=0;
			weight_SRC=0;
			weight_DEST=0;
			weight_TAG=0;
			weight_COLOR=0;
			weight_KEY=0;
			weight_DATA_OP=0;
			weight_AvgLoopIter=0;
				
			globalAvgCount=0;
			globalAvgCommTime=0;
			globalAvgCompTime=0;
			globalAvgLoopIter=0;
			
			SD_AvgCount=0;
			SD_AvgCommTime=0;
			SD_AvgCompTime=0;
			SD_AvgLoopIter=0;
			
			freqCallPath=0;
			freqSRC=0;
			freqDEST=0;
			freqTAG=0;
			freqCOLOR=0;
			freqKEY=0;
			freqDATA_OP=0;
			
			MergeScore =0;
	}

//void mergeCluster(map<long long int, Ranklist> *slav)
void Cluster::mergeSigClusters()
{
	map<long long , Ranklist>::iterator it;
	map<long long , Ranklist>::iterator it2;

	map<long long , Ranklist>::iterator itt;
	map<long long , Ranklist>::iterator itt2;

   if(L==0 || L==1){	
	if (SIG_FORMAT & 0x00000001){// "Call Path Clustering"; 
		for( it=recv_Call_Path_Clustering.begin(); it!=recv_Call_Path_Clustering.end(); it++)
		{
		
			it2 = Call_Path_Clustering.find(it->first);
		        if (it2!=Call_Path_Clustering.end())
        	       	{
	        	        //Existing Cluster - just modify ranklist
                		it2->second.merge(&it->second);
	               	}
		        else
               		{
		               //New Cluster
        	       		Call_Path_Clustering[it->first]= it->second;
               		}
		}
		while (!recv_Call_Path_Clustering.empty())
		{
			recv_Call_Path_Clustering.erase(recv_Call_Path_Clustering.begin());
		}
	}
    }
   if(L==0 || L==2){	
	if (SIG_FORMAT & 0x00000002){//	"Avg Count Clustering"; 
		for( it=recv_AvgCount_Clustering.begin(); it!=recv_AvgCount_Clustering.end(); it++)
		{
		
			it2 =AvgCount_Clustering.find(it->first);
		        if (it2!=AvgCount_Clustering.end())
        	       	{
	        	        //Existing Cluster - just modify ranklist
                		it2->second.merge(&it->second);
	               	}
		        else
               		{
		               //New Cluster
		 	      	AvgCount_Clustering[it->first]= it->second;
               		}
		}

		while (!recv_AvgCount_Clustering.empty())
		{
			recv_AvgCount_Clustering.erase(recv_AvgCount_Clustering.begin());
		}
	}
	if (SIG_FORMAT & 0x00000004){// "Avg Comm Clustering"; 
		for( it=recv_AvgCommTime_Clustering.begin(); it!=recv_AvgCommTime_Clustering.end(); it++)
		{
		
			it2 = AvgCommTime_Clustering.find(it->first);
		        if (it2!=AvgCommTime_Clustering.end())
        	       	{
	        	        //Existing Cluster - just modify ranklist
                		it2->second.merge(&it->second);
	               	}
		        else
               		{
		               //New Cluster
		 	      	AvgCommTime_Clustering[it->first]= it->second;
               		}
		}

		while (!recv_AvgCommTime_Clustering.empty())
		{
			recv_AvgCommTime_Clustering.erase(recv_AvgCommTime_Clustering.begin());
		}

	}
	if (SIG_FORMAT & 0x00000008){// "Avg Comp Clustering"; 
		for( it=recv_AvgCompTime_Clustering.begin(); it!=recv_AvgCompTime_Clustering.end(); it++)
		{
			it2 =AvgCompTime_Clustering.find(it->first);
		        if (it2!=AvgCompTime_Clustering.end())
        	       	{
	        	        //Existing Cluster - just modify ranklist
                		it2->second.merge(&it->second);
	               	}
		        else
               		{
		                //New Cluster
		 	      	AvgCompTime_Clustering[it->first]= it->second;
               		}
		}
		while (!recv_AvgCompTime_Clustering.empty())
		{
			recv_AvgCompTime_Clustering.erase(recv_AvgCompTime_Clustering.begin());
		}
	}
	if (SIG_FORMAT & 0x00000010){// "Avg SRC Clustering"; 
		for( it=recv_AvgSRC_Clustering.begin(); it!=recv_AvgSRC_Clustering.end(); it++)
		{
		
			it2 =AvgSRC_Clustering.find(it->first);
		        if (it2!=AvgSRC_Clustering.end())
        	       	{
	        	        //Existing Cluster - just modify ranklist
                		it2->second.merge(&it->second);
	               	}
		        else
               		{
		               //New Cluster
        	       		AvgSRC_Clustering[it->first]= it->second;
               		}
		}
		while (!recv_AvgSRC_Clustering.empty())
		{
			recv_AvgSRC_Clustering.erase(recv_AvgSRC_Clustering.begin());
		}

	}
	if (SIG_FORMAT & 0x00000020){// "Avg DEST Clustering"; 
		for( it=recv_AvgDEST_Clustering.begin(); it!=recv_AvgDEST_Clustering.end(); it++)
		{
		
			it2 =AvgDEST_Clustering.find(it->first);
		        if (it2!=AvgDEST_Clustering.end())
        	       	{
	        	        //Existing Cluster - just modify ranklist
                		it2->second.merge(&it->second);
	               	}
		        else
               		{
		               //New Cluster
        	       		AvgDEST_Clustering[it->first]= it->second;
               		}
		}
		while (!recv_AvgDEST_Clustering.empty())
		{
			recv_AvgDEST_Clustering.erase(recv_AvgDEST_Clustering.begin());
		}

	}
	if (SIG_FORMAT & 0x00000040){// "Avg TAG Clustering"; 
		for( itt=recv_AvgTAG_Clustering.begin(); itt!=recv_AvgTAG_Clustering.end(); itt++)
		{
		
			itt2 =AvgTAG_Clustering.find(itt->first);
		        if (itt2!=AvgTAG_Clustering.end())
        	       	{
	        	        //Existing Cluster - just modify ranklist
                		itt2->second.merge(&itt->second);
	               	}
		        else
               		{
		               //New Cluster
        	       		AvgTAG_Clustering[itt->first]= itt->second;
               		}
		}
		while (!recv_AvgTAG_Clustering.empty())
		{
			recv_AvgTAG_Clustering.erase(recv_AvgTAG_Clustering.begin());
		}

	}
	if (SIG_FORMAT & 0x00000080){ // "Avg COLOR Clustering"; 
		for( itt=recv_AvgCOLOR_Clustering.begin(); itt!=recv_AvgCOLOR_Clustering.end(); itt++)
		{
		
			itt2 =AvgCOLOR_Clustering.find(itt->first);
		        if (itt2!=AvgCOLOR_Clustering.end())
        	       	{
	        	        //Existing Cluster - just modify ranklist
                		itt2->second.merge(&itt->second);
	               	}
		        else
               		{
		               //New Cluster
        	       		AvgCOLOR_Clustering[itt->first]= itt->second;
               		}
		}
		while (!recv_AvgCOLOR_Clustering.empty())
		{
			recv_AvgCOLOR_Clustering.erase(recv_AvgCOLOR_Clustering.begin());
		}

	}
	if (SIG_FORMAT & 0x00000100){ // "Avg KEY Clustering"; 
		for( itt=recv_AvgKEY_Clustering.begin(); itt!=recv_AvgKEY_Clustering.end(); itt++)
		{
		
			itt2 =AvgKEY_Clustering.find(itt->first);
		        if (itt2!=AvgKEY_Clustering.end())
        	       	{
	        	        //Existing Cluster - just modify ranklist
                		itt2->second.merge(&itt->second);
	               	}
		        else
               		{
		               //New Cluster
        	       		AvgKEY_Clustering[itt->first]= itt->second;
               		}
		}
		while (!recv_AvgKEY_Clustering.empty())
		{
			recv_AvgKEY_Clustering.erase(recv_AvgKEY_Clustering.begin());
		}

	}
	if (SIG_FORMAT & 0x00000200){ // "Avg DATA OP Clustering"; 
 		for( it=recv_DATA_OP_Clustering.begin(); it!=recv_DATA_OP_Clustering.end(); it++)
		{
		
			it2 = DATA_OP_Clustering.find(it->first);
		        if (it2!=DATA_OP_Clustering.end())
        	       	{
	        	        //Existing Cluster - just modify ranklist
                		it2->second.merge(&it->second);
	               	}
		        else
               		{
		               //New Cluster
        	       		DATA_OP_Clustering[it->first]= it->second;
               		}
		}

		while (!recv_DATA_OP_Clustering.empty())
		{
			recv_DATA_OP_Clustering.erase(recv_DATA_OP_Clustering.begin());
		}

	}
	if (SIG_FORMAT & 0x00000400){ // "Avg Loop Iter Clustering";
 		for( it=recv_AvgLoopIter_Clustering.begin(); it!=recv_AvgLoopIter_Clustering.end(); it++)
		{
		
			it2 =AvgLoopIter_Clustering.find(it->first);
		        if (it2!=AvgLoopIter_Clustering.end())
        	       	{
	        	        //Existing Cluster - just modify ranklist
                		it2->second.merge(&it->second);
	               	}
		        else
               		{
		               //New Cluster
		 	      	AvgLoopIter_Clustering[it->first]= it->second;
               		}
		}

		while (!recv_AvgLoopIter_Clustering.empty())
		{
			recv_AvgLoopIter_Clustering.erase(recv_AvgLoopIter_Clustering.begin());
		}
        }
   }
			
}
int* Cluster::getClusterPackedSize(){

	int* size = new int[NUM_CLUSTERING];
	map<long long , Ranklist>::iterator it;
	map<long long , Ranklist>::iterator it2;

  if(L==0 || L==1){
	if (SIG_FORMAT & 0x00000001){
		size[CALL_PATH]=CALL_PATH;
		size[CALL_PATH] += sizeof (int); /*size of clusters - len*/
		for( it=Call_Path_Clustering.begin(); it!=Call_Path_Clustering.end(); it++)
		{
			size[CALL_PATH] += sizeof(long long );
			size[CALL_PATH] += it->second.getPackedSize(); 	
		}//	cout << endl << "Call Path Clustering"; 
	}
   }
   if(L==0 || L==2){
	if (SIG_FORMAT & 0x00000002){
		size[AVG_COUNT]=0;
		size[AVG_COUNT] += sizeof (int); /*size of clusters - len*/
		for( it=AvgCount_Clustering.begin(); it!=AvgCount_Clustering.end(); it++)
		{
			size[AVG_COUNT] += sizeof(long long );
			size[AVG_COUNT] += it->second.getPackedSize(); 	
		}
	//	cout << endl << "Avg Count Clustering"; 
	}
	if (SIG_FORMAT & 0x00000004){
		size[AVG_COMM_TIME]=0;
		size[AVG_COMM_TIME] += sizeof (int); /*size of clusters - len*/
		for( it=AvgCommTime_Clustering.begin(); it!=AvgCommTime_Clustering.end(); it++)
		{
			size[AVG_COMM_TIME] += sizeof(long long);
			size[AVG_COMM_TIME] += it->second.getPackedSize(); 	
		}//	cout << endl << "Avg Comm Clustering"; 
	}
	if (SIG_FORMAT & 0x00000008){
		size[AVG_COMP_TIME]=0;
		size[AVG_COMP_TIME] += sizeof (int); /*size of clusters - len*/
		for( it=AvgCompTime_Clustering.begin(); it!=AvgCompTime_Clustering.end(); it++)
		{
			size[AVG_COMP_TIME] += sizeof(long long);
			size[AVG_COMP_TIME] += it->second.getPackedSize(); 	
			it->second.toString();
		}//	cout << endl << "Avg Comp Clustering"; 
	}
	if (SIG_FORMAT & 0x00000010){
		size[D_SRC]=0;
		size[D_SRC] += sizeof (int); /*size of clusters - len*/
		for( it=AvgSRC_Clustering.begin(); it!=AvgSRC_Clustering.end(); it++)
		{
			size[D_SRC] += sizeof(long long);
			size[D_SRC] += it->second.getPackedSize(); 	
		}//	cout << endl << "Avg SRC Clustering"; 
	}
	if (SIG_FORMAT & 0x00000020){
		size[D_DEST]=0;
		size[D_DEST] += sizeof (int); /*size of clusters - len*/
		for( it=AvgDEST_Clustering.begin(); it!=AvgDEST_Clustering.end(); it++)
		{
			size[D_DEST] += sizeof(long long);
			size[D_DEST] += it->second.getPackedSize(); 	
		}//	cout << endl << "Avg DEST Clustering"; 
	}
	if (SIG_FORMAT & 0x00000040){
		size[D_TAG]=0;
		size[D_TAG] += sizeof (int); /*size of clusters - len*/
		for( it2=AvgTAG_Clustering.begin(); it2!=AvgTAG_Clustering.end(); it2++)
		{
			size[D_TAG] += sizeof(long long);
			size[D_TAG] += it2->second.getPackedSize(); 	
		}//	cout << endl << "Avg TAG Clustering"; 
	}
	if (SIG_FORMAT & 0x00000080){
		size[D_COLOR]=0;
		size[D_COLOR] += sizeof (int); /*size of clusters - len*/
		for( it2=AvgCOLOR_Clustering.begin(); it2!=AvgCOLOR_Clustering.end(); it2++)
		{
			size[D_COLOR] += sizeof(long long);
			size[D_COLOR] += it2->second.getPackedSize(); 	
		}//	cout << endl << "Avg COLOR Clustering"; 
	}
	if (SIG_FORMAT & 0x00000100){
		size[D_KEY]=0;
		size[D_KEY] += sizeof (int); /*size of clusters - len*/
		for( it2=AvgKEY_Clustering.begin(); it2!=AvgKEY_Clustering.end(); it2++)
		{
			size[D_KEY] += sizeof(long long);
			size[D_KEY] += it2->second.getPackedSize(); 	
		}//	cout << endl << "Avg KEY Clustering"; 
	}
	if (SIG_FORMAT & 0x00000200){
		size[D_DATA_OP]=0;
		size[D_DATA_OP] += sizeof (int); /*size of clusters - len*/
		for( it=DATA_OP_Clustering.begin(); it!=DATA_OP_Clustering.end(); it++)
		{
			size[D_DATA_OP] += sizeof(long long);
			size[D_DATA_OP] += it->second.getPackedSize(); 	
		}//	cout << endl << "Avg DATA OP Clustering"; 
	}
	if (SIG_FORMAT & 0x00000400){
		size[AVG_LOOP_ITER]=0;
		size[AVG_LOOP_ITER] += sizeof (int); /*size of clusters - len*/
		for( it=AvgLoopIter_Clustering.begin(); it!=AvgLoopIter_Clustering.end(); it++)
		{
			size[AVG_LOOP_ITER] += sizeof(long long);
			size[AVG_LOOP_ITER] += it->second.getPackedSize(); 	
			it->second.toString();
		}//	cout << endl << "Avg LOOP ITER Clustering"; 
	}
   }

	return size;
}


void Cluster::sendSigClusters(int dest){
	int* size  = getClusterPackedSize();
      	int bufsize=0; 
	int clusteringType;
	MPI_Comm comm = MPI_COMM_WORLD;
	void *buf;
	int position;
   if (L==0 || L ==1){// non-hierarchical or Level 1
	if (SIG_FORMAT & 0x00000001){ //Call-Path
		clusteringType=0;
		bufsize = size[CALL_PATH];
		buf = malloc(bufsize);
	        position = 0;
	        if(!buf){
        	        cerr << "Trace::sendCluster(): cannot allocate buffer" << endl;
                	exit(1);
        	}	
	        PMPI_Send(&bufsize, 1, MPI_INT, dest, 0, comm);
        	pack(buf, bufsize, &position, comm, clusteringType); 
	        PMPI_Send(buf, position, MPI_PACKED, dest, 0, comm);
        	free(buf);
	}
   }
   if(L==0 || L==2){ // non-hierarchical or Level 2
	if (SIG_FORMAT & 0x00000002){ //AvgCount
		clusteringType=AVG_COUNT;
		bufsize = size[AVG_COUNT];
		buf = malloc(bufsize);
	        position = 0;
	        if(!buf){
        	        cerr << "Trace::sendCluster(): cannot allocate buffer" << endl;
                	exit(1);
        	}	
	        PMPI_Send(&bufsize, 1, MPI_INT, dest, 0, comm);
        	pack(buf, bufsize, &position, comm, clusteringType); 
	        PMPI_Send(buf, position, MPI_PACKED, dest, 0, comm);
        	free(buf);
	}
	if (SIG_FORMAT & 0x00000004){ //AvgCommTime
		clusteringType=AVG_COMM_TIME;
		bufsize = size[AVG_COMM_TIME];
		buf = malloc(bufsize);
	        position = 0;
	        if(!buf){
        	        cerr << "Trace::sendCluster(): cannot allocate buffer" << endl;
                	exit(1);
        	}	
	        PMPI_Send(&bufsize, 1, MPI_INT, dest, 0, comm);
        	pack(buf, bufsize, &position, comm, clusteringType); 
	        PMPI_Send(buf, position, MPI_PACKED, dest, 0, comm);
        	free(buf);
	}
	if (SIG_FORMAT & 0x00000008){ //AvgCompTime
	
		clusteringType=AVG_COMP_TIME;
		bufsize = size[AVG_COMP_TIME];
		buf = malloc(bufsize);
	        position = 0;
	        if(!buf){
        	        cerr << "Trace::sendCluster(): cannot allocate buffer" << endl;
                	exit(1);
        	}	
	        PMPI_Send(&bufsize, 1, MPI_INT, dest, 0, comm);
        	pack(buf, bufsize, &position, comm, clusteringType); 
	        PMPI_Send(buf, position, MPI_PACKED, dest, 0, comm);
        	free(buf);
	}
	if (SIG_FORMAT & 0x00000010){ //AvgSRC
		clusteringType=D_SRC;
		bufsize = size[D_SRC];
		buf = malloc(bufsize);
	        position = 0;
	        if(!buf){
        	        cerr << "Trace::sendCluster(): cannot allocate buffer" << endl;
                	exit(1);
        	}	
	        
		PMPI_Send(&bufsize, 1, MPI_INT, dest, 0, comm);
        	pack(buf, bufsize, &position, comm, clusteringType); 
	        PMPI_Send(buf, position, MPI_PACKED, dest, 0, comm);
        	free(buf);
	}

	if (SIG_FORMAT & 0x00000020){ //AvgDEST
		clusteringType=D_DEST;
		bufsize = size[D_DEST];
		buf = malloc(bufsize);
	        position = 0;
	        if(!buf){
        	        cerr << "Trace::sendCluster(): cannot allocate buffer" << endl;
                	exit(1);
        	}	
	        PMPI_Send(&bufsize, 1, MPI_INT, dest, 0, comm);
        	pack(buf, bufsize, &position, comm, clusteringType); 
	        PMPI_Send(buf, position, MPI_PACKED, dest, 0, comm);
        	free(buf);
	}
	if (SIG_FORMAT & 0x00000040){ //AvgTAG
		clusteringType=D_TAG;
		bufsize = size[D_TAG];
		buf = malloc(bufsize);
	        position = 0;
	        if(!buf){
        	        cerr << "Trace::sendCluster(): cannot allocate buffer" << endl;
                	exit(1);
        	}	
	        PMPI_Send(&bufsize, 1, MPI_INT, dest, 0, comm);
        	pack(buf, bufsize, &position, comm, clusteringType); 
	        PMPI_Send(buf, position, MPI_PACKED, dest, 0, comm);
        	free(buf);
	}
	if (SIG_FORMAT & 0x00000080){ //AvgCOLOR
		clusteringType=D_COLOR;
		bufsize = size[D_COLOR];
		buf = malloc(bufsize);
	        position = 0;
	        if(!buf){
        	        cerr << "Trace::sendCluster(): cannot allocate buffer" << endl;
                	exit(1);
        	}	
	        PMPI_Send(&bufsize, 1, MPI_INT, dest, 0, comm);
        	pack(buf, bufsize, &position, comm, clusteringType); 
	        PMPI_Send(buf, position, MPI_PACKED, dest, 0, comm);
        	free(buf);
	}
	if (SIG_FORMAT & 0x00000100){ //AvgKEY
		clusteringType=D_KEY;
		bufsize = size[D_KEY];
		buf = malloc(bufsize);
	        position = 0;
	        if(!buf){
        	        cerr << "Trace::sendCluster(): cannot allocate buffer" << endl;
                	exit(1);
        	}	
	        PMPI_Send(&bufsize, 1, MPI_INT, dest, 0, comm);
        	pack(buf, bufsize, &position, comm, clusteringType); 
	        PMPI_Send(buf, position, MPI_PACKED, dest, 0, comm);
        	free(buf);
	}
	if (SIG_FORMAT & 0x00000200){ //DATA_OP
		clusteringType=D_DATA_OP;
		bufsize = size[D_DATA_OP];
		buf = malloc(bufsize);
	        position = 0;
	        if(!buf){
        	        cerr << "Trace::sendCluster(): cannot allocate buffer" << endl;
                	exit(1);
        	}	
	        PMPI_Send(&bufsize, 1, MPI_INT, dest, 0, comm);
        	pack(buf, bufsize, &position, comm, clusteringType); 
	        PMPI_Send(buf, position, MPI_PACKED, dest, 0, comm);
        	free(buf);
	}	
	if (SIG_FORMAT & 0x00000400){ //AvgLoopIter
	
		clusteringType=AVG_LOOP_ITER;
		bufsize = size[AVG_LOOP_ITER];
		buf = malloc(bufsize);
	        position = 0;
	        if(!buf){
        	        cerr << "Trace::sendCluster(): cannot allocate buffer" << endl;
                	exit(1);
        	}	
	        PMPI_Send(&bufsize, 1, MPI_INT, dest, 0, comm);
        	pack(buf, bufsize, &position, comm, clusteringType); 
	        PMPI_Send(buf, position, MPI_PACKED, dest, 0, comm);
        	free(buf);
	}
    }
}

void Cluster::recvSigClusters(int src){
	
      	int bufsize=0; 
	int clusteringType;
        void *buf;
        int position;
	MPI_Comm comm = MPI_COMM_WORLD;

   if (L==0 || L ==1){// non-hierarchical or Level 1
	if (SIG_FORMAT & 0x00000001){ //Call-Path
		clusteringType=CALL_PATH;
		position=0;
        	PMPI_Recv(&bufsize, 1, MPI_INT, src, 0, comm, MPI_STATUS_IGNORE);
		buf = malloc(bufsize);
	        if(!buf){
        	        cerr << "Trace::recvCluster(): cannot allocate buffer" << endl;
                	exit(1);
	        }
	        PMPI_Recv(buf, bufsize, MPI_PACKED, src, 0, comm, MPI_STATUS_IGNORE);
        	unpack(buf, bufsize, &position, comm, clusteringType);

	        free(buf);
	}
   }

   if (L==0 || L ==2){// non-hierarchical or Level 2
	if (SIG_FORMAT & 0x00000002){ 	//	cout << endl << "Avg Count Clustering"; 
		clusteringType=AVG_COUNT;
		position=0;
        	PMPI_Recv(&bufsize, 1, MPI_INT, src, 0, comm, MPI_STATUS_IGNORE);
		buf = malloc(bufsize);
	        if(!buf){
        	        cerr << "Trace::recvCluster(): cannot allocate buffer" << endl;
                	exit(1);
	        }
	        PMPI_Recv(buf, bufsize, MPI_PACKED, src, 0, comm, MPI_STATUS_IGNORE);
        	unpack(buf, bufsize, &position, comm, clusteringType);

	        free(buf);
	}
	if (SIG_FORMAT & 0x00000004){	//	cout << endl << "Avg Comm Clustering"; 
		clusteringType=AVG_COMM_TIME;
		position=0;
        	PMPI_Recv(&bufsize, 1, MPI_INT, src, 0, comm, MPI_STATUS_IGNORE);
		buf = malloc(bufsize);
	        if(!buf){
        	        cerr << "Trace::recvCluster(): cannot allocate buffer" << endl;
                	exit(1);
	        }
	        PMPI_Recv(buf, bufsize, MPI_PACKED, src, 0, comm, MPI_STATUS_IGNORE);
        	unpack(buf, bufsize, &position, comm, clusteringType);

	        free(buf);

	}
	if (SIG_FORMAT & 0x00000008){	//	cout << endl << "Avg Comp Clustering"; 
		clusteringType=AVG_COMP_TIME;
		position=0;
        	PMPI_Recv(&bufsize, 1, MPI_INT, src, 0, comm, MPI_STATUS_IGNORE);
		buf = malloc(bufsize);
	        if(!buf){
        	        cerr << "Trace::recvCluster(): cannot allocate buffer" << endl;
                	exit(1);
	        }
	        PMPI_Recv(buf, bufsize, MPI_PACKED, src, 0, comm, MPI_STATUS_IGNORE);
        	unpack(buf, bufsize, &position, comm, clusteringType);

	        free(buf);
	}
	if (SIG_FORMAT & 0x00000010){	//	cout << endl << "Avg SRC Clustering"; 
		clusteringType=D_SRC;
		position=0;
        	PMPI_Recv(&bufsize, 1, MPI_INT, src, 0, comm, MPI_STATUS_IGNORE);
		buf = malloc(bufsize);
	        if(!buf){
        	        cerr << "Trace::recvCluster(): cannot allocate buffer" << endl;
                	exit(1);
	        }
	        PMPI_Recv(buf, bufsize, MPI_PACKED, src, 0, comm, MPI_STATUS_IGNORE);
        	unpack(buf, bufsize, &position, comm, clusteringType);

	        free(buf);

	}
	if (SIG_FORMAT & 0x00000020){	//	cout << endl << "Avg DEST Clustering"; 
		clusteringType=D_DEST;
		position=0;
        	PMPI_Recv(&bufsize, 1, MPI_INT, src, 0, comm, MPI_STATUS_IGNORE);
		buf = malloc(bufsize);
	        if(!buf){
        	        cerr << "Trace::recvCluster(): cannot allocate buffer" << endl;
                	exit(1);
	        }
	        PMPI_Recv(buf, bufsize, MPI_PACKED, src, 0, comm, MPI_STATUS_IGNORE);
        	unpack(buf, bufsize, &position, comm, clusteringType);

	        free(buf);

	}
	if (SIG_FORMAT & 0x00000040){	//	cout << endl << "Avg TAG Clustering"; 
		clusteringType=D_TAG;
		position=0;
        	PMPI_Recv(&bufsize, 1, MPI_INT, src, 0, comm, MPI_STATUS_IGNORE);
		buf = malloc(bufsize);
	        if(!buf){
        	        cerr << "Trace::recvCluster(): cannot allocate buffer" << endl;
                	exit(1);
	        }
	        PMPI_Recv(buf, bufsize, MPI_PACKED, src, 0, comm, MPI_STATUS_IGNORE);
        	unpack(buf, bufsize, &position, comm, clusteringType);

	        free(buf);

	}
	if (SIG_FORMAT & 0x00000080){	//	cout << endl << "Avg COLOR Clustering"; 
		clusteringType=D_COLOR;
		position=0;
        	PMPI_Recv(&bufsize, 1, MPI_INT, src, 0, comm, MPI_STATUS_IGNORE);
		buf = malloc(bufsize);
	        if(!buf){
        	        cerr << "Trace::recvCluster(): cannot allocate buffer" << endl;
                	exit(1);
	        }
	        PMPI_Recv(buf, bufsize, MPI_PACKED, src, 0, comm, MPI_STATUS_IGNORE);
        	unpack(buf, bufsize, &position, comm, clusteringType);

	        free(buf);

	}
	if (SIG_FORMAT & 0x00000100){	//	cout << endl << "Avg KEY Clustering"; 
		clusteringType=D_KEY;
		position=0;
        	PMPI_Recv(&bufsize, 1, MPI_INT, src, 0, comm, MPI_STATUS_IGNORE);
		buf = malloc(bufsize);
	        if(!buf){
        	        cerr << "Trace::recvCluster(): cannot allocate buffer" << endl;
                	exit(1);
	        }
	        PMPI_Recv(buf, bufsize, MPI_PACKED, src, 0, comm, MPI_STATUS_IGNORE);
        	unpack(buf, bufsize, &position, comm, clusteringType);

	        free(buf);

	}
	if (SIG_FORMAT & 0x00000200){	//	cout << endl << "Avg DATA OP Clustering"; 
		clusteringType=D_DATA_OP;
		position=0;
        	PMPI_Recv(&bufsize, 1, MPI_INT, src, 0, comm, MPI_STATUS_IGNORE);
		buf = malloc(bufsize);
	        if(!buf){
        	        cerr << "Trace::recvCluster(): cannot allocate buffer" << endl;
                	exit(1);
	        }
	        PMPI_Recv(buf, bufsize, MPI_PACKED, src, 0, comm, MPI_STATUS_IGNORE);
        	unpack(buf, bufsize, &position, comm, clusteringType);

	        free(buf);
	}
	if (SIG_FORMAT & 0x00000400){	//	cout << endl << "Avg Loop Iter Clustering"; 
		clusteringType=AVG_LOOP_ITER;
		position=0;
        	PMPI_Recv(&bufsize, 1, MPI_INT, src, 0, comm, MPI_STATUS_IGNORE);
		buf = malloc(bufsize);
	        if(!buf){
        	        cerr << "Trace::recvCluster(): cannot allocate buffer" << endl;
                	exit(1);
	        }
	        PMPI_Recv(buf, bufsize, MPI_PACKED, src, 0, comm, MPI_STATUS_IGNORE);
        	unpack(buf, bufsize, &position, comm, clusteringType);

	        free(buf);
	}
    }

}

void Cluster::pack(void *buf, int bufsize, int *position, MPI_Comm comm, int clusteringType){

   int len;

   if (clusteringType>=0){
	map<long long , Ranklist>::iterator it1;
	map<long long , Ranklist>::iterator it2;
	if (clusteringType==CALL_PATH){	
		len = Call_Path_Clustering.size();  
        	PMPI_Pack(&len, 1, MPI_INT, buf, bufsize, position, comm);

		for( it1=Call_Path_Clustering.begin(); it1!=Call_Path_Clustering.end(); it1++)
		{
			PMPI_Pack((void*)&it1->first, 1, MPI_LONG_LONG, buf, bufsize, position, comm);
			it1->second.pack(buf, bufsize, position, comm);
		}
	}
	if (clusteringType==AVG_COUNT){	
		len = AvgCount_Clustering.size();  
        	PMPI_Pack(&len, 1, MPI_INT, buf, bufsize, position, comm);

		for( it1=AvgCount_Clustering.begin(); it1!=AvgCount_Clustering.end(); it1++)
		{
			PMPI_Pack((void*)&it1->first, 1, MPI_LONG_LONG, buf, bufsize, position, comm);
			it1->second.pack(buf, bufsize, position, comm);
		}
	}
	if (clusteringType==AVG_COMM_TIME){	
		len = AvgCommTime_Clustering.size();  
        	PMPI_Pack(&len, 1, MPI_INT, buf, bufsize, position, comm);

		for( it1=AvgCommTime_Clustering.begin(); it1!=AvgCommTime_Clustering.end(); it1++)
		{
			PMPI_Pack((void*)&it1->first, 1, MPI_LONG_LONG, buf, bufsize, position, comm);
			it1->second.pack(buf, bufsize, position, comm);
		}
	}
	if (clusteringType==AVG_COMP_TIME){	
		len = AvgCompTime_Clustering.size();  
        	PMPI_Pack(&len, 1, MPI_INT, buf, bufsize, position, comm);

		for( it1=AvgCompTime_Clustering.begin(); it1!=AvgCompTime_Clustering.end(); it1++)
		{
			PMPI_Pack((void*)&it1->first, 1, MPI_LONG_LONG, buf, bufsize, position, comm);
			it1->second.pack(buf, bufsize, position, comm);
		}
	}
	if (clusteringType==D_SRC){	
		len = AvgSRC_Clustering.size();  
        	PMPI_Pack(&len, 1, MPI_INT, buf, bufsize, position, comm);

		for( it1=AvgSRC_Clustering.begin(); it1!=AvgSRC_Clustering.end(); it1++)
		{
			PMPI_Pack((void*)&it1->first, 1, MPI_LONG_LONG, buf, bufsize, position, comm);
			it1->second.pack(buf, bufsize, position, comm);
		}
	}

	if (clusteringType==D_DEST){	
		len = AvgDEST_Clustering.size();  
        	PMPI_Pack(&len, 1, MPI_INT, buf, bufsize, position, comm);

		for( it1=AvgDEST_Clustering.begin(); it1!=AvgDEST_Clustering.end(); it1++)
		{
			PMPI_Pack((void*)&it1->first, 1, MPI_LONG_LONG, buf, bufsize, position, comm);
			it1->second.pack(buf, bufsize, position, comm);
			
		}

		
	}
	if (clusteringType==D_TAG){	
		len = AvgTAG_Clustering.size();  
        	PMPI_Pack(&len, 1, MPI_INT, buf, bufsize, position, comm);

		for( it2=AvgTAG_Clustering.begin(); it2!=AvgTAG_Clustering.end(); it2++)
		{
			PMPI_Pack((void*)&it2->first, 1, MPI_LONG_LONG, buf, bufsize, position, comm);
			it2->second.pack(buf, bufsize, position, comm);
		}
	}
	if (clusteringType==D_COLOR){	
		len = AvgCOLOR_Clustering.size();  
        	PMPI_Pack(&len, 1, MPI_INT, buf, bufsize, position, comm);

		for( it2=AvgCOLOR_Clustering.begin(); it2!=AvgCOLOR_Clustering.end(); it2++)
		{
			PMPI_Pack((void*)&it2->first, 1, MPI_LONG_LONG, buf, bufsize, position, comm);
			it2->second.pack(buf, bufsize, position, comm);
		}
	}
	if (clusteringType==D_KEY){	
		len = AvgKEY_Clustering.size();  
        	PMPI_Pack(&len, 1, MPI_INT, buf, bufsize, position, comm);

		for( it2=AvgKEY_Clustering.begin(); it2!=AvgKEY_Clustering.end(); it2++)
		{
			PMPI_Pack((void*)&it2->first, 1, MPI_LONG_LONG, buf, bufsize, position, comm);
			it2->second.pack(buf, bufsize, position, comm);
		}
	}
	if (clusteringType==D_DATA_OP){	
		len = DATA_OP_Clustering.size();  
        	PMPI_Pack(&len, 1, MPI_INT, buf, bufsize, position, comm);

		for( it1=DATA_OP_Clustering.begin(); it1!=DATA_OP_Clustering.end(); it1++)
		{
			PMPI_Pack((void*)&it1->first, 1, MPI_LONG_LONG, buf, bufsize, position, comm);
			it1->second.pack(buf, bufsize, position, comm);
		}
	}
	if (clusteringType==AVG_LOOP_ITER){	
		len = AvgLoopIter_Clustering.size();  
        	PMPI_Pack(&len, 1, MPI_INT, buf, bufsize, position, comm);

		for( it1=AvgLoopIter_Clustering.begin(); it1!=AvgLoopIter_Clustering.end(); it1++)
		{
			PMPI_Pack((void*)&it1->first, 1, MPI_LONG_LONG, buf, bufsize, position, comm);
			it1->second.pack(buf, bufsize, position, comm);
		}
	}
}
else{	//K Clustering Heads
	map<int, Ranklist>::iterator it;
	len = ClusterHeads.size();  
       	PMPI_Pack(&len, 1, MPI_INT, buf, bufsize, position, comm);
	for( it=ClusterHeads.begin(); it!=ClusterHeads.end(); it++)
	{
		PMPI_Pack((void*)&it->first, 1, MPI_INT, buf, bufsize, position, comm);
		it->second.pack(buf, bufsize, position, comm);
	}
    }
}

void Cluster::unpack(void *buf, int bufsize, int *position, MPI_Comm comm, int clusteringType){

        int len;
 	Ranklist rl;
if(clusteringType>=0){
	long long tempSig1;
	long long tempSig2;
	if (clusteringType==CALL_PATH){
	        PMPI_Unpack(buf, bufsize, position, &len, 1, MPI_INT, comm);                
		Ranklist rl1[len];
		for(int i=0; i<len; i++){
			PMPI_Unpack(buf, bufsize, position, &tempSig1, 1, MPI_LONG_LONG, comm);
			rl1[i].unpack(buf, bufsize, position, comm);
		        recv_Call_Path_Clustering[tempSig1]=rl1[i];         
		}
	}  
	if (clusteringType==AVG_COUNT){
	        PMPI_Unpack(buf, bufsize, position, &len, 1, MPI_INT, comm);                
		Ranklist rl1[len];
		for(int i=0; i<len; i++){
			PMPI_Unpack(buf, bufsize, position, &tempSig1, 1, MPI_LONG_LONG, comm);
			rl1[i].unpack(buf, bufsize, position, comm);
		        recv_AvgCount_Clustering[tempSig1]=rl1[i];         
		}
	}
	if (clusteringType==AVG_COMM_TIME){
	        PMPI_Unpack(buf, bufsize, position, &len, 1, MPI_INT, comm);                
		Ranklist rl1[len];
		for(int i=0; i<len; i++){
			PMPI_Unpack(buf, bufsize, position, &tempSig1, 1, MPI_LONG_LONG, comm);
			rl1[i].unpack(buf, bufsize, position, comm);
		        recv_AvgCommTime_Clustering[tempSig1]=rl1[i];         
		}
	}
	if (clusteringType==AVG_COMP_TIME){
		PMPI_Unpack(buf, bufsize, position, &len, 1, MPI_INT, comm);               		
		Ranklist rl1[len];
		for(int i=0; i<len; i++){
			PMPI_Unpack(buf, bufsize, position, &tempSig1, 1, MPI_LONG_LONG, comm);
			rl1[i].unpack(buf, bufsize, position, comm);
		        recv_AvgCompTime_Clustering[tempSig1]=rl1[i];         
		}

	}
	if (clusteringType==D_SRC){
	        PMPI_Unpack(buf, bufsize, position, &len, 1, MPI_INT, comm);                
		Ranklist rl1[len];
		for(int i=0; i<len; i++){
			PMPI_Unpack(buf, bufsize, position, &tempSig1, 1, MPI_LONG_LONG, comm);
			rl1[i].unpack(buf, bufsize, position, comm);
		        recv_AvgSRC_Clustering[tempSig1]=rl1[i];         
		}
	}
	if (clusteringType==D_DEST){
	        PMPI_Unpack(buf, bufsize, position, &len, 1, MPI_INT, comm);                
		Ranklist rl1[len];
		for(int i=0; i<len; i++){
			PMPI_Unpack(buf, bufsize, position, &tempSig1, 1, MPI_LONG_LONG, comm);
			rl1[i].unpack(buf, bufsize, position, comm);
		        recv_AvgDEST_Clustering[tempSig1]=rl1[i];         
		}
	}
	if (clusteringType==D_TAG){
	        PMPI_Unpack(buf, bufsize, position, &len, 1, MPI_INT, comm);                
		Ranklist rl1[len];
		for(int i=0; i<len; i++){
			PMPI_Unpack(buf, bufsize, position, &tempSig2, 1, MPI_LONG_LONG, comm);
			rl1[i].unpack(buf, bufsize, position, comm);
		        recv_AvgTAG_Clustering[tempSig2]=rl1[i];         
		}
	}
	if (clusteringType==D_COLOR){
	        PMPI_Unpack(buf, bufsize, position, &len, 1, MPI_INT, comm);                
		Ranklist rl1[len];
		for(int i=0; i<len; i++){
			PMPI_Unpack(buf, bufsize, position, &tempSig2, 1, MPI_LONG_LONG, comm);
			rl1[i].unpack(buf, bufsize, position, comm);
		        recv_AvgCOLOR_Clustering[tempSig2]=rl1[i];         
		}
	}
	if (clusteringType==D_KEY){
	        PMPI_Unpack(buf, bufsize, position, &len, 1, MPI_INT, comm);                
		Ranklist rl1[len];
		for(int i=0; i<len; i++){
			PMPI_Unpack(buf, bufsize, position, &tempSig2, 1, MPI_LONG_LONG, comm);
			rl1[i].unpack(buf, bufsize, position, comm);
		        recv_AvgKEY_Clustering[tempSig2]=rl1[i];         
		}
	}
	if (clusteringType==D_DATA_OP){
	        PMPI_Unpack(buf, bufsize, position, &len, 1, MPI_INT, comm);                
		Ranklist rl1[len];
		for(int i=0; i<len; i++){
			PMPI_Unpack(buf, bufsize, position, &tempSig1, 1, MPI_LONG_LONG, comm);
			rl1[i].unpack(buf, bufsize, position, comm);
		        recv_DATA_OP_Clustering[tempSig1]=rl1[i];         
		}
	}
	if (clusteringType==AVG_LOOP_ITER){
	        PMPI_Unpack(buf, bufsize, position, &len, 1, MPI_INT, comm);       
		Ranklist rl1[len];
		for(int i=0; i<len; i++){
			PMPI_Unpack(buf, bufsize, position, &tempSig1, 1, MPI_LONG_LONG, comm);
			rl1[i].unpack(buf, bufsize, position, comm);
		        recv_AvgLoopIter_Clustering[tempSig1]=rl1[i];         
		}
	}
}
else{ // unpack K Clustering
	int tempHead;
	PMPI_Unpack(buf, bufsize, position, &len, 1, MPI_INT, comm);              
	
	Ranklist rl1[len];
/*	for(int i=0; i<len; i++){
		PMPI_Unpack(buf, bufsize, position, &tempHead, 1, MPI_INT, comm);
		rl1[i].unpack(buf, bufsize, position, comm);
		recvClusterHeads[tempHead]=rl1[i];         
	}

*/

		if(clusteringType == -1) // LEFT CHILD
		{
			while (!recvClusterHeads_Left.empty())
			{
				 recvClusterHeads_Left.erase(recvClusterHeads_Left.begin());
			}

		}
		else if (clusteringType == -2) // RIGHT CHILD
		{
			while (!recvClusterHeads_Right.empty())
			{
				 recvClusterHeads_Right.erase(recvClusterHeads_Right.begin());
			}

		}
		else if (clusteringType == -3) // refers to Broadcast of clsuter heads 
		{
			while (!recvClusterHeads.empty())
			{
				 recvClusterHeads.erase(recvClusterHeads.begin());
			}
		}



	for(int i=0; i<len; i++){
		PMPI_Unpack(buf, bufsize, position, &tempHead, 1, MPI_INT, comm);
		rl1[i].unpack(buf, bufsize, position, comm);
		if(clusteringType == -1) // LEFT CHILD
		{
			recvClusterHeads_Left[tempHead]=rl1[i];         

		}
		else if (clusteringType == -2) // RIGHT CHILD
		{
			recvClusterHeads_Right[tempHead]=rl1[i];         

		}
		else if (clusteringType == -3) // refers to Broadcast of clsuter heads 
		{
			recvClusterHeads[tempHead]=rl1[i];         

		}

	}
}
}
void Cluster::outputSigClusters(const char* filename){

	map<long long , Ranklist>::iterator it;
	map<long long , Ranklist>::iterator it2;
	map<int, Ranklist>::iterator it3;

if(L==0 || L==1){
	if (SIG_FORMAT & 0x00000001){ 	// "Call Path Clustering"; 
		char clusterfilename_CallPath[10000];
		sprintf(clusterfilename_CallPath, "%s%s", filename, "XOR_CallPath_Clustering");

        	ofstream clusterfile(clusterfilename_CallPath);

	        if(!clusterfile){
        	        cerr << "error: unable to open cluster file: " << clusterfilename_CallPath;
                	return;
	        }
		for( it=Call_Path_Clustering.begin(); it!=Call_Path_Clustering.end(); it++)
		{
			clusterfile << hex<< (*it).first << " => " <<(*it).second.toString() << endl;
		}	
	        clusterfile.close();
	}
}
if(L==0 || L==2){
	if (SIG_FORMAT & 0x00000002){// "Avg Count Clustering"; 
		char clusterfilename_AvgCount[10000];
		sprintf(clusterfilename_AvgCount, "%s%s", filename, "AvgCount_Clustering");

        	ofstream clusterfile(clusterfilename_AvgCount);

	        if(!clusterfile){
        	        cerr << "error: unable to open cluster file: " << clusterfilename_AvgCount;
                	return;
	        }
		for( it=AvgCount_Clustering.begin(); it!=AvgCount_Clustering.end(); it++)
		{
			clusterfile << hex<< (*it).first << " => " <<(*it).second.toString() << endl;
		}	
	        clusterfile.close();

	}
	if (SIG_FORMAT & 0x00000004){ 	// "Avg Comm Clustering"; 
		char clusterfilename_AvgComm[10000];
		sprintf(clusterfilename_AvgComm, "%s%s", filename, "AvgComm_Clustering");

        	ofstream clusterfile(clusterfilename_AvgComm);

	        if(!clusterfile){
        	        cerr << "error: unable to open cluster file: " << clusterfilename_AvgComm;
                	return;
	        }
		for( it=AvgCommTime_Clustering.begin(); it!=AvgCommTime_Clustering.end(); it++)
		{
			clusterfile << hex<< (*it).first << " => " <<(*it).second.toString() << endl;
		}	
	        clusterfile.close();

	}
	if (SIG_FORMAT & 0x00000008){ 	// "Avg Comp Clustering"; 
		char clusterfilename_AvgComp[10000];
		sprintf(clusterfilename_AvgComp, "%s%s", filename, "AvgComp_Clustering");

        	ofstream clusterfile(clusterfilename_AvgComp);

	        if(!clusterfile){
        	        cerr << "error: unable to open cluster file: " << clusterfilename_AvgComp;
                	return;
	        }
		for( it=AvgCompTime_Clustering.begin(); it!=AvgCompTime_Clustering.end(); it++)
		{
			clusterfile << hex<< (*it).first << " => " <<(*it).second.toString() << endl;
		}	
	        clusterfile.close();
	}
	if (SIG_FORMAT & 0x00000010){ 	// "Avg SRC Clustering"; 
		char clusterfilename_SRC[10000];
		sprintf(clusterfilename_SRC, "%s%s", filename, "AvgSRC_Clustering");

        	ofstream clusterfile(clusterfilename_SRC);

	        if(!clusterfile){
        	        cerr << "error: unable to open cluster file: " << clusterfilename_SRC;
                	return;
	        }
		for( it=AvgSRC_Clustering.begin(); it!=AvgSRC_Clustering.end(); it++)
		{
			clusterfile << hex<< (*it).first << " => " <<(*it).second.toString() << endl;
		}	
	        clusterfile.close();
	}
	if (SIG_FORMAT & 0x00000020){//	"Avg DEST Clustering"; 
		char clusterfilename_DEST[10000];
		sprintf(clusterfilename_DEST, "%s%s", filename, "AvgDEST_Clustering");

        	ofstream clusterfile(clusterfilename_DEST);

	        if(!clusterfile){
        	        cerr << "error: unable to open cluster file: " << clusterfilename_DEST;
                	return;
	        }
		for( it=AvgDEST_Clustering.begin(); it!=AvgDEST_Clustering.end(); it++)
		{
			clusterfile << hex<< (*it).first << " => " <<(*it).second.toString() << endl;
		}	
	        clusterfile.close();

	}
	if (SIG_FORMAT & 0x00000040){ // "Avg TAG Clustering"; 
		char clusterfilename_TAG[10000];
		sprintf(clusterfilename_TAG, "%s%s", filename, "AvgTAG_Clustering");

        	ofstream clusterfile(clusterfilename_TAG);

	        if(!clusterfile){
        	        cerr << "error: unable to open cluster file: " << clusterfilename_TAG;
                	return;
	        }
		for( it2=AvgTAG_Clustering.begin(); it2!=AvgTAG_Clustering.end(); it2++)
		{
			clusterfile << hex<< (*it2).first << " => " <<(*it2).second.toString() << endl;
		}	
	        clusterfile.close();
	}
	if (SIG_FORMAT & 0x00000080){ 	// "Avg COLOR Clustering";
 		char clusterfilename_COLOR[10000];
		sprintf(clusterfilename_COLOR, "%s%s", filename, "AvgCOLOR_Clustering");

        	ofstream clusterfile(clusterfilename_COLOR);

	        if(!clusterfile){
        	        cerr << "error: unable to open cluster file: " << clusterfilename_COLOR;
                	return;
	        }
		for( it2=AvgCOLOR_Clustering.begin(); it2!=AvgCOLOR_Clustering.end(); it2++)
		{
			clusterfile << hex<< (*it2).first << " => " <<(*it2).second.toString() << endl;
		}	
	        clusterfile.close();
	}
	if (SIG_FORMAT & 0x00000100){ 	// "Avg KEY Clustering"; 
		char clusterfilename_KEY[10000];
		sprintf(clusterfilename_KEY, "%s%s", filename, "AvgKEY_Clustering");

        	ofstream clusterfile(clusterfilename_KEY);

	        if(!clusterfile){
        	        cerr << "error: unable to open cluster file: " << clusterfilename_KEY;
                	return;
	        }
		for( it2=AvgKEY_Clustering.begin(); it2!=AvgKEY_Clustering.end(); it2++)
		{
			clusterfile << hex<< (*it2).first << " => " <<(*it2).second.toString() << endl;
		}	
	        clusterfile.close();
	}
	if (SIG_FORMAT & 0x00000200){ //cout << endl << "Avg DATA OP Clustering"; 
		char clusterfilename_DATA_OP[10000];
		sprintf(clusterfilename_DATA_OP, "%s%s", filename, "AvgDATA_OP_Clustering");

        	ofstream clusterfile(clusterfilename_DATA_OP);

	        if(!clusterfile){
        	        cerr << "error: unable to open cluster file: " << clusterfilename_DATA_OP;
                	return;
	        }
		for( it=DATA_OP_Clustering.begin(); it!=DATA_OP_Clustering.end(); it++)
		{
			clusterfile << hex<< (*it).first << " => " <<(*it).second.toString() << endl;
		}	
	        clusterfile.close();

	}
	if (SIG_FORMAT & 0x00000400){ 	 // "Avg Loop Iter Clustering"; 
		char clusterfilename_AvgLoopIter[10000];
		sprintf(clusterfilename_AvgLoopIter, "%s%s", filename, "AvgLoopIter_Clustering");

        	ofstream clusterfile(clusterfilename_AvgLoopIter);

	        if(!clusterfile){
		  cerr << "error: unable to open cluster file: " << clusterfilename_AvgLoopIter;
                	return;
	        }
		for( it=AvgLoopIter_Clustering.begin(); it!=AvgLoopIter_Clustering.end(); it++)
		{
			clusterfile << hex<< (*it).first << " => " <<(*it).second.toString() << endl;
		}	
	        clusterfile.close();
        }
   }
		char clusterfilename[10000];
		sprintf(clusterfilename, "%s%s", filename, "K_Clusters");

		ofstream clusterfile(clusterfilename);

	        if(!clusterfile){
        	        cerr << "error: unable to open cluster file: " << clusterfilename;
                	return;
	        }

		for( it3=ClusterHeads.begin(); it3!=ClusterHeads.end(); it3++)
		{
			clusterfile <<"Cluster Head: " << (*it3).first << " => Cluster Ranklist: " <<(*it3).second.toString() << endl;
		}	
		
	
	//	clusterfile <<"K Cluster Ranklist: " << ClusterHeads.toString()<< endl;
	        clusterfile.close();


   
/*      clusterfile << "\n--> CallPath Signature: "<< hex <<getCallPath() << "\n";
        clusterfile << "\n--> Avg Count Signature: "<< hex <<getAvgCount() << "\n";
        clusterfile << "\n--> Avg Communication Time Signature: "<< hex <<getAvgCommTime() << "\n";
        clusterfile << "\n--> Avg Computation Time Signature: "<< hex <<getAvgCompTime() << "\n";
        clusterfile << "\n--> Avg Loop Iter Signature: "<< hex <<getAvgLoopIter() << "\n";
        clusterfile << "\n--> SRC Signature: "<< hex <<getAvgSRC() << "\n";
        clusterfile << "\n--> DEST Signature: "<< hex <<getAvgDEST() << "\n";
        clusterfile << "\n--> TAG Signature: "<< hex <<getAvgTAG() << "\n";
        clusterfile << "\n--> COLOR Signature: "<< hex <<getAvgCOLOR() << "\n";
        clusterfile << "\n--> KEY Signature: "<< hex <<getAvgKEY() << "\n";
        clusterfile << "\n--> DATA TYPE + OPERATION Signature: "<< hex <<getDATA_OP() << "\n"; */


}


void Cluster::sendSigFormat(int dest){

	MPI_Comm comm = MPI_COMM_WORLD;
	PMPI_Send(&SIG_FORMAT, 1, MPI_SHORT, dest, 0, comm);
}
void Cluster::recvSigFormat(int src){

	short int  temp_SIG_FORMAT;
	MPI_Comm comm = MPI_COMM_WORLD;
	PMPI_Recv(&temp_SIG_FORMAT, 1, MPI_SHORT, src, 0, comm, MPI_STATUS_IGNORE);
	SIG_FORMAT |= temp_SIG_FORMAT; 

}

void Cluster::sendSignature(int dest){
	MPI_Comm comm = MPI_COMM_WORLD;
	PMPI_Send(&CallPath, 1, MPI_LONG_LONG, dest, 1, comm);
	PMPI_Send(&AvgCount, 1, MPI_LONG_LONG, dest, 2, comm);
	PMPI_Send(&AvgCommTime, 1, MPI_LONG_LONG, dest, 3, comm);
	PMPI_Send(&AvgCompTime, 1, MPI_LONG_LONG, dest, 4, comm);
	PMPI_Send(&AvgSRC, 1, MPI_LONG_LONG, dest, 5, comm);
	PMPI_Send(&AvgDEST, 1, MPI_LONG_LONG, dest, 6, comm);
	PMPI_Send(&AvgTAG, 1, MPI_LONG_LONG, dest, 7, comm);
	PMPI_Send(&AvgCOLOR, 1, MPI_LONG_LONG, dest, 8, comm);
	PMPI_Send(&AvgKEY, 1, MPI_LONG_LONG, dest, 9, comm);
	PMPI_Send(&DATA_OP, 1, MPI_LONG_LONG, dest, 10, comm);
	PMPI_Send(&AvgLoopIter, 1, MPI_LONG_LONG, dest, 11, comm);
}

void Cluster::recvSignature(int src, int my_rank){
	MPI_Comm comm = MPI_COMM_WORLD;
	long long tempCallPath;
	long long tempAvgCount;
	long long tempAvgSRC;
	long long tempAvgDEST;
	long long tempAvgTAG;
	long long tempAvgCOLOR;
	long long tempAvgKEY;
	long long tempDATA_OP;
	long long tempAvgCompTime;
	long long tempAvgCommTime;
	long long tempAvgLoopIter;
	MergeScore = 0;
	PMPI_Recv(&tempCallPath, 1,  MPI_LONG_LONG, src, 1, comm, MPI_STATUS_IGNORE);
	PMPI_Recv(&tempAvgCount, 1,  MPI_LONG_LONG, src, 2, comm, MPI_STATUS_IGNORE);
	PMPI_Recv(&tempAvgCommTime, 1,  MPI_LONG_LONG, src, 3, comm, MPI_STATUS_IGNORE);
	PMPI_Recv(&tempAvgCompTime, 1,  MPI_LONG_LONG, src, 4, comm, MPI_STATUS_IGNORE);
	PMPI_Recv(&tempAvgSRC, 1,  MPI_LONG_LONG, src, 5, comm, MPI_STATUS_IGNORE);
	PMPI_Recv(&tempAvgDEST, 1,  MPI_LONG_LONG, src, 6, comm, MPI_STATUS_IGNORE);
	PMPI_Recv(&tempAvgTAG, 1,  MPI_LONG_LONG, src, 7, comm, MPI_STATUS_IGNORE);
	PMPI_Recv(&tempAvgCOLOR, 1,  MPI_LONG_LONG, src, 8, comm, MPI_STATUS_IGNORE);
	PMPI_Recv(&tempAvgKEY, 1,  MPI_LONG_LONG, src, 9, comm, MPI_STATUS_IGNORE);
	PMPI_Recv(&tempDATA_OP, 1,  MPI_LONG_LONG, src, 10, comm, MPI_STATUS_IGNORE);
	PMPI_Recv(&tempAvgLoopIter, 1,  MPI_LONG_LONG, src, 11, comm, MPI_STATUS_IGNORE);
	
	if (tempCallPath!=CallPath)
	{	
		SIG_FORMAT |= 0x0001;
		freqCallPath++;	
	}
	if (tempAvgCount!=AvgCount)
	{
		SIG_FORMAT |= 0x0002;
	}
	if (tempAvgCommTime!=AvgCommTime)
	{
		SIG_FORMAT |= 0x0004;
	}
	if (tempAvgCompTime!=AvgCompTime)
	{
		SIG_FORMAT |= 0x0008;
	}
	if (tempAvgSRC!=AvgSRC)
	{
		SIG_FORMAT |= 0x0010;
		freqSRC++;
	}
	if (tempAvgDEST!=AvgDEST)
	{
		SIG_FORMAT |= 0x0020;
		freqDEST++;
	}
	if (tempAvgTAG!=AvgTAG)
	{
		SIG_FORMAT |= 0x0040;
		freqTAG++;
	}
	if (tempAvgCOLOR!=AvgCOLOR)
	{
		SIG_FORMAT |= 0x0080;
		freqCOLOR++;
	}
	if (tempAvgKEY!=AvgKEY)
	{
		SIG_FORMAT |= 0x0100;
		freqKEY++;	
	}
	if (tempDATA_OP!=DATA_OP)
	{
		SIG_FORMAT |= 0x0200;
	}
	if (tempAvgLoopIter!=AvgLoopIter)
	{
		SIG_FORMAT |= 0x0400;
	}
	
}

void Cluster::sendClusterFormat(int dest){
	
		sendSignature(dest);	
		sendSigFormat(dest);	
}
void Cluster::recvClusterFormat(int src, int my_rank){

		recvSignature(src, my_rank);	
		recvSigFormat(src);
}
		
void Cluster::initSigClusters(){
	if (SIG_FORMAT & 0x00000001){
	//	cout << endl << "Call Path Clustering"; 
		Call_Path_Clustering[getCallPath()]=getRanklist();
	}
	if (SIG_FORMAT & 0x00000002){
	//	cout << endl << "Avg Count Clustering"; 
		AvgCount_Clustering[getAvgCount()]=getRanklist();
	}
	if (SIG_FORMAT & 0x00000004){
	//	cout << endl << "Avg Comm Clustering"; 
		AvgCommTime_Clustering[getAvgCommTime()]=getRanklist();
	}
	if (SIG_FORMAT & 0x00000008){
	//	cout << endl << "Avg Comp Clustering"; 
		AvgCompTime_Clustering[getAvgCompTime()]=getRanklist();
	}
	if (SIG_FORMAT & 0x00000010){
	//	cout << endl << "Avg SRC Clustering"; 
		AvgSRC_Clustering[getAvgSRC()]=getRanklist();
	}
	if (SIG_FORMAT & 0x00000020){
	//	cout << endl << "Avg DEST Clustering"; 
		AvgDEST_Clustering[getAvgDEST()]=getRanklist();
	}
	if (SIG_FORMAT & 0x00000040){
	//	cout << endl << "Avg TAG Clustering"; 
		AvgTAG_Clustering[getAvgTAG()]=getRanklist();
	}
	if (SIG_FORMAT & 0x00000080){
	//	cout << endl << "Avg COLOR Clustering"; 
		AvgCOLOR_Clustering[getAvgCOLOR()]=getRanklist();
	}
	if (SIG_FORMAT & 0x00000100){
	//	cout << endl << "Avg KEY Clustering"; 
		AvgKEY_Clustering[getAvgKEY()]=getRanklist();
	}
	if (SIG_FORMAT & 0x00000200){
	//	cout << endl << "Avg DATA OP Clustering"; 
		DATA_OP_Clustering[getDATA_OP()]=getRanklist();
	}
	if (SIG_FORMAT & 0x00000400){
	//	cout << endl << "Avg Loop Iter Clustering"; 
		AvgLoopIter_Clustering[getAvgLoopIter()]=getRanklist();
	}
	//init K Cluster Heads
	ClusterHeads[getRanklist().getFirstRank()] =getRanklist();
}

	

void Cluster::BcastClusterFormat(int rank){

	MPI_Comm comm = MPI_COMM_WORLD;
	if(rank ==0)
		SIG_FORMAT = SIG_FORMAT | 0x00000001;

       	PMPI_Bcast(&SIG_FORMAT, 1, MPI_INT, 0, comm);
	if (rank==0){
		cout << endl << "/**********************************************/" << endl ;
		cout << "Signature Format: " << SIG_FORMAT << endl;
		cout << "List of parameter(s) involving is as follows: " << endl;
		if (SIG_FORMAT & 0x00000001){
			cout << endl << "  Call-Path"; 
		}
		if (SIG_FORMAT & 0x00000002){
			cout << endl << "  Count"; 
		}
		if (SIG_FORMAT & 0x00000004){
			cout << endl << "  Communication Time"; 
		}
		if (SIG_FORMAT & 0x00000008){
			cout << endl << "  Computation Time"; 
		}
		if (SIG_FORMAT & 0x00000010){
			cout << endl << "  SRC"; 
		}
		if (SIG_FORMAT & 0x00000020){
			cout << endl << "  DEST"; 
		}
		if (SIG_FORMAT & 0x00000040){
			cout << endl << "  TAG"; 
		}
		if (SIG_FORMAT & 0x00000080){
			cout << endl << "  COLOR"; 
		}
		if (SIG_FORMAT & 0x00000100){
			cout << endl << "  KEY"; 
		}
		if (SIG_FORMAT & 0x00000200){
			cout << endl << "  DATA OP"; 
		}
		if (SIG_FORMAT & 0x00000400){
			cout << endl << "  Loop Iter"; 
		}
		cout << endl << "/**********************************************/" << endl ;
	}
}
int Cluster::getKClusterPackedSize(){

	int size=0;
	map<int, Ranklist>::iterator it;
	size+= sizeof (int); /*size of clusters - len*/
	for(it=ClusterHeads.begin(); it!=ClusterHeads.end(); it++)
	{
		size+= sizeof(int);
		size+= it->second.getPackedSize(); 	
	}
return size; 
}
void Cluster::sendKClusters(int dest){
	int bufsize = getKClusterPackedSize();
	void *buf = malloc(bufsize);
	MPI_Comm comm = MPI_COMM_WORLD;
	int position = 0;
	if(!buf){
		cerr << "Trace::sendKCluster(): cannot allocate buffer" << endl;
		exit(1);
	} 
	//cout << " dest: "<< dest <<" bufsize: " << bufsize << endl;
	PMPI_Send(&bufsize, 1, MPI_INT, dest, 0, comm);
	pack(buf, bufsize, &position, comm, -1); // -1 for KCluster packing and unpacking
	PMPI_Send(buf, position, MPI_PACKED, dest, 0, comm);
	free(buf);
}

void Cluster::recvKClusters(int src, int child){
	int bufsize=0;
	void *buf;
	MPI_Comm comm = MPI_COMM_WORLD;
	int position = 0;
        PMPI_Recv(&bufsize, 1, MPI_INT, src, 0, comm, MPI_STATUS_IGNORE);
	buf = malloc(bufsize);
	if(!buf){
		cerr << "Trace::recvKCluster(): cannot allocate buffer" << endl;
		exit(1);
	}
	PMPI_Recv(buf, bufsize, MPI_PACKED, src, 0, comm, MPI_STATUS_IGNORE);
	if(child == -1 || child==-2)
		unpack(buf, bufsize, &position, comm, child); // Negative value for KCluster packing and unpacking - Also -1/-2 for LEFT/Right child
	else
		cerr << "Lost Your Child" << endl;

	free(buf);
}

void Cluster::BcastKCluster(int rank){

	int bufsize;
	void *buf;
	int position = 0;
	MPI_Comm comm = MPI_COMM_WORLD;
	if (rank ==0)
	{
	        bufsize = getKClusterPackedSize();
	        buf = malloc(bufsize);
        	if(!buf){
	               	cerr << "Trace::sendBcastCluster(): cannot allocate buffer" << endl;
	                exit(1);
	       }

	       	pack(buf, bufsize, &position, comm, -1);
	}

        PMPI_Bcast(&bufsize, 1, MPI_INT, 0, comm);
	if (rank!=0)
	{
		buf = malloc(bufsize);
	        if(!buf){
	       	        cerr << "Trace::recvBcastCluster(): cannot allocate buffer" << endl;
	              	exit(1);
	        }
	}

	PMPI_Bcast(buf, bufsize, MPI_PACKED, 0, comm);
	if (rank != 0){
	/*	while (!ClusterHeads.empty())
		{
			 ClusterHeads.erase( ClusterHeads.begin());
		}
	*/
		unpack(buf, bufsize, &position, comm, -3);
		ClusterHeads = recvClusterHeads;	
		while (!recvClusterHeads.empty())
		{
			 recvClusterHeads.erase(recvClusterHeads.begin());
		}
	}
	free(buf);
}

void Cluster::initSigClusters_L2(int my_rank, int status){
 //findMyCluster(&trace, my_rank); //init call-path sig, Call_Path_Clustering [sig] = head_ranklist; // unroll head rank list and put the head of the cluster the first on the first element of the array
	
	map<int, Ranklist>::iterator it;
	Ranklist myClusterRanklist;
	const vector<int> *other_ranks;
	Ranklist rl;
	int count=-1;
if (status==1){
	int myClusterHead=-1;
	setCallPathHeads(ClusterHeads);
	//CallPathClusterHeads=ClusterHeads;
	for(it=ClusterHeads.begin(); it!=ClusterHeads.end(); it++)
	{
		if(it->second.hasMember(my_rank)){
			myClusterHead= it->first;
			deleteMap(Call_Path_Clustering);	
			Call_Path_Clustering[getCallPath()]=it->second;
			other_ranks= it->second.getRanks();
			rl = it->second;
			count = it->second.getNumRanks();
			break;
		}
	}
	if(myClusterHead==-1)
		cerr << "where is my cluster?" << endl;

	int *temp_rl = (int *)malloc(sizeof(int) * (other_ranks->size()));
	for(unsigned i=0; i<other_ranks->size(); i++)
		 temp_rl[i] = other_ranks->at(i); 
	int unroll [count];
	rl.decompressRL(temp_rl, count, unroll);

	setUnrolledRank(unroll, count);
	if(unroll[0]!=myClusterHead){
		for (int index=0; index< unrolled_rank_size; index++){
			if (unrolled_rank[index]==myClusterHead)
			{
				unrolled_rank[index]= unrolled_rank[0];
				unrolled_rank[0]=myClusterHead;
			}
		}
	}
	while (!ClusterHeads.empty())
		{
			 ClusterHeads.erase( ClusterHeads.begin());
		}
	

	ClusterHeads[getRanklist().getFirstRank()] =getRanklist();
	
	setClusterID(myClusterHead);
}
else if (status==2){

	int unroll[CallPathClusterHeads.size()];
	int index=0;
	for(it=CallPathClusterHeads.begin(); it!=CallPathClusterHeads.end(); it++){
		unroll[index] = it->first; 
		index++;	
	}
	setUnrolledRank(unroll, CallPathClusterHeads.size());

}
else if (status==3){

	int unroll[ClusterHeads.size()];
	int index=0;
	for(it=ClusterHeads.begin(); it!=ClusterHeads.end(); it++){
		unroll[index] = it->first; 
		//if(my_rank==0)
		//	cout << "Cluster Rep ID" << index << ": " << it->first << endl;
		index++;	
	}
	setUnrolledRank(unroll, ClusterHeads.size());
}
else cerr << "cluster level - status not recognized";


}


void Cluster::sendClusterHeadstoRoot(int my_rank){

	if (my_rank !=0)
	{
		sendKClusters(0);
	}

	if (my_rank==0)
	{
		int bufsize=0;
		void *buf;
		MPI_Comm comm = MPI_COMM_WORLD;
		int position = 0;
	        PMPI_Recv(&bufsize, 1, MPI_INT, MPI_ANY_SOURCE, 0, comm, MPI_STATUS_IGNORE);
		buf = malloc(bufsize);
		if(!buf){
			cerr << "Trace::recvKCluster(): cannot allocate buffer" << endl;
			exit(1);
		}
		PMPI_Recv(buf, bufsize, MPI_PACKED, MPI_ANY_SOURCE, 0, comm, MPI_STATUS_IGNORE);
		unpack(buf, bufsize, &position, comm, -3); 
		free(buf);
	}
	while (!ClusterHeads.empty())
	{
		ClusterHeads.erase(ClusterHeads.begin());
	}
	ClusterHeads = recvClusterHeads;	

}


void Cluster::printSignatures(){
	map<long long , Ranklist>::iterator it;
	map<long long , Ranklist>::iterator it2;
	map<int, Ranklist>::iterator it3;

	if (SIG_FORMAT & 0x00000001){ 	// "Call Path Clustering"; 
		for( it=Call_Path_Clustering.begin(); it!=Call_Path_Clustering.end(); it++)
		{
			cout << hex<< (*it).first << " => " <<(*it).second.toString() << endl;
		}	
	}
	if (SIG_FORMAT & 0x00000002){// "Avg Count Clustering"; 
		for( it=AvgCount_Clustering.begin(); it!=AvgCount_Clustering.end(); it++)
		{
			cout << hex<< (*it).first << " => " <<(*it).second.toString() << endl;
		}	
	}
	if (SIG_FORMAT & 0x00000004){ 	// "Avg Comm Clustering"; 
		for( it=AvgCommTime_Clustering.begin(); it!=AvgCommTime_Clustering.end(); it++)
		{
			cout << hex<< (*it).first << " => " <<(*it).second.toString() << endl;
		}	
	}
	if (SIG_FORMAT & 0x00000008){ 	// "Avg Comp Clustering"; 
		for( it=AvgCompTime_Clustering.begin(); it!=AvgCompTime_Clustering.end(); it++)
		{
			cout << hex<< (*it).first << " => " <<(*it).second.toString() << endl;
		}	
	}
	if (SIG_FORMAT & 0x00000010){ 	// "Avg SRC Clustering"; 
		for( it=AvgSRC_Clustering.begin(); it!=AvgSRC_Clustering.end(); it++)
		{
			cout << hex<< (*it).first << " => " <<(*it).second.toString() << endl;
		}	
	}
	if (SIG_FORMAT & 0x00000020){//	"Avg DEST Clustering"; 
		for( it=AvgDEST_Clustering.begin(); it!=AvgDEST_Clustering.end(); it++)
		{
			cout << hex<< (*it).first << " => " <<(*it).second.toString() << endl;
		}	
	}
	if (SIG_FORMAT & 0x00000040){ // "Avg TAG Clustering"; 
		for( it2=AvgTAG_Clustering.begin(); it2!=AvgTAG_Clustering.end(); it2++)
		{
			cout << hex<< (*it2).first << " => " <<(*it2).second.toString() << endl;
		}	
	}
	if (SIG_FORMAT & 0x00000080){ 	// "Avg COLOR Clustering";
		for( it2=AvgCOLOR_Clustering.begin(); it2!=AvgCOLOR_Clustering.end(); it2++)
		{
			cout << hex<< (*it2).first << " => " <<(*it2).second.toString() << endl;
		}	
	}
	if (SIG_FORMAT & 0x00000100){ 	// "Avg KEY Clustering"; 
		for( it2=AvgKEY_Clustering.begin(); it2!=AvgKEY_Clustering.end(); it2++)
		{
			cout << hex<< (*it2).first << " => " <<(*it2).second.toString() << endl;
		}	
	}
	if (SIG_FORMAT & 0x00000200){ //cout << endl << "Avg DATA OP Clustering"; 
		for( it=DATA_OP_Clustering.begin(); it!=DATA_OP_Clustering.end(); it++)
		{
			cout << hex<< (*it).first << " => " <<(*it).second.toString() << endl;
		}	
	}
	if (SIG_FORMAT & 0x00000400){ 	 // "Avg Loop Iter Clustering"; 
		for( it=AvgLoopIter_Clustering.begin(); it!=AvgLoopIter_Clustering.end(); it++)
		{
			cout << hex<< (*it).first << " => " <<(*it).second.toString() << endl;
		}	
        }

	for( it3=ClusterHeads.begin(); it3!=ClusterHeads.end(); it3++)
	{
		cout <<"Cluster Head: " << (*it3).first << " => Cluster Ranklist: " <<(*it3).second.toString() << endl;
	}	

}

int Cluster::findUnrolledRank(int rank){
	int pos=0;
	//cout << unrolled_rank_size << " : sizes" << endl;
	while( pos< unrolled_rank_size)
	{
		if (unrolled_rank[pos]==rank)
			return pos;
		pos++;
	}	

	cout<< endl <<"Couldn't find the rank of task"<<rank<< " in the second level of clustering!" << endl;
	exit(-1);
}

/*
void Cluster::merge(map<int, Ranklist> &LHeads, map<int, Ranklist> &RHeads, int myrank){
	
	map<int, Ranklist>::iterator it;
	map<int, Ranklist> myMap;
	myMap.insert(LHeads.begin(), LHeads.end());
	myMap.insert(RHeads.begin(), RHeads.end());
	
	while (!LHeads.empty())
        {
		LHeads.erase(LHeads.begin());
        }

	while (!RHeads.empty())
	{
 		RHeads.erase(RHeads.begin());
	}



	 map<long long , Ranklist>::iterator itc;
	// First, check if there is any call-path - If so, we MUST at least pick on cluster per each call-path 
	if(SIG_FORMAT & 0x00000001){
           for( itc=Call_Path_Clustering.begin(); itc!=Call_Path_Clustering.end(); itc++)
		formClusters(LHeads, itc, myrank);
	}
	else if (SIG_FORMAT & 0x00000010  || SIG_FORMAT & 0x00000020){

           for( itc=AvgSRC_Clustering.begin(); itc!=AvgSRC_Clustering.end(); itc++)
		formClusters(LHeads, itc, myrank);

	}
}
*/

void Cluster::merge(map<int, Ranklist> &LHeads, map<int, Ranklist> &RHeads, int myrank){
	
	map<int, Ranklist>::iterator it;
	map<int, Ranklist> myMap;
	myMap.insert(LHeads.begin(), LHeads.end());
	myMap.insert(RHeads.begin(), RHeads.end());
	
	while (!LHeads.empty())
        {
		LHeads.erase(LHeads.begin());
        }

	while (!RHeads.empty())
	{
 		RHeads.erase(RHeads.begin());
	}

/*	if(myrank==0){
		for (int index=0; index<17; index++){
			Ranklist ranklist(index);
			LHeads[index] = ranklist;
		}
	
		return;

	}
*/

	 map<long long , Ranklist>::iterator itc, it_temp;
	// First, check if there is any call-path - If so, we MUST at least pick on cluster per each call-path 
	if(myrank == 0){
		//cout << "Size of AvgComp Signatures: " << AvgCompTime_Clustering.size() << endl;
		//cout << "Size of CallPath Signatures: " << Call_Path_Clustering.size() << endl;
/*	        for( itc=Call_Path_Clustering.begin(); itc!=Call_Path_Clustering.end(); itc++){
			cout << "#### Chameleon: Call-Path Signature: " << itc->first << " Ranklist: " << itc->second.toString() << endl; 
		}
	        for( itc=AvgCompTime_Clustering.begin(); itc!=AvgCompTime_Clustering.end(); itc++){
			cout << "#### Chameleon: Compute Signature: " << itc->first << " Ranklist: " << itc->second.toString() << endl; 
		}
		
		 for( itc=AvgSRC_Clustering.begin(); itc!=AvgSRC_Clustering.end(); itc++){
			cout << "#### Chameleon: SRC Signature: " << itc->first << " Ranklist: " << itc->second.toString() << endl; 
		}
*/

	}
	
        if ((SIG_FORMAT & 0x00000001)){ // && Call_Path_Clustering.size()>1){ 
		unsigned int little_k= (unsigned int) K / Call_Path_Clustering.size();
		//int resid_little_k = K % Call_Path_Clustering.size();

		int size=  Call_Path_Clustering.size();	
		int NumSelectedClusters = 0;
		for( itc=Call_Path_Clustering.begin(); itc!=Call_Path_Clustering.end(); itc++){
			//formClusters(LHeads, itc, myrank);
			formClusters(RHeads, itc, myrank, little_k);

			size--;
			
			if(RHeads.size()>little_k){
				cerr << "Number of selected clusters is more than little K" << endl;
			}
			LHeads.insert(RHeads.begin(), RHeads.end());
			NumSelectedClusters = NumSelectedClusters + RHeads.size();
		
			if(RHeads.size()<little_k && size!=0){
				//cout << "RHeads.size()<=little_k" << endl;
				//Adjust little K
				little_k= (int) (K-NumSelectedClusters) / size;
				cout << "#### Chameleon: Adjusted # of Clusters: " << little_k << endl;
			}

			
/*
			else
			{
				//cout << "RHeads.size()>little_k" << endl;
				//LHeads.insert(RHeads.begin(), RHeads.begin()+little_k);
				int index=0;
				int tempK=little_k;
				if(RHeads.size()> little_k && resid_little_k > 0){
					tempK=little_k + 1; resid_little_k--;
				}
           			for(it=RHeads.begin(); it!=RHeads.end() && index<tempK; it++){
					LHeads.insert(*it);
					index++;
				}
			}
*/
			//Clean up the RHeads - Selected clusters
	       	        while (!RHeads.empty())
      			{
		           RHeads.erase(RHeads.begin());
	        	}
		}
	}
	else if (SIG_FORMAT & 0x00000010  || SIG_FORMAT & 0x00000020){
	  unsigned int little_k= (unsigned int) K /AvgSRC_Clustering.size();
	  //int resid_little_k = K % AvgSRC_Clustering.size();
          for( itc=AvgSRC_Clustering.begin(); itc!=AvgSRC_Clustering.end(); itc++)
		formClusters(LHeads, itc, myrank, little_k);

	}
	else if (SIG_FORMAT & 0x00000008){ //AvgCompTime
		unsigned int little_k= (unsigned int) K/ AvgCompTime_Clustering.size();
		//int resid_little_k = K%AvgCompTime_Clustering.size();

		int size=  AvgCompTime_Clustering.size();	
		int NumSelectedClusters = 0;

		for( itc=AvgCompTime_Clustering.begin(); itc!=AvgCompTime_Clustering.end(); itc++){
			//formClusters(LHeads, itc, myrank);
			formClusters(RHeads, itc, myrank, little_k);

			size --;

			if(RHeads.size()>little_k){
				cerr << "#### Chameleon: Number of selected clusters is more than little K" << endl;
			}
			LHeads.insert(RHeads.begin(), RHeads.end());
			NumSelectedClusters = NumSelectedClusters + RHeads.size();
		
			if(RHeads.size()<little_k && size!=0){
				//cout << "RHeads.size()<=little_k" << endl;
				//cout << "Num Selected Clusters" << NumSelectedClusters << endl;
				//cout << "Size" << size << endl;
				//Adjust little K
				little_k= (int) (K-NumSelectedClusters) / size;
				cout << "#### Chameleon: Adjusted # of Clusters: " << little_k << endl;
			}
		
		        while (!RHeads.empty())
      			{
		           RHeads.erase(RHeads.begin());
	        	}
		}
	}
}


/*void Cluster::BcastCallPathCluster(int rank){
	int bufsize;
	void *buf;
	int position = 0;
	MPI_Comm comm = MPI_COMM_WORLD;
	map<long long, Ranklist>::iterator it;
	if (rank ==0)
	{

		bufsize += sizeof (int); //size of clusters - len
		for( it=Call_Path_Clustering.begin(); it!=Call_Path_Clustering.end(); it++)
		{
			bufsize += sizeof(long long );
			bufsize += it->second.getPackedSize(); 	
		}//	cout << endl << "Call Path Clustering"; 

		buf = malloc(bufsize);
	        position = 0;
	        if(!buf){
        	        cerr << "Trace::sendCluster(): cannot allocate buffer" << endl;
                	exit(1);
        	}	
		
        	pack(buf, bufsize, &position, comm, CALL_PATH); 

	}

        PMPI_Bcast(&bufsize, 1, MPI_INT, 0, comm);
	if (rank!=0)
	{

		buf = malloc(bufsize);
	        if(!buf){
        	        cerr << "Trace::recvCluster(): cannot allocate buffer" << endl;
                	exit(1);
	        }
	}

	PMPI_Bcast(buf, bufsize, MPI_PACKED, 0, comm);
	if (rank != 0){
		unpack(buf, bufsize, &position, comm, CALL_PATH);
		Call_Path_Clustering = recv_Call_Path_Clustering;	
	}
        
	free(buf);

}*/

Ranklist Cluster::getClusterRanklist(int rank){

	map<int, Ranklist>::iterator it;

	for(it=ClusterHeads.begin(); it!=ClusterHeads.end(); ++it)
		if(it->first == rank)
			return it->second;
	
	cerr << "#### Chameleon: Cluster Rank not found" << endl;
	exit(0);

}

vector<int> Cluster::getArrayClusterHeads(){
	
	vector<int> temp;
	map<int, Ranklist>::iterator it;
	for(it=ClusterHeads.begin(); it!=ClusterHeads.end(); ++it){
		temp.push_back(it->first);	
	}

	return temp;
}


int Cluster::onlyHeadCluster(int myrank){
	
	map<int, Ranklist>::iterator it;
	for(it=ClusterHeads.begin(); it!=ClusterHeads.end(); ++it){
		if (myrank == it->first){
			//cout << "Only Head Cluster Representative Rank: " << myrank << endl; 
			return myrank;	
		}
	}


	return -1;
}

void Cluster::mergeKClusters(int myrank){

	//merge(ClusterHeads, recvClusterHeads, myrank);
	
	if(!recvClusterHeads_Left.empty() && !recvClusterHeads_Right.empty()){
		//merge(recvClusterHeads_Left, recvClusterHeads_Right, myrank);	
		//merge(ClusterHeads, recvClusterHeads_Left, myrank);
		recvClusterHeads_Left.insert(recvClusterHeads_Right.begin(), recvClusterHeads_Right.end());
		merge(ClusterHeads, recvClusterHeads_Left, myrank);
	}
	else if(!recvClusterHeads_Left.empty() && recvClusterHeads_Right.empty()){
		merge(ClusterHeads, recvClusterHeads_Left, myrank);
	}
	else if(recvClusterHeads_Left.empty() && !recvClusterHeads_Right.empty()){
		merge(ClusterHeads, recvClusterHeads_Right, myrank);
	}
	else{ // (recvClusterHeads_Left.empty() && recvClusterHeads_Right.empty())

		map<int, Ranklist>::iterator it;
		map<int, Ranklist> tmpHeads;
		Ranklist myClusterRanklist;
		const vector<int> *other_ranks;
		Ranklist rl;
		int count=-1;
		cout << "K Cluster Head Size: " << ClusterHeads.size() << endl;
		for(it=ClusterHeads.begin(); it!=ClusterHeads.end(); it++)
		{
			other_ranks= it->second.getRanks();
			rl = it->second;
			count = it->second.getNumRanks();
			int *temp_rl = (int *)malloc(sizeof(int) * (other_ranks->size()));
			for(unsigned i=0; i<other_ranks->size(); i++)
				 temp_rl[i] = other_ranks->at(i); 
			int unroll [count];
			rl.decompressRL(temp_rl, count, unroll);
			for(int j=0; j< (int)count; j++){
				Ranklist rl(unroll[j]);
				tmpHeads[unroll[j]]=rl;
				//cout <<"unroll[j]" <<  unroll[j] << endl; 
			}
		}
		
		//cout << "Cluster Head Size: " << ClusterHeads.size() << endl;
		while (!ClusterHeads.empty())
		{
			 ClusterHeads.erase(ClusterHeads.begin());
		}
		while (!recvClusterHeads_Left.empty())
		{
			 recvClusterHeads_Left.erase(recvClusterHeads_Left.begin());

		}
	

		ClusterHeads=tmpHeads;
		merge(ClusterHeads, recvClusterHeads_Left, myrank);
	}
}


void Cluster::formClusters(map<int, Ranklist> &LHeads,  map<long long , Ranklist>::iterator it, int myrank, unsigned int little_k)
{
	   		map<long long , Ranklist>::iterator it2;
           		map<long long, Ranklist> Second_Clusters;
                       // cout << hex<< (*it).first << " => " <<(*it).second.toString() << endl;
			if(SIG_FORMAT & 0x00000010  || SIG_FORMAT & 0x00000020 || SIG_FORMAT & 0x00000040 || SIG_FORMAT & 0x00000080 || SIG_FORMAT & 0x00000100 || SIG_FORMAT & 0x00000002 || SIG_FORMAT & 0x00000008)
			{

				long long SRC_Sig=0;
				long long DEST_Sig=0;
				long long TAG_Sig=0;
				long long KEY_Sig=0;
				long long COLOR_Sig=0;
				long long COMP_Sig=0;				

				const vector<int> *other_ranks;
				Ranklist rl;
				int count=-1;

				other_ranks= it->second.getRanks();
				rl = it->second;
				count = it->second.getNumRanks();
				int *temp_rl = (int *)malloc(sizeof(int) * (other_ranks->size()));
				for(unsigned i=0; i<other_ranks->size(); i++)
					temp_rl[i] = other_ranks->at(i); 
				int unroll [count];
				rl.decompressRL(temp_rl, count, unroll);

				map<long long , Ranklist>::iterator it2,it3;
				
/*           			for( it2=AvgCOLOR_Clustering.begin(); it2!=AvgCOLOR_Clustering.end(); it2++)
					cout << "COLOR: " << hex<< (*it2).first << " => " <<(*it2).second.toString() << endl;

           			for( it3=AvgKEY_Clustering.begin(); it3!=AvgKEY_Clustering.end(); it3++)
					cout << "KEY: " << hex<< (*it3).first << " => " <<(*it3).second.toString() << endl;

           			for( it3=AvgCount_Clustering.begin(); it3!=AvgCount_Clustering.end(); it3++)
					cout << "COUNT: " << hex<< (*it3).first << " => " <<(*it3).second.toString() << endl;
*/
				for(int index=0; index<count; index++){

					Ranklist ranklist(unroll[index]);
			
					if (SIG_FORMAT & 0x00000008)
			    	         	findMySig(AvgCompTime_Clustering, COMP_Sig, unroll[index]);
	        			if (SIG_FORMAT & 0x00000010)
		 	      	         	findMySig(AvgSRC_Clustering, SRC_Sig, unroll[index]);
        				if (SIG_FORMAT & 0x00000020)
              					findMySig(AvgDEST_Clustering, DEST_Sig, unroll[index]);
					if (SIG_FORMAT & 0x00000040 && (!(SIG_FORMAT & 0x00000010) && !(SIG_FORMAT & 0x00000020)))
              					findMySig(AvgTAG_Clustering, TAG_Sig, unroll[index]);
					if (SIG_FORMAT & 0x00000080)
              					findMySig(AvgCOLOR_Clustering, COLOR_Sig, unroll[index]);
					if (SIG_FORMAT & 0x00000100)
              					findMySig(AvgKEY_Clustering, KEY_Sig, unroll[index]);
				
					long long key = SRC_Sig + DEST_Sig + KEY_Sig + COLOR_Sig + TAG_Sig;
				
					it2 = Second_Clusters.find(key);
				        if (it2!=Second_Clusters.end())
			       	 	{
				        	//Existing Cluster - just modify ranklist
		       	        	 	it2->second.merge(&ranklist);
			        	}
					else
       		        		{
						//New Cluster
       			 			Second_Clusters[key]= ranklist;
       		        		}
				}
				
				/*Adding Clusters to the Head List*/	
				for( it2=Second_Clusters.begin(); it2!=Second_Clusters.end(); it2++)
	 	 			 LHeads[(*it2).second.getFirstRank()] = (*it2).second;
	
				while (!Second_Clusters.empty())
				{
					Second_Clusters.erase(Second_Clusters.begin());
				}
			}
			else{ 
	 	 		LHeads[(*it).second.getFirstRank()] = (*it).second;
			}


	if(LHeads.size()>little_k){
		/*Dynamically Update K*/	
		if(SIG_FORMAT & 0x00000001){
			if(Call_Path_Clustering.size()> (unsigned int) K){	
				cout << "WARNING: Number of Call-Path is more than the inpout K!" << endl;
				return;
			}
		}
	
		/*Removing Extra Clusters*/
		int diff = LHeads.size() - little_k;
	   	map<int , Ranklist>::iterator iterHeads, iter;
		iterHeads=LHeads.begin();
		iter=LHeads.begin();
		
		for(int index=0; index<diff; index++)
			iterHeads++;

		for(int index=0; index<diff; index++){
			iterHeads->second.merge(&iter->second);
			iter++;
		}		
			
		while (LHeads.size()>little_k)
		{
			LHeads.erase(LHeads.begin());
		}
	}
}

void Cluster::findTopK_SRC_DEST(map<int, Ranklist> &Heads){

	map<int, Ranklist>::iterator it;
	map<int, Ranklist> myMap;

	myMap.insert(Heads.begin(), Heads.end());

	while (!Heads.empty())
        {
		Heads.erase(Heads.begin());
        }

	if ((int)myMap.size() > K) {
		int Cluster[myMap.size()];	
		int freq [myMap.size()];
		//__int128_t 
		double totalDist[myMap.size()];
		bool selectedStatus[myMap.size()];
		double  MaxDist=-1;
		int selectedCluster=-1;
		double distanceMatrix[myMap.size()][myMap.size()];
		vector<int> currentSelection;

		for(int i=0;i<(int)myMap.size(); i++)
			for (int j=0; j<(int)myMap.size(); j++)	
				distanceMatrix[i][j]=0;
	
		int i=0, j=0;
		//cout << "Map Size: " << myMap.size() << endl;
		for(it=myMap.begin(); it!=myMap.end(); ++it, i++){
			Cluster[i] = it->first; 
			//cout << "Map member: " << Cluster[i] << endl;
			freq[i] = it->second.getNumRanks(); 
			selectedStatus[i]=false;
		}
		
		/*Calc Distance Matrix*/
		for(i=0; i<(int)myMap.size()-1; i++){
			j=i+1;
			for(;j<(int)myMap.size(); j++){

				long long sig_i; 
				long long sig_j; 
				double dist = 0;


				if (SIG_FORMAT & 0x00000010){
					findMySig(AvgSRC_Clustering, sig_i, Cluster[i]);
					findMySig(AvgSRC_Clustering, sig_j, Cluster[j]);
					dist += calcDist(sig_i, sig_j, 0);//weight =0 
				}
				//cout << "SRC Sig Rank: " << Cluster[i] << " : " << sig_i << " and  SRC Sig Rank: " << Cluster[j] <<" : " << sig_j << endl;
				if (SIG_FORMAT & 0x00000020){
					findMySig(AvgDEST_Clustering, sig_i, Cluster[i]);
					findMySig(AvgDEST_Clustering, sig_j, Cluster[j]);
					dist += calcDist(sig_i, sig_j, 0); // weight =0
				}
				//cout << "SRC Sig Rank: " << Cluster[i] << " : " << sig_i << " and  SRC Sig Rank: " << Cluster[j] <<" : " << sig_j << endl;

				distanceMatrix[i][j]=distanceMatrix[j][i]=dist; //findDist(Cluster[i], Cluster[j]);	
				//cout << "Dist b/w rank: " << Cluster[i] << " and  rank: " << Cluster[j] << " DIST: "<< dist<< endl;
			}
		}

		/**/
		for(int index=0; index<K; index++ ){
	
			if (Heads.empty()){		
				for (int index1=0; index1<(int)myMap.size(); index1++)
					if(selectedStatus[index1]==false){
						totalDist[index1]=0;
						for (int index2=0; index2<(int)myMap.size(); index2++){
							if(selectedStatus[index2]==false){
								totalDist[index1]+=distanceMatrix[index1][index2];
							}
						}
						//cout << "1Total Distance for node: "<< Cluster[index1] << " " << totalDist[index1] << endl;
					}
			}
			else{
				for (int index1=0; index1<(int)myMap.size(); index1++)
					if(selectedStatus[index1]==false){
						totalDist[index1]= DBL_MAX;
						for (int index2=0; index2<(int)currentSelection.size(); index2++){
							{
								if (totalDist[index1] > distanceMatrix[index1][currentSelection.at(index2)])
								{
									totalDist[index1] = distanceMatrix[index1][currentSelection.at(index2)];
								//	cout << "node: "<< Cluster[index1] << " " <<Cluster[currentSelection.at(index2)]<< " : " <<totalDist[index1] << "  Size: " << currentSelection.size() << endl;
								}
							}
						}
						//cout << "2Total Distance for node: "<< Cluster[index1] << " " << totalDist[index1] << endl;
					}
			}
			
			MaxDist =-1;	
			for (int index1=0; index1<(int)myMap.size(); index1++){
				if(selectedStatus[index1]==false){
					totalDist[index1]*= freq[index1]; //freq = number of cluster members 
					//cout << "   "<< totalDist[index1]; 
					if(MaxDist<totalDist[index1]){
						MaxDist=totalDist[index1];
						selectedCluster=index1;
					}
				}
			}
		
			currentSelection.insert(currentSelection.begin(), selectedCluster);
			//cout <<endl << "selected: " << selectedCluster << " Cluster:" <<  Cluster[selectedCluster] << endl;	
			//cout << "my map found: " <<myMap.find(Cluster[selectedCluster])->first << endl;
			if(myMap.find(Cluster[selectedCluster])->first == Cluster[selectedCluster])
			{
				//ClusterHeads[Cluster[selectedCluster]] = myMap.find(Cluster[selectedCluster])->second; 
				Heads[Cluster[selectedCluster]] = myMap.find(Cluster[selectedCluster])->second; 
				selectedStatus[selectedCluster] = true;	
			}
			else
				cerr << "Selected Cluster is wrong!" << endl;
	
		} 
	
		//Merge dropped clusters with the selected ones
		double minDist=DBL_MAX;
		int mergingCluster=-1;
		for(int index=0; index<(int)myMap.size(); index++){
			if(selectedStatus[index]==false) //dropped
			{
				//find the closest cluster
				for(int index1=0; index1<(int)myMap.size(); index1++){
					if(selectedStatus[index1] == true) //selected cluster
					{
						if(distanceMatrix[index][index1]<minDist)
						{	
							mergingCluster=index1;
							minDist=distanceMatrix[index][index1];
						}
					}
				}
				  if (mergingCluster==-1)
					cerr << "couldn't find any close cluster" <<endl;
				  else
				  	Heads.find(Cluster[mergingCluster])->second.merge(&myMap.find(Cluster[index])->second);
				  	//ClusterHeads.find(Cluster[mergingCluster])->second.merge(&myMap.find(Cluster[index])->second);
			
				minDist=DBL_MAX;
				mergingCluster=-1;
			}				
		}
	}
	else{ //K > mymap.size
		Heads=myMap;
	}	
		
	while (!myMap.empty())
        {
 		myMap.erase(myMap.begin());
        }


}


/*
void Cluster::mergeKClusters(int myrank){

	//merge(ClusterHeads, recvClusterHeads, myrank);
	
	if(!recvClusterHeads_Left.empty() && !recvClusterHeads_Right.empty()){
		merge(recvClusterHeads_Left, recvClusterHeads_Right, myrank);	
		merge(ClusterHeads, recvClusterHeads_Left, myrank);
	}
	else if(!recvClusterHeads_Left.empty() && recvClusterHeads_Right.empty()){
		merge(ClusterHeads, recvClusterHeads_Left, myrank);
	}
	else if(recvClusterHeads_Left.empty() && !recvClusterHeads_Right.empty()){
		merge(ClusterHeads, recvClusterHeads_Right, myrank);
	}
	else{ // (recvClusterHeads_Left.empty() && recvClusterHeads_Right.empty())

		map<int, Ranklist>::iterator it;
		map<int, Ranklist> tmpHeads;
		Ranklist myClusterRanklist;
		const vector<int> *other_ranks;
		Ranklist rl;
		int count=-1;
		cout << "Cluster Head Size: " << ClusterHeads.size() << endl;
		for(it=ClusterHeads.begin(); it!=ClusterHeads.end(); it++)
		{
			other_ranks= it->second.getRanks();
			rl = it->second;
			count = it->second.getNumRanks();
			int *temp_rl = (int *)malloc(sizeof(int) * (other_ranks->size()));
			for(unsigned i=0; i<other_ranks->size(); i++)
				 temp_rl[i] = other_ranks->at(i); 
			int unroll [count];
			rl.decompressRL(temp_rl, count, unroll);
			for(unsigned j=0; j< (int)count; j++){
				Ranklist rl(unroll[j]);
				tmpHeads[unroll[j]]=rl;
				cout <<"unroll[j]" <<  unroll[j] << endl; 
			}
		}
		
		cout << "Cluster Head Size: " << ClusterHeads.size() << endl;
		while (!ClusterHeads.empty())
		{
			 ClusterHeads.erase(ClusterHeads.begin());
		}
		while (!recvClusterHeads_Left.empty())
		{
			 recvClusterHeads_Left.erase(recvClusterHeads_Left.begin());
		}
	

		ClusterHeads=tmpHeads;

		merge(ClusterHeads, recvClusterHeads_Left, myrank);
	}

}


void Cluster::merge(map<int, Ranklist> &LHeads, map<int, Ranklist> &RHeads, int myrank){
	
	map<int, Ranklist>::iterator it;
	map<int, Ranklist> myMap;
//
//	myMap.insert(ClusterHeads.begin(), ClusterHeads.end());
//	myMap.insert(recvClusterHeads.begin(), recvClusterHeads.end());
//
//	while (!recvClusterHeads.empty())
//       {
//		recvClusterHeads.erase(recvClusterHeads.begin());
//      }
//
//	while (!ClusterHeads.empty())
//	{
// 		ClusterHeads.erase(ClusterHeads.begin());
//	}
//
	myMap.insert(LHeads.begin(), LHeads.end());
	myMap.insert(RHeads.begin(), RHeads.end());

	while (!LHeads.empty())
        {
		LHeads.erase(LHeads.begin());
        }

	while (!RHeads.empty())
	{
 		RHeads.erase(RHeads.begin());
	}

	if ((int)myMap.size() > K) {
		int Cluster[myMap.size()];	
		int freq [myMap.size()];
		//__int128_t 
		double totalDist[myMap.size()];
		bool selectedStatus[myMap.size()];
		double  MaxDist=-1;
		int selectedCluster=-1;
		double distanceMatrix[myMap.size()][myMap.size()];
		vector<int> currentSelection;

		for(int i=0;i<(int)myMap.size(); i++)
			for (int j=0; j<(int)myMap.size(); j++)	
				distanceMatrix[i][j]=0;
	
		int i=0, j=0;
		//cout << "Map Size: " << myMap.size() << endl;
		for(it=myMap.begin(); it!=myMap.end(); ++it, i++){
			Cluster[i] = it->first; 
			//cout << "Map member: " << Cluster[i] << endl;
			freq[i] = it->second.getNumRanks(); 
			selectedStatus[i]=false;
		}

		for(i=0; i<(int)myMap.size()-1; i++){
			j=i+1;
			for(;j<(int)myMap.size(); j++){
				distanceMatrix[i][j]=distanceMatrix[j][i]=findDist(Cluster[i], Cluster[j]);	
				//cout << "Dist b/w rank: " << Cluster[i] << " and  rank: " << Cluster[j] << endl;
			}
		}

		for(int index=0; index<K; index++ ){
	
			//if (ClusterHeads.empty()){		
			if (LHeads.empty()){		
				for (int index1=0; index1<(int)myMap.size(); index1++)
					if(selectedStatus[index1]==false){
						totalDist[index1]=0;
						for (int index2=0; index2<(int)myMap.size(); index2++){
							if(selectedStatus[index2]==false){
								totalDist[index1]+=distanceMatrix[index1][index2];
							}
						}
						//cout << "1Total Distance for node: "<< Cluster[index1] << " " << totalDist[index1] << endl;
					}
			}
			else{
				for (int index1=0; index1<(int)myMap.size(); index1++)
					if(selectedStatus[index1]==false){
						totalDist[index1]= DBL_MAX;
						for (int index2=0; index2<(int)currentSelection.size(); index2++){
							{
								if (totalDist[index1] > distanceMatrix[index1][currentSelection.at(index2)])
								{
									totalDist[index1] = distanceMatrix[index1][currentSelection.at(index2)];
								//	cout << "node: "<< Cluster[index1] << " " <<Cluster[currentSelection.at(index2)]<< " : " <<totalDist[index1] << "  Size: " << currentSelection.size() << endl;
								}
							}
						}
						//cout << "2Total Distance for node: "<< Cluster[index1] << " " << totalDist[index1] << endl;
					}
			}
			
			MaxDist =-1;	
			for (int index1=0; index1<(int)myMap.size(); index1++){
				if(selectedStatus[index1]==false){
					totalDist[index1]*= freq[index1]; //freq = number of cluster members 
				//	cout << "   "<< totalDist[index1]; 
					if(MaxDist<totalDist[index1]){
						MaxDist=totalDist[index1];
						selectedCluster=index1;
					}
				}
			}
		
			currentSelection.insert(currentSelection.begin(), selectedCluster);
			//cout <<endl << "selected: " << selectedCluster << " Cluster:" <<  Cluster[selectedCluster] << endl;	
			//cout << "my map found: " <<myMap.find(Cluster[selectedCluster])->first << endl;
			if(myMap.find(Cluster[selectedCluster])->first == Cluster[selectedCluster])
			{
				//ClusterHeads[Cluster[selectedCluster]] = myMap.find(Cluster[selectedCluster])->second; 
				LHeads[Cluster[selectedCluster]] = myMap.find(Cluster[selectedCluster])->second; 
				selectedStatus[selectedCluster] = true;	
			}
			else
				cerr << "Selected Cluster is wrong!" << endl;
	
		} 
	
		//Merge dropped clusters with the selected ones
		double minDist=DBL_MAX;
		int mergingCluster=-1;
		for(int index=0; index<(int)myMap.size(); index++){
			if(selectedStatus[index]==false) //dropped
			{
				//find the closest cluster
				for(int index1=0; index1<(int)myMap.size(); index1++){
					if(selectedStatus[index1] == true) //selected cluster
					{
						if(distanceMatrix[index][index1]<minDist)
						{	
							mergingCluster=index1;
							minDist=distanceMatrix[index][index1];
						}
					}
				}
				  if (mergingCluster==-1)
					cerr << "couldn't find any close cluster" <<endl;
				  else
				  	LHeads.find(Cluster[mergingCluster])->second.merge(&myMap.find(Cluster[index])->second);
				  	//ClusterHeads.find(Cluster[mergingCluster])->second.merge(&myMap.find(Cluster[index])->second);
			
				minDist=DBL_MAX;
				mergingCluster=-1;
			}				
		}
	}
	else{ //K > mymap.size
		LHeads=myMap;
	}	
		
	while (!myMap.empty())
        {
 		myMap.erase(myMap.begin());
        }

}
*/
//Output print KClusters


/*void Cluster::findMinScore(int &tempKey, int &tempVal){
	map<int, int>::iterator it;
	tempKey=-1;
	tempVal= INT_MAX;	
	for( it=ClusterHeads.begin(); it!=ClusterHeads.end(); it++){
		if (tempVal>it->second)
		{
			tempKey = it->first;
			tempVal = it->second;
		}	
	}
}*/
void Cluster::findMySig(map<long long, Ranklist> &myMap, long long &sig_i, int rank){
	map<long long, Ranklist>::iterator it;
	for(it=myMap.begin(); it!=myMap.end(); ++it){
		if(it->second.hasMember(rank)){
			sig_i=it->first;
			return;
		}
	}	

	cerr << endl << "No match for the input Cluster Head with rank: " << rank << endl;
}

long long Cluster::findVariation(map<long long, Ranklist> &myMap){
	long long tmp =0;
	map<long long, Ranklist>::iterator it;
	for(it=myMap.begin(); it!=myMap.end(); ++it){
		if (tmp < it->first)
			tmp = it->first;
		}	
   return tmp;

}
void Cluster::findWeight(int &w1, int &w2, int &w3, int &w4){

	map<long long, Ranklist>::iterator it;
	long long largeComp=0, largeComm=0, largeCount=0, largeLoop=0, largeSRC=0, largeDEST=0, largeCP=0, largeTAG=0, largeKEY=0, largeCOLOR=0;
	long long g1=0, g2=0, g3=0;

   if(L==0 || L==2)
    {
	if (SIG_FORMAT & 0x00000002){ //AvgCount
		largeCount = findVariation(AvgCount_Clustering);
		if (g1 <largeCount)
			g1=largeCount;
	}
	if (SIG_FORMAT & 0x00000004){ //AvgCommTime
		largeComm = findVariation(AvgCommTime_Clustering);
		if (g1 <largeComm)
			g1=largeComm;
	}
	if (SIG_FORMAT & 0x00000008){ //AvgCompTime
		largeComp = findVariation(AvgCompTime_Clustering);
		if (g1 <largeComp)
			g1=largeComp;
	}
	if (SIG_FORMAT & 0x00000400){ //AvgLoopIter
		largeLoop= findVariation(AvgLoopIter_Clustering);
		if (g2 <largeLoop)
			g2=largeLoop;
	}
	if (SIG_FORMAT & 0x00000010){ //SRC
		largeSRC= findVariation(AvgSRC_Clustering);
		if (g3 <largeSRC)
			g3=largeSRC;
	}
	if (SIG_FORMAT & 0x00000020){ //DEST
		largeDEST= findVariation(AvgDEST_Clustering);
		if (g3 <largeDEST)
			g3=largeDEST;
	}
	if (SIG_FORMAT & 0x00000040){//TAG
		largeTAG= findVariation(AvgTAG_Clustering);
		if (g3 <largeTAG)
			g3=largeTAG;
	}
	if (SIG_FORMAT & 0x00000080){//COLOR
		largeCOLOR= findVariation(AvgCOLOR_Clustering);
		if (g3 <largeCOLOR)
			g3=largeCOLOR;
	}
	if (SIG_FORMAT & 0x00000100){//KEY
		largeKEY= findVariation(AvgKEY_Clustering);
		if (g3 <largeKEY)
			g3=largeKEY;

	}
   }
   if(L==0 || L==1)
	if (SIG_FORMAT & 0x00000001){ //Call Path - XOR
		largeCP= findVariation(Call_Path_Clustering);
	}
	
	//cout << "TTTg1: " << g1 << " g2:" << g2 << " g3:" << g3 << " g4:" << largeCP<< endl;		
	int tmp_w1=0, tmp_w2=0, tmp_w3=0, tmp_w4=0; 
	if (g1!=0)
		tmp_w1 = ((int) ceil(log2(g1)));
	if (g2!=0)
		tmp_w2 = ((int) ceil(log2(g2)));
	if (g3!=0)
		tmp_w3 = ((int) ceil(log2(g3)));
	if (largeCP!=0)
		tmp_w4 = ((int) ceil(log2(largeCP)));
	//int tmp =0;
	// largest value for time is 28bits, more than 8 years	
	// largest value for SRC/DEST/TAG/KEY/COLOR is 60bits, 10^18	
	if (tmp_w1>28)
		cerr << "(COMMPUTATION/COMMUNICATION): Did your application take more than 8 years!?" << endl;
	if (tmp_w3>60)
		cerr << " (SRC/TAG/DEST/KEY/COLOR): Do you have a value above 10^18 application take more than 8 years!?" << endl;

	//cout << "tw1: " << tmp_w1 << " tw2:" << tmp_w2 << " tw3:" << tmp_w3 << " tw4:" << tmp_w4 << endl;		
	if (tmp_w1>=tmp_w3){ // w3 < 28bits
		//tmp = w3;
		if (tmp_w3+tmp_w1 <= 60)
			w3 = tmp_w1;
		else
			w3 = tmp_w1-tmp_w3+2;
	}
	if (tmp_w3>=tmp_w4 && tmp_w4!=0) // 10^18
			w4= tmp_w3-tmp_w4+2;
	
	//cout << "w1: " << w1 << " w2:" << w2 << " w3:" << w3 << " w4:" << w4 << endl;		
	if (tmp_w2<=tmp_w1){
		w2 = tmp_w1-tmp_w2+2;
	}


}




double Cluster::calcDist(long long sig_i, long long sig_j, int weight ){
	if (weight==0){
		return (llabs((sig_i - sig_j))); // Manhattan
//		return (double) (llabs((sig_i - sig_j) * (sig_i - sig_j))); //Euclidean ;
	}
	else{
		return (llabs((sig_i - sig_j)+1)<<weight); // Manhattan
//		return (double) ((llabs((sig_i - sig_j) * (sig_i - sig_j))+1)<<weight); //Euclidean ;
	}
}

double Cluster::findDist(int i, int j){
	long long sig_i; 
	long long sig_j; 
	double dist = 0;
	//__int128_t dist=0;	

//	int myrank = getRanklist().getFirstRank();
	int w1=0, w2=0, w3=0, w4=0;
	findWeight(w1, w2, w3, w4);
   if(L==0 || L ==2){
	if (SIG_FORMAT & 0x00000002){
		findMySig(AvgCount_Clustering, sig_i, i);
		findMySig(AvgCount_Clustering, sig_j, j);
		dist += calcDist(sig_i, sig_j, 0);
	}
	if (SIG_FORMAT & 0x00000004){
		findMySig(AvgCommTime_Clustering, sig_i, i);
		findMySig(AvgCommTime_Clustering, sig_j, j);
		dist += calcDist(sig_i, sig_j, 0);
	}
	if (SIG_FORMAT & 0x00000008){
		findMySig(AvgCompTime_Clustering, sig_i, i);
		findMySig(AvgCompTime_Clustering, sig_j, j);
		dist += calcDist(sig_i, sig_j, 0);
	}
	if (SIG_FORMAT & 0x00000010){
		findMySig(AvgSRC_Clustering, sig_i, i);
		findMySig(AvgSRC_Clustering, sig_j, j);
		dist += calcDist(sig_i, sig_j, w3);
	}
	if (SIG_FORMAT & 0x00000020){
		findMySig(AvgDEST_Clustering, sig_i, i);
		findMySig(AvgDEST_Clustering, sig_j, j);
		dist += calcDist(sig_i, sig_j, w3);
	}
	if (SIG_FORMAT & 0x00000040){
		findMySig(AvgTAG_Clustering, sig_i,  i);
		findMySig(AvgTAG_Clustering, sig_j,  j);
		dist += calcDist(sig_i, sig_j, w3);
	}
	if (SIG_FORMAT & 0x00000080){
		findMySig(AvgCOLOR_Clustering, sig_i, i);
		findMySig(AvgCOLOR_Clustering, sig_j, j);
		dist += calcDist(sig_i, sig_j, w3);
	}
	if (SIG_FORMAT & 0x00000100){
		findMySig(AvgKEY_Clustering, sig_i, i);
		findMySig(AvgKEY_Clustering, sig_j, j);
		dist += calcDist(sig_i, sig_j, w3);
	}
	if (SIG_FORMAT & 0x00000200){
		findMySig(DATA_OP_Clustering, sig_i, i);
		findMySig(DATA_OP_Clustering, sig_j, j);
		dist += calcDist(sig_i, sig_j, 0);
	
	}
	if (SIG_FORMAT & 0x00000400){
		findMySig(AvgLoopIter_Clustering, sig_i, i);
		findMySig(AvgLoopIter_Clustering, sig_j, j);
		dist += calcDist(sig_i, sig_j, w2);
	
	}
     }
   if(L==0 || L==1){
	if (SIG_FORMAT & 0x00000001){
		//Only for Call Path - XOR
		findMySig(Call_Path_Clustering, sig_i, i);
		findMySig(Call_Path_Clustering, sig_j, j);
		dist +=llabs(sig_i ^ sig_j) << w4; 		
	}

   }
   //double x = sqrt(fabs(dist)); //Euclidean
   double x = fabs(dist); // Manhattan
   return x;
}

