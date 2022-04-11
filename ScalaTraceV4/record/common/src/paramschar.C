/**********************************************************************************
 *This is all of the subroutines used in char (like filename) merge
 *
 * author:Xiaoqing Luo
 * date :2013-6-5
 * finally revise:2013-6-29
 * ********************************************************************************/


#include <iostream>
#include <string>
#include <vector>
#include <cmath>
#include <algorithm>
#include <stdio.h>
//#include "paramschar.h"
#include "ParamValue.h"
using namespace std;


// to extract numbers from the filename, and record the remaine substring of filename, and the number extracted. also record the position of numbers
vector <string> charextract(string filename)
{
	int length,i,count=0;
	char tem;
	int done_flag = 0; //to record if the filename was transfer to [ ]format
	vector <string> record;

	// record[0] records the remaining string in a filename
	// record[1] records the extracted numbers
	// record[2] records the position of char numbers(for example, '12' in a filename record as 2 chars "0""1" and 2 position here)
	// record[3] records the position of int numbers(for example,'12' record as one int-type number 12 here)
	string tem_record[4];

	length = filename.size();

	//initiate record[0]
	tem_record[0] = filename; 
	record.push_back(filename);
	
	for(i=0;i<length;i++)
	{
		if(int(*(filename.begin()+i))>=48 && int(*(filename.begin()+i))<=57) // compare the ascii code, to tell which is a number
		{
			// to erase the number in filename
			tem_record[0].erase(tem_record[0].begin()+i-count);

			//to record the number as one char one number ('12' means 2 numbers here)
			tem_record[1].append(&filename[i],1);

			// record the position of numbers
			tem = char(i);
			tem_record[2].append(&tem,1);
			count += 1;
		}
		if(*(filename.begin()+i) == '(' || *(filename.begin()+i) == ' ' || *(filename.begin()+i) == ')')
		{
			tem_record[0].erase(tem_record[0].begin()+i-count);
			done_flag = 1;
			count += 1;
		}
	}

	// caculate the exact position of a number
	tem_record[3] = cacu_num(filename,tem_record[2],done_flag);

	// store the records as vector
	for(i=0;i<4;i++)
	{
		record.push_back(tem_record[i]);
	}
	tem = char(done_flag);
	tem_record[0] = string(&tem);
	record.push_back(tem_record[0]); //record the done_flag, whether the filename is in [] format
	stringtoformat(record);
	return record;
}


// tell if two events matched or not
bool charnamematch(vector <string> record0, vector <string> record1)
{

	int length,length0,i,flag=0;
	length = record0.at(3).length();
	length0 = record1.at(3).length();
	if(length>0) // if the filename has numbers
	{ 
		if(record0.at(1) == record1.at(1) && record0.at(4) == record1.at(4))
		{
			return true;
		}
		else
		{
			return false;
		}
	}
	else // if the filename does not have numbers
	{
		if(record0.at(1) == record1.at(1) && length0 == length)
		{
			return true; 
		}
		else
		{
			return false;
		}
	}
}

string cacu_num(string filename,string num,int done_flag) // to caculate the number and begins position of numbers
{
	if(done_flag == 0){
		int length,i;

		// count is to record the number of int-type('12' for int 12) numbers in a filename
		int count = 1; 
		char tem;
		string number;
		length = num.length();	

		int *p = new int[length];
		p[0] = int(*(num.begin()));
		tem = char(p[0]);
		number.append(&tem,1);
		for(i=1;i<length;i++)
		{
			p[i] = int(*(num.begin()+i));

			// if the position is not contiguous count+1,
			if(p[i]-p[i-1]>1)
			{
				count += 1;
				tem = char(p[i]-i);
				number.append(&tem,1);
			}
		}

		tem = char(count);
		number.append(&tem,1);
		delete p;
		return number;
	}
	else if(done_flag == 1)
	{
		int i,count = 0,flag = 0,counter = 0;
		int length =  filename.length();
		char tem;
		string number;
		for(i=0;i<length;i++)
		{
			if(flag == 0)
			{
				if(*(filename.begin()+i)!= '(')
					count += 1;
				else
				{
					flag = 1;
					counter += 1;
					tem = char(count);
					number.append(&tem,1);
				}
			}	
			else if(flag == 1)
			{
				if(*(filename.begin()+i) == ')')
				{
					flag = 0;
				}
			}

		}
			
		tem = char(counter);
		number.append(&tem,1);
		return number;
	}
}


