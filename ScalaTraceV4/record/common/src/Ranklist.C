#include "Ranklist.h"
#include <stdlib.h>
#include <limits.h>
#include <stdio.h>
#include <iostream>
#include <string.h>
#include <vector>

void Ranklist::initRanklist(int rank){
	ranks.push_back(1);
	ranks.push_back(-1);
	ranks.push_back(rank);
	ranks.push_back(1);
	ranks.push_back(0);

	numRanks = 1;
}

Ranklist::Ranklist():
numRanks(0)
{}

Ranklist::Ranklist(int rank):
numRanks(0)
{
	initRanklist(rank);	
}

Ranklist::~Ranklist(){}

Ranklist::Ranklist(const Ranklist& copy):
ranks( *(copy.getRanks()) ),
numRanks( copy.getNumRanks() )
{}

void Ranklist::merge(Ranklist *other){
	int *rl1 = (int *)malloc(sizeof(int) * ranks.size());
	for(unsigned i=0; i<ranks.size(); i++)
		rl1[i] = ranks[i];

	const vector<int> *other_ranks = other->getRanks();
	int *rl2 = (int *)malloc(sizeof(int) * other_ranks->size());

	for(unsigned i=0; i<other_ranks->size(); i++)
		rl2[i] = other_ranks->at(i);

	int NumCommonRanks = merge_recursive_lists(&rl1, numRanks, rl2, other->getNumRanks());
	ranks.clear();
	ranks.insert(ranks.begin(), rl1, rl1 + size_recursive_list(rl1));
	numRanks += other->getNumRanks() - NumCommonRanks;


	free(rl1);
	free(rl2);
}
void Ranklist::exclude(Ranklist *other){
	int *rl1 = (int *)malloc(sizeof(int) * ranks.size());
	for(unsigned i=0; i<ranks.size(); i++)
		rl1[i] = ranks[i];

	const vector<int> *other_ranks = other->getRanks();
	int *rl2 = (int *)malloc(sizeof(int) * other_ranks->size());

	for(unsigned i=0; i<other_ranks->size(); i++)
		rl2[i] = other_ranks->at(i);


	//Make sure two ranklists are not identical
	if(other_ranks->size() == ranks.size()){
		bool identicalFlag=true;
		for(unsigned int i=0; i<ranks.size() && identicalFlag==true; i++)
			if(rl1[i]!=rl2[i])
				identicalFlag=false;
		if (identicalFlag)
			return;	
	}


	int NumCommonRanks = exclude_common_merge_recursive_lists(&rl1, numRanks, rl2, other->getNumRanks());
	ranks.clear();
	ranks.insert(ranks.begin(), rl1, rl1 + size_recursive_list(rl1));
	numRanks -=NumCommonRanks;
	cout << "Num: " << numRanks << " Ranklist: " <<toString() << endl;

	free(rl1);
	free(rl2);
}


/*
 * Returns the ranklist with the common ranks of rl2.
 * Caller need to free the returned ranklist.
 * Return NULL if there are no common ranks.
 */
Ranklist* Ranklist::getCommonRanklist(Ranklist* rl2) {
 // if(this->equals(rl2)){
    //both ranklists are equal
 //   return new Ranklist(*rl2);
  //}

  int *rl1_list = (int *)malloc(sizeof(int) * ranks.size());
  for(unsigned i = 0; i < ranks.size(); i++)
    rl1_list[i] = ranks.at(i);

  const vector<int> *rl2_ranks = rl2->getRanks();
  int *rl2_list = (int *)malloc(sizeof(int) * rl2_ranks->size());
  for(unsigned i = 0; i < rl2_ranks->size(); i++)
    rl2_list[i] = rl2_ranks->at(i);

  int count1 = numRanks;
  int count2 = rl2->getNumRanks();

  int *unrolled1 = (int *)malloc(sizeof(int) * count1);
  int *next1 = unrolled1;
  decompress_rl(rl1_list, count1, unrolled1);

  int *unrolled2 = (int *)malloc(sizeof(int) * count2);
  int *next2 = unrolled2;
  decompress_rl(rl2_list, count2, unrolled2);

  int *output = NULL;

  //Decompress function returns the sorted list
  while (next1 != unrolled1 + count1 && next2 != unrolled2 + count2) {
    int next;

    if (*next1 == *next2) {
      next = *next1++;
      next2++;
      //including only common ranks
      if(!output) {
	create_recursive_list(next, &output);
      } else {
	merge_single(next, &output);
      }
    } else if (*next1 < *next2) {
      next1++;
    } else {
      next2++;
    }
  }

  Ranklist* common_ranklist;
  if(output)
    {
      common_ranklist = new Ranklist();
      vector<int> common_ranks;
      common_ranks.insert(common_ranks.begin(), output,
			  output + size_recursive_list(output));
      common_ranklist->setRanks(common_ranks);
      common_ranklist->setNumRanks(Ranklist::memberCount(&common_ranks));
    }

  free(output);
  free(unrolled1);
  free(unrolled2);
  free(rl1_list);
  free(rl2_list);

  return common_ranklist;
}




