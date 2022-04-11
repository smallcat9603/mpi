#ifndef OP_GENERATOR_H
#define OP_GENERATOR_H

//#include "rsd_queue.h"
#include "mpi.h"
#include "code_generator_handler.h"
#include "Event.h"

#define REQ_LIST_SIZE 1000

/*
 * The section below takes care of printing out
 * the defaults types, comms, ops in the generated code.
 */
typedef struct _mpi_type_t {
  const MPI_Datatype type;
  const char *const type_name;
} mpi_type_t;

static mpi_type_t MPI_DEFINED_DATATYPES[] = {

  /* Elementary datatypes (C) */
  {MPI_CHAR, "MPI_CHAR"},
  {MPI_SHORT, "MPI_SHORT"},
  {MPI_INT, "MPI_INT"},
  {MPI_LONG, "MPI_LONG"},
  {MPI_UNSIGNED_CHAR, "MPI_UNSIGNED_CHAR"},
  {MPI_UNSIGNED_SHORT, "MPI_UNSIGNED_SHORT"},
  {MPI_UNSIGNED, "MPI_UNSIGNED"},
  {MPI_UNSIGNED_LONG, "MPI_UNSIGNED_LONG"},
  {MPI_FLOAT, "MPI_FLOAT"},
  {MPI_DOUBLE, "MPI_DOUBLE"},
  {MPI_LONG_DOUBLE, "MPI_LONG_DOUBLE"},
  {MPI_BYTE, "MPI_BYTE"},
  {MPI_PACKED, "MPI_PACKED"},

  /* Elementary datatypes (Fortran) */
  {MPI_INTEGER, "MPI_INTEGER"},
  {MPI_REAL, "MPI_REAL"},
  {MPI_DOUBLE_PRECISION, "MPI_DOUBLE_PRECISION"},
  {MPI_COMPLEX, "MPI_COMPLEX"},
  {MPI_DOUBLE_COMPLEX, "MPI_DOUBLE_COMPLEX"},
  {MPI_LOGICAL, "MPI_LOGICAL"},
  {MPI_CHARACTER, "MPI_CHARACTER"},
  /* MPI_BYTE   is mentioned here but covered by C. */
  /* MPI_PACKED is mentioned here but covered by C. */

  /* Datatypes for reduction functions (C) */
  {MPI_FLOAT_INT, "MPI_FLOAT_INT"},
  {MPI_DOUBLE_INT, "MPI_DOUBLE_INT"},
  {MPI_LONG_INT, "MPI_LONG_INT"},
  {MPI_2INT, "MPI_2INT"},
  {MPI_SHORT_INT, "MPI_SHORT_INT"},
  {MPI_LONG_DOUBLE_INT, "MPI_LONG_DOUBLE_INT"},

  /* Datatypes for reduction functions (Fortran) */
  {MPI_2REAL, "MPI_2REAL"},
  {MPI_2DOUBLE_PRECISION, "MPI_2DOUBLE_PRECISION"},
  {MPI_2INTEGER, "MPI_2INTEGER"},

  /* Optional datatypes (Fortran) */
#ifdef MPI_INTEGER1
  {MPI_INTEGER1, "MPI_INTEGER1"},
#endif
#ifdef MPI_INTEGER2
  {MPI_INTEGER2, "MPI_INTEGER2"},
#endif
#ifdef MPI_INTEGER4
  {MPI_INTEGER4, "MPI_INTEGER4"},
#endif
#ifdef MPI_INTEGER8
  {MPI_INTEGER8, "MPI_INTEGER8"},  /* not mentioned in the standard */
#endif
#ifdef MPI_INTEGER16
  {MPI_INTEGER16, "MPI_INTEGER16"},  /* not mentioned in the standard */
#endif

#ifdef MPI_REAL2
  {MPI_REAL2, "MPI_REAL2"},
#endif
#ifdef MPI_REAL4
  {MPI_REAL4, "MPI_REAL4"},
#endif
#ifdef MPI_REAL8
  {MPI_REAL8, "MPI_REAL8"},
#endif
#ifdef MPI_REAL16
  {MPI_REAL16, "MPI_REAL16"},  /* not mentioned in the standard. */
#endif

#ifdef MPI_COMPLEX8
  {MPI_COMPLEX8, "MPI_COMPLEX8"},  /* not mentioned in the standard */
#endif
#ifdef MPI_COMPLEX16
  {MPI_COMPLEX16, "MPI_COMPLEX16"},  /* not mentioned in the standard */
#endif
#ifdef MPI_COMPLEX32
  {MPI_COMPLEX32, "MPI_COMPLEX32"},  /* not mentioned in the standard */
#endif

  /* Optional datatypes (C) */
#ifdef MPI_LONG_LONG_INT
  {MPI_LONG_LONG_INT, "MPI_LONG_LONG_INT"},
#endif
#ifdef MPI_LONG_LONG
  {MPI_LONG_LONG, "MPI_LONG_LONG"},  /* Not really in MPI-1, but allowed as a synonym
                                              for MPI_LONG_LONG_INT in most implementations. */
#endif

  /* Special datatypes for constructing derived datatypes */
  {MPI_UB, "MPI_UB"},
  {MPI_LB, "MPI_LB"},

  /* Old versions of OpenMPI had #defines but not definitions
     for these.  Should work with newer versions, but if you
     must compile with oldv ersions, you can define
     SCALATRACE_OMIT_OMPI_UNDEFINED_TYPES at build time.       */
#ifndef SCALATRACE_OMIT_OMPI_UNDEFINED_TYPES
#ifdef MPI_2COMPLEX
  {MPI_2COMPLEX, "MPI_2COMPLEX"},
#endif
#ifdef MPI_2DOUBLE_COMPLEX
  {MPI_2DOUBLE_COMPLEX, "MPI_2DOUBLE_COMPLEX"},
#endif
#endif /* SCALATRACE_OMIT_OMPI_UNDEFINED_TYPES */

  /* ---------------------------------------------------- */
  /* MPI-2 datatypes.                                     */
  /* ---------------------------------------------------- */
#if (MPI_VERSION >= 2)
  /* Named Predefined Datatypes */
  {MPI_WCHAR, "MPI_WCHAR"},

  /* Optional C and C++ (no fortran) Named Predefined Datatypes */
#ifdef MPI_UNSIGNED_LONG_LONG
  {MPI_UNSIGNED_LONG_LONG, "MPI_UNSIGNED_LONG_LONG"},
#endif
#ifdef MPI_SIGNED_CHAR
  {MPI_SIGNED_CHAR, "MPI_SIGNED_CHAR"},
#endif
#endif /* (MPI_VERSION >= 2) */
  /* ---------------------------------------------------- */
  /* MPI-1 datatypes.                                     */
  /* ---------------------------------------------------- */
  {MPI_DATATYPE_NULL, "MPI_DATATYPE_NULL"}
};

