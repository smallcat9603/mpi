/**
 * Author: Xing Wu
 */

#ifndef __PARAM_H__
#define __PARAM_H__

#include "ParamValue.h"
#include "paramtype.h"
#include <string>
#include <sstream>
#include <cassert>
#include "Histogram.h"
#include "ValueSet.h"
#include <mpi.h>
#include <cmath>
#include "RandMnger.h"
#include <typeinfo> 
#define HISTO_THRESHOLD 1

extern int my_rank;

using namespace std;

class Param{
	private:
		int param_type;
		vector<ValueSet *> param_values;

	public:
		Param(int param_type);
		Param();
		~Param();

		Param* duplicate();

		int getParamType(){
			return param_type;
		}
		vector<ValueSet *> *getParamValues(){
			return &param_values;
		}

		bool initWithScalar(int value, int rank);
		bool initWithChar(char *value,int rank);
		bool initWithVector(int count, int* values, int rank);

		int *getParamValueAt(int i, int* count);
		int getScalarValueForRank(int rank = my_rank);
		int getValueForRank(int rank, RandMnger* randmnger = NULL);
		int* getValuesForRank(int count, int rank, RandMnger* randmnger = NULL);
		int getCurrentValue(int rank);
		int getOffsetValue(int rank, int numranks);
		int* getCurrentValues(int count, int rank);
		vector<int>* getValues(int rank = my_rank);
		string getCharValueForRank(int num, int rank, RandMnger* randmnger = NULL);
		int getNumValues(bool expand = false, int rank = my_rank);
		void expandLoop(int factor, int rank = my_rank);
		void loopSetOne(int rank = my_rank);
		void resetTraversalIterators();
		void merge(Param *other);

		void setRanklist(Ranklist& rl){
			//	cout << param_values[0]->toString() << "\n";
	        if(param_values.size() == 1){
        	        //rtn << hex << param_type << " ";
                	param_values[0]->setRanklist(rl);
		} else {
	                for(unsigned i=0; i<param_values.size(); i++){
        	                //rtn << hex << param_type << " ";
                	        param_values[i]->setRanklist(rl);
        	        	}
	        	}
		}

		string toString();
		string loopParamToString();
		void input(string& buf, int mode, int rank, Ranklist *rl = NULL);

		bool histogramizable(){
			switch(param_type){
				case COUNT:
				case SOURCE:
				case DEST:
#ifdef FEATURE_PARAM_HISTO
					return true;
#endif
				default:
					return false;
			}
		}

		bool triggerHistogramize();

		void histogramize();

		void incValues(int inc);

		int getPackedSize();
		void pack(void *buf, int bufsize, int *position, MPI_Comm comm);
		void unpack(void *buf, int bufsize, int *position, MPI_Comm comm);

		int getValueSig(int rank, int comm_size);
		int getDistinctValueSig();
		int getValueSig(); 
		/*for extrapolation*/
		void clearto(int size);
		void addvalues(ValueSet *vs){
			param_values.push_back(vs);
		}
};

#endif /*__PARAM_H__*/
