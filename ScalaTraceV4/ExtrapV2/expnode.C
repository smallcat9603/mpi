#include "ecommon.h"
#include "eutil.h"
#include "expnode.h"
#include <assert.h>
#include <string.h>
#include <limits.h>
#include <math.h>

// add trace files for sources
void	EXPNODE::addtrace(char	*dirname , char	*filename, int rank)
{
	char fullpath[256] = {0} ;
	sprintf(fullpath, "%s/%s", dirname, filename) ;
	VMASTER *t;
	if(rank==-1)
		t = new VMASTER(fullpath, atoi(filename)) ;
	else
		t = new VMASTER(fullpath, rank) ;
	t->scan_dimension() ;
	t->scan_sizes();
	vm.push_back(t) ;
}

// add a target file
// read the last file into mem -> extrapolate
void	EXPNODE::readBase(char	*dirname , char	*filename , int	target)
{
	printf("Base file name: %s\n", filename);
	char fullpath[256] = {0} ;
	sprintf(fullpath, "%s/%s", dirname, filename) ;
	ext = new VMASTER(fullpath, target) ;
	ext->rank = target ;
	ext->lcd_vec.clear() ;
	ext->scan_dimension() ;
#ifdef CG
	ext->maxDepth = 2;
#endif
	ext->_decompose(ext->rank , ext->maxDepth ) ;
}

	
int	EXPNODE::map_elements()
{
	Event *e, *evm;

	assert(ext) ;
	// init iterator to denote the first element
	for (vector<VMASTER *>::iterator vm_it = vm.begin() ;vm_it != vm.end() ; vm_it ++ ) {
		(*vm_it)->vq_it = (*vm_it)->vq.begin() ;
	}

	for ( vector<RSD_NODE *>::iterator it = ext->vq.begin(); it != ext->vq.end(); it ++ ) { //to map if the number of dest are equal among the tracefiles
		RSD_HLINK	*rsd_hlink = new RSD_HLINK ;
		e = (*it)->q;
		map<int, Param*>::iterator it;
		for (vector<VMASTER *>::iterator vm_it = vm.begin(); vm_it != vm.end(); vm_it ++ ) { //map every trace togther
			evm = (*((*vm_it)->vq_it))->q;
#ifndef CG
			if(evm->getOp()!=e->getOp()) {
				cout<<"Event "<<e->getId()<<" doesn't get the corresponding event on other trace file"<<"\n";
				return -1;  //didn't consider about loops and vector size of each paramvalue.
			}
			if((*(evm->getParams())).size() != (*(e->getParams())).size()) {
				cout<<"size of parameters of event "<<e->getId()<<" doesn't equal"<<"\n";
				return -1;
			}
			/*map<int, Param*>::iterator itmv;
			for(it=(*(e->getParams())).begin(); it!=(*(e->getParams())).end(); it++){
				itmv = (*(evm->getParams())).find(it->first);
				if((*(itmv->second->getParamValues())).size() != (*(it->second->getParamValues())).size()){
					cout<<"size of parametersize of event"<<e->getId()<<"doesn't equal"<<"\n";
					return -1;
				}
			}*/
#endif
			rsd_hlink->hlink.push_back( *((*vm_it)->vq_it) ) ; //push_back each event of every trace file
			rsd_hlink->hsize ++ ;
			(*vm_it)->vq_it ++ ;
		}
		hq.push_back(rsd_hlink) ;
	}
	return 1;
}

int	EXPNODE::comp2Val(float	a, float	b)
{
	int	absame = 0;
	if ( a < 0 || b < 0) return 0 ;
	if ( a >= 0 && b >= 0 && a == b ) absame = 1 ;
	return ( absame ) ;
}

