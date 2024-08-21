#include "scanner.h"

//FSA Table
int fsaTable[9][28] = 
	{{1, 1, 2, -5, 3, 4, 4, -5, 6, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 7, 8, 0, 1005, -1},
	 {1, 1, 1, 1, 1001, 1001, 1001, 1001, 1001, 1001, 1001, 1001, 1001, 1001, 1001, 1001, 1001, 1001, 1001, 1001, 1001, 1001, 1001, 1001, 1001, 1001, 1001, -1},
	{1002, 1002, 2, 1002, 1002, 1002, 1002, 1002, 1002, 1002, 1002, 1002, 1002, 1002, 1002, 1002, 1002, 1002, 1002, 1002, 1002, 1002, 1002, 1002, 1002, 1002, 1002, -1},
	{1003, 1003, 1003, 1003, 4, 1003, 1003, 5, 1003, 1003, 1003, 1003, 1003, 1003, 1003, 1003, 1003, 1003, 1003, 1003, 1003, 1003, 1003, 1003, 1003, 1003, 1003, -1},
	{1003, 1003, 1003, 1003, 1003, 1003, 1003, 1003, 1003, 1003, 1003, 1003, 1003, 1003, 1003, 1003, 1003, 1003, 1003, 1003, 1003, 1003, 1003, 1003, 1003, 1003, 1003, -1},
	{-2, -2, -2, -2, 4, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -1},
	{1003, 1003, 1003, 1003, 4, 1003, 1003, 1003, 1003, 1003, 1003, 1003, 1003, 1003, 1003, 1003, 1003, 1003, 1003, 1003, 1003, 1003, 1003, 1003, 1003, 1003, 1003, -1},
	{-3, -3, -3, -3, -3, -3, -3, -3, -3, -3, -3, -3, -3, -3, -3, -3, -3, -3, -3, -3, -3, -3, -3, 4, -3, -3, -3, -1},
	{-4, -4, -4, -4, -4, -4, -4, -4, -4, -4, -4, -4, -4, -4, -4, -4, -4, -4, -4, -4, -4, -4, -4, -4, 4, -4, -4, -1}};

//holds the string to be associated with the token
char S[10];
//holds the line and char number to be associated with the token
char lineChar[20];
//lists out the keywords for checking if token is keyword or identifier
char *keywords[17] = {"start", "stop", "while", "repeat", "until", "label", "return", "cin", "cout", "tape", "jump", "if", "then", "pick", "create", "set", "func"};
//char *tokenNames[5] = {"Identifier", "Number", "operator", "Keyword", "EOF"};

//Once called examines the provided file and outputs the next token
struct tokeType Driver(FILE *inputFilePointer)
{
	static int line = 1;
	static int character = 1;
	int tempLine;
	int tempChar;
	int nextChar = fgetc(inputFilePointer);
	int col;
	int flag = 1;
	int state = 0;
        int nextState = 0;
        struct tokeType token;
	int commentFlag = 0;
	//resets whats contained within the token string
	memset(S, '\0', 10);
	int start = 1;
	