typedef struct _mpi_ops_t {
  const MPI_Op op;
  const char *const op_name;
} mpi_ops_t;

static mpi_ops_t MPI_DEFINED_OPS[] = {

  {MPI_MAX, "MPI_MAX"},
  {MPI_MIN, "MPI_MIN"},
  {MPI_SUM, "MPI_SUM"},
  {MPI_PROD, "MPI_PROD"},
  {MPI_MAXLOC, "MPI_MAXLOC"},
  {MPI_MINLOC, "MPI_MINLOC"},
  {MPI_BAND, "MPI_BAND"},
  {MPI_BOR, "MPI_BOR"},
  {MPI_BXOR, "MPI_BXOR"},
  {MPI_LAND, "MPI_LAND"},
  {MPI_LOR, "MPI_LOR"},
  {MPI_LXOR, "MPI_LXOR"},

  /* ---------------------------------------------------- */
  /* MPI-1 Ops.                                           */
  /* ---------------------------------------------------- */
#if (MPI_VERSION >= 2)
  {MPI_REPLACE, "MPI_REPLACE"},
#endif // (MPI_VERSION >= 2)

  /* ---------------------------------------------------- */
  /* MPI-1 Ops.                                           */
  /* ---------------------------------------------------- */
  {MPI_OP_NULL, "MPI_OP_NULL"}
};

typedef struct _mpi_comms_t {
  const MPI_Comm comm;
  const char *const comm_name;
} mpi_comms_t;

static mpi_comms_t MPI_DEFINED_COMMS[] = {
  {MPI_COMM_SELF, "MPI_COMM_SELF"},
  {MPI_COMM_WORLD, "MPI_COMM_WORLD"},
  {MPI_COMM_NULL, "MPI_COMM_NULL"}
};


/*This structure is used to hold the differnt count groups.*/
typedef struct {
        int value;
        int *nodes;
} count_grp_t;

void generate_event_code(Event *event, int depth);
char* construct_value_string(vector<ValueSet *>* event_param_values,
                                       bool gen_rl_cond_everytime);
char* construct_param_string(Event* event, int paramtype);
#endif /* OP_GENERATOR_H */
