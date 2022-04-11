#include <iostream>
#include <string>
#include <vector>
#include <cmath>
#include <algorithm>
using namespace std;

extern int my_size;
vector <string> charextract(string filename);
bool charnamematch(vector <string> record0, vector <string> record1);
string cacu_num(string filename,string num,int done_flag);
void stringtoformat(vector <string> &record);
void filenamemerge(vector<string> &record0,vector<string> &record1);
void opstrmerge(string &str0, string &str1);
vector <string> opgetsubstring(string str);
void maincharmerge(vector <string> &vect0, vector <string> *vect1);
vector<int> ranksort(int n);
string getCharForRank(string str, int rank, vector<int> rank_sort, const vector<int> *ranklist);
void div_format(string &input, Ranklist *list);
