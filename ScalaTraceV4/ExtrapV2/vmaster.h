#ifndef _VMASTER_H
#define _VMASTER_H

#include "ecommon.h"
#include "eutil.h"
#include "Event.h"
#include "Trace.h"
#include "context.h"
#include <vector>
#include "rsd_node.h"

class RSD_NODE;

class VMASTER {
public:
	VMASTER(char *filename, int node) ;
	~VMASTER() ;

	int rank ;
	int x, y, z ;//added for 3d
	int maxDepth ;

	Trace *rtrace ;
	int id;
	vector<RSD_NODE *> vq ;
	vector<RSD_NODE *>::iterator vq_it ;
	vector<int>  calls;
	char fname[256] ;
	vector<LCDSET> lcd_vec ;
	void _decompose(int	num , int	token) ;
	void scan_dimension();
	void scan_sizes();
	void resolve_sizes();

	//defined for the symmetry based dimension size detection algorithm
	int get_difference(int i, int j);
	void symmetry_analysis();

	vector<int *> pattern_lib;
	vector<int> pattern_group_size;
	void build_pattern_lib();
	int detect_loop();

	//modified by xing wu, used to find the dimensions
	vector<int> **adj_lists;
	vector<int> **adj_freq;
	int *pattern_type;//record which pattern this node belongs to


	void update_adjlist(vector<int> *values, vector<int> &ranks);
	void print_adjlist();
	void filter_adjlist();
	void inc_freq(vector<int> *v, vector<int> *freq, int dest);
	void print(char *filename);

private:
	int lcd_val[3] ;//modified for 3d

	int isArranged() ;
	int islcd(int t , int lcd_val) ;
	void get_max_size(RSD_NODE *r) ;


} ;

#endif
