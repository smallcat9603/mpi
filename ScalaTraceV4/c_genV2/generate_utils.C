#include "generate_utils.h"

int use_comm_total_flag = 0; //Use this flag to indicate that while generating max size function we need to use comm_total.
extern var_dump_t * var;
extern generator_context_t* h_gen_context;

/*Generates initial part of main.c*/
void generate_main_c_header(FILE * skel_fh){
  fprintf(skel_fh, "#include <stdio.h>\n");
  fprintf(skel_fh, "#include <stdlib.h>\n");
  fprintf(skel_fh, "#include <string.h>\n");
  fprintf(skel_fh, "#include <mpi.h>\n");
  fprintf(skel_fh, "#include \"main.h\"\n");
  fprintf(skel_fh, "#include \"util.h\"\n\n");
  fprintf(skel_fh, "int main(int argc, char *argv[]) {\n");
  //printindent(skel_fh);
  //fprintf(skel_fh, "DEFINE_FH\n");
  printindent(skel_fh);
  fprintf(skel_fh, "int myrank;\n");
  printindent(skel_fh);
  fprintf(skel_fh, "int comm_size;\n");
  printindent(skel_fh);
  fprintf(skel_fh, "int buffersize = 0;\n");
  printindent(skel_fh);
  /*
   *TODO: can we use one buffer instead two i.e. buffer,buffer2??
   */
  fprintf(skel_fh, "void* buffer = NULL;\n");
  printindent(skel_fh);
  fprintf(skel_fh, "void* buffer2 = NULL;\n");
  printindent(skel_fh);
  fprintf(skel_fh, "struct timeval tv, tv1;\n");
  printindent(skel_fh);
  fprintf(skel_fh, "MPI_Request request, *requests;\n");
  printindent(skel_fh);
  fprintf(skel_fh, "MPI_Status status, *statuses;\n");
  printindent(skel_fh);
  fprintf(skel_fh, "int *temp_int_array;\n");
  printindent(skel_fh);
  fprintf(skel_fh, "int current = 0;\n");          
  printindent(skel_fh);
  fprintf(skel_fh, "req_handler_t req;\n");  
  printindent(skel_fh);
  fprintf(skel_fh, "init_req(&req,10000);\n");
  printindent(skel_fh);
  fprintf(skel_fh, "comm_handler_t communicators;\n");
  printindent(skel_fh);
  fprintf(skel_fh, "event_time_t* event_times, *io_times;\n");
  printindent(skel_fh);
  fprintf(skel_fh, "init_comm(&communicators, 10000);\n");
  printindent(skel_fh);
  fprintf(skel_fh, "init_params();\n");
  printindent(skel_fh);
  fprintf(skel_fh, "char name[256];\n");
}

/*Generates closing part of main.c*/
void generate_main_c_end(FILE * skel_fh){
  printindent(skel_fh);
  fprintf(skel_fh, "if(buffersize > 0) {\n");
  indent++;
  printindent(skel_fh);
  fprintf(skel_fh, "free(buffer); free(buffer2);\n");
  indent--;
  printindent(skel_fh);
  fprintf(skel_fh, "}\n");
  //printindent(skel_fh);
  //fprintf(skel_fh, "free_skel_rank();\n");
  printindent(skel_fh);
  fprintf(skel_fh, "finalize_req(req);\n");
  printindent(skel_fh);
  fprintf(skel_fh, "free_params();\n");
  printindent(skel_fh);
  fprintf(skel_fh, "free_comm(&communicators);\n");
  //Writing the total time taken to 0 rank process
  printindent(skel_fh);
  fprintf(skel_fh,"if(myrank==0) {\n");
  indent++;
  printindent(skel_fh);
  fprintf(skel_fh, "if(tv1.tv_usec < tv.tv_usec){\n");
  indent++;
  printindent(skel_fh);
  fprintf(skel_fh, "printf(\"time: %%d.%%d\\n\", tv1.tv_sec - tv.tv_sec - 1, tv1.tv_usec + 1000000 - tv.tv_usec);\n");
  indent--;
  printindent(skel_fh);
  fprintf(skel_fh, "} else {\n");
  indent++;
  printindent(skel_fh);
  fprintf(skel_fh, "printf(\"time: %%d.%%d\\n\", tv1.tv_sec - tv.tv_sec, tv1.tv_usec - tv.tv_usec);\n");
  indent--;
  printindent(skel_fh);
  fprintf(skel_fh, "}\n");
    
  indent--;
  printindent(skel_fh);
  fprintf(skel_fh,"}\n");
  printindent(skel_fh);
  fprintf(skel_fh,"return 0;\n");
  fprintf(skel_fh,"}\n");
}

/**
 * by Xing WU
 */
void generate_util_h(FILE *skel_type_fh){
  fprintf(skel_type_fh, "#ifndef UTIL_H\n");
  fprintf(skel_type_fh, "#define UTIL_H\n\n");

  fprintf(skel_type_fh, "#include <time.h>\n");
  fprintf(skel_type_fh, "#include <sys/time.h>\n");
  fprintf(skel_type_fh, "#include <stdio.h>\n");
  fprintf(skel_type_fh, "#include <stdlib.h>\n");
  fprintf(skel_type_fh, "#include <limits.h>\n");
  fprintf(skel_type_fh, "#include <string.h>\n");
  fprintf(skel_type_fh, "#include <assert.h>\n");
  fprintf(skel_type_fh, "#include <mpi.h>\n\n\n");

  //global variabled in util.h
  fprintf(skel_type_fh, "\
int *cur_iter;\n\
int *iters_len;\n\
int *cur_mems;\n\
int *mems_len;\n\
");

  fprintf(skel_type_fh, "\ntypedef struct req_handler{\n");
  fprintf(skel_type_fh, "\tMPI_Request *req_buf;\n");
  fprintf(skel_type_fh, "\tMPI_Status  *statuses;\n");
  fprintf(skel_type_fh, "\tint req_bufsize;\n");
  fprintf(skel_type_fh, "\tint req_bufcur;\n");
  fprintf(skel_type_fh, "} * req_handler_t;\n\n\n");

  fprintf(skel_type_fh, "\
typedef struct comm_handler{\n\
\tMPI_Comm *comms;\n\
\tint count;\n\
\tint size;\n\
} comm_handler_t;\n\n\n\
");

  fprintf(skel_type_fh, "\
typedef struct event_time{\n\
\tint prev_event;\n\
\tlong long time;\n\
} event_time_t;\n\n\n\
");

  //structures for params
  fprintf(skel_type_fh, "\
typedef struct {\n\
\tint * val;\n\
\tchar * charval;\n\
\tint iter;\n\
} param_t;\n\n\n\
typedef enum {\n\
\tSOURCE = 0,\n\
\tDEST,\n\
\tCOUNT,\n\
\tREQUEST,\n\
\tCOLOR,\n\
\tKEY,\n\
\tFILENAME,\n\
\tSIZE,\n\
\tFH,\n\
\tAMODE,\n\
\tOFFSET,\n\
\tWHENCE,\n\
\tNEWTYPE,\n\
\tPOSIX_FD,\n\
\tPOSIX_FLAGS,\n\
\tPOSIX_MODE,\n\
\tTIME,\n\
\tMAX_PARAM_TYPE\n\
}param_type_t;\n\n\n\
typedef struct {\n\
\tparam_t param[MAX_PARAM_TYPE];\n\
}event_param_t;\n\n\n\
");

  fprintf(skel_type_fh, "\
void init_params();\n\
void free_params();\n\
int* get_param_val_list(int rank, char* val_str);\n\
int get_param_val(int rank, char* val_str, param_t* event_param);\n\
int get_iters_length(int rank, char* val_str, int loop_idx, int loop_depth);\n\
int get_mems_length(int rank, char* val_str, int loop_idx, int loop_depth);\n\
int get_max_size();\n\
void do_compute(int cur_event, event_time_t* event_times, int length);\n\
int *parse_val_str(char *buf);\n\
int member_rank_helper(int rank_target, int *list, int start, int depth);\n\
int is_member(int rank, char *buf);\n\
int unroll_is_member(int rank, char *buf);\n\
int is_memb(int rank, int *list);\n\
int is_loop_member(int rank, char* buf, int loop_depth);\n\
void update_mem_cnts(int close_depth);\n\
void init_req(req_handler_t *r, int size);\n\
int add_request(req_handler_t req);\n\
void find_requests(req_handler_t req, int count, int *indices, MPI_Request **requests);\n\
int reset_req(req_handler_t req, int *req_indices, int count);\n\
void finalize_req(req_handler_t req);\n\
void init_comm(comm_handler_t *cm, int size);\n\
MPI_Comm* get_next_comm(comm_handler_t *cm);\n\
MPI_Comm get_comm(comm_handler_t *cm, int index);\n\
MPI_Comm* get_comm_pointer(comm_handler_t *cm, int index);\n\
void free_comm(comm_handler_t *cm);\n\
void do_isend(void* buffer, MPI_Datatype datatype, char* ccounts, char* cdests, int tag, MPI_Comm comm, req_handler_t req, int myrank, int comm_size);\n\
void do_send(void* buffer, MPI_Datatype datatype, char* ccounts, char* cdests, int tag, MPI_Comm comm, int myrank, int comm_size);\n\
void do_irecv(void* buffer, MPI_Datatype datatype, char* ccounts,  char* csources, int tag, MPI_Comm comm, req_handler_t req, int myrank, int comm_size);\n\
void do_recv(void* buffer, MPI_Datatype datatype, char* ccounts, char* csources, int tag, MPI_Comm comm, MPI_Status *status, int myrank, int comm_size);\n\
void do_waitall(char *cwaits, req_handler_t req, int myrank);\n\
void do_wait(char *cwaits, req_handler_t req, int myrank);\n\
void do_reduce(void *sendbuf, void* recvbuf, char *ccounts, MPI_Datatype datatype, MPI_Op op, int root, MPI_Comm comm, int myrank);\n\
void do_allreduce(void *sendbuf, void* recvbuf, char *ccounts, MPI_Datatype datatype, MPI_Op op, MPI_Comm comm, int myrank);\n\
void do_bcast(void *buffer, char *ccounts, MPI_Datatype datatype, int root, MPI_Comm comm, int myrank);\n\
void do_comm_split(MPI_Comm comm, char* ccolors, char* ckeys, MPI_Comm *newcomm, int myrank);\n\n\
");

  fprintf(skel_type_fh, "#endif\n");
}

void generate_util_h_parse_once_params(FILE *skel_type_fh, int length){
  fprintf(skel_type_fh, "\n\n\
typedef struct parse_once_param{\n\
\tint *count_vector;\n\
} parse_once_param_t;\n\n\
void parse_once(char *count_string, int trip_count, int myrank, int index);\n\
void free_parse_once_params();\n\n\
static parse_once_param_t parse_once_parameters[%d];\n\n\
", length);

}

void generate_util_c_header(FILE *skel_type_fh){
  fprintf(skel_type_fh, "#include <math.h>\n");
  fprintf(skel_type_fh, "#include <sys/stat.h>\n");
  fprintf(skel_type_fh, "#include <sys/time.h>\n");
  fprintf(skel_type_fh, "#include \"util.h\"\n\n\n");

  fprintf(skel_type_fh, "\
#define MAX_NUM_EVENTS %d\n\
#define MAX_NUM_LOOPS %d\n\
#define MAX_LOOP_DEPTH 100\n\
#define RL_SEPARATOR ','\n\
#define PARAM_ENTRY_SEPARATOR ';'\n\n\
", 
h_gen_context->event_count, h_gen_context->loop_count
);

  fprintf(skel_type_fh, "\
int prev_event = 0;\n\
event_param_t* event_params;\n\
param_t* loops;\n\
param_t* mems;\n\
MPI_File fh[256];\n\
MPI_Datatype data_type[256];\n\
int fd[256];\n\
FILE *file_fh[256];\n\
struct timeval begin, end;\n\
long long ioleft=0;\n\
int IOTIMER = 1;\n\
");
}

