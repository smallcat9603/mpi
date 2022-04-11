#include "ecommon.h"
#include "eutil.h"
#include "rsd_hlink.h"
#include <assert.h>
#include <string.h>
#include <limits.h>
#include <math.h>
#include <Stat.h>
#include <iostream>
using namespace std;

#define getdim(d, p, index) ( (int)pow((double)(d==1 ? hlink.at(index)->parent->x : \
				(d==2 ? hlink.at(index)->parent->y : \
				hlink.at(index)->parent->z)), (double)p ) )


void	RSD_HLINK::gaussian(int	size)
{
	int	i,j,k ;
	double t ;

	for ( k = 0 ; k < size - 1; k++ ) {
		for ( i = k + 1 ; i < size ; i ++ ) {
			if(gmatrix[k][k]!=0){
				t = gmatrix[i][k] / gmatrix[k][k] ;
				for ( j = k + 1 ; j <= size ; j ++ )
					gmatrix[i][j] -= t * gmatrix[k][j] ;
			} else {
				gmatrix[i][k] = 0;
			}
		}
	}

	for ( i = size -1 ; i >= 0 ; i -- ) {
		t = gmatrix[i][size] ;
		for ( j = i + 1 ; j < size ; j ++ )
			t -= gmatrix[i][j] * gmatrix[j][size] ;
		if ( gmatrix[i][i] != 0 )
			gmatrix[i][size] = t / gmatrix[i][i] ;
		else
			gmatrix[i][size] = 0 ;
	}
}



double RSD_HLINK::ext_histo_inverse_constant(double *input, double *output, int *dimensions, int out_dim, double variance, int out_type){

	int size = hlink.size();
	double products[size];
	double results, sd;

	int p = 0;	

	//if(input[0] == 18378563) p = 1;

	for(int i=0; i<size; i++){
		products[i] = input[i] * dimensions[i];
		if(p) printf("%f %f\n", input[i], products[i]);
	}	

	sd = ext_histo_linear(products, &results, dimensions, out_dim, variance, out_type);
	if(p) printf("sd = %f\n", sd);
	if(sd > 0){
		*output = results / out_dim;
		return sd;
	} else {
		*output = 0;
		if(out_type) return sd;
		return -1;
	}

}



//should try to test whether a linear relationship exists, and then do the real calculation
double 	RSD_HLINK::ext_histo_linear(double *input, double *output, int *dimensions, int out_dim, double variance, int out_type){

	double sx = 0, sy = 0, sxy = 0, sxx = 0, syy = 0, lxy, lxx, lyy;
	double coef = 0, offset = 0, sd, avg = 0, sum = 0;
	double size = (double)hlink.size();
	int strict_inc;

	for(int i=0; i<size; i++){
		sx += (double)dimensions[i];
		sy += input[i];
		sxy += (double)dimensions[i] * input[i];
		sxx += (double)dimensions[i] * (double)dimensions[i];
		syy += input[i] * input[i];
	}

	lxy = sxy - sx * sy / size;
	lxx = sxx - sx * sx / size;
	lyy = syy - sy * sy / size;

	coef = lxy / lxx;
	offset = sy / size - coef * ( sx / size );

	sd = (1/(size-2) * ( lyy - coef * coef * lxx ));
	sd = sqrt(sd);

	for(int i=0; i<size; i++){
		avg += (offset + coef*dimensions[i]);
	} 
	avg = avg / size;

	if( avg > 0 && sd/avg < variance ){

		strict_inc = 1;
		for(int i=1; i<size; i++){
			if(input[i]<input[i-1]){
				strict_inc = 0;
				break;
			}
		}
                                        
		if( coef < max_slope && coef > (0 - max_slope) && !strict_inc ){
			for(int i=0; i<size; i++){
				sum += input[i];
			}
			*output = sum/size;
			if(out_type) return sd/avg;
			return -1;
		} else {
			*output = offset + coef * (double)out_dim;
			/*
			if(*output>1000000){
				cout<<"input="<<input[0]<<" "<<input[1]<<" "<<input[2]<<" "<<input[3]<<"\n";
				cout<<"output="<<*output<<" offset="<<offset<<" coef="<<coef<<"\n";
			}*/
			return sd/avg;
		}
	} else if(avg == 0) {
		*output = 0;
		return 0;
	} else {
		*output = offset + coef * (double)out_dim;
		if(out_type) return sd/avg;
		return -1;
	}
}



double	RSD_HLINK::ext_histo_constant(double *input, double *output, double variance, int out_type){
	double avg, sum = 0, sd = 0, diff[4];
	unsigned i;
        int max_diff_item = 0;

	for(i=0; i<hlink.size(); i++){
		sum += input[i];
	}
	avg = sum/hlink.size();

	for(i=0; i<hlink.size(); i++){
		diff[i] = input[i] > avg ? input[i] - avg : avg - input[i];
		if( diff[i] > diff[max_diff_item] )
			max_diff_item = i;
	}

	avg = ( sum - input[max_diff_item] ) / (double)( hlink.size() - 1 );

	for(i=0; i<hlink.size(); i++){
		if(i != (unsigned) max_diff_item)
			sd += (input[i] - avg) * (input[i] - avg);
	}

	sd = sqrt( sd/(double)(hlink.size()-1) );

	//printf("sd/avg = %f\n, variance=%f\n", sd/avg, variance);

	if( avg > 0 && sd/avg <= variance){
		*output = avg;
		return sd/avg;
	} else if(avg == 0) {
		*output = 0;
		return 0;
	} else {
		*output = avg;
		if(out_type) return sd/avg;
		return -1;
	}
}




	
double     RSD_HLINK::ext_histo_inverse(double *input, double *output, int *dimensions, double variance, RSD_NODE *r, int out_type){
	double products[4], avg=0, diff[4], sd = 0;
	int max_diff_item = 0;
	unsigned i;
	double min_sd = -1;
	double best_avg = 0;

	int x, y, z;
	int best_x, best_y, best_z;
	int max_pow = 3;

	//double kk, bb, vv, best_kk, best_bb;

	int p = 0;
	//int type = 0, best_type = 0;
	
	//if(input[0] == 18378563) p = 1;

	// this only works for x, y, z
	for(int xx=0; xx<max_pow; xx++){	
		for(int yy=0; yy<max_pow; yy++){
			for(int zz=0; zz<max_pow; zz++){
				for(i=0; i<hlink.size(); i++){
					x = getdim(1, xx, i);
					y = getdim(2, yy, i);
					z = getdim(3, zz, i);
					dimensions[i] = x*y*z;
				
				}

				avg = 0;

				for(i=0; i<hlink.size(); i++){
					products[i] = input[i] * (double)dimensions[i];
					avg += products[i];
				}
				avg = avg/hlink.size();
				for(i=0; i<hlink.size(); i++){
					diff[i] = products[i] > avg ? products[i] - avg : avg - products[i];
					if(diff[i] > diff[max_diff_item])
						max_diff_item = i;
				}
				avg = (avg * hlink.size() - products[max_diff_item]) / (double)(hlink.size() - 1);
				
				for(i=0; i<hlink.size(); i++){
					if(i != (unsigned) max_diff_item ){
						sd += (products[i] - avg) * (products[i] - avg);
					}
				}
				sd = sqrt( sd/(double)(hlink.size()-1) );
				sd = sd/avg;

				
				if( avg > 0 && sd < variance && (sd < min_sd || min_sd == -1) )	{
					min_sd = sd;
					//best_type = type;
					//best_kk = kk;
					//best_bb = bb;
					best_avg = avg;
					best_x = xx; 
					best_y = yy; 
					best_z = zz;
					if(p){
						printf("%d %d %d\n", xx, yy, zz);
						for(int xw = 0; xw<4; xw++)
							printf("%d ", dimensions[xw]);
						printf("\navg %f  sd %f  sd/avg %f\n\n", avg, sd, sd/avg);
					}
				}
					
			}
		}
	}

	

	if( best_avg > 0 && min_sd != -1 ){
		if(p) printf("inverse %f\n", min_sd);
                *output = best_avg / (pow((double)r->parent->x, (double)best_x) 
								* pow((double)r->parent->y, (double)best_y) 
								* pow((double)r->parent->z, (double)best_z));
                return min_sd;
	} else if(best_avg == 0) {
                *output = 0;
                return 0;
        } else {
		*output = best_avg / (pow((double)r->parent->x, (double)best_x) 
						* pow((double)r->parent->y, (double)best_y) 
						* pow((double)r->parent->z, (double)best_z));
		if(out_type) return sd;
                return -1;
        }
}

double     RSD_HLINK::ext_lagrange_interpolation(double *input, double *output, int *dimensions, RSD_NODE *r, double variance, int out_type){
	int size = hlink.size();
	int out_dim = r->parent->rank;
	*output = 0;
	double result=0; 
	if(size < 4){
		return 1000;  //size=1,2, same as constant model; size=3 same as linear model, so ignore it.
	}
	for(int i=0; i<size-1; i++){
		double temp = input[i], temp1 = input[i];
		for(int j=0; j<size-1; j++){
			if(i!=j){
				temp *= (dimensions[size-1] - dimensions[j]);
				temp1 *= (out_dim - dimensions[j]);
				temp /= (dimensions[i] - dimensions[j]);
				temp1 /= (dimensions[i] - dimensions[j]);
			}
		}
		result += temp;
		*output += temp1;
	}
	double err = abs (result - input[size-1]);
	if( err <= variance )
		return err;
	else
		return -1;
}

double	RSD_HLINK::ext_cubic_spline_interpolation(double *input, double *output, int *dimensions, RSD_NODE *r, double variance, int out_type){
	int size = hlink.size();
	int out_dim = r->parent->rank;
	double *x = (double*)dimensions, *y = input;
	if(size < 4)
		return 1000;
	//choose the first 4 inputs
	double h[4], b[4], C[4], D[4], z[4];
	for(int i=0; i<3; i++){
		h[i] = x[i+1] - x[i];
		b[i] = (y[i+1] - y[i])/h[i];
	}
	z[0] = z[3] = 0; //natural
	z[1] = (6*h[1]*(b[2]-b[1])-12*(h[1]+h[2])*(b[1]-b[0]))/(h[1]*h[1]-4*(h[0]+h[1])*(h[1]+h[2]));
	z[2] = (6*(b[1]-b[0])-2*(h[0]+h[1])*z[1])/h[1];

	for(int i=0; i<3; i++){
		C[i] = y[i+1]/h[i] - h[i]*z[i+1]/6;
		D[i] = y[i]/h[i] - h[i]*z[i]/6;
	}

	double err = 10000.0;
	double avg_input, avg_output;
	for(int i=0; i<3; i++){
		double sum_input = 0, sum_output = 0, sum_error = 0;
		for(int j=2; j<size; j++){
			int index = (i+j)%size;
			double inx = x[index];	
			double evaluate = z[i+1]*pow((inx-x[i]),3)/(6*h[i]) + z[i]*pow((x[i+1]-inx),3)/(6*h[i]) + C[i]*(inx-x[i]) + D[i]*(x[i+1]-inx);

			// use NMSE to calculate the error
			sum_output += evaluate;
			sum_input += input[index];
			sum_error += (input[index]-evaluate)*(input[index]-evaluate);
		}
		avg_input = sum_input / (size-2);
		avg_output = sum_output / (size-2);
		double sd = sum_error/((size-2) * avg_input * avg_output);
		if(sd < err){
			*output = z[i+1]*pow((out_dim-x[i]),3)/(6*h[i]) + z[i]*pow((x[i+1]-out_dim),3)/(6*h[i]) + C[i]*(out_dim-x[i]) + D[i]*(x[i+1]-out_dim);
			err = sd;
		}
	}
	if( err <= variance )
		return err;
	else
		return -1;
}


