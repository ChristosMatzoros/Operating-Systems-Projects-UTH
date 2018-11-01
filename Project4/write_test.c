#include <stdio.h>
#include <stdlib.h>
 
int main(int argc, char *argv[]){
	FILE *fptr;
	int i = 0;

	// Open one file for reading and writing
	fptr = fopen("input.txt", "ab+");
	if (fptr == NULL){
		printf("Cannot open file\n");
		return 0;
	}

	// Change the number of iterations to create a larger file
	for (i = 0; i < 500; ++i){
		fprintf(fptr, "%d", rand() % 10);
	}

	printf("Random file created\n");
	
	fclose(fptr);
	
	return 0;
}