void generate_util_compute_time_function(FILE *skel_type_fh){
  //using usleep for computation
  fprintf(skel_type_fh, "\
\n\nstatic void compute(long long delay) {\n\
#ifndef FEATURE_USE_SLEEP\n\
\tstruct timeval param, begin, now, rem;\n\
\tint second = delay / 1000000;\n\
\tint microsecond = delay % 1000000;\n\
\tparam.tv_sec = second;\n\
\tparam.tv_usec = microsecond;\n\
\tgettimeofday (&begin, NULL);\n\
\ttimeradd (&begin, &param, &param);\n\
\trem.tv_sec = 0;\n\
\trem.tv_usec = 0;\n\
\twhile (rem.tv_sec >= 0){\n\
\t\tgettimeofday (&now, NULL);\n\
\t\ttimersub (&param, &now, &rem);\n\
\t}\n\
#else\n\
\tusleep(delay);\n\
#endif\n\
}\n\
");

  fprintf(skel_type_fh, "\
\n\nvoid do_compute(int cur_event, event_time_t* event_times, int length){\n\
\tint idx = 0;\n\
\tfor(idx = 0; idx < length; idx++){\n\
\t\tif(event_times[idx].prev_event == prev_event){\n\
\t\t\tbreak;\n\
\t\t}\n\
\t}\n\
\tassert(idx < length);\n\
\tcompute(event_times[idx].time);\n\
\tprev_event = cur_event;\n\
}\n\
");

}

void generate_max_size_function(FILE* skel_type_fh, derived_type_dump_t* type) {
  int i=0;
  extern int comm_total;
  //TODO: the get_max_size function should be fixed, the max value should 
  //be calculated when traversing the trace
  fprintf(skel_type_fh, "\nint get_max_size() {\n");
  /*fprintf(skel_type_fh, "\tint value = 0;\n\tMPI_Aint size = 0;\n");

  if(type != NULL && type->total_count > 0) {
    for(i=0;i<type->total_count;i++) {
      if(type->type_usage_count[i]>0) {
        fprintf(skel_type_fh, "\tMPI_Type_extent(%s,&size);\n",type->type_name[i]);
        fprintf(skel_type_fh, "\tif(value < (%d * size)) value += %d*size;\n", type->type_usage_count[i],type->type_usage_count[i]);
      }
    }
  }

  if(use_comm_total_flag==0)
    fprintf(skel_type_fh, "\treturn value;\n");
  else
    fprintf(skel_type_fh, "\treturn %d * value;\n",comm_total);*/
  fprintf(skel_type_fh, "return %d;\n", 1 << 29);
  fprintf(skel_type_fh, "}\n");

  /* commented by Xing
  fprintf(skel_type_fh, "\n\nvoid skel_sleep(long long sec, long long nsec) {\n");
  fprintf(skel_type_fh, "\tstruct timeval param, begin, now, rem;\n");
  fprintf(skel_type_fh, "\tparam.tv_sec = sec;\n");
  fprintf(skel_type_fh, "\tparam.tv_usec = nsec/1000;\n");
  fprintf(skel_type_fh, "\tgettimeofday (&begin, NULL);\n");
  fprintf(skel_type_fh, "\ttimeradd (&begin, &param, &param);\n");
  fprintf(skel_type_fh, "\trem.tv_sec = 0;\n");
  fprintf(skel_type_fh, "\trem.tv_usec = 0;\n");
  fprintf(skel_type_fh, "\twhile (rem.tv_sec >= 0){\n");
  fprintf(skel_type_fh, "\t\tgettimeofday (&now, NULL);\n");
  fprintf(skel_type_fh, "\t\ttimersub (&param, &now, &rem);\n");
  fprintf(skel_type_fh, "\t}\n");
  //fprintf(skel_type_fh, "struct timespec ts;\n");
  //fprintf(skel_type_fh, "ts.tv_sec = sec;\n");
  //fprintf(skel_type_fh, "ts.tv_nsec = nsec;\n");
  //fprintf(skel_type_fh, "nanosleep(&ts, NULL);\n");
  fprintf(skel_type_fh, "}\n");
  */
}

void generate_is_member_function(FILE *skel_type_fh){	
  fprintf(skel_type_fh, "\
\n\nint *parse_val_str(char *buf) {\n\
\tchar *start_pos = buf, *iter;\n\
\tint i;\n\
\twhile (*start_pos == ' ') start_pos++;\n\
\tint len = 1;\n\
\tfor (iter = start_pos + 1; ; iter++) {\n\
\t\tif (*iter == ' ') {\n\
\t\t\tlen++;\n\
\t\t\tcontinue;\n\
\t\t}\n\
\t\tif ((*iter < '0' || *iter > '9') && *iter != '-') {\n\
\t\t\tbreak;\n\
\t\t}\n\
\t}\n\
\tint *rlist = (int*)malloc(sizeof(int) * len);\n\
\tif(!rlist)\n\
\t\texit(1);\n\
\tint num_prsds;\n\
\tsscanf(start_pos, \"%%d\", &num_prsds);\n\
\trlist[0] = num_prsds;\n\
\tstart_pos = strchr(start_pos, ' ') + 1;\n\
\tfor(i = 1; i < len; i++) {\n\
\t\tsscanf(start_pos, \"%%d\", &rlist[i]);\n\
\t\tstart_pos = strchr(start_pos, ' ') + 1;\n\
\t}\n\
\treturn rlist;\n\
}\n\
");

  fprintf(skel_type_fh, "\
\n\nint member_rank_helper(int rank_target, int *list, int start, int depth){\n\
\tint i, rank, iters, stride;\n\
\tif(depth > -1) return 0;\n\
\titers = list[0];\n\
\tstride = list[1];\n\
\tfor(i = 0, rank = start; i < iters; i++, rank += stride) {\n\
\t\tif(rank == rank_target)\n\
\t\t\treturn 1;\n\
\t\tif(depth != -1) {\n\
\t\t\tif(member_rank_helper(rank_target, list + 2, rank, depth + 1))\n\
\t\t\t\treturn 1;\n\
\t\t}\n\
\t}\n\
\treturn 0;\n\
}\n\
");


  fprintf(skel_type_fh, "\
\n\nint is_member(int rank, char *buf){\n\
\tint *list = parse_val_str(buf);\n\
\tint rtn = is_memb(rank, list);\n\
\tfree(list);\n\
\treturn rtn;\n\
}\n\
");

  fprintf(skel_type_fh, "\
\n\nint unroll_is_member(int rank, char *buf){\n\
\tint *list = parse_val_str(buf);\n\
\tint i;\n\
\tfor(i=1; i<=list[0]; i++){\n\
\t\tif(list[i] == rank){\n\
\t\t\tfree(list);\n\
\t\t\treturn 1;\n\
\t\t}\n\
\t}\n\
\tfree(list);\n\
\treturn 0;\n\
}\n\
");

  fprintf(skel_type_fh, "\
\n\nint is_memb(int rank, int *list) {\n\
\tint pos, cur_prsd, depth, start;\n\
\tif(!list) return 0;\n\
\tpos = 1;\n\
\tcur_prsd = 0;\n\
\twhile(cur_prsd < list[0]) {\n\
\t\tdepth = list[pos];\n\
\t\tstart = list[pos + 1];\n\
\t\tpos += 2;\n\
\t\tif(member_rank_helper(rank, list + pos, start, depth)) {\n\
\t\t\treturn 1;\n\
\t\t}\n\
\t\tif(cur_prsd + 1 == list[0]) break;\n\
\t\tpos++;\n\
\t\twhile(list[pos] > 0) pos++;\n\
\t\tcur_prsd++;\n\
\t}\n\
\treturn 0;\n\
}\n\
");

  //is_loop_member function
  fprintf(skel_type_fh, "\
\n\nint is_loop_member(int rank, char* buf, int loop_depth){\n\
\tint rtn = 0;\n\
\tif(is_member(rank, buf)){\n\
\t\trtn = 1;\n\
\t\tint idx = 0;\n\
\t\tfor(idx = 0; idx < loop_depth; idx++){\n\
\t\t\tif(cur_mems[idx] >= mems_len[idx] &&\n\
\t\t\tcur_iter[idx] != iters_len[idx] - 1){\n\
\t\t\t\trtn = 0;\n\
\t\t\t\tbreak;\n\
\t\t\t}\n\
\t\t}\n\
\t}\n\
\tif(rtn){\n\
\t\tint idx = 0;\n\
\t\tfor(idx = 0; idx < loop_depth; idx++){\n\
\t\t\tcur_mems[idx]++;\n\
\t\t}\n\
\t}\n\
\treturn rtn;\n\
}\n\
");

  //update_mem_cnts function
  fprintf(skel_type_fh, "\
\n\nvoid update_mem_cnts(int closing_depth){\n\
\tif(cur_iter[closing_depth - 1] == iters_len[closing_depth - 1] - 1){\n\
\t\tcur_mems[closing_depth - 1] = 0;\n\
\t\treturn;\n\
\t}\n\
\tint idx = 0;\n\
\tfor(idx = 0; idx < closing_depth; idx++){\n\
\t\tcur_mems[idx] -= cur_mems[closing_depth - 1];\n\
\t}\n\
}\n\
");
}


/* The following function was implemented by Vivek and commented out by Xing.
 * The function above is the one in use!!!
 */