void	RSD_HLINK::finish_timing(){
	
	double negative_revision[num_types];
	double sum = 0;
	vector<ext_values_t>::iterator iter;
	int min = 0;
	double min_sd = INT_MAX; // an arbitrary large value


	for(int i=0; i<num_types; i++){
		sum += type_count[i];
	}

	for(int i=0; i<num_types; i++){
		negative_revision[i] = max_negative * ((double)type_count[i]/sum);
	}
	
	for(iter = pending_values.begin(); iter!=pending_values.end(); iter++){
		
		min_sd = INT_MAX;
		min = 0;

		for(int i=0; i<num_types; i++){
			if(iter->sds[i] != -1)
				iter->sds[i] -= negative_revision[i];
			else
				iter->sds[i] = max_inaccuracy - negative_revision[i];
		}
	
		for(int i=0; i<num_types; i++){
			if(iter->sds[i]<min_sd){
				min_sd = iter->sds[i];
				min = i;
			}
		}
		*(iter->target) = iter->values[min];
		switch (iter->type){
			case 1:
				iter->pointer->set_max(*(iter->target), 0);
				break;
			case 2:
				iter->pointer->set_min(*(iter->target), 0);
				break;
			case 3:
				iter->pointer->set_avg(*(iter->target), 0);
				break;
			default:
				break;
		}
	}
}



double 	RSD_HLINK::ext_final_value(double *sds, double *outputs, double* output_p, int type, Histogram *pointer){
	int min = -1, second_min = -1; 
	double min_sd = INT_MAX; // an arbitrary large number
	ext_values_t ext_values;

	for(int i=0; i<num_types; i++){
		if(sds[i]!=-1 && sds[i] < min_sd){
			min_sd = sds[i];
			min = i;
		}	
	}

	min_sd = INT_MAX; // an arbitrary large number

	for(int i=0; i<num_types; i++){
		if( i == min ){
			continue;
		}
		if(sds[i]!=-1 && sds[i] < min_sd){
			min_sd = sds[i];
			second_min = i;
		}
	}


	if ( (min>-1 && second_min==-1) || (min>-1 && second_min>-1 && (sds[second_min]-sds[min])/sds[min] > least_gap) ){
		type_count[min]++;
		return outputs[min] > 0 ? outputs[min] : 0;
	} else {
		for(int i=0; i<num_types; i++){
			ext_values.values[i] = outputs[i] > 0 ? outputs[i] : 0;
			ext_values.sds[i] = sds[i];
		}
		ext_values.target = output_p;
		ext_values.type = type;
		ext_values.pointer = pointer;
		pending_values.push_back(ext_values);
		return -1;
	}
}



int 	RSD_HLINK::ext_histo_param(double *input, RSD_NODE   *r, double *output_array, double *output_p, int print, int type, Histogram *pointer){
	
	int  dimensions[4];
	double rtn[num_types], outputs[num_types];
	int temp;
	
	//test constant, if passed, return value is output_p
	rtn[0] = ext_histo_constant(input, output_p, max_inaccuracy);
	outputs[0] = *output_p;
	
		
	//test inverse proportion
	for(unsigned i=0; i<hlink.size(); i++){
		dimensions[i] = hlink.at(i)->parent->rank;
	}
	rtn[1] = ext_histo_inverse(input, output_p, dimensions, max_inaccuracy, r);
	outputs[1] = *output_p;
	
	
	//test linear between time and # rank
	for(unsigned i=0; i<hlink.size(); i++){
		dimensions[i] = hlink.at(i)->parent->rank;
	}
	rtn[2] = ext_histo_linear(input, output_p, dimensions, r->parent->rank, max_inaccuracy);
	outputs[2] = *output_p;
	
	
	//test y=k/x+b
	for(unsigned i=0; i<hlink.size(); i++){
		dimensions[i] = hlink.at(i)->parent->rank;
	}
	rtn[3] = ext_histo_inverse_constant(input, output_p, dimensions, r->parent->rank, max_inaccuracy);
	outputs[3] = *output_p;



	//if the number of nodes is a square number, test linear trend wrt. the square root
	temp = (int)sqrt((double)r->parent->rank);
	if( temp * temp == r->parent->rank){
		for(unsigned i=0; i<hlink.size(); i++){
			dimensions[i] = (int) sqrt((double)(hlink.at(i)->parent->rank));
		}
		rtn[4] = ext_histo_linear(input, output_p, dimensions, temp, max_inaccuracy);
		outputs[4] = *output_p;
	} else {
		rtn[4] = INT_MAX;
		outputs[4] = -1.0;
	}

	rtn[5] = ext_lagrange_interpolation(input, output_p, dimensions, r, max_inaccuracy);
	outputs[5] = *output_p;
	
	rtn[6] = ext_cubic_spline_interpolation(input, output_p, dimensions, r, max_inaccuracy);
	outputs[6] = *output_p;

	*output_p = ext_final_value(rtn, outputs, output_p, type, pointer);

	return 0;
}



void	RSD_HLINK::ext_histo(RSD_NODE 	*r){

	double input[4], output[4], start[4], min[4], max[4], end[4], avg[4];
	int rtn;
	map<int, Stat*> *com[2];
       	com[0] = r->q->getCompStats();
	com[1] = r->q->getCommStats();
	Histogram* tr[4], *target;
	map<int, Stat*> ::iterator it;

	//extraplate Histogram min_value and Histogram max_value
	for(int i = 0; i < 2; i++){
		if(com[i]->size() == 0)
			continue;
		for(it=(*com[i]).begin(); it!=(*com[i]).end(); it++){
			map<int, Stat*> *temp[4];
			map<StackSig, Histogram *> ::iterator pit, ppit[4];
			for(unsigned k=0; k<hlink.size(); k++){
			        temp[k] = (i==0)?hlink[k]->q->getCompStats():hlink[k]->q->getCommStats();
				map<int, Stat*> ::iterator it1 = temp[k]->find(it->first);
			        ppit[k] = it1->second->getPathstats()->begin();	
			}
			for(pit=it->second->getPathstats()->begin(); pit!=it->second->getPathstats()->end(); pit++){//map<StackSig, Histogram*> belong to Stat.
				double *out_put = (double*)malloc(sizeof(double));
				int ext_rank;

				for(unsigned k=0; k<hlink.size(); k++){
					tr[k] = ppit[k]->second;
					ppit[k]++;
				}
				target = pit->second;

				//max_value & max_rank
				for(unsigned k=0; k<hlink.size(); k++){
					input[k] = tr[k]->max();
				}
				rtn = ext_histo_param(input, r, output, out_put, 0, 1, target);
				
				target->set_max(*out_put, 0);
				/*
				printf("maxvalue: %f, %f, %f, %f -> %f\n", input[0], input[1], input[2], input[3], *out_put);
				printf("maxrank: %d, %d, %d, %d -> %d\n", tr[0]->getMaxRank(), tr[1]->getMaxRank(), tr[2]->getMaxRank(), tr[3]->getMaxRank(), ext_rank);
				*/
				
                        	//min_value & min_rank
				for(unsigned k=0; k<hlink.size(); k++){
					input[k] = tr[k]->min();
				}
				rtn = ext_histo_param(input, r, output, out_put, 0, 2, target);

				target->set_min(*out_put, 0);

				//num_elements and avg_value
				for(unsigned k=0; k<hlink.size(); k++){
					input[k] = tr[k]->avg();
				}
				rtn = ext_histo_param(input, r, output, out_put, 0, 3, target);

				
				for(unsigned k=0; k<hlink.size(); k++){
					gmatrix[k][0] = 1;
					gmatrix[k][1] = hlink[k]->x;
					gmatrix[k][2] = (hlink[k]->x) * (hlink[k]->y);
					gmatrix[k][3] = hlink[k]->parent->rank;
					gmatrix[k][4] = tr[k]->getNumElems();
				}	
				gaussian(4);
				ext_rank = (int) (gmatrix[0][4] + gmatrix[1][4]*r->parent->x + gmatrix[2][4]*(r->parent->x)*(r->parent->y) + gmatrix[3][4]*r->parent->rank);
				
				target->set_avg(*out_put, ext_rank);
			
			/*	
				cout<<"(" <<target->getNumElems()<<" ";
				cout<<setprecision(2) << fixed << target->avg()<<" ";
				cout<< target->getMinRank()<<" ";
				cout<< setprecision(2) << fixed<<target->min()<<" ";
				cout<< target->getMaxRank()<<" ";
				cout<< setprecision(2) <<fixed<<target->max()<<")"<<"\n";
				*/

				for(unsigned j=0; j<(*(target->getBins())).size(); j++){
					HistoBin *bin[4];
					//HistoBin *target_bin;
					double start[4], min[4], max[4], end[4], avg[4];
					for(unsigned k=0; k<hlink.size(); k++){
						bin[k] = (*(tr[k]->getBins()))[j];
						start[k] = bin[k]->getStart();
						min[k] = bin[k]->getMin();
						max[k] = bin[k]->getMax();
						end[k] = bin[k]->getEnd();
						avg[k] = bin[k]->getAverage();
					}	
				}
				//extrapolate bin->start
				rtn = ext_histo_param(start, r, output, out_put, 0);

				//extrapolate bin->min
				rtn = ext_histo_param(min, r, output, out_put, 0);

				//extrapolate bin->max
				rtn = ext_histo_param(max, r, output, out_put, 0);

				//extrapolate bin->end
				rtn = ext_histo_param(end, r, output, out_put, 0);

				//extrapolate bin->average
				rtn = ext_histo_param(avg, r, output, out_put, 0);

			}
		}
	}
	finish_timing();
}

