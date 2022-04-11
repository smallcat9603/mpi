/**
 * Author: Xing Wu
 */

#ifndef __HISTOGRAM_H__
#define __HISTOGRAM_H__

#include <string>
#include <sstream>
#include <vector>
#include <float.h>
#include "HistoBin.h"
#include "ValueSet.h"
#include "ParamValue.h"
#include <mpi.h>
#undef max
#undef min

#define NUM_BINS 5

//TODO: find a better way for this
#define DOUBLE_EPSILON 0.000001

using namespace std;

extern int my_rank;

class Histogram : public ValueSet {

private:

    int num_elems;        /* num of values added to the histogram */
    int num_bins;         /* num of bins */
    int max_rank;         /* recorded rank of node with max value */
    int min_rank;         /* recorded rank of node with min value */
    double max_value;
    double min_value;
    double avg_value;


    vector<HistoBin *> bins;
  
    /**
     * 'smoothen' the histogram - finds the largest and smallest bins and then
     * splits the large bin only when two bins can be merged. This ensures that the
     * number of bins is maintained constant.
     */
    void smooth();

    /**
     * Given a value, find the bin to which it falls into in this histogram.
     */
    int find_bin(double value);



public:

    /**
     *  Creates an empty histogram with numBins bins.
     */
    Histogram();

	vector<HistoBin *> *getBins(){
		return &bins;
	}

    /**
     * free all allocated memory for histogram
     */
    ~Histogram();
  
    /**
     * Add a value v to an existing histogram. This method will take a value and add
     * it into the appropriate histogram while updating the average and standard
     * deviation.
     *
     * If this is the first add, bin sizes are set so that the histogram's range is
     * 2v with value at the center of the range.
     */
    void add(double value, int rank = -1);

    /**
     * Merges other histogram into this one.  
     * Note: this will alter this histogram.
     */
    bool merge(Histogram *other);
	bool merge(ParamValue *pv);


  
    /* minimum value in this histogram */
    double min(){
        return min_value;
    }
    /* Updates min value according to new value + rank. */
    void update_min(double v, int rank) {
        if (v < min_value) {
            min_value = v;
            min_rank = rank;
        }
    }

    /* maximum value in this histogram */
    double max(){
        return max_value;
    }
    /* Updates the max value and max rank. */
    void update_max(double v, int rank) {
        if (v > max_value) {
            max_value = v;
            max_rank = rank;
        } 
	}
    void set_max(double v, int rank){
	    max_value = v;
	    max_rank = rank;
    }
    void set_min(double v, int rank){
	    min_value = v;
	    min_rank = rank;
    }
    void set_avg(double v, int num){
	    
	    avg_value = v;
	    num_elems = num>0?num:num_elems;
    }

	/* get a random value, the seed should be set elsewhere */
	double randomValue(int rand);

    /* average value of this histogram */
    double avg(){
        return avg_value;
    }
    /* update the avg value and count */
    void update_avg(double v, int n) {
		if(num_elems == 1){
			avg_value = v;
		} else {
	        avg_value = (double)(avg_value * num_elems + v * n) / (double)(num_elems + n) ;
		}
    }

	int getMaxRank(){
		return max_rank;
	}
	int getMinRank(){
		return min_rank;
	}
	int getNumElems(){
		return num_elems;
	}

    string toString();
	string valuesToString();
	void input(string& buf);

	int getPackedSize();
	void pack(void *buf, int bufsize, int *position, MPI_Comm comm);
	void unpack(void *buf, int bufsize, int *position, MPI_Comm comm);
};

  
#endif /*__HISTOGRAM_H__*/
  
