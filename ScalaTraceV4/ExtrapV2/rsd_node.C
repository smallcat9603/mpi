/*this is the attribute of each Event, for ranklist*/
#include "ecommon.h"
#include "rsd_node.h"
#include <assert.h>
#include <string.h>
#include <limits.h>
#include <math.h>
#include "Param.h"

RSD_NODE::RSD_NODE(VMASTER *parent, Event *event)
{

	this->parent = parent ;
	q = event ;
	const vector <int> *rlist = q->getRanklist()->getRanks();
	num_ranklist = (*rlist)[0];
	maxdepth = 1;
	//commdepth = 1;

	CAL_DIM(rlist, &xyz, &maxdepth);
}

RSD_NODE::~RSD_NODE(){
	
	map<ValueSet*, EPARAM*> ::iterator it;
	for(it=VSEP.begin(); it!=VSEP.end(); it++)
		delete it->second;
}

int  RSD_NODE::get_maxdepth()
{
	if( is_sendop(q->getOp()) ){
		map<int, Param*>::iterator it = (*(q->getParams())).find(DEST);
		assert(it!=(*(q->getParams())).end());
		Param* temp_param = it->second;
		vector<ValueSet*> *Parav = temp_param->getParamValues();
		for(unsigned i=0; i<(*Parav).size(); i++){
			EPARAM *eparam = new EPARAM((*Parav)[i]);  //remember to delete after extrapolation!!!!!
			VSEP.insert(pair<ValueSet*, EPARAM*>((*Parav)[i], eparam)); //this is only contain DEST param
			maxdepth = MAX(maxdepth, eparam->pmaxdepth);
		}
	}
	return maxdepth;
}

// to decide x,y,z dimension 
// As they are not placed in order , decide x,y,z by comparing stride
// with x*y
void RSD_NODE::decode( int	x, int	y, int z)
{
	this->x = x ; 
	this->y = y ; 
	this->z = z ;
	
	for(unsigned i=0; i<xyz.size(); i++){
		if(xyz[i].dimension == 3){
			xyz[i].zdepth = xyz[i].single_list[2];
			xyz[i].zstride = xyz[i].single_list[3];
			xyz[i].zmask = 1;
			xyz[i].ydepth = xyz[i].single_list[4];
			xyz[i].ystride = xyz[i].single_list[5];
			xyz[i].ymask = 1;
			xyz[i].xdepth = xyz[i].single_list[6];
			xyz[i].xstride = xyz[i].single_list[7];
			xyz[i].xmask = 1;
		}
		if(xyz[i].dimension == 2){
			xyz[i].ydepth = xyz[i].single_list[2];
			xyz[i].ystride = xyz[i].single_list[3];
			xyz[i].ymask = 1;
			xyz[i].xdepth = xyz[i].single_list[4];
			xyz[i].xstride = xyz[i].single_list[5];
			xyz[i].xmask = 1;
		}
		if(xyz[i].dimension == 1){
			if(xyz[i].single_list[3] >= x){
				xyz[i].ydepth = xyz[i].single_list[2];
				xyz[i].ystride = xyz[i].single_list[3];
				xyz[i].ymask = 1;
			} else {
				xyz[i].xdepth = xyz[i].single_list[2];
				xyz[i].xstride = xyz[i].single_list[3];
				xyz[i].xmask = 1;
			}	
		}		
	}
	map<ValueSet*, EPARAM*>::iterator it;
	for(it=VSEP.begin(); it!=VSEP.end(); it++){
		it->second->decode(x, y, z);
	}
}

/*
// revert to the integer array
void	RSD_NODE::encode()
{
	int	idx = 3 ;
	if ( ymask ) {}
	if ( xmask ) {}
}*/

