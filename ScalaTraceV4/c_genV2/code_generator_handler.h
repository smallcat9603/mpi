#ifndef CODE_GENERATOR_HANDLER_H
#define CODE_GENERATOR_HANDLER_H

#define TYPE_LIST_SIZE 100
#define VAR_LIST_SIZE 500
#define COMM_LIST_SIZE 100
#define REQUEST_BUFFER_SIZE 10000 //Should match with config.C

#include <vector>
#include <map>
#include "ParamValue.h"
#include "Event.h"
#include "Stat.h"

/*
 * This struct contains an array of char pointer to hold new variables
 * that needs to be created and listed in code generation.
 * total_count will maintain the count of variables currently active in
 * the array.
 */
typedef struct var_dump {
  char* var_detail[VAR_LIST_SIZE];
  int is_global[VAR_LIST_SIZE];
  int total_count;
} var_dump_t;
  
var_dump_t* init_var_dump();
  
int add_var_dump(var_dump_t* var_dump, char* detail, int is_global);

void free_var_dump(var_dump_t* var_dump);

/*
 * This struct will hold all new datatypes created and new variable names
 * to be used in the skeleton code.
 */
typedef struct derived_type_dump {
  char* type_detail[TYPE_LIST_SIZE];
  char* type_name[TYPE_LIST_SIZE];
  int type_usage_count[TYPE_LIST_SIZE];
  int is_derived_type[TYPE_LIST_SIZE];
  int type_trace_index[TYPE_LIST_SIZE];
  int total_count;
} derived_type_dump_t;
  
derived_type_dump_t* init_derived_type_dump();

int add_derived_type(derived_type_dump_t* type, int trace_index);

int add_defined_type(derived_type_dump_t* type, char* name, int trace_index);

void update_usage_count(derived_type_dump_t* type, int usage_count, int trace_index);

char* get_type_name_from_index(derived_type_dump_t* type, int trace_index);

void free_derived_type_dump(derived_type_dump_t* type);

typedef struct file_handle_dump {
  char* fh_detail[TYPE_LIST_SIZE];
  char* fh_name[TYPE_LIST_SIZE];
  int fh_trace_index[TYPE_LIST_SIZE];
  int total_count;
} file_handle_dump_t;
  
file_handle_dump_t* init_fh_dump();

int add_file_handle(file_handle_dump_t* dump, int trace_index);

char* get_file_handle_from_index(file_handle_dump_t* dump, int trace_index);

void free_fh_dump(file_handle_dump_t* dump);

/*
 * This struct will hold all new communicators created
 * to be used in the skeleton code.
 */
//Vivek - start
typedef struct comm_dump {
  char* comm_name[COMM_LIST_SIZE];
  int comm_trace_index[COMM_LIST_SIZE];
  int split_tracker_flag[COMM_LIST_SIZE];
  int total_count;
}comm_dump_t;

comm_dump_t* init_comm_dump();

int add_comm(comm_dump_t* comm_dump, char* name ,int trace_index);

char* get_comm_name_from_index(comm_dump_t* comm, int trace_index);

void free_comm_dump(comm_dump_t* comm);

int is_comm_splitted(comm_dump_t* comm, int trace_index,int comm_out);

typedef struct request_node {
int request_handle;
int request_position;
struct request_node * next;
}request_node_t;

//request_node_t * reqhead=NULL;

int add_request(int i);

void remove_request(int handle,int i);

//Vivek - end

typedef struct generator_context{
  
  /*TODO: Move global variables like indent, "skel_fh" 
   * into this structure
   */

 int nest_loops_depth;//current event loop depth starting from 1
 int max_loops_depth;//max loop depth as of now
 int loop_count;//total number of loops encountered
 int event_count;//total number of events
 //handler to main.c
 FILE* skel_fh;

 /*
  * Map between stack signature and event number.
  * choosing multimap, as sometimes(e.g. LU) same stack signature is observed
  * for different events.   
  * */
 multimap<StackSig, int>* event_nums;
}generator_context_t;


extern int indent;
void printindent(FILE *file);

void generate_loop_start_code(Event* loophead, int loop_depth);
void generate_loop_end_code();

void generate_ranklist_cond(Ranklist * ranklist,bool is_inline);
bool is_generate_loop(Event* loophead, int depth);

void generate_compute_time(FILE *skel_fh, Event* event);
#endif
