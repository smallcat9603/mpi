
/**
 * Author: Xing Wu
 */

#include "ParamValue.h"
#include <string>
#include "paramschar.C"
#include <typeinfo>
#include <limits.h>
#include <string.h>


ParamValue::ParamValue()
{}

ParamValue::ParamValue(int value, int rank):
ranklist(rank),
iters(1)
{
	values.push_back(value);
	cnt = iters * ranklist.getNumRanks();
	if(cnt < 0) {
		cerr << "warning: integer overflow" << endl;
		cnt = INT_MAX; 
	}
}

ParamValue::ParamValue(char * value,int rank):
ranklist(rank),
iters(1)
{
	string tem = string(value);
	valuechar.push_back(tem);
	cnt = iters * ranklist.getNumRanks();
	if(cnt < 0){
		cerr << "warning:integer overflow" <<endl;
		cnt = INT_MAX;
	}
} //added by qing


ParamValue::ParamValue(int count, int* vector, int rank):
ranklist(rank),
iters(1)
{
	for(int i=0; i<count; i++)
		values.push_back(vector[i]);
	cnt = iters * ranklist.getNumRanks();
	if(cnt < 0) {
		cerr << "warning: integer overflow" << endl;
		cnt = INT_MAX; 
	}
}

ParamValue::~ParamValue(){};

ParamValue* ParamValue::duplicate(){
	ParamValue *pv = new ParamValue();
	pv->getRanklist()->setRanks(*(ranklist.getRanks()) );
	pv->getRanklist()->setNumRanks(ranklist.getNumRanks());
	pv->setValues(values);
	pv->setIters(iters);
	pv->setCnt(cnt);
	return pv;
}


int ParamValue::compressValues(int *arr, int size){
	int *subarray, len, index;
	bool flag;
	// iterate through all the possible lengths
	for(len=1; len<size/2+1; len++){
		if( size % len == 0 ){
			flag = true;
			// compare the subarrays
			for(subarray=arr+len, index=len; index<size; index+=len, subarray+=len){
				if(memcmp(arr, subarray, len*sizeof(int)) != 0){
					flag = false;
					break;
				}
			}
			if(flag)
				return len;
		}
	}
	return size;
}

void ParamValue::resetTraversalIterators(){
	traverse_iterators.clear();
}

int ParamValue::getScalarValue(){
	if(values.size() != 1){
		cerr << "error: cannot get scalar value from vector" << endl;
		cerr << toString() << endl;
		exit(1);
	}
	return values[0];
}

int ParamValue::getCurrentValue(bool inc, int rank){
	if(rank == -1)
		rank = my_rank;

	map<int, size_t>::iterator it = traverse_iterators.find(rank);
	if(it == traverse_iterators.end()){
		traverse_iterators[rank] = 0;
		it = traverse_iterators.find(rank);
	}
	size_t traverse_iterator = it->second;

	if(values.size() <= traverse_iterator){
		cerr << "cannot find the next value in vector, values.size() = " << values.size() << ", traverse_iterator = " << traverse_iterator << endl;
		exit(1);
	} 
	int rtn = values[traverse_iterator];
	if(inc){
		traverse_iterator = (traverse_iterator + 1) % values.size();
		traverse_iterators[rank] = traverse_iterator;
	}
	return rtn;
}


string ParamValue::getParamCharValue(int num, int rank){

	if(rank == -1)
		rank = my_rank;
	vector<int> sort;
	vector <string> *name = getCharValues();
	string temp = (*name).at(0);

	if(temp.find('(')!=string::npos && temp.find("-1")==string::npos){
		if(num == 0){
			const vector <int> *p = getRanklist()->getRanks();
			int Rstart = (*p).at(2);
			int Rnum = (*p).at(3);
			int Rstrid = (*p).at(4);
			sort = ranksort((Rstart+(Rnum-1)*Rstrid+1));
			for(unsigned i=0; i<sort.size(); i++){
				if(sort.at(i) < Rstart)
					sort.erase(sort.begin()+i);
			}
		}else{
			sort = ranksort(num);
		}
		
	}

	//traverse for loops
	vector<string> re_cord;
	while(temp.length() > 0){
		int pos = temp.find("  ");
		re_cord.push_back(temp.substr(0, pos+2));
		temp.erase(0, pos+2);
	}
	map<int, size_t>::iterator it = traverse_iterators.find(rank);
	if(it == traverse_iterators.end()){
		traverse_iterators[rank] = 0;
		it = traverse_iterators.find(rank);
	}
	size_t traverse_iterator = it->second;

	if(re_cord.size() <= traverse_iterator){
		cerr << "cannot find the next value in vector, values.size() = " << re_cord.size() << ", traverse_iterator = " << traverse_iterator << endl;
		exit(1);
	}
	string input = re_cord[traverse_iterator];
	traverse_iterator = (traverse_iterator + 1) % re_cord.size();
	traverse_iterators[rank] = traverse_iterator;


	string result =  getCharForRank(input, rank, sort, ranklist.getRanks());

	return result;
	
}
int ParamValue::getOffsetValue(int rank, int numranks){
	vector<int> rank_sort;
	int rtn;
	if(rank == -1)
		rank = my_rank;
	if(values.at(0) == -1){
		if(rank <= values.at(3))
			return values.at(1)+rank*values.at(2);
		else{
			cerr <<"no corresponded offset"<<endl;
			exit(1);
		}
	}
	if(values.size() == 1)
		rtn = values[0];
	else{
		rank_sort = ranksort(numranks);
		vector<int> ::iterator it;
		it = find(rank_sort.begin(), rank_sort.end(), rank);
		int position = it - rank_sort.begin() + 1;
		rtn = values[position];
	}
	return rtn;
}

