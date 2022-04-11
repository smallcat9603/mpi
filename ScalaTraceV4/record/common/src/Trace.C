/**
 * Author: Xing Wu
 */
#include <typeinfo>
#include "Trace.h"
#ifdef FEATURE_CODE_GENERATOR
#include "code_generator_handler.h"
#endif


/*************************************************************************
 * PendingIterations
 * **********************************************************************/

PendingIterations::PendingIterations(){}

PendingIterations::~PendingIterations(){
	iterationmap_iterator_t it;
	for(it = mh_key.begin(); it != mh_key.end(); it++){
		if(it->second)
			delete it->second;
	}
	mh_key.clear();
	mt_key.clear();
	th_key.clear();
	tt_key.clear();
}

void PendingIterations::insert(iteration_t *iteration){
	mh_key.insert(pair<Event*, iteration_t*>(iteration->merge_head, iteration));
	mt_key.insert(pair<Event*, iteration_t*>(iteration->merge_tail, iteration));
	th_key.insert(pair<Event*, iteration_t*>(iteration->target_head, iteration));
	tt_key.insert(pair<Event*, iteration_t*>(iteration->target_tail, iteration));
}

void PendingIterations::create(Event* merge_head, Event* merge_tail, int merge_length,
						Event* target_head, Event* target_tail, int target_length){
	iteration_t *iteration = new iteration_t;
	iteration->merge_head = merge_head;
	iteration->merge_tail = merge_tail;
	iteration->merge_length = merge_length;
	iteration->target_head = target_head;
	iteration->target_tail = target_tail;
	iteration->target_length = target_length;
	insert(iteration);
}

vector<iteration_t *> PendingIterations::getIterationsByMergeHead(Event* merge_head){
	pair<iterationmap_iterator_t, iterationmap_iterator_t> iterations;
	iterations = mh_key.equal_range(merge_head);
	iterationmap_iterator_t it;
	vector<iteration_t *> rtn;
	for(it = iterations.first; it != iterations.second; ++it)
		rtn.push_back(it->second);
	return rtn;
}
vector<iteration_t *> PendingIterations::getIterationsByMergeTail(Event* merge_tail){
	pair<iterationmap_iterator_t, iterationmap_iterator_t> iterations;
	iterations = mt_key.equal_range(merge_tail);
	iterationmap_iterator_t it;
	vector<iteration_t *> rtn;
	for(it = iterations.first; it != iterations.second; ++it)
		rtn.push_back(it->second);
	return rtn;
}
vector<iteration_t *> PendingIterations::getIterationsByTargetHead(Event* target_head){
	pair<iterationmap_iterator_t, iterationmap_iterator_t> iterations;
	iterations = th_key.equal_range(target_head);
	iterationmap_iterator_t it;
	vector<iteration_t *> rtn;
	for(it = iterations.first; it != iterations.second; ++it)
		rtn.push_back(it->second);
	return rtn;
}
vector<iteration_t *> PendingIterations::getIterationsByTargetTail(Event* target_tail){
	pair<iterationmap_iterator_t, iterationmap_iterator_t> iterations;
	iterations = tt_key.equal_range(target_tail);
	iterationmap_iterator_t it;
	vector<iteration_t *> rtn;
	for(it = iterations.first; it != iterations.second; ++it)
		rtn.push_back(it->second);
	return rtn;
}

void PendingIterations::deleteIteration(iteration_t* iteration){
	pair<iterationmap_iterator_t, iterationmap_iterator_t> iterations;
	iterationmap_iterator_t it;

	iterations = mh_key.equal_range(iteration->merge_head);
	for(it = iterations.first; it != iterations.second; ++it)
		if(it->second == iteration){
			mh_key.erase(it);
			break;
		}
	iterations = mt_key.equal_range(iteration->merge_tail);
	for(it = iterations.first; it != iterations.second; ++it)
		if(it->second == iteration){
			mt_key.erase(it);
			break;
		}
	iterations = th_key.equal_range(iteration->target_head);
	for(it = iterations.first; it != iterations.second; ++it)
		if(it->second == iteration){
			th_key.erase(it);
			break;
		}
	iterations = tt_key.equal_range(iteration->target_tail);
	for(it = iterations.first; it != iterations.second; ++it)
		if(it->second == iteration){
			tt_key.erase(it);
			break;
		}
}

void PendingIterations::print(){
	iterationmap_iterator_t it;
	for(it = mh_key.begin(); it != mh_key.end(); it++){
		if(it->second){
			cerr << it->second->merge_head << " ";
			cerr << it->second->merge_head->getOp() << " ";
			cerr << it->second->merge_tail << " ";
			cerr << it->second->merge_tail->getOp() << " ";
			cerr << it->second->merge_length << " ";
			cerr << it->second->target_head << " ";
			cerr << it->second->target_head->getOp() << " ";
			cerr << it->second->target_tail << " ";
			cerr << it->second->target_tail->getOp() << " ";
			cerr << it->second->target_length << endl;
		}
	}
	cerr << endl;
	cerr << endl;
	cerr << endl;
}

void PendingIterations::updateIterationMergeHead(iteration_t* iteration, Event* new_merge_head){
	pair<iterationmap_iterator_t, iterationmap_iterator_t> iterations;
	iterationmap_iterator_t it;
	iterations = mh_key.equal_range(iteration->merge_head);
	for(it = iterations.first; it != iterations.second; ++it){
		if(it->second == iteration){
			mh_key.erase(it);
			mh_key.insert(pair<Event*, iteration_t*>(new_merge_head, iteration));
			iteration->merge_head = new_merge_head;
			break;
		}
	}
}
void PendingIterations::updateIterationMergeTail(iteration_t* iteration, Event* new_merge_tail){
	pair<iterationmap_iterator_t, iterationmap_iterator_t> iterations;
	iterationmap_iterator_t it;
	iterations = mt_key.equal_range(iteration->merge_tail);
	for(it = iterations.first; it != iterations.second; ++it){
		if(it->second == iteration){
			mt_key.erase(it);
			mt_key.insert(pair<Event*, iteration_t*>(new_merge_tail, iteration));
			iteration->merge_tail = new_merge_tail;
			break;
		}
	}
}
void PendingIterations::updateIterationTargetHead(iteration_t* iteration, Event* new_target_head){
	pair<iterationmap_iterator_t, iterationmap_iterator_t> iterations;
	iterationmap_iterator_t it;
	iterations = th_key.equal_range(iteration->target_head);
	for(it = iterations.first; it != iterations.second; ++it){
		if(it->second == iteration){
			th_key.erase(it);
			th_key.insert(pair<Event*, iteration_t*>(new_target_head, iteration));
			iteration->target_head = new_target_head;
			break;
		}
	}
}
void PendingIterations::updateIterationTargetTail(iteration_t* iteration, Event* new_target_tail){
	pair<iterationmap_iterator_t, iterationmap_iterator_t> iterations;
	iterationmap_iterator_t it;
	iterations = tt_key.equal_range(iteration->target_tail);
	for(it = iterations.first; it != iterations.second; ++it){
		if(it->second == iteration){
			tt_key.erase(it);
			tt_key.insert(pair<Event*, iteration_t*>(new_target_tail, iteration));
			iteration->target_tail = new_target_tail;
			break;
		}
	}
}


/*************************************************************************
 * Trace
 * **********************************************************************/

Trace::Trace(){
	head = NULL;
	tail = NULL;
	ReClusteringFlag=false;
	NoClustering = false;
	InterCompression = false;
	SlackTime=0;
	NumEvents=0;		
	NumIters=0;	
	cnt_AT=0;
	cnt_L=0;
	cnt_C=0;	
	MarkerCommValue=-1;		
}

Trace::~Trace(){
	Event* iter = head, *next;
	while(iter){
		next = iter->next;
		delete iter;
		iter = next;
	}
	delete myCluster; 
}

void Trace::deleteTrace(Trace *trace){
	Event* iter = head, *next;
	while(iter){
		next = iter->next;
		if(iter != lastTail)
			delete iter;
		iter = next;
	}
	
		head =NULL;
		tail = NULL;
}

void Trace::onlineFastCompressLoop(int window) { //, Event* slaveTail ){
	static int warned_once = 0;
	int distance;

	int head_flag;      /* Whether target head has been found. */
	int match_flag;     /* Whether a node matching target head has been found. */
	int found_flag;     /* */

	int target_length=1, merge_length=1;

	Event *target_tail = NULL;
	Event *target_head = NULL;
	Event *merge_tail = NULL;
	Event *merge_head = NULL;

	distance = 0;
	head_flag = 0;
	found_flag = 1;

	do {
		/*Place of change*/
		target_tail = tail;

		if(found_flag || !head_flag) {
			/* if the found flag is set - start a new search */
			target_head = target_tail;
			distance = 0;
			target_length = merge_length = 1;
		} else {
			/* otherwise continue a previous search */
			target_head = target_head->prev;
			target_length++;
			merge_length = 1;
		}

		head_flag = 0;
		match_flag = 0;
		found_flag = 0;


		/* Search backwards in the queue until we find a match with the target tail.
		   After this loop is done, either head_flag will be false (and nothing found)
		   or the head will be just after the matching tail.
		 */
		while(target_head->prev != NULL && (window == -1 || window > distance)) {
			if( !target_head->checkLoc(MEMBER) 
					&& target_tail->sigMatch(target_head->prev) 
					&& target_tail->opMatch(target_head->prev)){
				head_flag = 1;
				break;
			}  
			target_head = target_head->prev;
			target_length++;
			distance++;
		}

		/* didn't find head: can skip the rest. */
		if (!head_flag) {
			/* Warn if the window was exceeded, but only warn once. */
			if(!warned_once && window != -1 && window <= distance) {
				fprintf(stderr, "warning: window exceeded - possible compression missed (window set at %d)\n", window);
				fflush(stderr);
				warned_once = 1;
			}
			continue;
		}

		/* If the head flag is true, we've got the target_tail, the target_head, and the merge_tail (just before target_head).
		   Now we can try to find a match for the target_head somewhere before the merge tail.  This will be merge_head.*/
		merge_head = target_head->prev;
		merge_tail = target_head->prev;

		while(merge_head != NULL) {
			if( !merge_head->checkLoc(MEMBER) 
					&& merge_head->sigMatch(target_head) 
					&& merge_head->opMatch(target_head)
					&& (merge_length == target_length) 
			  ) {
				match_flag = 1;
				break;
			}
			merge_head = merge_head->prev;
			merge_length++;
		}

		/* check lengths of sequences and avoid a traversal if there's clearly no match */
		if(merge_length != target_length ){
			continue;
		}

		/* If we didn't find the merge head, we know there's no match in the sequence. Bail. */
		if(!match_flag) continue;

		// Make sure the sequences [target_head .. target_tail] and [merge_head .. merge_tail] match.
		if (!matchLoop(merge_head, merge_tail, merge_length, target_head, target_tail, target_length)) {
			continue;
		} else {
			found_flag = 1;
			//cout << "FOUND a Loop match!" << endl;
			mergeLoopEvents(merge_head, merge_tail, target_head, target_tail);
			// this adjusts prsd info in the merge sequence to reflect the new loop we're adding 
			//updateOnlineLoopInfo
		//	updateOnlineLoopInfo(merge_head, merge_tail, target_head, target_tail, merge_length);
			updateLoopInfo(merge_head, merge_tail, target_head, target_tail, merge_length);
			// if the compression was successful, go through and free all the target nodes from the end
			// of the seqence.
		//	onlineDeleteTail(merge_tail, slaveTail);
			deleteTail(merge_tail);
		}
		
	
	} while(head_flag || found_flag);

} /* interFastCompressLoop */


void Trace::fastCompressLoop(int window){
	static int warned_once = 0;
	int distance;

	int head_flag;      /* Whether target head has been found. */
	int match_flag;     /* Whether a node matching target head has been found. */
	int found_flag;     /* */

	int target_length=1, merge_length=1;

	Event *target_tail = NULL;
	Event *target_head = NULL;
	Event *merge_tail = NULL;
	Event *merge_head = NULL;

	distance = 0;
	head_flag = 0;
	found_flag = 1;

	do {
		target_tail = tail;

		if(found_flag || !head_flag) {
			/* if the found flag is set - start a new search */
			target_head = target_tail;
			distance = 0;
			target_length = merge_length = 1;
		} else {
			/* otherwise continue a previous search */
			target_head = target_head->prev;
			target_length++;
			merge_length = 1;
		}

		head_flag = 0;
		match_flag = 0;
		found_flag = 0;


		/* Search backwards in the queue until we find a match with the target tail.
			After this loop is done, either head_flag will be false (and nothing found)
			or the head will be just after the matching tail.
		*/
		while(target_head->prev != NULL && (window == -1 || window > distance)) {
			if( !target_head->checkLoc(MEMBER) 
							&& target_tail->sigMatch(target_head->prev) 
							&& target_tail->opMatch(target_head->prev)){
				head_flag = 1;
				break;
			}  
			target_head = target_head->prev;
			target_length++;
			distance++;
		}

		/* didn't find head: can skip the rest. */
		if (!head_flag) {
			/* Warn if the window was exceeded, but only warn once. */
			if(!warned_once && window != -1 && window <= distance) {
				fprintf(stderr, "warning: window exceeded - possible compression missed (window set at %d)\n", window);
				fflush(stderr);
				warned_once = 1;
			}
			continue;
		}

		/* If the head flag is true, we've got the target_tail, the target_head, and the merge_tail (just before target_head).
		Now we can try to find a match for the target_head somewhere before the merge tail.  This will be merge_head.*/
		merge_head = target_head->prev;
		merge_tail = target_head->prev;

		while(merge_head != NULL) {
			if( !merge_head->checkLoc(MEMBER) 
							&& merge_head->sigMatch(target_head) 
							&& merge_head->opMatch(target_head)
							&& (merge_length == target_length) 
							) {
				match_flag = 1;
				break;
			}
			merge_head = merge_head->prev;
			merge_length++;
		}

	    /* check lengths of sequences and avoid a traversal if there's clearly no match */
		if(merge_length != target_length ){
			continue;
		}

		/* If we didn't find the merge head, we know there's no match in the sequence. Bail. */
		if(!match_flag) continue;

		// Make sure the sequences [target_head .. target_tail] and [merge_head .. merge_tail] match.
		if (!matchLoop(merge_head, merge_tail, merge_length, target_head, target_tail, target_length)) {
			continue;
		} else {
			found_flag = 1;
			mergeLoopEvents(merge_head, merge_tail, target_head, target_tail);
			// this adjusts prsd info in the merge sequence to reflect the new loop we're adding 
			updateLoopInfo(merge_head, merge_tail, target_head, target_tail, merge_length);
			// if the compression was successful, go through and free all the target nodes from the end
			// of the seqence.
			deleteTail(merge_tail);
		}

	} while(head_flag || found_flag);

} /* fastCompressLoop */


bool Trace::matchLoop(Event* merge_head, Event* merge_tail, int merge_length, 
				Event *target_head, Event* target_tail, int target_length){

	Event *merge_iter = merge_tail;
	Event *target_iter = target_tail;

	do {
		/* check signatures */
		if( !(target_iter->sigMatch(merge_iter) && target_iter->opMatch(merge_iter)) ) {
			return false;
		}

		/* check loop information - the merge head is a special case 
			we can merge the target into any loop layer */
		int cur_depth;
		if(merge_head == merge_iter) {
			cur_depth = target_iter->getLoopDepth();
		} else {
			cur_depth = merge_iter->getLoopDepth();
		}

		if(!merge_iter->loopMatch(target_iter, cur_depth, target_length) ) {
			return false;
		}

		merge_iter = merge_iter->prev;
		target_iter = target_iter->prev;

	} while (merge_iter && target_iter && merge_iter != merge_head->prev && target_iter != target_head->prev);

	// if we got through the whole sequence, this is true.
	return (merge_iter == merge_head->prev && target_iter == target_head->prev);
} /* matchLoop */