void RSD_HLINK :: ext_loops(RSD_NODE	*r){

	vector<loop_t *> *target_loop = r->q->getLoopStack()->getLoops();
        vector<loop_t *> *input_loop[4];
	unsigned size = target_loop->size();
	if(size == 0) return; /*need to be checked*/

	for(unsigned i=0; i<hlink.size(); i++){
		input_loop[i] = hlink[i]->q->getLoopStack()->getLoops();
		if(size != input_loop[i]->size()){
			cerr<<"error: loopstack doesn't match for event "<<r->q->getId()<<"\n";
			exit(0);
		}
	}	
	for(unsigned i=0; i<target_loop->size(); i++){

		loop_t *target_param = (*target_loop)[i];
		loop_t *input_param[4];

		for(unsigned j=0; j<hlink.size(); j++)
			input_param[j] = (*(input_loop[j]))[i];

		vector<ValueSet *> *target_P;
		vector<ValueSet *> *input_P[4];
		
		for(int j=0; j<2; j++){
			if(j==0){
				target_P = target_param->mem->getParamValues();
				for(unsigned k=0; k<hlink.size(); k++)
					input_P[k] = input_param[k]->mem->getParamValues();
			} else {
				target_P = target_param->iter->getParamValues();
				for(unsigned k=0; k<hlink.size(); k++)
					input_P[k] = input_param[k]->iter->getParamValues();
			}
			
			for(unsigned k=0; k<target_P->size(); k++){
				const vector<int> *target = (*target_P)[k]->getRanklist()->getRanks();
				const vector<int> *input[4];

				vector<int> *target_values = (*target_P)[k]->getValues();
				vector<int> *input_values[4];

				for(unsigned kk=0; kk<hlink.size(); kk++){

					/*test if length of ranklist matches*/
					input[kk] = (*(input_P[kk]))[k]->getRanklist()->getRanks();
					if(input[kk]->size()!=target->size()){
						cerr<<"size of ranklist of loop doesn't match in event"<<r->q->getId()<<"\n";
						exit(0);
					}

					/*test if length of values matches*/
					input_values[kk] = (*(input_P[kk]))[k]->getValues();
					if(input_values[kk]->size() != target_values->size()){
						cerr<<"size of mem or iter of loop doesn't match in event"<<r->q->getId()<<"\n";
						exit(0);
					}
				}

				vector<int> temp;
				for(unsigned kk=0; kk<target->size(); kk++){
					if(kk==0 || target->at(kk) < 0)
						temp.push_back(target->at(kk));
					else {
						for(unsigned kkk=0; kkk<hlink.size(); kkk++){
							gmatrix[kkk][0] = 1;
							gmatrix[kkk][1] = hlink.at(kkk)->x;
							gmatrix[kkk][2] = hlink.at(kkk)->x * hlink.at(kkk)->y;
							gmatrix[kkk][3] = hlink.at(kkk)->parent->rank;
							gmatrix[kkk][4] = input[kkk]->at(kk);
						}
						gaussian(4);
						int extrank = (int) (gmatrix[0][4]+gmatrix[1][4]*r->parent->x + gmatrix[2][4]*(r->parent->x)*(r->parent->y)+gmatrix[3][4]*r->parent->rank);
						temp.push_back(extrank);
					}
				}
				(*target_P)[k]->getRanklist()->set_ext_Ranks(temp);

				temp.clear();
				for(unsigned kk=0; kk<target_values->size(); kk++){
					for(unsigned kkk=0; kkk<hlink.size(); kkk++){
						gmatrix[kkk][0] = 1;
						gmatrix[kkk][1] = hlink.at(kkk)->x;
						gmatrix[kkk][2] = hlink.at(kkk)->x * hlink.at(kkk)->y;
						gmatrix[kkk][3] = hlink.at(kkk)->parent->rank;
						gmatrix[kkk][4] = input_values[kkk]->at(kk);	
					}
					gaussian(4);
					int extvalue = (int) (gmatrix[0][4]+gmatrix[1][4]*r->parent->x + gmatrix[2][4]*(r->parent->x)*(r->parent->y)+gmatrix[3][4]*r->parent->rank);
					temp.push_back(extvalue);
				}
				(*target_P)[k]->setValues(temp);
			}	
				
		}

	}
}



void	RSD_HLINK::ext_rlist(RSD_NODE	*r)
{
	int num_rlist = (*(r->q->getRanklist()->getRanks()))[0];
	int num_pairs=0;
	assert(num_rlist = (r->xyz).size());
	for(int rl_iter=0; rl_iter<num_rlist; rl_iter++){

		num_pairs = -(r->xyz)[rl_iter].single_list.at(0);

		for(int i=1; i<1+2*num_pairs; i++){

			for ( unsigned j = 0 ; j < hlink.size() ; j ++ ) {
				gmatrix[j][0] = 1 ;
				gmatrix[j][1] = hlink.at(j)->x ; 
				gmatrix[j][2] = hlink.at(j)->x * hlink.at(j)->y;
				gmatrix[j][3] = hlink.at(j)->parent->rank ;
				gmatrix[j][4] = (hlink.at(j)->xyz)[rl_iter].single_list.at(i);
			}

			gaussian(4) ; 

			(r->xyz)[rl_iter].single_list.at(i) = (int) (gmatrix[0][4] + gmatrix[1][4]*r->parent->x + gmatrix[2][4]*(r->parent->x)*(r->parent->y) + gmatrix[3][4]*r->parent->rank) ;
		}

	}//end of for(rl_iter...)
	vector<int> temp;
	temp.push_back(num_rlist);
	for(int i=0; i<num_rlist; i++){
		for(unsigned j=0; j<(r->xyz)[i].single_list.size(); j++)
			temp.push_back((r->xyz)[i].single_list.at(j));
	}
	r->q->GenRankList(temp);

	for(unsigned j=0; j<hlink.size(); j++){
		gmatrix[j][0] = 1 ;
		gmatrix[j][1] = hlink.at(j)->x ; 
		gmatrix[j][2] = hlink.at(j)->x * hlink.at(j)->y;
		gmatrix[j][3] = hlink.at(j)->parent->rank ;
		gmatrix[j][4] = hlink[j]->q->getRanklist()->getNumRanks();
	}
	gaussian(4);
	int numranks = (int) (gmatrix[0][4] + gmatrix[1][4]*r->parent->x + gmatrix[2][4]*(r->parent->x)*(r->parent->y) + gmatrix[3][4]*r->parent->rank);
	r->q->getRanklist()->setNumRanks(numranks);



	/*cout<<"ranklist = ";
	for(int i=0; i<(*(r->q->getRanklist()->getRanks())).size(); i++){
		cout<<(*(r->q->getRanklist()->getRanks()))[i]<<" ";
	}
	cout<<"\n";*/
}




void RSD_HLINK::ext_param(RSD_NODE *r){

	/*extrapolate for parameters*/
	map<int, Param*> *node_params = r->q->getParams();
	map<int, Param*>::iterator it;

	/*extrapolate from the beginning*/
	for(it=node_params->begin(); it!=node_params->end(); it++){
		Param* node_param = it->second;
		int size = node_param->getParamValues()->size();
		int type = node_param->getParamType();

		/*gather the input param from hlink*/
		Param* input_params[4];
		int flag=0, flag1=0;
		for(unsigned i=0; i<hlink.size(); i++){
			map<int, Param*> *h_params = hlink.at(i)->q->getParams();
			map<int, Param*> ::iterator it1 = h_params->find(type);

			/*check if the param size are matched*/
			int size1 = it1->second->getParamValues()->size();
			
			if(size1!=size){
				flag1 = 1;
				//cerr<<"cannot normally extrapolate for parameter "<<type<<" in event "<<hlink.at(i)->q->getId() <<" because size doesn't match"<<"\n";
				if(size1 != hlink[i]->q->getRanklist()->getNumRanks()) flag = 1;
			}
			input_params[i] = it1->second;
		}
		if(flag == 1){
			ext_value_unnormal(input_params, node_param, r);
		} else if (flag1 == 1)
			ext_value_2dimension(input_params, node_param, r);
		else{
			for(int i=0; i<size; i++){
				ValueSet* input_value[4];
				ValueSet* ext_target = (*(node_param->getParamValues())).at(i);
				for(unsigned j=0; j<hlink.size(); j++){
					input_value[j] = (*(input_params[j]->getParamValues())).at(i); 
				}
				ext_paramvalue(input_value, ext_target, type, r);
			}
		}
	}
}

/*extrapolate for bt benchmark's count parameter pattern*/
void	RSD_HLINK::ext_value_unnormal(Param* input_value[], Param* node_param, RSD_NODE *r){

	vector<int> rtn;
	int num=0;
	for(unsigned i=0; i<hlink.size(); i++){
		vector<ValueSet*>::iterator it;
		for(it=input_value[i]->getParamValues()->begin(); it!=input_value[i]->getParamValues()->end(); it++)
			num = MAX(num, (int)(*it)->getValues()->size());
	}	

	for(int j=0; j<num; j++){
		int temp;	
		if( (temp=is_input_const(input_value, j, r))>0 ){
			rtn.push_back(temp);
		}
		else{
			cout<<"not const relation, can not be merged into one here in event "<<r->q->getId()<<"\n";
			exit(0);
		}
	}
	node_param->getParamValues()->at(0)->setValues(rtn);
	node_param->getParamValues()->at(0)->getRanklist()->setRanks(*(r->q->getRanklist()->getRanks()));
	node_param->clearto(1);
	/*
	cout << "sizeofextra="<<node_param->getParamValues()->size()<<"\n";
	cout << "valuesize="<<node_param->getParamValues()->at(0)->getValues()->size()<<"\n";
	cout << "ranklist="<<node_param->getParamValues()->at(0)->getRanklist()->toString()<<"\n";
	*/
}