int* ParamValue::getCurrentValues(int count, bool inc, int rank){
	if(rank == -1)
		rank = my_rank;

	map<int, size_t>::iterator it = traverse_iterators.find(rank);
	if(it == traverse_iterators.end()){
		traverse_iterators[rank] = 0;
		it = traverse_iterators.find(rank);
	}
	size_t traverse_iterator = it->second;

	if(values.size() <= traverse_iterator){
		cerr << "cannot find the next value in vector, values.size() = " << values.size() << ", traverse_iterator = " << traverse_iterator << endl;
		exit(1);
	} 
	int *rtns = (int *)malloc(count * sizeof(int));
	if(rtns == NULL)
		return NULL;

	int size = values.size();
	for(int i=0; i<count; i++)
		rtns[i] = values[(traverse_iterator + i) % size];
	if(inc){
		traverse_iterator = (traverse_iterator + count) % size;
		traverse_iterators[rank] = traverse_iterator;
	}
	return rtns;

}

bool ParamValue::ranklistEquals(ValueSet* vs){
	if(typeid(*vs) == typeid(ParamValue))
		return ranklist.equals(dynamic_cast<ParamValue *>(vs)->getRanklist() );
	else if (typeid(*vs) == typeid(Histogram))
		return true;
	else
		return false;
}


bool ParamValue::merge(Histogram* h){
	return false;
}

bool ParamValue::merge(ParamValue* pv){
	vector<string>   *vc = pv->getCharValues();
	if(ranklistEquals( pv)){ 
		/* matching ranklist, different value list, this is a 
		 * loop compression, compress the value list */
		vector<int>	 *v = pv->getValues();
		if(values == *v){
			iters += pv->getIters();
			if(valuechar != *vc && (*vc).size()>0)
				maincharmerge(valuechar, vc, 1); //added by xiaoqing
		} else {
			int size = iters * values.size() + pv->getIters() * v->size();
			int *expanded = (int *)malloc( size * sizeof(int));
			int iter = 0;
			for(int i=0; i<iters; i++){
				for(unsigned int j=0; j<values.size(); j++){
					expanded[iter++] = values[j];
				}
			}
			for(int i=0; i<pv->getIters(); i++){
				for(unsigned int j=0; j<v->size(); j++){
					expanded[iter++] = v->at(j);
				}
			}
			int len = compressValues(expanded, size);
			assert(size % len == 0);
			iters = size / len;
			values.clear();
			values.insert(values.begin(), expanded, expanded+len);
			free(expanded);
			if((*vc).size()>0){
				cout <<"intra node"<<"\n";
				maincharmerge(valuechar,vc, 1); //added by xiaoqing
			}
		}
		cnt = iters * ranklist.getNumRanks();
		if(cnt < 0) {
			cerr << "warning: integer overflow" << endl;
			cnt = INT_MAX; 
		}
		return true;
	} else if ((values.size()>0 && values == *(pv->getValues())) || (valuechar.size()>0 && valuechar == *(pv->getCharValues()))) { 
		/*vector<string> *vc = pv->getCharValues();
		if(valuechar != *vc && (*vc).size()>0)
			maincharmerge(valuechar,vc); //added by xiaoqing*/
		/* matching value list, different ranklist, this is a 
		 * inter-node compression, compress the ranklists */
		cnt += pv->getCnt();
		if(cnt < 0) {
			cerr << "warning: integer overflow" << endl;
			cnt = INT_MAX; 
		}
		ranklist.merge(pv->getRanklist());
		return true;
	} else if (valuechar.size()>0){
		if(maincharmerge(valuechar, vc, 0)){
			ranklist.merge(pv->getRanklist());
			return true;
		}
		else
			return false;
			
	} else {
		/* neither the ranklist, nor the value list, is matching, cannot compress*/
		return false;
	}
}

string ParamValue::toString(){
	ostringstream rtn;
	if(valuechar.size()>0){
		rtn << ":";
		for(unsigned j=0;j<valuechar.size();j++){
			div_format(valuechar.at(j), &ranklist);
			rtn << valuechar.at(j) << "  ";
		}
		rtn << "["<<ranklist.toString()<<"]";
	}
	else{
		rtn << "( "  ;
		for(unsigned i = 0; i < values.size(); i++)
			rtn << values[i] << " ";
		rtn << ")[ " << ranklist.toString() << " ]";
	}

	
	return rtn.str();
}

