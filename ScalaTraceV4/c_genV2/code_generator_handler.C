#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "common.h"
#include "code_generator_handler.h"
#include "op_generator.h"

//#define DEBUG

int indent;

extern generator_context_t* h_gen_context;

extern var_dump_t * var;

void generate_compute_time(FILE *skel_fh, Event* event){
  //assuming that there aren't more than 125 entries
  char times_str[2000];
 
  map<int, Stat*>::iterator event_stat_iter;
  Stat* event_stat;


  event_stat_iter = event->getCompStats()->find(STAT_TIME);
  event_stat = event_stat_iter->second; 

  if( event_stat  == NULL ){
    printf("ERROR: Event stat can't be NULL\n");
    assert(0);
  }
  map<StackSig, Histogram *> stats = *(event_stat->getPathstats());

  //construct times_str string
  //Array FORMAT: { {prev_event, event_time}, .. }
  char *pos = times_str;
  map<StackSig, Histogram *>::iterator stats_itr;

  int entry_cnt = 0;
  pos += sprintf(pos,"{");  
  for(stats_itr = stats.begin(); stats_itr != stats.end();
                                   stats_itr++){
    int comp_time =  (int) stats_itr->second->avg();
    /*
     * Keeping all possible events for a stack signature.
     * */
    pair<multimap<StackSig, int>::iterator, 
          multimap<StackSig, int>::iterator> stack_sigs;
    multimap<StackSig, int>::iterator stack_sig;
    stack_sigs = h_gen_context->event_nums->equal_range(stats_itr->first);
    for(stack_sig = stack_sigs.first; stack_sig != stack_sigs.second; stack_sig++){
      pos += sprintf(pos, "{%d, %d},", stack_sig->second, comp_time);
      entry_cnt++;
    }
  }
  //removing trailing comma
  pos--;
  pos += sprintf(pos,"}");

  //print compute time
  printindent(skel_fh);
  fprintf(skel_fh, "do_compute(%d, (event_time_t[])%s, %d);\n", 
		  event->getId(), times_str, entry_cnt);
}


/*
 * NOTE: This function is nolonger being used. New approach
 *       (keeping stack signatures/event numbers in benchmark itself) is used to generate compute time.
 *
 * Helper function to generate compute time based on the previous event
 * executed. 
 * Need to be called before generating the code for event.
 * Parameters:  skel_fh : File handle
 *              event  : Event
 * 
 */
void generate_compute_time_v1(FILE *skel_fh, Event* event){
#ifdef DEBUG
  printf("%s-Event %d START\n", __func__, event->getOp());
#endif 

  map<int, Stat*>::iterator event_stat_iter;
  Stat* event_stat;

  event_stat_iter = event->getCompStats()->find(STAT_TIME);
  event_stat = event_stat_iter->second; 

  if( event_stat  == NULL ){
    printf("ERROR: Event stat can't be NULL\n");
    assert(0);
  }
  map<StackSig, Histogram *> stats = *(event_stat->getPathstats());
  Ranklist* remaining_ranks = new Ranklist(*(event->getRanklist()));

  Event* prev_event = event->prev;
  while( !stats.empty() ){

    //searching backwards to find the entries
    while(prev_event != NULL) {
      StackSig* prev_event_sig = prev_event->getSignature();

      map<StackSig, Histogram *>::iterator stats_iter;
      stats_iter = stats.find(*prev_event_sig);

      if( stats_iter != stats.end() ){
        /*event with matching signature is found*/
        
        //get common ranks
        Ranklist* common_ranks = remaining_ranks->getCommonRanklist(prev_event->getRanklist()); 

        bool is_cond_reqd = false;
        //add condition if  prev event didn't run for all ranks of current event
        if( !common_ranks->equals(event->getRanklist() ) ){
          is_cond_reqd = true;
          generate_ranklist_cond(common_ranks, true);
        }

        //print compute time
        printindent(skel_fh);
        fprintf(skel_fh, "compute(%d);\n", (int)event_stat->getValue(prev_event_sig));
        
        if(is_cond_reqd){
          indent--;
        }
        //update remaining ranks
        remaining_ranks->removeRanks(common_ranks);

        delete common_ranks;

        //clear entry for the matched stack signature
        stats.erase(*prev_event_sig); 
      }	
      prev_event = prev_event->prev;
    }

   /*
    * 1. Is it possible to decide whether to traverse forward or backward based on stack signature
    * 2. Loop is handled in generate_loop_start(look at that and understand any problem in passing loop tail)
    */
  }

  //clean up
  delete remaining_ranks;

#ifdef DEBUG
  printf("%s-Event %d END\n", __func__, event->getOp());
#endif 
}

