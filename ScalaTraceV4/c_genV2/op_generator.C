#include "op_generator.h"
#include "PtrHandler.h"
#include "Event.h"
#include "umpi_mpi_ops.h"
#include "ReqHandler.h"

#include "generate_utils.h"
#include "Timer.h"
#include "code_generator_handler.h"
//#include "op_exec.C"
//#include "mem_handler.h"

//#define DEBUG

/*
 * TODO: Future work
 * Loop need to be unrolled, if an event has different values based on rank or 
 * loop iteration for parameters that have symbolic names like MPI_Op, Datatype.
 * 
 * Disabled this feature as all changes are not completed. Currently handled 
 * only for loops having single entry.
 * */
//#define UNROLL_LOOP
#define PRIO_DEBUG

#define INLINE
/*
 * TODO: Looks like LOCAL/GLOBAL are not being used properly.
 * Only dumping "LOCAL" in main.h as global variables.
 * Remove GLOBAL use if it won't being used at all.
 * */
#define LOCAL 0
#define GLOBAL 1
#define DEFINED 0
#define DERIVED 1

#define RL_SEPARATOR ','
#define PARAM_ENTRY_SEPARATOR ';'

#define MAX_TYPE_NAME_LENGTH 100
#define MAX_OP_NAME_LENGTH 100

extern generator_context_t* h_gen_context;

int RECORDED_MPI_REQUEST_NULL;
ptr_handler_t replay_ptr;

req_handler_t replay_req;

extern var_dump_t * var;
extern derived_type_dump_t * type;
extern comm_dump_t * comm_dump;
int new_type_tracker;
extern FILE* skel_type_fh;
FILE* skel_rank_fh;
FILE* skel_alltoallv_fh;
extern file_handle_dump_t* fh_dump;
int comm_total;

int req_count;                              //Vivek Added for MPI_Waitall.  Get rid once *req_cnt is functional.
int *req_cnt;                               //Per process count of requests. Reset at Waitall.
int req_pos;
request_node_t **reqhead;
int event_count;
int iter_cond_val;
int iter_cond_val1;
int iter_cond_flag_in_wait;
int *iter_cond;                             //Array to hold values for Iteration condition in conditional Isend, Irecv 
int *iter_cond1;                             //Array to hold values for Iteration condition in conditional Isend, Irecv 
int *rank_array;                            //For a particular event, this is the array of ranks taking part in the event.
int wait_count;                             //Vivek Added to count the count in Waitall.
int max_wait_count;                         //Vivek Added to get the maximum size of request array to be declared in skeleton header.
int max_offset;
//char req_array_size_buffer[33];             //String to declare the maximum size of request array
int comm_trace_index;
int count_dump_flag;
int alltoallv_count;

extern int use_comm_total_flag;

/*Utility functions to generate events*/
void generate_init_code(Event* event);
void generate_send_code(Event* event);
void generate_recv_code(Event* event);
void generate_wait_code(Event* event);
void generate_waitall_code(Event* event);
void generate_alltoall_code(Event* event);
void generate_alltoallV_code(Event* event);
void generate_reduce_code(Event* event, int depth);
void generate_bcast_code(Event* event, int depth);
void generate_comm_split_code(Event* event);
void generate_mpi_open_code(Event* event);
void generate_mpi_read_write_code(Event* event);
void generate_mpi_close(Event* event);
void generate_mpi_seek_code(Event* event);
void generate_mpi_read_write_at(Event* event);
void generate_mpi_read_write_end(Event* event);
void generate_mpi_delete(Event* event);
void generate_mpi_type_contiguous(Event* event);
void generate_mpi_type_commit(Event* event);
void generate_mpi_type_free(Event* event);
void generate_posix_open(Event* event);
void generate_posix_close(Event* event);
void generate_posix_read_write(Event* event);
void generate_posix_seek(Event* event);
void generate_fopen(Event* event);
void generate_fread_fwrite(Event* event);
void generate_fclose(Event* event);
void generate_remove(Event* event);

void populate_typestring(int type_idx, char* type_name){
  MPI_Datatype datatype;
  int i;

  datatype = index_to_type(replay_ptr, type_idx);
  i = 0;
  //Check if the datatype is part of defined datatype
  while(MPI_DEFINED_DATATYPES[i].type != MPI_DATATYPE_NULL) {
    if(MPI_DEFINED_DATATYPES[i].type == datatype) {
      strcpy(type_name, MPI_DEFINED_DATATYPES[i].type_name);
      break;
    }
    i++;
  }

  //assert(strlen(type_name) > 0);
}

/*
 * populate_recvtype - constructs recvtype string
 * Currently not handling rank/iteration specific behavior.
 */
void populate_recvtype(Event *event, char* type_name){
  int type_idx = 0;
  type_idx = event->getCurrentValue(RECVTYPE);
  populate_typestring(type_idx, type_name);
}

/*
 * populate_datatype - constructs datatype string
 * Currently not handling rank/iteration specific behavior.
 */
void populate_datatype(Event *event, char* type_name){
  int type_idx = 0;
  type_idx = event->getCurrentValue(DATATYPE);
  populate_typestring(type_idx, type_name);
}

int get_num_digits(int num){
  int cnt = 0;
  while(num > 0){
    num = num / 10;
    cnt++;
  }
  return cnt;
}

/*
 *Utility function to construct parameter value string with pairs 
 *<values vector, ranklist>.
 *Note: It is responsibility of the caller to free up the returned string.
 *Parameters:event_param_Values: Vector of ParamValue objects
 *                          buf:
 *        gen_rl_cond_everytime: generates ranklist condition even though
 *                               there is only one value. 
 *Return: Value string
 */
