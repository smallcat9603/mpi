/**
 * author: Xing Wu
 */

#include "Event.h"
#include "assert.h"
#include "Stat.h"
#include "StatTime.h"
#include "PtrHandler.h"

Event::Event(int op, int rank):
op(op),
loc(SINGLE),
ranklist(rank),
id(0),
prev(0),
next(0)
{
	signature.unwind();
	StatTime *st_comp = new StatTime(STAT_TIME);
	compStats[STAT_TIME] = st_comp;
	StatTime *st_comm = new StatTime(STAT_TIME);
	commStats[STAT_TIME] = st_comm;
#ifdef FEATURE_LOOP_LCS
	insertLoop(1, 1);
#endif

	lastCallValue=0;
}

Event::Event():
id(0),
prev(0),
next(0)
{}


Event::~Event(){
	map<int, Param*>::iterator it;
	for(it=params.begin(); it!=params.end(); it++){
		if( (*it).second )
			delete (*it).second;
	}
	map<int, Stat*>::iterator statit;
	for(statit = compStats.begin(); statit != compStats.end(); statit++){
		if(statit->second)	
			delete statit->second;
	}
	for(statit = commStats.begin(); statit != commStats.end(); statit++){
		if(statit->second)	
			delete statit->second;
	}
}

bool Event::checkLoc(location_info check){
		return loc & check;
}

Event* Event::getDummyEvent(){
	Event *e = new Event();
	e->setOp(op);
	e->setRanklist(ranklist);
	e->setSignature(signature);
	int depth = loopStack.getDepth();
	if(depth > 0)
		e->insertLoop(1, 0);
	for(int i=1; i<depth; i++)
		e->insertLoop(1, 1);
	return e;
}

void Event::eventStatsMerge(Event *e){
	map<int, Stat*> *stats;
	map<int, Stat*>::iterator iter1;
	map<int, Stat*>::iterator iter2;
	stats = e->getCompStats();
	int size1, size2;

	//assert(compStats.size() == stats->size() );

	iter1 = compStats.begin();
	iter2 = stats->begin();
	size1 = compStats.size();
	size2 = stats->size();

	if(size1 == size2){ // merge two real events
		for(; iter1 != compStats.end(); iter1++, iter2++){
			assert(iter1->first == iter2->first);
			iter1->second->merge(iter2->second);
		}
	} else if(size1 == 0){ // merge a real event into a dummy event
		for(; iter2 != stats->end(); iter2++){
			compStats[iter2->first] =iter2->second;
			iter2->second = NULL;
		}
		stats->clear();
	} else if(size2 == 0){ // merge a dummy event into a real event
		//do nothing
	} else {
		cerr << "error: merge events with different types of statistics" << endl;
		cerr << toString(-1) << endl;
		cerr << e->toString(-1) << endl;
		exit(1);
	}

	stats = e->getCommStats();

	//assert(commStats.size() == stats->size() );

	iter1 = commStats.begin();
	iter2 = stats->begin();
	size1 = commStats.size();
	size2 = stats->size();

	if(size1 == size2){ // merge two real events
		for(; iter1 != commStats.end(); iter1++, iter2++){
			assert(iter1->first == iter2->first);
			iter1->second->merge(iter2->second);
		}
	} else if(size1 == 0){ // merge a real event into a dummy event
		for(; iter2 != stats->end(); iter2++){
			commStats[iter2->first] =iter2->second;
			iter2->second = NULL;
		}
		stats->clear();
	} else if(size2 == 0){ // merge a dummy event into a real event
		//do nothing
	} else {
		cerr << "error: merge events with different types of statistics" << endl;
		cerr << toString(-1) << endl;
		cerr << e->toString(-1) << endl;
		exit(1);
	}
}

void Event::updateStatsSig(StackSig* oldSig, StackSig* newSig){
	map<int, Stat*>::iterator iter;

	for(iter = compStats.begin(); iter != compStats.end(); iter++)
		iter->second->updateSig(oldSig, newSig);

	for(iter = commStats.begin(); iter != commStats.end(); iter++)
		iter->second->updateSig(oldSig, newSig);
}

double Event::getStatValue(int phase, int stattype, StackSig* prev_sig){
	map<int, Stat*>::iterator it;
	if(phase == PHASE_COMP){
		it = compStats.find(stattype);
		if(it != compStats.end() ){
			if(it->second)
				return it->second->getValue(prev_sig);
			else
				return 0;
		} else {
			//cerr << "invalid stattype: " << stattype << " for phase " << phase << endl;
			return 0;
		}
	} else if (phase == PHASE_COMM) {
		it = commStats.find(stattype);
		if(it != commStats.end() ){
			if(it->second)
				return it->second->getValue(prev_sig);
			else
				return 0;
		} else {
			//cerr << "invalid stattype: " << stattype << " for phase " << phase << endl;
			return 0;
		}
	} else {
		//cerr << "invalid phase: " << phase << endl;
		return 0;
	}
}
double Event::getStatValue(int phase, int stattype, StackSig* prev_sig, int my_rank){
	//if (my_rank==0)
	//	cout <<"ID: "<< getId() << endl;
	map<int, Stat*>::iterator it;
	if(phase == PHASE_COMP && getOp()!=1084 ){
		it = compStats.find(stattype);
		if(it != compStats.end()){
			if(it->second)
				return it->second->getValue(prev_sig);
			else
				return 0;
		} else {
			if (prev_sig!=NULL)
					cerr << "invalid stattype: " << stattype << " for phase " << phase  << " Event ID:" << getId() << endl;
			else
					cerr << "invalid stattype: " << stattype << " for phase " << phase << " Event ID:" << getId() <<" PREV Signature: NULL" << "OP" << getOp()<< endl;
			
			return 0;
		}
	} else if (phase == PHASE_COMM && getOp()!=1045 && getOp()!=1084) {
		it = commStats.find(stattype);
		if(it != commStats.end() ){
			if(it->second)
				return it->second->getValue(prev_sig);
			else
				return 0;
		} else {
			if (prev_sig!=NULL)
					cerr << "invalid stattype: " << stattype << " for phase " << phase  << " Event ID:" << getId() <<  endl;
			else
					cerr << "invalid stattype: " << stattype << " for phase " << phase  << " Event ID:" << getId() << " PREV Signature: NULL" << "OP" << getOp() << endl;

			//ce`rr << "invalid stattype: " << stattype << " for phase " << phase << endl;
			return 0;
		}
	} else {
		if(getOp()!=1045 && getOp()!=1084)
			cerr << "invalid phase: " << phase << " Event ID:" << getId() << endl;
		return 0;
	}
}

