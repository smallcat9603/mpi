#ifndef _EXPNODE_H
#define _EXPNODE_H

#include "ecommon.h"
#include "vmaster.h"
#include "rsd_hlink.h"

class	EXPNODE {
public:
	EXPNODE() {}
	~EXPNODE() { clear(); }
	void clear() ;
	void print() ;
	VMASTER	*ext ;
	vector<VMASTER *> vm ;	
	vector<RSD_HLINK *> hq ;

	void addtrace(char	*dirname , char	*fname, int node = -1) ;
	void readBase(char	*dirname , char	*fname, int node) ;
	int map_elements() ;
	int rank ;
	void extrapolate(int target, char* dirname) ;
	int comp2Val(float a, float b) ;

protected:
	char filename[256] ;
};

#endif