/*extrapolate for 2 dimension in loops, in this pattern, each Param has the same size as rank size*/
/*default for collective call, all the ranks involved in the event*/
void	RSD_HLINK::ext_value_2dimension(Param* input_value[], Param* node_param, RSD_NODE *r){

	int *input[4];
	int size = 0;
	int start[4], end[4];

	/*get the max number of item in one ValueSet*/
	for(unsigned i=0; i<input_value[0]->getParamValues()->size(); i++)
		size = MAX(size, (int)input_value[0]->getParamValues()->at(i)->getValues()->size());
	vector<int> temp[size];

	vector<int> rsort = ranksort(r->parent->rank);
	for(int i=rsort.size()-1; i>=0; i--){
		if(!r->q->getRanklist()->hasMember(i))
			rsort.erase(rsort.begin()+i);
	}

	for(int i=0; i<size; i++){
		for(unsigned j=0; j<hlink.size(); j++){
			input[j] = (int*)malloc((input_value[j]->getParamValues()->size())*sizeof(int));
			vector<int> sort = ranksort(hlink[j]->parent->rank);
			for(int k=sort.size()-1; k>=0; k--){
				if(!hlink[j]->q->getRanklist()->hasMember(sort[k]))
					sort.erase(sort.begin()+k);
			}
			end[j] = 0;
			start[j] = hlink[j]->parent->rank;
			for(unsigned k=0; k<hlink[j]->parent->rank; k++){

				/*the parameters need to be sorted as same order as inter-compression order*/
				if(k<input_value[j]->getParamValues()->size()){
					assert(input_value[j]->getParamValues()->at(k)->getRanklist()->hasMember(sort[k]));
					if(sort[k] > end[j])
						end[j] = sort[k];
					if(sort[k] < start[j])
						start[j] = sort[k];	
					int size1 = input_value[j]->getParamValues()->at(k)->getValues()->size();  /*get correct elements for loops' compression*/
					input[j][sort[k] ] = input_value[j]->getParamValues()->at(k)->getValues()->at(i%size1);
				}
			}
		}
		if(fitmodel_linear(input, r, temp[i], rsort, start, end))
			continue;
		else if(fitmodel_mod(input, r, temp[i], rsort, start, end))
			continue;
		else{
			cout<<"parameter in event "<<r->q->getId()<<" doesn't fit linear or mod model, exit"<<"\n";
			exit(0);
		}
		for(unsigned j=0; j<hlink.size(); j++)
			free(input[j]);
	}
	node_param->clearto(0);
	for(int i=0; i<rsort.size(); i++){
		vector<int> params;
		for(int j=0; j<size; j++){
			params.push_back(temp[j][i]);
		}
		vector<int> ranks;
		ranks.push_back(1);
		ranks.push_back(-1);
		ranks.push_back(rsort[i]);
		ranks.push_back(1);
		ranks.push_back(0);

		ValueSet *vs = new ParamValue();
		vs->setValues(params);
		vs->getRanklist()->set_ext_Ranks(ranks);
		vs->getRanklist()->setNumRanks(1);
		node_param->addvalues(vs);
	}
}



void	RSD_HLINK::ext_paramvalue(ValueSet* input_value[], ValueSet* ext_target, int type, RSD_NODE *r){

	const vector<int> *rank_list[4];
	vector<int> temp;
	int size = input_value[0]->getRanklist()->getRanks()->size();
	for(unsigned i=0; i<hlink.size(); i++){
		rank_list[i] = input_value[i]->getRanklist()->getRanks();
		if(rank_list[i]->size() != (unsigned)size){
			cerr <<"size of ranklist doesn't match, fail to extrap for event "<<r->q->getId()<<"\n";
			exit(0);
		}
	}
	for(int i=0; i<size; i++){
		if(i==0 || (rank_list[0]->at(i) < 0)){
			temp.push_back(rank_list[0]->at(i));
		} else {
			for(unsigned j=0; j<hlink.size(); j++){
				gmatrix[j][0] = 1;
				gmatrix[j][1] = hlink.at(j)->x;
				gmatrix[j][2] = hlink.at(j)->x * hlink.at(j)->y;
				gmatrix[j][3] = hlink.at(j)->parent->rank;
				gmatrix[j][4] = rank_list[j]->at(i); 
			}
			gaussian(4);
			int extrank =(int) (gmatrix[0][4]+gmatrix[1][4]*r->parent->x + gmatrix[2][4]*(r->parent->x)*(r->parent->y)+gmatrix[3][4]*r->parent->rank);
			temp.push_back(extrank);
		}
	}
	ext_target->getRanklist()->set_ext_Ranks(temp);
	temp.clear();
	int dims[6], input[4], rp_c, extparam;
	unsigned max_i;

	vector<int> new_num;
	vector< vector<int> > new_value;
	int not_equal_flag = not_equal_size(input_value, new_value, new_num, r);  /*the size of each value item are different*/
	if(not_equal_flag)
		max_i = new_value[0].size(); /*for bt, merge the same value into one*/	
	else
		max_i = input_value[0]->getValues()->size();

	/*for offset extrapolation*/
	int *off_input[4];
	vector<int> sort;

	/*for iotime extrapolation*/
	double rtn[num_types],outputs[num_types], min_rtn, inputs[4];
	int dimensions[4], inttemp;
	double *output_p = (double*) malloc (sizeof(double));
	double extra=1.0;

	/*for string extrapolation*/
	vector<string> char_record[4];
	string target_string;


	switch (type){
		case DEST:
		case SOURCE:
			for(unsigned i=0; i<max_i; i++){
				for(unsigned j=0; j<hlink.size(); j++){
					gmatrix[j][0] = 1;
					gmatrix[j][1] = hlink.at(j)->x;
					gmatrix[j][2] = hlink.at(j)->x * hlink.at(j)->y;
					gmatrix[j][3] = hlink.at(j)->parent->rank;
					if(not_equal_flag)
						gmatrix[j][4] = new_value[j][i];
					else
						gmatrix[j][4] =	input_value[j]->getValues()->at(i);	
				}
				gaussian(4);
				extparam =(int) (gmatrix[0][4]+gmatrix[1][4]*r->parent->x + gmatrix[2][4]*(r->parent->x)*(r->parent->y)+gmatrix[3][4]*r->parent->rank);
				if(not_equal_flag){
					for(int k=0; k<new_num[i]; k++)
						temp.push_back(extparam);
				}else
					temp.push_back(extparam);
			}
			ext_target->setValues(temp);
			break;
		case COUNT:
			for(unsigned i=0; i<max_i; i++){
				for(unsigned j=0; j<hlink.size(); j++){

					if(not_equal_flag)
						input[j] = new_value[j][i];
					else
						input[j] = input_value[j]->getValues()->at(i); 	
				}
				if((rp_c = is_count_rp(dims, input)) > 0){
					extparam = ext_p2p_rp(r, rp_c, dims);
				}else{
					extparam = ext_p2p_linear(r, input);	
				}
				if(not_equal_flag){
					for(int k=0; k<new_num[i]; k++)
						temp.push_back(extparam);
				}else
					temp.push_back(extparam);
			}
			ext_target->setValues(temp);
			break;
		case FILENAME:
			target_string.clear();
			for(unsigned i=0; i<hlink.size(); i++){
				string filename = (dynamic_cast<ParamValue *>(input_value[i]))->getCharValues()->at(0);
				while(filename.length()>0){
					int pos = filename.find("  ");
					char_record[i].push_back(filename.substr(0, pos+2));
					filename.erase(0,pos+2);
				}
				if(char_record[i].size()!=char_record[0].size()){
					cout<<"error: size of filename doesn't match in event "<<r->q->getId()<<"\n";
					exit(0);
				}
			}
			for(unsigned i=0; i<char_record[0].size(); i++){
				int flag = 0;
				string string_input[4];
				for(unsigned j=0; j<hlink.size(); j++){
					string_input[j] = char_record[j].at(i);
					if(char_record[j].at(i) != char_record[0].at(i)){
						flag = 1;			
					}
				}
				if(flag == 0){
					target_string += char_record[0].at(i);		
				} else{
					assert(string_input[0].find('(')!=string::npos);
					if(string_input[0].find("-1") == string::npos){
						cout<<"filename is not linear, can not be extrapolate in event"<<r->q->getId()<<"\n";
						exit(0);
					}
					target_string += ext_string(string_input, input_value, ext_target, r);
				}
			}
			dynamic_cast<ParamValue *>(ext_target)->setCharValues(target_string);
			break;
		case OFFSET:

			/*if offset can be record as RSD pattern*/
			if(input_value[0]->getValues()->at(0) == -1 && !not_equal_flag){
				temp.push_back(-1);
				int flag = 0;
				for(unsigned i=0; i<max_i; i++){
					int total = input_value[0]->getValues()->at(i)*(hlink[0]->parent->rank);
					for(unsigned j=1; j<hlink.size(); j++){
						if(input_value[j]->getValues()->at(i)*hlink[j]->parent->rank != total){
							flag = 1;
							break;
						}
					}
					if(flag == 1) break;
				}

				/*strong scaling*/
				if(flag == 0){
					for(unsigned i=1; i<max_i; i++)
						temp.push_back(input_value[0]->getValues()->at(i)*hlink[0]->parent->rank/r->parent->rank);
				} else {  /*weak scaling*/
					for(unsigned i=1; i<max_i; i++){
						for(unsigned j=0; j<hlink.size(); j++){
							gmatrix[j][0] = 1;
							gmatrix[j][1] = hlink.at(j)->x;
							gmatrix[j][2] = hlink.at(j)->x * hlink.at(j)->y;
							gmatrix[j][3] = hlink.at(j)->parent->rank;
							gmatrix[j][4] =	input_value[j]->getValues()->at(i);	
						}	
						gaussian(4);
						extparam =(int) (gmatrix[0][4]+gmatrix[1][4]*r->parent->x + gmatrix[2][4]*(r->parent->x)*(r->parent->y)+gmatrix[3][4]*r->parent->rank);
						temp.push_back(extparam);
					}
				}
				ext_target->setValues(temp);
			} else if (input_value[0]->getValues()->at(0) == -2 && not_equal_flag){  /*offset record as list pattern*/
				
				/*get sorted offset for each trace file*/	
				for(unsigned i=0; i<hlink.size(); i++){
					off_input[i] = (int*)malloc((hlink[i]->parent->rank)*sizeof(int));
					sort = ranksort(hlink[i]->parent->rank);
					for(int j=0; j<sort.size(); j++)
						off_input[i][ sort[j] ] = input_value[i]->getValues()->at(j+1);
				}
				//sort = ranksort(r->parent->rank);
				sort = ranksort(r->parent->rank);
				
				/*check if the offset fit the ((rank+a)%size)*b+c model*/
				if(fitmodel_mod(off_input, r, temp, sort, NULL, NULL)){
					temp.insert(temp.begin(),-2);
				}
				for(int i=0; i<4; i++)
					free(off_input[i]);
				ext_target->setValues(temp);	
			}
			break;
		case TIME:
			for(unsigned i=0; i<input_value[0]->getValues()->size(); i++){
				for(unsigned j=0; j<hlink.size(); j++){
					dimensions[j] = hlink[j]->parent->rank;
					inputs[j] = (double) input_value[j]->getValues()->at(i);
					//if(inputs[j]>10000000) extra = 100.0;
				}
				for(unsigned j=0; j<hlink.size(); j++)
					inputs[j] = inputs[j]/extra;

				rtn[0] = ext_histo_constant(&inputs[0], output_p, max_inaccuracy, 1);
				outputs[0] = *output_p;
				rtn[1] = ext_histo_inverse(&inputs[0], output_p, dimensions, max_inaccuracy, r, 1);
				outputs[1] = *output_p;
				rtn[2] = ext_histo_linear(&inputs[0], output_p, dimensions, r->parent->rank, max_inaccuracy, 1);
				outputs[2] = *output_p;
				rtn[3] = ext_histo_inverse_constant(&inputs[0], output_p, dimensions, r->parent->rank, max_inaccuracy);
				outputs[3] = *output_p;
	
				inttemp = (int)sqrt((double)r->parent->rank);
				if( inttemp * inttemp == r->parent->rank){
					for(unsigned i=0; i<hlink.size(); i++){
						dimensions[i] = (int) sqrt((double)(hlink.at(i)->parent->rank));
					}
					rtn[4] = ext_histo_linear(&inputs[0], output_p, dimensions, inttemp, max_inaccuracy, 1);
					outputs[4] = *output_p;
				} else {
					rtn[4] = INT_MAX;
					outputs[4] = -1.0;
				}
				inttemp = (int) outputs[0];
				min_rtn = rtn[0];
			
				/*	
				cout<<"input time = "<<inputs[0]<<" "<<inputs[1]<<" "<<inputs[2]<<" "<<inputs[3]<<"\n";
				cout<<"output = "<<outputs[0]<<" "<<outputs[1]<<" "<<outputs[2]<<" "<<outputs[3]<<" "<<outputs[4]<<"\n";
				cout<<"rtn = "<<rtn[0]<<" "<<rtn[1]<<" "<<rtn[2]<<" "<<rtn[3]<<" "<<rtn[4]<<"\n";*/	
				for(int j=0; j<num_types; j++){
					if(rtn[j] > 0 && outputs[j]>=0 && min_rtn > rtn[j]){
						inttemp = (int) outputs[j];
						min_rtn = rtn[j];
					}	
				}
				temp.push_back((int)(inttemp*extra));
				//if(min_rtn > max_inaccuracy) cout<<"IOTIME extrapolation is max than threshold"<<"\n";
			}
			free(output_p);
			ext_target->setValues(temp);
			break;

		case RANGES:
			size = input_value[0]->getValues()->size();
			if(size > 0){
				int ranges[size][4];
				for(unsigned i=0; i<hlink.size(); i++){
					for(int j=0; j<size; j++)
						ranges[j][i] = input_value[i]->getValues()->at(j);
				}
				int out[size];
				getcoeff(ranges, out, r, size);
				for(int i=0; i<size; i++)
					temp.push_back(out[i]);
				ext_target->setValues(temp);
			}
			break;

		default:
			break;
	}	
}