void Trace::mergeLoopEvents(Event* merge_head, Event* merge_tail, Event* target_head, Event* target_tail){
	Event* merge_iter;
	Event* target_iter;
#if FEATURE_SIG_DIFF > 0
	/* the following code changes the signature of the histograms */
	merge_iter = merge_tail;
	target_iter = target_tail;

	Event* search;
	StackSig* target_sig;
	StackSig* merge_sig;

	while(target_iter != merge_tail){
		target_sig = target_iter->getSignature();
		merge_sig = merge_iter->getSignature();
		search = target_tail;
		while(search != merge_tail){
			search->updateStatsSig(target_sig, merge_sig);
			search = search->prev;
		}
		target_iter->setSignature(*merge_sig);
		target_iter = target_iter->prev;
		merge_iter = merge_iter->prev;
	}
#endif
	merge_iter = merge_tail;
	target_iter = target_tail;
	while(target_iter != merge_tail){


#ifdef ONLINE_CLUSTERING
	if(target_iter->getOnlineClusteringFlag())
		merge_iter->setOnlineClusteringFlagON();	
#endif


		merge_iter->eventParamMerge(target_iter);
		merge_iter->eventStatsMerge(target_iter);
		target_iter = target_iter->prev;
		merge_iter = merge_iter->prev;
	
		if(merge_iter!=NULL && target_iter!=NULL){	
			merge_iter->setLastCallValue(target_iter->getLastCallValue());
		}

	}
} /* mergeLoopEvents */


void Trace::updateLoopInfo(Event* merge_head, Event *merge_tail, Event *target_head, Event *target_tail, int merge_length){
	/* first, merge the loop structures in the loop body of the current depth */
	Event *miter = merge_tail, *titer = target_tail;
	while(titer != target_head){
		miter->getLoopStack()->mergeLoopStack(titer->getLoopStack() );
		miter->setLoc(MEMBER);
		miter = miter->prev;
		titer = titer->prev;
	}

	/* then, merge the loop heads */
	int target_depth = target_head->getLoopDepth();
	int merge_depth = merge_head->getLoopDepth();
	if(target_depth == merge_depth){
		merge_head->getLoopStack()->mergeLoopStack(target_head->getLoopStack() );
		merge_head->insertLoop(merge_length, 2);
	} else {
		assert(merge_depth > target_depth);
		merge_head->getLoopStack()->mergeLoopStack(target_head->getLoopStack() );
		merge_head->getLoopStack()->incLoopIter(merge_depth - target_depth - 1);
	}
	merge_head->setLoc(LEADER);
} /* updateLoopInfo */

void Trace::updateOnlineLoopInfo(Event* merge_head, Event *merge_tail, Event *target_head, Event *target_tail, int merge_length){

	/* first, merge the loop structures in the loop body of the current depth */
	Event *miter = merge_tail, *titer = target_tail;
	while(titer != target_head){
		miter->getLoopStack()->mergeLoopStack(titer->getLoopStack() );
		miter->setLoc(MEMBER);
		miter = miter->prev;
		titer = titer->prev;
	}


	/* then, merge the loop heads*/ 
	int target_depth = target_head->getLoopDepth();
	int merge_depth = merge_head->getLoopDepth();

	/*
	assert(merge_depth > target_depth);
	merge_head->getLoopStack()->mergeLoopStack(target_tail->getLoopStack());
	merge_head->getLoopStack()->incLoopIter(0);
	*/


	vector<Event*> heads;
	vector<int> positions;
	miter = merge_tail;
	titer = target_tail;

	//Find Point of Entries
	Event *tmp = merge_head;
	for(int i=0; i<merge_length-1; i++){
		map<int, Stat*> *compstats = tmp->getCompStats();
		bool searchFlag=true;
		for(map<int, Stat*>::iterator it = compstats->begin(); it != compstats->end() && searchFlag==true; it++) {
			if ((*it).second->getValue(merge_tail->getSignature()) != 0){
				//cout << "Head Node: " << endl;
				//cout << tmp->toString()<<endl;
				heads.push_back(tmp);		
				searchFlag=false;	
				positions.push_back(i);
			}
		}	
		tmp = tmp->next;
	}

	vector<Ranklist> headsMemRanklists;
	for(unsigned int i = 0; i != heads.size(); i++) 
		headsMemRanklists.push_back(*(heads[i]->getRanklist()));

	//Calculate number of members and Ranklists
	vector<unsigned int> NumConnections;
	for(unsigned int i = 0; i != heads.size(); i++) {
		unsigned int ConnectivityCounter=0;
		Ranklist *rl = heads[i]->getRanklist();
		for(unsigned int j = i+1; j != heads.size(); j++) {
			
			map<int, Stat*> *compstats = heads[j]->getCompStats();
			for(map<int, Stat*>::iterator it = compstats->begin(); it != compstats->end(); it++) {
				if ((*it).second->getValue(heads[i]->getSignature()) == 0)
					ConnectivityCounter++;	
				else{
					headsMemRanklists[j].exclude(rl);
				}
					
			}	
		}
		
		NumConnections.push_back(ConnectivityCounter+i);
	}


	int index1=0, index2=0;	

	index1=0, index2=0;	
	Event* tmp_target=target_head;

	for(std::vector<Event *>::iterator it = heads.begin(); it != heads.end(); ++it) {
	    // std::cout << *it; ...

		//Search for target heads
		while(index2!=positions[index1]){
			tmp_target=tmp_target->next;
			index2++;
		}
			
		//TODO:If the outer loop contains all the mebers then no new loop
		if(target_depth == merge_depth){
			(*it)->getLoopStack()->mergeLoopStack(tmp_target->getLoopStack());
			(*it)->onlineInsertLoop(merge_length - NumConnections[index1], 2, headsMemRanklists[index1]);

		}
		else{

			//assert(merge_depth > target_depth);
			(*it)->getLoopStack()->mergeLoopStack(tmp_target->getLoopStack());
					//(*it)->getLoopStack()->incLoopIter(merge_depth - target_depth - 1);
			(*it)->getLoopStack()->incLoopIter(0);
		}	

		index1++;
	}

	merge_head->setLoc(LEADER);

} /* updateOnlineLoopInfo */




void Trace::deleteTail(Event* merge_tail){
	Event *iter = tail, *prev;
	while(iter != merge_tail){
		prev = iter->prev;
		delete iter;
		iter = prev;
	}
	iter->next = NULL;
	tail = iter;
}

void Trace::onlineDeleteTail(Event* merge_tail, Event *slaveTail){
	Event *iter = slaveTail, *prev;
	while(iter != merge_tail){
		prev = iter->prev;
		delete iter;
		iter = prev;
	}
	iter->next = NULL;
	tail = iter;
}



void Trace::slowCompressLoop(int window){
	static int warned_once = 0;
	int distance;
	Event *iter;
	iteration_t *rtn;
	iteration_t *iteration;
	vector<iteration_t *> iterations;
	vector<iteration_t *>::iterator iteration_it;
	int i;

	int head_flag;      /* Whether target head has been found. */
	int match_flag;     /* Whether a node matching target head has been found. */
	int found_flag;     /* */

	int target_length=1, merge_length=1;
	int target_real_length, merge_real_length;
	double ratio;

	Event *target_tail = NULL;
	Event *target_head = NULL;
	Event *merge_tail = NULL;
	Event *merge_head = NULL;

#if defined FEATURE_SIG_DIFF && FEATURE_SIG_DIFF > 0
	Event* search;
	StackSig* target_sig;
	StackSig* merge_sig;
#endif

	distance = 0;
	head_flag = 0;
	found_flag = 1;

	do {
		target_tail = tail;

		if(found_flag || !head_flag) {
			/* if the found flag is set - start a new search */
			target_head = target_tail;
			distance = 0;
			target_length = merge_length = 1;
		} else {
			/* otherwise continue a previous search */
			target_head = target_head->prev;
			target_length++;
			merge_length = 1;
		}

		head_flag = 0;
		match_flag = 0;
		found_flag = 0;


		/* Search backwards in the queue until we find a match with the target tail. 
		 * After this loop is done, either head_flag will be false (and nothing found) 
		 * or the head will be just after the matching tail. */
		while(target_head->prev != NULL && (window == -1 || window > distance)) {
			if( !target_head->checkLoc(MEMBER) 
							&& !target_head->checkLoc(PENDING_MEMBER)
							&& !target_head->checkLoc(PENDING_LEADER)
							&& !target_head->checkLoc(PENDING_TAIL)
							&& target_tail->sigMatch(target_head->prev) 
							&& target_tail->opMatch(target_head->prev)){
				head_flag = 1;
				break;
			}  
			target_head = target_head->prev;
			target_length++;
			distance++;
		}

		/* didn't find head: can skip the rest. */
		if (!head_flag) {
			/* Warn if the window was exceeded, but only warn once. */
			if(!warned_once && window != -1 && window <= distance) {
				cerr<<"warning: window exceeded - possible compression missed (window: "<<window<<"   distance: "<<distance<<")"<<endl;
				warned_once = 1;
			}
			break;
		}

		/* If the head flag is true, we've got the target_tail, the target_head, and 
		 * the merge_tail (just before target_head). Now we can try to find a match 
		 * for the target_head somewhere before the merge tail.  This will be merge_head.*/
		merge_head = target_head->prev;
		merge_tail = target_head->prev;

		while(merge_head != NULL) {
			if( !merge_head->checkLoc(MEMBER) 
							&& merge_head->sigMatch(target_head) 
							&& merge_head->opMatch(target_head)){

				target_real_length = realLength(target_head, target_tail);
				merge_real_length = realLength(merge_head, merge_tail);
				assert(target_real_length > 0 && merge_real_length > 0);
				if(target_real_length > merge_real_length){
					ratio = (double)merge_real_length / (double)target_real_length;
				} else {
					ratio = (double)target_real_length / (double)merge_real_length;
				}
				if(1 - ratio < MAX_LENGTH_DIFF){
#if defined FEATURE_SIG_DIFF && FEATURE_SIG_DIFF > 0
					/* heuristic that helps to identify the best match */
					if( (target_tail->sigEquals(merge_tail) && target_head->sigEquals(merge_head)) 
									|| (merge_length == target_length && merge_length == 1 )  )
#endif
					if(merge_head->checkLoc(PENDING_MEMBER) || merge_head->checkLoc(PENDING_LEADER)
								|| merge_head->checkLoc(PENDING_TAIL)){
						for(iter = merge_head, i = 0; iter; iter = iter->prev, i++){
							iterations = pendingIterations.getIterationsByTargetHead(iter);
							if(iterations.size() == 1){
								if(iterations[0]->target_tail->getId() >= merge_head->getId()){
									break;
								}
							}else{
								/* only one pending iteration can have iter as the 
								 * target_head, because it's impossible to choose a
								 * PENDING_LEADER event as the target_head */
								assert(iterations.size()==0);
							}
							iterations.clear();
						}
						if(iterations.size() == 1 && (merge_tail == iterations[0]->target_tail)){
							match_flag = 1;
							break;
						}
					} else {
						match_flag = 1;
						break;
					}
			}
		}
		merge_head = merge_head->prev;
		merge_length++;
	}

		/* If we didn't find the merge head, we continue to the next iteration to try to 
		 * find a longer iteration. */
		if(!match_flag) 
			continue;
		found_flag = 1;

		/* mark the loop iteration */
		/* 1. check if the current iteration is a trailing iteration, if
		 * it is a trailing iteration, only the case where merge_tail equals
		 * to the tail of the pervious iteration is allowed */
		if(merge_head->checkLoc(PENDING_MEMBER) || merge_head->checkLoc(PENDING_LEADER) 
						|| merge_head->checkLoc(PENDING_TAIL)){
			for(iter = merge_head, i = 0; iter; iter = iter->prev, i++){
				iterations = pendingIterations.getIterationsByTargetHead(iter);
				if(iterations.size() == 1){
					//if(iterations[0]->target_length - i > 0){
					if(iterations[0]->target_tail->getId() >= merge_head->getId()){
						break;
					}
				}else{
					/* only one pending iteration can have iter as the 
					 * target_head, because it's impossible to choose a
					 * PENDING_LEADER event as the target_head */
					assert(iterations.size()==0);
				}
				iterations.clear();
			}

			if(iterations.size() == 1 && (merge_tail == iterations[0]->target_tail)){
				iterations = pendingIterations.getIterationsByTargetTail(merge_tail);
				for(iteration_it = iterations.begin(); 
								iteration_it != iterations.end(); iteration_it++)
					for(iter = (*iteration_it)->target_tail; 
									iter != (*iteration_it)->target_head; iter = iter->prev)
						if(iter == merge_head){
							(*iteration_it)->target_tail->unsetLoc(PENDING_TAIL);
							(*iteration_it)->target_tail->setLoc(PENDING_MEMBER);
							pendingIterations.updateIterationTargetTail((*iteration_it), target_tail);
							(*iteration_it)->target_length += target_length;
							break;
						}

			} else {
				found_flag = 0;
				continue;
			}
		}


#if defined FEATURE_SIG_DIFF && FEATURE_SIG_DIFF > 0
		if(!target_tail->sigEquals(merge_tail)){
			target_sig = target_tail->getSignature();
			merge_sig = merge_tail->getSignature();
			search = target_tail;
			while(search != merge_tail){
				search->updateStatsSig(target_sig, merge_sig);
				search = search->prev;
			}
			target_tail->setSignature(*merge_sig);
		}
		if(target_tail != target_head && !target_head->sigEquals(merge_head)){
			target_sig = target_head->getSignature();
			merge_sig = merge_head->getSignature();
			search = target_tail;
			while(search != merge_tail){
				search->updateStatsSig(target_sig, merge_sig);
				search = search->prev;
			}
			target_head->setSignature(*merge_sig);
		}
#endif


		/* 2. if merge_head ... merge_tail is a pending iteration, merge it */
		iterations = pendingIterations.getIterationsByTargetHead(merge_head);
		rtn = NULL;
		for(iteration_it = iterations.begin(); iteration_it != iterations.end(); iteration_it++){
			if((*iteration_it)->target_head == merge_head && (*iteration_it)->target_tail == merge_tail){
				iteration = *iteration_it;
				rtn = compressLoopLCS(iteration);
				updateIds(rtn->merge_head);
				break;
			}
		}

		/* 3. mark the current iteration as pending */
		if(rtn){
			pendingIterations.create(rtn->merge_head, rtn->merge_tail, rtn->merge_length,
							target_head, target_tail, target_length);
			delete rtn;
		} else {
			pendingIterations.create(merge_head, merge_tail, merge_length, target_head, target_tail, target_length);
			for(iter = merge_tail; iter != merge_head && iter != NULL; iter=iter->prev)
				iter->setLoc(MEMBER);
			assert(iter);
			iter->setLoc(LEADER);
		}
		/* mark the events in the target iteration as PENDING */
		target_tail->setLoc(PENDING_TAIL);
		if(target_tail != target_head){
			for(iter = target_tail->prev; iter != target_head && iter != NULL; iter = iter->prev)
				iter->setLoc(PENDING_MEMBER);
		} 
		target_head->setLoc(PENDING_LEADER);

	} while(head_flag && !found_flag);

} /* slowCompressLoop */

void Trace::updateIds(Event* from){
	int base = from->prev ? from->prev->getId() + 1 : 0;
	Event* iter = from;
	while(iter){
		iter->setId(base);
		base++;
		iter = iter->next;
	}
}

/* this function doesn't really return the real length due to 
 * performance consideration. It returns the longer one of 
 * the merge iteration and the target iteration */
int Trace::realLengthHelper(iteration_t* iteration, int bound){
	Event *iter;
	int max;
	iteration_t *current;
	vector<iteration_t *> iterations;
	vector<iteration_t *>::iterator iteration_it;
	int target_real_length = 0, merge_real_length = 0;

	for(iter = iteration->target_head; iter != iteration->target_tail->next && iter != NULL; ){
		if(iter->checkLoc(LEADER)){
			iterations = pendingIterations.getIterationsByMergeHead(iter);
			max = INT_MIN;
			current = NULL;
			for(iteration_it = iterations.begin(); iteration_it != iterations.end(); ++iteration_it){
				if((*iteration_it)->merge_length > max){
					current = *iteration_it;
					max = (*iteration_it)->merge_length;
				}
			}
			if(current){
				iter = current->target_tail->next;
				target_real_length += realLengthHelper(current, current->merge_length);
				continue;
			}
		}
		iter = iter->next;
		target_real_length++;
	}

	/* check if there is pending iteration inside the merge iteration */
	for(iter = iteration->merge_head; iter != iteration->merge_tail->next && iter != NULL; ){
		if(iter->checkLoc(LEADER)){
			iterations = pendingIterations.getIterationsByMergeHead(iter);
			max = INT_MIN;
			current = NULL;
			for(iteration_it = iterations.begin(); iteration_it != iterations.end(); ++iteration_it){
				if((*iteration_it)->merge_length < bound && (*iteration_it)->merge_length > max){
					current = *iteration_it;
					max = (*iteration_it)->merge_length;
				}
			}
			if(current){
				iter = current->target_tail->next;
				merge_real_length += realLengthHelper(current, current->merge_length);
				continue;
			}
		}
		iter = iter->next;
		merge_real_length++;
	}

	return merge_real_length > target_real_length ? merge_real_length : target_real_length;
}

