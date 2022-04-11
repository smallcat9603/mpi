/**
 * Author: Xing Wu
 */

#ifndef __RANKLIST_H__
#define __RANKLIST_H__

#include <vector>
#include <string>
#include <sstream>
#include <cassert>
#include <mpi.h>
#include <stdio.h>
#include <cstdlib>
#include <iostream>
#include <cstring>
#include "limits.h"

using namespace std;

class Ranklist {
	private:
		vector<int> ranks;
		int numRanks;

		void initRanklist(int rank);

		int size_recursive_list(int *list);
		int merge_recursive_lists(int **range1, int count1,  int *range2, int count2);
		int exclude_common_merge_recursive_lists(int **range1, int count1, int *range2, int count2);
		//void merge_recursive_lists(int **range1, int count1,  int *range2, int count2);
		void decompress_rl(int *rlist, int len, int *list);
		void dup_recursive_list(int **dest, int *src);
		int create_recursive_list(int rank, int **list);
		void merge_single(int rank, int **list);
		void add_single(int rank, int **list);
		int merge_compress(int **list);
		int do_compress(int **list, int last, int prev);
		bool member_rank_helper(int rank_target, int* list, int start, int depth);

	public:
		Ranklist();
		Ranklist(int rank);
		~Ranklist();

		Ranklist(const Ranklist& copy);

		bool hasMember(int rank);

		static int memberCount(vector<int> *r);

		Ranklist* getCommonRanklist(Ranklist* rl2);

		void removeRanks(Ranklist* rl);

		void decompressRanks(int *ranks);

		void GenRankList(vector<int> val);

		const vector<int> *getRanks() const{
			return &ranks;
		}
		void setRanks(const vector<int>& _ranks){
			ranks = _ranks;
		}

		void set_ext_Ranks(vector<int> &_ranks){
			ranks = _ranks;
		}

		bool equals(Ranklist* r){
			return ranks == *(r->getRanks());
		}

		const int getNumRanks() const{
			return numRanks;
		}
		void setNumRanks(int n){
			numRanks = n;
		}
		int getFirstRank(){
			assert(ranks.size() >= 5);
			return ranks[2];
		}
		void merge(Ranklist* other);

		int getPackedSize();
		void pack(void *buf, int bufsize, int *position, MPI_Comm comm);
		void unpack(void *buf, int bufsize, int *position, MPI_Comm comm);

		string toString();
		void input(string& buf);
		void decompressRL(int *rlist, int len, int *list)
		{
			decompress_rl(rlist, len, list);
		}
		void exclude(Ranklist *other);
};

#endif /*__RANKLIST_H__*/
