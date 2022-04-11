#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <string.h>
#include <unistd.h>

#include "common.h"
#include "code_generator_handler.h"
#include "op_generator.h"
#include "generate_utils.h"
#include "umpi_mpi_ops.h"
#include "Trace.h"

using namespace std;

//#define DEBUG 

/*Todo: try to move it to some header file*/
#define FILE_NAME_LENGTH 4096

generator_context_t* h_gen_context = NULL;

/*
 * Structures to hold variables, derived data types defined and communicators
 * created 
 */
var_dump_t * var;
derived_type_dump_t * type;
comm_dump_t * comm_dump;

/*definitions required for writing code to file*/
FILE* skel_type_fh;
file_handle_dump_t* fh_dump;

//global variable defined in code_generator_handle 
extern int indent;

/*
 *ToDo:Added this for compiling Trace.C
 * Need to remove this once we find the need of it
 */
int my_rank = -1;

/*Function declarations*/
void init_gen_context();
static int code_generator( Event *event, int depth, int iter);

/**
 * Visitor routine for executing replay operations.  This is called
 * by the traversal routines to translate ops to actual MPI function 
 * calls.
 */
static int code_generator( Event *event, int depth, int iter)
{
#ifdef DEBUG
  //fprintf(stdout,"DEBUG: executing Code_generator function \n");
  //fprintf(stdout,"DEBUG: Recvd event op: %d\n", event->getOp());
#endif

  //update events number and stack signature map
  if(event->getOp() == umpi_MPI_Init){

    if(!h_gen_context ||            
          !h_gen_context->event_nums){
      assert(0);
    }

    Event *itr = event; 
    while(itr != NULL){
      h_gen_context->event_nums->insert(pair<StackSig, int>
		      		(*(itr->getSignature()), itr->getId()));
      itr = itr->next;
    }
  }

  h_gen_context->event_count++;
  generate_event_code(event, depth);
}

void init_gen_context(){
  h_gen_context =  (generator_context_t*) malloc(sizeof(generator_context_t));
  assert(h_gen_context != NULL);
  memset(h_gen_context, 0, sizeof(generator_context_t));
  h_gen_context->event_nums = new multimap<StackSig, int>;
  assert(h_gen_context->event_nums != NULL);
}

int main(int argc, char *argv[])
{
  int rank = -1, len = -1, whole, frac;
  int retval = -1;
  int per_node_trace = FALSE;

  extern FILE* skel_rank_fh;
  char percent[3]="%\0";

  init_gen_context();

  //initializing the dumps that holds intermediate values generated
  var = init_var_dump();
  type = init_derived_type_dump();
  fh_dump = init_fh_dump();
  comm_dump = init_comm_dump();

  char genfile1[FILE_NAME_LENGTH];
  char genfile2[FILE_NAME_LENGTH];
  char genfile3[FILE_NAME_LENGTH];
  char genfile4[FILE_NAME_LENGTH];

  char target_dir[FILE_NAME_LENGTH];

  indent = 1;

  /* read in op trace for each node */
  char filename[FILE_NAME_LENGTH];  
  
  if(argc < 2)
  {
    fprintf(stderr, "Invalid usage. Usage is ./generator trace_file_path [target_directory]\n");
  }
 
  sprintf (filename, "%s", argv[1]);
  if(argc > 2)
    sprintf(target_dir, "%s", argv[2]);
  else
    sprintf(target_dir, "./");
  sprintf(genfile1, "%s/util.h", target_dir);
  sprintf(genfile2, "%s/util.c", target_dir); 
  sprintf(genfile3, "%s/main.h", target_dir);
  sprintf(genfile4, "%s/main.c", target_dir);
 
  //Reading the trace file
  Trace* trace = Trace::inputTrace(filename, INPUT_GLOBAL, -1);
#ifdef DEBUG
  //trace->printTrace();
#endif  
 
  /*main.c will have main function*/
  FILE* skel_fh = fopen(genfile4, "w");
  h_gen_context->skel_fh = skel_fh;

  //generating main.c header
  generate_main_c_header(skel_fh);
  
  //traverse the trace
  trace->code_gen_traverse(code_generator);  
 
  //generating closing part of main.h
  generate_main_c_end(skel_fh);
  fclose(skel_fh);

  /*Generating code for main.h*/
  skel_type_fh = fopen(genfile3, "w");
  fprintf(skel_type_fh, "#ifndef MAIN_H\n#define MAIN_H\n\n\n");
  if(var != NULL && var->total_count > 0){
    for(len = 0; len < var->total_count; len++) {
      if(var->is_global[len] == 0)
        fprintf(skel_type_fh, "%s\n",var->var_detail[len]);
    }
  }
  fprintf(skel_type_fh, "#endif\n");
  fclose(skel_type_fh);
 
  /*Generating code for util.h*/
  skel_type_fh = fopen(genfile1, "w");
  generate_util_h(skel_type_fh);
  fclose(skel_type_fh);
 
  /*generating code for util.c */
  skel_type_fh = fopen(genfile2, "w");
  generate_util_c_header(skel_type_fh);
  generate_util_compute_time_function(skel_type_fh);
  generate_max_size_function(skel_type_fh, type);
  generate_is_member_function(skel_type_fh);
  generate_req_handler_function(skel_type_fh);
  generate_comm_handler_functions(skel_type_fh);
  generate_p2p_ops(skel_type_fh);
  generate_collectives(skel_type_fh);
  generate_param_functions(skel_type_fh);
  generate_io_functions(skel_type_fh);
  fclose(skel_type_fh);

  /*freeing up the dumps created earlier*/
  free_var_dump(var);
  free_derived_type_dump(type);
  free_fh_dump(fh_dump);

  //cleanup
  if( h_gen_context != NULL ){
    if(h_gen_context->event_nums != NULL){
      delete h_gen_context->event_nums;
    }
    free(h_gen_context);
  }
  return 0;
}
/* EOF */
