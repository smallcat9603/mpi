#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
using namespace std;


#define IOTIME "52"
int main(int argc, char **argv){
	char filename[100];
	if(argc < 2){
		cerr << "usage: "<< argv[0] <<" <trace> " <<endl;
		return 1;
	}
	sprintf(filename, "%s", argv[1]);
	stringstream ss;
	
	ifstream tracefile(filename);
	string line;
	long int time=0;
	while(getline(tracefile, line)){
		if(line.substr(0,2) == IOTIME){
			int pos = line.find(' ');
			string temp = line.substr(pos+1, line.length()-pos);
			int deltime;
			ss << temp;
			ss >> deltime;
			time += deltime;
		}	
	}
	cout<<"time = "<<time<<"\n";
	return 0;
}
