#include "ecommon.h"
#include "vmaster.h"
#include "rsd_node.h"
#include <assert.h>
#include <string.h>
#include <limits.h>
#include <math.h>


VMASTER::VMASTER(char	*filename, int	rank)
{
	rtrace = Trace::inputTrace(filename, INPUT_GLOBAL, -1) ;
	strcpy( fname, filename) ;

	rtrace->resetTraversalIterators();
	this->rank = rank ;

	x = y = z = 1 ;

	maxDepth = 1 ;
	
	adj_lists = (vector<int> **)calloc(rank, sizeof(vector<int> *)); /*allocate rank units of int*/
	adj_freq = (vector<int> **)calloc(rank, sizeof(vector<int> *));
	
	pattern_type = (int *)calloc(rank, sizeof(int) );
	memset(adj_lists, 0, rank*sizeof(vector<int> *));
	memset(adj_freq, 0, rank*sizeof(vector<int> *));
	Event *e = rtrace->getHead();

	while(e) {
		RSD_NODE	*temp = new RSD_NODE(this, e) ;
		get_max_size(temp) ;
		vq.push_back(temp);	
		e = e->next;
	}
}

VMASTER::~VMASTER()
{
	for (vector<RSD_NODE *>::iterator it = vq.begin(); it != vq.end() ; it ++) {
		delete	*it ;
	}
	free(pattern_type);
	for(int i=0; i<rank; i++)
		delete adj_lists[i];
	free(adj_lists);
	for(int i=0; i<rank; i++)
		delete adj_freq[i];
	free(adj_lists);
	rtrace->~Trace();	// free all the events in rtrace
}


// scan rsd_node to decide MAX dimension and x,y,z
void	VMASTER::get_max_size(RSD_NODE	*r)
{
	// Must traverst only the communicative API ( rank_node )
	if ( !is_communicate(r->q->getOp())) return ;

	// RSD_NODE already decode rsd_node.RSD_NODE depth info start_rank, rank_depth, count, stride
	maxDepth = MAX(maxDepth , r->maxdepth) ;
}

static int compare_int(const void *p1, const void *p2){
	if( *((int*)p1) > *((int*)p2) ) return 1;
	else if( *((int*)p1) == *((int*)p2) ) return 0;
	else return -1;
}

void VMASTER::print_adjlist(){
	printf("\n\nadjacent list starts here\n\n");
	for(int i=0; i< rank; i++){
		printf("%d dest:   ", i);
		for(vector<int>::iterator it = (*(adj_lists[i])).begin(); it!=(*(adj_lists[i])).end(); it++  ){
			printf("%d ", *it);
		}
		printf("\n");
#ifdef CG
		printf("%d freq:   ", i);
                for(vector<int>::iterator it = (*(adj_freq[i])).begin(); it!=(*(adj_freq[i])).end(); it++  ){
                        printf("%d ", *it);
                }
                printf("\n");
#endif
	}
	printf("\n\nadjacent list ends here\n\n");
}


void VMASTER::update_adjlist(vector<int> *values, vector<int> &ranks){
	for(unsigned i=0; i<ranks.size(); i++){
		int Rank = ranks[i];
		if(adj_lists[Rank] == NULL){
			adj_lists[Rank] = new vector<int>();
			adj_freq[Rank] = new vector<int>();
		}
		for(unsigned j=0; j<(*values).size(); j++){
			int trans_dest = (ranks[i]+(*values).at(j)) % rank - ranks[i];
			vector<int>::iterator it = find((*(adj_lists[Rank])).begin(), (*(adj_lists[Rank])).end(), trans_dest);
			if(it == (*(adj_lists[Rank])).end())
				(*(adj_lists[Rank])).push_back(trans_dest);
		}
	}
}