string Ranklist::toString(){
	ostringstream rtn;
	if(numRanks > 0){
		rtn << ranks[0];
		for(unsigned i=1; i<ranks.size(); i++)
			rtn << " " << ranks[i] ;
	}
	return rtn.str();
}

void Ranklist::input(string& buf){
	stringstream ss (stringstream::in | stringstream::out);
	ss << buf;
	int elemt;
	while( ss >> elemt){
		ranks.push_back(elemt);
	}
	numRanks = Ranklist::memberCount(&ranks);
}

int Ranklist::getPackedSize(){
	return sizeof(int) + sizeof(int) * ranks.size() + sizeof(int);
}

void Ranklist::pack(void *buf, int bufsize, int *position, MPI_Comm comm){
	int size = ranks.size();
	PMPI_Pack(&size, 1, MPI_INT, buf, bufsize, position, comm);
	for(int i=0; i<size; i++)
		PMPI_Pack(&(ranks[i]), 1, MPI_INT, buf, bufsize, position, comm);
	PMPI_Pack(&numRanks, 1, MPI_INT, buf, bufsize, position, comm);
}


void Ranklist::unpack(void *buf, int bufsize, int *position, MPI_Comm comm){
	int size, value;
	PMPI_Unpack(buf, bufsize, position, &size, 1, MPI_INT, comm);
	for(int i=0; i<size; i++){
		PMPI_Unpack(buf, bufsize, position, &value, 1, MPI_INT, comm);	
		ranks.push_back(value);
	}
	PMPI_Unpack(buf, bufsize, position, &numRanks, 1, MPI_INT, comm);
}

void Ranklist::GenRankList(vector<int> val){
	for(unsigned i=0; i<val.size(); i++)
		ranks[i] = val[i];
}

int Ranklist::size_recursive_list(int *list) {
	if(!list) return 0;

	int cur = 1;
	int num_lists = list[0];
	for(int i = 0; i < num_lists; i++) {
		int num_rsds = -(list[cur]);
		cur += ((num_rsds+1)<<1);
	}
	return cur;
}

int Ranklist::merge_recursive_lists(int **range1, int count1, int *range2, int count2) {
	
	//Number of common ranks 
	int NumCommonRanks=0;

	// if range2 was NULL, nothing to add, so just bail.
	if (!range2) return NumCommonRanks;
	
	// if range1 was NULL, we just take range2 entirely.
	if (*range1 == NULL) {
		dup_recursive_list(range1, range2);
		return NumCommonRanks;
	}

	// otherwise, we need to do a full merge.  
	int unrolled1[count1+1];
	int *next1 = unrolled1;
	decompress_rl(*range1, count1, unrolled1);
	unrolled1[count1] = INT_MAX;

	int unrolled2[count2+1];
	int *next2 = unrolled2;
	decompress_rl(range2, count2, unrolled2);
	unrolled2[count2] = INT_MAX;


	int *output = NULL;

	while (next1 != unrolled1 + count1 || next2 != unrolled2 + count2) {
		int next;

		if (*next1 == *next2) {
			next = *next1++;
			next2++;
			NumCommonRanks++;
			
		} else if (*next1 < *next2) {
			next = *next1++;
		} else {
			next = *next2++;
		}

		if(!output) {
			create_recursive_list(next, &output);
		} else {
			merge_single(next, &output);
		}
	}

	free(*range1);
	*range1 = output;

	return NumCommonRanks;
}