char* construct_value_string(vector<ValueSet *>* event_param_values,
                                          bool gen_rl_cond_everytime = false){
  /*
   * Value String Format:
   * ("Number of Values" "Values","Ranklist";)*
   * */
  char* val_str = NULL;
  int val_str_len = 0;

  for(int itr = 0; itr < event_param_values->size(); itr++){
    ParamValue* param_value = (ParamValue*) (*event_param_values)[itr];

    //for RL and PARAM Separators
    val_str_len += 4;
    val_str_len += param_value->valuesToString().length();
    val_str_len += get_num_digits(param_value->getNumValues());
    if(param_value->getRanklist()){
      val_str_len += param_value->getRanklist()->toString().length();
    }
 
  }
  //for terminating character
  val_str_len++;
  //val_str_len = val_str_len*2; //added by xiaoqing

  val_str = (char*) malloc(sizeof(char) * val_str_len);
  assert(val_str != NULL);
  memset(val_str, 0, val_str_len);

  char* pos = val_str;
  for(int itr = 0; itr < event_param_values->size(); itr++){
    ParamValue* param_value = (ParamValue*) (*event_param_values)[itr];

    pos += sprintf(pos, "%d %s", param_value->getNumValues(), 
                                  param_value->valuesToString().c_str());
    if(*(pos-1)== ' '){
      //valueToString adds extra space at end. hence removing it
      pos--;
    }
    if(param_value->getRanklist() &&
        comm_total > param_value->getRanklist()->getNumRanks() &&
        (event_param_values->size() > 1 || gen_rl_cond_everytime)){
      pos += sprintf(pos, "%c", RL_SEPARATOR);
      pos += sprintf(pos, "%s", param_value->getRanklist()->toString().c_str());
    }
    pos += sprintf(pos, "%c", PARAM_ENTRY_SEPARATOR);
  }

  return val_str;
}

/*
 *Utility function to construct parameter value string.
 *Note: It is responsibility of the caller to free up the returned string.
 *Parameters:     event:
 *            paramtype:
 *Return: Value string of parameter
 */
char* construct_param_string(Event* event, int paramtype)
{
  map<int, Param*>* params;
  Param* event_param;
  vector<ValueSet *>* event_param_values;
  
  //getting the params map of event
  params = event->getParams();
  
  //get the param of paramtype
  event_param = params->find(paramtype)->second;
  event_param_values = event_param->getParamValues();
  
  return construct_value_string(event_param_values); //add paramtyp
}

/*
 *Generates the MPI_Op string based on the index given
 *Parameters: op_index[in]: MPI_OP parameter in trace
 *            op_name[out]: op_name will be written into it
 */
void generate_opname(int op_index, char* opname)
{
  MPI_Op mpi_op = index_to_op(replay_ptr, op_index);
  int i = 0;
  while(MPI_DEFINED_OPS[i].op != MPI_OP_NULL)
  {
    if(MPI_DEFINED_OPS[i].op == mpi_op)
    {
        strcpy(opname, MPI_DEFINED_OPS[i].op_name);
        break;
    }
    i++;
  }
}

/*
 *Generates conditional to handle rank specific behavior of loops.
 * Decrementing the indent and closing the brace are the
 * responsibilities of the caller.
 * Parameters:  Event*: pointer to event
 * */
void generate_loop_mem_cond(Event* event){
  int loop_depth = h_gen_context->nest_loops_depth;
  FILE* skel_fh = h_gen_context->skel_fh;

  printindent(skel_fh);
  fprintf(skel_fh, "if(is_loop_member(myrank, \"%s\", %d)){\n", 
                  event->getRanklist()->toString().c_str(), loop_depth);
  //incrementing indent
  indent++;
}

/*
 * Decodes and generates code for the operation represented by the event.
 * Parameters:  Event*: pointer to event
 * 		 depth: depth of loophead
 */
void generate_event_code(Event *event, int depth)
{
  FILE* skel_fh = h_gen_context->skel_fh;
  int loop_depth = h_gen_context->nest_loops_depth;
  int is_cond_gen = 0;

  if(loop_depth > 0){
    //conditional check to handle rank specific behavior of loops
    generate_loop_mem_cond(event);
    is_cond_gen = 1;
  }else if( comm_total >  event->getRanklist()->getNumRanks() ){
    //add conditional check if event doesn't run on all nodes
    generate_ranklist_cond(event->getRanklist(), false);
    is_cond_gen = 1;
  }

  if( event->getOp() != umpi_MPI_Init )
  {
    generate_compute_time(skel_fh, event);
  }
 
  /*Get the op*/ 
  switch(event->getOp())
  {
    case umpi_MPI_Init:
      generate_init_code(event);
      break;

    case umpi_MPI_Finalize:
      finalize_ptr (replay_ptr);
      printindent(skel_fh);
      fprintf(skel_fh, "gettimeofday(&tv1, NULL);\n");
      printindent(skel_fh);
      fprintf(skel_fh, "MPI_Finalize();\n");
      break;

    case umpi_MPI_Recv:
    case umpi_MPI_Irecv:
      generate_recv_code(event);
      break;

    case umpi_MPI_Send:
    case umpi_MPI_Isend:
      generate_send_code(event);
      break;

    case umpi_MPI_Wait:
      generate_wait_code(event);
      break;

    case umpi_MPI_Waitall:
      generate_waitall_code(event);      
      break;

    case umpi_MPI_Barrier:
      printindent(skel_fh);
      fprintf(skel_fh, "MPI_Barrier(get_comm(&communicators, %d));\n", event->getCurrentValue(COMM));
      break;
   
    case umpi_MPI_Bcast:
      generate_bcast_code(event, depth);      
      break;

    case umpi_MPI_Reduce:
    case umpi_MPI_Allreduce:
      generate_reduce_code(event, depth);
      break;

    case umpi_MPI_Alltoall:
      generate_alltoall_code(event);
      break;

    case umpi_MPI_Alltoallv:
      generate_alltoallV_code(event);
      break;

    case umpi_MPI_Comm_split:
      generate_comm_split_code(event);
      break;

    case umpi_MPI_Comm_dup:
      printindent(skel_fh);
      fprintf(skel_fh, "MPI_Comm_dup(get_comm(&communicators, %d), get_next_comm(&communicators));\n", event->getCurrentValue(COMM));
      break;

    case umpi_MPI_Comm_free:
      printindent(skel_fh);
      fprintf(skel_fh, "MPI_Comm_free(get_comm_pointer(&communicators, %d));\n", event->getCurrentValue(COMM));
      break;
    case umpi_MPI_File_open:   //added by xiaoqing
      generate_mpi_open_code(event);
      break;
    case umpi_MPI_File_close:
      generate_mpi_close(event);
      break;
    case umpi_MPI_File_read:
    case umpi_MPI_File_write:
    case umpi_MPI_File_read_shared:
    case umpi_MPI_File_write_shared:
    case umpi_MPI_File_read_all:
    case umpi_MPI_File_write_all:
    case umpi_MPI_File_read_ordered:
    case umpi_MPI_File_write_ordered:
    case umpi_MPI_File_read_all_begin:
    case umpi_MPI_File_write_all_begin:
    case umpi_MPI_File_read_ordered_begin:
    case umpi_MPI_File_write_ordered_begin:
    case umpi_MPI_File_iread:
    case umpi_MPI_File_iwrite:
    case umpi_MPI_File_iread_shared:
    case umpi_MPI_File_iwrite_shared:
      generate_mpi_read_write_code(event);
      break;
    case umpi_MPI_File_read_at:
    case umpi_MPI_File_write_at:
    case umpi_MPI_File_read_at_all:
    case umpi_MPI_File_write_at_all:
    case umpi_MPI_File_read_at_all_begin:
    case umpi_MPI_File_write_at_all_begin:
    case umpi_MPI_File_iread_at:
    case umpi_MPI_File_iwrite_at:
      generate_mpi_read_write_at(event);
      break;
    case umpi_MPI_File_read_all_end:
    case umpi_MPI_File_write_all_end:
    case umpi_MPI_File_read_ordered_end:
    case umpi_MPI_File_write_ordered_end:
    case umpi_MPI_File_read_at_all_end:
    case umpi_MPI_File_write_at_all_end:
      generate_mpi_read_write_end(event);
      break;
    case umpi_MPI_File_delete:
      generate_mpi_delete(event);
      break;
    case umpi_MPI_File_seek:
      generate_mpi_seek_code(event);
      break;
    case umpi_MPI_Type_contiguous:
      generate_mpi_type_contiguous(event);
      break;
    case umpi_MPI_Type_commit:
      generate_mpi_type_commit(event);
      break;
    case umpi_MPI_Type_free:
      generate_mpi_type_free(event);
      break;
    case umpi_open:
    case umpi_open64:
      generate_posix_open(event);
      break;
    case umpi_close:
      generate_posix_close(event);
      break;
    case umpi_write:
    case umpi_read:
      generate_posix_read_write(event);
      break;
    case umpi_lseek:
      generate_posix_seek(event);
      break;
    case umpi_fopen:
      generate_fopen(event);
      break;
    case umpi_fread:
    case umpi_fwrite:
      generate_fread_fwrite(event);
      break;
    case umpi_fclose:
      generate_fclose(event);
      break;
    case umpi_remove:
      generate_remove(event);
      break;
    default:
      fprintf(stderr, "\nERROR: Not supported event %d \n\n", event->getOp());
      assert(0);
  }

  //Closing conditional block
  if(is_cond_gen)
  {
    indent--;
    printindent(skel_fh);
    fprintf(skel_fh, "}\n");
  }else{

    //adding extra line after generating code for an event
    fprintf(skel_fh, "\n");
  }
}

