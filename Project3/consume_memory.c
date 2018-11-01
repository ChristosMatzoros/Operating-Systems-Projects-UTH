#include<stdio.h>
#include<stdlib.h>

#define MAX_ITERS 2000000

int main(int argc, char *argv[]){

	char *char_array;
	int i;
	
	for(i = 0; i < MAX_ITERS; i++){
		// allocate memory for an array of characters
		char_array = (char*)malloc(10*sizeof(char));
		if(char_array == NULL){
			printf("malloc failed to allocate memory\n");
		}
	}
	
	return 0;
}