void	EXPNODE::extrapolate(int	target, char *dirname)
{
	ext->rank = target ;

	sprintf(filename,"%s/e%d", dirname, target) ;
 
	if(map_elements()<0){
		fprintf(stderr, "Cannot build hlink\n");
		return ;	// make horizontal links
	}
	
	int	xgap = vm.at(1)->x - vm.at(0)->x ;
	int	ygap = vm.at(1)->y - vm.at(0)->y ;
	int 	zgap = vm.at(1)->z - vm.at(0)->z ;
	// calculate x , y , z
	int	prex = vm.at(1)->x , prey = vm.at(1)->y , prez = vm.at(1)->z ;
	for ( unsigned	i = 2 ; i < vm.size() ; i ++ ) {
		if ( vm.at(i)->x - prex != xgap || vm.at(i)->y - prey != ygap || vm.at(i)->z - prez != zgap){
			if( ( xgap==0 || (xgap!=0 && (vm.at(i)->x - prex)%xgap==0) ) && 
			    ( ygap==0 || (ygap!=0 && (vm.at(i)->y - prey)%ygap==0) ) && 
                            ( zgap==0 || (zgap!=0 && (vm.at(i)->z - prez)%zgap==0) ) ){
				if( 
					//assume xgap always != 0, when y increase not at the same rate as x, return wrong
					( xgap!=0 && ygap!=0 && (vm.at(i)->x - prex)/xgap != (vm.at(i)->y - prey)/ygap ) || 
					( xgap!=0 && ygap!=0 && zgap!=0 && 
					  !((vm.at(i)->x - prex)/xgap == (vm.at(i)->y - prey)/ygap && 
					  (vm.at(i)->x - prex)/xgap == (vm.at(i)->z - prez)/zgap) 
					)
				){
					printf("wrong, return!!!\n");
					return ;		// wrong
				}
			}else{
				printf("wrong, return!!!\n");
				return ;		// wrong
			}
		}

		prex = vm.at(i)->x ;
		prey = vm.at(i)->y ;
		prez = vm.at(i)->z ;
	}
		
	// needs 5 files. Well...?
	float	XcmpVal , YcmpVal, ZcmpVal;
	

	for ( unsigned i = 0 ; i < ext->lcd_vec.size() ; i ++ ) {
//		printf("%d %d\n", ext->lcd_vec.at(i).lcd_val[0], ext->lcd_vec.at(i).lcd_val[1]);

		if ( xgap ) 
			XcmpVal = ( ext->lcd_vec.at(i).lcd_val[0] - prex )	/ xgap ; 
		else XcmpVal = 0 ;
		
		if ( ygap ) 
			YcmpVal = ( ext->lcd_vec.at(i).lcd_val[1] - prey )	/ ygap ; 
		else YcmpVal = 0 ;
		
		if ( zgap ) 
			ZcmpVal = ( ext->lcd_vec.at(i).lcd_val[2] - prez )	/ zgap ; 
		else ZcmpVal = 0 ;


		if ( XcmpVal && !YcmpVal && !ZcmpVal && ext->maxDepth == 1) {
			ext->x = ext->lcd_vec.at(i).lcd_val[0] ;   
		} else if ( comp2Val(XcmpVal,YcmpVal) && ext->maxDepth == 2 ) {	// match
			ext->x = ext->lcd_vec.at(i).lcd_val[0] ;
			ext->y = ext->lcd_vec.at(i).lcd_val[1] ;
			break ;
		}else if( comp2Val(XcmpVal,YcmpVal) && comp2Val(YcmpVal, ZcmpVal)  && ext->maxDepth == 3 ){
			ext->x = ext->lcd_vec.at(i).lcd_val[0] ;
			ext->y = ext->lcd_vec.at(i).lcd_val[1] ;
			ext->z = ext->lcd_vec.at(i).lcd_val[2] ;
		}else{
			
		}
	}

	cout<<"x="<<ext->x<<" y="<<ext->y<<" z="<<ext->z<<"\n";
		
	for ( unsigned	i = 0 ; i < hq.size() ; i ++ ) {
		RSD_HLINK	*rsd_hlink = hq.at(i) ;
		rsd_hlink->ele_arr = (POINT *) calloc(rsd_hlink->hsize , sizeof(POINT)) ;
		rsd_hlink->target = ext->rank ;

		RSD_NODE	*v = ext->vq.at(i) ;
		
	
		rsd_hlink->ext_rlist(v);
		rsd_hlink->ext_histo(v);
		rsd_hlink->ext_loops(v);
		rsd_hlink->ext_param(v);
	}
}


void	EXPNODE::clear()
{
	for(int i=0; i<vm.size(); i++){
		delete vm[i];
	}
	free(ext) ;
}


void	EXPNODE::print()
{
	ext->print(filename);
}