void Event::eventParamMerge(Event *e){
	map<int, Param*> *p = e->getParams();

	//assert(params.size() == p->size() );

	map<int, Param*>::iterator iter1 = params.begin();
	map<int, Param*>::iterator iter2 = p->begin();

	int size1 = params.size();
	int size2 = p->size();

	if(size1 == size2){
		for(; iter1 != params.end(); iter1++, iter2++){
			assert(iter1->first == iter2->first );
			iter1->second->merge(iter2->second);
		}
	} else if( size1 == 0 ){//merge a real event to a dummy event
		for(; iter2 != p->end(); iter2++){
			params[iter2->first] = iter2->second;
			iter2->second = NULL;
		}
		p->clear();
	} else if( size2 == 0 ){//merge a dummy event to a real event
		// do nothing
	} else {
		cerr << "error: merge events with different parameter list" << endl;
		cerr << toString(-1) << endl;
		cerr << e->toString(-1) << endl;
		exit(1);
	}
}

void Event::eventRanklistMerge(Event *e){
	ranklist.merge(e->getRanklist());
}

void Event::addScalarValue(int type, int value, int rank){
	map<int, Param*>::iterator it = params.find(type);
	if( it == params.end()){
		Param *param = new Param(type);
		param->initWithScalar(value, rank);
		params[type] = param;
	} else {
		assert(0);
	}
}

void Event::addCharValue(int type, char *value, int rank){
	map<int,Param*>::iterator it = params.find(type);
	if( it == params.end()){
		Param *param = new Param(type);
		param->initWithChar(value,rank);
		params[type] = param;
	} else {
		assert(0);
	}
}
void Event::addVectorValue(int type, int count, int *values, int rank){
	map<int, Param*>::iterator it = params.find(type);
	if( it == params.end()){
		Param *param = new Param(type);
		param->initWithVector(count, values, rank);
		params[type] = param;
	} else {
		assert(0);
	}
}

void Event::addVectorValuev(int type, int count, int *values, int rank){

	if (count>0) { 
		int average=0;
		int index=0;
		for (int i =0 ; i<count; i++)
			{
				average+= values[i];
				index++;
			}
		average = int (average / count); 
		int quotient = average / 1000;
		average = quotient * 1000;
		addScalarValue(type, average, rank);
	}
	else{	
		map<int, Param*>::iterator it = params.find(type);
		if(it == params.end()){
			Param *param = new Param(type);
			param->initWithVector(count, values, rank);
			params[type] = param;
		}else{
			assert(0);
		}	
	}
}

void Event::GenRankList(vector<int> val){
	ranklist.GenRankList(val);
}

void Event::getParamValues(int type, int index, int *count, int **values){
	map<int, Param*>::iterator it = params.find(type);
	if( it == params.end()){
		// no matching parameter type
		*values = NULL;
		if(count)
			*count = 0;
	} else {
		*values = ((*it).second)->getParamValueAt(index, count);
	}
}

int Event::getScalarParamValue(int type){
	map<int, Param*>::iterator it = params.find(type);
	if( it == params.end()){
		cerr << "cannot find parameter of type: " << type << " in event\n" << toString() << endl;
		return -1;
	} else {
		int count;
		int *values = ((*it).second)->getParamValueAt(0, &count);
		if(values == NULL || count != 1){
			cerr << "cannot get a scalar value for parameter " 
					<< type << " in event\n" << toString() << endl;
			return -1;
		} else {
			return values[0];
		}
	}

}


int Event::getParamValueForRank(int type, int rank, RandMnger* randmnger){
	map<int, Param*>::iterator it = params.find(type);
	if( it == params.end()){
		cerr << "cannot find parameter of type: " << type << " in event\n" << toString() << endl;
		return -1;
	} else {
		return ((*it).second)->getValueForRank(rank, randmnger);
	}
}

int* Event::getParamValuesForRank(int type, int count, int rank, RandMnger* randmnger){
	map<int, Param*>::iterator it = params.find(type);
	if( it == params.end()){
		cerr << "cannot find parameter of type: " << type << " in event\n" << toString() << endl;
		return NULL;
	} else {
		return ((*it).second)->getValuesForRank(count, rank, randmnger);
	}
}

int Event::getCurrentValue(int type, int rank){
	map<int, Param*>::iterator it = params.find(type);
	int ret;
	if( it == params.end()){
		cerr << "cannot find parameter of type: " << type << " in event\n" << toString() << endl;
		return -1;
	} else {
		if(type == OFFSET){
			ret = ((*it).second)->getOffsetValue(rank, getRanklist()->getNumRanks());
			if(ret == -1){
				if(rank==-1) rank=my_rank;
				ret = getParamValueForRank(type, rank);
			}
			return ret;
		}
		else
			return ((*it).second)->getCurrentValue(rank);
	}
}