// to translate strings into vector to record the number of each numes, use -1 to separate two numbers here
void stringtoformat(vector <string> &record)
{
	int done_flag = int(*(record.at(5).begin()));
	int length,i;
	string filename = record.at(0);
	string num;
	if(done_flag == 1)
	{
		int flag = 0;
		length = filename.length();
		for(i=0;i<length;i++)
		{
			if(*(filename.begin()+i)=='(')
				flag = 1;
			if(flag == 1)
				num.append(&(*(filename.begin()+i)),1);
			if(*(filename.begin()+i)==')')
				flag = 0;
		}
		record.push_back(num);
	}
	else if(done_flag == 0)
	{
		char tem[2] = {')','('};
		length = record.at(3).length();
		int p[length];
		p[0] = int(*(record.at(3).begin()));
		num.append(&tem[1],1);
		num.append(&(*(filename.begin()+p[0])),1);
		for(i=1;i<length;i++)
		{
			p[i] = int(*(record.at(3).begin()+i));
			if(p[i]-p[i-1]>1)
			{
				num.append(tem,2);
			}
			num.append(&(*(filename.begin()+p[i])),1);
		}
		num.append(tem,1);
		record.push_back(num);
	}
}




// to merge the filename
void filenamemerge(vector<string> &record0,vector<string> &record1) //num is the number of numbers in a filename
{
	if(record0.at(2).length() == 0 && record1.at(2).length() == 0)
	{}
	else
	{
		int number = int(*(record0.at(4).end()-1));
		int length0 = record0.at(6).length();
		int length1 = record1.at(6).length();
		string str0 = record0.at(6);
		string str1 = record1.at(6);
		string tem0[number],tem1[number],tem,temp = record0.at(1);
		char braket[2] = {'(',')'};

		int i,tag = 0;
		for(i=0;i<length0;i++)
		{
			if(*(str0.begin()+i)!= '(' && *(str0.begin()+i)!= ')')
				tem0[tag].append(&(*(str0.begin()+i)),1);
			else if(*(str0.begin()+i) == ')')
				tag += 1;
		}
		tag = 0;
		for(i=0;i<length1;i++)
		{
			if(*(str1.begin()+i)!= '(' && *(str1.begin()+i)!= ')')
				tem1[tag].append(&(*(str1.begin()+i)),1);
			else if(*(str1.begin()+i) == ')')
				tag += 1;
		}

		for(i=0;i<number;i++)
		{
			opstrmerge(tem0[i],tem1[i]);
			tem0[i].insert(0,&braket[0],1);
			tem0[i].append(&braket[1],1);
			tem.append(tem0[i]);
		}
		for(i=number-1;i>=0;i--)
		{
			int position = int(*(record0.at(4).begin()+i));
			temp.insert(position,tem0[i]);

		}
		record0.at(6) = tem;
		record0.at(0) = temp;
	}
}

void opstrmerge(string &str0, string &str1)
{
	vector <string> vect0,vect1;
	vect0 = opgetsubstring(str0);
	vect1 = opgetsubstring(str1);
	unsigned i,j,flag;
	char tem = ' ';
	for(i=0;i<vect1.size();i++)
	{
		flag = 0;
		for(j=0;j<vect0.size();j++)
		{
			if(vect0.at(j) == vect1.at(i))
				flag = 1;
		}
		if(flag == 0)
		{
			str0.append(&tem,1);
			str0.append(vect1.at(i));
		}
	}
	
}

vector <string> opgetsubstring(string str)
{
	unsigned i;
	string tem;
	vector <string> vec;
	for(i=0; i<str.length();i++)
	{
		if(*(str.begin()+i) != ' ')
			tem.append(&(*(str.begin()+i)),1);
		else
		{
			vec.push_back(tem);
			tem.erase(0,tem.length());
		}
	}
	vec.push_back(tem);
	return vec;
}


