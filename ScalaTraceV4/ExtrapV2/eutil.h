#include "Param.h"
#ifndef _EUTIL_H
#define	_EUTIL_H

#define	MAX(a,b) (( a > b ) ? a : b )
#define	MIN(a,b) (( a < b ) ? a : b )


typedef	enum {
	LCDT_1D , LCDT_2D , LCDT_3D
} LCDSET_TYPE ;

typedef	struct {
	int	lcd_val[3] ;
	LCDSET_TYPE	ltype ;	
} LCDSET ;	// decrease order


int	decompose(int	*arr, int	count) ;
int	is_sendop(int	op);
int 	is_communicate(int 	op) ;
void	_decompose(int	num , int	token) ;
void	CAL_DIM(const vector<int> *rlist, vector<DIM> *xyz, int *mdepth);


#endif