string Event::getCurrentCharValue(int type, int rank){
	map<int, Param*>::iterator it = params.find(type);
	const int num = getRanklist()->getNumRanks();
	if (it == params.end()){
		cerr << "cannot find parameter of type: "<< type << "in event\n" << toString() << endl;
		return NULL;
	}else{
		vector<ValueSet *> *p;
		p = (*it).second -> getParamValues();
		if((*p).size() > 1){
			return getCharValueForRank(num, type, rank);
		}
		else{
			assert((*p).size() ==1 && typeid(*((*p).at(0))) == typeid(ParamValue));
			return dynamic_cast<ParamValue *>((*p).at(0))->getParamCharValue(num,rank);
		}
	}
}

string Event::getCharValueForRank(int num, int type, int rank, RandMnger* randmnger){
	map<int, Param*>::iterator it = params.find(type);
	if(it == params.end()){
		cerr << "cannot find parameter of type: "<< type << "in event\n" << toString() << endl;
		return NULL;
	}
	else{
		return ((*it).second)->getCharValueForRank(num, rank, randmnger);
	}
}
int* Event::getCurrentValues(int type, int count, int rank){
	map<int, Param*>::iterator it = params.find(type);
	if( it == params.end()){
		cerr << "cannot find parameter of type: " << type << " in event\n" << toString() << endl;
		return NULL;
	} else {
		return ((*it).second)->getCurrentValues(count, rank);
	}
}

void Event::resetTraversalIterators(){
	map<int, Param*>::iterator it;
	for(it = params.begin(); it != params.end(); it++){
		it->second->resetTraversalIterators();
	}
	loopStack.resetTraversalIterators();
}

bool Event::sigMatch(Event *e){
	return signature.match(e->getSignature() );
}

bool Event::sigEquals(Event *e){
	return signature.equals(e->getSignature() );
}

void Event::expandLoop(int expandFactor, int expandDepth){
	int depth = loopStack.getDepth();
	if(expandDepth == -1){
		for(int i=0; i<depth; i++){
			loopStack.getLoopAt(i)->mem->expandLoop(expandFactor);
			loopStack.getLoopAt(i)->iter->expandLoop(expandFactor);
		}
	} else {
		if(expandDepth >= 0 && expandDepth < depth){
			loopStack.getLoopAt(expandDepth)->mem->expandLoop(expandFactor);
			loopStack.getLoopAt(expandDepth)->iter->expandLoop(expandFactor);
		} else {
			cerr << "error: trying to expand a non-exist loop" << endl;
			exit(1);
		}
	}
}

void Event::alignLoop(int targetDepth){
	int depth = loopStack.getDepth();
	if(targetDepth <= depth)
		return;

	loop_t *loop;
	if(depth == 0){
		loopStack.insertLoop(1, 1);
		depth++;
	}

	Param* base_mem = loopStack.getLoopAt(0)->mem;
	Param* base_iter = loopStack.getLoopAt(0)->iter;
	for(int i=0; i<targetDepth - depth; i++){
		loop = new loop_t;
		loop->mem = base_mem->duplicate();
		loop->iter = base_iter->duplicate();
		loop->iter->loopSetOne();
		loopStack.insertLoop(loop);
	}
	
}

bool Event::loopMatch(Event *e, int cur_depth, int target_length){
	int count = 0;
	vector<loop_t *> *merge_loops = loopStack.getLoops();
	vector<loop_t *> *target_loops = e->getLoopStack()->getLoops();
	int merge_depth = merge_loops->size();
	int target_depth = target_loops->size();
	int merge_iter, target_iter;
	int *mem1, *mem2, *iter1, *iter2, len;

	if(merge_depth - target_depth != 0 && merge_depth - target_depth != 1)
		return false;

	for(count = 0; count < cur_depth; count++) {
		merge_iter = merge_depth - 1 - count;
		target_iter = target_depth - 1 - count;
		if(merge_iter < 0 || target_iter < 0){
			return false;
		}
	
		/* loop iteration can always match since we use rlparam and histogram */
		/*if( merge_loops->at(merge_iter)->iter != target_loops->at(target_iter)->iter ||
				merge_loops->at(merge_iter)->mem != target_loops->at(target_iter)->mem ) {
			return false;
		}*/
		mem1 = merge_loops->at(merge_iter)->mem->getParamValueAt(0, &len);
		assert(mem1 && len == 1); 
		mem2 = target_loops->at(target_iter)->mem->getParamValueAt(0, &len);
		assert(mem2 && len == 1); 
		if( mem1[0] != mem2[0] ){
			return false;
		}
		free(mem1);
		free(mem2);
		iter1 = merge_loops->at(merge_iter)->iter->getParamValueAt(0, &len);
		assert(iter1 && len == 1); 
		iter2 = target_loops->at(target_iter)->iter->getParamValueAt(0, &len);
		assert(iter2 && len == 1); 
		if( iter1[0] != iter2[0] ){
			return false;
		}
		free(iter1);
		free(iter2);

	}

	/* fix the compression problem on an event sequence like AAAAABABABAB
		otherwise, it will be compressed to A(7, 1)B */
	if(merge_depth - cur_depth >= 1){
		merge_iter = merge_depth - cur_depth - 1;
		assert(merge_iter >= 0);
		mem1 = merge_loops->at(merge_iter)->mem->getParamValueAt(0, &len);
		assert(mem1 && len == 1); 
		if(mem1[0] != target_length){
        	return 0;
		}
		free(mem1);
	}

	return true;
}

