#include <stdio.h>
 
int main(int argc, char *argv[]){
	FILE *fptr1;
	char c;

	// Open one file for reading
	fptr1 = fopen("input.txt", "r");
	if (fptr1 == NULL){
		printf("Cannot open file\n");
		return 0;
	}

	// Read contents from file
	c = fgetc(fptr1);
	while (c != EOF){
		c = fgetc(fptr1);
	}

	printf("File read\n");

	fflush(fptr1);

	fclose(fptr1);
	
	return 0;
}