void generate_rank_member(DIM *d, vector<int> &ranks){
	int start, start_pos, i, pos;
	vector<int> slist = d->single_list;
	assert(slist.size()>=4);
	pos = 0;
	for(i=0; i<slist[2]; i++)
		ranks.push_back( slist[1]+i*slist[3] );
	
	if(slist[0] != -1){
		assert(slist.size()>=6);
		while(1){
			start = ranks.at(pos);
			for(i=1; i<slist[4]; i++)
				ranks.push_back( start+i*slist[5] );
			if(pos < slist[2]-1) pos += 1;
			else break;
		}	
	}
	if(slist[0] == -3){
		assert(slist.size()==8);
		pos = 0;
		while(1){
			start = ranks.at(pos);
			for(i=1; i<slist[6]; i++)
				ranks.push_back( start+i*slist[7] );
			if(pos < start_pos+slist[2]*slist[4]-1) pos+=1;
			else break;
		}
	}
}


void VMASTER::scan_sizes(){  //for generate x, y, z, scan the size of each dimension

	int all_collective = 1;//if there is send/recieve operation, all_collective = 0
	Event *e;
	
	for(vector<RSD_NODE *>::iterator it = vq.begin(); it!=vq.end(); it++){
		e = (*it)->q;
		id = e->getId();
		if( is_sendop( e->getOp() ) ){
			all_collective = 0;
			// build the adjacent list
			map<ValueSet*, EPARAM*>::iterator vit = ((*it)->VSEP).begin();
			for(; vit!=((*it)->VSEP).end(); vit++){
				vector<int> *dest_values = vit->first->getValues();
				for(int i=0; i<vit->second->num_ranklist; i++){ //generate rank members
					vector<int> ranks;
					generate_rank_member(&((vit->second->pxyz)[i]), ranks);	
					update_adjlist(dest_values, ranks);	
				} //already generate the rank members, need to generate adj list later
			}
		}
	}

#ifdef CG
        filter_adjlist();
#endif
        //print_adjlist();
#ifdef CG
	if(all_collective == 1){
                x = rank;
                y = 1;
                z = 1;
        } else {
                build_pattern_lib();
                resolve_sizes();
        }
#else
	if(all_collective == 1){
		x = rank;
		y = 1;
		z = 1;
	}else if(maxDepth == 1){
		x = rank;
		y = 1;
		z = 1;
	} else {
		build_pattern_lib();
		/*printf("groupsize= %d\n", pattern_group_size.size());
		for(unsigned k=0; k<pattern_group_size.size();k++){
			printf("num=%d, pattern=%d %d %d %d\n", pattern_group_size[k], (*(pattern_lib[k])), (*(pattern_lib[k]+1)), (*(pattern_lib[k]+2)),(*(pattern_lib[k]+3)));
		}*/
		resolve_sizes();
	}
#endif

	printf("x=%d, y=%d, z=%d\n", x, y, z);                                                                    

        for ( vector<RSD_NODE *>::iterator it = vq.begin() ; it != vq.end() ; it ++ ) {                           
                (*it)->decode( x , y , z) ;                                                                       
        }
}



void VMASTER::resolve_sizes(){  //change here to tollerate 2 and 3 dimension
	int *dest_array = NULL;
	int *corner = (int *)calloc(maxDepth, sizeof(int));
	int nextcorner=0;
	int number_of_corners = 0;
	int finished=0;
	int j;
	int loop;
	int corner_group_size;

	loop = detect_loop();

	printf("loop = %d \n", loop);

#ifdef CG
	x = loop;
	y = rank/loop;
	z = 1;
	maxDepth = 2;
	return; 
#endif

	corner_group_size = rank/loop;

	vector<int>::iterator group_it;
        
        for(int i=0; i<rank; i++){                                                                                

                dest_array = (int *)calloc((*(adj_lists[i])).size(), sizeof(int));       
		j=0;                         
                for(vector<int>::iterator it = (*(adj_lists[i])).begin(); it!=(*(adj_lists[i])).end(); it++ ){
                        dest_array[j] = (int)(*it);
			j++;
                }                                                                                                 
                qsort(dest_array, j, sizeof(int), compare_int);                                                        

		group_it = pattern_group_size.begin();
                for(vector<int *>::iterator iit=pattern_lib.begin(); iit!=pattern_lib.end(); iit++ ){

                        if( memcmp(dest_array, *iit, j*sizeof(int))==0 && *group_it== corner_group_size ){
				if(number_of_corners==0 || number_of_corners==1 ||number_of_corners==3){
                   			corner[nextcorner]=i;      
					nextcorner++;
					if(nextcorner == MIN(maxDepth,3)){
						finished=1;
						break;				
					}                                    
				}
				number_of_corners++;
                        }
			group_it++;                
                }
                free(dest_array);
		if(finished) break;
        }


	if(nextcorner == 2){
		x = corner[1] - corner[0] + 1;
		y = rank / x;
	} else if (nextcorner == 3) {
		x = corner[1] - corner[0] + 1;
		y = (corner[2] - corner[0] + 1)/x;
		z = rank / (x * y);
	}
}


