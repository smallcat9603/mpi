/**
 * Author: Xing Wu
 */

#include "Param.h"
#include "Event.h"
#include <typeinfo>
#include <vector>
#include <iostream>
#include <algorithm>

extern vector<int> ranksort(int n);

Param::Param(int param_type):
param_type(param_type)
{}

Param::Param(){}

Param::~Param(){
	vector<ValueSet *>::iterator it;
	for(it = param_values.begin(); it != param_values.end(); it++){
		if(*it != NULL)
			delete *it;
	}
}

Param* Param::duplicate(){
	if(param_values.size() == 1 && typeid(*(param_values[0])) == typeid(Histogram) ){
		cerr << "error: Param::duplicateParam() should not be called against a histogram" << endl;
		cerr << toString() << endl;
		exit(0);
	} else {
		Param *p = new Param(param_type);
		for(size_t i=0; i<param_values.size(); i++)
			p->getParamValues()->push_back(dynamic_cast<ParamValue *>(param_values[i])->duplicate());
		return p;
	}
	return NULL;
}


bool Param::initWithScalar(int value, int rank){
	if(param_values.size() > 0)
		return false; /* has already been initialized */

	ParamValue *scalar_value = new ParamValue(value, rank);
	param_values.push_back(scalar_value);

	return true;
}

bool Param::initWithChar(char *value,int rank){
	if(param_values.size()>0)
		return false;
	
	ParamValue *char_value = new ParamValue(value,rank);
	param_values.push_back(char_value);

	return true;
} //added by qing

bool Param::initWithVector(int count, int *values, int rank){
	if(param_values.size() > 0)
		return false; /* has already been initialized */

	ParamValue *vector_value = new ParamValue(count, values, rank);
	param_values.push_back(vector_value);

	return true;
}

int* Param::getParamValueAt(int i, int* count){
	if( i<0 || i>= (int)param_values.size() ) { 
		if(count)
			*count = 0;
		return NULL;
	} else if (typeid(*(param_values[i])) == typeid(Histogram) ) {
		if(count)
			*count = 0;
		return NULL;
	} else {
		ParamValue * pv = dynamic_cast<ParamValue *>(param_values[i]);
		assert(pv);
		vector<int> *values = pv->getValues();
		if(!values || values->size() == 0){
			if(count)
				*count = 0;
			return NULL;
		} else {
			if(count)
				*count = values->size();
			int* rtn = (int*)malloc(values->size() * sizeof(int));
			for(unsigned k=0; k<values->size(); k++)
				rtn[k] = values->at(k);
			return rtn;
		}
	}
}

int Param::getScalarValueForRank(int rank){
	if(param_values.size() == 1 && typeid(*(param_values[0])) == typeid(Histogram) ){
		cerr << "error: Param::getScalarValueForRank() should not be called against a histogram" << endl;
		cerr << toString() << endl;
		exit(0);
	} else {
		for(size_t i=0; i<param_values.size(); i++){
			if(dynamic_cast<ParamValue *>(param_values[i])->hasMember(rank))
				return dynamic_cast<ParamValue *>(param_values[i])->getScalarValue();
		}
	}
	return -1;
}


int Param::getValueForRank(int rank, RandMnger *randmnger){
	if(param_values.size() == 1 && typeid(*(param_values[0])) == typeid(Histogram) ){
		if(!randmnger) {
			return (int)dynamic_cast<Histogram *>(param_values[0])->avg();
		} else {
			double rtn = randmnger->getRandomValue(dynamic_cast<Histogram *>(param_values[0]));
			return (int)round(rtn);
		}
	} else {
		for(size_t i=0; i<param_values.size(); i++){
			if(dynamic_cast<ParamValue *>(param_values[i])->hasMember(rank)){
				return dynamic_cast<ParamValue *>(param_values[i])->getCurrentValue(true, rank);
			}
		}
	}
	return 0;
}

