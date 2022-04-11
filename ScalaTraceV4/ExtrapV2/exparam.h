#ifndef _EXPARAM_H
#define _EXPARAM_H

#include "ParamValue.h"
#include "ecommon.h"
#include "eutil.h"

class EPARAM{
public:
	EPARAM(ValueSet *VS);
	vector<DIM> pxyz;
	int pmaxdepth;

	int num_ranklist;
	int x, y, z;
	void decode(int x, int y, int z);
};

#endif
