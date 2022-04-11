/**
 * Author: Xing Wu
 */

#ifndef __PARAMVALUE_H__
#define __PARAMVALUE_H__

#include <vector>
#include <string>
#include <map>
#include "Ranklist.h"
#include <sstream>
#include <cassert>
#include "ValueSet.h"
#include "Histogram.h"
#include <mpi.h>
#include <typeinfo> 
using namespace std;

extern int my_rank, my_size;

class ParamValue : public ValueSet{
	private:
		vector<int> values;
		vector <string> valuechar;
		Ranklist ranklist;
		int iters;
		int cnt; /* for each single value in the vector of values, how many times
						it has been used. It equals to the sum of the usage counts of
						all the ranks in the associated ranklist. This member will be
						used for 'histogramization' */
		map<int, size_t> traverse_iterators; /* only used for traversal (replay) */

		int compressValues(int *arr, int size);


	public:
		ParamValue();
		ParamValue(int value, int rank = 0);
		ParamValue(char* value, int rank = 0);
		ParamValue(int count, int* vector, int rank);
		~ParamValue();

		ParamValue* duplicate();

		vector<int>* getValues(){
			return &values;
		}
		vector<string>* getCharValues(){
			return &valuechar;
		}
		void setValues(vector<int>& _values){
			values = _values;
		}

		void setCharValues(string &str){
			valuechar.clear();
			valuechar.push_back(str);	
		}

		Ranklist* getRanklist(){
			return &ranklist;
		}
		int getIters(){
			return iters;
		}
		void setIters(int _iters){
			iters = _iters;
		}
		int getCnt(){
			return cnt;
		}
		void setCnt(int _cnt){
			cnt = _cnt;
		}
		int getNumValues(bool expand = false){
			if(expand)
				return values.size() * iters;
			else
				return values.size();
		}
		bool hasMember(int rank){
			return ranklist.hasMember(rank);
		}
		void expand(int factor){
			iters *= factor;
			cnt *= factor;
		}
		void setOne(){
			for(vector<int>::iterator it = values.begin(); it != values.end(); it++)
				(*it) = 1;
		}

		void resetTraversalIterators();
		int getScalarValue();
		int getCurrentValue(bool inc = false, int rank = -1);
		int getOffsetValue(int rank, int numranks);
		int* getCurrentValues(int count, bool inc = false, int rank = -1);
		bool ranklistEquals(ValueSet* vs);

		bool merge(ParamValue* pv);
		bool merge(Histogram* h);

		string toString();
		string valuesToString();
		void input(string& buf, Ranklist* rl = NULL);

		void increase(int inc);

		int getPackedSize();
		void pack(void *buf, int bufsize, int *position, MPI_Comm comm);
		void unpack(void *buf, int bufsize, int *position, MPI_Comm comm);
		string getParamCharValue(int num, int rank = -1);
		 void setRanklist(Ranklist& rl){
                	ranklist = rl;
	        }

};
vector <string> charextract(string filename); //added by xiaoqing
bool charnamematch(vector <string> record0, vector <string> record1); //added by xiaoqing
string cacu_num(string filename,string num,int done_flag); //added by xiaoqing
void stringtoformat(vector <string> &record); //added by xiaoqing
void filenamemerge(vector<string> &record0,vector<string> &record1);//added by xiaoqing
void opstrmerge(string &str0, string &str1); //added by xiaoqing
vector <string> opgetsubstring(string str); //added by xiaoqing
void maincharmerge(vector <string> &vect0, vector <string> *vect1); //added by xiaoqing
vector<int> ranksort(int n); //added by xiaoqing
string getCharForRank(string str, int rank, vector<int> rank_sort, const vector<int> *ranklist); //added by xiaoqing
#endif /*__PARAMVALUE_H__*/
