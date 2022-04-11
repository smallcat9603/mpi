#include "exparam.h"


EPARAM::EPARAM(ValueSet *VS)
{
	const vector<int> *rlist = VS->getRanklist()->getRanks();
	num_ranklist = (*rlist)[0];
	pmaxdepth = 1;
	
	CAL_DIM(rlist, &pxyz, &pmaxdepth);
}

void EPARAM::decode(int x, int y, int z)
{
	this->x = x;
	this->y = y;
	this->z = z;

	for(unsigned i=0; i<pxyz.size(); i++){
		if(pxyz[i].dimension == 3){
			pxyz[i].zdepth = pxyz[i].single_list[2];
			pxyz[i].zstride = pxyz[i].single_list[3];
			pxyz[i].zmask = 1;
			pxyz[i].ydepth = pxyz[i].single_list[4];
			pxyz[i].ystride = pxyz[i].single_list[5];
			pxyz[i].ymask = 1;
			pxyz[i].xdepth = pxyz[i].single_list[6];
			pxyz[i].xstride = pxyz[i].single_list[7];
			pxyz[i].xmask = 1;
		}
		if(pxyz[i].dimension == 2){
			pxyz[i].ydepth = pxyz[i].single_list[2];
			pxyz[i].ystride = pxyz[i].single_list[3];
			pxyz[i].ymask = 1;
			pxyz[i].xdepth = pxyz[i].single_list[4];
			pxyz[i].xstride = pxyz[i].single_list[5];
			pxyz[i].xmask = 1;
		}
		if(pxyz[i].dimension == 1){
			if(pxyz[i].single_list[3] >= x){
				pxyz[i].ydepth = pxyz[i].single_list[2];
				pxyz[i].ystride = pxyz[i].single_list[3];
				pxyz[i].ymask = 1;
			} else {
				pxyz[i].xdepth = pxyz[i].single_list[2];
				pxyz[i].xstride = pxyz[i].single_list[3];
				pxyz[i].xmask = 1;
			}	
		}
	}		

}