void generate_init_code(Event* event){
  int i = 0;
  FILE* skel_fh;

  skel_fh = h_gen_context->skel_fh;

  RECORDED_MPI_REQUEST_NULL = -1;
  //init_req(&replay_req, REQ_LIST_SIZE);
  //init_mem(REQ_LIST_SIZE);
  replay_ptr = init_ptr (PTR_HANDLER_SZ);

  //storing the size of communicator
  comm_total = event->getRanklist()->getNumRanks();

  req_count = 0;                        //Vivek: Initialize the count to zero
  req_cnt = (int*)malloc(comm_total*sizeof(int));
  iter_cond = (int*)malloc(comm_total*sizeof(int));
  iter_cond1 = (int*)malloc(comm_total*sizeof(int));
  reqhead = (request_node_t**)malloc(comm_total*sizeof(request_node_t*)); 
  for(i=0;i<comm_total;i++)
  {                                           
    req_cnt[i]=0;
    iter_cond[i]=0;
    iter_cond1[i]=0;
    reqhead[i]=NULL;
  }
  wait_count = 0;                       //Vivek: initialize to zero.
  max_wait_count = 0;                   //Vivek: initialize to zero.
  event_count = 0;                     //Vivek: initialize to zero.
  iter_cond_flag_in_wait = 0;
  count_dump_flag = 0;
  alltoallv_count = 0;
  comm_trace_index = PTR_HANDLER_SZ;    //Initialize comm_trace_index to PTR_HANDLER_SZ = 1000 declared in ptr_handler.h
  add_var_dump(var,"int recv_count = 0;",LOCAL);
  add_var_dump(var,"int send_count = 0;",LOCAL);
/*char skel_rank_file[4096];
sprintf(skel_rank_file, "%s/skel_rank", target_dir);
skel_rank_fh=fopen(skel_rank_file,"w");*/
  printindent(skel_fh);
  //writing MPI_Init to file
  fprintf(skel_fh,"MPI_Init(&argc,&argv);\n");
  printindent(skel_fh);
  fprintf(skel_fh,"IoTimeStart();\n");
  if(add_var_dump(var, "int myrank;", GLOBAL)) 
  {
    fprintf(stderr,"Write buffer to hold variables is possibly full.");
    exit(0);
  }
  
  printindent(skel_fh);
  fprintf(skel_fh,"MPI_Comm_rank(MPI_COMM_WORLD, &myrank);\n");

  if(add_var_dump(var, "int comm_size;", GLOBAL)) 
  {
    fprintf(stderr,"Write buffer to hold variables is possibly full.");
    exit(0);
  }
  printindent(skel_fh);
  fprintf(skel_fh,"MPI_Comm_size(MPI_COMM_WORLD, &comm_size);\n");
  printindent(skel_fh);
  fprintf(skel_fh, "gettimeofday(&tv, NULL);\n");
  printindent(skel_fh);
  fprintf(skel_fh, "buffersize = get_max_size();\n");
  printindent(skel_fh);
  fprintf(skel_fh, "if(buffersize > 0) {\n");
  indent++;
  printindent(skel_fh);
  fprintf(skel_fh, "buffer = malloc(buffersize);\n");
  printindent(skel_fh);
  fprintf(skel_fh, "buffer2 = malloc(buffersize);\n");
  indent--;
  printindent(skel_fh);
  fprintf(skel_fh, "}\n");
}