string RSD_HLINK::ext_string(string string_input[], ValueSet* input_value[], ValueSet* ext_target, RSD_NODE* r){
	size_t sstart=0, send=0;
	vector<int> num[4];
	vector<int> target;
	string out;
	while((sstart = string_input[0].find('(', send))!=string::npos){
		send = string_input[0].find(')', sstart);
		if(string_input[0].substr(sstart, send-sstart).find(' ')!=string::npos){
			for(unsigned i=0; i<hlink.size(); i++){
				assert(string_input[i].at(sstart) == '(');
				int end = string_input[i].find(')',sstart);
				string snum = string_input[i].substr(sstart+1, end-sstart-1);
			        while(snum.length() >0){
					if(snum.find(' ')!=string::npos){
						num[i].push_back(atoi((snum.substr(0, snum.find(' '))).c_str()));
						snum.erase(0, snum.find(' ')+1);
					}else{
						num[i].push_back(atoi(snum.c_str()));
						snum.clear();
					}
				}
				assert(num[i][0] == -1);
			}
			/*extrapolate for num "-1 start size strid"*/
			for(unsigned j=1; j<4; j++){
				for(unsigned k=0; k<hlink.size(); k++){
					gmatrix[k][0] = 1;
					gmatrix[k][1] = hlink[k]->x;
					gmatrix[k][2] = (hlink[k]->x) * (hlink[k]->y);
					gmatrix[k][3] = hlink[k]->parent->rank;
					gmatrix[k][4] = num[k][j];
				}
				gaussian(4);
				int ext_num = (int) (gmatrix[0][4] + gmatrix[1][4]*r->parent->x + gmatrix[2][4]*(r->parent->x)*(r->parent->y) + gmatrix[3][4]*r->parent->rank);
				target.push_back(ext_num);
			}
			char charnum[30];
			sprintf(charnum, "-1 %d %d %d", target[0], target[1], target[2]);
			out += string_input[0].substr(0, sstart+1);
			out.append(charnum);
			out += string_input[0].substr(send);
			return out;
		}
	}
	return out;
}

/*
void ext_cg(int phase, int64_t **endpoints, int n, int x, int total, int *l){
	int length;
	//int total = x*x;
	int a, b;

	switch (phase){
		case 1:
			length = (int)log2(x);
			*endpoints = (int64_t *)malloc(length*sizeof(int64_t));
			for(int i=0; i<length; i++){
				(*endpoints)[i] = (total + (int)pow(-1, (double)(n/(int)pow(2, (double)i)) ) * (int)pow(2, (double)i)) % total;
			}
			break;
		case 2:
			length = 1;
			*endpoints = (int64_t *)malloc(sizeof(int64_t));
			a = n / x;
			b = n % x;
			if(total == x*x){
				(*endpoints)[0] = (b*x + a + total - n) % total;
			} else {
				(*endpoints)[0] = ((b/2)*x + a*2 + total - (n/2)*2 ) % total;
			}
			break;
		default:
			assert(0);
	}
	*l = length;
}
*/

/*
void RSD_HLINK::ext_rlparam(RSD_NODE *r){

        if(r->q->data.op != 1090 && r->q->data.op != 1113){
                return; 
        }        
        int bases = hlink.size();
        int list_count[bases];
	int count, size, length;
	int *rlist;
	int *endpoints;
	int64_t *rl;
	int64_t *ep;
	cvector *rv;
	cvector *ev;
	source_dest_vector_t *source_dest_node;
	int phase;
	

	for(int i=0; i<bases; i++)
		list_count[i] = 0;

        for(int i=0; i<bases; i++){
                if(source_dest == DEST_COUNT){
			for(int j=0; j<hlink.at(i)->q->data.dest_count; j++){
				rlist = vector_to_array2( 
					get_source_dest_vector(&(hlink.at(i)->q->data.dest_vectors), j)->rank_list, NULL);
				endpoints = vector_to_array2(
                                        get_source_dest_vector(&(hlink.at(i)->q->data.dest_vectors), j)->sd_vector, &size);
	                        list_count[i] += member_count(rlist);
				free(rlist);
				free(endpoints);
			}
		} else if (source_dest == SOURCE_COUNT) {
			for(int j=0; j<hlink.at(i)->q->data.source_count; j++){
				rlist = vector_to_array2(
                                        get_source_dest_vector(&(hlink.at(i)->q->data.source_vectors), j)->rank_list, NULL);
				endpoints = vector_to_array2(
                                        get_source_dest_vector(&(hlink.at(i)->q->data.source_vectors), j)->sd_vector, &size);
                        	list_count[i] += member_count(rlist);
				free(rlist);
				free(endpoints);
			}
		} else {
                        assert(0);
		}
        }

	if(size>1){
		phase = 1;
	} else { 
		phase = 2;
	}

        for ( int j = 0 ; j < hlink.size() ; j ++ ) {
		gmatrix[j][0] = 1 ;
		gmatrix[j][1] = hlink.at(j)->x ;
		gmatrix[j][2] = hlink.at(j)->x * hlink.at(j)->y;
		gmatrix[j][3] = hlink.at(j)->parent->node ;
		gmatrix[j][4] = list_count[j];
	}
	gaussian(4) ;

	count = gmatrix[0][4] + gmatrix[1][4]*r->parent->x + gmatrix[2][4]*(r->parent->x)*(r->parent->y) 
					+ gmatrix[3][4]*r->parent->node;
	if(source_dest == DEST_COUNT){
		destroy_source_dest_vectors(&(r->q->data.dest_vectors));
		r->q->data.dest_vectors = NULL;
		//r->q->data.dest_count = count;
	}else{
		destroy_source_dest_vectors(&(r->q->data.source_vectors));
		r->q->data.source_vectors = NULL;
		//r->q->data.source_count = count;
	}

	for(int i=0; i<count; i++){
		rl = (int64_t *)malloc(5*sizeof(int64_t) );
		rl[0] = 1; rl[1] = -1; rl[2] = i; rl[3] = 1; rl[4] = 0;
		rv = array_to_vector2(rl, 5);
		ext_cg(phase, &ep, i, r->parent->x, r->parent->node, &length);
		ev = array_to_vector2(ep, length);
		source_dest_node = (source_dest_vector_t *)malloc(sizeof(source_dest_vector_t));
		source_dest_node->sd_vector = ev;
		source_dest_node->rank_list = rv;
		source_dest_node->next=NULL;
		if(source_dest == DEST_COUNT)
			enq_source_dest_vectors(&(r->q->data.dest_vectors), source_dest_node);
		else
			enq_source_dest_vectors(&(r->q->data.source_vectors), source_dest_node);
		free(rl);
		free(ep);
	}

	if(source_dest == DEST_COUNT)
		r->q->data.dest_count = source_dest_vectors_size( &(r->q->data.dest_vectors) );
	else
		r->q->data.source_count = source_dest_vectors_size( &(r->q->data.source_vectors) );
}
*/



int	RSD_HLINK::is_count_rp(int *dims, int input[]){

	double total_counts[4];
	double sum=0, avg=0, sd=0;

	double threshold = 0.1;

	int max_pow = 3;
	int xx, yy, zz, x1, y1, z1;

	int x, y, z;
	int dimensions[4];
       	double log_dims[4];

	double min_sd = INT_MAX; // a large value
	double min_avg;

	int p=0;


	for(xx=0; xx<max_pow; xx++){
		for(yy=0; yy<max_pow; yy++){
			for(zz=0; zz<max_pow; zz++){
				for(unsigned i=0; i<hlink.size(); i++){
                                        x = getdim(1, xx, i);
                                        y = getdim(2, yy, i);
                                        z = getdim(3, zz, i);
                                        dimensions[i] = x*y*z;
                                }
				for(x1=0; x1<max_pow; x1++){
					for(y1=0; y1<max_pow; y1++){
						for(z1=0; z1<max_pow; z1++){
							for(unsigned i=0; i<hlink.size(); i++){
								x = getdim(1, x1, i);
								y = getdim(2, y1, i);
								z = getdim(3, z1, i);
								log_dims[i] = log( (double) (x*y*z))/log(2);

								if(x1==0 && y1==0 && z1==0) 
									log_dims[i]=1;
							}
							
							sum = 0;
							sd = 0;

							for(unsigned i=0; i<hlink.size(); i++){
								total_counts[i] = dimensions[i] * log_dims[i] * input[i];
								sum += total_counts[i];	
							}

							avg = sum / double(hlink.size());

							for(unsigned i=0; i<hlink.size(); i++){
								sd += (total_counts[i]-avg) * (total_counts[i] - avg);
							}

							sd = sqrt( sd/(double)(hlink.size() ) );
							sd = sd / avg;

							if(sd < min_sd){
								min_sd = sd;
								min_avg = avg;
								dims[0] = xx;
								dims[1] = yy;
								dims[2] = zz;
								dims[3] = x1;
								dims[4] = y1;
								dims[5] = z1;
							}
						}
					}
				}	

			}
		}
	}
	if(p) printf("%f\n", min_sd);
	return min_sd < threshold ? (int)min_avg : 0;

}