bool maincharmerge(vector <string> &vect0, vector <string> *vect1, int type)
{
	int length0 = vect0.size(),length1 = (*vect1).size();
	vector <string> record0[length0], record1[length1];
	int i,j;
	for(i=0;i<length0;i++)
		record0[i] = charextract(vect0.at(i));
	for(i=0;i<length1;i++)
		record1[i] = charextract((*vect1).at(i));


	int flag;
	if(type == 0){ //compress inter-node
		for(i=0; i<length1; i++){
			if(!charnamematch(record0[i], record1[i]))
				return false;
		}
	}
	for(i=0;i<length1;i++)
		{
			flag = 0;
			for(j=0;j<length0;j++)
			{
				if(charnamematch(record1[i],record0[j]))
				{
					filenamemerge(record0[j],record1[i]);
					flag = 1;
					vect0.at(j) = record0[j].at(0);
				}
			}
			if(flag == 0 && type != 0){
				vect0.push_back((*vect1).at(i));
			}
		}
	return true;
}

//sort the number as the same order as the inter-node merge
vector<int> ranksort(int n)
{
	vector<int> sort_rank;
	sort_rank.push_back(0);
	int i,j;
	double tem = 2.0;
	int layer = int(ceil(log(double(n+1))/log(tem))-1);
	vector<int> ::iterator it;


	if(layer>0){
		for(i=0; i<layer; i++){
			for(j = int(pow(tem,i)-1); j<int(pow(tem,(i+1))-1); j++){
				it = find(sort_rank.begin(),sort_rank.end(),j);
				it++;
				if(j+pow(tem,i)<n){
					if(j == int(pow(tem,(i+1))-2))
						sort_rank.push_back(int(j+pow(tem,i)));
					else
						sort_rank.insert(it, int(j+pow(tem,i)));
				}
				it++;
				if(j+pow(tem,(i+1))<n){
					if(j == int(pow(tem,(i+1))-2))
						sort_rank.push_back(int(j+pow(tem,(i+1))));
					else
						sort_rank.insert(it, int(j+pow(tem,i+1)));
				}
			}
		}
	}
	else if(n == 0){
		cerr <<"number of ranks must be >0"<<endl;
		exit(0);
	}
	return sort_rank;
}

/*
string getCharForRank(string str, int rank, vector<int> rank_sort){
	if(str.find("(") == string::npos)
		return str;

	cout<<"string="<<str<<"size="<<str.length()<<"\n";

	int i, tag = 0;
	vector<string> nrecord;
	vector<int> postart,poend;
	string temp;

	for(i=0; i<str.length(); i++){
		if(*(str.begin()+i) == '('){
			tag = 1;
			postart.push_back(i);
		}
		if(tag == 1)
			temp.append(&(*(str.begin()+i)),1);
		if(*(str.begin()+i) == ')'){
			tag = 0;
			nrecord.push_back(temp);
			temp.erase(0,temp.length());
			poend.push_back(i);
		}
	}

	temp.erase(0,temp.length());

	for(i = nrecord.size()-1; i >= 0; i--){
		nrecord.at(i).erase(0,1);
		nrecord.at(i).erase(nrecord.at(i).length()-1,1);
		if(nrecord.at(i).find(" ") == string::npos){
			str.insert(postart.at(i),nrecord.at(i),0,nrecord.at(i).length());
			str.erase(postart.at(i)+nrecord.at(i).length(), poend.at(i)-postart.at(i)+1);
		}
		else{
			vector<string> num;
			for(unsigned j=0; j<nrecord.at(i).length(); j++){
				if(*(nrecord.at(i).begin()+j) == ' '){
					num.push_back(temp);
					temp.erase(0,temp.length());
				}
				else
					temp.append(&(*(nrecord.at(i).begin()+j)),1);
			}

			num.push_back(temp);
			temp.erase(0,temp.length());
			assert(num.size() == rank_sort.size());
			vector<int> ::iterator it;
			it = find(rank_sort.begin(), rank_sort.end(), rank);
			if(it != rank_sort.end()){
				int rposi = it - rank_sort.begin();
				str.insert(postart.at(i), num.at(rposi), 0, num.at(rposi).length());
				str.erase(postart.at(i)+num.at(rposi).length(), poend.at(i)-postart.at(i)+1);
			}
		}
	}
	if(str.find("  ")!=string::npos && str.find("  ") == str.length()-2)
		str.erase(str.find("  "), 2);
	return str;
	
}
*/