int Event::getPackedSize(){
	int size = 0;
	size += sizeof(int); /* op */
	size += sizeof(int); /* loc */
	size += ranklist.getPackedSize();
	size += sizeof(int); /* num of stats for both comp and comm*/
	map<int, Stat*>::iterator sit;
	for(sit = compStats.begin(); sit != compStats.end(); sit++ ){
		size += sit->second->getPackedSize();
	}
	for(sit = commStats.begin(); sit != commStats.end(); sit++ ){
		size += sit->second->getPackedSize();
	}
	size += sizeof(int); /* num of params */
	map<int, Param*>::iterator it;
	for(it = params.begin() ; it != params.end(); it++){
		size += it->second->getPackedSize();
	}
	size += signature.getPackedSize();
	size += loopStack.getPackedSize();

	return size;
}

void Event::pack(void *buf, int bufsize, int *position, MPI_Comm comm){
	PMPI_Pack(&op, 1, MPI_INT, buf, bufsize, position, comm);
	PMPI_Pack(&loc, 1, MPI_INT, buf, bufsize, position, comm);
	ranklist.pack(buf, bufsize, position, comm);
	int numStats = compStats.size();
	assert(numStats == (int)commStats.size());
	PMPI_Pack(&numStats, 1, MPI_INT, buf, bufsize, position, comm);
	map<int, Stat*>::iterator sit;
	for(sit = compStats.begin(); sit != compStats.end(); sit++ ){
		sit->second->pack(buf, bufsize, position, comm);
	}
	for(sit = commStats.begin(); sit != commStats.end(); sit++ ){
		sit->second->pack(buf, bufsize, position, comm);
	}
	int numParam = params.size();
	PMPI_Pack(&numParam, 1, MPI_INT, buf, bufsize, position, comm);
	map<int, Param*>::iterator it;
	for(it = params.begin() ; it != params.end(); it++){
		it->second->pack(buf, bufsize, position, comm);
	}
	signature.pack(buf, bufsize, position, comm);
	loopStack.pack(buf, bufsize, position, comm);
}

void Event::unpack(void *buf, int bufsize, int *position, MPI_Comm comm){
	PMPI_Unpack(buf, bufsize, position, &op, 1, MPI_INT, comm);
	PMPI_Unpack(buf, bufsize, position, &loc, 1, MPI_INT, comm);
	ranklist.unpack(buf, bufsize, position, comm);
	int numParam, type, numStats;
	PMPI_Unpack(buf, bufsize, position, &numStats, 1, MPI_INT, comm);
	for(int i=0; i<numStats; i++){
		Stat *stat = new Stat();
		stat->unpack(buf, bufsize, position, comm);
		type = stat->getStattype();
		compStats[type] = stat;
	}
	for(int i=0; i<numStats; i++){
		Stat *stat = new Stat();
		stat->unpack(buf, bufsize, position, comm);
		type = stat->getStattype();
		commStats[type] = stat;
	}
	PMPI_Unpack(buf, bufsize, position, &numParam, 1, MPI_INT, comm);
	for(int i=0; i<numParam; i++){
		Param *param = new Param();
		param->unpack(buf, bufsize, position, comm);
		type = param->getParamType();
		params[type] = param;
	}
	signature.unpack(buf, bufsize, position, comm);
	loopStack.unpack(buf, bufsize, position, comm);
}

int eid = 0;

Event* Event::inputEvent(string& buf, int mode, int rank){
	stringstream ss (stringstream::in | stringstream::out);
	stringstream linestream (stringstream::in | stringstream::out);
	ss << buf;
	string line, fields, temp;
	Event *e = NULL;
	while( getline(ss, line) ){
		if(line.size() > 5 && line.substr(0, 5) == "RANKS"){
			Ranklist rl;
			fields = line.substr(line.find(":") + 1 );
			rl.input( fields );
			if(mode == INPUT_NODE && !rl.hasMember(rank))
				return NULL;
			e = new Event();
			e->setId(eid++);
			e->setRanklist(rl);
		} else if (line.size() > 5 && line.substr(0, 5) == "EVENT"){
			linestream.clear();
			linestream << line.substr( line.find(":")+1 );
			int op;
			linestream >> op;
			assert(e);
			e->setOp(op);
			linestream >> fields;
			StackSig sig;
			sig.input(fields);
			e->setSignature(sig);
		} else if (line.size() > 5 && line.substr(0, 4) == "COMP"){
			temp = line.substr(line.find(" "), line.find("-") - line.find(" "));
			linestream.clear();
			linestream << temp;
			int st;
			linestream >> hex >> st;
			map<int, Stat*> *compstats = e->getCompStats();
			map<int, Stat*>::iterator it = compstats->find(st);
			fields = line.substr(line.find(":")+1);
			if( it != compstats->end() ){
				it->second->input(fields);
			} else {
				Stat *stat = new Stat(st);
				stat->input(fields);
				(*compstats)[st] = stat;
			}
		} else if (line.size() > 5 && line.substr(0, 4) == "COMM"){
			temp = line.substr(line.find(" "), line.find("-") - line.find(" "));
			linestream.clear();
			linestream << temp;
			int st;
			linestream >> hex >> st;
			map<int, Stat*> *commstats = e->getCommStats();
			map<int, Stat*>::iterator it = commstats->find(st);
			fields = line.substr(line.find(":")+1);
			if( it != commstats->end() ){
				it->second->input(fields);
			} else {
				Stat *stat = new Stat(st);
				stat->input(fields);
				(*commstats)[st] = stat;
			}
		} else if(line.size() > 5 && line.substr(0, 4) == "LOOP"){
			fields = line.substr(line.find(":")+1);
			e->getLoopStack()->input(fields, mode, rank);
		} else if (line.size() > 0) {
			temp = line.substr(0, line.find(" "));
			linestream.clear();
			linestream << temp;
			int ptype;
			linestream >> hex >> ptype;
			map<int, Param*> *parameters = e->getParams();
			map<int, Param*>::iterator pit = parameters->find(ptype);
			fields = line.substr(line.find(" ")+1);
			if(pit != parameters->end() ){
				pit->second->input(fields, mode, rank, e->getRanklist());
			} else {
				Param *param = new Param(ptype);
				param->input(fields, mode, rank, e->getRanklist());
				(*parameters)[ptype] = param;
			}
		}

	}
	return e;
}