/*
 * Definition: Returns True, if loop need to be generated.
 * Parameters: loophead:
 *                depth:
 */
bool is_generate_loop(Event* loophead, int depth)
{
  bool status = false;
  Loop* loopstack;

  if(loophead == NULL || 
      (loopstack = loophead->getLoopStack()) == NULL){
    fprintf(stderr, "ERROR: %s: loop is null \n", __func__);
    return status;
  }
  
  if(loopstack->getDepth() > depth){
    status = true;
  }

  return status;
}

/*
 * Generates the code at start of loop
 * Parameters:  loophead: 
 *                 depth: current depth of loophead
 */
void generate_loop_start_code(Event* loophead, int depth)
{
  /*
   * Loop variables: cur_iter[#]: iteration variable
   *                 iters_len[#]: iteration length
   *                 cur_mems[#]: members count variable
   *                 mems_len[#]: total members
   * */
  int loop_depth;

  //keep track of current loop depth
  h_gen_context->nest_loops_depth++;
  loop_depth = h_gen_context->nest_loops_depth;

  Loop* loopstack;
  loop_t *loop;
  vector<ValueSet *> *mems;
  vector<ValueSet *> *iters;

  loopstack = loophead->getLoopStack();
  loop = loopstack->getLoopAt(depth);
  if(loop == NULL){
    fprintf(stderr,"%s: No loop at depth:%d \n", __func__, depth);
    return;
  }

  mems = loop->mem->getParamValues();
  iters = loop->iter->getParamValues();

  char* iters_str = construct_value_string(iters, true);
  char* mems_str = construct_value_string(mems, true);

  //generate loop start code 
  FILE* skel_fh = h_gen_context->skel_fh;

  int loop_idx = loop_depth - 1;

  /* 
   *Note:mems len should be retrieved first, 
   * as it may be used in get_iters_length incase of dummy loops
   * */
  printindent(skel_fh);
  fprintf(skel_fh, "mems_len[%d] = get_mems_length(myrank, \"%s\", %d, %d);\n",
                          loop_idx, mems_str, h_gen_context->loop_count, loop_depth);
  printindent(skel_fh);
  fprintf(skel_fh, "iters_len[%d] = get_iters_length(myrank, \"%s\", %d, %d);\n",
                          loop_idx, iters_str, h_gen_context->loop_count, loop_depth);
  printindent(skel_fh);
  fprintf(skel_fh,"for( cur_iter[%d] = 0; cur_iter[%d] < iters_len[%d]; cur_iter[%d]++ ) {\n",
                  loop_idx, loop_idx, loop_idx, loop_idx);
  indent++;

  //increment total number of loops encountered
  h_gen_context->loop_count++;

#ifdef DEBUG
  fprintf(stderr,"%s: max_depth-%d and loop_depth-%d \n", __func__, 
                                 h_gen_context->max_loops_depth, loop_depth);
#endif

  free(iters_str);
  free(mems_str);
}

/*
 * Generates the code required at end of loop.
 */
void generate_loop_end_code(){
  FILE* skel_fh = h_gen_context->skel_fh; 
  int loop_depth = h_gen_context->nest_loops_depth;
  
  printindent(skel_fh);
  fprintf(skel_fh, "update_mem_cnts(%d);\n", loop_depth);

  indent--;
  printindent(skel_fh);
  fprintf(skel_fh, "}\n");

  h_gen_context->nest_loops_depth--;

#ifdef DEBUG
  fprintf(stderr, "%s: updated loop depth %d \n", __func__, 
                                        h_gen_context->nest_loops_depth);
#endif
}

void printindent(FILE *file){
	int i;
	for(i=0; i<indent; i++)
		fprintf(file, "\t");
}

/*
 * Utility function to generate condition checks.
 * Decrementing the indent and closing the brace(if not inline) are the
 * responsibilities of the caller. 
 * Parameters: param_values:
 *		  is_inline:whether condition need to be generated inline.
 */