int Trace::realLength(Event* head, Event* tail){
	Event* iter;
	int max;
	int len = 0;
	vector<iteration_t *> iterations;
	vector<iteration_t *>::iterator iteration_it;
	iteration_t *current;
	for(iter = head; iter != NULL && iter != tail->next; ){
		if(iter->checkLoc(LEADER)){
			iterations = pendingIterations.getIterationsByMergeHead(iter);
			max = INT_MIN;
			for(iteration_it = iterations.begin(); iteration_it != iterations.end(); ++iteration_it){
				if((*iteration_it)->merge_length > max){
					current = *iteration_it;
					max = (*iteration_it)->merge_length;
				}
			}
			if(current){
				iter = current->target_tail->next;
				len += realLengthHelper(current, current->merge_length);
				continue;
			}
		}
		iter = iter->next;
		len++;
	}
	return len;
}



iteration_t* Trace::compressLoopLCS(iteration_t* iteration){
	Event *iter;
	int max, count, iteration_length;
	iteration_t *current, *rtn;
	vector<matching_pair_t *> *pairs = NULL;
	vector<iteration_t *> iterations;
	vector<iteration_t *>::iterator iteration_it;

	/* delete the current iteration from pendingIterations so that it won't be
	 * evaluated repeatedly */
	pendingIterations.deleteIteration(iteration);

	/* check if there is pending iteration inside the target iteration */
	for(iter = iteration->target_head; iter != iteration->target_tail->next && iter != NULL; iter = iter->next){
		if(iter->checkLoc(LEADER)){
			/* loops are either completely disjoint or perfectly embedded: if there is 
			 * a LEADER event (merge_head) in an iteration, the corresponding merge_tail, 
			 * target_head, target_tail must also be in the same iteration. Therefore, when
			 * we merge target_head ... target_tail into the previous iteration and remove
			 * them, we only have to handle the case where target_tail is also the last event
			 * of a larger iteration that directly or indirectly contains the two iterations 
			 * merge_head ... merge_tail target_head ... target_tail.
			 */
			iterations = pendingIterations.getIterationsByMergeHead(iter);
			max = INT_MIN;
			current = NULL;
			for(iteration_it = iterations.begin(); iteration_it != iterations.end(); ++iteration_it){
				if((*iteration_it)->merge_length > max){
					current = *iteration_it;
					max = (*iteration_it)->merge_length;
				}
			}
			if(current){
				if(iteration->target_tail == current->target_tail){
					rtn = compressLoopLCS(current);
					iteration->target_tail = rtn->merge_tail;
				} else {
					rtn = compressLoopLCS(current);
				}
				if(rtn)
					delete rtn;
			}
		}
	}

	/* check if there is pending iteration inside the merge iteration */
	for(iter = iteration->merge_head; iter != iteration->merge_tail->next && iter != NULL; iter = iter->next){
		if(iter->checkLoc(LEADER)){
			iterations = pendingIterations.getIterationsByMergeHead(iter);
			max = INT_MIN;
			current = NULL;
			for(iteration_it = iterations.begin(); iteration_it != iterations.end(); ++iteration_it){
				if((*iteration_it)->merge_length > max){
					current = *iteration_it;
					max = (*iteration_it)->merge_length;
				}
			}
			if(current){
				if(iteration->merge_tail == current->target_tail){
					rtn = compressLoopLCS(current);
					iteration->merge_tail = rtn->merge_tail;
				} else {
					rtn = compressLoopLCS(current);
				}
				if(rtn)
					delete rtn;
			}
		}
	}

	for(iter = iteration->target_head, count = 1; iter != iteration->target_tail; iter = iter->next, count++);
	iteration->target_length = count;
	for(iter = iteration->merge_head, count = 1; iter != iteration->merge_tail; iter = iter->next, count++);
	iteration->merge_length = count;

	pairs = matchLoopLCS(iteration->merge_head, iteration->merge_tail, iteration->merge_length, 
					iteration->target_head, iteration->target_tail, iteration->target_length );
	if(!pairs){
		assert(0);
	} else {
		iteration_length = updateLoopLCS(iteration->merge_head, iteration->merge_tail, 
					iteration->merge_length, iteration->target_head, iteration->target_tail, pairs);
		mergeLoopEvents(iteration->merge_head, iteration->merge_tail, 
					iteration->target_head, iteration->target_tail);
		updateLoopInfo(iteration->merge_head, iteration->merge_tail, iteration->target_head, 
					iteration->target_tail, iteration_length);
		deleteIteration(iteration->target_head, iteration->target_tail);
	}

	for(unsigned i=0; i<pairs->size(); i++)
		delete pairs->at(i);
	delete pairs;

	iteration->target_head = NULL;
	iteration->target_tail = NULL;
	iteration->target_length = 0;
	iteration->merge_length = iteration_length;
	return iteration;
}

void Trace::finalizePendingIterations(){
	Event *iter;
	vector<iteration_t *> iterations;
	vector<iteration_t *>::iterator iteration_it;
	iteration_t *current, *rtn;
	int max;

	for(iter = head; iter != NULL; iter = iter->next){
		if(iter->checkLoc(LEADER)){
			iterations = pendingIterations.getIterationsByMergeHead(iter);
			max = INT_MIN;
			current = NULL;
			rtn = NULL;
			for(iteration_it = iterations.begin(); iteration_it != iterations.end(); ++iteration_it){
				if((*iteration_it)->merge_length > max){
					current = *iteration_it;
					max = (*iteration_it)->merge_length;
				}
			}
			if(current)
				rtn = compressLoopLCS(current);
			if(rtn)
				delete rtn;
		}
	}
}

void Trace::cleanupLoops(){
	Event *iter = head;
	while(iter){
		iter->getLoopStack()->cleanup();
		iter = iter->next;
	}
}


vector<matching_pair_t *>* Trace::matchLoopLCS(Event* merge_head, Event* merge_tail, int merge_length,
						Event *target_head, Event* target_tail, int target_length){
	assert(merge_length > 0 && target_length > 0);

	/* calculate the longest common subsequence */
	vector<matching_pair_t *> *pairs;
	/* merge_head and target_head should always be in the LCS */
	if(merge_length == 1 && target_length == 1){
		pairs = new vector<matching_pair_t *>();
		matching_pair_t *pair = new matching_pair_t;
		pair->index1 = 0;
		pair->index2 = 0;
		pairs->push_back(pair);
		return pairs;
	} else if( (merge_length == 1 && target_length == 2) || (merge_length == 2 && target_length == 1)){
		/* should never reach here! */
		assert(0);
	} else {
		merge_head = merge_head->next;
		merge_length--;
		target_head = target_head->next;
		target_length--;
	}
	
	int width = target_length + 1;
	int *b, *c;

	b = (int *) calloc( (merge_length+1) * (target_length+1), sizeof(int) );
	if(!b){
		cerr << "calloc(" << (merge_length+1) * (target_length+1) * sizeof(int) << ") b failed" << endl;
		exit(1);
	}
	c = (int *) calloc( (merge_length+1) * (target_length+1), sizeof(int) );
	if(!c){
		cerr << "calloc(" << (merge_length+1) * (target_length+1) * sizeof(int) << ") c failed" << endl;
		exit(1);
    }
	Event *miter, *titer;
	int i, j;
	for(i=0, miter = merge_head; miter != merge_tail->next && miter != NULL; i++, miter = miter->next ){
		for(j=0, titer = target_head; titer != target_tail->next && titer != NULL; j++, titer = titer->next ){
			if(miter->sigMatch(titer) && miter->opMatch(titer) ){
				c[(i+1)*width + (j+1)] = c[i*width + j] + 1;
				b[(i+1)*width + (j+1)] = DIRECTION_UL;
			} else if(c[i*width + (j+1)] >= c[(i+1)*width + j]) {
				c[(i+1)*width + (j+1)] = c[i*width + (j+1)] ;
				b[(i+1)*width + (j+1)] = DIRECTION_UP;
			} else {
				c[(i+1)*width + (j+1)] = c[(i+1)*width + j] ;
				b[(i+1)*width + (j+1)] = DIRECTION_LEFT;
			}
		}
	}
	
	pairs = getLCS(b, merge_length, target_length);
	for(unsigned i=0; i<pairs->size(); i++){
		pairs->at(i)->index1++;
		pairs->at(i)->index2++;
	}
	matching_pair_t *pair = new matching_pair_t;
	pair->index1 = 0;
	pair->index2 = 0;
	pairs->insert(pairs->begin(), pair);
	free(b);
	free(c);
	return pairs;
} /* matchLoopLCS */



void Trace::insertEvent(Event *e, Event *position){
	if( !e || !position ){
		cerr << "error: cannot insert " << e << " after " << position << endl;
		exit(1);
	}
	e->prev = position;
	if(position->next){
		e->next = position->next;
		e->next->prev = e;
	}
	position->next = e;
}


int Trace::updateLoopLCS(Event* merge_head, Event* merge_tail, int merge_length, Event* target_head, Event* target_tail, vector<matching_pair_t *>* pairs){

	int i = 0, j = 0, mend_pos = 0, tend_pos = 0, tbegin_pos = 0, expand, mdepth, tdepth; 
	int n = pairs->size();
	Event *mbegin, *mend, *tbegin, *tend, *iter, *dummy, *temp;
	mbegin = mend = merge_head;
	tbegin = tend = target_head;
	bool changeMade = false;
	/* prerequisite: merge_head matches target_head, merge_tail matches target_tail */
	assert(pairs->size() > 0 && pairs->at(0)->index1 == 0 && pairs->at(0)->index2 == 0 );

	loop_t *outer;
	loophead_t* head;
	vector<loophead_t *> mloopheads;
	vector<loophead_t *> tloopheads;

	if(merge_head->getLoopDepth() > 1){ /* all events have one level of loop for themselves (mem:1 iter:1) */
		head = new loophead_t;
		head->event = merge_head;
		head->pos = mend_pos;
		mloopheads.push_back(head);
	}
	if(target_head->getLoopDepth() > 1){
		head = new loophead_t;
		head->event = target_head;
		head->pos = tend_pos;
		tloopheads.push_back(head);
	}

	for(int pair_iter = 0; pair_iter < n; pair_iter++){
		while(i < pairs->at(pair_iter)->index1){
			mend = mend->next;
			i++;
			mend_pos++;
			if(mend->getLoopDepth() > 1){
				head = new loophead_t;
				head->event = mend;
				head->pos = mend_pos;
				mloopheads.push_back(head);
			}
		}
		while(j < pairs->at(pair_iter)->index2){
			tend = tend->next;
			j++;
			tend_pos++;
			if(tend->getLoopDepth() > 1){
				head = new loophead_t;
				head->event = tend;
				head->pos = tend_pos;
				tloopheads.push_back(head);
			}
		}

		temp = mend->prev;
		if(tbegin != tend){
			/* mend->prev should always exist */
			//assert(mend->prev);
			expand = mend->getLoopStack()->getLoopAt(0)->mem->getNumValues(true);
			for(iter = tbegin; iter != tend; iter = iter->next){
				dummy = iter->getDummyEvent();
				/* expand the loop information by referring to the immediate following event: 
				 * assume the following event is E and the number of times E was called is n, 
				 * n equals to the (uncompressed) length of the mem/iter vector of the outer 
				 * most loop of E. If E starts loop structures, n equals to the times E was 
				 * called if there is no loop structures on E. Therefore, in either case, n
				 * will be the times the dummy event will be evaluated if inserted to the merge
				 * iteration */
				dummy->expandLoop(expand);
				insertEvent(dummy, mend->prev);
				mend_pos++;
				/* increase mem by one for loops that contain the inserted event */
				for(vector<loophead_t *>::iterator it = mloopheads.begin(); it != mloopheads.end(); ){
					changeMade = (*it)->event->getLoopStack()->incLoopMem(mend_pos - 1 - (*it)->pos);
					if(!changeMade)
						mloopheads.erase(it);
					else 
						it++;
				}
			}
		}
		if(mbegin != mend){
			/* tend->prev should always exist */
			assert(tbegin->prev);
			expand = tbegin->getLoopStack()->getLoopAt(0)->mem->getNumValues(true);
			for(iter = mbegin; iter != temp->next; iter = iter->next){
				dummy = iter->getDummyEvent();
				dummy->expandLoop(expand);
				insertEvent(dummy, tbegin->prev);
				tend_pos++;
				tbegin_pos++;
				/* increase mem by one for loops that contain the inserted event */
				for(vector<loophead_t *>::iterator it = tloopheads.begin(); it != tloopheads.end(); ){
					changeMade = (*it)->event->getLoopStack()->incLoopMem(tbegin_pos - (*it)->pos);
					if(!changeMade)
						tloopheads.erase(it);
					else 
						it++;
				}
			}
		}

		if(mend != merge_head && tend != target_head){
			mdepth = mend->getLoopDepth();
			tdepth = tend->getLoopDepth();
			if(mdepth > tdepth)
				tend->alignLoop(mdepth);
			else
				mend->alignLoop(tdepth);
		} else if(mend == merge_head && tend == target_head){
			/* heads are handled differently */
			/* check the outer most loop of merge_head, if it's a loop of the entire 
			 * iteration (mem == merge_length), align the rest of the loop nest with
			 * the loops on target_head, otherwise align all loops on merge_head with
			 * the loops on target_head */
			if(mend->getLoopStack()->getLoopAt(0)->mem->getNumValues(true) == 1
					&& mend->getLoopStack()->getLoopAt(0)->mem->getScalarValueForRank() == merge_length ){
				outer = mend->getLoopStack()->popOuterLoop();
			} else {
				outer = NULL;
			}
			mdepth = mend->getLoopDepth();
			tdepth = tend->getLoopDepth();
			if(mdepth > tdepth)
				tend->alignLoop(mdepth);
			else
				mend->alignLoop(tdepth);
			if(outer)
				mend->getLoopStack()->insertLoop(outer);
		} else {
			assert(0); /* should never reach here!! */
		}

		mbegin = mend->next;
		tbegin = tend->next;
		tbegin_pos = tend_pos;
	}

	mloopheads.clear();
	tloopheads.clear();

	return mend_pos + 1;
}

void Trace::deleteIteration(Event* target_head, Event* target_tail){
	if(target_head->prev){
		target_head->prev->next = target_tail->next;
	}
	if(target_tail->next){
		target_tail->next->prev = target_head->prev;
	}
	if(target_tail == tail){
		tail = target_head->prev;
	}
	target_head->prev = NULL;
	target_tail->next = NULL;

	Event *iter = target_tail, *prev;
	while(iter){
		prev = iter->prev;
		delete iter;
		iter = prev;
	}
}




void Trace::resetTraversalIterators(){
	Event *iter = head;
	while(iter){
		iter->resetTraversalIterators();
		iter = iter->next;
	}
}

void Trace::appendEvent(Event* event, bool compress){
	/*This condition helps non-representatives to skip tracing*/
	if(getNoClusteringFlag()){
			if (onlyHeadCluster(getMyRank())==-1){
			//	cout << "Tracing is off" << endl;
				return;
			}
	}
	
	if(!event)
		return;
#ifdef ONLINE_CLUSTERING
	incNumEvents();
	event->setOnlineClusteringFlagON();	

#endif
	if(head == NULL){
		event->setId(0);
		head = tail = event;
	} else {
		
		event->setId(tail->getId()+1);
		tail->next = event;
		event->prev = tail;
		tail = event;
	}

	if(compress){
#ifndef FEATURE_LOOP_LCS
		fastCompressLoop(WINDOW_SIZE);
		//cout << "fastCompressLoop" << endl;
#else
		slowCompressLoop(WINDOW_SIZE);
		//cout << "slowCompressLoop" << endl;
#endif
	}
}