void generate_send_code(Event* event){
  char type_name[MAX_TYPE_NAME_LENGTH];
  FILE* skel_fh;

  skel_fh = h_gen_context->skel_fh;
#ifdef DEBUG
  // fprintf(stdout,"DEBUG:op_generator:MPI_Send \n");
#endif
  populate_datatype(event, type_name);
  
  char* count_string = construct_param_string(event, COUNT);
  char* peer_string = construct_param_string(event, DEST);

  printindent(skel_fh);
  if(event->getOp() == umpi_MPI_Send){
    fprintf(skel_fh, "do_send(buffer, %s, \"%s\", \"%s\", %d, get_comm(&communicators, %d), myrank, comm_size);\n",
      type_name, count_string, peer_string, event->getCurrentValue(TAG), 
      event->getCurrentValue(COMM));
  }else if(event->getOp() == umpi_MPI_Isend){
    fprintf(skel_fh, "do_isend(buffer, %s, \"%s\", \"%s\", %d, get_comm(&communicators, %d), req, myrank, comm_size);\n",
      type_name, count_string, peer_string, event->getCurrentValue(TAG), 
      event->getCurrentValue(COMM));
  }
  
  free(count_string);
  free(peer_string);
}

void generate_recv_code(Event* event){
  char type_name[MAX_TYPE_NAME_LENGTH];
  FILE* skel_fh;

  char *recv_type_string; //indicates recv/irecv string
  char *recv_status_string; //wait incase of irecv else status
 
  skel_fh = h_gen_context->skel_fh;
  if(umpi_MPI_Recv == event->getOp())
  {
    recv_type_string = "do_recv";
    recv_status_string = "&status";
  }
  else
  {
    recv_type_string = "do_irecv";
    recv_status_string = "req";
  }
#ifdef DEBUG
  //fprintf(stdout,"DEBUG:op_generator:MPI_Irecv \n");
#endif
  populate_datatype(event, type_name);

  char* count_string = construct_param_string(event, COUNT);
  char* peer_string = construct_param_string(event, SOURCE);

  printindent(skel_fh);
  fprintf(skel_fh, "%s(buffer2, %s, \"%s\", \"%s\", %d, get_comm(&communicators, %d), %s, myrank, comm_size);\n", 
      recv_type_string, type_name, count_string, peer_string,
      event->getCurrentValue(TAG), event->getCurrentValue(COMM), recv_status_string);

  free(count_string);
  free(peer_string);
}

void generate_wait_code(Event* event){
  FILE* skel_fh = h_gen_context->skel_fh;
  char* wait_string = construct_param_string(event, REQUEST);
  
  printindent(skel_fh);
  fprintf(skel_fh, "do_wait(\"%s\", req, myrank);\n", wait_string);

  free(wait_string);
}

void generate_waitall_code(Event* event){
  FILE* skel_fh = h_gen_context->skel_fh;

  /*
   * TODO: Future Work: Currently not supporting iteration specific behavior.
   * Need to pass count and wait list need to be parsed based on it.
   * Currently handling only rank specific behavior.
   * */
  char* wait_string = construct_param_string(event, ARRAY_OF_REQUESTS);

  printindent(skel_fh);
  fprintf(skel_fh, "do_waitall(\"%s\", req, myrank);\n", wait_string);

  free(wait_string);
}

/*
 * Replaces MPI_AlltoAllV with MPI_AlltoAll
 */
void generate_alltoallV_code(Event* event){
  FILE* skel_fh;
  char recvname[MAX_TYPE_NAME_LENGTH];
  char type_name[MAX_TYPE_NAME_LENGTH];

  double average_count;

  memset(type_name, 0, MAX_TYPE_NAME_LENGTH);
  memset(recvname, 0, MAX_TYPE_NAME_LENGTH);

  skel_fh = h_gen_context->skel_fh;

  populate_datatype(event, type_name);

  map<int, Param*>* params;
  Param* event_param;
  vector<ValueSet *>* event_param_values;

  //getting the params map of event
  params = event->getParams();

  //calculate the average send count
  //get the param of counts
  event_param = params->find(COUNTS)->second;
  event_param_values = event_param->getParamValues();

  for( int param_idx = 0; param_idx < event_param_values->size(); param_idx++ )
  {
    ParamValue* param_value = (ParamValue*)event_param_values->at(param_idx);
    vector<int>* values = param_value->getValues();
  
    double temp_count = 0;
    for( int val_idx = 0; val_idx < values->size(); val_idx++ )
    {
      temp_count += values->at(val_idx);
    }
    temp_count /= (double)values->size();
    average_count += temp_count; 
  }

  if( event_param_values->size() > 0 )
  {
    average_count /=  (double)event_param_values->size();
  }

  //capturing count,datatypes of receiver
  populate_recvtype(event, recvname);

  double average_count_recv = 0;

  //calculate the average recv count
  //get the param of recv counts
  event_param = params->find(RECVCOUNTS)->second;
  event_param_values = event_param->getParamValues();

  for( int param_idx = 0; param_idx < event_param_values->size(); param_idx++ )
  {
    ParamValue* param_value = (ParamValue*)event_param_values->at(param_idx);
    vector<int>* values = param_value->getValues();
  
    double temp_count = 0;
    for( int val_idx = 0; val_idx < values->size(); val_idx++ )
    {
      temp_count += values->at(val_idx);
    }
    temp_count /= (double)values->size();
    average_count_recv += temp_count; 
  }

  if( event_param_values->size() > 0 )
  {
    average_count_recv /=  (double)event_param_values->size();
  }

  printindent(skel_fh);
  //fprintf(skel_fh, "MPI_Alltoall(buffer, %d, %s, buffer2, %d, %s, get_comm(&communicators, %d) );\n",
  //      (int) average_count, type_name, (int) average_count_recv, recvname, event->getCurrentValue(COMM));

  //observing some difference with average count and average recv count, hence using same value.
  //TODO: need to find out why that difference is coming.
  fprintf(skel_fh, "MPI_Alltoall(buffer, %d, %s, buffer2, %d, %s, get_comm(&communicators, %d) );\n",
          (int) average_count, type_name, (int) average_count, recvname, event->getCurrentValue(COMM));
}

