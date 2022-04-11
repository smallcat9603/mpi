/**
 * Author: Xing Wu
 */

#ifndef __VALUESET_H__
#define __VALUESET_H__

#include <string>
#include <mpi.h>
#include <vector>
#include "Ranklist.h"
using namespace std;

class Histogram;
class ParamValue;

class ValueSet {
	public:
		ValueSet(){};
		virtual ~ValueSet(){}

		virtual string toString(){return NULL; };
		virtual string valuesToString(){return NULL; };
		virtual bool ranklistEquals(ValueSet* vs){
			return true;
		}

		virtual bool merge(ParamValue * pv){ return false; }
		virtual bool merge(Histogram * h){ return false; }

		virtual int getPackedSize(){return 0; }
		virtual void pack(void *buf, int bufsize, int *position, MPI_Comm comm){}
		virtual void unpack(void *buf, int bufsize, int *position, MPI_Comm comm){}

		virtual void setRanklist(Ranklist& rl){};
		virtual vector<int> *getValues(){return NULL;}
		virtual void setValues(vector<int>& _values){} 
		virtual Ranklist  *getRanklist(){return NULL;}
		virtual int getOffsetValue(int rank, int numranks){return 0;}
};

#endif