int Ranklist::exclude_common_merge_recursive_lists(int **range1, int count1, int *range2, int count2) {
	
	//Number of common ranks 
	int NumCommonRanks=0;

	// if range2 was NULL, nothing to add, so just bail.
	if (!range2) return NumCommonRanks;
	
	// if range1 was NULL, we just take range2 entirely.
	if (*range1 == NULL) {
		dup_recursive_list(range1, range2);
		return NumCommonRanks;
	}

	// otherwise, we need to do a full merge.  
	int unrolled1[count1+1];
	int *next1 = unrolled1;
	decompress_rl(*range1, count1, unrolled1);
	unrolled1[count1] = INT_MAX;

	int unrolled2[count2+1];
	int *next2 = unrolled2;
	decompress_rl(range2, count2, unrolled2);
	unrolled2[count2] = INT_MAX;


	int *output = NULL;

	while (next1 != unrolled1 + count1 || next2 != unrolled2 + count2) {
		int next;
		
		if (*next1 == *next2) {
			next1++;
			next2++;
			NumCommonRanks++;
			
		}else{
			if (*next1 < *next2) {
				next = *next1++;
			} else {
				next = *next2++;
			}
			if(!output) {
				create_recursive_list(next, &output);
			} else {
				merge_single(next, &output);
			}
		}
	}

	free(*range1);
	*range1 = output;
	
	return NumCommonRanks;
}


/*void Ranklist::merge_recursive_lists(int **range1, int count1, int *range2, int count2) {
	// if range2 was NULL, nothing to add, so just bail.
	if (!range2) return;
	
	// if range1 was NULL, we just take range2 entirely.
	if (*range1 == NULL) {
		dup_recursive_list(range1, range2);
		return;
	}

	// otherwise, we need to do a full merge.  
	int unrolled1[count1+1];
	int *next1 = unrolled1;
	decompress_rl(*range1, count1, unrolled1);
	unrolled1[count1] = INT_MAX;

	int unrolled2[count2+1];
	int *next2 = unrolled2;
	decompress_rl(range2, count2, unrolled2);
	unrolled2[count2] = INT_MAX;

	int *output = NULL;

	while (next1 != unrolled1 + count1 || next2 != unrolled2 + count2) {
		int next;

		if (*next1 == *next2) {
			next = *next1++;
			next2++;
		} else if (*next1 < *next2) {
			next = *next1++;
		} else {
			next = *next2++;
		}

		if(!output) {
			create_recursive_list(next, &output);
		} else {
			merge_single(next, &output);
		}
	}

	free(*range1);
	*range1 = output;
}*/


void Ranklist::dup_recursive_list(int **dest, int *src) {
	if (*dest) {
		free(*dest);
	}

	if (src) {
		int len = size_recursive_list(src);
		*dest = (int*)malloc(sizeof(int) * len);
		memcpy(*dest, src, sizeof(int) * len);
	} else {
		*dest = NULL;
	}
}

static int cmpint(const void *p1, const void *p2){
	if( *((int*)p1) > *((int*)p2) ) return 1;
	else if( *((int*)p1) == *((int*)p2) ) return 0;
	else return -1;
}

void Ranklist::decompress_rl(int *rlist, int len, int *list){

	int count, depth, start_rank, stride, iters, i, j, k, cur, iter, begin, end, length;

	count = rlist[0];
	iter = 1;
	cur = 0;
	for(i=0; i<count; i++){
		depth = -rlist[iter++];
		start_rank = rlist[iter++];
		begin = end = cur;
		list[begin] = start_rank;

		for(j=0; j<depth; j++){
			iters = rlist[iter++];
			stride = rlist[iter++];
			length = end - begin + 1;
			for(cur=0; cur<length; cur++){
				for(k=1; k<iters; k++){
					list[end+cur*(iters-1)+k] = list[begin+cur] + stride*k;
				}
			}
			end = begin + length * iters - 1;
		}
		cur = end + 1;
	}
	qsort(list, len, sizeof(int), cmpint);
}