/*
void generate_is_member_function(FILE * skel_type_fh)
{
	fprintf(skel_type_fh,"int x=0;\n"); 
	fprintf(skel_type_fh,"int y =0;\n"); 
	fprintf(skel_type_fh,"int **rank_list;\n"); 
	fprintf(skel_type_fh,"void read_skel_rank() {\n");
	fprintf(skel_type_fh,"FILE *fp;\n"); 
	fprintf(skel_type_fh,"int cnt;\n"); 
	fprintf(skel_type_fh,"char *line;\n"); 
	fprintf(skel_type_fh,"char *ptr;\n"); 
	fprintf(skel_type_fh,"int i,j;\n"); 
	fprintf(skel_type_fh,"int tmp;\n"); 
	fprintf(skel_type_fh,"line=malloc(LINE_MAX*sizeof(char));\n"); 
	fprintf(skel_type_fh,"fp=fopen(\"skel_rank\",\"r\");\n"); 
	fprintf(skel_type_fh,"while(!feof(fp))\n"); 
	fprintf(skel_type_fh,"{\n"); 
	fprintf(skel_type_fh,"   fgets(line,LINE_MAX,fp);\n"); 
	fprintf(skel_type_fh,"   if(*line=='\\0')\n");
	fprintf(skel_type_fh,"    return;");
	fprintf(skel_type_fh,"   y++;\n"); 
	fprintf(skel_type_fh,"   ptr=line;\n"); 
	fprintf(skel_type_fh,"   cnt=0;\n"); 
	fprintf(skel_type_fh,"   while(*ptr!='\\n')\n"); 
	fprintf(skel_type_fh,"   {\n"); 
	fprintf(skel_type_fh,"      if(*ptr==\' \')\n"); 
	fprintf(skel_type_fh,"         cnt++;\n"); 
	fprintf(skel_type_fh,"      ptr++;\n"); 
	fprintf(skel_type_fh,"   }\n"); 
	fprintf(skel_type_fh,"   if(cnt > x)\n"); 
	fprintf(skel_type_fh,"      x = cnt;\n"); 
	fprintf(skel_type_fh,"}\n"); 
	fprintf(skel_type_fh,"y--;\n"); 
	fprintf(skel_type_fh,"fclose(fp);\n"); 
	fprintf(skel_type_fh,"fp=fopen(\"skel_rank\",\"r\");\n"); 
	fprintf(skel_type_fh,"rank_list=malloc(y*sizeof(int*));\n"); 
	fprintf(skel_type_fh,"for(i=0;i<y;i++)\n"); 
	fprintf(skel_type_fh,"  rank_list[i]=malloc(x*sizeof(int));\n"); 
	fprintf(skel_type_fh,"for(j=0;j<y;j++)\n"); 
	fprintf(skel_type_fh,"{\n"); 
	fprintf(skel_type_fh,"   fgets(line,LINE_MAX,fp);\n"); 
	fprintf(skel_type_fh,"   ptr= line;\n"); 
	fprintf(skel_type_fh,"   sscanf(ptr,\"%%d\",&tmp); \n"); 
	fprintf(skel_type_fh,"   while(*ptr!=\' \')\n"); 
	fprintf(skel_type_fh,"      ptr++;\n"); 
	fprintf(skel_type_fh,"   i=0;\n"); 
	fprintf(skel_type_fh,"   while(*ptr!=\'\\n\')\n"); 
	fprintf(skel_type_fh,"   {\n"); 
	fprintf(skel_type_fh,"     ptr++;\n"); 
	fprintf(skel_type_fh,"     sscanf(ptr,\"%%d\",&rank_list[j][i]); \n"); 
	fprintf(skel_type_fh,"     while(*ptr!=\' \')\n"); 
	fprintf(skel_type_fh,"     {\n"); 
	fprintf(skel_type_fh,"       ptr++;\n"); 
	fprintf(skel_type_fh,"       if(*ptr=='\\n')\n"); 
	fprintf(skel_type_fh,"         break;\n"); 
	fprintf(skel_type_fh,"     }\n"); 
	fprintf(skel_type_fh,"     i++;\n"); 
	fprintf(skel_type_fh,"   }\n"); 
	fprintf(skel_type_fh,"}\n"); 
	fprintf(skel_type_fh,"fclose(fp);\n"); 
	fprintf(skel_type_fh,"}\n"); 
	fprintf(skel_type_fh,"int is_member(int a,int rank)\n"); 
	fprintf(skel_type_fh,"{\n");   
	fprintf(skel_type_fh,"int event_number;\n"); 
	fprintf(skel_type_fh,"int no_ranklist;\n"); 
	fprintf(skel_type_fh,"int start;\n"); 
	fprintf(skel_type_fh,"int return_val=0; \n"); 
	fprintf(skel_type_fh,"int  depth, start_rank, stride, iters, i, j, k;\n"); 
	fprintf(skel_type_fh,"int flag = 0;\n"); 
	fprintf(skel_type_fh,"int ptr;\n"); 
	fprintf(skel_type_fh,"ptr=0;\n"); 
	fprintf(skel_type_fh,"no_ranklist = rank_list[a-1][ptr];\n"); 
	fprintf(skel_type_fh,"ptr++;\n"); 
	fprintf(skel_type_fh,"        for(k=0; k<no_ranklist; k++){\n"); 
	fprintf(skel_type_fh,"               flag=0;\n"); 
	fprintf(skel_type_fh,"               depth = rank_list[a-1][ptr];\n"); 
	fprintf(skel_type_fh,"               ptr++;\n"); 
	fprintf(skel_type_fh,"               depth = -depth;\n"); 
	fprintf(skel_type_fh,"               start_rank = rank_list[a-1][ptr];\n"); 
	fprintf(skel_type_fh,"               ptr++;\n"); 
	fprintf(skel_type_fh,"               if(rank==start_rank)\n"); 
	fprintf(skel_type_fh,"               {\n"); 
	fprintf(skel_type_fh,"                   return 1;\n"); 
	fprintf(skel_type_fh,"               }\n"); 
	fprintf(skel_type_fh,"               if(rank<start_rank)\n"); 
	fprintf(skel_type_fh,"               {\n"); 
	fprintf(skel_type_fh,"                   return 0;\n"); 
	fprintf(skel_type_fh,"               }\n"); 
	fprintf(skel_type_fh,"                start = start_rank;\n"); 
	fprintf(skel_type_fh,"                if(depth > 1)\n"); 
	fprintf(skel_type_fh,"                {\n"); 
	fprintf(skel_type_fh,"                   for(j=0; j<depth-1;j++){\n"); 
	fprintf(skel_type_fh,"                        iters = rank_list[a-1][ptr];\n"); 
	fprintf(skel_type_fh,"                        ptr++;\n"); 
	fprintf(skel_type_fh,"                        stride = rank_list[a-1][ptr];\n"); 
	fprintf(skel_type_fh,"                        ptr++;\n"); 
	fprintf(skel_type_fh,"                        for(i=start;i<=(start+((iters)*stride));i+=stride)\n"); 
	fprintf(skel_type_fh,"                        {\n"); 
	fprintf(skel_type_fh,"                            if(rank<i)\n"); 
	fprintf(skel_type_fh,"                            {\n"); 
	fprintf(skel_type_fh,"                                i-=stride;\n"); 
	fprintf(skel_type_fh,"                                break;\n"); 
	fprintf(skel_type_fh,"                            }\n"); 
	fprintf(skel_type_fh,"                        }\n"); 
	fprintf(skel_type_fh,"                        if(i>=(start+((iters)*stride)))\n");
	fprintf(skel_type_fh,"                            flag=1;            \n"); 
	fprintf(skel_type_fh,"                        start = i;\n"); 
	fprintf(skel_type_fh,"                    }\n"); 
	fprintf(skel_type_fh,"                 }\n"); 
	fprintf(skel_type_fh,"                        iters = rank_list[a-1][ptr];\n"); 
	fprintf(skel_type_fh,"                        ptr++;\n"); 
	fprintf(skel_type_fh,"                        stride = rank_list[a-1][ptr];\n"); 
	fprintf(skel_type_fh,"                        ptr++;\n"); 
	fprintf(skel_type_fh,"                 if(flag==1)\n"); 
	fprintf(skel_type_fh,"                   continue;   \n"); 
	fprintf(skel_type_fh,"                 if(stride > 1 && iters > 1) {\n"); 
	fprintf(skel_type_fh,"                    return_val = (((rank-start)%%stride==0) && (rank <= (start+((iters-1)*stride)))) ? 1 : 0;\n"); 
	fprintf(skel_type_fh,"                 }\n"); 
	fprintf(skel_type_fh,"                 else if(stride == 1 && iters > 1) {\n"); 
	fprintf(skel_type_fh,"                    return_val = (rank >= start && rank <=(start+((iters-1)*stride))) ? 1 : 0;\n"); 
	fprintf(skel_type_fh,"                 }\n"); 
	fprintf(skel_type_fh,"                 else if(stride == 0 && iters == 1) {\n"); 
	fprintf(skel_type_fh,"                    return_val = (rank == start) ? 1 : 0;\n"); 
	fprintf(skel_type_fh,"                 }\n"); 
	fprintf(skel_type_fh,"\n"); 
	fprintf(skel_type_fh,"                 if(return_val == 1)\n"); 
	fprintf(skel_type_fh,"                 {   \n"); 
	fprintf(skel_type_fh,"                    return return_val;\n"); 
	fprintf(skel_type_fh,"                 }  \n"); 
	fprintf(skel_type_fh,"        }\n"); 
	fprintf(skel_type_fh,"\n"); 
	fprintf(skel_type_fh,"return return_val;\n"); 
	fprintf(skel_type_fh,"}\n"); 
	fprintf(skel_type_fh,"void free_skel_rank()\n"); 
	fprintf(skel_type_fh,"{\n"); 
	fprintf(skel_type_fh,"int i= 0;\n"); 
	fprintf(skel_type_fh,"if(rank_list)\n"); 
	fprintf(skel_type_fh,"{\n"); 
	fprintf(skel_type_fh,"  for(i=0;i<y;i++)\n"); 
	fprintf(skel_type_fh,"  {\n"); 
	fprintf(skel_type_fh,"    free(rank_list[i]);\n"); 
	fprintf(skel_type_fh,"  }\n"); 
	fprintf(skel_type_fh,"  free(rank_list);\n"); 
	fprintf(skel_type_fh,"}\n"); 
	fprintf(skel_type_fh,"}\n"); 
}
*/


/**
 *
 * author: Xing Wu
 */
void generate_comm_handler_functions(FILE *skel_type_fh){

  fprintf(skel_type_fh, "\
\n\nvoid init_comm(comm_handler_t *cm, int size){\n\
\tint i;\n\
\tcm->comms = (MPI_Comm *)malloc(size * sizeof(MPI_Comm));\n\
\tfor(i=0; i<size; i++)\n\
\t\tcm->comms[i] = MPI_COMM_NULL;\n\
\tcm->count = 3;\n\
\tcm->size = size;\n\
\tcm->comms[0] = MPI_COMM_NULL;\n\
\tcm->comms[1] = MPI_COMM_SELF;\n\
\tcm->comms[2] = MPI_COMM_WORLD;\n\
}\n\
");



  fprintf(skel_type_fh, "\
\n\nMPI_Comm* get_next_comm(comm_handler_t *cm){\n\
\tif(cm->count == cm->size)\n\
\t\treturn NULL;\n\
\t(cm->count)++;\n\
\treturn cm->comms+cm->count-1;\n\
}\n\
");

  fprintf(skel_type_fh, "\
\n\nMPI_Comm get_comm(comm_handler_t *cm, int index){\n\
\tint real_index = index < 1000 ? index : index - 1000 + 3;\n\
\treturn cm->comms[real_index];\n\
}\n\
");

  fprintf(skel_type_fh, "\
\n\nMPI_Comm* get_comm_pointer(comm_handler_t *cm, int index){\n\
\tint real_index = index < 1000 ? index : index - 1000 + 3;\n\
\treturn cm->comms + real_index;\n\
}\n\
");

  fprintf(skel_type_fh, "\
\n\nvoid free_comm(comm_handler_t *cm){\n\
\tfree(cm->comms);\n\
\tcm->count = 0;\n\
\tcm->size = 0;\n\
}\n\
");
}


/**
 * author: Xing Wu
 */
