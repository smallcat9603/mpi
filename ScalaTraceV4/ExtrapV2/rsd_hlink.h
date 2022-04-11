#ifndef RSD_HLINK_H
#define RSD_HLINK_H

#include "vmaster.h"
//#include "plugins.h"

class RSD_HLINK	{

	typedef	enum {
		ST_INVALID	= -1 ,
		ST_INIT=0 ,ST_ZERO, ST_CONST, ST_ONED , ST_TWOD
	} STATE ;

	typedef	enum {
		EV_ZERO = 0 , EV_CONSTANT, EV_INC, EV_DEC 
	} EVENT ;
public:
	RSD_HLINK() {
		prex = prey = prez = preyv = 0 ; //added for 3d
		state = ST_ZERO; 
		source_dest=NONE; 
		source_dest_count=0;
		hsize=0;
		for(int i=0; i<num_types; i++){
			type_count[i] = 0;
		}
	}
	int	target ;
	POINT	*ele_arr ;
	static 	const 	double	max_inaccuracy = 0.3;
	static 	const 	double	max_slope = 0.1;
	static 	const	double	least_gap = 0.05;
	static 	const	double	max_negative = 0.15; // 0.5 * max_inaccuracy
	
	static	const	double	const_threshold = 0.3;	

	//void	ext_rsd_node(rsd_node	*t ) ;
	void	ext_rlist(RSD_NODE	*r) ;
	void	ext_rlparam(RSD_NODE	*r) ;
	void	ext_destsrcparam(RSD_NODE	*r) ;
	void	ext_color_and_key(RSD_NODE	*r) ;
	
	void	ext_param(RSD_NODE	*r);
	void	ext_paramvalue(ValueSet* input_value[], ValueSet* ext_target, int type, RSD_NODE *r);

	// extrapolate the COUNT parameter
	void	ext_alltoallv_rp(RSD_NODE	*r) ;
	void	ext_alltoall_rp (RSD_NODE	*r) ;
	int	ext_p2p_linear(RSD_NODE *r, int	total_counts[]) ;
	int	is_count_rp(int *dims, int input[]);
	int	ext_p2p_rp(RSD_NODE *r, int c, int *dims);

	void    ext_histo(RSD_NODE   *r);
	void 	ext_count(RSD_NODE   *r);
	void 	ext_loops(RSD_NODE   *r);
	int     ext_histo_param(double *input, RSD_NODE   *r, double *output_array, double *output_p, int print, int type=-1, Histogram *pointer=NULL);
        double	ext_histo_linear(double *input, double *output, int *dimensions, int out_dim, double variance, int out_type=0);
	double 	ext_histo_constant(double *input, double *output, double variance, int out_type=0);
	double 	ext_histo_inverse(double *input, double *output, int *dimensions, double variance, RSD_NODE *r, int out_type=0);
	double  ext_histo_inverse_constant(double *input, double *output, int *dimensions, int out_dim, double variance, int out_type=0);
	double ext_lagrange_interpolation(double *input, double *output, int *dimensions, RSD_NODE *r, double variance, int out_type=0); 
	double	ext_cubic_spline_interpolation(double *input, double *output, int *dimensions, RSD_NODE *r, double variance, int out_type=0);
	double	ext_final_value(double *sds, double *outputs, double* output_p, int type, Histogram *pointer);	
	void	finish_timing();

	int	get_rlist_size(int **rlist);
	int	get_rlist_node_num(int *rlist);
	int	hsize ;
	vector<RSD_NODE* > hlink ;  /*corresponding event in each trace file*/
	//modified by xing wu
	int source_dest;
	int source_dest_count;

	void 	ext_value_unnormal(Param* input_value[], Param* node_param, RSD_NODE *r);
	void	ext_value_2dimension(Param* input_value[], Param* node_param, RSD_NODE *r);
	int 	is_input_const(Param* input_value[], int pos, RSD_NODE* r);
        int 	not_equal_size(ValueSet* input_value[], vector< vector<int> > &new_value, vector<int> &new_num, RSD_NODE *r);
	int 	fitmodel_mod(int *input[4], RSD_NODE *r, vector<int> &temp, vector<int> &out, int *start, int *end);
	int	fitmodel_linear(int *input[4], RSD_NODE *r, vector<int> &temp, vector<int> &out, int *start, int *end);
	void	getcoeff(int a[][4], int *ab, RSD_NODE *r, int size);
private:
	int	prex;
	int	prey;
	int 	prez;//added for 3d
	int	preyv;
	int	prediag;

	double	gmatrix[8][8] ;//modified for 3d

	STATE	state ;
	void	gaussian(int size) ;
	//void	gaussian2(int size) ;
	int	exp_elements(POINT	*p) ;
	void	transition(int	x , int	y) ;
	//void	CHECK2(cvector	**B) ; 

	static	const	int 	num_types = NUM_HISTO_EXT_TYPES;

	int type_count[NUM_HISTO_EXT_TYPES];
	vector<ext_values_t> pending_values;
	string ext_string(string string_input[], ValueSet* input_value[], ValueSet* ext_target, RSD_NODE* r);

} ;

#endif