string Param::getCharValueForRank(int num, int rank, RandMnger *randmnger){
	if(param_values.size() == 1){
		return dynamic_cast<ParamValue *>(param_values[0])->getParamCharValue(num, rank);
	} else {
		for(size_t i=0; i<param_values.size(); i++){
			if(dynamic_cast<ParamValue *>(param_values[i])->hasMember(rank)){
				return dynamic_cast<ParamValue *>(param_values[i])->getParamCharValue(0, rank);
			}
		}
	}
	return NULL;	
}

int* Param::getValuesForRank(int count, int rank, RandMnger* randmnger){
	if(param_values.size() == 1 && typeid(*(param_values[0])) == typeid(Histogram) ){
		cerr << "error: Param::getValuesForRank() should not be called against a histogram" << endl;
		cerr << toString() << endl;
		exit(0);
	} else {
		for(size_t i=0; i<param_values.size(); i++){
			if(dynamic_cast<ParamValue *>(param_values[i])->hasMember(rank))
				return dynamic_cast<ParamValue *>(param_values[i])->getCurrentValues(count, true, rank);
		}
	}
	return NULL;
}

/**
 * For efficiency reasons, 
 * this function assumes there is only 1 element
 * int param_values and it is of the type ParamValue.
 * Besides, it returns the "current value" without
 * check if the rank of the calling MPI process is
 * in the ranklist of the ParamValue
 */
int Param::getCurrentValue(int rank){
	assert(param_values.size() == 1 && typeid(*(param_values[0])) == typeid(ParamValue));
	return dynamic_cast<ParamValue *>(param_values[0])->getCurrentValue(true, rank);
}
int Param::getOffsetValue(int rank, int numranks){
	if(param_values.size() == 1 && typeid(*(param_values[0])) == typeid(ParamValue))
		return dynamic_cast<ParamValue *>(param_values[0])->getOffsetValue(rank, numranks);
	else
		return -1;
}
/**
 * For efficiency reasons, 
 * this function assumes there is only 1 element
 * int param_values and it is of the type ParamValue.
 * Besides, it returns the "current values" without
 * check if the rank of the calling MPI process is
 * in the ranklist of the ParamValue
 */
int* Param::getCurrentValues(int count, int rank){
	assert(param_values.size() == 1 && typeid(*(param_values[0])) == typeid(ParamValue));
	return dynamic_cast<ParamValue *>(param_values[0])->getCurrentValues(count, true,  rank);
}

vector<int>* Param::getValues(int rank){
	if(param_values.size() == 1 && typeid(*(param_values[0])) == typeid(Histogram) ){
		cerr << "error: Param::getValues() should not be called against a histogram" << endl;
		cerr << toString() << endl;
		exit(0);
	} else {
		for(size_t i=0; i<param_values.size(); i++){
			if(dynamic_cast<ParamValue *>(param_values[i])->hasMember(rank))
				return dynamic_cast<ParamValue *>(param_values[i])->getValues();
		}
	}
	return NULL;
}

int Param::getNumValues(bool expand, int rank){
	if(param_values.size() == 1 && typeid(*(param_values[0])) == typeid(Histogram) ){
		cerr << "error: Param::getNumValues() should not be called against a histogram" << endl;
		cerr << toString() << endl;
		exit(0);
	} else {
		for(size_t i=0; i<param_values.size(); i++){
			if(dynamic_cast<ParamValue *>(param_values[i])->hasMember(rank))
				return dynamic_cast<ParamValue *>(param_values[i])->getNumValues(expand);
		}
	}
	return -1;
}

void Param::expandLoop(int factor, int rank){
	if(param_values.size() == 1 && typeid(*(param_values[0])) == typeid(Histogram) ){
		cerr << "error: Param::expandLoop() should not be called against a histogram" << endl;
		cerr << toString() << endl;
		exit(0);
	} else {
		for(size_t i=0; i<param_values.size(); i++){
			if(dynamic_cast<ParamValue *>(param_values[i])->hasMember(rank))
				dynamic_cast<ParamValue *>(param_values[i])->expand(factor);
		}
	}
}

