#include <stdio.h>
 
int main(int argc, char *argv[]){
	FILE *fptr1, *fptr2;
	char c;

	// Open one file for reading
	fptr1 = fopen("input.txt", "r");
	if (fptr1 == NULL){
		printf("Cannot open file\n");
		return 0;
	}

	// Open another file for writing
	fptr2 = fopen("output.txt", "w");
	if (fptr2 == NULL){
		printf("Cannot open file\n");
		return 0;
	}

	// Read contents from the input file and copy them to the output file
	c = fgetc(fptr1);
	while (c != EOF){
		fputc(c, fptr2);
		c = fgetc(fptr1);
	}

	printf("File copied\n");

	fclose(fptr1);
	fclose(fptr2);
	
	return 0;
}
