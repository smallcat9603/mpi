#include <iostream>
#include "ecommon.h"
#include "expnode.h"
#include "extrap.h"
using namespace std;

EPOLOPT gopt;

int main(int argc, char **argv)
{
	char	opt;
	char	filename[256];

	memset(&gopt, 0x00, sizeof(EPOLOPT));
	while((opt = (char) getopt(argc, argv, "d:t:")) != -1){
		switch(opt){
			case 't':
				gopt.target = atoi(optarg); break;
			case 'd':
				gopt.dir_name = optarg; break;
			default:
				cerr << "Usage: "<<argv[0]<<" -t target -d directory tracefile .."<<"\n";
				exit(EXIT_FAILURE);
		}
	}
	printf("traget=%d, dir_name=%s\n", gopt.target, gopt.dir_name);

	EXPNODE *pExt = new EXPNODE();

#ifdef DROPEVENT
	//lcs_drop(argc, argv, optind, gopt.dir_name);
	while (optind < argc){
		sprintf(filename, "%s_out", argv[optind]);
		pExt->addtrace(gopt.dir_name, filename, atoi(argv[optind++]));
	}
	pExt->readBase(gopt.dir_name, filename, gopt.target);
#else
	while( optind < argc ) {
		pExt->addtrace(gopt.dir_name, argv[optind++]);
	}
	pExt->readBase(gopt.dir_name, argv[-- optind], gopt.target);
#endif
	pExt->extrapolate(gopt.target, gopt.dir_name);
	pExt->print();
	pExt->clear();
	return 0;
}
