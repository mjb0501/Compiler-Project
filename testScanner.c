#include "testScanner.h"
//Array used to print out what type of token it is
char *tokenNames[5] = {"Identifier", "Number", "operator", "Keyword", "EOF"};

void testScanner(FILE *inputFilePointer)
{
	//Grab first token and print out
        struct tokeType current = Driver(inputFilePointer);
        printf("\n%s ", tokenNames[current.tokenID - 1001]);
        printf("%s ", current.tokenInstance);
        printf("%s\n", current.lineCharNum);
	//While an EOF token has not been produced
        while(current.tokenID != eofTK)
        {
                current = Driver(inputFilePointer);
                printf("%s ", tokenNames[current.tokenID - 1001]);
                printf("%s ", current.tokenInstance);
                printf("%s\n", current.lineCharNum);
        }
}