/* CallPath Signature */

long long Event::getCallPathSignature(){
	//cout<< op << ":  " <<signature.toString() << "\n";
	//cout<< op << ":  "; signature.getSignature(); cout << "\n";
	return signature.getSignature();
}


/* Parameter Signature */
void Event::getParameterSignature(long long int &temp_Count, long long int &tempCompTime, long long int &tempCommTime, long long int &tempDataCommOP, long long int &tempSRC, long long int &tempDEST, long long int &tempTAG, long long int &tempKEY, long long int &tempCOLOR, StackSig* prev_sig){

	
	int param_type=-1;
//	int DT = 0;
//	int OP=0;
//	int COM=0;

	map<int, Param*>::iterator it;
	temp_Count=0;
	tempDataCommOP=0;
	tempSRC=0;
	tempDEST=0;
	tempKEY=0;
	tempCOLOR=0;
	
	tempCompTime = (long long) getStatValue(PHASE_COMP, STAT_TIME, prev_sig, -1);
	tempCommTime = (long long) getStatValue(PHASE_COMM, STAT_TIME, prev_sig, -1);

	for(it = params.begin(); it != params.end(); it++)
		{ 
		
			 param_type = (*it).second-> getParamType();
			
			switch(param_type)
			{
			case 18: //Count
				{
					temp_Count=(*it).second->getDistinctValueSig();
					break;
				}
		/*
			case 20: //Data_type
				{
			DT=(*it).second->getValueSig();
			MPI_Datatype Dtype= index_to_type(record_ptr, DT);
			if ((Dtype == MPI_CHAR) || (Dtype == MPI_UNSIGNED_CHAR) || (Dtype == MPI_CHARACTER) || (Dtype == MPI_SIGNED_CHAR) || (Dtype == MPI_BYTE))
						{
							//Char	
							tempDataCommOP|=0x0001;
							//Temp_Byte_Count=1;
							
						}
			else if ((Dtype == MPI_LOGICAL) || (Dtype==MPI_WCHAR))
						{
							//Char
							tempDataCommOP|=0x0001;
							//Temp_Byte_Count=4;
						}
			else if (Dtype == MPI_INTEGER1)
						{
                                                        //INT  
                                                        tempDataCommOP|=0x0002;
                                                        //Temp_Byte_Count=1;
						}
			else if ((Dtype== MPI_SHORT) || (Dtype == MPI_UNSIGNED_SHORT) || (Dtype == MPI_INTEGER2))
						{
                                                        //INT
                                                        tempDataCommOP|=0x0002;
                                                       // Temp_Byte_Count=2;
						}
			else if ((Dtype== MPI_UNSIGNED) || (Dtype == MPI_INTEGER4) || (Dtype == MPI_INT) || (Dtype == MPI_INTEGER))
						{
                                                        //INT  
                                                        tempDataCommOP|=0x0002;
                                                       // Temp_Byte_Count=4;
						
						}
			else if (Dtype== MPI_SHORT_INT)
						{
	                                                      //INT  
                                                        tempDataCommOP|=0x0002;
                                                        //Temp_Byte_Count=6;
						
						}
						
			else if (Dtype== MPI_FLOAT_INT)
						{
                                                        //INT + FLOAT 
                                                        tempDataCommOP|=0x0002;
                                                        tempDataCommOP|=0x0008;
                                                        //Temp_Byte_Count=8;
                                                
						}

			else if ((Dtype== MPI_UNSIGNED_LONG) || (Dtype ==MPI_2INT) || (Dtype ==MPI_2INTEGER) || (Dtype == MPI_INTEGER8) || (Dtype ==MPI_LONG) || (Dtype == MPI_LONG_LONG_INT) || (Dtype == MPI_LONG_LONG) || (Dtype == MPI_UNSIGNED_LONG_LONG))
						{
                                                        //INT  
                                                        tempDataCommOP|=0x0002;
                                                        //Temp_Byte_Count=8;
						

						}

			else if (Dtype== MPI_LONG_INT)
						{
                                                        //INT
                                                        tempDataCommOP|=0x0002;
                                                        //Temp_Byte_Count=12;
						

						}
			else if (Dtype==MPI_LONG_DOUBLE_INT)
						{
                                                        //INT + Double
                                                        tempDataCommOP|=0x0002;
							tempDataCommOP|=0x0004;
                                                        //Temp_Byte_Count=20;
							

						}
			else if ((Dtype== MPI_DOUBLE_PRECISION) || (Dtype == MPI_DOUBLE))
						{
                                                        //DOUBLE  
                                                        tempDataCommOP|=0x0004;
                                                        //Temp_Byte_Count=8;
							

						}

			else if (Dtype== MPI_DOUBLE_INT)
						{
                                                        //DOUBLE + INT
                                                        tempDataCommOP|=0x0002;
							tempDataCommOP|=0x0004;
                                                       // Temp_Byte_Count=12;
							

						}
			else if((Dtype== MPI_DOUBLE_COMPLEX) || (Dtype == MPI_LONG_DOUBLE) || (Dtype == MPI_2DOUBLE_PRECISION))
                                                {
                                                        //DOUBLE 
							//COMPLEX
                                                        tempDataCommOP|=0x0020;
                                                        tempDataCommOP|=0x0004;
                                                        //Temp_Byte_Count=16;
						

                                                }
			else if (Dtype== MPI_2DOUBLE_COMPLEX)
                                                {
                                                        //DOUBLE
							//COMPLEX  
                                                        tempDataCommOP|=0x0004;
							tempDataCommOP|=0x0020;
                                                        //Temp_Byte_Count=32;
							

                                                }

			else if ((Dtype == MPI_REAL) || (Dtype == MPI_REAL4) || (Dtype == MPI_FLOAT))
                                                {
                                                        //REAL + FLOAT
                                                        tempDataCommOP|=0x0008;
                                                        //Temp_Byte_Count=4;
							
                                                }
			else if ((Dtype== MPI_2REAL) || (Dtype == MPI_REAL8))
                                                {
                                                        //REAL + FLOAT
                                                        tempDataCommOP|=0x0008;
                                                        //Temp_Byte_Count=8;
							
                                                }
						
			else if (Dtype== MPI_PACKED)
						{
                                                        //PACKED  
                                                        tempDataCommOP|=0x0010;
                                                       // Temp_Byte_Count=1;
						
						}
			else if ((Dtype == MPI_COMPLEX) || (Dtype == MPI_COMPLEX8))
						{
                                                        //COMPLEX 
                                                        tempDataCommOP|=0x0020;
                                                        //Temp_Byte_Count=8;
						

						}
			else if ((Dtype== MPI_COMPLEX16) || (Dtype == MPI_2COMPLEX))
						{
                                                        //COMPLEX
                                                        tempDataCommOP|=0x0020;
                                                        //Temp_Byte_Count=16;

						}
			else if ((Dtype == MPI_UB) || (Dtype == MPI_LB))
						{
                                                        //MPI_UB + LB  
                                                        tempDataCommOP|=0x0040;
                                                        //Temp_Byte_Count=0;

						}
				
				  break;
				}
						
				case 24://Operation
				{
				        OP=(*it).second->getValueSig();
					MPI_Op OPtype= index_to_op (record_ptr, OP);
                                        if ((OPtype==MPI_MAX) || (OPtype==MPI_MAXLOC))
						{
                                                        //MAX 
                                                        tempDataCommOP|=0x0080;
				
						}
					else if ((OPtype == MPI_MIN) || (OPtype== MPI_MINLOC))
						{
                                                        //MIN  
                                                        tempDataCommOP|=0x0100;

						}

					else if (OPtype == MPI_PROD)
						{
                                                        //PROD 
                                                        tempDataCommOP|=0x0200;

						}
					else if ((OPtype==MPI_BAND) || (OPtype==MPI_LAND))
						{
                                                        //AND  
                                                        tempDataCommOP|=0x0400;

						}
					else if ((OPtype==MPI_BOR) || (OPtype==MPI_LOR))
						{
                                                        //OR   
                                                        tempDataCommOP|=0x0800;

						}
					else if ((OPtype == MPI_LXOR ) || (OPtype==MPI_BXOR ))
						{
                                                        //XOR  
                                                        tempDataCommOP|=0x1000;
						}
					else if (OPtype ==MPI_SUM)
						{
                                                        //SUM  
                                                        tempDataCommOP|=0x2000;

						}
					
					break;
				}	
				case 16://Communicator
				{
					COM=(*it).second->getValueSig();
                                        
					MPI_Comm COMtype= index_to_comm(record_ptr, COM);
				
					if ((COMtype==MPI_COMM_SELF) || (COMtype==MPI_COMM_WORLD))
							{
                                                        //MPI_COMM_WORLD
                                        	                tempDataCommOP|=0x4000;
							}		
					else if (COMtype==MPI_COMM_NULL)
							{
                	                                        //MPI_COMM_NULL  
                        	                                tempDataCommOP|=0x8000;
							}
					
						break;
				}
				case 35: //COLOR
				{
	                       		tempCOLOR=(long long) (*it).second->getValueSig();
	                                break;
				}
				case 36: //KEY
				{
	                       		tempKEY=(long long) (*it).second->getValueSig();
	                                break;
				}
		*/
				case 44:
				case 45:
				case 27: //TAG
				{
	                       		tempTAG=(long long) (*it).second->getValueSig();
	                                break;
				}

				case 26: /*DEST*/
				{
	                       		tempDEST=(long long) (*it).second->getValueSig();
	                                break;
				}

				case 43:/*SOURCE*/
				{
					tempSRC=(long long) (*it).second->getValueSig();
					break;
				}
			}	
			
		}

}



