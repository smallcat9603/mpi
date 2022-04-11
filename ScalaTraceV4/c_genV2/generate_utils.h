#ifndef GENERATE_UTILS_H
#define GENERATE_UTILS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "code_generator_handler.h"
#include "op_generator.h"

void generate_main_c_header(FILE * skel_fh);
void generate_main_c_end(FILE * skel_fh);

void generate_util_h(FILE *skel_type_fh);
void generate_util_h_parse_once_params(FILE *skel_type_fh, int length);


void generate_util_c_header(FILE *skel_type_fh);

void generate_util_compute_time_function(FILE *skel_type_fh);

void generate_max_size_function(FILE* skel_type_fh, derived_type_dump_t* type);
void generate_is_member_function(FILE *skel_type_fh);
void generate_req_handler_function(FILE* skel_type_fh);
void generate_comm_handler_functions(FILE* skel_type_fh);

void generate_p2p_ops(FILE *skel_type_fh);
void generate_collectives(FILE *skel_type_fh);
void generate_param_functions(FILE *skel_type_fh);
void generate_io_functions(FILE *skel_type_fh);
#endif