void generate_ranklist_cond(Ranklist *rank_list,bool is_inline){
  FILE* skel_fh = h_gen_context->skel_fh;
  printindent(skel_fh);
  fprintf(skel_fh, "if(is_member(myrank, \"%s\")", rank_list->toString().c_str());

  if(is_inline){
    fprintf(skel_fh, ")\n");
  }
  else
  {
    fprintf(skel_fh, "){\n");
  }
  indent++;
}

var_dump_t* init_var_dump() {
  var_dump_t* var = NULL;
  var = (var_dump_t*)malloc(sizeof(var_dump_t));
  var->total_count = 0;
  return var;
}


int add_var_dump(var_dump_t* var, char* detail, int is_global) {
  int success = 1;
  if(var == NULL) {
    var = init_var_dump();
  }
  if(var != NULL && strlen(detail)>0 && var->total_count < (VAR_LIST_SIZE-1)) {
    var->var_detail[var->total_count] = (char*)malloc((strlen(detail)+1)*sizeof(char));
    strcpy(var->var_detail[var->total_count], detail);
    var->is_global[var->total_count] = is_global;
    (var->total_count)++;
    success = 0;
  }
  return success;
}

void free_var_dump(var_dump_t* var) {
  int i = 0;
  if(var != NULL && var->total_count > 0) {
    for(i=0;i<var->total_count;i++) {
      free(var->var_detail[i]);
    }
    free(var);
  }
}

derived_type_dump_t* init_derived_type_dump() {
  derived_type_dump_t* type = NULL;
  type = (derived_type_dump_t*)malloc(sizeof(derived_type_dump_t));
  type->total_count = 0;
  return type;
}

int add_derived_type(derived_type_dump_t* type, int trace_index) {
  int success = 1;
  char typevar[20];
  char typedet[100];
  char* check = NULL;
  if(type == NULL) {
    type = init_derived_type_dump();
  }
  if(check = get_type_name_from_index(type, trace_index)) {
    success = 0;
    return success;
  }
  if(type != NULL && type->total_count < (TYPE_LIST_SIZE-1)) {
    sprintf(typevar, "datatype%d",type->total_count);
    sprintf(typedet, "MPI_Datatype %s;",typevar);
    type->type_detail[type->total_count] = (char*)malloc((strlen(typedet)+1)*sizeof(char));
    type->type_name[type->total_count] = (char*)malloc((strlen(typevar)+1)*sizeof(char));
    strcpy(type->type_detail[type->total_count], typedet);
    strcpy(type->type_name[type->total_count], typevar);
    type->is_derived_type[type->total_count] = 1;
    type->type_trace_index[type->total_count] = trace_index;
    (type->total_count)++;
    success = 0;
  }
  return success;
}

int add_defined_type(derived_type_dump_t* type, char* name, int trace_index) {
  int success = 1;
  char* check = NULL;
  if(type == NULL) {
    type = init_derived_type_dump();
  }
  if(check = get_type_name_from_index(type, trace_index)) {
    success = 0;
    return success;
  }
  if(type != NULL && type->total_count < (TYPE_LIST_SIZE-1)) {
    type->type_name[type->total_count] = (char*)malloc((strlen(name)+1)*sizeof(char));
    strcpy(type->type_name[type->total_count], name);
    type->is_derived_type[type->total_count] = 0;
    type->type_trace_index[type->total_count] = trace_index;
    (type->total_count)++;
    success = 0;
  }
  return success;
}

void update_usage_count(derived_type_dump_t* type, int usage_count, int trace_index) {
  int i=0;
  if(type != NULL && type->total_count > 0) {
    for(i=0;i<type->total_count;i++) {
      if(type->type_trace_index[i] == trace_index) {
        if(type->type_usage_count[i] < usage_count) {
          type->type_usage_count[i] = usage_count;
        }
        break;
      }
    }
  }
}

char* get_type_name_from_index(derived_type_dump_t* type, int trace_index) {
  char* type_name = NULL;
  int i=0;
  if(type == NULL) {
    return type_name;
  }
  if(type!=NULL && type->total_count>0) {
    for(i=0;i<type->total_count;i++) {
      if(trace_index == type->type_trace_index[i]) {
        type_name = type->type_name[i];
        break;
      }
    }
  }
  return type_name;
}