int	RSD_HLINK::ext_p2p_rp(RSD_NODE *r, int c, int *dims){  /*continue here*/
	int x = (int)pow((double)r->parent->x, (double)dims[0]);
	int y = (int)pow((double)r->parent->y, (double)dims[1]);
	int z = (int)pow((double)r->parent->z, (double)dims[2]);
	int out_count = 0;

	if(dims[3]==0 && dims[4]==0 && dims[5]==0)
		out_count = (int)( (double)c / (double)(x * y * z) ); 
	else{
		int x1 = (int)pow((double)r->parent->x, (double)dims[3]);
		int y1 = (int)pow((double)r->parent->y, (double)dims[4]);
		int z1 = (int)pow((double)r->parent->z, (double)dims[5]);

		double logs = log(x1*y1*z1)/log(2);
		out_count = (int) ( (double)c / ( (double)(x*y*z) * logs));	
	}
	return out_count;

}



int	RSD_HLINK::ext_p2p_linear(RSD_NODE *r, int total_counts[]){
		//printf("called !!\n");

	int target_count;

	for(unsigned i=0; i<hlink.size(); i++){
		gmatrix[i][0] = 1;
		gmatrix[i][1] = hlink.at(i)->x;
		gmatrix[i][2] = hlink.at(i)->x * hlink.at(i)->y;
		gmatrix[i][3] = hlink.at(i)->parent->rank ;
		gmatrix[i][4] = total_counts[i];
	}

	gaussian(4);
	target_count = (int) (gmatrix[0][4] + gmatrix[1][4]*r->parent->x + gmatrix[2][4]*(r->parent->x)*(r->parent->y) + gmatrix[3][4]*r->parent->rank);
	return target_count;
}


/*
void    RSD_HLINK::ext_alltoall_rp (RSD_NODE *r){

	double total_counts[4];
	double total_recvcounts[4];
	int rp_count = 1, rp_recvcount = 1;

	for(int i=0; i<hlink.size() ; i++){
		//TODO: none of the below two methods to extrapolate the inverse proportional trend is correct, we should 
		//consider the communicator size here!!!
		//total_counts[i] = hlink.at(i)->q->data.avg_count * hlink.at(i)->parent->node * hlink.at(i)->parent->node;
		//total_recvcounts[i] = hlink.at(i)->q->data.recvcount * hlink.at(i)->parent->node * hlink.at(i)->parent->node;
		total_counts[i] = hlink.at(i)->q->data.avg_count * hlink.at(i)->parent->node ;
		total_recvcounts[i] = hlink.at(i)->q->data.recvcount * hlink.at(i)->parent->node ;
	}

	for(int i=0; i<hlink.size()-1; i++){
		if(total_counts[i] != total_counts[i+1]){
			//counts are not in an inverse proportion relationship
			rp_count = 0;
		}
		if(total_recvcounts[i] != total_recvcounts[i+1]){
			//recvcounts are not in an inverse proportion relationship
			rp_recvcount = 0;
		}
	}

	//TODO: none of the below two methods to extrapolate the inverse proportional trend is correct, we should 
	//consider the communicator size here!!!
	if(rp_count)
		r->q->data.count = (int) ( total_counts[0] / (double)(r->parent->node ) )  ;
	if(rp_recvcount)
		r->q->data.recvcount = (int) ( total_recvcounts[0] / (double)(r->parent->node) )  ;
}
*/

/*
void RSD_HLINK::ext_alltoallv_rp(RSD_NODE *r){

	int hsize = hlink.size();
	rlparam_list_t **rlparams = (rlparam_list_t **)malloc(hsize * sizeof(rlparam_list_t *) );
    int rlparams_length;
	double *constants = (double *)malloc(hsize * sizeof(double) );
	int scaling;//0: unknown, 1:weak, 2:strong
	int *array;
	int *rank_array;
	int size;
	double count;

	for(int i=0; i<hsize; i++){
		if( !(find_rlparam_in_list(&(hlink.at(i)->q->data.rlparam), RECVCOUNTS))
			|| !(find_rlparam_in_list(&(hlink.at(i)->q->data.rlparam), DISPLS))
			|| !(find_rlparam_in_list(&(hlink.at(i)->q->data.rlparam), COUNTS))
			|| !(find_rlparam_in_list(&(hlink.at(i)->q->data.rlparam), RDISPLS)))
		{
			return;
		}
	}

	//extrapolate RECVCOUNTS
	for(int i=0; i<hsize; i++){
		rlparams[i] = get_rlparam_list(find_rlparam_in_list(&(hlink.at(i)->q->data.rlparam), RECVCOUNTS), &rlparams_length);
		assert(rlparams_length == 1);
		constants[i] = 0;
		for(int j=0; j<rlparams[i][0].size; j++){
			constants[i] += rlparams[i][0].values[j];
		}
	}
	// first test weak scaling
	scaling = 1;
	for(int i=0; i<hsize-1; i++){
		if(constants[i] != constants[i+1]){
			// test strong scaling
			scaling = 2;
			break;
		}
	}
	if(scaling == 2){
		for(int i=0; i<hsize - 1; i++){
			if(constants[i]*hlink.at(i)->parent->node != constants[i+1]*hlink.at(i+1)->parent->node){
				// neither weak nor strong
				scaling = 0;
				return;
			}
		}
	}
	switch(scaling){
		case 1:
			rlparam_free(&(r->q->data.rlparam));
			rlparam_init(&(r->q->data.rlparam));
			size = r->parent->node;

			rank_array = (int *)malloc(5 * sizeof(int) );
			rank_array[0]=1;
			rank_array[1]=-1; 
			rank_array[2]=0; 
			rank_array[3]=size; 
			rank_array[4]=1; 

			count = constants[0] / size;
			array = (int *)malloc(size * sizeof(int));
			for(int i=0; i<size; i++) array[i] = (int)count;

			add_rlparam(&(r->q->data.rlparam), RECVCOUNTS, size, array, rank_array);
			add_rlparam(&(r->q->data.rlparam), COUNTS, size, array, rank_array);

			for(int i=0; i<size; i++) array[i] = i*(int)count; 

			add_rlparam(&(r->q->data.rlparam), DISPLS, size, array, rank_array);
			add_rlparam(&(r->q->data.rlparam), RDISPLS, size, array, rank_array);

			free(array);
			free(rank_array);
			break;
		case 2:
			rlparam_free(&(r->q->data.rlparam));
			rlparam_init(&(r->q->data.rlparam));
			size = r->parent->node;

			rank_array = (int *)malloc(5 * sizeof(int) );
			rank_array[0]=1;
			rank_array[1]=-1; 
			rank_array[2]=0; 
			rank_array[3]=size; 
			rank_array[4]=1; 

			count = constants[0] * hlink.at(0)->parent->node / (size * size);
			array = (int *)malloc(size * sizeof(int));
			for(int i=0; i<size; i++) array[i] = (int)count;

			add_rlparam(&(r->q->data.rlparam), RECVCOUNTS, size, array, rank_array);
			add_rlparam(&(r->q->data.rlparam), COUNTS, size, array, rank_array);

			for(int i=0; i<size; i++) array[i] = i*(int)count; 

			add_rlparam(&(r->q->data.rlparam), DISPLS, size, array, rank_array);
			add_rlparam(&(r->q->data.rlparam), RDISPLS, size, array, rank_array);

			free(array);
			free(rank_array);
			break;
		default:
			assert(0);
	}

	free(rlparams);
	free(constants);
}
*/