void generate_req_handler_function(FILE* skel_type_fh){
	fprintf(skel_type_fh,"\n\n\n");
	fprintf(skel_type_fh,"void init_req(req_handler_t *r, int size){\n");
	fprintf(skel_type_fh,"\treq_handler_t req;\n");
	fprintf(skel_type_fh,"\tint i;\n");
	fprintf(skel_type_fh,"\treq = (req_handler_t) malloc(sizeof(struct req_handler));\n");
	fprintf(skel_type_fh,"\treq->req_bufcur = 0;\n");
	fprintf(skel_type_fh,"\treq->req_bufsize = size;\n");
	fprintf(skel_type_fh,"\treq->req_buf = (MPI_Request *) malloc(sizeof(MPI_Request) * req->req_bufsize);\n");
	fprintf(skel_type_fh,"\treq->statuses = (MPI_Status *) malloc(sizeof(MPI_Status) * req->req_bufsize);\n");
	fprintf(skel_type_fh,"\tfor(i=0; i<req->req_bufsize; i++)\n");
	fprintf(skel_type_fh,"\t\treq->req_buf[i] = MPI_REQUEST_NULL;\n");
	fprintf(skel_type_fh,"\t*r = req;\n");
	fprintf(skel_type_fh,"}\n\n");

	fprintf(skel_type_fh,"int add_request(req_handler_t req){\n");
	fprintf(skel_type_fh,"\tint rtn = req->req_bufcur;\n");
	fprintf(skel_type_fh,"\treq->req_bufcur++;\n");
	fprintf(skel_type_fh,"\tif(req->req_bufcur >= req->req_bufsize)\n");
	fprintf(skel_type_fh,"\t\treq->req_bufcur = 0;\n");
	fprintf(skel_type_fh,"\treturn rtn;\n");
	fprintf(skel_type_fh,"}\n\n");

	fprintf(skel_type_fh,"void find_requests(req_handler_t req, int count, int *indices, MPI_Request **requests){\n");
	fprintf(skel_type_fh,"\tint i=0;\n");
	fprintf(skel_type_fh,"\t*requests = (MPI_Request *)malloc(sizeof(MPI_Request) * count);\n");
	fprintf(skel_type_fh,"\tfor(i=0; i<count; i++)\n");
	fprintf(skel_type_fh,"\t\tif(indices[i] != -1)\n");
	fprintf(skel_type_fh,"\t\t\t(*requests)[i] = req->req_buf[(req->req_bufcur + indices[i]) % req->req_bufsize];\n");
	fprintf(skel_type_fh,"\t\telse\n");
	fprintf(skel_type_fh,"\t\t\t(*requests)[i] = MPI_REQUEST_NULL;\n");
	fprintf(skel_type_fh,"}\n\n");

	fprintf(skel_type_fh,"int reset_req(req_handler_t req, int *req_indices, int count){\n");
	fprintf(skel_type_fh,"\tint i;\n");
	fprintf(skel_type_fh,"\tfor(i=0; i<count; i++)\n");
	fprintf(skel_type_fh,"\t\tif(req_indices[i] != -1)\n");
	fprintf(skel_type_fh,"\t\t\treq->req_buf[(req->req_bufcur + req_indices[i]) % req->req_bufsize] = MPI_REQUEST_NULL;\n");
	fprintf(skel_type_fh,"}\n\n");

	fprintf(skel_type_fh,"void finalize_req(req_handler_t req){\n");
	fprintf(skel_type_fh,"\tfree(req->req_buf);\n");
	fprintf(skel_type_fh,"\treq->req_buf = NULL;\n");
	fprintf(skel_type_fh,"\tfree(req->statuses);\n");
	fprintf(skel_type_fh,"\treq->req_bufsize = 0;\n");
	fprintf(skel_type_fh,"\treq->req_bufcur = 0;\n");
	fprintf(skel_type_fh,"\tfree(req);\n");
	fprintf(skel_type_fh,"}\n");
}

/* The follow function was implemented by Vivek and removed by Xing,
 * looks like the recursive buffer based request handle management
 * approach was not correctly implemented by the following function.
 * A newer version done by Xing is the one above.*/
/*
void generate_req_handler_function(FILE* skel_type_fh){
	fprintf(skel_type_fh,"void init_req(req_handler_t * r, int size)\n");
	fprintf(skel_type_fh,"{\n");
	fprintf(skel_type_fh,"  req_handler_t req;\n");
	fprintf(skel_type_fh,"  int i;\n");

	fprintf(skel_type_fh,"  req = (req_handler_t) malloc (sizeof(struct req_handler));\n");

	fprintf(skel_type_fh,"  req->req_bufcur = 0;\n");
	fprintf(skel_type_fh,"  req->req_bufsize = size;\n");
	fprintf(skel_type_fh,"  req->req_buf = (MPI_Request *) malloc(sizeof(MPI_Request) * req->req_bufsize);\n");
	fprintf(skel_type_fh,"  req->statuses = (MPI_Status *) malloc(sizeof(MPI_Status) * req->req_bufsize);\n");
	fprintf(skel_type_fh,"  for(i = 0; i < req->req_bufsize; i++)\n");
	fprintf(skel_type_fh,"    req->req_buf[i] = MPI_REQUEST_NULL;\n");

	fprintf(skel_type_fh,"  *r = req;\n");
	fprintf(skel_type_fh,"}\n");
	fprintf(skel_type_fh,"int add_request(req_handler_t req)\n");
	fprintf(skel_type_fh,"{\n");
	fprintf(skel_type_fh,"   int pos,i,done;\n");
	fprintf(skel_type_fh,"   pos = req->req_bufcur;\n");
	fprintf(skel_type_fh,"   done = 0;\n");
	fprintf(skel_type_fh,"   for(i = pos+1; i < req->req_bufsize; i++)\n");
	fprintf(skel_type_fh,"   {\n");
	fprintf(skel_type_fh,"      if(req->req_buf[i]==MPI_REQUEST_NULL){\n");
	fprintf(skel_type_fh,"         done = 1;\n");
	fprintf(skel_type_fh,"         break;\n");
	fprintf(skel_type_fh,"      }\n");
	fprintf(skel_type_fh,"   }\n");
	fprintf(skel_type_fh,"   if(done==0)\n");
	fprintf(skel_type_fh,"   {\n");
	fprintf(skel_type_fh,"      for(i = 0; i < pos; i++)\n");
	fprintf(skel_type_fh,"      {\n");
	fprintf(skel_type_fh,"        if(req->req_buf[i]==MPI_REQUEST_NULL){\n");
	fprintf(skel_type_fh,"          done = 1;\n");
	fprintf(skel_type_fh,"          break;\n");
	fprintf(skel_type_fh,"        }\n");
	fprintf(skel_type_fh,"      }\n");
	fprintf(skel_type_fh,"   }\n");
	fprintf(skel_type_fh,"   if(done==0)\n");
	fprintf(skel_type_fh,"   {\n");
	fprintf(skel_type_fh,"     printf(\"ERROR: BUFFER IS SMALL.\\n\");\n");
	fprintf(skel_type_fh,"     exit;\n");
	fprintf(skel_type_fh,"   }\n");
	fprintf(skel_type_fh,"   req->req_bufcur = i;\n");
	fprintf(skel_type_fh,"   return pos;\n");
	fprintf(skel_type_fh,"}\n");
	fprintf(skel_type_fh,"void reset_req(req_handler_t req,int offset,int count)\n");
	fprintf(skel_type_fh,"{\n");
	fprintf(skel_type_fh,"int pos,i;\n");
	fprintf(skel_type_fh,"pos = req->req_bufsize - (offset + 1);\n");
	fprintf(skel_type_fh,"if(pos < req->req_bufcur)\n");
	fprintf(skel_type_fh,"   req->req_bufcur = pos;\n");
	fprintf(skel_type_fh,"for(i=0;i<count;i++)\n");
	fprintf(skel_type_fh,"{\n");
	fprintf(skel_type_fh,"   req->req_buf[pos]= MPI_REQUEST_NULL;\n");
	fprintf(skel_type_fh,"   pos++;\n");
	fprintf(skel_type_fh,"}\n");
	fprintf(skel_type_fh,"}\n");

	fprintf(skel_type_fh,"void finalize_req(req_handler_t req)\n");
	fprintf(skel_type_fh,"{\n");
	fprintf(skel_type_fh,"  free(req->req_buf);\n");
	fprintf(skel_type_fh,"  req->req_buf = NULL;\n");
	fprintf(skel_type_fh,"  free(req->statuses);\n");
	fprintf(skel_type_fh,"  req->req_bufsize = 0;\n");
	fprintf(skel_type_fh,"  req->req_bufcur = 0;\n");
	fprintf(skel_type_fh,"  free(req);\n");
	fprintf(skel_type_fh,"}\n");
}*/

/*
 * Generate wrappers for point-point MPI routines
 * */
void generate_p2p_ops(FILE *skel_type_fh){
  fprintf(skel_type_fh, "\
\n\nvoid do_isend(void* buffer, MPI_Datatype datatype, char* ccounts, char* cdests, \n\
\t\t\t\tint tag, MPI_Comm comm, req_handler_t req, int myrank, int comm_size){\n\
\tint count, dest, req_id;\n\
\tevent_param_t* event_param = event_params + prev_event;\n\
\tcount = get_param_val(myrank, ccounts, &(event_param->param[COUNT]));\n\
\tdest = get_param_val(myrank, cdests, &(event_param->param[DEST]));\n\
\tdest = (dest + myrank + comm_size) %% comm_size;\n\
\treq_id = add_request(req);\n\
\tassert(MPI_Isend(buffer, count, datatype, dest, tag, comm, &(req->req_buf[req_id])) == MPI_SUCCESS);\n\
}\n\
");

  fprintf(skel_type_fh, "\
\n\nvoid do_send(void* buffer, MPI_Datatype datatype, char* ccounts, char* cdests,\n\
\t\t\t\tint tag, MPI_Comm comm, int myrank, int comm_size){\n\
\tint count, dest;\n\
\tevent_param_t* event_param = event_params + prev_event;\n\
\tcount = get_param_val(myrank, ccounts, &(event_param->param[COUNT]));\n\
\tdest = get_param_val(myrank, cdests, &(event_param->param[DEST]));\n\
\tdest = (dest + myrank + comm_size) %% comm_size;\n\
\tassert(MPI_Send(buffer, count, datatype, dest, tag, comm) == MPI_SUCCESS) ;\n\
}\n\
");

  fprintf(skel_type_fh, "\
\n\nvoid do_irecv(void* buffer, MPI_Datatype datatype, char* ccounts, char* csources,\n\
\t\t\t\tint tag, MPI_Comm comm, req_handler_t req, int myrank, int comm_size){\n\
\tint count, source, req_id;\n\
\tevent_param_t* event_param = event_params + prev_event;\n\
\tcount = get_param_val(myrank, ccounts, &(event_param->param[COUNT]));\n\
\tsource = get_param_val(myrank, csources, &(event_param->param[SOURCE]));\n\
\tif(source == 2147483647)\n\
\t\tsource = MPI_ANY_SOURCE;\n\
\telse\n\
\t\tsource = (source + myrank + comm_size) %% comm_size;\n\
\treq_id = add_request(req);\n\
\tassert(MPI_Irecv( buffer, count, datatype, source, tag, comm, &(req->req_buf[req_id])) == MPI_SUCCESS);\n\
}\n\
");

  fprintf(skel_type_fh, "\
\n\nvoid do_recv(void* buffer, MPI_Datatype datatype, char* ccounts, char* csources,\n\
\t\t\t\tint tag, MPI_Comm comm, MPI_Status *status, int myrank, int comm_size){\n\
\tint count, source;\n\
\tevent_param_t* event_param = event_params + prev_event;\n\
\tcount = get_param_val(myrank, ccounts, &(event_param->param[COUNT]));\n\
\tsource = get_param_val(myrank, csources, &(event_param->param[SOURCE]));\n\
\tif(source == 2147483647)\n\
\t\tsource = MPI_ANY_SOURCE;\n\
\telse\n\
\t\tsource = (source + myrank + comm_size) %% comm_size;\n\
\tassert(MPI_Recv(buffer, count, datatype, source, tag, comm, status) == MPI_SUCCESS);\n\
}\n\
");

  fprintf(skel_type_fh, "\
\n\nvoid do_waitall(char *cwaits, req_handler_t req, int myrank){\n\
\tint *waits = get_param_val_list(myrank, cwaits);\n\
\tMPI_Status *statuses = (MPI_Status *)malloc(sizeof(MPI_Status) * waits[0]);\n\
\tMPI_Request *requests;\n\
\tfind_requests(req, waits[0], waits+1, &requests);\n\
\tassert(MPI_Waitall(waits[0], requests, statuses) == MPI_SUCCESS);\n\
\treset_req(req, waits+1, waits[0]);\n\
\tfree(waits);\n\
\tfree(requests);\n\
\tfree(statuses);\n\
}\n\
");

  fprintf(skel_type_fh, "\
\n\nvoid do_wait(char *cwaits, req_handler_t req, int myrank){\n\
\tMPI_Status status;\n\
\tMPI_Request *request;\n\
\tevent_param_t* event_param = event_params + prev_event;\n\
\tint wait = get_param_val(myrank, cwaits, &(event_param->param[REQUEST]));\n\
\tfind_requests(req, 1, &wait, &request);\n\
\tassert(MPI_Wait(request, &status) == MPI_SUCCESS);\n\
\treset_req(req, &wait, 1);\n\
\tfree(request);\n\
}\n\
");
}