void free_derived_type_dump(derived_type_dump_t* type) {
  int i = 0;
  if(type != NULL && type->total_count > 0) {
    for(i=0;i<type->total_count;i++) {
      free(type->type_detail[i]);
      free(type->type_name[i]);
    }
    free(type);
  }
}

file_handle_dump_t* init_fh_dump() {
  file_handle_dump_t* dump = NULL;
  dump = (file_handle_dump_t*)malloc(sizeof(file_handle_dump_t));
  dump->total_count = 0;
  return dump;
}

int add_file_handle(file_handle_dump_t* dump, int trace_index) {
  int success = 1;
  char fhvar[20];
  char fhdet[100];
  char* check = NULL;
  if(dump == NULL) {
    dump = init_fh_dump();
  }
  if(check = get_file_handle_from_index(dump, trace_index)) {
    success = 0;
    return success;
  }
  if(dump != NULL && dump->total_count < (TYPE_LIST_SIZE-1)) {
    sprintf(fhvar, "fh%d",dump->total_count);
    sprintf(fhdet, "MPI_File %s;",fhvar);
    dump->fh_detail[dump->total_count] = (char*)malloc((strlen(fhdet)+1)*sizeof(char));
    dump->fh_name[dump->total_count] = (char*)malloc((strlen(fhvar)+1)*sizeof(char));
    strcpy(dump->fh_detail[dump->total_count], fhdet);
    strcpy(dump->fh_name[dump->total_count], fhvar);
    dump->fh_trace_index[dump->total_count] = trace_index;
    (dump->total_count)++;
    success = 0;
  }
  return success;
}


char* get_file_handle_from_index(file_handle_dump_t* dump, int trace_index) {
  char* fhname = NULL;
  int i=0;
  if(dump == NULL) {
    return fhname;
  }
  if(dump!=NULL && dump->total_count>0) {
    for(i=0;i<dump->total_count;i++) {
      if(trace_index == dump->fh_trace_index[i]) {
        fhname = dump->fh_name[i];
        break;
      }
    }
  }
  return fhname;
}

void free_fh_dump(file_handle_dump_t* dump) {
  int i = 0;
  if(dump != NULL && dump->total_count > 0) {
    for(i=0;i<dump->total_count;i++) {
      free(dump->fh_detail[i]);
      free(dump->fh_name[i]);
    }
    free(dump);
  }
}

//Vivek - Start
comm_dump_t* init_comm_dump() {
  comm_dump_t* comm = NULL;
  comm = (comm_dump_t*)malloc(sizeof(comm_dump_t));
  comm->total_count = 0;
  for (int i =0; i < COMM_LIST_SIZE; i++)
      comm->split_tracker_flag[i]=0;
  comm->comm_name[comm->total_count] = (char*)malloc((strlen("MPI_COMM_WORLD")+1)*sizeof(char));
  strcpy(comm->comm_name[comm->total_count], "MPI_COMM_WORLD");
  comm->comm_trace_index[comm->total_count] = 2;
  (comm->total_count)++;
  return comm;
}

int add_comm(comm_dump_t* comm_dump, char* name, int trace_index) {
  int success = 1;
  char* check = NULL;
  if(comm_dump == NULL) {
    comm_dump = init_comm_dump();
  }
  if(check = get_comm_name_from_index(comm_dump, trace_index)) {
    success = 0;
    return success;
  }
  if(comm_dump != NULL && comm_dump->total_count < (COMM_LIST_SIZE-1)) {
    comm_dump->comm_name[comm_dump->total_count] = (char*)malloc((strlen(name)+1)*sizeof(char));
    strcpy(comm_dump->comm_name[comm_dump->total_count], name);
    comm_dump->comm_trace_index[comm_dump->total_count] = trace_index;
    (comm_dump->total_count)++;
    success = 0;
  }
  return success;
}

char* get_comm_name_from_index(comm_dump_t* comm, int trace_index) {
  char* commname = NULL;
  int i=0;
  if(comm == NULL) {
    return commname;
  }
  if(comm!=NULL && comm->total_count>0) {
    for(i=0;i<comm->total_count;i++) {
      if(trace_index == comm->comm_trace_index[i]) {
        commname = comm->comm_name[i];
        break;
      }
    }
  }
  return commname;
}