	//while the token has not been finished
        while (state <= 1000)
        {
		if (!start)
		{
                	nextState = fsaTable[state][col];
		}
		//If error has occurred
                if (nextState < 0)
                {
			sprintf(lineChar, "%d.%d", tempLine, tempChar);
			//Invalid Character
                        if (nextState == -1)
                        {
                                fprintf(stderr, "Error: symbol not found in language used\n");
				exit(1);
                        }
			//Invalid character during =!= operator
			else if (nextState == -2)
			{
				fprintf(stderr, "Error: unexpected character in '=!=' expected '='\n");
				exit(1);
			}
			//Invalid character during || operator
			else if (nextState == -3)
                        {
                                fprintf(stderr, "Error: unexpected character in '||' expected another '|'\n");
                                exit(1);
                        }
			//Invalid character during && operator
			else if (nextState == -4)
                        {
                                fprintf(stderr, "Error: unexpected character in '&&' expected another '&'\n");
                                exit(1);
                        }
                }
		//If the roken is complete
                if (nextState >= 1000)
                {
			sprintf(lineChar, "%d.%d", tempLine, tempChar);
			
			if (nextChar != -1 && nextChar != 10)
			{
				fseek(inputFilePointer, -1, SEEK_CUR);
			}
			//if token is identifier
                        if (nextState == 1001)
                        {
				int i;
				//loop ensures identifier token is not a keyword
				for (i = 0; i < 17; i++)
				{
					if (strcmp(S, keywords[i]) == 0)
					{
						token.tokenID = 1004;
						token.tokenInstance = S;
						token.lineCharNum = lineChar;
						return token;
					}
				}
				//if not a keyword then return identifier
                                token.tokenID = 1001;
				token.tokenInstance = S;
				token.lineCharNum = lineChar;
				return token;
                        }
			//if not identifier token return as specified token
                        else
                        {
                        	token.tokenID = nextState;
				token.tokenInstance = S;
				if (nextState == 1005)
				{
					sprintf(lineChar, "%d.%d", line, character);
				}
				token.lineCharNum = lineChar;
				return token;
                        }
                }
		//if token is not complete/reached final state in FSA
                else
                {
			if (!start)
			{
                        	state = nextState;
                       		// strncat(S, (const char *) nextChar, 1);
                       		if (state != 0)
				{
					sprintf(S, "%s%c", S, (char)nextChar);
				}
				if (S[0] != 0 && S[1] == 0)
				{
					tempLine = line;
					tempChar = character;
				}
			}
			flag = 1;
			while (flag)
			{
				if (!start || nextChar == 10)
				{
					if (!start && !commentFlag)
					{
						character++;
					}
					commentFlag = 0;
					nextChar = fgetc(inputFilePointer);
				}
				if (nextChar != 10)
				{
					start = 0;
				}
				//If char is / check and ensure it is not a comment
				if (nextChar == 47)
				{
					if (nextChar == fgetc(inputFilePointer))
					{
						while (nextChar != 10 && nextChar != EOF)
						{
							nextChar = fgetc(inputFilePointer);
							character++;
						}
						line++;
						character = 1;
						col = 25;
						flag = 0;
						nextChar = 0;
						commentFlag = 1;
					}
					else
					{
						fseek(inputFilePointer, -1, SEEK_CUR);
						col = 12;
						flag = 0;
					}
				}
				//If char is \n
				else if (nextChar == 10)
				{
					line++;
					character = 1;
					col = 25;
					flag = 0;
					if (!start)
					{
						nextChar = 0;
					}
					commentFlag = 1;
				}
				//If char is whitespace
				else if (isspace(nextChar))
				{
					col = 25;
					flag = 0;
					nextChar = 0;
				}
				//If char is lowercase letter
				else if (islower(nextChar))
				{
					col = 0;
					flag = 0;
				}
				//If char is uppercase letter
				else if (isupper(nextChar))
				{
					col = 1;
					flag = 0;
				}
				//If char is digit
				else if (isdigit(nextChar))
				{
					col = 2;
					flag = 0;
				}
				//If char is EOF
				else if (nextChar == EOF)
				{
					col = 26;
					flag = 0;
				}
				//Else it is a operator or invalid character
				else
				{
					switch(nextChar)
					{
						// _
						case 95: col = 3; flag = 0; break;
						// =
						case 61: col = 4; flag = 0; break;
						// <
						case 60: col = 5; flag = 0; break;
						// >
						case 62: col = 6; flag = 0; break;
						// !
						case 33: col = 7; flag = 0; break;
						// :
						case 58: col = 8; flag = 0; break;
						// +
						case 43: col = 9; flag = 0; break;
						// -
						case 45: col = 10; flag = 0; break;
						// *
						case 42: col = 11; flag = 0; break;
						// /
						case 47: col = 12; flag = 0; break;
						// ^
						case 94: col = 13; flag = 0; break;
						// .
						case 46: col = 14; flag = 0; break;
						// (
						case 40: col = 15; flag = 0; break;
						// )
						case 41: col = 16; flag = 0; break;
						// ,
						case 44: col = 17; flag = 0; break;
						// {
						case 123: col = 18; flag = 0; break;
						// }
						case 125: col = 19; flag = 0; break;
						// ;
						case 59: col = 20; flag = 0; break;
						// [
						case 91: col = 21; flag = 0; break;
						// ]
						case 93: col = 22; flag = 0; break;
						// |
						case 124: col = 23; flag = 0; break;
						// &
						case 38: col = 24; flag = 0; break;
						// error
						default: col = 27; flag = 0; break;	
					}
				}
			}
                }
        }
	//If code breaks this will be returned
	fprintf(stderr, "Error: code encountered errror");
	token.tokenID = 1006;
	token.tokenInstance = "error";
	token.lineCharNum = lineChar;
	return token;
}

