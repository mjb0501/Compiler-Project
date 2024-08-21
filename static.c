#include "static.h"
//stack structure used in static
typedef struct 
{
	char* items[100];
	int top;
} Stack;

//checks if the stack is empty
int isEmpty(Stack* stack)
{
	return stack->top == -1;
}

//pushes item onto stack
void push(Stack* stack, char* item)
{
	if(stack->top == 100 -1)
	{
		printf("Stack Overflow\n");
		exit(1);
	}
	stack->items[++stack->top] = item;
}

//pops item from stack
void pop(Stack* stack)
{
	if (isEmpty(stack))
	{
		printf("Stack already empty\n");
		return;
	}
	
	stack->top--;
}

//searches stack for item anad returns its distance from top if found if not found then returns -1
int find(Stack* stack, char* item)
{
	int distance = 0;
	int i;
	for (i = stack->top; i >= 0; --i)
	{
		if (!strcmp(stack->items[i], item))
		{	
			//printf("comparing %s and %s\n", stack->items[i], item);
			//printf("%d\n", strcmp(stack->items[i], item));
			//printf("distance: %d\n", distance);
			return distance;
		}
		distance++;
	}
	return -1;
}

//counts the labels, counts the temp variables
static int LabelCntr = 0;
static int VarCntr = 0;
typedef enum {VAR, LABEL} nameType;
static char Name[20];

//generates new temp variables and labels and returns them
static char *newName(nameType what)
{
	if (what==VAR)
	{
		sprintf(Name, "T%d", VarCntr++);
	}
	else
	{
		sprintf(Name, "L%d", LabelCntr++);
	}
	return(Name);
}