void Trace::appendEventOnline(Event* event, bool compress){ //, Event* slaveHead, Event* slaveTail){
	if(!event)
		return;

	if(head == NULL){
		event->setId(0);
		head = tail = event;
	} else {
		
		event->setId(tail->getId()+1);
		tail->next = event;
		event->prev = tail;
		tail = event;
	}

	if(compress){
#ifndef FEATURE_LOOP_LCS
		fastCompressLoop(WINDOW_SIZE);
		//onlineFastCompressLoop(WINDOW_SIZE);
		//cout << "fastCompressLoop" << endl;
#else
		slowCompressLoop(WINDOW_SIZE);
		//cout << "slowCompressLoop" << endl;
#endif
	}


}

	void Trace::outputSignature(const char* filename){

		char sigfilename[10000];
		sprintf(sigfilename, "%s%s", filename, "Sig");

		ofstream sigfile(sigfilename);


		if(!sigfile ){
			cerr << "error: unable to open trace file: " << sigfilename;
			return;
		}


		sigfile << "\n--> Size Ref Signature: "<< getRefSize() << "\n";
		sigfile << "\n--> Parameter Signature: "<< hex <<getParameterSignature() << "\n";
		sigfile << "\n--> CallPath Signature: "<< hex <<getCallPathSignature() << "\n";

		map<int, map<int, long long int> > tempRef = getReferenceSignature();  	
		map<int, map<int, long long int> >::iterator it;
		map<int, long long int>::iterator inner_it;

		for ( it=tempRef.begin() ; it != tempRef.end(); it++ ) {
			sigfile << "\n\nNew element\n" << (*it).first << endl;
			for( inner_it=(*it).second.begin(); inner_it != (*it).second.end(); inner_it++)
				sigfile << (*inner_it).first << " => " << (*inner_it).second << endl;
		}
		sigfile.close();

	}

	void Trace::initMyCluster(int rank){ 
		myCluster =  new Cluster();
		myCluster->initClusterRanklist(rank);

		myCluster->setCallPath(tempCluster->getCallPath());
		myCluster->setAvgCount(tempCluster->getAvgCount());
		myCluster->setAvgCommTime(tempCluster->getAvgCommTime());
		myCluster->setAvgCompTime(tempCluster->getAvgCompTime());
		myCluster->setAvgSRC(tempCluster->getAvgSRC());
		myCluster->setAvgDEST(tempCluster->getAvgDEST());
		myCluster->setAvgTAG(tempCluster->getAvgTAG());
		myCluster->setAvgCOLOR(tempCluster->getAvgCOLOR());
		myCluster->setAvgKEY(tempCluster->getAvgKEY());
		myCluster->setDATA_OP(tempCluster->getDATA_OP());
		myCluster->setAvgLoopIter(tempCluster->getAvgLoopIter());
		
		delete tempCluster;

	}


void Trace::createSignature(int rank, int comm_size){
		Event *iter = head;
		Event *prev_event = NULL;
		StackSig* prev_sig = NULL;
		/*init your Cluster*/
//		initMyCluster(rank);
	
		tempCluster =  new Cluster();
		tempCluster->initClusterRanklist(rank);

		unsigned int seq_num = 0; /*Number of Events*/
		unsigned int tmp_mod_seq_num = 1;
		long long CallPath_Sig=0; /*CallPath Signature 64b*/
		long long AvgSRC=0;
		long long AvgDEST=0;
		long long AvgTAG=0;
		long long AvgCOLOR=0;
		long long AvgKEY=0;
		long long DATA_OP=0;

		vector<long long> CompTimeV;
		vector<long long> CommTimeV;
		vector<long long> CountV;
		vector<long long> LoopIterL0V;
		vector<long long> LoopIterL1V;
		vector<long long> LoopIterL2V;
		vector<long long> LoopIterL3V;

		long long AvgCompTime=0;
		long long AvgCommTime=0;
		long long AvgCount=0; 
		long long AvgLoopIter=0;


		set<long long> COUNT_Set;
		set<long long> SRC_Set;
		set<long long> DEST_Set;
		set<long long int> TAG_Set;
		set<long long int> KEY_Set;
		set<long long int> COLOR_Set;
		set<long long>::iterator itSet;	
		set<long long>::iterator itSet2;

		long long int tempSRC=0;
		long long int tempDEST=0;
		long long int tempTAG=0;
		long long int tempKEY=0;
		long long int tempCOLOR=0;
		long long int tempDataCommOP=0;
		long long int tempCompTime=0;
		long long int tempCommTime=0;
		long long int temp_AvgCount=0; 
		//unsigned int numCount=0; /*Number of events contain Count*/
		//unsigned int numLoop=0;

		//unsigned long L1=0;
		//unsigned long L2=0;
		//unsigned long L3=0;
		//unsigned long L0=0;


		map<int, map<int, long long int> > Ref_Sig;
		//int Ref_Size=0;	

		vector<int> iterMem;
		vector<int> iterLoop;

	while(iter){
		if(iter->getOnlineClusteringFlag()){
			seq_num++;
			tmp_mod_seq_num = (seq_num % 10)+1;
			// Call-Path Signature
			CallPath_Sig=CallPath_Sig ^ (iter->getCallPathSignature()* tmp_mod_seq_num);
			if(prev_event)
				prev_sig = prev_event->getSignature();

			iter->getParameterSignature(temp_AvgCount, tempCompTime, tempCommTime, tempDataCommOP, tempSRC, tempDEST, tempTAG, tempKEY, tempCOLOR, prev_sig);

			CompTimeV.push_back(iter->getLastCallValue());
			CommTimeV.push_back(tempCommTime);

			SRC_Set.insert(tempSRC);
			DEST_Set.insert(tempDEST);
			COUNT_Set.insert(temp_AvgCount);
			CountV.push_back(temp_AvgCount);

		}
			prev_event= iter;
			iter = iter->next;
	}


	unsigned int index=0;
	if (CountV.size()>0){
		//AvgCount= calcAverage<long long>(CountV);
		for (std::set<long long>::iterator it = COUNT_Set.begin(); it != COUNT_Set.end(); ++it) {
		    AvgCount += *it;
		}
	}

	if(CompTimeV.size()>0){
		for (std::vector<long long>::iterator it = CompTimeV.begin(); it != CompTimeV.end(); ++it) {
		    AvgCompTime += *it;
		    index++;
		} 
	//	AvgCompTime = calcAverage<long long>(CompTimeV);
	}

	if(CommTimeV.size()>0){
		for (std::vector<long long>::iterator it = CommTimeV.begin(); it != CommTimeV.end(); ++it) {
		    AvgCommTime += *it;
		} 
		//AvgCommTime = calcAverage<long long>(CommTimeV);
	}

	if (SRC_Set.size()>0){
		vector<long long> input_AvgSRC(SRC_Set.begin(), SRC_Set.end());
		AvgSRC = calcAverage<long long>(input_AvgSRC);		
	}

	if (DEST_Set.size()>0){
		vector<long long> input_AvgDEST(DEST_Set.begin(), DEST_Set.end());
		AvgDEST = calcAverage<long long>(input_AvgDEST);		
	}
	tempCluster->setCallPath(CallPath_Sig);
	tempCluster->setAvgCount(AvgCount);
	tempCluster->setAvgCommTime(AvgCommTime);
	tempCluster->setAvgCompTime(getDeltaTime()); //AvgCompTime);
	tempCluster->setAvgSRC(AvgSRC);
	tempCluster->setAvgDEST(AvgDEST);
	tempCluster->setAvgTAG(AvgTAG);
	tempCluster->setAvgCOLOR(AvgCOLOR);
	tempCluster->setAvgKEY(AvgKEY);
	tempCluster->setDATA_OP(DATA_OP);
	tempCluster->setAvgLoopIter(AvgLoopIter);

	setParameterSignature(AvgCount);
	setCallPathSignature(CallPath_Sig);
	setAvgCountSignature(AvgCount);
	//setDeltaTime(AvgCompTime);
	setReferenceSignature(Ref_Sig);
	
}
template<typename T> 
T Trace::findUniqueSig(vector<T> val){

	T hash;

	if (val.size()==1)
		return  val.at(0);

	if(val.size()%2){
		T tmp= val.back();
		val.pop_back();
		hash = findAverage(val);
		hash = (hash*2 + tmp)/3;
	}
	else{
		hash = findAverage(val);
	}

	return hash;
}

void Trace::outputTrace(const char* filename){
	ofstream tracefile(filename);
	if(!tracefile ){
		cerr << "error: (output) unable to open trace file: " << filename;
		return;
	}
	Event *iter = head;
	unsigned int seq_num = 0;
	while(iter){
		tracefile << iter->toString(seq_num++) << "\n";
		iter = iter->next;
	}
	tracefile.close();
}


void Trace::outputOnlineTrace(const char* filename, Event * head){
	ofstream tracefile(filename);
	if(!tracefile ){
		cerr << "error: unable to open trace file: " << filename;
		return;
	}
	Event *iter = head;
	unsigned int seq_num = 0; /*Event Sequence Number*/
	while(iter){
		tracefile << iter->toString(seq_num++) << "\n";
		iter = iter->next;
	}
	tracefile.close();
}

void Trace::callSignatures(const char* filename, int rank, int comm_size)
{
	createSignature(rank, comm_size);	
	/*to print signatures uncomment the following function*/
	//outputSignature(filename);
}

Trace* Trace::inputTrace(const char* filename, int mode, int rank){
	string line, eventbuf, bufsize;
	ifstream tracefile(filename);
	if(!tracefile){
		cerr << "error: (input) unable to open trace file: " << filename << endl;
		exit(1);
	}
	Trace *trace = new Trace();
	int seqnum = 0;


	while(getline(tracefile, line)){
		if(line.size() > 0){
			eventbuf += line + "\n";
		} else {
			Event *e = Event::inputEvent(eventbuf, mode, rank);
			if(e){
				e->setId(seqnum++);
				trace->appendEvent(e, false);
			}
			eventbuf.clear();
		}
	}
	tracefile.close();
	return trace;
}


void Trace::printTrace(){
	Event *iter = head;
	while(iter){
		cout << iter->toString() << "\n";
		iter = iter->next;
	}

}

/*This function set all the flag of all old events we considered in the clustering off.
Therefore, we can set their flag on again if there is a same event adding in the future!
*/
void Trace::setOFFOnlineClusteringFlags(int rank){
	if (head ==NULL)
		return;
	Event *iter = head;
	int numFlaggedEvents=0, numEvents=0;
	while(iter){
		if(iter->getOnlineClusteringFlag()){
			iter->setOnlineClusteringFlagOFF();
			numFlaggedEvents++;
		}
		iter = iter->next;
		numEvents++;
	}

}


int Trace::getPackedSize(){
	int size = 0;
	size += sizeof(int); /* num of events */
	Event *iter = head;
	while(iter){
		size += iter->getPackedSize();
		iter = iter->next;
	}
	return size;
}

void Trace::pack(void *buf, int bufsize, int *position, MPI_Comm comm){
	int len = traceLen();
	PMPI_Pack(&len, 1, MPI_INT, buf, bufsize, position, comm);
	Event *iter = head;
	while(iter){
		iter->pack(buf, bufsize, position, comm);
		iter = iter->next;
	}
}

void Trace::unpack(void *buf, int bufsize, int *position, MPI_Comm comm){
	int len;
	PMPI_Unpack(buf, bufsize, position, &len, 1, MPI_INT, comm);
	for(int i=0; i<len; i++){
		Event *e = new Event();
		e->unpack(buf, bufsize, position, comm);
		appendEvent(e, false);
	}
}

void Trace::sendTrace(int dest){
	int bufsize = getPackedSize();
	void *buf = malloc(bufsize);
	MPI_Comm comm = MPI_COMM_WORLD;
	int position = 0;
	if(!buf){
		cerr << "Trace::sendTrace(): cannot allocate buffer" << endl;
		exit(1);
	}
	PMPI_Send(&bufsize, 1, MPI_INT, dest, 0, comm);
	pack(buf, bufsize, &position, comm);
	PMPI_Send(buf, position, MPI_PACKED, dest, 0, comm);
	free(buf);
}

void Trace::recvTrace(int src){
	int bufsize;
	void *buf;
	MPI_Comm comm = MPI_COMM_WORLD;
	int position = 0;

	PMPI_Recv(&bufsize, 1, MPI_INT, src, 0, comm, MPI_STATUS_IGNORE);
	buf = malloc(bufsize);
	if(!buf){
		cerr << "Trace::recvTrace(): cannot allocate buffer" << endl;
		exit(1);
	}
	PMPI_Recv(buf, bufsize, MPI_PACKED, src, 0, comm, MPI_STATUS_IGNORE);
	unpack(buf, bufsize, &position, comm);
	free(buf);
}


void Trace::get_lcs_helper(int *b, int i, int j, int width, vector<matching_pair_t *> *pairs){
	if(i == 0 || j == 0)
		return;
	if(b[i*width+j] == DIRECTION_UL){
		get_lcs_helper(b, i-1, j-1, width, pairs);
		matching_pair_t *pair = new matching_pair_t;
		pair->index1 = i-1;
		pair->index2 = j-1;
		pairs->push_back(pair);
		return;
	} else if (b[i*width+j] == DIRECTION_UP){
		get_lcs_helper(b, i-1, j, width, pairs);
		return ;
	} else {
		get_lcs_helper(b, i, j-1, width, pairs);
		return ;
	}
}

vector<matching_pair_t *> * Trace::getLCS(int *b, int mlen, int slen){
	vector<matching_pair_t *> *pairs = new vector<matching_pair_t *>();
	get_lcs_helper(b, mlen, slen, slen+1, pairs);
	return pairs;
}