int VMASTER::detect_loop(){ /*for detect how many communication group*/
	
	int start = 0;
	int end, search_start, search_end;
	int loop_found=1;

	for(int i=1; i<rank; i++){
		if(pattern_type[i]==pattern_type[0]){
			end = i-1;
			search_start = i;
			search_end = search_start + (end - start);
			if(pattern_type[end] == pattern_type[search_end]){
				for(int j=0; j<search_start; j++){
					if(pattern_type[j]!=pattern_type[search_start+j]){
						loop_found=0;
						break;
					}
				}
				if(loop_found){
					return search_start - start;
				}
				loop_found = 1;
			}
		}
	}
	return rank;
}


void VMASTER::build_pattern_lib(){

	int *dest_array = NULL;
	int j, exists=0, k;
	vector<int>::iterator group_it;

	for(int i=0; i<rank; i++){

		dest_array = (int *)calloc((*(adj_lists[i])).size(), sizeof(int));
		j=0;
		for(vector<int>::iterator it = (*(adj_lists[i])).begin(); it!=(*(adj_lists[i])).end(); it++  ){
			dest_array[j] = *it;	
			j++;
		}
		qsort(dest_array, j, sizeof(int), compare_int);
		k=0;
		group_it=pattern_group_size.begin();
		for(vector<int *>::iterator iit=pattern_lib.begin(); iit!=pattern_lib.end(); iit++){
			if( memcmp(dest_array, *iit, j*sizeof(int))==0 ){
				exists=1;
				pattern_type[i] = k;
				(*group_it)++;
				break;
			}
			k++;
			group_it++;
		}
		if(!exists){
			pattern_lib.push_back((int *)calloc(j, sizeof(int)));
			memcpy(pattern_lib.back(), dest_array, j*sizeof(int));
			pattern_group_size.push_back(1);
			pattern_type[i] = k;
		}

		free(dest_array);
		exists = 0;
	}
}


void VMASTER::scan_dimension(){ /*this is the max dimension of whole trace*/
	for(vector<RSD_NODE *>::iterator it = vq.begin(); it!=vq.end(); it++){
		int temp_depth = (*it)->get_maxdepth();
		maxDepth = MAX(maxDepth, temp_depth);	
	}
}


int	VMASTER::islcd(int	t , int	lcd_val)
{
	if ( (t % lcd_val) == 0 ) return 1 ;
	else return -1 ;
}

// recursive function to decide x,y,z 
void	VMASTER::_decompose(int	node_num , int	token)
{
	//printf("node_num=%d, token=%d\n",node_num, token);
	if ( token == 1 ) {	// Add to the vecotrs
		if ( node_num == 1 ) return ;

		lcd_val[maxDepth - token] = node_num ;

		//if ( !isArranged() ) return ;

		LCDSET	ls ;
		memcpy(ls.lcd_val, lcd_val , 3 * sizeof(int)) ;
		ls.ltype = LCDT_3D ;		//
		lcd_vec.push_back(ls) ;	//
	} else {
		for ( int i = 2 ; (i <= node_num / 2); i ++ ) {
			if ( islcd(node_num , i ) > 0 )	{ 
				lcd_val[maxDepth - token] = i ;
				_decompose( node_num / i, token - 1) ;
			}
		}
	}
}

void 	VMASTER::print(char *filename)
{
	rtrace->outputTrace(filename);
}
