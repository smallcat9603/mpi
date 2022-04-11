#ifndef _RSD_NODE_H
#define _RSD_NODE_H

#include "ecommon.h"
#include "vmaster.h"
#include "Event.h"
#include "exparam.h"
#include <map>

class VMASTER;



class RSD_NODE {
public:
	RSD_NODE (VMASTER *parent, Event *event) ;
	~RSD_NODE();
	void decode(int	x, int	y, int z) ;//modified for 3d
	void encode() ;
	int  get_maxdepth();

	VMASTER	*parent ;
	Event *q ;

	map<ValueSet*, EPARAM*> VSEP;

	int x, y, z;
	int num_ranklist ;
	int maxdepth; //max dimension of all ranklist
	//int commdepth; //dimension of communication

	vector<DIM> xyz; //for multiple ranklist merge together
	void extrapolate();

} ;

#endif