/*
void    RSD_HLINK::ext_alltoallv_rp(RSD_NODE    *r){
	
	int list_counts[4][4]; 
	double constants[4], sum, avg, diff, *recvcounts, *counts, tmpvalue;
	
	//check if params are valid and param lists match each other
	for(int i=0; i<hlink.size(); i++){
		if (!hlink.at(i)->q->data.rlparam) {
			//incomplete parameters
			return;
		} else {
			for(int j=0; j<4; j++){
				rlparam_t *tmp = ((rlparam_t **)(hlink.at(i)->q->data.rlparam))[j];
				if(!tmp){
					//incomplete parameters
					return;
				} else if (tmp->param != RECVCOUNTS && tmp->param != DISPLS
						&& tmp->param != COUNTS && tmp->param != RDISPLS){
					//incorrect param type
					return;
				} else if (!tmp->value_list || list_size(tmp->value_list) <=0) {
					//incomplete params
					return;
				} else {
					list_counts[i][j] = list_size(tmp->value_list);
					list_t list_node = tmp->value_list;
					do {
						if( ((valuelist_t*)list_value(list_node))->values[0] != hlink.at(i)->parent->node ){
							//the length of the param array should equal to the COMM size
							return;
						}
						if( ((valuelist_t*)list_value(list_node))->rlist[2] != 0 ||
						((valuelist_t*)list_value(list_node))->rlist[3] != hlink.at(i)->parent->node ||	
						((valuelist_t*)list_value(list_node))->rlist[4] != 1)
						{
							//not all the nodes are included in the participant list
							return;
						}
					} while(list_next(list_node));
					if( i == hlink.size()-1 ){
						for(int k=0; k<hlink.size()-1; k++){
							if(list_counts[k][j] != list_counts[k+1][j]){
								//param lists do not match
								return;
							}
						}
					}
				}
			}
		}
	}

	recvcounts = (double *)calloc(list_counts[0][0], sizeof(double));
	counts = (double *)calloc(list_counts[0][2], sizeof(double));

	//do the extrapolation for RECVCOUNTS
	for(int i=0; i<list_counts[0][0]; i++){
		sum = 0;
		for(int j=0; j<hlink.size(); j++){
			rlparam_t *tmp = ((rlparam_t **)(hlink.at(j)->q->data.rlparam))[0];
			int64_t *values = ( (valuelist_t *)list_value( list_at(tmp->value_list, i) ) )->values;
			constants[j] = 0;
			for(int k=1; k<values[0]+1; k++){
				//we do not check whether array alements equal to each other
				constants[j] += values[k];
			}
			constants[j] *= hlink.at(j)->parent->node;
			sum += constants[j];
		}
		// strong scaling
		avg = sum / hlink.size();
		for(int j=0; j<hlink.size(); j++){
			diff = constants[j] < avg ? avg - constants[j] : constants[j] - avg;
			if( diff/avg < ALLTOALLV_CONSTANTS_DIFF){
				recvcounts[i] = avg;
			}else{
				break;
			}
			// no need to test weak scaling
			if(j==hlink.size()-1) continue;
		}

		// weak scaling
		sum = 0;
		for(int j=0; j<hlink.size(); j++){
			constants[j] /= hlink.at(j)->parent->node;
			sum += constant[j];
		}
		avg = sum / hlink.size();
		for(int j=0; j<hlink.size(); j++){
			diff = constants[j] < avg ? avg - constants[j] : constants[j] - avg;
			if( diff/avg < ALLTOALLV_CONSTANTS_DIFF){
                                recvcounts[i] = avg * r->parent->node;
                        }else{
                                //it is neither strong scaling nor weak scaling
                                return;
			}
		}
		
	}

	//do the extrapolation for COUNTS                                                                     
        for(int i=0; i<list_counts[0][2]; i++){
                sum = 0;
                for(int j=0; j<hlink.size(); j++){
                        rlparam_t *tmp = ((rlparam_t **)(hlink.at(j)->q->data.rlparam))[2];                       
                        int64_t *values = ( (valuelist_t *)list_value( list_at(tmp->value_list, i) ) )->values;   
                        constants[j] = 0;                                                              
                        for(int k=1; k<values[0]+1; k++){                                                         
                                //we do not check whether array alements equal to each other                      
                                constants[j] += values[k];                                             
                        }
                        constants[j] *= hlink.at(j)->parent->node;                                     
                        sum += constants[j];
                }
		// strong scaling                                                                   
                avg = sum / hlink.size();
                for(int j=0; j<hlink.size(); j++){
                        diff = constants[j] < avg ? avg - constants[j] : constants[j] - avg;
                        if( diff/avg < ALLTOALLV_CONSTANTS_DIFF){
                                counts[i] = avg;
                        }else{
                                break;
                        }
			// no need to test weak scaling
			if(j==hlink.size()-1) continue;
                }

		//weak scaling
		sum = 0;
                for(int j=0; j<hlink.size(); j++){
                        constants[j] /= hlink.at(j)->parent->node;
                        sum += constants[j];
                }
                avg = sum / hlink.size();
                for(int j=0; j<hlink.size(); j++){
                        diff = constants[j] < avg ? avg - constants[j] : constants[j] - avg;
                        if( diff/avg < ALLTOALLV_CONSTANTS_DIFF){
                                counts[i] = avg * r->parent->node;
                        }else{
				//it is neither strong scaling nor weak scaling
				return;
                        }
                }
        }

	//update the values
	//TODO: there are some memory leak, should free the pointers recursively, fix this in the future
	if(r->q->data.rlparam){
		free(r->q->data.rlparam);
	}
	r->q->data.rlparam = (rlparam_t **)calloc(4, sizeof(rlparam_t *));
	((rlparam_t **)r->q->data.rlparam)[0] = (rlparam_t *)malloc(sizeof(rlparam_t));
	((rlparam_t **)r->q->data.rlparam)[0]->param = RECVCOUNTS;
	((rlparam_t **)r->q->data.rlparam)[0]->value_list = NULL;
	
	((rlparam_t **)r->q->data.rlparam)[1] = (rlparam_t *)malloc(sizeof(rlparam_t));
	((rlparam_t **)r->q->data.rlparam)[1]->param = DISPLS;
	((rlparam_t **)r->q->data.rlparam)[1]->value_list = NULL;

	((rlparam_t **)r->q->data.rlparam)[2] = (rlparam_t *)malloc(sizeof(rlparam_t));
	((rlparam_t **)r->q->data.rlparam)[2]->param = COUNTS;
	((rlparam_t **)r->q->data.rlparam)[2]->value_list = NULL;

	((rlparam_t **)r->q->data.rlparam)[3] = (rlparam_t *)malloc(sizeof(rlparam_t));
	((rlparam_t **)r->q->data.rlparam)[3]->param = RDISPLS;
	((rlparam_t **)r->q->data.rlparam)[3]->value_list = NULL;

	for(int i=0; i<list_counts[0][0]; i++){
		valuelist_t *vlist = (valuelist_t *)malloc(sizeof(valuelist_t));
		vlist->values = (int64_t *)calloc(r->parent->node+1, sizeof(int64_t));
		vlist->rlist = (int64_t *)calloc(5, sizeof(int64_t));
		vlist->values[0] = r->parent->node;
		tmpvalue = recvcounts[i] / (r->parent->node * r->parent->node);
		for(int j=1; j<vlist->values[0]+1; j++){
			vlist->values[j] = (int64_t)tmpvalue;
			//printf("%d ", (int)recvcounts[i]);
		}
		vlist->rlist[0] = 1;
		vlist->rlist[1] = -1;
		vlist->rlist[2] = 0;
		vlist->rlist[3] = r->parent->node;
		vlist->rlist[4] = 1;
		((rlparam_t **)r->q->data.rlparam)[0]->value_list 
			= list_insert(((rlparam_t **)r->q->data.rlparam)[0]->value_list, (void *)vlist);
		//printf("\n\n");
	}

        for(int i=0; i<list_counts[0][2]; i++){                                                                   
                valuelist_t *vlist = (valuelist_t *)malloc(sizeof(valuelist_t));                                  
                vlist->values = (int64_t *)calloc(r->parent->node+1, sizeof(int64_t));                            
                vlist->rlist = (int64_t *)calloc(5, sizeof(int64_t));                                             
                vlist->values[0] = r->parent->node;                                                               
                tmpvalue = counts[i] / (r->parent->node * r->parent->node);                                      
                for(int j=1; j<vlist->values[0]+1; j++){                                                          
                        vlist->values[j] = (j-1)*(int64_t)tmpvalue;      
                }                                                                                                 
                vlist->rlist[0] = 1;                                                                              
                vlist->rlist[1] = -1;                                                                             
                vlist->rlist[2] = 0;                                                                              
                vlist->rlist[3] = r->parent->node;                                                                
                vlist->rlist[4] = 1;                                                                              
                ((rlparam_t **)r->q->data.rlparam)[1]->value_list                                                 
                        = list_insert(((rlparam_t **)r->q->data.rlparam)[1]->value_list, (void *)vlist);      
        }


	for(int i=0; i<list_counts[0][2]; i++){
                valuelist_t *vlist = (valuelist_t *)malloc(sizeof(valuelist_t));
                vlist->values = (int64_t *)calloc(r->parent->node+1, sizeof(int64_t));
                vlist->rlist = (int64_t *)calloc(5, sizeof(int64_t));
                vlist->values[0] = r->parent->node;
                tmpvalue = counts[i] / (r->parent->node * r->parent->node);
                for(int j=1; j<vlist->values[0]+1; j++){
                        vlist->values[j] = (int64_t)tmpvalue;
			//printf("%d ", (int) counts[i]);                                                       
                }
                vlist->rlist[0] = 1;
                vlist->rlist[1] = -1;
                vlist->rlist[2] = 0;
                vlist->rlist[3] = r->parent->node;
                vlist->rlist[4] = 1;
                ((rlparam_t **)r->q->data.rlparam)[2]->value_list 
                        = list_insert(((rlparam_t **)r->q->data.rlparam)[2]->value_list, (void *)vlist);
		//printf("\n\n");     
        }

        for(int i=0; i<list_counts[0][0]; i++){
                valuelist_t *vlist = (valuelist_t *)malloc(sizeof(valuelist_t));
                vlist->values = (int64_t *)calloc(r->parent->node+1, sizeof(int64_t));
                vlist->rlist = (int64_t *)calloc(5, sizeof(int64_t));
                vlist->values[0] = r->parent->node;
                tmpvalue = recvcounts[i] / (r->parent->node * r->parent->node);
                for(int j=1; j<vlist->values[0]+1; j++){
                        vlist->values[j] = (j-1) * (int64_t)tmpvalue;
                }
                vlist->rlist[0] = 1;
                vlist->rlist[1] = -1;
                vlist->rlist[2] = 0;
                vlist->rlist[3] = r->parent->node;
                vlist->rlist[4] = 1;
                ((rlparam_t **)r->q->data.rlparam)[3]->value_list
                        = list_insert(((rlparam_t **)r->q->data.rlparam)[3]->value_list, (void *)vlist);
        }   
	
	//set the flag so that output_rsd_node() will use another function to gen the trace file
	r->q->data.flag = 1;

}*/

#define	SHIFT_SIZE	4
#define	FIELD_NAME_SIZE	32

#define	CHECK(A) {\
	for ( int	j = 0 ; j < hlink.size() ; j ++ ) {\
		RSD_NODE	*v = hlink.at(j) ;\
		ele_arr[j].x = v->parent->x ;\
		ele_arr[j].y = (int ) v->q->A ;\
	}\
	exp_elements(&result) ;\
	if ( state != ST_INVALID )\
		memcpy(&t->A , &result.y , sizeof(int)) ;\
}

// for vector operation
//void	CHECK2(cvector	*A,cvector	*B) {
/*
void	RSD_HLINK::CHECK2(cvector	**B) {
	int	size ;
	int	*r_arr = vector_to_array(*B, &size) ;
	int	*arr_all[256] ; // Anyway..

	int	offset = 0 ;

	if ( *B == NULL ) return ;

	for ( int j = 0 ; j < hlink.size() ; j ++ ) {
		RSD_NODE	*v = hlink.at(j) ;
		ele_arr[j].x = v->parent->node ;
		arr_all[j] =  vector_to_array( v->q->data.dest_vector , &size );
	}

	for ( int i = 0 ; i < size ; i ++ ) {
		for ( int j = 0 ; j < hlink.size() ; j ++ ) {
			int	*k = arr_all[j] ;
			ele_arr[j].y = k[i] ;
		}
		POINT	result;	result.x = target ;//chi
		exp_elements(&result) ;
		if ( state != ST_INVALID )
			memcpy(&r_arr[i] , &result.y , sizeof(int)) ;
	}

	*B = array_to_vector((int64_t *) r_arr , size ) ;
}
*/

/*
void	RSD_HLINK::ext_rsd_node(rsd_node	*t )
{
	POINT	result ; 
	result.x = target ;
	// node
	CHECK(node) 
	// rsd_type_info
	CHECK(rsd_info) 

	// match
	CHECK(match) 

	// data
	CHECK(data.sync_bytes)
	CHECK(data.seq_num)
	CHECK(data.op)
	CHECK(data.arrays)
	CHECK(data.array_size1)
	CHECK(data.array_size2)
	CHECK(data.array_size3)
	CHECK(data.array_size4)
	CHECK(data.array_size_displs)
	CHECK(data.fields)
	CHECK(data.pvectors)
	CHECK(data.rank_offsets)

	CHECK(data.global_world)
	CHECK(data.null_req)
	CHECK(data.comm)
	CHECK(data.count)
	CHECK(data.datatype)
	CHECK(data.recvcount)
	CHECK(data.recvtype)
	CHECK(data.mpi_op)
	CHECK(data.root)
	CHECK(data.tag)

	//CHECK2(&t->data.dest_vector);

	CHECK(data.group)
	CHECK(data.request) 

	CHECK(data.color)
	CHECK(data.key)
	CHECK(data.group1)
	CHECK(data.group2)
	CHECK(data.local_leader)
	CHECK(data.peer_comm)
	CHECK(data.remote_leader)
	CHECK(data.high)

	CHECK(data.recvtag)
	CHECK(data.newtype)
	CHECK(data.oldtype)
	CHECK(data.blocklen)
	CHECK(data.stride)
	CHECK(data.reorder)
	CHECK(data.newrank)
	CHECK(data.outcount)

	for ( int i = 0 ; i < t->data.array_size1 ; i ++ )
		CHECK(data.array1[i]) ;
	for ( int i = 0 ; i < t->data.array_size2 ; i ++ )
		CHECK(data.array2[i]) ;
	for ( int i = 0 ; i < t->data.array_size3 ; i ++ )
		CHECK(data.array3[i]) ;
	for ( int i = 0 ; i < t->data.array_size4 ; i ++ )
		CHECK(data.array4[i]) ;
}
*/