void generate_alltoall_code(Event* event){
  FILE* skel_fh;
  MPI_Datatype recvtype = 0;

  char recvname[MAX_TYPE_NAME_LENGTH];
  char type_name[MAX_TYPE_NAME_LENGTH];

  memset(type_name, 0, MAX_TYPE_NAME_LENGTH);
  memset(recvname, 0, MAX_TYPE_NAME_LENGTH);

  skel_fh = h_gen_context->skel_fh;

  populate_datatype(event, type_name);
  populate_recvtype(event, recvname);

  printindent(skel_fh);
  fprintf(skel_fh, "MPI_Alltoall(buffer, %d, %s, buffer2, %d, %s, get_comm(&communicators, %d) );\n",
        event->getCurrentValue(COUNT), type_name, event->getCurrentValue(RECVCOUNT), recvname, event->getCurrentValue(COMM));
}

void generate_reduce_code(Event* event, int depth){
  char type_name[MAX_TYPE_NAME_LENGTH];
  char opname[MAX_OP_NAME_LENGTH];
  char* count_string;
  FILE* skel_fh;

  skel_fh = h_gen_context->skel_fh;

#ifndef UNROLL_LOOP 
  populate_datatype(event, type_name);

  count_string = construct_param_string(event, COUNT);
  generate_opname(event->getCurrentValue(MPI_OP), opname);

  printindent(skel_fh);
  if(event->getOp() == umpi_MPI_Reduce)
  {
    fprintf(skel_fh, "do_reduce(buffer2,buffer, \"%s\", %s, %s, %d, get_comm(&communicators, %d), myrank);\n", 
        count_string, type_name, opname, event->getCurrentValue(RT), 
        event->getCurrentValue(COMM));
  }
  else if(event->getOp() == umpi_MPI_Allreduce)
  {
    fprintf(skel_fh, "do_allreduce(buffer, buffer2, \"%s\", %s, %s, get_comm(&communicators, %d), myrank);\n",
        count_string, type_name, opname, event->getCurrentValue(COMM));
  }

  free(count_string);

#else /*UNROLL_LOOP: currently this feature is disabled*/
  Loop* loop_stack;
  loop_stack = event->getLoopStack();
  if( loop_stack->getDepth() > depth && !is_generate_loop(event, depth))
  {
    //Loop parameters differ in parameters that have symbolic representation. Hence unrolling the loop.
    //getting the length of the loop
    vector<ValueSet*>* iters = loop_stack->getLoopAt(depth)->iter->getParamValues();
    int loop_length = dynamic_cast<ParamValue*>(iters->at(0))->getCurrentValue(true);

    /*TODO: Handle iteration specific behavior*/
    int ranklist_idx = 0;

    int cnt_count;
    int *cnt_values;
    event->getParamValues(COUNT, ranklist_idx, &cnt_count, &cnt_values);

    int op_count;
    int *op_values;
    event->getParamValues(MPI_OP, ranklist_idx, &op_count, &op_values);
    
    int index = 0;
    for(index = 0; index < loop_length; index++)
    {
      if(index > 0)
      {
        //adding line between events
        fprintf(skel_fh, "\n");
        //generating the compute time, first event timestamp is added at the begining itself
        generate_compute_time(skel_fh, event);
      }
      //generating type name
      populate_datatype(type, event, type_name, index);

      //generating count string
      int count = (cnt_count > 1) ? cnt_values[index] : cnt_values[0]; 
      sprintf(count_string,"%d %d;", 1, count);
       
      //generating opname
      int op_val = (op_count > 1) ? op_values[index] : op_values[0]; 
      generate_opname(op_val, opname);
      printindent(skel_fh);
      if(event->getOp() == umpi_MPI_Reduce)
      {
        fprintf(skel_fh, "do_reduce(buffer2,buffer, \"%s\", %s, %s, %d, get_comm(&communicators, %d), myrank);\n",
            count_string, type_name, opname, event->getCurrentValue(RT), 
            event->getCurrentValue(COMM));
      }
      else if(event->getOp() == umpi_MPI_Allreduce)
      {
        fprintf(skel_fh, "do_allreduce(buffer, buffer2, \"%s\", %s, %s, get_comm(&communicators, %d), myrank);\n",
            count_string, type_name, opname, event->getCurrentValue(COMM));
      }
    }      
  }
  else
  {
    /*No loop*/
    map<int, Param*>* event_params;
    Param* op_param;
    Param* count_param;
    vector<ValueSet *>* op_param_values;
    vector<ValueSet *>* count_param_values;

    event_params = event->getParams();
    op_param = event_params->find(MPI_OP)->second;
    op_param_values = op_param->getParamValues();

    count_param = event_params->find(COUNT)->second;
    count_param_values = count_param->getParamValues();

    /*TODO: Handle when only count param values grater than one. Consider using parse_once in this case.*/

    if(op_param_values->size() > 1)
    {
       /*Different ranklists have different MPI_OP values, hence generating multiple events*/
      int index = 0;
      for(index = 0; index < op_param_values->size(); index++)
      {
        int op_val;
        ParamValue* op_param_val;
        ParamValue* count_param_val;         

        populate_datatype(type, event, type_name);

        //construct_param_string(event, COUNT, count_string);
        if(count_param_values->size() > 1)
        {           
          /*TODO: What if count param ranklists are different from mpi_op params?*/ 
          count_param_val = (ParamValue*)(count_param_values->at(index));
        }
        else
        {
          count_param_val = (ParamValue*)(count_param_values->at(0));
        } 
        sprintf(count_string,"%d %d;", 1, count_param_val->getValues()->at(0));

        op_param_val = (ParamValue*)(op_param_values->at(index));
        if(op_param_val->getValues()->size() > 1)
        {
          fprintf(stderr, "ERROR:Op_generator MPI_Op param size - expected:1, actual %d\n", op_param_val->getValues()->size());
        }
        op_val = op_param_val->getValues()->at(0);  
        generate_opname(op_val, opname);

        //generating rank list condition code
        generate_ranklist_cond(op_param_val->getRanklist(), false);
        //generating event code
        printindent(skel_fh);
        if(event->getOp() == umpi_MPI_Reduce)
        {
          fprintf(skel_fh, "do_reduce(buffer2,buffer, \"%s\", %s, %s, %d, get_comm(&communicators, %d), myrank);\n", 
              count_string, type_name, opname, event->getCurrentValue(RT),
              event->getCurrentValue(COMM));
        }
        else if(event->getOp() == umpi_MPI_Allreduce)
        {
          fprintf(skel_fh, "do_allreduce(buffer, buffer2, \"%s\", %s, %s, get_comm(&communicators, %d), myrank);\n",
              count_string, type_name, opname, event->getCurrentValue(COMM));
        }
      
        //generating end of 'if' code
        indent--;
        printindent(skel_fh);
        fprintf(skel_fh, "}\n");
      }
    }
    else
    {
      populate_datatype(type, event, type_name);

      count_string = construct_param_string(event, COUNT);
      generate_opname( event->getCurrentValue(MPI_OP),opname);

      printindent(skel_fh);
      if(event->getOp() == umpi_MPI_Reduce)
      {
        fprintf(skel_fh, "do_reduce(buffer2,buffer, \"%s\", %s, %s, %d, get_comm(&communicators, %d), myrank);\n",
            count_string, type_name, opname, event->getCurrentValue(RT), 
            event->getCurrentValue(COMM));
      }
      else if(event->getOp() == umpi_MPI_Allreduce)
      {
        fprintf(skel_fh, "do_allreduce(buffer, buffer2, \"%s\", %s, %s, get_comm(&communicators, %d), myrank);\n",
            count_string, type_name, opname, event->getCurrentValue(COMM));
      }
      free(count_string);
    }
  }
#endif /*UNROLL_LOOP*/ 
}

