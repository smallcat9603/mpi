#ifndef _ECOMMON_H
#define	_ECOMMON_H

#include <stdio.h>
#include <stdlib.h>
#include <math.h>


#include <list>
#include <string>
#include <vector>
#include <Stat.h>


typedef	unsigned	int	UINT32 ;
typedef	unsigned	short	UINT16 ;

using	namespace	std ;

#ifndef TRUE
#define TRUE 1
#endif

#ifndef FALSE
#define FALSE 0
#endif

#ifdef	DEBUG
#define	debug(format, args...) printf(format, ##args)
#else
#define	debug(format, args...)
#endif

#define SOURCE_COUNT 1
#define DEST_COUNT 2
#define NONE 3
#define NUM_HISTO_EXT_TYPES 7

//#define CG
#define ALLTOALLV_CONSTANTS_DIFF 0.01
#define BIN_PARTICIPANT_LIST 0  //set to 1 to output a mock histogram bin participant list, else the global average

//#define DROPEVENT

typedef struct {
	UINT16	target ;
	UINT16	numoftrace ;
	UINT16	last_index ;
	UINT16	dummy ;
	int			debug ;
	char*		dir_name ;
} EPOLOPT ;

typedef	struct {
	int	x ;
	int	y ;
	int	z ;//added for 3d
} POINT ;

typedef struct ext_values {
	double values[NUM_HISTO_EXT_TYPES];
	double sds[NUM_HISTO_EXT_TYPES];
	double *target;
	int type;
	Histogram *pointer;
} ext_values_t;

typedef struct DIM{
	vector<int> single_list ;
	int start_rank ;
	int dimension ;
	int x, y, z;
	int xdepth, xstride, xmask ;
	int ydepth, ystride, ymask ;
	int zdepth, zstride, zmask ;
};

#endif