string ParamValue::valuesToString(){
	ostringstream rtn;
	
	if(valuechar.size()>0){
		rtn << ":";
		for(unsigned j=0;j<valuechar.size();j++){
			div_format(valuechar.at(j), &ranklist);
			rtn << valuechar.at(j) << "  ";
		}
	} //added by xiaoqing
	else{
		for(unsigned i = 0; i < values.size(); i++){
			rtn << values[i] << " ";
		}
	}
	return rtn.str();
}


void ParamValue::input(string& buf,  Ranklist *rl){
	stringstream ss(stringstream::in | stringstream::out);
	int v;
	string temp;
	if(*(buf.begin()) == ':') /*parameter is a string*/
	{
		if(buf.find("[") == string::npos){
			assert(rl);
			temp = buf.substr(1,buf.length()-1);
			valuechar.push_back(temp);
			ranklist = *rl;
		}
		else{
			temp = buf.substr(1, buf.find("[")-1);
			valuechar.push_back(temp);
			string rl_str = buf.substr(buf.find("[")+1);
			rl_str = rl_str.substr(0, rl_str.find("]") );
			ranklist.input(rl_str);
		}
	}
	else{
		if(buf.find("[") == string::npos){
			/* a ranklist must be provided */
			assert(rl);
			ss << buf;
			while( ss >> v )
				values.push_back(v);
			ranklist = *rl;
		} else {
			string vals = buf.substr(0, buf.find(")"));
			vals = vals.substr(vals.find("(")+1);
			ss << vals;
			while (ss >> v)
				values.push_back(v);
			string rl_str = buf.substr(buf.find("[")+1);
			rl_str = rl_str.substr(0, rl_str.find("]") );
			ranklist.input(rl_str);
		}
	}
}


void ParamValue::increase(int inc){
	assert(values.size() == 1); // see comments in Loop::incLoopIter() 
	values[0] += 1;
}

int ParamValue::getPackedSize(){
	int size = 0;
	size += sizeof(int); /* type of the current ValueSet */
	size += sizeof(int); /* values.size() */
	size += sizeof(int) * values.size();
	size += ranklist.getPackedSize(); 
	size += sizeof(int); /* iters */
	size += sizeof(int); /* cnt */

	size += sizeof(int);/*valuechar.size()*/
	if(valuechar.size()>0)
	{
		int i;
		for(i=0; i<valuechar.size(); i++)
		{
			size += sizeof(int);
			size += valuechar.at(i).length() * sizeof(char);
		}		
	} //added by xiaoqing

	return size;
}


void ParamValue::pack(void *buf, int bufsize, int *position, MPI_Comm comm){
	int type = 0; /* 0 -- ParamValue, 1 -- Histogram */
	PMPI_Pack(&type, 1, MPI_INT, buf, bufsize, position, comm);
	int size = values.size();
	PMPI_Pack(&size, 1, MPI_INT, buf, bufsize, position, comm);
	for(int i=0; i<size; i++)
		PMPI_Pack(&(values[i]), 1, MPI_INT, buf, bufsize, position, comm);
	ranklist.pack(buf, bufsize, position, comm);
	PMPI_Pack(&iters, 1, MPI_INT, buf, bufsize, position, comm);
	PMPI_Pack(&cnt, 1, MPI_INT, buf, bufsize, position, comm);


	int i,length = valuechar.size();
	PMPI_Pack(&length, 1, MPI_INT, buf, bufsize, position, comm);
	if(length > 0)
	{
		char *ch[length];
		for(i=0; i<length; i++)
		{
			int len = valuechar.at(i).length();
			PMPI_Pack(&len, 1, MPI_INT, buf, bufsize, position, comm); /*length of each  string*/
			ch[i] = new char[len]; 
			valuechar.at(i).copy(ch[i],len,0);
			PMPI_Pack(ch[i], len, MPI_CHAR, buf, bufsize, position, comm);
		}
		for(i=0; i<length; i++)
			delete ch[i];
	}//added by xiaoqing

}


void ParamValue::unpack(void *buf, int bufsize, int *position, MPI_Comm comm){
	int size, value;
	PMPI_Unpack(buf, bufsize, position, &size, 1, MPI_INT, comm);
	for(int i=0; i<size; i++){
		PMPI_Unpack(buf, bufsize, position, &value, 1, MPI_INT, comm);
		values.push_back(value);
	}
	ranklist.unpack(buf, bufsize, position, comm);
	PMPI_Unpack(buf, bufsize, position, &iters, 1, MPI_INT, comm);
	PMPI_Unpack(buf, bufsize, position, &cnt, 1, MPI_INT, comm);

	//added by xiaoqing, unpack of string
	int length;
	PMPI_Unpack(buf, bufsize, position, &length, 1, MPI_INT, comm);
	if(length > 0)
	{
		string tem[length];
		char *ch = new char[1];
		for(int j=0; j<length; j++)
		{
			int len;
			PMPI_Unpack(buf, bufsize, position, &len, 1, MPI_INT, comm);
			for(int k=0; k<len; k++)
			{
				PMPI_Unpack(buf, bufsize, position, ch, 1, MPI_CHAR, comm);
				tem[j].append(ch,1);
			}
			valuechar.push_back(tem[j]);
		}
		delete ch;
	}

}