void free_comm_dump(comm_dump_t* comm) {
  int i = 0;
  if(comm != NULL && comm->total_count > 0) {
    for(i=0;i<comm->total_count;i++) {
      free(comm->comm_name[i]);
    }
    free(comm);
  }
}

int is_comm_splitted(comm_dump_t* comm, int trace_index,int comm_out)
{
  int i=0;
  if(comm == NULL) {
    return 0;
  }
  if(comm!=NULL && comm->total_count>0) {
    for(i=0;i<comm->total_count;i++) {
      if(trace_index == comm->comm_trace_index[i]) {
        if(comm->split_tracker_flag[i]!=0)
          return comm->split_tracker_flag[i];
        else
        { 
          comm->split_tracker_flag[i]=comm_out;
          return 0;
        }
      }
    }
  }

}


//extern int comm_total;
//request_node_t * reqhead[comm_total]=NULL;
/*
 *ToDo:Removed extern for following variable
 *     as op_generator is removed temporarily
 */
extern request_node_t ** reqhead;

int add_request(int i) {
	request_node_t * new_node;
	request_node_t *curr;
	request_node_t *prev;
	int temp = 9999;
	new_node = (request_node_t *) malloc(sizeof(struct request_node));
	if(reqhead[i]==NULL){
		new_node->request_handle = 9999;
		new_node->request_position = 0;
		new_node->next = NULL;
		reqhead[i] = new_node;  
		return new_node->request_position; 
	} else {
		curr = reqhead[i];
		prev = NULL;
		do{
			if(curr->request_handle >= temp){ 
				temp--;
				prev=curr;
				if(curr->next!=NULL)
					curr=curr->next;
				else
					curr=NULL;
			}
			else
				break;
		} while(curr!=NULL);
		if(prev==NULL){ //add first node
			new_node->request_handle = temp;
			new_node->request_position = 9999 - temp;
			new_node->next = curr;
			reqhead[i] = new_node;
			return new_node->request_position; 
		} else if(curr==NULL){  //add last node
			new_node->request_handle = temp;
			new_node->request_position = 9999 - temp;
			new_node->next = NULL;
			prev->next = new_node;
			return new_node->request_position; 
		} else {                //add middle node
			new_node->request_handle = temp;
			new_node->request_position = 9999 - temp;
			new_node->next = curr;
			prev->next = new_node;
			return new_node->request_position; 
		}
	}
}

void remove_request(int handle,int i)
{
  request_node_t *curr;
  request_node_t *prev;
  if(reqhead[i]==NULL)
    return;
  curr = reqhead[i];
  prev = curr;
  do
  {
     if(curr->request_handle == handle)
     {
         if(curr == reqhead[i])                    //delete first node
         {
            if(curr->next == NULL)
                reqhead[i]=NULL;
            else
              reqhead[i] = curr->next;
           free(curr);
           return;
         }
         else if(curr->next == NULL)             //last node
         {
           prev->next=NULL;
           free(curr);
           return;
         }
         else
         {
             prev->next = curr->next;
             free(curr);
         }
     }
     prev = curr;
     if(curr->next == NULL)
         curr= NULL;
     else
        curr = curr->next;
  }while(curr!=NULL);
}

//Vivek - End

/*malloc_dump_t* init_malloc_dump() {
  malloc_dump_t* dump = NULL;
  dump = (malloc_dump_t*)malloc(sizeof(malloc_dump_t));
  dump->total_count = 0;
  return dump;
}

int add_malloc_dump(malloc_dump_t* dump, char* detail, int seq_num) {
  int success = 1;
  if(dump == NULL) {
    dump = init_malloc_dump();
  }
  if(dump != NULL && strlen(detail)>0 && var->total_count < (VAR_LIST_SIZE-1)) {
    var->var_detail[var->total_count] = (char*)malloc((strlen(detail)+1)*sizeof(char));
    strcpy(var->var_detail[var->total_count], detail);
    (var->total_count)++;
    success = 0;
  }
  return success;
}

void free_malloc_dump(malloc_dump_t* dump) {
  int i = 0;
  if(var != NULL && var->total_count > 0) {
    for(i=0;i<var->total_count;i++) {
      free(var->var_detail[i]);
    }
    free(var);
  }
}*/