void generate_bcast_code(Event* event, int depth){
  char type_name[MAX_TYPE_NAME_LENGTH];
  char* count_string;
  FILE* skel_fh;

  skel_fh = h_gen_context->skel_fh;

#ifndef UNROLL_LOOP 
  populate_datatype(event, type_name);
  count_string = construct_param_string(event, COUNT);
  printindent(skel_fh);
  fprintf(skel_fh, "do_bcast(buffer, \"%s\", %s, %d, get_comm(&communicators, %d), myrank);\n", 
      count_string, type_name, event->getCurrentValue(RT),
      event->getCurrentValue(COMM));
  free(count_string);
#else/*UNROLL_LOOP: currently this feature is disabled */
  Loop* loop_stack;
  loop_stack = event->getLoopStack();
  if( loop_stack->getDepth() > depth && !is_generate_loop(event, depth))
  {
    /*Loop parameters are differ in parameters that have symbolic representation. Hence unrolling the loop.*/

    //getting the length of the loop
    vector<ValueSet*>* iters = loop_stack->getLoopAt(depth)->iter->getParamValues();
    int loop_length = dynamic_cast<ParamValue*>(iters->at(0))->getCurrentValue(true);

    /*TODO: Handle iteration specific behavior. This ranklist index value will change based on rank.*/
    int ranklist_idx = 0;

    int cnt_count;
    int *cnt_values;
    event->getParamValues(COUNT, ranklist_idx, &cnt_count, &cnt_values);

    int index = 0;
    for(index = 0; index < loop_length; index++)
    {
      populate_datatype(type, event, type_name, index);

      if(index > 0)
      {
        //adding line between events
        fprintf(skel_fh, "\n");
        //generating the compute time, first event timestamp is added at the begining itself
        generate_compute_time(skel_fh, event);
      }

      //generating count string
      int count = (cnt_count > 1) ? cnt_values[index] : cnt_values[0]; 
      sprintf(count_string,"%d %d;", 1, count);

      printindent(skel_fh);
      fprintf(skel_fh, "do_bcast(buffer, \"%s\", %s, %d, get_comm(&communicators, %d), myrank);\n", 
          count_string, type_name, event->getCurrentValue(RT), 
          event->getCurrentValue(COMM));
    }
  }
  else
  {
    populate_datatype(type, event, type_name);
    count_string = construct_param_string(event, COUNT);
    printindent(skel_fh);
    fprintf(skel_fh, "do_bcast(buffer, \"%s\", %s, %d, get_comm(&communicators, %d), myrank);\n",
        count_string, type_name, event->getCurrentValue(RT),
        event->getCurrentValue(COMM));
    free(count_string);
  }
#endif/*UNROLL_LOOP*/
}

void generate_comm_split_code(Event* event){
  FILE* skel_fh = h_gen_context->skel_fh;
  char* color_str = construct_param_string(event, COLOR);
  char* key_str = construct_param_string(event, KEY);

  printindent(skel_fh);
  fprintf(skel_fh, "do_comm_split(get_comm(&communicators, %d), \"%s\", \"%s\", get_next_comm(&communicators), myrank);\n",
      event->getCurrentValue(COMM), color_str, key_str);

  free(color_str);
  free(key_str);
}

void generate_mpi_seek_code(Event* event){
	FILE* skel_fh = h_gen_context->skel_fh;
	int numrank = (event->getRanklist())->getNumRanks();
	char *whence = construct_param_string(event, WHENCE);
	char *indexfh = construct_param_string(event, FH);
	char *offset = construct_param_string(event, OFFSET);

	printindent(skel_fh);
	fprintf(skel_fh, "do_mpi_File_seek(\"%s\", \"%s\", \"%s\", %d, myrank);\n", indexfh, offset, whence, numrank);
	free(whence);
	free(indexfh);
	free(offset);	
}

void generate_mpi_open_code(Event* event){
  FILE* skel_fh = h_gen_context->skel_fh;
  int numrank = (event->getRanklist())->getNumRanks();
  char* indexfh = construct_param_string(event, FH);
  char* amode = construct_param_string(event, AMODE);
  char* filename = construct_param_string(event, FILENAME);
  char* iotime = construct_param_string(event, TIME);

  printindent(skel_fh);
  fprintf(skel_fh, "do_File_open(get_comm(&communicators, %d), \"%s\", \"%s\", MPI_INFO_NULL, \"%s\", %d, myrank, \"%s\");\n",
		  event->getCurrentValue(COMM), filename, amode, indexfh, numrank, iotime);
  free(filename);
  free(amode);
  free(indexfh);
  free(iotime);
}