void Trace::LCSDoMerge(vector<matching_pair_t *> *pairs, Trace *slave){
	int i = 0, j = 0; 
	int n = pairs->size();
	Event *mend, *sbegin, *send, *temp;
	mend = head;
	sbegin = send = slave->getHead();

	/*Assumption that both slave and master traces have the same satrt points is not correct*/
	//assert(pairs->size()>0 && pairs->at(0)->index1 == 0 && pairs->at(0)->index2 == 0 );

	for(int pair_iter = 0; pair_iter < n; pair_iter++){
		while(i < pairs->at(pair_iter)->index1){
			mend = mend->next;
			i++;
		}
		while(j < pairs->at(pair_iter)->index2){
			send = send->next;
			j++;
		}
		if(sbegin != send){
			temp = send->prev;
			if(sbegin->prev){
				sbegin->prev->next = send;
				send->prev = sbegin->prev;
			} else {
				slave->setHead(send);
				send->prev = NULL;
			}

			if(mend->prev){
				sbegin->prev = mend->prev;
				mend->prev->next = sbegin;
			} else {
				head = sbegin;
				sbegin->prev = NULL;
			}
			mend->prev = temp;
			temp->next = mend;
		}
		if(mend->getSignature()!=send->getSignature()){
			mend->eventRanklistMerge(send);
			mend->eventParamMerge(send);
			mend->eventStatsMerge(send);
			mend->getLoopStack()->interMergeLoopStack(send->getLoopStack());
		}
		//mbegin = mend->next;
		sbegin = send->next;
	}

}
void Trace::onlineLCSDoMerge(vector<matching_pair_t *> *pairs, Trace *slave, int clusteringFlag){
	int i = 0, j = 0; 
	int n = pairs->size();
	Event *mend, *sbegin, *send, *temp;
	mend = head;
	sbegin = send = slave->getHead();


	/*Assumption that both slave and master traces have the same satrt points is not correct*/
	//assert(pairs->size()>0 && pairs->at(0)->index1 == 0 && pairs->at(0)->index2 == 0 );

	for(int pair_iter = 0; pair_iter < n; pair_iter++){
	
		while(i < pairs->at(pair_iter)->index1){
			mend = mend->next;
			i++;
		}
		while(j < pairs->at(pair_iter)->index2){
			send = send->next;
			j++;
		}
		if(clusteringFlag == 0){
			if (pair_iter == 0){
			        cout << "========================= Head of Merge  ===============================" << endl;
			        cout << mend->toString() << endl;
			        cout << send->toString() << endl;
			}
			else if(pair_iter == n-1){
			        cout << "========================= End of Merge  ===============================" << endl;
			        cout << mend->toString() << endl;
			        cout << send->toString() << endl;
			}
		}

		if(sbegin != send){

			temp = send->prev;
			if(sbegin->prev){
				sbegin->prev->next = send;
				send->prev = sbegin->prev;
			} else {
				slave->setHead(send);
				send->prev = NULL;
			}

			if(mend->prev){
				sbegin->prev = mend->prev;
				mend->prev->next = sbegin;
			} else {
				head = sbegin;
				sbegin->prev = NULL;
			}
			mend->prev = temp;
			temp->next = mend;
		}
		if(mend->getSignature()!=send->getSignature()){

	
			mend->eventRanklistMerge(send);
			mend->eventParamMerge(send);
			mend->eventStatsMerge(send);
			
			mend->getLoopStack()->interMergeLoopStack(send->getLoopStack());



			//mend->getLoopStack()->mergeLoopStack(send->getLoopStack());
			//In case of Finalized then consider Online trace stack
		/*	if(clusteringFlag==-2){
				mend->getLoopStack()->onlineMergeLoopStack(send->getLoopStack());
				if(clusteringFlag == -2){	
					cout << "mend->getSignature() != send->getSignature()" << endl;
					cout << mend->toString()<< endl;
				}
			}
			else{


					if(mend->getOp()==1113){
					cout << "<====mend1113====>" << endl;	
					cout << "Before Merge:" << endl;
					cout << mend->toString() << endl;	
					cout << "<====send1113====>" << endl;	
					cout << send->toString() << endl;	
				}
			
				//mend->getLoopStack()->interMergeLoopStack(send->getLoopStack());
				mend->getLoopStack()->mergeLoopStack(send->getLoopStack());
	
				if (pair_iter == 0){
				        cout << "========================= After Head of Merge  ===============================" << endl;
				        cout << mend->toString() << endl;
				        cout << send->toString() << endl;
				}

				if(mend->getOp()==1113){
					cout << "mend After Merge:" << endl;
					cout << mend->toString() << endl;	
				}

			}
			*/

		}
		else{
			if(clusteringFlag == -2){	
				cout << "mend->getSignature() == send->getSignature()" << endl;
				cout << send->toString()<< endl;
			}

		}

//		mbegin = mend->next;
		sbegin = send->next;
	}

}

void Trace::LinearMerge(Trace *slave){
	Event *mend, *send;
	mend = head;
	send = slave->getHead();

	while (mend!=NULL)
	{
		mend->eventRanklistMerge(send);
		mend->eventParamMerge(send);
		mend->eventStatsMerge(send);
		mend->getLoopStack()->interMergeLoopStack(send->getLoopStack());
		mend = mend->next;
		send = send->next;
	}
}

void Trace::LCSMergeTrace(Trace *slave){
	int mlen = traceLen();
	int slen = slave->traceLen();
	int width = slen + 1;
	int *b, *c;

	b = (int *) calloc( (mlen+1) * (slen+1), sizeof(int) );
	if(!b){
		fprintf(stderr, "calloc(%ld) b failed\n", (mlen+1) * (slen+1) * sizeof(int));
		exit(1);
	}
	c = (int *) calloc( (mlen+1) * (slen+1), sizeof(int) );
	if(!c){
		fprintf(stderr, "calloc(%ld) c failed\n", (mlen+1) * (slen+1) * sizeof(int));
		exit(1);
	}
	Event *miter, *siter;
	int i, j;
	for(i=0, miter = head; miter != NULL; i++, miter = miter->next ){
		for(j=0, siter = slave->getHead(); siter != NULL; j++, siter = siter->next ){
			if(miter->sigEquals(siter)){
				c[(i+1)*width + (j+1)] = c[i*width + j] + 1;
				b[(i+1)*width + (j+1)] = DIRECTION_UL;
			} else if(c[i*width + (j+1)] >= c[(i+1)*width + j]) {
				c[(i+1)*width + (j+1)] = c[i*width + (j+1)] ;
				b[(i+1)*width + (j+1)] = DIRECTION_UP;
			} else {
				c[(i+1)*width + (j+1)] = c[(i+1)*width + j] ;
				b[(i+1)*width + (j+1)] = DIRECTION_LEFT;
			}
		}
	}

	vector<matching_pair_t *> *pairs = getLCS(b, mlen, slen);
	
	if( pairs->size() != 0)
		LCSDoMerge(pairs, slave);
	else{

		slave->getTail()->next = head;
		head->prev = slave->getTail();
		head = slave->getHead();
	}
	
	for(unsigned i=0; i<pairs->size(); i++)
		delete pairs->at(i);
	delete pairs;
	free(b);
	free(c);
}

void Trace::onlineLCSMergeTrace(Trace *slave, int clusteringFlag){
	int mlen = traceLen();
	int slen = slave->traceLen();

/*	cout << "LCS MERGE Master Trace len: " << mlen << endl;
	cout << "LCS MERGE Slave Trace len: " << slen << endl;

	cout << "========================= Start ONLINE Trace (Slave) Before Merge: ===============================" << endl;
	slave->printTrace();
	cout << "========================= End ONLINE Trace (Slave) Before Merge ===============================" << endl;
	cout << "========================= Start Master TRACE Before Merge: ===============================" << endl;
	printTrace();
	cout << "========================= End Master TRACE Before Merge ===============================" << endl;
*/



	int width = slen + 1;
	int *b, *c;

	b = (int *) calloc( (mlen+1) * (slen+1), sizeof(int) );
	if(!b){
		fprintf(stderr, "calloc(%ld) b failed\n", (mlen+1) * (slen+1) * sizeof(int));
		exit(1);
	}
	c = (int *) calloc( (mlen+1) * (slen+1), sizeof(int) );
	if(!c){
		fprintf(stderr, "calloc(%ld) c failed\n", (mlen+1) * (slen+1) * sizeof(int));
		exit(1);
	}
	Event *miter, *siter;
	int i, j;
	for(i=0, miter = head; miter != NULL; i++, miter = miter->next ){
		for(j=0, siter = slave->getHead(); siter != NULL; j++, siter = siter->next ){
			if(miter->sigEquals(siter)){
				c[(i+1)*width + (j+1)] = c[i*width + j] + 1;
				b[(i+1)*width + (j+1)] = DIRECTION_UL;
			} else if(c[i*width + (j+1)] >= c[(i+1)*width + j]) {
				c[(i+1)*width + (j+1)] = c[i*width + (j+1)] ;
				b[(i+1)*width + (j+1)] = DIRECTION_UP;
			} else {
				c[(i+1)*width + (j+1)] = c[(i+1)*width + j] ;
				b[(i+1)*width + (j+1)] = DIRECTION_LEFT;
			}
		}
	}

	vector<matching_pair_t *> *pairs = getLCS(b, mlen, slen);

	/*TODO: check if there is only one event overlap!*/
	if( pairs->size() > 0){
			//cout << "Complete Match!" << endl;
			//Exchange the traces 
			Event * tmpHead = slave->getHead();
			Event * tmpTail = slave->getTail();
			slave->setHead(head);
			slave->setTail(tail);
			head = tmpHead;
			tail = tmpTail;
		
			Event * tmp = slave->getHead();
        
			while(tmp!=NULL){

				appendEventOnline(tmp, false);
				tmp=tmp->next;

			}
	}
	else{
		slave->getTail()->next = head;
		head->prev = slave->getTail();
		head = slave->getHead();
	}



	for(unsigned i=0; i<pairs->size(); i++)
		delete pairs->at(i);


	delete pairs;
	free(b);
	free(c);
}





#ifdef FEATURE_CODE_GENERATOR
/*
 * Definition: Traverse till all the events that are part of loop are visited
 * and returns last visited event.
 * Parmeters: loophead[input]: head of loop
 * depth [input]
 * */
Event* Trace::getLoopTail(Event* loophead, int depth){
  loop_t *loop;
  vector<ValueSet *> *mems;
  Loop* loopstack; 

  if(loophead == NULL){
    cerr << __func__ << "loophead is null";
    return NULL;
  }
  loopstack = loophead->getLoopStack();
  loop = loopstack->getLoopAt(depth);
  if(loop == NULL){
    cerr << __func__ << "No loop at depth:" << depth;
    return NULL;
  }

  mems = loop->mem->getParamValues();

  Event* looptail = NULL;
  for(int idx = 0; idx < mems->size(); idx++){
    //get member count vector
    ParamValue* param_value = (ParamValue*) (*mems)[idx];
    vector<int>* mem_cnts = param_value->getValues();
    //iterate over member count vector
    for(int mem_cnt_idx = 0; mem_cnt_idx < mem_cnts->size(); mem_cnt_idx++){
      int mem_cnt = (*mem_cnts)[mem_cnt_idx];

      Ranklist* mem_rl = param_value->getRanklist();
      int num_ranks = mem_rl->getNumRanks();
      int* mem_ranks = (int*)malloc(sizeof(int)*num_ranks);

      mem_rl->decompressRanks(mem_ranks);

      //iterate over member ranklist
      for(int mem_rank_idx = 0; mem_rank_idx < num_ranks; mem_rank_idx++){
	int cur_rank = mem_ranks[mem_rank_idx];
	int cur_cnt = 0;
	Event* cur_iter = loophead;
	while(cur_cnt < mem_cnt){
	  if(cur_iter->hasMember(cur_rank)){
	    cur_cnt++;
	    if(looptail == NULL ||
	       looptail->getId() < cur_iter->getId()){
	      looptail = cur_iter;
	    }
	  }
	  cur_iter = cur_iter->next;
	}
      }
      free(mem_ranks);
    }
  }

  return looptail;
}

/*
 *Definition: Derived from traverse function of Trace. 
 *Used for benchmark code generator.
 */
bool Trace::code_gen_traverse(event_handler handler){
  /*
   * First event(MPI_Init) can't have loop, 
   * hence passing 0 for both depth and loopdepth.
   * */
  return code_gen_traverse_recursive(head, 0, handler, NULL, 0);
}

/*
 *Definition: Derived from traverse_recursive function of Trace. 
 *Traverse the trace and writes for loops.
 *Parameters:  loophead[input] :head i.e. first event of loop
 *                depth[input] :current depth in loopstack of loophead,
 *                starting from 0.
 *              handler[input] :callback function
 *                  len[output]:length of the loop
 *            loopdepth[input] :nested loopdepth of loophead, starting from 1.
 *                              Differs from "depth", if current event is 
 *                              part of an outer loop but not it's head.
 *                              If there is one outer loop and one inner loop,
 *                              Depth would be 0, but loopdepth would be 2.
 */
bool Trace::code_gen_traverse_recursive(Event* loophead, int depth, 
					event_handler handler, int *len, int loopdepth){
  if( loophead == NULL){
    cerr << __func__ << " called with NULL loophead, depth = " << depth << endl;
    return false;
  }
  if(handler == NULL){
    cerr << __func__ << " called without event handler" << endl;
    return false;
  }

  
  Event *looptail, *event_iter;
  Loop* loopstack = loophead->getLoopStack();
  if(loopstack->getDepth() == 0){
    looptail = tail;
  } else if( 0 <= depth && depth < loopstack->getDepth() ){
    looptail = getLoopTail(loophead, depth);
    assert(looptail != NULL);
#ifdef DEBUG
    cout << __func__ << "LoopHead event number " << loophead->getId()  
	 << " current LoopTail event number" << looptail->getId() << "\n"; 
#endif
  } else {
    cerr << __func__ << ": invalid depth: loophead: \n" << loophead->toString() << " depth = " << depth << endl;
    return false;
  }

  bool done ;

  event_iter = loophead;
  done = false;

  if(is_generate_loop(loophead, depth))
    {
      generate_loop_start_code(loophead, depth);
    }

  while(!done){
    if(event_iter == NULL){
      cerr << __func__ << ": NULL event" << endl;
      exit(1);
    }

    /*Case 1: event_iter is loop head of current loop and has nested loop*/
    if(event_iter == loophead && event_iter->getLoopStack()->getDepth() > (depth + 1)){
      int length = 0;
      if( !code_gen_traverse_recursive(event_iter, depth + 1, handler, &length, loopdepth + 1) ){
	return false;
      }
      for(int j=1; j<length; j++)
	event_iter = event_iter->next;
      //note: inner loop events may cross outer loop events
      if(event_iter->getId() >= looptail->getId()){
	done = true;
	looptail = event_iter;
      }
      event_iter = event_iter->next;
    } else if(event_iter != NULL && event_iter != loophead
	      && event_iter->getLoopStack()->getDepth() > 0){
      /*Case 2: event_iter isn't loop head of current loop but has a loop with it*/
      int length = 0;
      if( !code_gen_traverse_recursive(event_iter, 0, handler, &length, loopdepth + 1) ){
	return false;
      }
      for(int j=1; j<length; j++)
	event_iter = event_iter->next;
      //note: inner loop events may cross outer loop events
      if(event_iter->getId() >= looptail->getId()){
	done = true;
	looptail = event_iter;
      }
      event_iter = event_iter->next;
    } else if (event_iter != NULL){
      /*Case 3: event_iter is neither loop head nor has any loop */
      if(event_iter == looptail)
	done = true;

      /* calling call back registered to generate code */
      handler(event_iter, depth, 0);
      event_iter = event_iter->next;
    }
  }
  /*calculate length of the loop*/
  if(len){
    *len = 1;
    Event* temp_iter = loophead;
    while(temp_iter != looptail){
      (*len)++;
      temp_iter = temp_iter->next;
    }
#ifdef DEBUG
    cout << __func__ << "LoopHead event number " << loophead->getId()  
	 << " updated LoopTail event number " << looptail->getId()
	 << " loop length " << *len << "\n";
#endif
  }
  if( is_generate_loop(loophead, depth) )
    {
      generate_loop_end_code();
    }

  return true;
}
#endif /*FEATURE_CODE_GENERATOR*/

bool Trace::traverse(event_handler handler){
	return traverse_recursive(head, 0, handler, NULL);
}

bool Trace::traverse_recursive(Event* loophead, int depth, event_handler handler, int *len){
	if( loophead == NULL){
		cerr << "traverse_recursive() called with NULL loophead, depth = " << depth << endl;
		return false;
	}
	if(handler == NULL){
		cerr << "traverse_recursive() called without event handler" << endl;
		return false;
	}

	Event *looptail, *event_iter;
	Loop* loopstack = loophead->getLoopStack();
	loop_t *loop;
	vector<ValueSet *> *mems = NULL, *iters = NULL;
	int iter = 0, mem = 0, length;
	if(loopstack->getDepth() == 0){
		looptail = tail;
		iter = 1;
	} else if( 0 <= depth && depth < loopstack->getDepth() ){
		loop = loopstack->getLoopAt(depth);
		mems = loop->mem->getParamValues();
		iters = loop->iter->getParamValues();
		/* trace reader ensures that only the loop rlparam related to the current node is read */
		assert(mems->size() == 1);
		assert(iters->size() == 1);
		if(typeid(*(mems->at(0))) == typeid(Histogram) || typeid(*(iters->at(0))) == typeid(Histogram)){
			cerr << "traverse_recursive() doesn't support histogram trace: \n" << loophead->toString();
			return false;
		}
		mem = dynamic_cast<ParamValue *>(mems->at(0) )->getCurrentValue(true);
		iter = dynamic_cast<ParamValue *>(iters->at(0) )->getCurrentValue(true);
		looptail = loophead;
		for(int i=1; i<mem; i++)
			looptail = looptail->next;
		if(len)
			*len = mem;
	} else {
		cerr << "invalid depth: loophead: \n" << loophead->toString() << " depth = " << depth << endl;
		return false;
	}

	bool done ;
	for(int i=0; i<iter; i++){
		event_iter = loophead;
		done = false;
		while(!done){
			if(event_iter == NULL){
				cerr << "NULL event" << endl;
				exit(1);
			}

			if(event_iter == loophead && event_iter->getLoopStack()->getDepth() > depth+1){
				if( !traverse_recursive(event_iter, depth+1, handler, &length) ){
					return false;
				}
				for(int j=1; j<length; j++)
					event_iter = event_iter->next;
				if(event_iter == looptail)
					done = true;
				event_iter = event_iter->next;
			} 

			if(done)
				break;

			if(event_iter != NULL && event_iter != loophead 
					&& event_iter->getLoopStack()->getDepth() > 0){
				if( !traverse_recursive(event_iter, 0, handler, &length) ){
					return false;
				}
				for(int j=1; j<length; j++)
					event_iter = event_iter->next;
				if(event_iter == looptail)
					done = true;
				event_iter = event_iter->next;
			} else if (event_iter != NULL){
				if(event_iter == looptail)
					done = true;
				handler(event_iter, depth, i);
				event_iter = event_iter->next;
			}
		}
	}
	return true;

}
void Trace::addCluster(){
	/*Add the cluster to the list of clusters- Initialization for leaf nodes*/	
	int clusteringType=getClusteringType();
	if (clusteringType <= CALL_PATH_PARAMETER_CLUSTERING)
	{
		while (!clusters.empty())
		{
			clusters.erase(clusters.begin());
		}

		while (!recv_clusters.empty())
		{
			recv_clusters.erase(recv_clusters.begin());
		}

		if (clusteringType==0)
			clusters[getCallPathSignature()]=getRanklist();
		else if (clusteringType==1)	
			clusters[getParameterSignature()]=getRanklist();

	}
	else /*(getClusteringType() == REFERENCE_CLUSTERING)*/
	{
		RLRefClustering.insert(RLRefClustering.end(), getRanklist());
		refClusters[getRanklist().getFirstRank()]=getReferenceSignature();
	}
}