int Ranklist::create_recursive_list(int rank, int **list) {
	int *tlist = NULL;

	tlist = (int *) malloc(sizeof(int) * 5);
	if(!tlist) {
		exit(0);
	}

	tlist[0] = 1;
	tlist[1] = -1;
	tlist[2] = rank;
	tlist[3] = 1;
	tlist[4] = 0;

	*list = tlist;

	return 5; // size of created list 
}

void Ranklist::merge_single(int rank, int **list) {
	add_single(rank, list);
	if ((*list)[0] > 1) 
		while(merge_compress(list));
	return;
}

void Ranklist::add_single(int rank, int **list) {
	int *tlist, num_prsds, next_rank, i, num_rsds, cur, last, last_depth, last_start, last_1iter, last_1stride, last_adjusted;

	/* inits */
	tlist = *list;
	if(!tlist) {
		fprintf(stderr, "error: add_single pass NULL list\n");
		exit(0);
	}

	num_prsds = tlist[0];
	last_adjusted = 0;
	last = 1;
	cur = 1;

	/* determine last prsd in the list (also use cur for size of list) */
	for(i = 0; i < num_prsds; i++) {
		num_rsds = -(tlist[cur]);
		if(i == num_prsds - 1)
			last = cur;
		cur += ((num_rsds+1)<<1);
	}

	/* setup some useful vals for easier code reading */
	last_depth = tlist[last];
	last_start = tlist[last + 1];
	last_1iter = tlist[last + 2];
	last_1stride = tlist[last + 3];

	/* insert into existing prsd */
	if(last_depth == -1) {
		next_rank = last_start + last_1iter*last_1stride;

		if (next_rank == rank || next_rank == last_start) {
			tlist[last + 2]++;  // iter is now 2
			tlist[last + 3] += rank - next_rank; // sets the stride if this is the second element
			last_adjusted = 1;
		}
	}

	/* append a new prsd to the end of the list */
	if(!last_adjusted) {
		/* create new prsd */
		*list = tlist = (int *) realloc(tlist, sizeof(int) * (cur + 4));
		if(!tlist) {
			fprintf(stderr, "realloc tlist - add_single\n");
			exit(0);
		}
		tlist[0]++;
		tlist[cur] = -1;
		tlist[cur + 1] = rank;
		tlist[cur + 2] = 1;
		tlist[cur + 3] = 0;
	}

	return;
}


int Ranklist::merge_compress(int **list) {
	int *tlist, num_prsds, i, num_rsds, cur, last;
	int *depth_positions;

	/* inits */
	tlist = *list;
	if(!tlist) {
		fprintf(stderr, "error: merge_compress pass NULL list\n");
		exit(0);
	}

	num_prsds = tlist[0];

	//if no or only one prsd, nothing to compress
	if(num_prsds < 2){
		return 0;
	}

	depth_positions = (int *)calloc(num_prsds, sizeof(int));

	last = -1;
	cur = 1;

	/* find last elements */
	for(i = 0; i < num_prsds; i++) {
		num_rsds = -(tlist[cur]);
		depth_positions[i]=cur;
		if(i == num_prsds - 1)
			last = cur;
		cur += ((num_rsds+1)<<1);
	}

	//try to compress the last prsd with one of the previous prsds
	//check in reversed order
	for(i = num_prsds - 2 ; i >= 0; i--){
		if ( do_compress(list, last, depth_positions[i]) ){
			free(depth_positions);
			return 1;
		}
	}

	free(depth_positions);
	return 0;
}