string getCharForRank(string str, int rank, vector<int> rank_sort, const vector<int> *ranklist){
	if(str.find("(") == string::npos)
		return str;

	int sstart=0, send=0;
	while((sstart=str.find("(")) != string::npos){
		send = str.find(")");
		string snum = str.substr(sstart+1, send-sstart-1);
		if(snum.find(' ')==string::npos){
			str.erase(sstart,1);
			str.erase(str.find(")"),1);
		}else{
			vector<int> num;
			while(snum.length()>0){
				if(snum.find(' ')!=string::npos){
					num.push_back(atoi((snum.substr(0, snum.find(' '))).c_str()));
					snum.erase(0, snum.find(' ')+1);
				} else {
					num.push_back(atoi(snum.c_str()));
					snum.clear();
				}
			}
			int new_num;
			if(num[0] == -1){
				assert(num.size() == 4);
				int start = ranklist->at(2);
				int size = ranklist->at(3);
				int strid = ranklist->at(4);
				int pos = (rank-start)/strid;
				assert(pos < num[2]);
				new_num = num[1] + pos*num[3];
			} else {
				assert(num.size() == rank_sort.size());
				vector<int>::iterator it = find(rank_sort.begin(), rank_sort.end(), rank);
				new_num = num[it-rank_sort.begin()]; 
			}
			char string_num[30];
			sprintf(string_num, "%d", new_num);
			str.erase(str.begin()+sstart, str.begin()+send+1);
			str.insert(sstart, string_num);
		}
	}
	if(str.find("  ")!=string::npos && str.find("  ")==str.length()-2)
		str.erase(str.find("  "),2);
	return str;
}

/*change the format of filename into RSD (-1 start size strid)*/
void div_format(string &input, Ranklist *list){
	if(input.find("-1")!=string::npos)
		return;
	size_t sstart=0, send=0;
	while((sstart = input.find("(",send)) != string::npos){
		send = input.find(")",sstart);
		string snum = input.substr(sstart+1, send-sstart-1);
		if(snum.find(' ')!=string::npos){
			vector<int> sortranks = ranksort(my_size);
			vector<int> num;
			while(snum.length()>0){
				if(snum.find(' ')!=string::npos){
					num.push_back(atoi((snum.substr(0, snum.find(' '))).c_str()));
					snum.erase(0, snum.find(' ')+1);
				} else {
					num.push_back(atoi(snum.c_str()));
					snum.clear();
				}
			}
			for(unsigned i=0; i<sortranks.size(); i++){
				if(!list->hasMember(sortranks[i]))
					sortranks.erase(sortranks.begin()+i);
			}
			assert(num.size()==sortranks.size());
			int start=-1, strid=-1, num1=-1, num0=-1;
			for(unsigned i=0; i<my_size; i++){
				vector<int>::iterator it1 = find(sortranks.begin(), sortranks.end(), (int)i);
				if(it1 == sortranks.end())
					continue;
				if(num0 == -1){
					start = num[it1-sortranks.begin()];
					num0 = start;
				} else if (num1 == -1){
					num1 = num[it1-sortranks.begin()];
					strid = num1 - num0;
					num0 = num1;
				} else {
					num1 = num[it1-sortranks.begin()];
					if(num1-num0 != strid)
						return;
					num0 = num1;
				}
			}
			
			char string_num[30];
			sprintf(string_num, "-1 %d %d %d", start, num.size(), strid);
			input.erase(input.begin()+sstart+1, input.begin()+send);
			input.insert(sstart+1, string_num);
			send = input.find(")", sstart); 
		}
	}
	return;
}