/* Reference Signature  */
map<int, long long int> Event::getRefSignature()
{
        int param_type=-1;
        //vector<int> values;
        map<int, Param*>::iterator it;

	map<int, long long int> tempSig;	
	for(it = params.begin(); it != params.end(); it++)
		{ 
			param_type = (*it).second-> getParamType();
			
			switch(param_type)
			{
			
				case 17:
				case 19:
				case 23:
				case 30:
				case 31:
				case 34:	
				case 47:
				case 58:
				case 61:
				case 66:
				case 68:
				{	
					break;
				}		
		
				case 1:
				{
					tempSig[param_type-1]=(*it).second->getValueSig();
        	                        break;
				}	
				case 2:
				{
					tempSig[param_type-1]=(*it).second->getValueSig();
        	                        break;
				}	

				case 3:
				{
					tempSig[param_type-1]=(*it).second->getValueSig();
        	                        break;
				}	

				case 4:
				{
					tempSig[param_type-1]=(*it).second->getValueSig();
        	                        break;
				}	

				case 5:
				{
					tempSig[param_type-1]=(*it).second->getValueSig();
        	                        break;
				}	

				case 6:
				{
					tempSig[param_type-1]=(*it).second->getValueSig();
        	                        break;
				}	

				case 7:
				{
					tempSig[param_type-1]=(*it).second->getValueSig();
        	                        break;
				}	

				case 8:
				{
					tempSig[param_type-1]=(*it).second->getValueSig();
        	                        break;
				}	

				case 9:
				{
					tempSig[param_type-1]=(*it).second->getValueSig();
        	                        break;
				}	

				case 10:
				{
					tempSig[param_type-1]=(*it).second->getValueSig();
        	                        break;
				}	

				case 11:
				{
					tempSig[param_type-1]=(*it).second->getValueSig();
        	                        break;
				}	

				case 12:
				{
					tempSig[param_type-1]=(*it).second->getValueSig();
        	                        break;
				}	

				case 13:
				{
					tempSig[param_type-1]=(*it).second->getValueSig();
        	                        break;
				}	

				case 14:
				{
					tempSig[param_type-1]=(*it).second->getValueSig();
        	                        break;
				}	

				case 15:
				{
					tempSig[param_type-1]=(*it).second->getValueSig();
        	                        break;
				}	

				case 16: /*Communicator*/
				{
					tempSig[param_type-1]=(*it).second->getValueSig();
        	                        break;
				}	
			
				case 18: /*Count*/
				{
					tempSig[param_type-2]=(*it).second->getValueSig();
        	                        break;
				}	

				case 20: /*Data_type*/ 
				{
					tempSig[param_type-3]=(*it).second->getValueSig();
        	                        break;
				}	


				case 21: 
				{
					tempSig[param_type-3]=(*it).second->getValueSig();
        	                        break;
				}	

				case 22: 
				{
					tempSig[param_type-3]=(*it).second->getValueSig();
        	                        break;
				}	

				case 24:/*Operation*/ 
				{
					tempSig[param_type-4]=(*it).second->getValueSig();
        	                        break;
				}	

				case 25: 
				{
					tempSig[param_type-4]=(*it).second->getValueSig();
        	                        break;
				}	

				case 26: 
				{
					tempSig[param_type-4]=(*it).second->getValueSig();
        	                        break;
				}	

				case 27: 
				{
					tempSig[param_type-4]=(*it).second->getValueSig();
        	                        break;
				}	

				case 28: 
				{
					tempSig[param_type-4]=(*it).second->getValueSig();
        	                        break;
				}	

				case 29: 
				{
					tempSig[param_type-4]=(*it).second->getValueSig();
        	                        break;
				}	

				case 32: 
				{
					tempSig[param_type-6]=(*it).second->getValueSig();
        	                        break;
				}	

				case 33: 
				{
					tempSig[param_type-6]=(*it).second->getValueSig();
        	                        break;
				}	

				case 35: 
				{
					tempSig[param_type-7]=(*it).second->getValueSig();
        	                        break;
				}	

				case 36: 
				{
					tempSig[param_type-7]=(*it).second->getValueSig();
        	                        break;
				}	

				case 37: 
				{
					tempSig[param_type-7]=(*it).second->getValueSig();
        	                        break;
				}	

				case 38: 
				{
					tempSig[param_type-7]=(*it).second->getValueSig();
        	                        break;
				}	

				case 39: 
				{
					tempSig[param_type-7]=(*it).second->getValueSig();
        	                        break;
				}	

				case 40: 
				{
					tempSig[param_type-7]=(*it).second->getValueSig();
        	                        break;
				}	

				case 41: 
				{
					tempSig[param_type-7]=(*it).second->getValueSig();
        	                        break;
				}	

				case 42:	 
				{
					tempSig[param_type-7]=(*it).second->getValueSig();
        	                        break;
				}	

				case 43: 
				{
					tempSig[param_type-7]=(*it).second->getValueSig();
        	                        break;
				}	

				case 44: 
				{
					tempSig[param_type-7]=(*it).second->getValueSig();
        	                        break;
				}	

				case 45: 
				{
					tempSig[param_type-7]=(*it).second->getValueSig();
        	                        break;
				}	

				case 46: 
				{
					tempSig[param_type-7]=(*it).second->getValueSig();
        	                        break;
				}	

				case 48: 
				{
					tempSig[param_type-8]=(*it).second->getValueSig();
        	                        break;
				}	

				case 49: 
				{
					tempSig[param_type-8]=(*it).second->getValueSig();
        	                        break;
				}	

				case 50: 
				{
					tempSig[param_type-8]=(*it).second->getValueSig();
        	                        break;
				}	

				case 51: 
				{
					tempSig[param_type-8]=(*it).second->getValueSig();
        	                        break;
				}	

				case 52: 
				{
					tempSig[param_type-8]=(*it).second->getValueSig();
        	                        break;
				}	

				case 53: 
				{
					tempSig[param_type-8]=(*it).second->getValueSig();
        	                        break;
				}	

				case 54: 
				{
					tempSig[param_type-8]=(*it).second->getValueSig();
        	                        break;
				}	

				case 55: 
				{
					tempSig[param_type-8]=(*it).second->getValueSig();
        	                        break;
				}	

				case 56: 
				{
					tempSig[param_type-8]=(*it).second->getValueSig();
        	                        break;
				}	

				case 57: 
				{
					tempSig[param_type-8]=(*it).second->getValueSig();
        	                        break;
				}	

				case 59: 
				{
					tempSig[param_type-9]=(*it).second->getValueSig();
        	                        break;
				}	

				case 60: 
				{
					tempSig[param_type-9]=(*it).second->getValueSig();
        	                        break;
				}	

				case 62: 
				{
					tempSig[param_type-10]=(*it).second->getValueSig();
        	                        break;
				}	

				case 63: 
				{
					tempSig[param_type-10]=(*it).second->getValueSig();
        	                        break;
				}	

				case 64: 
				{
					tempSig[param_type-10]=(*it).second->getValueSig();
        	                        break;
				}	

				case 65: 
				{
					tempSig[param_type-10]=(*it).second->getValueSig();
        	                        break;
				}	

				case 67: 
				{
					tempSig[param_type-11]=(*it).second->getValueSig();
        	                        break;
				}	

				case 69: 
				{
					tempSig[param_type-12]=(*it).second->getValueSig();
        	                        break;
				}	

				case 70: 
				{
					tempSig[param_type-12]=(*it).second->getValueSig();
        	                        break;
				}	

				case 71: 
				{
					tempSig[param_type-12]=(*it).second->getValueSig();
        	                        break;
				}	

				case 72: 
				{
					tempSig[param_type-12]=(*it).second->getValueSig();
        	                        break;
				}	

				case 73: 
				{
					tempSig[param_type-12]=(*it).second->getValueSig();
        	                        break;
				}	

				case 74: 
				{
					tempSig[param_type-12]=(*it).second->getValueSig();
        	                        break;
				}	

				case 75: 
				{
					tempSig[param_type-12]=(*it).second->getValueSig();
        	                        break;
				}	

			
			}	
			
		}

	
	return tempSig;

}