int Ranklist::do_compress(int **list, int last, int prev){
	int *tlist = *list, i, num_rsds;
	int start;

	/* depths match - attempt to realloc & merge 
	 * example:
	 *    2 -2 448 2,16 4,2 -2 472 2,16 4,2
	 *
	 * compresses to
	 *    2 -3 448 2,24 2,16, 4,2
	 */
	if(tlist[prev] == tlist[last]) {
		/* ensure rsds match */
		num_rsds = -(tlist[last]);
		for(i = 0; i < (num_rsds * 2); i += 2) {
			if(tlist[last + i + 2] != tlist[prev + i + 2] || tlist[last + i + 3] != tlist[prev + i + 3])
				return 0;
		}
	
		//store the start rank of the last prsd
		start = tlist[last+1];

		/* reclaim some memory! */
		*list = tlist = (int *) realloc(tlist, sizeof(int) * (last + 2));
		if(!tlist) {
			fprintf(stderr, "malloc tlist - merge_compress\n");
			exit(0);
		}

		//move back to find two empty slots for an additional dimension
		for(i = last-1; i > prev+1; i--){
			tlist[i+2] = tlist[i];
		}

		tlist[0]--;          // decrement total prsd count
		tlist[prev]--;       // decrement the depth
		tlist[prev + 2] = 2; // set iters to 2
		tlist[prev + 3] = start - tlist[prev + 1]; // set stride

		return 1;
	}

	/* depths off by 1 - attempt to simply inc iters 
	 * example:
	 * 2  -3  216  2,120 4,16 4,2
	 *    -2  456  4,16 4,2
	 *
	 *    merges to:
	 * 2  -3  216  3,120 4,16 4,2
	 */
	else if(tlist[prev] == tlist[last] - 1) {
		/* ensure correct start val */
		if(tlist[prev + 1] + (tlist[prev + 2] * tlist[prev + 3]) != tlist[last + 1])
			return 0;

		/* ensure rsds match */
		num_rsds = -(tlist[last]);
		for(i = 0; i < (num_rsds * 2); i += 2) {
			if(tlist[last + 2 + i] != tlist[prev + 4 + i] || tlist[last + 3 + i] != tlist[prev + 5 + i])
				return 0;
		}

		tlist[0]--;        // decrement total prsd count
		tlist[prev + 2]++; // inc iters

		/* reclaim some memory! */
		*list = tlist = (int *) realloc(tlist, sizeof(int) * last);
		if(!tlist) {
			fprintf(stderr, "malloc tlist - merge_compress");
			exit(0);
		}

		return 1;
	}
  
	/* depths do not match */
	else
		return 0;
}

bool Ranklist::member_rank_helper(int rank_target, int *list, int start, int depth) {
	int i, rank, iters, stride;

	if(depth > -1)
		return false;

	iters = list[0];
	stride = list[1];

	for(i = 0, rank = start; i < iters; i++, rank += stride) {
		if(rank == rank_target)
			return true;
		if(depth != -1) {
			if(member_rank_helper(rank_target, list + 2, rank, depth + 1))
				return true;
		}
	}
	//VT_AK: member_rank_helper should return a zero if its not a member
	return false;
}

bool Ranklist::hasMember(int rank) {
	int pos, cur_prsd, depth, start;

	if(ranks.size() == 0)
	    return false;

	int *list = &ranks[0];
	pos = 1;
	cur_prsd = 0;


	while(cur_prsd < list[0]) {
		depth = list[pos];
		start = list[pos + 1];
		pos += 2;
		if(member_rank_helper(rank, list + pos, start, depth))
			return true;

		/* find next prsd */
		if(cur_prsd + 1 == list[0]) {
			break;
		}

		pos++;
		while(list[pos] > 0) {
			pos++;
		}

		cur_prsd++;
	}
	//VT_AK: Must return to verify that the node is not a memeber
	return false;
}


int Ranklist::memberCount(vector<int> *r) {
	if(!r || r->size() == 0)
		return 0;

	int *list = &(*r)[0];

	int num_lists = list[0];
	int total_members = 0;

	int list_start = 1;  // start at first list entry
	for (int i=0; i < num_lists; i++) {
		int depth = -list[list_start];

		// calculate product of all the iter counts in list to get member count
		if (depth != 0) {
			int cur_members = 1;
			for (int l=1; l <= depth; l++) {
				cur_members *= list[list_start + (2*l)];
			}
			total_members += cur_members;
		}
		list_start += 2 * (depth + 1); // skip to next list
	}
	return total_members;
}

/*
 * Returns the ranklist with the common ranks of rl2.
 * Caller need to free the returned ranklist.
 * Return NULL if there are no common ranks.
 */