//generates the code based on tree structure
void recGen(struct node* root, FILE *output)
{
	static int error = 0;
	char label[20], label2[20];
	static int blockCount;
	static int varCount[25];
	static int global;
	static char globalVars[500];
	static char funcName[20];
	static FILE * funcFile;
	//static char funcBlock[500];
	
	//initialize stack
	static Stack* stack = NULL;
	if (stack == NULL)
	{
		stack = (Stack*)malloc(sizeof(Stack));
		stack->top = -1;
	}
	
	//if node is null return
	if (root==NULL || error == 1)
	{
		return;
	}
	
	//following if-else statements generate assembly code for the node specified in the if header
	if (!strcmp(root->label, "program"))
	{
		global = 1;
		recGen(root->child1, output);
		recGen(root->child2, output);
		fprintf(output, "\tSTOP\n");
		fprintf(output, "%s\n", globalVars);
		if (access("temp.txt", F_OK) != -1)
		{
			remove("temp.txt");
		}
		
		free(stack);
	}
	else if (!strcmp(root->label, "programPrime"))
	{
		if (root->tokens[0].tokenID > 1000)
		{
			funcFile = fopen("temp.txt", "w");
			fclose(funcFile);
			funcFile = fopen("temp.txt", "a+");
			strcpy(funcName, root->tokens[0].tokenInstance);
			push(stack, root->tokens[0].tokenInstance);
			recGen(root->child1, funcFile);
			fclose(funcFile);
			recGen(root->child2, output);
		}
		else
		{
			recGen(root->child1, output);
		}
	}
	else if (!strcmp(root->label, "block"))
	{
		blockCount++;
		recGen(root->child1, output);
		recGen(root->child2, output);
		while (varCount[blockCount] > 0)
		{
			pop(stack);
			fprintf(output, "\tPOP\n");
			varCount[blockCount]--;
		}
		blockCount--;
	}
	else if (!strcmp(root->label, "vars"))
	{
		if (global)
		{
			if (root->tokens[0].tokenID > 1000)
			{
				char temp[100];
				if (root->child1->tokens[0].tokenID > 1000)
				{
					sprintf(temp, "\t%s\t%s\n", root->tokens[0].tokenInstance, root->child1->tokens[0].tokenInstance);
				}
				else
				{
					sprintf(temp, "\t%s\t-7\n", root->tokens[0].tokenInstance);
				}
				//push(stack, root->tokens[0].tokenInstance);
				strcat(globalVars, temp);
			}
			recGen(root->child1, output);
			global = 0;
		}
		else
		{
			if (root->tokens[0].tokenID == 1001)
			{
				if (find(stack, root->tokens[0].tokenInstance) >= 0)
				{
					fprintf(stderr, "Error: %s declared more than once in scope. \n", root->tokens[0].tokenInstance);
					error = 1;
					return;
				}
				
				push(stack, root->tokens[0].tokenInstance);
				fprintf(output, "\tPUSH\n");
				if (root->child1->tokens[0].tokenID > 1000)
				{
					fprintf(output, "\tLOAD\t%s\n", root->child1->tokens[0].tokenInstance);
					fprintf(output, "\tSTACKW\t0\n");
				}
				else
				{
					fprintf(output, "\tLOAD\t-7\n");
                                        fprintf(output, "\tSTACKW\t0\n");
				}
				varCount[blockCount]++;
			}
			recGen(root->child1, output);
		}
	}
	else if (!strcmp(root->label, "varsPrime"))
	{
		recGen(root->child1, output);
	}
	else if (!strcmp(root->label, "stats"))
	{
		recGen(root->child1, output);
		recGen(root->child2, output);
	}
	else if (!strcmp(root->label, "stat"))
	{
		recGen(root->child1, output);
	}
	else if (!strcmp(root->label, "mStat"))
	{
		recGen(root->child1, output);
		recGen(root->child2, output);
	}
	else if (!strcmp(root->label, "in"))
	{
		int n;
                if ((n = find(stack, root->tokens[0].tokenInstance)) >= 0)
                {
                	char temp[20];
			strcpy(temp, newName(VAR));
			char globalTemp[50];
			sprintf(globalTemp, "\t%s\t-7\n", temp);
			strcat(globalVars, globalTemp);
			fprintf(output, "\tREAD\t%s\n", temp);
			fprintf(output, "\tLOAD\t%s\n", temp);
			fprintf(output, "\tSTACKW\t%d\n", n);
                }
                else if (strstr(globalVars, root->tokens[0].tokenInstance))
                {
                        fprintf(output, "\tREAD\t%s\n", root->tokens[0].tokenInstance);
                }
                else
                {
                        error = 1;
                        printf("Error: reference to variable that does not exist.");
                        exit(1);
                }
	}
	else if (!strcmp(root->label, "out"))
	{
		recGen(root->child1, output);
		char temp[20];
		strcpy(temp, newName(VAR));
		char globalTemp[50];
		sprintf(globalTemp, "\t%s\t-7\n", temp);
		strcat(globalVars, globalTemp);
		fprintf(output, "\tSTORE\t%s\n", temp);
		fprintf(output, "\tWRITE\t%s\n", temp);
	}
	else if (!strcmp(root->label, "if"))
	{
		if (!strcmp(root->child2->tokens[0].tokenInstance, "."))
		{
			recGen(root->child3, output);
			char temp[20];
			strcpy(temp, newName(VAR));
			char globalTemp[50];
                        sprintf(globalTemp, "\t%s\t-7\n", temp);
			strcat(globalVars, globalTemp);
			fprintf(output, "\tSTORE\t%s\n", temp);
			fprintf(output, "\tDIV\t2\n");
			fprintf(output, "\tMULT\t2\n");
			fprintf(output, "\tSUB\t%s\n", temp);
			fprintf(output, "\tSTORE\t%s\n", temp);
			recGen(root->child1, output);
			char temp2[20];
			strcpy(temp2, newName(VAR));
			sprintf(globalTemp, "\t%s\t-7\n", temp2);
			strcat(globalVars, globalTemp);
			fprintf(output, "\tSTORE\t%s\n", temp2);
			fprintf(output, "\tDIV\t2\n");
			fprintf(output, "\tMULT\t2\n");
			fprintf(output, "\tSUB\t%s\n", temp2);
			fprintf(output, "\tSUB\t%s\n", temp);
			strcpy(label, newName(LABEL));
			fprintf(output, "\tBRNEG\t%s\n", label);
			fprintf(output, "\tBRPOS\t%s\n", label);
			recGen(root->child4, output);
			fprintf(output, "\t%s:\tNOOP\n", label);
		}
		else
		{
			recGen(root->child3, output);
			char temp[20];
			strcpy(temp, newName(VAR));
			char globalTemp[50];
			sprintf(globalTemp, "\t%s\t-7\n", temp);
			strcat(globalVars, globalTemp);
			fprintf(output, "\tSTORE\t%s\n", temp);
			recGen(root->child1, output);
			fprintf(output, "\tSUB\t%s\n", temp);
			strcpy(label, newName(LABEL));
			if (!strcmp(root->child2->tokens[0].tokenInstance, "=="))
			{
				fprintf(output, "\tBRNEG\t%s\n", label);
				fprintf(output, "\tBRPOS\t%s\n", label);
			}
			else if (!strcmp(root->child2->tokens[0].tokenInstance, "<"))
			{
				fprintf(output, "\tBRZPOS\t%s\n", label);
			}
			else if (!strcmp(root->child2->tokens[0].tokenInstance, ">"))
			{
				fprintf(output, "\tBRZNEG\t%s\n", label);
			}
			else if (!strcmp(root->child2->tokens[0].tokenInstance, "=!="))
			{
				fprintf(output, "\tBRZERO\t%s\n", label);
			}
			
			recGen(root->child4, output);
			fprintf(output, "\t%s:\tNOOP\n", label);
		}
	}
	else if (!strcmp(root->label, "pick"))
	{
		recGen(root->child1, output);
		recGen(root->child2, output);
	}
	else if (!strcmp(root->label, "pickBody"))
	{
		strcpy(label, newName(LABEL));
                fprintf(output, "\tBRZERO\t%s\n", label);
                recGen(root->child1, output);
                strcpy(label2, newName(LABEL));
                fprintf(output, "\tBR\t%s\n", label2);
                fprintf(output, "\t%s:\tNOOP\n", label);
                recGen(root->child2, output);
		fprintf(output, "\t%s:\tNOOP\n", label2);
	}
	else if (!strcmp(root->label, "loop1"))
	{
		strcpy(label, newName(LABEL));
		fprintf(output, "\t%s:\tNOOP\n", label);
		recGen(root->child3, output);
                char temp[20];
                strcpy(temp, newName(VAR));
                char globalTemp[50];
                sprintf(globalTemp, "\t%s\t-7\n", temp);
                strcat(globalVars, globalTemp);
                fprintf(output, "\tSTORE\t%s\n", temp);
                recGen(root->child1, output);
                fprintf(output, "\tSUB\t%s\n", temp);
                strcpy(label2, newName(LABEL));
                if (!strcmp(root->child2->tokens[0].tokenInstance, "=="))
                {
                        fprintf(output, "\tBRNEG\t%s\n", label2);
                        fprintf(output, "\tBRPOS\t%s\n", label2);
                }
                else if (!strcmp(root->child2->tokens[0].tokenInstance, "<"))
                {
                        fprintf(output, "\tBRZPOS\t%s\n", label2);
                }
                else if (!strcmp(root->child2->tokens[0].tokenInstance, ">"))
                {
                        fprintf(output, "\tBRZNEG\t%s\n", label2);
                }
                else if (!strcmp(root->child2->tokens[0].tokenInstance, "=!="))
                {
                        fprintf(output, "\tBRZERO\t%s\n", label2);
                }

		recGen(root->child4, output);
		fprintf(output, "\tBR\t%s\n", label);
		fprintf(output, "\t%s:\tNOOP\n", label2);

	}
	else if (!strcmp(root->label, "loop2"))
	{
		strcpy(label, newName(LABEL));
		fprintf(output, "\t%s:\tNOOP\n", label);
		recGen(root->child1, output);
		recGen(root->child4, output);
                char temp[20];
                strcpy(temp, newName(VAR));
                char globalTemp[50];
                sprintf(globalTemp, "\t%s\t-7\n", temp);
                strcat(globalVars, globalTemp);
                fprintf(output, "\tSTORE\t%s\n", temp);
                recGen(root->child2, output);
                fprintf(output, "\tSUB\t%s\n", temp);
                strcpy(label2, newName(LABEL));
                if (!strcmp(root->child3->tokens[0].tokenInstance, "=="))
                {
                        fprintf(output, "\tBRZERO\t%s\n", label2);
                }
                else if (!strcmp(root->child3->tokens[0].tokenInstance, "<"))
                {
                        fprintf(output, "\tBRNEG\t%s\n", label2);
                }
                else if (!strcmp(root->child3->tokens[0].tokenInstance, ">"))
                {
                        fprintf(output, "\tBRPOS\t%s\n", label2);
                }
                else if (!strcmp(root->child3->tokens[0].tokenInstance, "=!="))
                {
                        fprintf(output, "\tBRPOS\t%s\n", label2);
			fprintf(output, "\tBRNEG\t%s\n", label2);
                }
		
		fprintf(output, "\tBR\t%s\n", label);
		fprintf(output, "\t%s:\tNOOP\n", label2);
	}
	else if (!strcmp(root->label, "label"))
	{
		fprintf(output, "\t%s:\tNOOP\n", root->tokens[0].tokenInstance);
	}
	else if (!strcmp(root->label, "goto"))
	{
		if (!strcmp(root->tokens[0].tokenInstance, funcName))
		{
			char buf;
			funcFile = fopen("temp.txt", "a+");
			while ((buf = fgetc(funcFile)) != EOF)
			{
				fprintf(output, "%c", buf);
			}
			/*while (!feof(funcFile))
			{
				fgets(buf, sizeof(buf), funcFile);
				printf("%s", buf);
				printf("in here");
				if (buf[strlen(buf) - 1] == '\n')
				{
					fprintf(output, "%s", buf);
				}
			}
			*/
		}
		else
		{
			fprintf(output, "\tBR\t%s\n", root->tokens[0].tokenInstance);
		}
	}
	else if (!strcmp(root->label, "assign"))
	{
		recGen(root->child1, output);

                int n;
                if ((n = find(stack, root->tokens[0].tokenInstance)) >= 0)
                {
                        fprintf(output, "\tSTACKW\t%d\n", n);
                }
                else if (strstr(globalVars, root->tokens[0].tokenInstance))
                {
                        fprintf(output, "\tSTORE\t%s\n", root->tokens[0].tokenInstance);
                }
                else
                {
                        error = 1;
                        printf("Error: reference to variable that does not exist.");
                        exit(1);
                }
	}
	else if (!strcmp(root->label, "expr"))
	{
		recGen(root->child1, output);
		recGen(root->child2, output);
	}
	else if (!strcmp(root->label, "exprPrime"))
	{
		if (root->tokens[0].tokenID == 1003)
		{
			char temp[20];
			strcpy(temp, newName(VAR));
			fprintf(output, "\tSTORE\t%s\n", temp);
			char globalTemp[50];
			sprintf(globalTemp, "\t%s\t-7\n", temp);
                	strcat(globalVars, globalTemp);
                        recGen(root->child1, output);
                        char temp2[20];
                        strcpy(temp2, newName(VAR));
			sprintf(globalTemp, "\t%s\t-7\n", temp2);
                	strcat(globalVars, globalTemp);
                        fprintf(output, "\tSTORE\t%s\n", temp2);
                        fprintf(output, "\tLOAD\t%s\n", temp);
                        fprintf(output, "\tSUB\t%s\n", temp2);
		}
	}
	else if (!strcmp(root->label, "N"))
	{
		recGen(root->child1, output);
		recGen(root->child2, output);
	}
	else if (!strcmp(root->label, "NPrime"))
	{	
		if (root->tokens[0].tokenID == 1003)
		{
			if (!strcmp(root->tokens[0].tokenInstance, "/"))
			{
				char temp[20];
				strcpy(temp, newName(VAR));
				char globalTemp[50];
				sprintf(globalTemp, "\t%s\t-7\n", temp);
                		strcat(globalVars, globalTemp);
				fprintf(output, "\tSTORE\t%s\n", temp);
				recGen(root->child1, output);
				char temp2[20];
				strcpy(temp, newName(VAR));
                		sprintf(globalTemp, "\t%s\t-7\n", temp2);
                		strcat(globalVars, globalTemp);
				fprintf(output, "\tSTORE\t%s\n", temp2);
				fprintf(output, "\tLOAD\t%s\n", temp);
				fprintf(output, "\tDIV\t%s\n", temp2);
				recGen(root->child2, output);
			}
			else if(!strcmp(root->tokens[0].tokenInstance, "+"))
			{
				char temp[20];
				strcpy(temp, newName(VAR));
				char globalTemp[50];
                		sprintf(globalTemp, "\t%s\t-7\n", temp);
                		strcat(globalVars, globalTemp);
				fprintf(output, "\tSTORE\t%s\n", temp);
				recGen(root->child1, output);
				fprintf(output, "\tADD\t%s\n", temp);
				recGen(root->child2, output);
			}
		}
	}
	else if (!strcmp(root->label, "A"))
	{
		recGen(root->child1, output);
		recGen(root->child2, output);
	}
	else if (!strcmp(root->label, "APrime"))
	{
		if (root->tokens[0].tokenID == 1003)
		{
			char temp[20];
			strcpy(temp, newName(VAR));
			char globalTemp[50];
			sprintf(globalTemp, "\t%s\t-7\n", temp);
                	strcat(globalVars, globalTemp);
			fprintf(output, "\tSTORE\t%s\n", temp);
			recGen(root->child1, output);
			fprintf(output, "\tMULT\t%s\n", temp);
			recGen(root->child2, output);
		}
	}
	else if (!strcmp(root->label, "M"))
	{
		if (root->tokens[0].tokenID == 1003)
		{
			recGen(root->child1, output);
			strcpy(label, newName(LABEL));
			fprintf(output, "\tBRPOS\t%s\n", label);
			fprintf(output, "\tBRNEG\t%s\n", label);
			fprintf(output, "\tLOAD\t1\n");
			strcpy(label2, newName(LABEL));
			fprintf(output, "\tBR\t%s\n", label2);
			fprintf(output, "\t%s:\tSUB\t1\n", label);
			strcpy(label, newName(LABEL));
			fprintf(output, "\tBRPOS\t%s\n", label);
			fprintf(output, "\tBRNEG\t%s\n", label);
			fprintf(output, "\tBR\t%s\n", label2);
			fprintf(output, "\t%s:\tADD\t1\n", label);
			fprintf(output, "\tMULT\t-1\n");
			fprintf(output, "\t%s:\tNOOP\n", label2);
		}
		else
		{
			recGen(root->child1, output);
		}
		
	}
	else if (!strcmp(root->label, "R"))
	{
		if (root->child1 != NULL)
		{
			recGen(root->child1, output);
		}
		else if (root->tokens[0].tokenID == 1002)
		{
			fprintf(output, "\tLOAD\t%s\n", root->tokens[0].tokenInstance);
		}
		else
		{
			int n;
			if ((n = find(stack, root->tokens[0].tokenInstance)) >= 0)
			{
				fprintf(output, "\tSTACKR\t%d\n", n);
			}
			else if (strstr(globalVars, root->tokens[0].tokenInstance))
			{
				fprintf(output, "\tLOAD\t%s\n", root->tokens[0].tokenInstance);
			}
			else
			{
				error = 1;
				printf("Error: reference to variable that does not exist.");
				exit(1);
			}
		}
	}
}