string Event::toString(int seq_num){
	ostringstream rtn;


	rtn << "RANKS: " << ranklist.toString() << "\n";

	if(seq_num == -1)
		rtn << "EVENT: " << op << " " << signature.toString() << "\n";
	else
		rtn << "EVENT" << seq_num << ": " << op << " " << signature.toString() << "\n";

	map<int, Stat*>::iterator statit;
	for(statit = compStats.begin(); statit != compStats.end(); statit++){
		if(statit->second)	
			rtn << statit->second->toString(PHASE_COMP);
	}
	for(statit = commStats.begin(); statit != commStats.end(); statit++){
		if(statit->second)	
			rtn << statit->second->toString(PHASE_COMM);
	}
	rtn << loopStack.toString();

	map<int, Param*>::iterator it;
	for(it = params.begin(); it != params.end(); it++)
		rtn << (*it).second->toString();

	return rtn.str();
}

void Event::reset(int phase){
	map<int, Stat*>::iterator statit;
	if(phase == PHASE_COMP){
		for(statit = compStats.begin(); statit != compStats.end(); statit++){
			if(statit->second)	
				statit->second->resetStat();
		}
	} else if(phase == PHASE_COMM){
		for(statit = commStats.begin(); statit != commStats.end(); statit++){
			if(statit->second)	
				statit->second->resetStat();
		}
	}
}