void generate_mpi_close(Event *event){
	FILE *skel_fh = h_gen_context->skel_fh;
	char *indexfh = construct_param_string(event, FH);
  	char* iotime = construct_param_string(event, TIME);
	int numrank = (event->getRanklist())->getNumRanks();

	printindent(skel_fh);
	fprintf(skel_fh, "do_File_close(\"%s\", myrank, \"%s\", %d);\n", indexfh, iotime, numrank);
	free(indexfh);
	free(iotime);
}

void generate_mpi_read_write_code(Event* event){
  FILE* skel_fh = h_gen_context->skel_fh;
  char type_name[MAX_TYPE_NAME_LENGTH];
  populate_datatype(event, type_name);
  
  char* count_string = construct_param_string(event, COUNT);
  char* indexfh = construct_param_string(event, FH);
  int op;
  switch(event->getOp()){
  	case umpi_MPI_File_read:
		op = 0;
		break;
	case umpi_MPI_File_write:
		op = 1;
		break;
	case umpi_MPI_File_read_shared:
		op = 2;
		break;
	case umpi_MPI_File_write_shared:
		op = 3;
		break;
	case umpi_MPI_File_read_all:
		op = 4;
		break;
	case umpi_MPI_File_write_all:
		op = 5;
		break;
	case umpi_MPI_File_read_ordered:
		op = 6;
		break;
	case umpi_MPI_File_write_ordered:
		op = 7;
		break;
	case umpi_MPI_File_read_all_begin:
		op = 8;
		break;
	case umpi_MPI_File_write_all_begin:
		op = 9;
		break;
	case umpi_MPI_File_read_ordered_begin:
		op = 10;
		break;
	case umpi_MPI_File_write_ordered_begin:
		op = 11;
		break;
	case umpi_MPI_File_iread:
		op = 12;
		break;
	case umpi_MPI_File_iwrite:
		op = 13;
		break;
	case umpi_MPI_File_iread_shared:
		op = 14;
		break;
	case umpi_MPI_File_iwrite_shared:
		op = 15;
		break;
	default:
		printf("not a read or write function");
		exit(0);
  }
  printindent(skel_fh);
  fprintf(skel_fh, "do_File_read_write(buffer, \"%s\", \"%s\", %s, %d, &status, myrank, req);\n", indexfh, count_string, type_name, op);
  free(indexfh);
  free(count_string);
}

void generate_mpi_read_write_at(Event *event){
  FILE* skel_fh = h_gen_context->skel_fh;
  char type_name[MAX_TYPE_NAME_LENGTH];
  populate_datatype(event, type_name);
  int numrank = (event->getRanklist())->getNumRanks();
  
  char* count_string = construct_param_string(event, COUNT);
  char* indexfh = construct_param_string(event, FH);
  char *offset = construct_param_string(event, OFFSET);
  char* data_type = construct_param_string(event, DATATYPE);

  int op, flag;
  switch(event->getOp()){
  	case umpi_MPI_File_read_at:
		op = 0;
		break;
	case umpi_MPI_File_write_at:
		op = 1;
		break;
	case umpi_MPI_File_read_at_all:
		op = 2;
		break;
	case umpi_MPI_File_write_at_all:
		op = 3;
		break;
	case umpi_MPI_File_read_at_all_begin:
		op = 4;
		break;
	case umpi_MPI_File_write_at_all_begin:
		op = 5;
		break;
	case umpi_MPI_File_iread_at:
		op = 6;
		break;
	case umpi_MPI_File_iwrite_at:
		op = 7;
		break;
	default:
		printf("not read_write_at function\n");
		exit(0);
  }
  printindent(skel_fh);
  if(strlen(type_name) > 0)
	  flag = 0;
  else{
	  sprintf(type_name, "NULL"); 
	  flag = 1;
  }

  fprintf(skel_fh, "do_File_read_write_at(buffer, \"%s\", \"%s\", \"%s\", \"%s\", %d, &status, myrank, req, %d, %s, %d);\n", indexfh, count_string, offset, data_type, op, numrank, type_name, flag);

  free(count_string);
  free(offset);	
  free(data_type);
   free(indexfh);
}

void generate_mpi_read_write_end(Event *event){
	FILE* skel_fh = h_gen_context->skel_fh;
	char* indexfh = construct_param_string(event, FH);

	int op;
	switch(event->getOp()){
		case umpi_MPI_File_read_all_end:
			op = 0;
			break;
    		case umpi_MPI_File_write_all_end:
			op = 1;
			break;
    		case umpi_MPI_File_read_ordered_end:
			op = 2;
			break;
    		case umpi_MPI_File_write_ordered_end:
			op = 3;
			break;
    		case umpi_MPI_File_read_at_all_end:
			op = 4;
			break;
    		case umpi_MPI_File_write_at_all_end:
			op = 5;
			break;
		default:
			printf("not a end function\n");
			exit(0);
	}
	printindent(skel_fh);
	fprintf(skel_fh, "do_File_read_write_end(buffer, \"%s\", &status, %d, myrank);\n", indexfh, op);
	free(indexfh);
}

void generate_mpi_delete(Event *event){
	FILE* skel_fh = h_gen_context->skel_fh;
  	char* filename = construct_param_string(event, FILENAME);
  	int numrank = (event->getRanklist())->getNumRanks();
	
	printindent(skel_fh);
	fprintf(skel_fh, "do_File_delete(\"%s\", myrank, %d);\n", filename, numrank);
	free(filename);
}