void Param::loopSetOne(int rank){
	if(param_values.size() == 1 && typeid(*(param_values[0])) == typeid(Histogram) ){
		cerr << "error: Param::loopSetOne() should not be called against a histogram" << endl;
		cerr << toString() << endl;
		exit(0);
	} else {
		for(size_t i=0; i<param_values.size(); i++){
			if(dynamic_cast<ParamValue *>(param_values[i])->hasMember(rank))
				dynamic_cast<ParamValue *>(param_values[i])->setOne();
		}
	}
}

bool Param::triggerHistogramize(){
	if(!histogramizable())
		return false;

	if(param_values.size() == 1 && typeid(*(param_values[0])) == typeid(Histogram) )
		return false;

	int total = 0;
	for(unsigned i=0; i<param_values.size(); i++){
		assert(typeid(*(param_values[i])) == typeid(ParamValue));
		total += dynamic_cast<ParamValue *>(param_values[i])->getNumValues();
	}
	if(total >= HISTO_THRESHOLD)
		return true;
	else 
		return false;
}

void Param::histogramize(){
	Histogram *h = new Histogram();
	ParamValue *pv;

	for(unsigned i=0; i<param_values.size(); i++){
		assert(typeid(*(param_values[i])) == typeid(ParamValue));
		pv = dynamic_cast<ParamValue *>(param_values[i]);
		vector<int>* values = pv->getValues();
		/*int numRanks = pv->getRanklist()->getNumRanks();
		int iter = pv->getIters();
		iter *= numRanks;*/
		int iter = pv->getCnt();
		int rank = pv->getRanklist()->getFirstRank();

		for(unsigned j=0; j<values->size(); j++)
			for(int k=0; k<iter; k++)
				h->add((double)(values->at(j)), rank);
	}

	for(unsigned i=0; i<param_values.size(); i++)
		delete param_values[i];
	param_values.clear();
	param_values.push_back(h);

}

void Param::resetTraversalIterators(){
	for(unsigned i=0; i<param_values.size(); i++)
		if(typeid(*(param_values[i])) == typeid(ParamValue))
			dynamic_cast<ParamValue *>(param_values[i])->resetTraversalIterators();
}

void Param::merge(Param *other){
	if(param_type != other->getParamType())
		return;

	vector<ValueSet *> *values = other->getParamValues();

	if(param_values.size() == 1 && values->size() == 1 
					&& param_values[0]->ranklistEquals(values->at(0))){
		/* loop compression */
		if(typeid(*(param_values[0])) == typeid(ParamValue) 
						&& typeid(*(values->at(0))) == typeid(ParamValue) ){
			/* rlparam into rlparam*/
			param_values[0]->merge(dynamic_cast<ParamValue *>(values->at(0)) );
			if(triggerHistogramize()){
				histogramize();
			}
		} else if (typeid(*(param_values[0])) == typeid(ParamValue) 
						&& typeid(*(values->at(0))) == typeid(Histogram) ){
			/* histogram into rlparam*/
			/* 1. histogramize the rlparam */
			assert(histogramizable() );
			histogramize();
			/* 2. histogram into histogram */
			param_values[0]->merge(dynamic_cast<Histogram *>(values->at(0)) );
		} else if (typeid(*(param_values[0])) == typeid(Histogram) 
						&& typeid(*(values->at(0))) == typeid(ParamValue) ){
			/* rlparam into histogram */
			param_values[0]->merge(dynamic_cast<ParamValue *>(values->at(0)) );
		} else {
			/* histogram into histogram */
			param_values[0]->merge(dynamic_cast<Histogram *>(values->at(0)) );
		}

	} else {
		/* inter-node compression */
		if(param_values.size() == 1 && typeid(*(param_values[0])) == typeid(Histogram) ){
			/* rlparam or histogram into histogram*/
			for(unsigned i = 0; i<values->size(); i++){
				if(typeid(*(values->at(i))) == typeid(ParamValue) )
					param_values[0]->merge(dynamic_cast<ParamValue *>(values->at(i)) );
				else
					param_values[0]->merge(dynamic_cast<Histogram *>(values->at(i)) );
			}
		} else {
			if(typeid(*(values->at(0))) == typeid(Histogram)){
				/* histogram into rlparam */
				/* 1. histogramize the rlparam */
				assert(histogramizable() );
				histogramize();
				/* 2. histogram into histogram */
				param_values[0]->merge(dynamic_cast<Histogram *>(values->at(0)) );
			} else {   
				int flag;
				/* multiple rlparams into multiple rlparams */
				for(unsigned i=0; i<values->size(); i++){
					assert(typeid(*(values->at(i))) == typeid(ParamValue) );
					flag = false;
					for(unsigned j=0; j<param_values.size(); j++){
						assert(typeid(*(param_values[j])) == typeid(ParamValue) );
						if(param_values[j]->merge(dynamic_cast<ParamValue *>(values->at(i))) ){
							flag = true;
							break;
						}
					}
					if(!flag){
						param_values.push_back(dynamic_cast<ParamValue *>(values->at(i)) );
						values->at(i) = NULL;
					}
				}
				if(triggerHistogramize()){
					histogramize();
				}
			}
		}
	}
}