void Event::record(Event *last, int phase){
	StackSig *prevSig;
	bool delete_flag = false;
	if(last == NULL){
		prevSig = new StackSig();
		delete_flag = true;
	} else {
		prevSig = last->getSignature();
	}
	map<int, Stat*>::iterator statit;
	if(phase == PHASE_COMP){
		for(statit = compStats.begin(); statit != compStats.end(); statit++){
			if(statit->second){
				statit->second->recordStat(prevSig);
				setLastCallValue(statit->second->getLastCallValue());
			}
		}
	} else if(phase == PHASE_COMM){
		for(statit = commStats.begin(); statit != commStats.end(); statit++){
			if(statit->second){
				statit->second->recordStat(prevSig);
			}
		}
	}
	if(delete_flag)
		delete prevSig;
}

/* ---- C wrapper functions follow ----- */

Event *event;

void createEvent(Event **event, int op, int rank){
	(*event) = new Event(op, rank);
}

void resetStats(Event *event, int phase){
	if(event!=NULL)
		event->reset(phase);
}

void recordStats(Event *event, Event *last, int phase){
	event->record(last, phase);
}

/*
void addScalarValue(Event *event, int type, int value, int rank){
	event->addScalarValue(type, value, rank);
}
*/

void addScalarValue(Event *event, int type, int value, int rank){
	event->addScalarValue(type, value, rank);
}

void addCharValue(Event *event, int type, char* value, int rank){
	event->addCharValue(type,value,rank);
}
void addVectorValue(Event *event, int type, int count, int* values, int rank){
	event->addVectorValue(type, count, values, rank);
}

void addVectorValuev(Event *event, int type, int count, int* values, int rank){
	event->addVectorValuev(type, count, values, rank);
}

void getParamValues(Event *event, int type, int index, int *count, int **values){
	event->getParamValues(type, index, count, values);
}

int getScalarParamValue(Event *event, int type){
	return event->getScalarParamValue(type);
}

char *outputEvent(Event *event){
	string output = event->toString();
	char *rtn = (char *) malloc(sizeof(char) * (output.size() + 1) );
	strcpy(rtn, output.c_str());
	return rtn;
}

void deleteEvent(Event **event){
	if( (*event) ){
		delete (*event);
	}
	(*event) = NULL;
}