/*
 * Generate wrappers for MPI collectives
 * */
void generate_collectives(FILE *skel_type_fh){

  fprintf(skel_type_fh, "\
\n\nvoid do_comm_split(MPI_Comm comm, char* ccolors, char* ckeys,\n\
\t\t\t\tMPI_Comm *newcomm, int myrank){\n\
\tevent_param_t* event_param = event_params + prev_event;\n\
\tint color = get_param_val(myrank, ccolors, &(event_param->param[COLOR]));\n\
\tint key = get_param_val(myrank, ckeys, &(event_param->param[KEY]));\n\
\tassert(MPI_Comm_split(comm, color, key, newcomm) == MPI_SUCCESS);\n\
}\n\
");

  fprintf(skel_type_fh, "\
\n\nvoid do_bcast(void *buffer, char *ccounts, MPI_Datatype datatype,\n\
\t\t\t\tint root, MPI_Comm comm, int myrank){\n\
\tevent_param_t* event_param = event_params + prev_event;\n\
\tint count = get_param_val(myrank, ccounts, &(event_param->param[COUNT]));\n\
\tassert(MPI_Bcast(buffer, count, datatype, root, comm) == MPI_SUCCESS);\n\
}\n\
");

  fprintf(skel_type_fh, "\
\n\nvoid do_allreduce(void *sendbuf, void* recvbuf, char *ccounts, MPI_Datatype datatype,\n\
\t\t\t\tMPI_Op op, MPI_Comm comm, int myrank){\n\
\tevent_param_t* event_param = event_params + prev_event;\n\
\tint count = get_param_val(myrank, ccounts, &(event_param->param[COUNT]));\n\
\tassert(MPI_Allreduce(sendbuf, recvbuf, count, datatype, op, comm) == MPI_SUCCESS);\n\
}\n\
");

  fprintf(skel_type_fh, "\
\n\nvoid do_reduce(void *sendbuf, void* recvbuf, char *ccounts, MPI_Datatype datatype,\n\
\t\t\t\tMPI_Op op, int root, MPI_Comm comm, int myrank){\n\
\tevent_param_t* event_param = event_params + prev_event;\n\
\tint count = get_param_val(myrank, ccounts, &(event_param->param[COUNT]));\n\
\tassert(MPI_Reduce(sendbuf, recvbuf, count, datatype, op, root, comm) == MPI_SUCCESS);\n\
}\n\
");
}


