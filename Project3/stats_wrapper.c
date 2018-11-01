#include <sys/syscall.h>
#include <unistd.h>
#include "stats_wrapper.h"

long slob_get_total_alloc_mem_wrapper(void){
	return( syscall(__NR_slob_get_total_alloc_mem));
}
long slob_get_total_free_mem_wrapper(void){
	return( syscall(__NR_slob_get_total_free_mem));
}

