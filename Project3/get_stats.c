#include <stdio.h>
#include "stats_wrapper.h"

int main(int argc, char* argv[]) {

	printf("allocated memory: %ld\n",slob_get_total_alloc_mem_wrapper());
	printf("free memory: %ld\n", slob_get_total_free_mem_wrapper());
	return 0;
}