void generate_io_functions(FILE *skel_type_fh){
  fprintf(skel_type_fh,"\
\n\nvoid IoTimeStart(){\n\
\tgettimeofday(&begin, NULL);\n\
}\n\
");

  fprintf(skel_type_fh, "\
\n\nMPI_Datatype string_to_type(MPI_Datatype datatype){\n\
\treturn datatype;\n\
}\n\
");

  fprintf(skel_type_fh, "\
\n\nvoid compute_io_time(int myrank, char* iotime, event_param_t* event_param, int numrank){\n\
\tlong long io_time = get_param_val(myrank, iotime, &(event_param->param[TIME]));\n\
\tgettimeofday(&end, NULL);\n\
\tlong long sleeptime = (end.tv_sec-begin.tv_sec)*1000000+(end.tv_usec-begin.tv_usec);\n\
\tif((sleeptime = io_time-sleeptime+ioleft) > 0){\n\
\t\tusleep(sleeptime);\n\
\t\tioleft = 0;\n\
\t}else\n\
\t\tioleft = sleeptime;\n\
\tgettimeofday(&begin, NULL);\n\
}\n\
");

  fprintf(skel_type_fh, "\
\n\nvoid do_File_open(MPI_Comm comm, char *temp, char* a_mode, \n\
\t\t\t\tMPI_Info info, char *fh_index, int numrank, int myrank, char* iotime){\n\
\tevent_param_t* event_param = event_params + prev_event;\n\
\tchar filename[256];\n\
\tget_char_val(myrank, temp, &(event_param->param[FILENAME]), filename, numrank);\n\
\tint amode = get_param_val(myrank, a_mode, &(event_param->param[AMODE]));\n\
\tint fhindex = get_param_val(myrank, fh_index, &(event_param->param[FH])) - 1000;\n\
\tassert(MPI_File_open(comm, filename, amode, info, &fh[fhindex]) == MPI_SUCCESS);\n\
\tif(IOTIMER == 1)\n\
\t\tcompute_io_time(myrank, iotime, event_param, numrank);\n\
}\n\
");

fprintf(skel_type_fh, "\
\n\nvoid do_File_close(char *fh_index, int myrank, char* iotime, int numrank){\n\
\tevent_param_t* event_param = event_params + prev_event; \n\
\tint fhindex = get_param_val(myrank, fh_index, &(event_param->param[FH])) - 1000;\n\
\tassert(MPI_File_close(&fh[fhindex]) == MPI_SUCCESS);\n\
\tif(IOTIMER == 1)\n\
\t\tcompute_io_time(myrank, iotime, event_param, numrank);\n\
}\n\
");


fprintf(skel_type_fh, "\
\n\nvoid do_mpi_File_seek(char *fh_index, char *ooffset, char *wwhence, int numrank, int myrank){\n\
\tevent_param_t* event_param = event_params + prev_event;\n\
\tint fhindex = get_param_val(myrank, fh_index, &(event_param->param[FH])) - 1000;\n\
\tint whence = get_param_val(myrank, wwhence, &(event_param->param[WHENCE]));\n\
\tint offset = get_param_offset(myrank, ooffset, &(event_param->param[OFFSET]), numrank);\n\
\tassert(MPI_File_seek(fh[fhindex], offset, whence) == MPI_SUCCESS);\n\
}\n\
");

  fprintf(skel_type_fh, "\
\n\nvoid do_posix_seek(char* fh_index, char* ooffset, char* wwhence, int myrank, int numrank){\n\
\tevent_param_t* event_param = event_params + prev_event;\n\
\tint fhindex = get_param_val(myrank, fh_index, &(event_param->param[POSIX_FD]))-1000;\n\
\tint whence = get_param_val(myrank, wwhence, &(event_param->param[WHENCE]));\n\
\tint offset = get_param_offset(myrank, ooffset, &(event_param->param[OFFSET]), numrank);\n\
\tassert(lseek(fd[fhindex], offset, whence)+1);\n\
}\n\
");

fprintf(skel_type_fh, "\
\n\nvoid do_File_read_write(void *buffer, char *fh_index, char *ccount, MPI_Datatype datatype, int op_code, MPI_Status *status, int myrank, req_handler_t req){\n\
\tevent_param_t* event_param = event_params + prev_event; \n\
\tint indexfh = get_param_val(myrank, fh_index, &(event_param->param[FH]))-1000;\n\
\tint count = get_param_val(myrank, ccount, &(event_param->param[COUNT]));\n\
\tint req_id;\n\
\tswitch(op_code){\n\
\t\tcase 0:\n\
\t\t\tassert(MPI_File_read(fh[indexfh], buffer, count, datatype, status) == MPI_SUCCESS);\n\
\t\t\tbreak;\n\
\t\tcase 1:\n\
\t\t\tassert(MPI_File_write(fh[indexfh], buffer, count, datatype, status) == MPI_SUCCESS);\n\
\t\t\tbreak;\n\
\t\tcase 2:\n\
\t\t\tassert(MPI_File_read_shared(fh[indexfh], buffer, count, datatype, status) == MPI_SUCCESS);\n\
\t\t\tbreak;\n\
\t\tcase 3:\n\
\t\t\tassert(MPI_File_write_shared(fh[indexfh], buffer, count, datatype, status) == MPI_SUCCESS);\n\
\t\t\tbreak;\n\
\t\tcase 4:\n\
\t\t\tassert(MPI_File_read_all(fh[indexfh], buffer, count, datatype, status) == MPI_SUCCESS);\n\
\t\t\tbreak;\n\
\t\tcase 5:\n\
\t\t\tassert(MPI_File_write_all(fh[indexfh], buffer, count, datatype, status) == MPI_SUCCESS);\n\
\t\t\tbreak;\n\
\t\tcase 6:\n\
\t\t\tassert(MPI_File_read_ordered(fh[indexfh], buffer, count, datatype, status) == MPI_SUCCESS);\n\
\t\t\tbreak;\n\
\t\tcase 7:\n\
\t\t\tassert(MPI_File_write_ordered(fh[indexfh], buffer, count, datatype, status) == MPI_SUCCESS);\n\
\t\t\tbreak;\n\
\t\tcase 8:\n\
\t\t\tassert(MPI_File_read_all_begin(fh[indexfh], buffer, count, datatype) == MPI_SUCCESS);\n\
\t\t\tbreak;\n\
\t\tcase 9:\n\
\t\t\tassert(MPI_File_write_all_begin(fh[indexfh], buffer, count, datatype) == MPI_SUCCESS);\n\
\t\t\tbreak;\n\
\t\tcase 10:\n\
\t\t\tassert(MPI_File_read_ordered_begin(fh[indexfh], buffer, count, datatype) == MPI_SUCCESS);\n\
\t\t\tbreak;\n\
\t\tcase 11:\n\
\t\t\tassert(MPI_File_write_ordered_begin(fh[indexfh], buffer, count, datatype) == MPI_SUCCESS);\n\
\t\t\tbreak;\n\
\t\tcase 12:\n\
\t\t\treq_id = add_request(req);\n\
\t\t\tassert(MPI_File_iread(fh[indexfh], buffer, count, datatype, &(req->req_buf[req_id])) == MPI_SUCCESS);\n\
\t\t\tbreak;\n\
\t\tcase 13:\n\
\t\t\treq_id = add_request(req);\n\
\t\t\tassert(MPI_File_iwrite(fh[indexfh], buffer, count, datatype, &(req->req_buf[req_id])) == MPI_SUCCESS);\n\
\t\t\tbreak;\n\
\t\tcase 14:\n\
\t\t\treq_id = add_request(req);\n\
\t\t\tassert(MPI_File_iread_shared(fh[indexfh], buffer, count, datatype, &(req->req_buf[req_id])) == MPI_SUCCESS);\n\
\t\t\tbreak;\n\
\t\tcase 15:\n\
\t\t\treq_id = add_request(req);\n\
\t\t\tassert(MPI_File_iwrite_shared(fh[indexfh], buffer, count, datatype, &(req->req_buf[req_id])) == MPI_SUCCESS);\n\
\t\t\tbreak;\n\
\t\tdefault:\n\
\t\t\tprintf(\"not a read or write function\\n\");\n\
\t\t\texit(0);\n\
\t}\n\
}\n\
");

  fprintf(skel_type_fh, "\
\n\nvoid do_File_read_write_at(void* buffer, char *fh_index, char *ccount, char *ooffset, char* d_type, int op, MPI_Status *status, int myrank, req_handler_t req, int numrank, MPI_Datatype da_type, int flag){\n\
\tevent_param_t* event_param = event_params + prev_event; \n\
\tint indexfh = get_param_val(myrank, fh_index, &(event_param->param[FH]))-1000;\n\
\tint count = get_param_val(myrank, ccount, &(event_param->param[COUNT]));\n\
\tint req_id;\n\
\tMPI_Datatype datatype;\n\
\tint offset = get_param_offset(myrank, ooffset, &(event_param->param[OFFSET]), numrank);\n\
\tif(flag == 0)\n\
\t\tdatatype = da_type;\n\
\telse{\n\
\t\tint d_index = get_param_val(myrank, d_type, &(event_param->param[NEWTYPE]))-1000;\n\
\t\tassert(d_index >= 0);\n\
\t\tdatatype = data_type[d_index];\n\
\t}\n\
\tswitch(op){\n\
\t\tcase 0:\n\
\t\t\tassert(MPI_File_read_at(fh[indexfh], offset, buffer, count, datatype, status) == MPI_SUCCESS);\n\
\t\t\tbreak;\n\
\t\tcase 1:\n\
\t\t\tassert(MPI_File_write_at(fh[indexfh], offset, buffer, count, datatype, status) == MPI_SUCCESS);\n\
\t\t\tbreak;\n\
\t\tcase 2:\n\
\t\t\tassert(MPI_File_read_at_all(fh[indexfh], offset, buffer, count, datatype, status) == MPI_SUCCESS);\n\
\t\t\tbreak;\n\
\t\tcase 3:\n\
\t\t\tassert(MPI_File_write_at_all(fh[indexfh], offset, buffer, count, datatype, status) == MPI_SUCCESS);\n\
\t\t\tbreak;\n\
\t\tcase 4:\n\
\t\t\tassert(MPI_File_read_at_all_begin(fh[indexfh], offset, buffer, count, datatype) == MPI_SUCCESS);\n\
\t\t\tbreak;\n\
\t\tcase 5:\n\
\t\t\tassert(MPI_File_write_at_all_begin(fh[indexfh], offset, buffer, count, datatype) == MPI_SUCCESS);\n\
\t\t\tbreak;\n\
\t\tcase 6:\n\
\t\t\treq_id = add_request(req);\n\
\t\t\tassert(MPI_File_iread_at(fh[indexfh], offset, buffer, count, datatype, &(req->req_buf[req_id])) == MPI_SUCCESS);\n\
\t\t\tbreak;\n\
\t\tcase 7:\n\
\t\t\treq_id = add_request(req);\n\
\t\t\tassert(MPI_File_iwrite_at(fh[indexfh], offset, buffer, count, datatype, &(req->req_buf[req_id])) == MPI_SUCCESS);\n\
\t\t\tbreak;\n\
\t\tdefault:\n\
\t\t\tprintf(\"not a read_write at function\\n\");\n\
\t\t\texit(0);\n\
\t}\n\
}\n\
");

  fprintf(skel_type_fh, "\
\n\nvoid do_File_read_write_end(void *buffer, char* fh_index, MPI_Status *status, int op, int myrank){\n\
\tevent_param_t* event_param = event_params + prev_event; \n\
\tint indexfh = get_param_val(myrank, fh_index, &(event_param->param[FH]))-1000;\n\
\tswitch(op){\n\
\t\tcase 0:\n\
\t\t\tassert(MPI_File_read_all_end(fh[indexfh], buffer, status) == MPI_SUCCESS);\n\
\t\t\tbreak;\n\
\t\tcase 1:\n\
\t\t\tassert(MPI_File_write_all_end(fh[indexfh], buffer, status) == MPI_SUCCESS);\n\
\t\t\tbreak;\n\
\t\tcase 2:\n\
\t\t\tassert(MPI_File_read_ordered_end(fh[indexfh], buffer, status) == MPI_SUCCESS);\n\
\t\t\tbreak;\n\
\t\tcase 3:\n\
\t\t\tassert(MPI_File_write_ordered_end(fh[indexfh], buffer, status) == MPI_SUCCESS);\n\
\t\t\tbreak;\n\
\t\tcase 4:\n\
\t\t\tassert(MPI_File_read_at_all_end(fh[indexfh], buffer, status) == MPI_SUCCESS);\n\
\t\t\tbreak;\n\
\t\tcase 5:\n\
\t\t\tassert(MPI_File_write_at_all_end(fh[indexfh], buffer, status) == MPI_SUCCESS);\n\
\t\t\tbreak;\n\
\t\tdefault:\n\
\t\t\tprintf(\"not a read_write end function\\n\");\n\
\t\t\texit(0);\n\
\t}\n\
}\n\
");

  fprintf(skel_type_fh, "\
\n\nvoid do_File_delete(char *temp, int myrank, int numrank){\n\
\tevent_param_t* event_param = event_params + prev_event;\n\
\tchar filename[256];\n\
\tget_char_val(myrank, temp, &(event_param->param[FILENAME]), filename, numrank);\n\
\tassert(MPI_File_delete(filename, MPI_INFO_NULL) == MPI_SUCCESS);\n\
}\n\
");

 /* fprintf(skel_type_fh, "\
\n\nvoid do_type_create_darray(char *ssize, char *rrank, char *ccount, char *gsizes, char *distribs, char *dargs, char *psizes, char *oorder, MPI_Datatype datatype, char *newtype){\n\
\tevent_param_t* event_param = event_params + prev_event;\n\
\t
");*/
  fprintf(skel_type_fh, "\
\n\nvoid do_type_contiguous(char *ccount, MPI_Datatype datatype, char *newtype, int myrank){\n\
\tevent_param_t* event_param = event_params + prev_event;\n\
\tint count = get_param_val(myrank, ccount, &(event_param->param[COUNT]));\n\
\tint new_type = get_param_val(myrank, newtype, &(event_param->param[NEWTYPE]))-1000;\n\
\tassert(MPI_Type_contiguous(count, datatype, &data_type[new_type]) == MPI_SUCCESS);\n\
}\n\
");
  fprintf(skel_type_fh, "\
\n\nvoid do_type_commit(char *newtype, int myrank){\n\
\tevent_param_t* event_param = event_params + prev_event;\n\
\tint new_type = get_param_val(myrank, newtype, &(event_param->param[NEWTYPE]))-1000;\n\
\tassert(MPI_Type_commit(&data_type[new_type]) == MPI_SUCCESS);\n\
}\n\
");

  fprintf(skel_type_fh, "\
\n\nvoid do_type_free(char *newtype, int myrank){\n\
\tevent_param_t* event_param = event_params + prev_event;\n\
\tint new_type = get_param_val(myrank, newtype, &(event_param->param[NEWTYPE]))-1000;\n\
\tassert(MPI_Type_free(&data_type[new_type]) == MPI_SUCCESS);\n\
\tdata_type[new_type] = MPI_DATATYPE_NULL;\n\
}\n\
");

  fprintf(skel_type_fh, "\
\n\nvoid do_posix_open(char *temp, char *fflag, char *ffd, int numrank, int myrank, int op, char* iotime){\n\
\tevent_param_t* event_param = event_params + prev_event;\n\
\tchar filename[256];\n\
\tget_char_val(myrank, temp, &(event_param->param[FILENAME]), filename, numrank);\n\
\tint flag = get_param_val(myrank, fflag, &(event_param->param[POSIX_FLAGS]));\n\
\tint index = get_param_val(myrank, ffd, &(event_param->param[POSIX_FD]))-1000;\n\
\tif(access(filename, 0) == 0){\n\
\t\tif(op == 0)\n\
\t\t\tfd[index] = open(filename, flag);\n\
\t\telse\n\
\t\t\tfd[index] = open64(filename, flag);\n\
\t} else {\n\
\t\tif(op == 0)\n\
\t\t\tfd[index] = open(filename, flag, S_IREAD|S_IWRITE);\n\
\t\telse\n\
\t\t\tfd[index] = open64(filename, flag, S_IREAD|S_IWRITE);\n\
\t}\n\
\tassert(fd[index]);\n\
\tif(IOTIMER == 1)\n\
\t\tcompute_io_time(myrank, iotime, event_param, numrank);\n\
}\n\
");

  fprintf(skel_type_fh, "\
\n\nvoid do_posix_close(char *ffd, int myrank, char* iotime, int numrank){\n\
\tevent_param_t* event_param = event_params + prev_event;\n\
\tint index = get_param_val(myrank, ffd, &(event_param->param[POSIX_FD]))-1000;\n\
\tassert(close(fd[index])+1);\n\
\tfd[index] = -1;\n\
\tif(IOTIMER == 1)\n\
\t\tcompute_io_time(myrank, iotime, event_param, numrank);\n\
}\n\
");

  fprintf(skel_type_fh, "\
\n\nvoid do_posix_read_write(char* ffd, void* buffer, char* ccount, int myrank, int op){\n\
\tevent_param_t* event_param = event_params + prev_event;\n\
\tint index = get_param_val(myrank, ffd, &(event_param->param[POSIX_FD]))-1000;\n\
\tint count = get_param_val(myrank, ccount, &(event_param->param[COUNT]));\n\
\tif(op == 0)\n\
\t\tassert(read(fd[index], buffer, count)+1);\n\
\telse\n\
\t\tassert(write(fd[index], buffer, count)+1);\n\
}\n\
");

  fprintf(skel_type_fh, "\
\n\nvoid do_fopen(char* temp, char* mmode, char* indexfh, int myrank, int numrank, char* iotime){\n\
\tevent_param_t* event_param = event_params + prev_event;\n\
\tchar filename[256];\n\
\tchar mode[16];\n\
\tget_char_val(myrank, temp, &(event_param->param[FILENAME]), filename, numrank);\n\
\tget_char_val(myrank, mmode, &(event_param->param[POSIX_MODE]), mode, numrank);\n\
\tint fh = get_param_val(myrank, indexfh, &(event_param->param[POSIX_FD]))-1000;\n\
\tassert((file_fh[fh] = fopen(filename, mode)) != NULL);\n\
\tif(IOTIMER == 1)\n\
\t\tcompute_io_time(myrank, iotime, event_param, numrank);\n\
}\n\
");

  fprintf(skel_type_fh, "\
\n\nvoid do_fread_fwrite(void* buffer, char* ssize, char* ccount, char* indexfh, int myrank, int op){\n\
\tevent_param_t* event_param = event_params + prev_event;\n\
\tint fh = get_param_val(myrank, indexfh, &(event_param->param[POSIX_FD]))-1000;\n\
\tint size = get_param_val(myrank, ssize, &(event_param->param[SIZE]));\n\
\tint count = get_param_val(myrank, ccount, &(event_param->param[COUNT]));\n\
\tif(op == 0)\n\
\t\tassert(fread(buffer, size, count, file_fh[fh])+1);\n\
\telse\n\
\t\tassert(fwrite(buffer, size, count, file_fh[fh])+1);\n\
}\n\
");

  fprintf(skel_type_fh, "\
\n\nvoid do_fclose(char* indexfh, int myrank, char* iotime, int numrank){\n\
\tevent_param_t* event_param = event_params + prev_event;\n\
\tint fh = get_param_val(myrank, indexfh, &(event_param->param[POSIX_FD]))-1000;\n\
\tassert(fclose(file_fh[fh])+1);\n\
\tfile_fh[fh] = NULL;\n\
\tif(IOTIMER == 1)\n\
\t\tcompute_io_time(myrank, iotime, event_param, numrank);\n\
}\n\
");

  fprintf(skel_type_fh, "\
\n\nvoid do_remove(char* temp, int myrank, int numrank){\n\
\tevent_param_t* event_param = event_params + prev_event;\n\
\tchar filename[256];\n\
\tget_char_val(myrank, temp, &(event_param->param[FILENAME]), filename, numrank);\n\
\tremove(filename);\n\
}\n\
");

}