void Trace::mergeCluster()
{

	if (getClusteringType() <= CALL_PATH_PARAMETER_CLUSTERING)
	{
		map<long long int, Ranklist>::iterator it;
		map<long long int, Ranklist>::iterator it2;
		map<long long int, Ranklist>::iterator it3;

		for( it=recv_clusters.begin(); it!=recv_clusters.end(); it++)
		{
			it2 = clusters.find(it->first);
			if (it2!=clusters.end())
			{
				/*Existing Cluster - just modify ranklist*/
				it2->second.merge(&it->second);

			}
			else
			{
				long long int temp2;
				long long int temp1 = it->first;
				long long int temp1_rest = it->first & 0xFFFFFFFFFFFFFF00;
				unsigned short temp2_AvgCount;
				unsigned short temp1_AvgCount=(unsigned short) temp1; 
				bool rs = false;  
				for( it3=clusters.begin(); it3!=clusters.end() && rs!=true ; it3++)
				{
					temp2 =	it3->first;
					temp2_AvgCount = (unsigned short) temp2; 
					float fract = temp1_AvgCount/temp2_AvgCount;
					temp2 = temp2 & 0xFFFFFFFFFFFFFF00; 
					/*We need to check the rest of the content signature is equal*/
					if ((temp1_rest == temp2) && ((fract >= 0.9) || (fract <= 1.1)))
					{
						rs=true;
						/*Existing Cluster - just modify ranklist*/
						it3->second.merge(&it->second);
					}			
				}

				/*New Cluster*/
				if (rs==false)
					clusters[it->first]= it->second;
			}
		}	

		while (!recv_clusters.empty())
		{
			recv_clusters.erase(recv_clusters.begin());
		} 
	}
	else if (getClusteringType() == REFERENCE_CLUSTERING)
	{
		map<int, map<int, map<int, long long int> > >::iterator masterSigIt1;
		map<int, map<int, long long int> >::iterator masterEventIt2;
		map<int, long long int>::iterator masterParamIt3;

		map<int, map<int, map<int, long long int> > >::iterator slaveSigIt4;
		map<int, map<int, long long int> >::iterator slaveEventIt5;
		map<int, long long int>::iterator slaveParamIt6;
		bool flag;
		list<Ranklist>::iterator rl1;
		list<Ranklist>::iterator rl2;

		int * mergeIndex = new int [recvRefClusters.size()];
		for (unsigned int i=0; i< recvRefClusters.size(); i++)
			mergeIndex[i]=-1;

		int index=0;

		for(slaveSigIt4=recvRefClusters.begin(), rl2=recvRLRefClustering.begin(); slaveSigIt4!=recvRefClusters.end(); slaveSigIt4++,rl2++)
		{
			flag=false;
			masterSigIt1=refClusters.begin();
			rl1=RLRefClustering.begin(); 

			while(masterSigIt1!=refClusters.end() && flag!=true)  
			{		
				flag=true;
				masterEventIt2=masterSigIt1->second.begin();
				for (slaveEventIt5 = slaveSigIt4->second.begin(); slaveEventIt5!=slaveSigIt4->second.end() && masterEventIt2!=masterSigIt1->second.end(); slaveEventIt5++) //check event sequence
				{		
					masterParamIt3 = masterEventIt2->second.begin(); 
					for(slaveParamIt6 = slaveEventIt5->second.begin(); slaveParamIt6!=slaveEventIt5->second.end() && masterParamIt3!=masterEventIt2->second.end() ;slaveParamIt6++) // check Param ID/Values
					{
						if((slaveParamIt6->first!= masterParamIt3->first) ||  (masterParamIt3->second!=slaveParamIt6->second)) 
						{	flag=false;
						}
						masterParamIt3++;
					}
					if ((slaveParamIt6==slaveEventIt5->second.end() && masterParamIt3!=masterEventIt2->second.end()) || (slaveParamIt6!=slaveEventIt5->second.end() && masterParamIt3==masterEventIt2->second.end()) )
					{
						flag=false;
					}
					masterEventIt2++; //next event
				}
				if ((slaveEventIt5==slaveSigIt4->second.end() && masterEventIt2!=masterSigIt1->second.end()) || (slaveEventIt5!=slaveSigIt4->second.end() && masterEventIt2==masterSigIt1->second.end()))
				{	flag=false;
				}

				if (flag==true)
				{
					mergeIndex[index]=0;			
					rl1->merge(&(*rl2));
				}
				masterSigIt1++;
				rl1++;
			}
			index++;
		}

		slaveSigIt4=recvRefClusters.begin();
		int val =0;// refClusters.rbegin->first;
		for (masterSigIt1=refClusters.begin(); masterSigIt1!=refClusters.end(); masterSigIt1++)
			val = masterSigIt1->first;	
		rl2=recvRLRefClustering.begin();
		for (unsigned int i=0; i< recvRefClusters.size(); i++, slaveSigIt4++, rl2++)
		{
			if (mergeIndex[i]==-1)
			{	
				RLRefClustering.insert(RLRefClustering.end(), *rl2);
				refClusters[++val]=slaveSigIt4->second;
			}
		}


	}	

}
void Trace::sendCluster(int dest){
	int bufsize = getClusterPackedSize();
	void *buf = malloc(bufsize);

	MPI_Comm comm = MPI_COMM_WORLD;
	int position = 0;
	if(!buf){
		cerr << "Trace::sendCluster(): cannot allocate buffer" << endl;
		exit(1);
	}
	PMPI_Send(&bufsize, 1, MPI_INT, dest, 0, comm);
	packCluster(buf, bufsize, &position, comm);
	PMPI_Send(buf, position, MPI_PACKED, dest, 0, comm);
	free(buf);
}


void Trace::recvCluster(int src){

	int bufsize;
	void *buf;
	MPI_Comm comm = MPI_COMM_WORLD;
	int position = 0;
	PMPI_Recv(&bufsize, 1, MPI_INT, src, 0, comm, MPI_STATUS_IGNORE);

	buf = malloc(bufsize);
	if(!buf){
		cerr << "Trace::recvCluster(): cannot allocate buffer" << endl;
		exit(1);
	}
	PMPI_Recv(buf, bufsize, MPI_PACKED, src, 0, comm, MPI_STATUS_IGNORE);
	unpackCluster(buf, bufsize, &position, comm);

	free(buf);
}

void Trace::findMyCluster(int rank)
{
	map<long long int, Ranklist>::iterator it3;
	long long int my_SIG;


	if (getClusteringType()==0)
		my_SIG=getCallPathSignature();
	else if (getClusteringType()==1)
		my_SIG = getParameterSignature();


	if (rank == 0)
	{recv_clusters=clusters;}


	setNumberOfClusters(clusters.size());

	int tempInterClusterRL [recv_clusters.size()];
	int index=0;	
	int CID=0;
	for( it3=recv_clusters.begin(); it3!=recv_clusters.end(); it3++)
	{

		const vector<int> *other_ranks = (*it3).second.getRanks();
		tempInterClusterRL[index]= other_ranks->at(2);	
		index++;
		if ((*it3).first == my_SIG)
		{
			int count = (*it3).second.getNumRanks();
			int *temp_rl = (int *)malloc(sizeof(int) * (other_ranks->size()));
			for(unsigned i=0; i<other_ranks->size(); i++)
				temp_rl[i] = other_ranks->at(i); 
			int unroll [count];
			(*it3).second.decompressRL(temp_rl, count, unroll);

			setUnrolledRank(unroll, count);
			setClusterID(CID);
		}
		CID++;
	}

	/*Set Inter-Cluster Ranklist that will be used by the second level of reduction*/
	std::sort(tempInterClusterRL, tempInterClusterRL+recv_clusters.size());
	setInterClusterRL(tempInterClusterRL, recv_clusters.size());
	checkInterClusterRL();

}

/*We need to make sure node zero is a representative- if not, add it to the set*/
void Trace::checkInterClusterRL()
{
	int temp [interClusterRLSize+1];
	for(int index=0; index<interClusterRLSize; index++)
	{
		temp[index+1]=interClusterRL[index]; 
		if (interClusterRL[index]==0)
			return;
	}
	cout<< endl << " Inter Cluster Zero case"<< endl;	
	temp[0]=0;
	setInterClusterRL(temp, interClusterRLSize+1);
}

