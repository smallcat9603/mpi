/*
 * rpms: libunwind-devel
 * cc -o unwind unwind.c -lunwind
 * ./unwind
 *     *   outputs ip (PC) and sp of all frames:
 *      *     show_backtrace, bar, foo, main, start
 *       *
 *        * on arc and ppc64:
 *         * tar xzf libunwind-1.1.tar.gz
 *          * cd libunwind-1.1
 *           * ./configure
 *            * make
 *             * cd ..
 *              * cc -o unwind unwind.c -Ilibunwind-1.1/include/ libunwind-1.1/src/.libs/libunwind.a
 *               */

#define UNW_LOCAL_ONLY
#include <libunwind.h>
#include <stdio.h>
#include "walker.h"

#define UP_SKIP_DEPTH  0

intptr_t* do_stack_unwind (void) {
	unw_cursor_t cursor; unw_context_t uc;	
	unw_word_t ip, sp, eh;

	unw_getcontext(&uc);
	unw_init_local(&cursor, &uc);
	int i=0, j=0;
	while (unw_step(&cursor) > 0) {
		unw_get_reg(&cursor, UNW_REG_IP, &ip);
		if(i>=SKIP_DEPTH)
			pc_array[i-SKIP_DEPTH] = (intptr_t)ip;
		i++;
	}
	int skip_depth = 0;
	for(j=i-2-skip_depth; j>i-2-UP_SKIP_DEPTH-SKIP_DEPTH; j--){
		pc_array[j] = (intptr_t) 0;
	}
	return pc_array;
}

void do_stack_free(intptr_t *t)
{}