void generate_param_functions(FILE *skel_type_fh){
  fprintf(skel_type_fh, "\
\n\nvoid init_params(){\n\
\tevent_params = (event_param_t*) malloc(sizeof(event_param_t) * MAX_NUM_EVENTS);\n\
\tloops = (param_t*) malloc(sizeof(param_t) * MAX_NUM_LOOPS);\n\
\tmems = (param_t*) malloc(sizeof(param_t) * MAX_NUM_LOOPS);\n\
\tassert(event_params != NULL);\n\
\tassert(loops != NULL);\n\
\tassert(mems != NULL);\n\
\tmemset(event_params, 0, sizeof(event_param_t) * MAX_NUM_EVENTS);\n\
\tmemset(loops, 0, sizeof(param_t) * MAX_NUM_LOOPS);\n\
\tmemset(mems, 0, sizeof(param_t) * MAX_NUM_LOOPS);\n\
\tcur_iter = (int *)malloc(sizeof(int) * MAX_LOOP_DEPTH);\n\
\titers_len = (int *)malloc(sizeof(int) * MAX_LOOP_DEPTH);\n\
\tcur_mems = (int *)malloc(sizeof(int) * MAX_LOOP_DEPTH);\n\
\tmems_len = (int *)malloc(sizeof(int) * MAX_LOOP_DEPTH);\n\
\tassert(cur_iter != NULL);\n\
\tassert(iters_len != NULL);\n\
\tassert(cur_mems != NULL);\n\
\tassert(mems_len != NULL);\n\
\tmemset(cur_iter, 0, sizeof(int) * MAX_LOOP_DEPTH);\n\
\tmemset(iters_len, 0, sizeof(int) * MAX_LOOP_DEPTH);\n\
\tmemset(cur_mems, 0, sizeof(int) * MAX_LOOP_DEPTH);\n\
\tmemset(mems_len, 0, sizeof(int) * MAX_LOOP_DEPTH);\n\
}\n\
");

  fprintf(skel_type_fh, "\
\n\nvoid free_params(){\n\
\tint idx = 0;\n\
\tfor(idx = 0; idx < MAX_NUM_EVENTS; idx++){\n\
\t\tint param_idx = 0;\n\
\t\tfor(param_idx = 0; param_idx < MAX_PARAM_TYPE; param_idx++){\n\
\t\t\tif(event_params[idx].param[param_idx].val)\n\
\t\t\t\tfree(event_params[idx].param[param_idx].val);\n\
\t\t}\n\
\t}\n\
\tfor(idx = 0; idx < MAX_NUM_LOOPS; idx++){\n\
\t\tif(loops[idx].val)\n\
\t\t\tfree(loops[idx].val);\n\
\t\tif(mems[idx].val)\n\
\t\t\tfree(mems[idx].val);\n\
\t}\n\
\tfree(event_params);\n\
\tfree(loops);\n\
\tfree(mems);\n\
\tfree(cur_iter);\n\
\tfree(iters_len);\n\
\tfree(cur_mems);\n\
\tfree(mems_len);\n\
}\n\
");

  fprintf(skel_type_fh, "\
\n\nint* get_param_val_list(int rank, char* val_str){\n\
\tchar *iter = val_str;\n\
\tchar *pos = NULL;\n\
\tif(strchr(iter, RL_SEPARATOR) == NULL){\n\
\t\tpos = iter;\n\
\t} else {\n\
\t\tpos = iter;\n\
\t\twhile(iter && (iter = strchr(iter, PARAM_ENTRY_SEPARATOR)) != NULL){\n\
\t\t\tchar* rl_pos = strchr(pos, RL_SEPARATOR) + 1;\n\
\t\t\tif(is_member(rank, rl_pos)){\n\
\t\t\t\tbreak;\n\
\t\t\t}\n\
\t\t\titer++;\n\
\t\t\tpos = iter;\n\
\t\t}\n\
\t\tif(iter == NULL)\n\
\t\t\treturn NULL;\n\
\t}\n\
\treturn parse_val_str(pos);\n\
}\n\
");

  fprintf(skel_type_fh, "\
\n\nvoid ranksort(int n, int *out){\n\
\tstruct intlist{\n\
\t\tint val;\n\
\t\tstruct intlist *next;\n\
\t};\n\
\tstruct intlist *head = (struct intlist*)malloc(sizeof(struct intlist));\n\
\tstruct intlist *current, *current1;\n\
\tint i,j;\n\
\tdouble tem = 2.0;\n\
\thead -> val = 0;\n\
\thead -> next = NULL;\n\
\tint layer = (int)(ceil(log((double)(n+1))/log(tem))-1);\n\
\tif(layer > 0){\n\
\t\tfor(i=0; i<layer; i++){\n\
\t\t\tfor(j=(int)(pow(tem, i)-1); j<(int)(pow(tem, (i+1))-1); j++){\n\
\t\t\t\tcurrent = (struct intlist*)malloc(sizeof(struct intlist));\n\
\t\t\t\tcurrent1 = (struct intlist*)malloc(sizeof(struct intlist));\n\
\t\t\t\tstruct intlist *temp = head;\n\
\t\t\t\twhile(temp->val != j)\n\
\t\t\t\t\ttemp = temp->next;\n\
\t\t\t\tif(j+pow(tem, i)<n){\n\
\t\t\t\t\tcurrent->val = (int)(j+pow(tem,i));\n\
\t\t\t\t\tcurrent->next = temp->next;\n\
\t\t\t\t\ttemp->next = current;\n\
\t\t\t\t\tif(j+pow(tem,(i+1))<n){\n\
\t\t\t\t\t\tcurrent1->val = (int)(j+pow(tem,(i+1)));\n\
\t\t\t\t\t\tcurrent1->next = current->next;\n\
\t\t\t\t\t\tcurrent->next = current1;\n\
\t\t\t\t\t}\n\
\t\t\t\t}\n\
\t\t\t}\n\
\t\t}\n\
\t}\n\
\tstruct intlist *temp = head;\n\
\tfor(i=0; i<n; i++){\n\
\t\tout[i] = temp->val;\n\
\t\tstruct intlist *dele = temp;\n\
\t\tif(i != n-1)\n\
\t\t\ttemp = temp->next;\n\
\t\tfree(dele);\n\
\t}\n\
\treturn;\n\
}\n\
");

  fprintf(skel_type_fh, "\
\n\nvoid Dele(char *name, char *posi, int num){\n\
\tassert(posi >= name && posi < name+strlen(name));\n\
\tchar *temp = posi+num;\n\
\twhile(*temp != '\\0'){\n\
\t\t*posi = *temp;\n\
\t\tposi += 1;\n\
\t\ttemp = posi+num;\n\
\t}\n\
\t*posi = *temp;\n\
\treturn;\n\
}\n\
");

  fprintf(skel_type_fh, "\
\n\nchar *str_for_rank(int rank, char* buf, int position, int Rstart){\n\
\tchar *temp1 = (char*)malloc(strlen(buf)+1);\n\
\tstrcpy(temp1, buf);\n\
\tchar *end = (strstr(temp1, \"  \") == NULL) ? NULL : strstr(temp1, \"  \");\n\
\tif(end != NULL)\n\
\t\t*end = '\\0';\n\
\tif(strchr(buf, '(') == NULL)\n\
\t\treturn temp1;\n\
\tchar *temp_init = (char*)malloc(strlen(buf)+1), *start, *temp;\n\
\twhile(strchr(temp1, '(') != NULL){\n\
\t\tstart = strchr(temp1, '(');\n\
\t\tend = strchr(temp1, ')');\n\
\t\ttemp = temp_init;\n\
\t\tstrncpy(temp, start+1, (end - start - 1));\n\
\t\ttemp[end-start-1] = '\\0';\n\
\t\tchar num[8], *cur;\n\
\t\tint record[4], i=0;\n\
\t\tif(strstr(temp, \"-1\") != NULL){\n\
\t\t\tchar half[100];\n\
\t\t\tstrncpy(half, temp1, start-temp1);\n\
\t\t\thalf[start-temp1] = '\\0';\n\
\t\t\twhile((cur=strchr(temp,' ')) != NULL){\n\
\t\t\t\tstrncpy(num, temp, cur-temp);\n\
\t\t\t\tnum[cur-temp] = '\\0';\n\
\t\t\t\trecord[i] = atoi(num);\n\
\t\t\t\ti++;\n\
\t\t\t\ttemp=cur+1;\n\
\t\t\t}\n\
\t\t\trecord[3] = atoi(temp);\n\
\t\t\tint number = (rank-Rstart)*record[3]+record[1];\n\
\t\t\tsprintf(temp1, \"%%s%%d%%s\", half, number, end+1);\n\
\t\t}\n\
\t\telse if(strchr(temp, ' ') != NULL){\n\
\t\t\tint record = 1;\n\
\t\t\tchar *search, *search1 = NULL;\n\
\t\t\tfor(search=start+1; search<end; search++){\n\
\t\t\t\tif(record == position+1){\n\
\t\t\t\t\tif(search1 == NULL)\n\
\t\t\t\t\t\tsearch1 = search;\n\
\t\t\t\t\tif(!(*(search+1)<='9' && *(search+1)>='0'))\n\
\t\t\t\t\t\tbreak;\n\
\t\t\t\t}\n\
\t\t\t\tif(*search == ' ')\n\
\t\t\t\t\trecord += 1;\n\
\t\t\t}\n\
\t\t\tif(record != position){\n\
\t\t\t\tprintf(\"error: not a valid position\\n\");\n\
\t\t\t\texit(0);\n\
\t\t\t}\n\
\t\t\tDele(temp1, search+1, end-search);\n\
\t\t\tDele(temp1, start, search1-start);\n\
\t\t}\n\
\t\telse{\n\
\t\t\tDele(temp1, end, 1);\n\
\t\t\tDele(temp1, start, 1);\n\
\t\t}\n\
\t}\n\
\tfree(temp_init);\n\
\treturn temp1;\n\
}\n\
");

  fprintf(skel_type_fh, "\
\n\nchar *str_list_rank(int rank, char* buf, int position, int Rstart){\n\
\tchar *end = (strchr(buf, RL_SEPARATOR) != NULL) ? strchr(buf, RL_SEPARATOR) : strchr(buf, PARAM_ENTRY_SEPARATOR);\n\
\tchar *name1 = (char*)malloc((end-buf)*sizeof(char)+1);\n\
\tchar *name = (char*)malloc((end-buf)*sizeof(char)+1);\n\
\tstrncpy(name1, buf, end-buf);\n\
\tname1[end-buf] = '\\0';\n\
\tchar *temname = name1;\n\
\tchar *siglename = str_for_rank(rank, temname, position, Rstart);\n\
\tstrcpy(name, siglename);\n\
\tfree(siglename);\n\
\ttemname = strstr(temname, \"  \")==NULL ? NULL : strstr(temname, \"  \")+2;\n\
\tif(temname != NULL)\n\
\t\tstrcat(name, \"  \");\n\
\telse{\n\
\t\tfree(name1);\n\
\t\treturn name;\n\
\t}\n\
\twhile((strstr(temname, \"  \")) != NULL){\n\
\t\tsiglename = str_for_rank(rank, temname, position, Rstart);\n\
\t\tstrcat(name, siglename);\n\
\t\tstrcat(name, \"  \");\n\
\t\tfree(siglename);\n\
\t\ttemname = strstr(temname, \"  \")+2;\n\
\t}\n\
\tsiglename = str_for_rank(rank, temname, position, Rstart);\n\
\tstrcat(name, siglename);\n\
\tfree(siglename);\n\
\tfree(name1);\n\
\treturn name;\n\
}\n\
");

  fprintf(skel_type_fh, "\
\n\nchar *parse_char_str(int rank, char *buf, int numrank){\n\
\tint *rank_sort = (int*)malloc(numrank*sizeof(int));\n\
\tint i, position = 0;\n\
\tif(strchr(buf, RL_SEPARATOR) == NULL){\n\
\t\tranksort(numrank, rank_sort);\n\
\t\tfor(i=0; i<numrank; i++){\n\
\t\t\tif(rank_sort[i] == rank){\n\
\t\t\t\tposition = i+1;\n\
\t\t\t\tbreak;\n\
\t\t\t}\n\
\t\t}\n\
\t\tfree(rank_sort);\n\
\t\treturn str_list_rank(rank, buf, position, 0);\n\
\t}\n\
\tchar *end = strchr(buf, RL_SEPARATOR);\n\
\tchar *charlist = (char*)malloc((end-buf)*sizeof(char));\n\
\tchar *temp = end+1,*cur;\n\
\tint Rstart, Rnum, Rstrid;\n\
\tchar numstore[8];\n\
\tfor(i=0; i<4; i++){\n\
\t\tcur = strchr(temp, ' ');\n\
\t\tif(i == 2){\n\
\t\t\tstrncpy(numstore, temp, cur-temp);\n\
\t\t\tnumstore[cur-temp] = '\\0';\n\
\t\t\tRstart = atoi(numstore);\n\
\t\t\ttemp = cur + 1;\n\
\t\t}else if(i == 3){\n\
\t\t\tstrncpy(numstore, temp, cur-temp);\n\
\t\t\tnumstore[cur-temp] = '\\0';\n\
\t\t\tRnum = atoi(numstore);\n\
\t\t\ttemp = cur + 1;\n\
\t\t\tcur = strchr(temp, PARAM_ENTRY_SEPARATOR);\n\
\t\t\tstrncpy(numstore, temp, cur-temp);\n\
\t\t\tnumstore[cur-temp] = '\\0';\n\
\t\t\tRstrid = atoi(numstore);\n\
\t\t}else{\n\
\t\t\ttemp = cur + 1;\n\
\t\t}\n\
\t}\n\
\tranksort(Rstart+(Rnum-1)*Rstrid+1, rank_sort);\n\
\tfor(i=0; i<=Rstart+(Rnum-1)*Rstrid; i++){\n\
\t\tif(rank_sort[i] >= Rstart)\n\
\t\t\tposition += 1;\n\
\t\tif(rank_sort[i] == rank)\n\
\t\t\tbreak;\n\
\t}\n\
\tfree(rank_sort);\n\
\treturn str_list_rank(rank, buf, position, Rstart);\n\
}\n\
");

  fprintf(skel_type_fh, "\
\n\nchar* get_char_val_list(int rank, char* val_str, int numrank){\n\
\tchar *iter = val_str;\n\
\tchar *pos = NULL;\n\
\tif(strchr(iter, RL_SEPARATOR) == NULL){\n\
\t\tpos = strchr(iter, ':') + 1;\n\
\t} else{\n\
\t\tpos = strchr(iter,':') + 1;\n\
\t\twhile(iter && (iter = strchr(iter, PARAM_ENTRY_SEPARATOR)) != NULL){\n\
\t\t\tchar* rl_pos = strchr(pos, RL_SEPARATOR) + 1;\n\
\t\t\tif(is_member(rank, rl_pos)){\n\
\t\t\t\tbreak;\n\
\t\t\t}\n\
\t\t\titer = strchr(iter, ':') + 1;\n\
\t\t\tpos = iter;\n\
\t\t}\n\
\t\tif(iter == NULL)\n\
\t\t\treturn NULL;\n\
\t}\n\
\treturn parse_char_str(rank, pos, numrank);\n\
}\n\
");

  fprintf(skel_type_fh, "\
\n\nint get_param_val(int rank, char* val_str, param_t* param){\n\
\tint val = -1;\n\
\tif(param->val == NULL){\n\
\t\tparam->val = get_param_val_list(rank, val_str);\n\
\t}\n\
\tif(param->val){\n\
\t\tval = param->val[param->iter %% param->val[0] + 1];\n\
\t\tparam->iter++;\n\
\t}\n\
\treturn val;\n\
}\n\
");


fprintf(skel_type_fh, "\
\n\nint get_param_offset(int rank, char* val_str, param_t* param, int numrank){\n\
\tint start1, end1, i;\n\
\tchar num[10], *start=NULL, *end=NULL, *temp;\n\
\tif(strchr(val_str, RL_SEPARATOR) != NULL)\n\
\t\treturn get_param_val(rank, val_str, param);\n\
\tif((temp = strstr(val_str, \"-1\")) != NULL){\n\
\t\tstart = strchr(temp, ' ') + 1;\n\
\t\tend = strchr(start, ' ') - 1;\n\
\t\tstrncpy(num, start, end-start+1);\n\
\t\tnum[end-start+1] = '\\0';\n\
\t\tstart1 = atoi(num);\n\
\t\tstart = strchr(end, ' ') + 1;\n\
\t\tend = strchr(start, ' ') - 1;\n\
\t\tstrncpy(num, start, end-start+1);\n\
\t\tnum[end-start+1] = '\\0';\n\
\t\tend1 = atoi(num);\n\
\t\treturn (start1 + rank*end1);\n\
\t}\n\
\tif((temp = strstr(val_str, \"-2\")) != NULL){\n\
\t\tint *rank_sort = (int*)malloc(numrank*sizeof(int));\n\
\t\tranksort(numrank, rank_sort);\n\
\t\tfor(i=0; i<numrank; i++){\n\
\t\t\tif(rank_sort[i] == rank){\n\
\t\t\t\tstart1 = i+1;\n\
\t\t\t\tbreak;\n\
\t\t\t}\n\
\t\t}\n\
\t\tend1 = 0;\n\
\t\twhile(temp != NULL){\n\
\t\t\tif(end1 == start1){\n\
\t\t\t\tstart = temp + 1;\n\
\t\t\t\tend = (strchr(start, ' ')!=NULL)?(strchr(start, ' ')-1):(strchr(start, PARAM_ENTRY_SEPARATOR)-1);\n\
\t\t\t}\n\
\t\t\ttemp = strchr(temp+1, ' ');\n\
\t\t\tend1 += 1;\n\
\t\t}\n\
\t\tstrncpy(num, start, end-start+1);\n\
\t\tnum[end-start+1] = '\\0';\n\
\t\tfree(rank_sort);\n\
\t\treturn (atoi(num));\n\
\t}\n\
}\n\
");

  fprintf(skel_type_fh, "\
\n\nvoid get_char_val(int rank, char* val_str, param_t* param, char *name, int numrank){\n\
\tif(param->charval == NULL){\n\
\t\tparam->charval = get_char_val_list(rank, val_str, numrank);\n\
\t}\n\
\tif(param->charval){\n\
\t\tint record = 1, flag = 0;\n\
\t\tchar *temp = param->charval, *end;\n\
\t\twhile(strstr(temp, \"  \") != NULL){\n\
\t\t\trecord += 1;\n\
\t\t\ttemp = strstr(temp, \"  \") + 2;\n\
\t\t}\n\
\t\ttemp = param->charval;\n\
\t\tint position = param->iter %% record;\n\
\t\trecord = 0;\n\
\t\twhile((end = strstr(temp, \"  \")) != NULL){\n\
\t\t\tif(record == position){\n\
\t\t\t\tstrncpy(name, temp, end-temp);\n\
\t\t\t\tname[end-temp] = '\\0';\n\
\t\t\t\tflag = 1;\n\
\t\t\t\tbreak;\n\
\t\t\t}\n\
\t\t\telse{\n\
\t\t\t\trecord += 1;\n\
\t\t\t\ttemp = end + 2;\n\
\t\t\t}\n\
\t\t}\n\
\t\tif(flag == 0)\n\
\t\t\tstrcpy(name, temp);\n\
\t\tparam->iter++;\n\
\t}\n\
}\n\
");

/*
 *1. Returns iteration length if loop need to be executed now.
 *2. Returns zero if all members of any outer loop is executed except for their last iteration.
 *3. Returns one for ranks that are not part of loop.
 *4. Incase of dummy loops(iteration length = 0), member counts are updated.
 * */
  fprintf(skel_type_fh, "\
\n\nint get_iters_length(int rank, char* val_str, int loop_idx, int loop_depth){\n\
\tint idx = 0;\n\
\tfor(idx = 0; idx < (loop_depth - 1); idx++){\n\
\t\tif(cur_mems[idx] >= mems_len[idx] &&\n\
\t\tcur_iter[idx] != iters_len[idx] - 1){\n\
\t\t\treturn 0;\n\
\t\t}\n\
\t}\n\
\tint iter = get_param_val(rank, val_str, &loops[loop_idx]);\n\
\tif(iter == 0){\n\
\t\tfor(idx = 0; idx < loop_depth - 1; idx++){\n\
\t\t\tcur_mems[idx] += mems_len[loop_depth - 1];\n\
\t\t}\n\
\t}\n\
\treturn (iter >= 0) ? iter : 1;\n\
}\n\
");

/*
 *1. Returns members count if loop need to be executed now.
 *2. Returns zero if all members of any outer loop is executed except for their last iteration.
 *3. Returns zero for ranks that are not part of loop.
 * */
  fprintf(skel_type_fh, "\
\n\nint get_mems_length(int rank, char* val_str, int loop_idx, int loop_depth){\n\
\tint idx = 0;\n\
\tfor(idx = 0; idx < (loop_depth - 1); idx++){\n\
\t\tif(cur_mems[idx] >= mems_len[idx] &&\n\
\t\tcur_iter[idx] != iters_len[idx] - 1){\n\
\t\t\treturn 0;\n\
\t\t}\n\
\t}\n\
\tint mem = get_param_val(rank, val_str, &mems[loop_idx]);\n\
\treturn (mem > 0) ? mem : 0;\n\
}\n\
");
}