void Trace::bcastCluster(int rank){

	if (getClusteringType() <= CALL_PATH_PARAMETER_CLUSTERING)
	{
		int bufsize;
		void *buf;
		int position = 0;
		MPI_Comm comm = MPI_COMM_WORLD;

		if (rank ==0)
		{
			bufsize = getClusterPackedSize();
			buf = malloc(bufsize);

			if(!buf){
				cerr << "Trace::sendBcastCluster(): cannot allocate buffer" << endl;
				exit(1);
			}

			packCluster(buf, bufsize, &position, comm);
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
		if (rank != 0)
			unpackCluster(buf, bufsize, &position, comm);

		free(buf);

	}
	else if (getClusteringType() == REFERENCE_CLUSTERING)
	{
		int bufsize;
		int *selectedSet;
		//int position = 0;
		MPI_Comm comm = MPI_COMM_WORLD;

		if (rank ==0)
		{
			list<Ranklist>::iterator rl1 = RLRefClustering.begin();
			selectedSet= new int [ RLRefClustering.size()]; 
			int index=0;
			for (; rl1!=RLRefClustering.end(); rl1++ )
			{	
				selectedSet[index] = (*rl1).getFirstRank(); 
				index++;
			}
			bufsize= (RLRefClustering.size() * sizeof (int));
			std::sort(selectedSet, selectedSet+RLRefClustering.size());

		}

		PMPI_Bcast(&bufsize, 1, MPI_INT, 0, comm);
		if (rank!=0)
		{
			selectedSet=(int *)malloc(bufsize*sizeof(int));
			if(!selectedSet){
				cerr << "Trace::recvBcastCluster(): cannot allocate buffer" << endl;
				exit(1);
			}
		}

		PMPI_Bcast(selectedSet, bufsize, MPI_INT, 0, comm);
		int size = (bufsize/sizeof (int));

		setUnrolledRank(selectedSet, size); 

		free(selectedSet);
	}	
}

int Trace::getClusterPackedSize(){

	int size = 0;


	size += sizeof (int); /*size of clusters - len*/

	if (getClusteringType() <= CALL_PATH_PARAMETER_CLUSTERING)
	{
		map<long long int, Ranklist>::iterator it;

		for( it=clusters.begin(); it!=clusters.end(); it++)
		{
			size += sizeof(long long int);
			size += it->second.getPackedSize(); 	
		}
	}

	else if (getClusteringType() == REFERENCE_CLUSTERING)
	{
		map<int, map<int, map<int, long long int> > >::iterator it;
		map<int, map<int, long long int> >::iterator it2;
		map<int, long long int>::iterator it3;

		for( it=refClusters.begin(); it!=refClusters.end(); it++)
		{
			size += sizeof (int); /*trace unique num*/
			size += sizeof (int); /*event Count*/
			for (it2 = it->second.begin(); it2!=it->second.end(); it2++)
			{
				size += sizeof (int); /*sequence num*/
				size += sizeof (int); /*param Count*/
				for(it3 = it2->second.begin(); it3!=it2->second.end();it3++)
				{
					size += sizeof (int); /* Param ID */
					size += sizeof(long long int); /*Param Values*/
				}				
			}	 	
		}

		list<Ranklist>::iterator rl;
		for( rl = RLRefClustering.begin(); rl!=RLRefClustering.end(); rl++)
		{
			size+= rl->getPackedSize();
		}


		//	if (getRefSize()==-1)
		setRefSize(size);	
		//	cout << endl << "size: " << size<<endl;
	}

	return size;
} 

void Trace::packCluster(void *buf, int bufsize, int *position, MPI_Comm comm){

	int len;

	if (getClusteringType() <= CALL_PATH_PARAMETER_CLUSTERING)
	{
		len = clusters.size();
		PMPI_Pack(&len, 1, MPI_INT, buf, bufsize, position, comm);

		map<long long int, Ranklist>::iterator it; 
		//long long int temp;
		for( it=clusters.begin(); it!=clusters.end(); it++)
		{
			PMPI_Pack((void*)&it->first, 1, MPI_UNSIGNED_LONG_LONG, buf, bufsize, position, comm);
			it->second.pack(buf, bufsize, position, comm);
		}
	}
	else if (getClusteringType() == REFERENCE_CLUSTERING)
	{

		int paramCount=0, eventCount=0;
		len = refClusters.size();
		PMPI_Pack(&len, 1, MPI_INT, buf, bufsize, position, comm);

		map<int, map<int, map<int, long long int> > >::iterator it;
		map<int, map<int, long long int> >::iterator it2;
		map<int, long long int>::iterator it3;

		for(it=refClusters.begin(); it!=refClusters.end(); it++)
		{
			PMPI_Pack((void*)&it->first, 1, MPI_INT, buf, bufsize, position, comm);	 /*Trace unique num*/

			eventCount=it->second.size();
			PMPI_Pack(&eventCount, 1, MPI_INT, buf, bufsize, position, comm);	 /*event Count*/

			for (it2 = it->second.begin(); it2!=it->second.end(); it2++)
			{

				PMPI_Pack((void*)&it2->first, 1, MPI_INT, buf, bufsize, position, comm);	 /*event seq num*/
				paramCount=it2->second.size();
				PMPI_Pack(&paramCount, 1, MPI_INT, buf, bufsize, position, comm);	 /*param count*/
				for(it3 = it2->second.begin(); it3!=it2->second.end();it3++)
				{

					PMPI_Pack((void*)&it3->first, 1, MPI_INT, buf, bufsize, position, comm); /* Param ID */
					PMPI_Pack((void*)&it3->second, 1, MPI_UNSIGNED_LONG_LONG, buf, bufsize, position, comm); /*Param Values*/
				}				
			}	 	
		}

		list<Ranklist>::iterator rl;
		for( rl = RLRefClustering.begin(); rl!=RLRefClustering.end(); rl++)
		{
			rl->pack(buf, bufsize, position, comm);
		}
	} 
}

void Trace::unpackCluster(void *buf, int bufsize, int *position, MPI_Comm comm){

	int len;

	if (getClusteringType() <= CALL_PATH_PARAMETER_CLUSTERING)
	{
		PMPI_Unpack(buf, bufsize, position, &len, 1, MPI_INT, comm);                
		long long int sig;
		Ranklist rl[len]; 
		for(int i=0; i<len; i++){
			PMPI_Unpack(buf, bufsize, position, &sig, 1, MPI_UNSIGNED_LONG_LONG, comm);
			rl[i].unpack(buf, bufsize, position, comm);
			recv_clusters[sig]=rl[i];         
		} 
	}	
	else if (getClusteringType() == REFERENCE_CLUSTERING)
	{
		long long int temp3;
		int temp0, temp1, temp2, eventCount, paramCount ;
		//cout << "unpack"<< endl;
		PMPI_Unpack(buf, bufsize, position, &len, 1, MPI_INT, comm);                
		//	cout << endl <<"len:"<< len;
		recvRefClusters.clear();
		for (int index1=0; index1<len; index1++)
		{
			PMPI_Unpack(buf, bufsize, position, &temp0, 1, MPI_INT, comm); /*trace unique number*/
			//cout << endl <<"temp0:"<< temp0;
			PMPI_Unpack(buf, bufsize, position, &eventCount, 1, MPI_INT, comm);                
			//cout << endl <<"eventCount:"<< eventCount;
			map<int, map<int, long long int> >refSig;
			refSig.clear();
			for (int index2=0;index2<eventCount; index2++)
			{

				PMPI_Unpack(buf, bufsize, position, &temp1, 1, MPI_INT, comm); /*event seq number*/
				//cout << endl <<"temp1:"<< temp1;
				PMPI_Unpack(buf, bufsize, position, &paramCount, 1, MPI_INT, comm);
				//cout << endl <<"paramCount:"<< paramCount;
				map<int, long long int> tempSig;
				tempSig.clear();
				for (int index3=0; index3<paramCount; index3++)
				{
					PMPI_Unpack(buf, bufsize, position, &temp2, 1, MPI_INT, comm); /*Param ID*/
					//cout << endl <<"temp2:"<< temp2;
					PMPI_Unpack(buf, bufsize, position, &temp3, 1, MPI_UNSIGNED_LONG_LONG, comm); /*Param Values*/
					//cout << endl <<"temp3:"<< temp3;
					tempSig[temp2]=temp3;
				}
				refSig[temp1]=tempSig; 	
			}
			recvRefClusters[temp0]=refSig;
		}

		//cout << "unpack"<< endl;
		Ranklist rl [len];
		recvRLRefClustering.clear();
		for(int index =0 ; index<len; index++ )
		{
			rl[index].unpack(buf, bufsize, position, comm);
			//cout << endl<< "AM:" << rl[index].toString()<< endl;
			//cout << endl;
			recvRLRefClustering.insert(recvRLRefClustering.end(), rl[index]);
		}	

	}
}

void Trace::printParameters(const char* filename){

	ofstream paramfile(filename);
	if(!paramfile ){
		cerr << "error: unable to open cluster file: " << filename;
		return;
	}

	Event *iter = head;
	Event *prev_event = NULL;
	StackSig* prev_sig = NULL;


	unsigned int seq_num = 0; /*Number of Events*/

	long long CallPath_Sig=0; /*CallPath Signature 64b*/
	long long AvgSRC=0;
	long long AvgDEST=0;
	long long AvgTAG=0;
	long long AvgCOLOR=0;
	long long AvgKEY=0;
	long long DATA_OP=0;

	vector<long long> CompTimeV;
	vector<long long> CommTimeV;
	vector<long long> CountV;
	vector<long long> LoopIterL0V;
	vector<long long> LoopIterL1V;
	vector<long long> LoopIterL2V;
	vector<long long> LoopIterL3V;

	long long AvgCompTime=0;
	long long AvgCommTime=0;
	long long AvgCount=0; 
	long long AvgLoopIter=0;


	set<long long> SRC_Set;
	set<long long> DEST_Set;
	set<long long int> TAG_Set;
	set<long long int> KEY_Set;
	set<long long int> COLOR_Set;
	set<long long>::iterator itSet;	
	set<long long>::iterator itSet2;

	long long int tempSRC=0;
	long long int tempDEST=0;
	long long int tempTAG=0;
	long long int tempKEY=0;
	long long int tempCOLOR=0;
	long long int tempDataCommOP=0;
	long long int tempCompTime=0;
	long long int tempCommTime=0;
	long long int temp_AvgCount=0; 
	//unsigned int numCount=0; /*Number of events contain Count*/
	//unsigned int numLoop=0;

	unsigned long L1=0;
	unsigned long L2=0;
	unsigned long L3=0;
	unsigned long L0=0;


	map<int, map<int, long long int> > Ref_Sig;
	int Ref_Size=0;	

	vector<int> iterMem;
	vector<int> iterLoop;

	unsigned int tmp_mod_seq_num=1;	

	while(iter){
		seq_num++;
		tmp_mod_seq_num = (seq_num % 10)+1;
		/* Call-Path Signature */
		CallPath_Sig=CallPath_Sig ^ (iter->getCallPathSignature() * tmp_mod_seq_num);
		if(prev_event)
			prev_sig = prev_event->getSignature();

		/* DEST/SRC/TAG/DATA_OP/AvgCompTime/AvgCommTime/AvgCount Signature*/
		iter->getParameterSignature(temp_AvgCount, tempCompTime, tempCommTime, tempDataCommOP, tempSRC, tempDEST, tempTAG, tempKEY, tempCOLOR, prev_sig);

		DATA_OP|=tempDataCommOP;
		CompTimeV.push_back(tempCompTime);
		CommTimeV.push_back(tempCommTime);
		SRC_Set.insert(tempSRC);
		DEST_Set.insert(tempDEST);
		TAG_Set.insert(tempTAG); 
		KEY_Set.insert(tempKEY); 
		COLOR_Set.insert(tempCOLOR); 
		vector<loop_t *> *eventLoops = iter->getLoopStack()->getLoops();

		if (!eventLoops->empty()){ /*Leader*/
			int iterMemSize, iterLoopSize;
			for(vector<loop_t *>::iterator it = eventLoops->begin(); it != eventLoops->end(); it++){
				iterMemSize = dynamic_cast<ParamValue *>((*it)->mem->getParamValues()->at(0) )->getCurrentValue(true);
				iterLoopSize= dynamic_cast<ParamValue *>((*it)->iter->getParamValues()->at(0) )->getCurrentValue(true);
				iterMem.push_back(iterMemSize);

				iterLoop.push_back(iterLoopSize);
				/*if(!iterLoop.empty())
				  {
				  iterLoop.push_back(iterLoopSize * iterLoop[iterLoop.size()-1]);
				  }
				  else{
				  iterLoop.push_back(iterLoopSize);
				  } */
				int level = iterLoop.size()-1;
				if (level == 0){
					LoopIterL0V.push_back(iterLoop[iterLoop.size()-1]);
				}
				else if (level == 1){
					LoopIterL1V.push_back(iterLoop[iterLoop.size()-1]);
				}
				else if(level == 2){
					LoopIterL2V.push_back(iterLoop[iterLoop.size()-1]);
				}
				else if (level >= 3){
					LoopIterL3V.push_back(iterLoop[iterLoop.size()-1]);
				}
				//AvgLoopIter+=iterLoop[iterLoop.size()-1];
				//numLoop++;
			}
		}

		if(!iterMem.empty()){ /*Part of a loop*/
			for(unsigned int i=0; i< iterMem.size(); i++)
			{
				--iterMem[i];
			}
			//AvgCount += iterLoop[iterLoop.size()-1]*temp_AvgCount;
			CountV.push_back(iterLoop[iterLoop.size()-1]*temp_AvgCount);

			//if (temp_AvgCount!=0){ /*Count events with COUNT parameter*/
			//	numCount+=iterLoop[iterLoop.size()-1];
			//}

			int loop_mem_size = iterMem.size();
			/*remove loops which are considered*/
			while(loop_mem_size>1)
			{
				if (iterMem[iterMem.size()-1]==0){
					iterMem.pop_back();
					iterLoop.pop_back();
					loop_mem_size= iterMem.size();
				}
				else
					loop_mem_size=0;
			}

			if(iterMem.size()==1 && iterMem[0]==0)
			{
				iterMem.erase(iterMem.begin());
				iterLoop.erase(iterLoop.begin());
			}
		}
		else{ /*Not a part of loop*/
			CountV.push_back(temp_AvgCount);
			//AvgCount+=temp_AvgCount;
			//	if (temp_AvgCount!=0){ /*Count events with COUNT parameter*/
			//		numCount++;}
		}

		/* Reference Signature*/	
		Ref_Sig[seq_num-1]=iter->getRefSignature();
		Ref_Size+= iter->getRefSignature().size();

		prev_event= iter;
		iter = iter->next;
	}
	if (CountV.size()>0)
		AvgCount= calcAverage<long long>(CountV);

	if(CompTimeV.size()>0){
		AvgCompTime = calcAverage<long long>(CompTimeV);
	}
	if(CommTimeV.size()>0){
		AvgCommTime = calcAverage<long long>(CommTimeV);
	}

	if (SRC_Set.size()>0){
		vector<long long> input_AvgSRC(SRC_Set.begin(), SRC_Set.end());
		AvgSRC = calcAverage<long long>(input_AvgSRC);		
	}

	if (DEST_Set.size()>0){
		vector<long long> input_AvgDEST(DEST_Set.begin(), DEST_Set.end());
		AvgDEST = calcAverage<long long>(input_AvgDEST);		
	}

	if (TAG_Set.size()>0){
		vector<long long> input_AvgTAG(TAG_Set.begin(), TAG_Set.end());
		AvgTAG = calcAverage<long long>(input_AvgTAG);		
	}
	if(COLOR_Set.size()>0){
		vector<long long> input_AvgCOLOR(COLOR_Set.begin(), COLOR_Set.end());
		AvgCOLOR = calcAverage<long long>(input_AvgCOLOR);		
	}
	if(KEY_Set.size()>0){
		vector<long long> input_AvgKEY(KEY_Set.begin(), KEY_Set.end());
		AvgKEY = calcAverage<long long>(input_AvgKEY);		
	}

	//take average of bit positions for Data_OP

	if(LoopIterL0V.size()>0)
		L0 = calcAverage<long long> (LoopIterL0V);
	if(LoopIterL1V.size()>0)
		L1 = calcAverage<long long> (LoopIterL1V);
	if(LoopIterL2V.size()>0)
		L2 = calcAverage<long long> (LoopIterL2V);
	if(LoopIterL3V.size()>0)
		L3 = calcAverage<long long> (LoopIterL3V);

	if (L0 || L1 || L2 || L3){
		AvgLoopIter |= L0; 
		AvgLoopIter |= (L1<<16);
		AvgLoopIter |= (L2<<32);
		AvgLoopIter |= (L3<<48);
	}

	paramfile << "Avg-Communication-Time   Avg-Computation-Time   Avg-Count   Avg-TAG   Avg-Key   Avg-Color   Avg-DEST   Avg-SRC   Avg-LOOP  XOR-Call-Path " << endl;
	paramfile << hex<< AvgCommTime << " " << AvgCompTime << " " << AvgCount << " " << AvgTAG <<" "<< AvgKEY << " "  << AvgCOLOR <<" " << AvgDEST << " " << AvgSRC << " " << AvgLoopIter << " " <<  CallPath_Sig<< endl;

	cout << "Avg-Communication-Time   Avg-Computation-Time   Avg-Count   Avg-TAG   Avg-Key   Avg-Color   Avg-DEST   Avg-SRC   Avg-LOOP  XOR-Call-Path " << endl;
	cout << hex<< AvgCommTime << " " << AvgCompTime << " " << AvgCount << " " << AvgTAG <<" "<< AvgKEY << " "  << AvgCOLOR <<" " << AvgDEST << " " << AvgSRC << " " << AvgLoopIter << " " << CallPath_Sig << endl;


	paramfile.close();


}



void Trace::outputCluster(const char* filename){

	ofstream clusterfile(filename);

	if(!clusterfile ){
		cerr << "error: unable to open cluster file: " << filename;
		return;
	}
	if (getClusteringType() <= CALL_PATH_PARAMETER_CLUSTERING)
	{
		map<long long int, Ranklist>::iterator it;
		for( it=clusters.begin(); it!=clusters.end(); it++)
		{
			clusterfile << hex<< (*it).first << " => " <<(*it).second.toString() << endl;
		}	
	}	
	else if (getClusteringType() == REFERENCE_CLUSTERING)
	{
		list<Ranklist>::iterator rl_temp; 
		/*for(rl_temp=RLRefClustering.begin(); rl_temp!=RLRefClustering.end(); rl_temp++)
		  {
		  cout<< (*rl_temp).toString();
		  }*/
		rl_temp=RLRefClustering.begin(); 
		map<int, map<int, map<int, long long int> > >::iterator it;
		map<int, map<int, long long int> >::iterator it2;
		map<int, long long int>::iterator it3;
		clusterfile << "\nSize2: " <<getRefSize()<< "\n"<<endl;
		for(it=refClusters.begin(); it!=refClusters.end(); it++)
		{
			//cout << "SIZE:" <<refClusters.size(); 	
			clusterfile << "\n\nSignature ID\n" << (*it).first << "    Ranklist: "<< (*rl_temp).toString() << endl;
			rl_temp++;
			/*	for (it2 = it->second.begin(); it2!=it->second.end(); it2++)
				{
				for(it3 = it2->second.begin(); it3!=it2->second.end();it3++)
				{
				clusterfile << (*it3).first << " => " << (*it3).second << endl;
				}				
				}	 	
			 */
		}


		clusterfile.close();
	}
}
void Trace::initCluster(int rank){
	Ranklist ranklist_temp(rank);
	setRanklist(ranklist_temp);	
	addCluster();
	setRefSize(-1);
}

void Trace::selectSamples(int rank){
	/*select the first rank of each cluster as the representative of that cluster */
	setClusterRank(unrolled_rank,  unrolled_rank_size);
	map<long long int, Ranklist>::iterator it;
	int count = clusters.size(); 
	int selectedSamples[count];
	int index =0;

	/*add your rank for the first element*/	

	for( it=clusters.begin(); it!=clusters.end(); it++)
	{
		const vector<int> *other_ranks = (*it).second.getRanks();
		selectedSamples[index] = other_ranks->at(2);
		if (selectedSamples[0]!=rank)
			if (selectedSamples[index] == rank){
				int temp = selectedSamples[0];
				selectedSamples[0] = rank;
				selectedSamples[index] = temp;
			}

		index++;
	}

	/*for (int index2=0; index2<count; index2++)
	  cout << endl << selectedSamples[index2];
	 */

	setUnrolledRank(selectedSamples, count);
}


void Trace::sendSamples(int my_rank){

	if (cluster_size>1){
		MPI_Comm comm = MPI_COMM_WORLD;	
		int index=0;
		while(index<cluster_size)
		{
			if (cluster_ranks[index]!=my_rank)
			{
				PMPI_Send(&unrolled_rank_size, 1, MPI_INT, cluster_ranks[index], 0, comm);
				PMPI_Send(unrolled_rank, unrolled_rank_size, MPI_INT, cluster_ranks[index], 0, comm);
			}

			index++;
		}	

	}

}

void Trace::recvSamples(){
	int src = unrolled_rank[0]; /*Head of the cluster created by the firts level*/
	MPI_Comm comm = MPI_COMM_WORLD;
	PMPI_Recv(&unrolled_rank_size, 1, MPI_INT, src, 0, comm, MPI_STATUS_IGNORE);	
	unrolled_rank= new int [unrolled_rank_size];
	PMPI_Recv(unrolled_rank, unrolled_rank_size, MPI_INT, src, 0, comm, MPI_STATUS_IGNORE);
}

void Trace::expandClusterRanklist(Ranklist rl){

	Event *iter = head;
	while(iter){
		iter->setRanklist(rl);
		iter->expandParamRanklist(rl);
		iter = iter->next;
	}
}

void Trace::recvClusterRanklist(){
	int src = unrolled_rank[0]; /*Head of the cluster created by the firts level*/
	int bufsize;
	void *buf;
	MPI_Comm comm = MPI_COMM_WORLD;
	int position = 0;
	Ranklist rl; 

	PMPI_Recv(&bufsize, 1, MPI_INT, src, 0, comm, MPI_STATUS_IGNORE);
	buf = malloc(bufsize);
	if(!buf){
		cerr << "Trace::recvClusterRanklist(): cannot allocate buffer" << endl;
		exit(1);
	}
	PMPI_Recv(buf, bufsize, MPI_PACKED, src, 0, comm, MPI_STATUS_IGNORE);
	rl.unpack(buf, bufsize, &position, comm);
	expandClusterRanklist(rl);	
	free(buf);
}

/*To create the full ranklist at the second level - head of the second level clusters need to send their ranklist to sub-clusters */
void Trace::sendClusterRanklist(int myrank){
	MPI_Comm comm = MPI_COMM_WORLD;
	int bufsize;
	int dest;
	int position; 

	if (getClusteringType() <= CALL_PATH_PARAMETER_CLUSTERING)
	{

		map<long long int, Ranklist>::iterator it;

		for( it=clusters.begin();it!=clusters.end(); it++)
		{
			bufsize = (*it).second.getPackedSize();
			void *buf = malloc(bufsize);
			dest = (*it).second.getFirstRank();// unrolled_rank[index];

			if (dest != myrank){
				position = 0;
				if(!buf){
					cerr << "Trace::sendCluterRanklist(): cannot allocate buffer" << endl;
					exit(1);
				}

				PMPI_Send(&bufsize, 1, MPI_INT, dest, 0, comm);
				(*it).second.pack(buf, bufsize, &position, comm);
				PMPI_Send(buf, position, MPI_PACKED, dest, 0, comm);
				free(buf);
			}
			else{
				if ((*it).second.getNumRanks()>1)	
				{	
					expandClusterRanklist((*it).second);	
				}
			}
		}
	}
	else if (getClusteringType() == REFERENCE_CLUSTERING)
	{

		list<Ranklist>::iterator rl1;

		for( rl1= RLRefClustering.begin();rl1!= RLRefClustering.end(); rl1++)
		{
			bufsize = (*rl1).getPackedSize();
			void *buf = malloc(bufsize);
			dest = (*rl1).getFirstRank();
			//cout << endl << " dest:	" << dest << endl;
			if (dest != myrank){
				position = 0;
				if(!buf){
					cerr << "Trace::sendCluterRanklist(): cannot allocate buffer" << endl;
					exit(1);
				}


				PMPI_Send(&bufsize, 1, MPI_INT, dest, 0, comm);
				(*rl1).pack(buf, bufsize, &position, comm);
				PMPI_Send(buf, position, MPI_PACKED, dest, 0, comm);
				free(buf);
			}
			else{

				if ((*rl1).getNumRanks()>1)	
				{	
					expandClusterRanklist(*rl1);	
				}
			}

		}

	}
}

int Trace::checkSampleRanks(int my_rank){

	for (int index=0; index< unrolled_rank_size; index++){

		if (my_rank==unrolled_rank[index])
		{
			return 1; /*You are a representative*/
		}
	}

	return -1; /*Not a representative*/
}

int Trace::getPRSDClusterFlag(){
	int glob_PRSD_flag_cluster = 0; 

	PMPI_Reduce(&PRSD_flag_cluster, &glob_PRSD_flag_cluster, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);
	PMPI_Bcast(&glob_PRSD_flag_cluster, 1, MPI_INT, 0, MPI_COMM_WORLD);

	return glob_PRSD_flag_cluster;
}



int Trace::matchSignatures(int my_rank, int comm_size, long int num_events){
// TODO: if sig_format is zero!!!

	const int SKIP=1, NOC=0, C=-4;

	

/*	cout << "OLD NUM EVENTS: " << getOldNumEvents() << "  Current: " << getNumEvents() << endl;
	//if(getNumEvents() ==1 ||  ((getOldNumEvents()+1)== getNumEvents() || getOldNumEvents() == getNumEvents() )){
	if(getNumEvents() ==1 ||  (getOldNumEvents()+10 getNumEvents() || getOldNumEvents() == getNumEvents() )){
		cout << "------------->>>>>>ONE DIFFERENCE<<<<==========" << endl;
		return	SKIP;
	}
	else{
		setOldNumEvents(getNumEvents());
	}
*/


	incNumIters();
	if(my_rank == 0)
		cout << "#### Chameleon: <------ Iteration ------> " << getNumIters() << endl;

	if(getNumIters()%num_events!=0){
		return SKIP;
	}

	if(my_rank == 0)
		cout << "#### Chameleon: Total Number of Events is: " << getNumEvents() << endl;

	setMyRank(my_rank);
	//TODO: In case there is only no event b/w two clustering markers

	// When Not in NO-C 
	// OR if my_rank is a representative, then I have to calc sigs  
	if(!getNoClusteringFlag() || onlyHeadCluster(my_rank)!=-1){

	// In this case, we ignore clustering and continue w/o clusteing
		Event *iter = head;
		int cnt=0;
		while(iter){
			iter = iter->next;
			cnt++;
		}
		createSignature(my_rank, comm_size);	
	}
	else if(getNoClusteringFlag() && onlyHeadCluster(my_rank)==-1){
		//not a lead node, and tracing mode is off
	        setCallPathSignature(getOldCallPath());
	}


	//First time calling marker!!! This is Skip!
	if(getOldCallPath() == 0){
		initMyCluster(my_rank); //NEW
		initCluster(my_rank); /*Init Cluster*/
		setOldCallPath(getCallPathSignature()); //myCluster->getCallPath()); // save the init state
		
		setNoClusteringFlag(false);

		if(my_rank==0)
			cout << "First time calling marker" << endl;
		setReClusteringFlag(true);

		cnt_AT++;
		return SKIP;
	}

	int valClustering = 0, globValClustering = 0; 


	if(getOldCallPath() != getCallPathSignature()) //tempCluster->getCallPath()) //NEW
	{
		valClustering = 1;
	}

	PMPI_Reduce(&valClustering, &globValClustering, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);
	PMPI_Bcast(&globValClustering, 1, MPI_INT, 0, MPI_COMM_WORLD);

	setOldCallPath(getCallPathSignature());
	
	//Either C or NO-C	
	if(globValClustering==0){
		if(getReClusteringFlag()){
			setReClusteringFlag(false);
			cnt_C++;
			return C;
		}
		else{
			//We are in NC
			setInterCompressionFlag(true);

#ifdef TURN_OFF_TRACING_FOR_NONE_LEADS
			//Set NoClustering
			setNoClusteringFlag(true);
#endif

			cnt_L++;
			return SKIP;
		}
	}

	setReClusteringFlag(true);
	if(getInterCompressionFlag()){
		
		setNoClusteringFlag(false);

		setInterCompressionFlag(false);
		//cnt_L++;
		return NOC;
	}

	if(getNoClusteringFlag()){
		cerr <<"Error: State is not No-Clustering, but still the tracing was off!" << endl;
	}		

	//Prepare for clustering; delete the old ones
	if (globValClustering != 0){ 
		deleteCluster();
		initMyCluster(my_rank);
		initCluster(my_rank); //Init Cluster
	}
	else
		delete tempCluster;


	cnt_AT++;

return SKIP; //globValClustering;
}


/* ---- C wrapper functions follow ----- */

Trace trace, left_trace, right_trace, cluster, left_cluster, right_cluster, root_trace, left, right, OnlineTrace, K_trace;
extern int my_rank;

void appendEvent(Trace *trace, Event* event){
#ifdef COMPRESS_INTRA_NODE
	trace->appendEvent(event);
#else
	trace->appendEvent(event, false);
#endif
}

void finalizePendingIterations(Trace *trace){
	trace->finalizePendingIterations();
}

void cleanupLoops(Trace *trace){
	trace->cleanupLoops();
}

Trace* inputTrace(char *filename, int mode ){
	return Trace::inputTrace(filename, mode);
}

/**/

void deleteCluster(Trace *trace){
	
	trace->deleteCluster();
}

void sendClusterFormat(Trace *trace, int dest){

	trace->sendClusterFormat(dest);
}

void recvClusterFormat(Trace *trace, int src, int my_rank){
	trace->recvClusterFormat(src, my_rank);
}

int onlyHeadCluster(Trace *trace, int rank){
	return trace->onlyHeadCluster(rank);
}

void BcastClusterFormat(Trace *trace, int rank){
	trace->BcastClusterFormat(rank);
}
void updateTraceRanklist(Trace *trace, int my_rank){
	trace->updateTraceRanklist(my_rank);
}

void initIndirectRanklist(Trace *trace){
	trace->initIndirectRanklist();
}

void BcastKCluster(Trace *trace, int rank){
	trace->BcastKCluster(rank);
}

void sendSigClusters(Trace *trace, int dest){
	trace->sendSigClusters(dest);
}
void recvSigClusters(Trace *trace, int src){
	trace->recvSigClusters(src);
}
void initSigClusters(Trace *trace){
	trace->initSigClusters();
}
void initSigClusters_L2(Trace *trace, int my_rank, int status){
	trace->initSigClusters_L2(my_rank, status);
}
void mergeSigClusters(Trace *trace){
	trace->mergeSigClusters();
}
void outputSigClusters(Trace *trace, char *filename){
	trace->outputSigClusters(filename);
}
void setK(Trace *trace, int K){
	trace->setK(K);
}
int getNumCallPath(Trace *trace){
	return (trace->getNumCallPath());
}
void setLevel(Trace *trace, int Level){
	trace->setLevel(Level);
}
void recvKClusters(Trace *trace, int src, int child){
	trace->recvKClusters(src, child);
}
void sendKClusters(Trace *trace, int dest){
	trace->sendKClusters(dest);
}
void mergeKClusters(Trace *trace, int myrank){
	trace->mergeKClusters(myrank);
}

void sendClusterHeadstoRoot(Trace *trace, int my_rank){
	trace->sendClusterHeadstoRoot(my_rank);
}


/**/

void initCluster(Trace *trace, int rank){

	trace->initCluster(rank);
}

void outputCluster(Trace *trace, char *filename){
	trace->outputCluster(filename);
}

void createCluster(Trace *trace, int rank, int comm_size){
	trace->createSignature(rank, comm_size);
}

void printParameters(Trace *trace, char *filename){
	trace->printParameters(filename);
}


void sendCluster(Trace *trace, int dest){

	trace->sendCluster(dest);
}

void recvCluster(Trace *trace, int src){
	trace->recvCluster(src);
}

void mergeCluster(Trace *trace){
	trace->mergeCluster();
}

void bcastCluster(Trace *trace, int rank){
	trace->bcastCluster(rank);
}

void findMyCluster(Trace *trace, int rank){
	trace->findMyCluster(rank);
}

void setClusteringType(Trace *trace, int temp_SIGTYPE){
	trace->setClusteringType(temp_SIGTYPE);
}
int findUnrolledRank(Trace *trace, int rank){

	return trace->findUnrolledRank(rank);
}

int findUnrolledSize(Trace *trace){
	return trace->findUnrolledSize();
}

int convertUnrolledRank(Trace *trace, int position){
	return trace->convertUnrolledRank(position);
}

void callSignatures(Trace *trace, char *filename, int rank, int comm_size){
	trace->callSignatures(filename, rank, comm_size);
}

void selectSamples(Trace *trace, int rank){
	trace->selectSamples(rank);
}

void sendSamples(Trace *trace, int my_rank){
	trace->sendSamples(my_rank);
}

void recvSamples(Trace *trace){
	trace->recvSamples();
}

int checkSampleRanks(Trace *trace, int my_rank)
{
	return trace->checkSampleRanks(my_rank);
}

int getClusterID(Trace *trace)
{
	return trace->getClusterID();
}

int getNumberOfClusters(Trace *trace)
{
	return trace->getNumberOfClusters();
}
void initInterClusterRL(Trace *trace){
	trace->initInterClusterRL();
}
void recvClusterRanklist(Trace *trace){
	trace->recvClusterRanklist();
}
void sendClusterRanklist(Trace *trace, int myrank){
	trace->sendClusterRanklist(myrank);
}

void LinearMerge(Trace *trace, Trace *slave){
	trace->LinearMerge(slave);
}

void outputTrace(Trace *trace, char *filename){
	trace->outputTrace(filename);
}

void outputOnlineTrace(Trace *OnlineTrace, const char* filename){
	cout << "Online address; " << OnlineTrace << endl;
	//OnlineTrace->outputOnlineTrace(filename, OnlineTrace->getHead());
	OnlineTrace->outputTrace(filename);

}
void sendTrace(Trace* trace, int dest){
	trace->sendTrace(dest);
}

void recvTrace(Trace* trace, int src){
	trace->recvTrace(src);
}

void mergeOnlineTrace(Trace *master, Trace *slave, int clusteringFlag){
	int slen = slave->traceLen();
	//int mlen = master->traceLen();

	if(slen>0){ // && slen<=mlen){
		master->onlineLCSMergeTrace(slave, clusteringFlag);
		//cout << ">>> Online LCSMerge - Done!" << endl;
	}
	else{
		//cout << ">>> Size of Slave Trace is 0! - No Online LCSMerge" << endl;
		if (clusteringFlag==0){
			cout << slave->getHead()->toString() << endl;
			cout << slave->getTail()->toString() << endl;
		}

	}
}

void mergeTrace(Trace *master, Trace *slave){
		if(slave->traceLen()>0)
			master->LCSMergeTrace(slave);
}

void deleteTrace(Trace *trace){
	if(trace->traceLen()>0)
		trace->deleteTrace(trace);
}

void saveLastTail(Trace *trace){

	trace->setLastTail(trace->getTail());

}


Event *tailEvent(Trace *trace){

	Event *e = trace->getTail();
	if(!e){
		e= trace->getLastTail();

	}

	return e;
}

Event *headEvent(Trace *trace){
	return trace->getHead();
}

void startTime(Trace *trace){
	trace->startTime();
}

void endTime(Trace *trace){
	trace->endTime();
}

int matchSignatures(Trace *trace, int my_rank, int comm_size, long int num_events){
	
	return trace->matchSignatures(my_rank, comm_size, num_events);
}

//int matchCallPathSignatures(Trace *trace, int my_rank, int comm_size, long int num_events){
	
//	return trace->matchCallPathSignatures(my_rank, comm_size, num_events);
//}

void initMyCluster(Trace *trace, int my_rank){
	
	trace->initMyCluster(my_rank);
}

void offNoClusteringFlag(Trace *trace){
	trace->setNoClusteringFlag(false);
}
void resetPRSDClusterFlag(Trace *trace){
	trace->resetPRSDClusterFlag();
}
void setPRSDClusterFlag(Trace *trace){
	trace->setPRSDClusterFlag();
}
int getPRSDClusterFlag(Trace *trace){
 	return trace->getPRSDClusterFlag();
}

void setOFFOnlineClusteringFlags(Trace *trace, int rank){
	trace->setOFFOnlineClusteringFlags(rank);
}
int getNoClusteringFlag(Trace *trace){
	if(trace->getNoClusteringFlag())
		return 1;
	else
		return 0;
}

void copyTrace(Trace *trace, Trace *OnlineCluster){
//	if (trace){
	//	Trace tempTrace = *trace;
	//	OnlineCluster = &tempTrace;
//	}
//	cout << "========================= Befoer Copy TRACE ======================" << endl;
//	trace->printTrace();


///	cout << "========================= Before Copy Online Cluster ======================" << endl;
//	OnlineCluster->printTrace();


	OnlineCluster->setTail(trace->getTail());
	OnlineCluster->setHead(trace->getHead());
	
//	cout << "========================= After Copy TRACE ======================" << endl;
//	trace->printTrace();


//	cout << "========================= After Copy Online Cluster ======================" << endl;
//	OnlineCluster->printTrace();


	trace->setTail(NULL);
	trace->setHead(NULL);
//	cout << "========================= Done w/ Copy  ======================" << endl;



}
void resetNumEvents(Trace *trace, int rank){
	//cout << "My rank: " << rank << " Number of Events: " << trace->getNumEvents() <<endl;
	trace->resetNumEvents();
}

long int getNumEvents(Trace *trace, int rank){
	return trace->getNumEvents();
}

void printClusteringCounters(Trace *trace, int rank){
	trace->printClusteringCounters(rank);

}

void setMarkerCommValue(Trace *trace, int input){
	trace->setMarkerCommValue(input);
}
int getMarkerCommValue(Trace *trace){
	return trace->getMarkerCommValue();
}