void Param::incValues(int inc){
	//assert(param_values.size() == 1); // this function is called only during intra-node compression
	assert(typeid(*(param_values[0])) == typeid(ParamValue)); // see comments in Loop::incLoopIter() 
	dynamic_cast<ParamValue *>(param_values[0])->increase(inc);
}


void Param::input(string& buf, int mode, int rank, Ranklist *rl){
	if(rl && !rl->hasMember(rank) && mode == INPUT_NODE)
		return;

	if(buf.find("{") != string::npos){
		/* input a histogram */
		Histogram *h = new Histogram();
		if(!h)
			exit(1);
		h->input(buf);
		param_values.push_back(h);
	} else {
		/* input a rlparam */
		ParamValue *pv = new ParamValue();
		if(!pv)
			exit(1);
		pv->input(buf, rl);
		if( mode == INPUT_GLOBAL || pv->getRanklist()->hasMember(rank) ){
			param_values.push_back(pv);
		} else {
			delete pv;
		}
	}
}

int Param::getValueSig(){
	
	ParamValue *pv;

	pv = dynamic_cast<ParamValue *>(param_values[0]);
	vector<int>* values = pv->getValues();

	int temp=0;

	for(unsigned j=0; j<values->size(); j++)
	{
			temp+=values->at(j);
	}

	return temp;
}

int Param::getDistinctValueSig(){
	
	ParamValue *pv;

	pv = dynamic_cast<ParamValue *>(param_values[0]);
	vector<int>* values = pv->getValues();

	set<int> tempSet;
	int sum=0;

	for(unsigned j=0; j<values->size(); j++)
	{
			tempSet.insert(values->at(j));
	}


	for (std::set<int>::iterator it = tempSet.begin(); it != tempSet.end(); ++it) {
	    sum += *it;
	}

	return sum;
}