/*Ranklist* Ranklist::getCommonRanklist(Ranklist* rl2) {
  if(this->equals(rl2)){
    //both ranklists are equal
    return new Ranklist(*rl2);
  }

  int *rl1_list = (int *)malloc(sizeof(int) * ranks.size());
  for(unsigned i = 0; i < ranks.size(); i++)
    rl1_list[i] = ranks.at(i);

  const vector<int> *rl2_ranks = rl2->getRanks();
  int *rl2_list = (int *)malloc(sizeof(int) * rl2_ranks->size());
  for(unsigned i = 0; i < rl2_ranks->size(); i++)
    rl2_list[i] = rl2_ranks->at(i);

  int count1 = numRanks;
  int count2 = rl2->getNumRanks();

  int *unrolled1 = (int *)malloc(sizeof(int) * count1);
  int *next1 = unrolled1;
  decompress_rl(rl1_list, count1, unrolled1);

  int *unrolled2 = (int *)malloc(sizeof(int) * count2);
  int *next2 = unrolled2;
  decompress_rl(rl2_list, count2, unrolled2);

  int *output = NULL;

  //Decompress function returns the sorted list
  while (next1 != unrolled1 + count1 && next2 != unrolled2 + count2) {
    int next;

    if (*next1 == *next2) {
      next = *next1++;
      next2++;
      //including only common ranks
      if(!output) {
	create_recursive_list(next, &output);
      } else {
	merge_single(next, &output);
      }
    } else if (*next1 < *next2) {
      next1++;
    } else {
      next2++;
    }
  }

  Ranklist* common_ranklist;
  if(output)
    {
      common_ranklist = new Ranklist();
      vector<int> common_ranks;
      common_ranks.insert(common_ranks.begin(), output,
			  output + size_recursive_list(output));
      common_ranklist->setRanks(common_ranks);
      common_ranklist->setNumRanks(Ranklist::memberCount(&common_ranks));
    }

  free(output);
  free(unrolled1);
  free(unrolled2);
  free(rl1_list);
  free(rl2_list);

  return common_ranklist;
}*/


/*
 * Removes the ranks present in rl
 */
void Ranklist::removeRanks(Ranklist* rl2) {
  int *rl1_list = (int *)malloc(sizeof(int) * ranks.size());
  for(unsigned i = 0; i < ranks.size(); i++)
    rl1_list[i] = ranks.at(i);

  const vector<int> *rl2_ranks = rl2->getRanks();
  int *rl2_list = (int *)malloc(sizeof(int) * rl2_ranks->size());
  for(unsigned i = 0; i < rl2_ranks->size(); i++)
    rl2_list[i] = rl2_ranks->at(i);

  int count1 = numRanks;
  int count2 = rl2->getNumRanks();

  int *unrolled1 = (int *)malloc(sizeof(int) * (count1 + 1));
  int *next1 = unrolled1;
  decompress_rl(rl1_list, count1, unrolled1);
  unrolled1[count1] = INT_MAX;

  int *unrolled2 = (int *)malloc(sizeof(int) * (count2+1));
  int *next2 = unrolled2;
  unrolled2[count2] = INT_MAX;

  int *output = NULL;

  //Decompress function returns the sorted list
  while (next1 != unrolled1 + count1 || next2 != unrolled2 + count2) {
    int next;

    if (*next1 == *next2) {
      next1++;
      next2++;
    } else if (*next1 < *next2) {
      next = *next1++;
      //including only ranks that are "not" part of rl passed
      if(!output) {
	create_recursive_list(next, &output);
      } else {
	merge_single(next, &output);
      }
    } else {
      next2++;
    }
  }

  ranks.clear();
  ranks.insert(ranks.begin(), output, output + size_recursive_list(output));
   
  numRanks = Ranklist::memberCount(&ranks);

  free(output);
  free(unrolled1);
  free(unrolled2);
  free(rl1_list);
  free(rl2_list);
}

/*
 * Gives decompressed ranks.
 * Parameters: ranks_list[output]: list of decompredded ranks
 * */
void Ranklist::decompressRanks(int *ranks_list){
  if(ranks_list == NULL){
    cerr << __func__ << "ranks list shouldn't be null";
    return;
  }
  int *rl_list = (int *)malloc(sizeof(int) * ranks.size());
  for(unsigned i = 0; i < ranks.size(); i++)
    rl_list[i] = ranks.at(i);

  decompress_rl(rl_list, numRanks, ranks_list);
  free(rl_list);
}
