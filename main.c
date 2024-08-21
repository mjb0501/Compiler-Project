#include "static.h"

int main(int argc, char** argv)
{
	FILE *inputFilePointer = NULL;
	FILE *outputFilePointer = NULL;
	int inputFileCharacter;
	char fileName[100];
	//If input file is provided
	if (argv[1] != NULL)
	{
		strcpy(fileName, argv[1]);
		strcat(fileName, ".asm");
		inputFilePointer = fopen(argv[1], "r");
		if (inputFilePointer == NULL)
		{
			fprintf(stderr, "Cannot open file provided");
			return 1;
		}
		printf("Reading from input file\n");

		fseek(inputFilePointer, 0L, SEEK_END);
	
		long int res = ftell(inputFilePointer);
		
		if (res <= 0)
		{
			fclose(inputFilePointer);
			fprintf(stderr, "File provided is empty\n");
			return 1;
		}

		rewind(inputFilePointer);
		outputFilePointer = fopen(fileName, "w");
	}
	//If input file is not provided
	else
	{
		inputFilePointer = fopen("tempFile.txt", "w");
		
		printf("If you did not redirect a file, enter the code and once done type ctrl + d.\n");

		if ((inputFileCharacter = fgetc(stdin)) != -1)
		{
			while (inputFileCharacter != EOF)
			{
				fputc(inputFileCharacter, inputFilePointer);
				inputFileCharacter = fgetc(stdin);
			}
		}

		fseek(inputFilePointer, 0L, SEEK_END);
		
		long int res = ftell(inputFilePointer);

		if (res <= 0)
		{
			fclose(inputFilePointer);
			fprintf(stderr, "File provided is empty\n");
			return 1;
		}

		rewind(inputFilePointer);

		fclose(inputFilePointer);
		inputFilePointer = fopen("tempFile.txt", "r");
		outputFilePointer = fopen("kb.asm", "a");
	}
	//prints out tokenized input
	struct node* root = parser(inputFilePointer);
	print(root, 1);
	recGen(root, outputFilePointer);
	//printf("Got Here\n");
	//print(root, 1);
	freeTree(root);
	return 0;
}