string Param::toString(){
	ostringstream rtn;
	int flag = 0, numranks = 0;
	vector<int> valuerecord;
	Ranklist R;
		
	if(param_values.size() == 1){
		rtn << hex << param_type << " ";
		rtn << param_values[0]->valuesToString() << "\n";
	} else {
		if(param_type == OFFSET)
		{
			for(unsigned j=0; j<param_values.size(); j++){
				vector<int> *pj = param_values[j]->getValues();
				if((*pj).size()!= 1){
					flag = 1;
				}
				numranks += param_values[j]->getRanklist()->getNumRanks();
			}
			if(flag == 0 && (unsigned)numranks == param_values.size()){
				vector<int> *p0 = param_values[0]->getValues();
				Ranklist *Rp0 = param_values[0]->getRanklist();
				for(unsigned j=1; j<param_values.size(); j++)
				{
					vector<int> *pj = param_values[j]->getValues();
					(*p0).insert((*p0).end(), (*pj).begin(), (*pj).end());
				
					Ranklist *Rpj = param_values[j]->getRanklist();
					Rp0->merge(Rpj);
				}
				vector <int> rank_sort = ranksort(param_values.size());
				int divi_flag = 0;
				int divi = 0;
				vector<int>::iterator it = find(rank_sort.begin(), rank_sort.end(), 0);
				int offset0 = (*p0).at(it-rank_sort.begin());
				int offset_start = offset0;
				for(unsigned j=1; j<param_values.size(); j++){
					it = find(rank_sort.begin(), rank_sort.end(), (int)j);
					int offset1 = (*p0).at(it-rank_sort.begin());
					if(j == 1)
						divi = offset1 - offset0;
					if(offset1 - offset0 != divi){
						divi_flag = 1;
						break;
					}
					offset0 = offset1;
				}
				if(divi_flag == 0){
					(*p0).erase((*p0).begin(),(*p0).end());
					(*p0).push_back(-1);
					(*p0).push_back(offset_start);
					(*p0).push_back(divi);
					(*p0).push_back(numranks);
				}else{
					(*p0).insert((*p0).begin(), -2);	
				}
				rtn << hex << param_type <<" ";
				rtn << param_values[0] -> valuesToString() <<"\n";
			}
			else{
				for(unsigned j=0; j<param_values.size(); j++){
					rtn << hex << param_type <<" ";
					rtn << param_values[j] -> toString() <<"\n";
				}
			}
			
		}
		else if (param_type == TIME){  /*calculate the average time*/
			int size = param_values[0]->getValues()->size();
			Ranklist *Rp0 = param_values[0]->getRanklist();
			vector<int> *p0 = param_values[0]->getValues();
			for(unsigned jj=0; jj<size; jj++){
				long long V = 0;
				numranks = 0;
				for(unsigned j=0; j<param_values.size(); j++){
					 V += param_values[j]->getValues()->at(jj);
					 numranks += param_values[j]->getRanklist()->getNumRanks();
					 if(jj==0 && j!=0){
						Ranklist *Rpj = param_values[j]->getRanklist();
						Rp0->merge(Rpj);
					 }
				}
				(*p0).at(jj) = int (V/numranks);
			}
			rtn << hex<< param_type << " ";
			rtn << param_values[0]->valuesToString()<<"\n";
		}
		else
		{
			for(unsigned i=0; i<param_values.size(); i++){
				rtn << hex << param_type << " ";
				rtn << param_values[i]->toString() << "\n";
			}
		}
	}

	return rtn.str();

}

string Param::loopParamToString(){
	ostringstream rtn;
	for(unsigned i=0; i<param_values.size(); i++){
		rtn << param_values[i]->toString();
	}

	return rtn.str();

}

int Param::getPackedSize(){
	int size = 0;
	size += sizeof(int);
	size += sizeof(int); /* param_values.size() */
	for(unsigned i = 0; i<param_values.size(); i++){
		size += param_values[i]->getPackedSize();
	}
	return size;
}


void Param::pack(void *buf, int bufsize, int *position, MPI_Comm comm){
	PMPI_Pack(&param_type, 1, MPI_INT, buf, bufsize, position, comm);
	int size = param_values.size();
	PMPI_Pack(&size, 1, MPI_INT, buf, bufsize, position, comm);
	for(unsigned i = 0; i<param_values.size(); i++)
		param_values[i]->pack(buf, bufsize, position, comm);
}


void Param::unpack(void *buf, int bufsize, int *position, MPI_Comm comm){
	PMPI_Unpack(buf, bufsize, position, &param_type, 1, MPI_INT, comm);
	int size, type;
	PMPI_Unpack(buf, bufsize, position, &size, 1, MPI_INT, comm);
	for(int i=0; i<size; i++){
		PMPI_Unpack(buf, bufsize, position, &type, 1, MPI_INT, comm);
		/* 0 -- ParamValue, 1 -- Histogram */
		ValueSet *vs;
		if( type == 0 ){
			vs = new ParamValue();
		} else {
			vs = new Histogram();
		}
		vs->unpack(buf, bufsize, position, comm);
		param_values.push_back(vs);
	}
}


/*for extrapolation*/
void Param::clearto(int size){
	if(size >= param_values.size())
		return;
	for(unsigned i=size; i<param_values.size(); i++){
		delete param_values[i];
		param_values[i] == NULL;
	}
	param_values.erase(param_values.begin()+size, param_values.end());	
}