void generate_mpi_type_contiguous(Event *event){
	/*FILE* skel_fh = h_gen_context->skel_fh;
	char* size = construct_param_string(event, SIZE);
	char* rank = construct_param_string(event, RANK);
	char* count = construct_param_string(event, COUNT);
	char* order = construct_param_string(event, ORDER);
	char* newtype = construct_param_string(event, NEWTYPE);
  	char type_name[MAX_TYPE_NAME_LENGTH];
  	populate_datatype(event, type_name);

	char* array_of_gsizes = construct_param_string(event, ARRAY_OF_GSIZES);
	char* array_of_distribs = construct_param_string(event, ARRAY_OF_DISTRIBS);
	char* array_of_dargs = construct_param_string(event, ARRAY_OF_DARGS);
	char* array_of_psizes = construct_param_string(event, ARRAY_OF_PSIZES);

	printfindent(skel_fh);
	fprintf(skel_fh, "do_type_create_darray(\"%s\", \"%s\", \"%s\", \"%s\", \"%s\", \"%s\", \"%s\", \"%s\", %s, \"%s\");\n",size, rank, count, array_of_gsizes, array_of_distribs, array_of_dargs, array_of_psizes, order, type_name, newtype);
	free(size);
	free(rank);
	free(count);
	free(order);
	free(newtype);
	free(array_of_gsizes);
	free(array_of_distribs);
	free(array_of_dargs);
	free(array_of_psizes);*/
	FILE* skel_fh = h_gen_context->skel_fh;
	char* count = construct_param_string(event, COUNT);
  	char type_name[MAX_TYPE_NAME_LENGTH];
  	populate_datatype(event, type_name);

	char* newtype = construct_param_string(event, NEWTYPE);
	
	printindent(skel_fh);
	fprintf(skel_fh, "do_type_contiguous(\"%s\", %s, \"%s\", myrank);\n", count, type_name, newtype);
	free(count);
	free(newtype);
}

void generate_mpi_type_commit(Event* event){
	FILE* skel_fh = h_gen_context->skel_fh;
	char* newtype = construct_param_string(event, NEWTYPE);

	printindent(skel_fh);
	fprintf(skel_fh, "do_type_commit(\"%s\", myrank);\n",newtype);
	free(newtype);
}

void generate_mpi_type_free(Event* event){
	FILE* skel_fh = h_gen_context->skel_fh;
	char* newtype = construct_param_string(event, NEWTYPE);
	printindent(skel_fh);
	fprintf(skel_fh, "do_type_commit(\"%s\", myrank);\n", newtype);
	free(newtype);	
}

void generate_posix_open(Event* event){
	FILE* skel_fh = h_gen_context->skel_fh;
  	int numrank = (event->getRanklist())->getNumRanks();
  	char* fd = construct_param_string(event, POSIX_FD);
  	char* flag = construct_param_string(event, POSIX_FLAGS);
  	char* filename = construct_param_string(event, FILENAME);
	char* iotime = construct_param_string(event, TIME);

	int op;
	if(event->getOp() == umpi_open)
		op = 0;
	else
		op = 1;

	printindent(skel_fh);
	fprintf(skel_fh, "do_posix_open(\"%s\", \"%s\", \"%s\", %d, myrank, %d, \"%s\");\n", filename, flag, fd, numrank, op, iotime);
	free(fd);
	free(flag);
	free(filename);
	free(iotime);
}

void generate_posix_close(Event* event){
	FILE* skel_fh = h_gen_context->skel_fh;
	char* fd = construct_param_string(event, POSIX_FD);
	char* iotime = construct_param_string(event, TIME);
  	int numrank = (event->getRanklist())->getNumRanks();
	
	printindent(skel_fh);
	fprintf(skel_fh, "do_posix_close(\"%s\", myrank, \"%s\", %d);\n", fd, iotime, numrank);
	
	free(fd);
	free(iotime);	
}

void generate_posix_read_write(Event* event){
	FILE* skel_fh = h_gen_context->skel_fh;
	char* fd = construct_param_string(event, POSIX_FD);
	char* count = construct_param_string(event, COUNT);
	int op;

	if(event->getOp() == umpi_read)
		op = 0;
	else
		op = 1;
	
	printindent(skel_fh);
	fprintf(skel_fh, "do_posix_read_write(\"%s\", buffer, \"%s\", myrank, %d);\n", fd, count, op);

	free(fd);
	free(count);
}

void generate_posix_seek(Event* event){
	FILE* skel_fh = h_gen_context->skel_fh;
	char* fd = construct_param_string(event, POSIX_FD);
	char* offset = construct_param_string(event, OFFSET);
	char* whence = construct_param_string(event, WHENCE);
	int numrank = (event->getRanklist())->getNumRanks();

	printindent(skel_fh);
	fprintf(skel_fh, "do_posix_seek(\"%s\", \"%s\", \"%s\", myrank, %d);\n", fd, offset, whence, numrank);

	free(fd);
	free(offset);
	free(whence);
}

void generate_fopen(Event* event){
	FILE* skel_fh = h_gen_context->skel_fh;
	char* indexfh = construct_param_string(event, POSIX_FD);
	char* filename = construct_param_string(event, FILENAME);
	char* mode = construct_param_string(event, POSIX_MODE);
	char* iotime = construct_param_string(event, TIME);
	int numrank = (event->getRanklist())->getNumRanks();

	printindent(skel_fh);
	fprintf(skel_fh, "do_fopen(\"%s\", \"%s\", \"%s\", myrank, %d, \"%s\");\n", filename, mode, indexfh, numrank, iotime);

	free(indexfh);
	free(filename);
	free(mode);
	free(iotime);
}

void generate_fread_fwrite(Event* event){
	FILE* skel_fh = h_gen_context->skel_fh;
	char* indexfh = construct_param_string(event, POSIX_FD);
	char* size = construct_param_string(event, SIZE);
	char* count = construct_param_string(event, COUNT);
	int op;
	if(event->getOp() == umpi_fread)
		op = 0;
	else
		op = 1;
	
	printindent(skel_fh);
	fprintf(skel_fh, "do_fread_fwrite(buffer, \"%s\", \"%s\", \"%s\", myrank, %d);\n",size, count, indexfh, op);

	free(indexfh);
	free(size);
	free(count);
}

void generate_fclose(Event* event){
	FILE* skel_fh = h_gen_context->skel_fh;
	char* indexfh = construct_param_string(event, POSIX_FD);
	char* iotime = construct_param_string(event, TIME);
  	int numrank = (event->getRanklist())->getNumRanks();

	printindent(skel_fh);
	fprintf(skel_fh, "do_fclose(\"%s\", myrank, \"%s\", %d);\n", indexfh, iotime, numrank);

	free(indexfh);
}

void generate_remove(Event* event){
	FILE* skel_fh = h_gen_context->skel_fh;
	char* filename = construct_param_string(event, FILENAME);
	int numrank = (event->getRanklist()) -> getNumRanks();

	printindent(skel_fh);
	fprintf(skel_fh, "do_remove(\"%s\", myrank, %d);\n", filename, numrank);

	free(filename);
}