/*
int	RSD_HLINK::exp_elements(POINT	*p)
{
	prex = ele_arr[1].x ;
	prey = ele_arr[1].y ;
	preyv = ele_arr[1].y - ele_arr[0].y ;

	if ( ele_arr[0].x == ele_arr[1].x )
		prediag = 0 ;
	else 
		prediag = (ele_arr[1].y - ele_arr[0].y ) / (ele_arr[1].x - ele_arr[0].x ) ;

	state = (( ele_arr[1].y == ele_arr[0].y ) ? ST_ZERO:ST_CONST) ;
	for ( int i = 2 ; i < hsize ; i ++ ) {
		transition(ele_arr[i].x , ele_arr[i].y) ;
		if ( state == ST_INVALID ) return state ;
	}

	// calc ext value
	switch ( state ) {
		case ST_ZERO:
			p->y = prey ; break ;
		case ST_CONST:
			p->y = prey + prediag * ( p->x - prex ) ;
		default:
			break ;	
	}

	return state ;
}
*/

/*
void	RSD_HLINK::transition(int	x , int	y)
{
	int	yv = y - prey ;
	int	xv = x - prex ;

	if ( xv == 0 ) return ; // invalid

	int	diag = yv / xv ;

	EVENT	event ;
	if ( !diag ) {	// xy == 0 , prey == y 
		event = EV_ZERO ;
	} else {
		if ( prediag == diag )	event = EV_CONSTANT ;
		else if ( prediag < diag )	event = EV_INC ;
		else	event = EV_DEC ;
	}

	switch (state) {
	case	ST_ZERO: 
		if ( event == EV_CONSTANT ) state = ST_CONST ;	
		else if ( event == EV_ZERO ) ;
		else	state = ST_INVALID ;

		break ;
	case	ST_CONST:			
		if ( event == EV_CONSTANT ) ;	
		else if ( event == EV_INC ) state = ST_TWOD;
		else	state = ST_INVALID ;

		break ;
	default:
		state = ST_INVALID;			
		break ;
	}

	prex = x ;
	prey = y ;
	preyv = yv;
	prediag = diag ;
}
*/

int RSD_HLINK :: is_input_const(Param* input_value[], int pos, RSD_NODE* r){

	int input[4],dims[6],rp_c;
        for(unsigned i=0; i<hlink.size(); i++){
		vector<ValueSet*> :: iterator it;
		int sum = 0;
		for(it=input_value[i]->getParamValues()->begin(); it!=input_value[i]->getParamValues()->end(); it++){
			if(pos < (int)(*it)->getValues()->size())
				sum += (*it)->getValues()->at(pos);
			else
				sum += (*it)->getValues()->at(pos % ((*it)->getValues()->size())); 
		}

		double avg = (double)sum/(double)(input_value[i]->getParamValues()->size());

		double rtn=0;
		for(it=input_value[i]->getParamValues()->begin(); it!=input_value[i]->getParamValues()->end(); it++){

			double value = (double)(*it)->getValues()->at(pos % ((*it)->getValues()->size()));
			rtn += (value-avg)*(value-avg);
		}
		rtn = sqrt(rtn/(double)(input_value[i]->getParamValues()->size()));


		if(rtn/avg > const_threshold)
			return -1;

		input[i] = (int)avg;
	}

	if((rp_c = is_count_rp(dims, input)) > 0)
		return ext_p2p_rp(r, rp_c, dims);
	else
		return ext_p2p_linear(r, input);
}

int  RSD_HLINK::not_equal_size(ValueSet* input_value[], vector< vector<int> > &new_value, vector<int> &new_num, RSD_NODE* r){

	int flag = 0;
	int flag1 = 0;
	for(unsigned i=1; i<hlink.size(); i++){
		if(input_value[i]->getValues()->size() != input_value[0]->getValues()->size()){
			flag = 1;
			break;
		}
	}
	if(flag == 0)
		return 0;

	vector<int> num[4];
	for(unsigned i=0; i<hlink.size(); i++){
		vector<int> *temp = input_value[i]->getValues();
		vector<int> value;
		value.push_back((*temp)[0]);
		int number=1;
		for(unsigned j=1; j<temp->size(); j++){
			if((*temp)[j] != (*temp)[j-1]){
				num[i].push_back(number);
				value.push_back((*temp)[j]);	
				number = 1;
			}else
				number += 1;
		}
		num[i].push_back(number);
		new_value.push_back(value);
		if(num[i].size() != temp->size()){
			flag1 = 1;
			if(new_value[i].size()!=new_value[0].size() || num[i].size()!=num[0].size()){
				cout<<"error: parameter size doesn't match for event "<< r->q->getId()<<"\n";
				exit(0);
			}
		}
	}

	if(flag1 == 0) return 1;
	
	for(unsigned i=0; i<num[0].size(); i++){
		for(unsigned j=0; j<hlink.size(); j++){
			gmatrix[j][0] = 1;
			gmatrix[j][1] = hlink.at(j)->x;
			gmatrix[j][2] = hlink.at(j)->x * hlink.at(j)->y;
			gmatrix[j][3] = hlink.at(j)->parent->rank;
			gmatrix[j][4] = num[j].at(i);
		}
		gaussian(4);
		int extnum = (int) (gmatrix[0][4]+gmatrix[1][4]*r->parent->x + gmatrix[2][4]*(r->parent->x)*(r->parent->y)+gmatrix[3][4]*r->parent->rank);
		new_num.push_back(extnum);
	}
	return 1;
}


/*model for offset extrapolation*/

/*this model is about ((rank+a)%size)*b+c*/
int RSD_HLINK::fitmodel_mod(int *input[4], RSD_NODE *r, vector<int> &temp, vector<int> &out, int *start, int *end){
	
	/*get a and b*/
	int abc[5][4],flag=0;
	int coeff[5];
	int S[4],E[4];
	for(unsigned i=0; i<hlink.size(); i++){
		S[i] = start==NULL?0:start[i];
		E[i] = end==NULL?hlink[i]->parent->rank-1:end[i];

		/*abc[1][i] record for stride, which is b*/ 
		if(input[i][S[i]+1]-input[i][S[i] ] > 0){
			abc[1][i] = input[i][S[i]+1] - input[i][S[i] ];
		} else {
			abc[1][i] = input[i][S[i]+2] - input[i][S[i]+1];
		}

		/*abc[2][i] record for c*/
		abc[2][i] = input[i][S[i] ];
		for(int j=S[i]; j<=E[i]; j++)
			abc[2][i] = MIN(abc[2][i], input[i][j]);

		/*abc[0][i] record for a*/
		abc[0][i] = (input[i][S[i] ]-abc[2][i])/abc[1][i];
		if(abc[0][i]!=abc[0][0] ||  abc[1][i]!=abc[1][0] || abc[2][i]!=abc[2][0])
			flag = 1;
		abc[3][i] = S[i];
		abc[4][i] = E[i];
	}

	/*test if the sequence match the model or not, assume offset occure in collective calls, all ranks are involved*/
	for(unsigned i=0; i<hlink.size(); i++){
		for(int j=S[i]; j<=E[i]; j++)
			if((j+abc[0][i])%hlink[i]->parent->rank*abc[1][i]+abc[2][i]!=input[i][j])
				return 0;
	}

	if(flag == 0){
		for(int i=0; i<out.size(); i++){
			temp.push_back((out[i]+abc[0][0])%r->parent->rank*abc[1][0]+abc[2][0]);
		}
		getcoeff(&abc[4], &coeff[4], r, 2);
	} else {
		getcoeff(abc, coeff, r, 5);
		for(int i=0; i<out.size(); i++){
			temp.push_back((out[i]+coeff[0])%r->parent->rank*coeff[1]+coeff[2]);
		}	
	}
	return 1;

}

// for model a*rank_number+b;
int RSD_HLINK::fitmodel_linear(int *input[4], RSD_NODE *r, vector<int> &temp, vector<int> &out, int *start, int *end){

	
	/*get a and b*/
	int a[4][4], flagab=0;
	int S[4],E[4];
	for(unsigned i=0; i<hlink.size(); i++){
		S[i] = start==NULL?0:start[i];
		E[i] = end==NULL?hlink[i]->parent->rank:end[i];
		int diff = input[i][S[i]+1] - input[i][S[i] ];
		for(int j=S[i]+1; j<=E[i]; j++){
			if(input[i][j]-input[i][j-1] != diff)
				return 0;
		}
		a[0][i] = diff;
		a[1][i] = input[i][S[i] ] - diff * S[i];
		a[2][i] = S[i];
		a[3][i] = E[i];
		if(a[0][i]!=a[0][0] || a[1][i] != a[1][0])
			flagab = 1;
	}

	int ab[4];

		/*weak scaling*/
		if(flagab == 0){
			ab[0] = a[0][0];
			ab[1] = a[1][0];
			getcoeff(&a[3], &ab[3], r, 2);
		}

		/*strong scaling*/
		else{
			getcoeff(a, ab, r, 4);
		}

	for(int i=0; i<=out.size(); i++)
		temp.push_back(ab[0]*out[i] + ab[1]);

	return 1;
}


void RSD_HLINK::getcoeff(int a[][4], int *ab, RSD_NODE *r, int size){
	int flag = 0;
	int total[4];
	for(int j=0; j<size; j++){
		for(unsigned i=0; i<hlink.size(); i++){
			total[i] = a[j][i] * hlink[i]->parent->rank;
			if(total[i] != total[0])
				flag = 1;
		}
		if(flag == 0)
			ab[j] = total[0] / r->parent->rank;
		else{
			for(unsigned k=0; k<hlink.size(); k++){
				gmatrix[k][0] = 1;
				gmatrix[k][1] = hlink[k]->x;
				gmatrix[k][2] = hlink[k]->x * hlink[k]->y;
				gmatrix[k][3] = hlink[k]->parent->rank;
				gmatrix[k][4] = a[j][k];	
			}
			gaussian(4);
			ab[j] = (int) (gmatrix[0][4]+gmatrix[1][4]*r->parent->x+gmatrix[2][4]*(r->parent->x)*(r->parent->y)+gmatrix[3][4]*r->parent->rank);
		}	
	}
}
