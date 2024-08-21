#include "parser.h"
struct node* program();
struct node* programPrime();
struct node* block();
struct node* vars();
struct node* varsPrime();
struct node* expr();
struct node* exprPrime();
struct node* N();
struct node* NPrime();
struct node* A();
struct node* APrime();
struct node* M();
struct node* R();
struct node* stats();
struct node* mStat();
struct node* stat();
struct node* in();
struct node* out();
struct node* If();
struct node* pick();
struct node* loop1();
struct node* loop2();
struct node* assign();
struct node* label();
struct node* goTo();
struct node* pickBody();
struct node* RO();
FILE *inputFilePointer;
struct tokeType token;

//outputs error message provided and then exits the program
void error(char *err)
{
	fprintf(stderr, "%s\n", err);
	exit(1);
}

//creates a node and labels it using the provided string
struct node* createNode(char *label)
{
	struct node* newNode = malloc(sizeof(struct node));
	strcpy(newNode->label, label);
	newNode->child1 = NULL;
	newNode->child2 = NULL;
	newNode->child3 = NULL;
	newNode->child4 = NULL;
	return newNode;
}

//clears out the tree and dynamically allocated memory after program is run
void freeTree(struct node* root)
{
	if (root == NULL)
	{
		return;
	}
	freeTree(root->child1);
	freeTree(root->child2);
	freeTree(root->child3);
	freeTree(root->child4);
	int i = 0;
	while (root->tokens[i].tokenID > 1000)
	{
		free(root->tokens[i].tokenInstance);
		free(root->tokens[i].lineCharNum);
		i++;
	}
	free(root);
}

//prints out the created tree in preorder
void print(struct node* root, int level)
{
	if (root == NULL)
	{
		return;
	}
	printf("%*cLabel: %s ", level*2, ' ', root->label);
	int i = 0;
	while (root->tokens[i].tokenID > 1000)
	{
		printf("| Token %d: %s ", i+1, root->tokens[i].tokenInstance);
		i++;
	}
	printf("\n");
	print(root->child1, level+1);
	print(root->child2, level+1);
	print(root->child3, level+1);
	print(root->child4, level+1);
	if (level == 1)
	{
	//	freeTree(root);
	}
}

//copies a tokens char array pointer content
char* copyToken(char* tokenBeingCopied)
{
	int length = 0;
	const char* temp = tokenBeingCopied;
	while (*temp != '\0')
	{
		length++;
		temp++;
	}

	char *copy = (char*)malloc((length + 1) * sizeof(char));
	
	if (copy == NULL)
	{
		printf("Error");
		return NULL;
	}
	
	while (*tokenBeingCopied != '\0')
	{
		*copy = *tokenBeingCopied;
		copy++;
		tokenBeingCopied++;
	}
	*copy = '\0';
	copy = copy - length;
	return copy;
}

//starting function that kicks off parsing
struct node* parser(FILE *file)
{
	inputFilePointer = file;
	struct node* root;
	token = Driver(inputFilePointer);
	root = program();
	token = Driver(inputFilePointer);
	if (token.tokenID == 1005)
	{
		return root;
	}
	else
	{
		char err[100];
		sprintf(err, "Error: expected EOF but received %s on line.char %s", token.tokenInstance, token.lineCharNum);
		error(err);
	}
	return root;
}

//program bnf
struct node* program()
{
	//create a node for this bnf
	struct node* p = createNode("program");
	//assign the first child node a vars node
	p->child1 = vars();
	
	//if tape is current token accept and continue
	if (!strcmp(token.tokenInstance, "tape"))
	{
		//copy the token into the tree
		p->tokens[0].tokenID = token.tokenID;
                p->tokens[0].tokenInstance = copyToken(token.tokenInstance);
                p->tokens[0].lineCharNum = copyToken(token.lineCharNum);
		//obtain new token
		token = Driver(inputFilePointer);
		//assign child2 node a programPrime node
		p->child2 = programPrime();
		//return the node
		return p;
	}
	//if tape is not the next token error out
	else
	{
		char err[100];
                sprintf(err, "Error: expected tape but received %s on line.char %s", token.tokenInstance, token.lineCharNum);
                error(err);
	}
	return p;
}

//program prime bnf
struct node* programPrime()
{
	struct node* p = createNode("programPrime");
	if (!strcmp(token.tokenInstance, "func"))
	{
		//p->tokens[0].tokenID = token.tokenID;
                //p->tokens[0].tokenInstance = copyToken(token.tokenInstance);
                //p->tokens[0].lineCharNum = copyToken(token.lineCharNum);
		token = Driver(inputFilePointer);
		if (token.tokenID == 1001)
		{
			p->tokens[0].tokenID = token.tokenID;
                	p->tokens[0].tokenInstance = copyToken(token.tokenInstance);
                	p->tokens[0].lineCharNum = copyToken(token.lineCharNum);
			token = Driver(inputFilePointer);
			p->child1 = block();
			p->child2 = block();
			return p;
		}
		else
        	{
        	        char err[100];
        	        sprintf(err, "Error: expected an identifier but received %s on line.char %s", token.tokenInstance, token.lineCharNum);
        	        error(err);
        	}
	}
	else if (!strcmp(token.tokenInstance, "{"))
	{
		p->child1 = block();
		return p;
	}
	else
	{
		char err[100];
                sprintf(err, "Error: expected func or { but received %s on line.char %s", token.tokenInstance, token.lineCharNum);
                error(err);
	}
	return p;
}

struct node* block()
{
	struct node* p = createNode("block");
	if (!strcmp(token.tokenInstance, "{"))
        {
                //p->tokens[0].tokenID = token.tokenID;
                //p->tokens[0].tokenInstance = copyToken(token.tokenInstance);
                //p->tokens[0].lineCharNum = copyToken(token.lineCharNum);

                token = Driver(inputFilePointer);
                p->child1 = vars();
                p->child2 = stats();

                if (!strcmp(token.tokenInstance, "}"))
                {
                        //p->tokens[1].tokenID = token.tokenID;
                        //p->tokens[1].tokenInstance = copyToken(token.tokenInstance);
                        //p->tokens[1].lineCharNum = copyToken(token.lineCharNum);
                        token = Driver(inputFilePointer);
                        return p;
                }
                else
                {
                        char err[100];
                        sprintf(err, "Error: expected } but received %s on line.char %s", token.tokenInstance, token.lineCharNum);
                        error(err);
                }
        }
	else
	{
		char err[100];
                sprintf(err, "Error: expected func or { but received %s on line.char %s", token.tokenInstance, token.lineCharNum);
                error(err);
	}
	return p;
}

//vars bnf
struct node* vars()
{
	struct node* p = createNode("vars");
	if (!strcmp(token.tokenInstance, "create"))
	{
		//p->tokens[0].tokenID = token.tokenID;
                //p->tokens[0].tokenInstance = copyToken(token.tokenInstance);
                //p->tokens[0].lineCharNum = copyToken(token.lineCharNum);
		token = Driver(inputFilePointer);
		if (token.tokenID == 1001)
		{
			p->tokens[0].tokenID = token.tokenID;
                	p->tokens[0].tokenInstance = copyToken(token.tokenInstance);
                	p->tokens[0].lineCharNum = copyToken(token.lineCharNum);
			token = Driver(inputFilePointer);
			p->child1 = varsPrime();
			return p;
		}
		else
        	{
                	char err[100];
                	sprintf(err, "Error: expected identifier but received %s on line.char %s", token.tokenInstance, token.lineCharNum);
                	error(err);
        	}
	}
	else
	{
		return p;
	}
	return p;
}

//varsPrime bnf
struct node* varsPrime()
{
	struct node* p = createNode("varsPrime");
	if (!strcmp(token.tokenInstance, ";"))
        {
		//p->tokens[0].tokenID = token.tokenID;
                //p->tokens[0].tokenInstance = copyToken(token.tokenInstance);
                //p->tokens[0].lineCharNum = copyToken(token.lineCharNum);
		token = Driver(inputFilePointer);
        	return p;
        }
        else if (!strcmp(token.tokenInstance, ":="))
        {
		//p->tokens[0].tokenID = token.tokenID;
                //p->tokens[0].tokenInstance = copyToken(token.tokenInstance);
                //p->tokens[0].lineCharNum = copyToken(token.lineCharNum);
        	token = Driver(inputFilePointer);
                if (token.tokenID == 1002)
                {
			p->tokens[0].tokenID = token.tokenID;
                	p->tokens[0].tokenInstance = copyToken(token.tokenInstance);
                	p->tokens[0].lineCharNum = copyToken(token.lineCharNum);
                	token = Driver(inputFilePointer);
                        if (!strcmp(token.tokenInstance, ";"))
			{
				//p->tokens[2].tokenID = token.tokenID;
                		//p->tokens[2].tokenInstance = copyToken(token.tokenInstance);
                		//p->tokens[2].lineCharNum = copyToken(token.lineCharNum);
				token = Driver(inputFilePointer);
				p->child1 = vars();
				return p;
			}
			else
        		{
                		char err[100];
                		sprintf(err, "Error: expected ; but received %s on line.char %s", token.tokenInstance, token.lineCharNum);
                		error(err);
       		 	}
                }
		else
        	{
                	char err[100];
                	sprintf(err, "Error: expected number but received %s on line.char %s", token.tokenInstance, token.lineCharNum);
                	error(err);
       	 	}
        }
	else
	{
		char err[100];
                sprintf(err, "Error: expected ; or := but received %s on line.char %s", token.tokenInstance, token.lineCharNum);
                error(err);
	}
	return p;
}

//expr bnf
struct node* expr()
{
	struct node* p = createNode("expr");
	p->child1 = N();
	//token = Driver(inputFilePointer);
	p->child2 = exprPrime();
	return p;
}

//exprPrime bnf
struct node* exprPrime()
{
	struct node* p = createNode("exprPrime");
	if (!strcmp(token.tokenInstance, "-"))
	{
		p->tokens[0].tokenID = token.tokenID;
                p->tokens[0].tokenInstance = copyToken(token.tokenInstance);
                p->tokens[0].lineCharNum = copyToken(token.lineCharNum);
		token = Driver(inputFilePointer);
		p->child1 = expr();
		return p;
	}
	else
	{
		return p;
	}
	return p;
}

//N bnf
struct node* N()
{
	struct node* p = createNode("N");
	p->child1 = A();
	p->child2 = NPrime();
	return p;
}

//NPrime bnf
struct node* NPrime()
{
	struct node* p = createNode("NPrime");
	if (!strcmp(token.tokenInstance, "/"))
	{
		p->tokens[0].tokenID = token.tokenID;
                p->tokens[0].tokenInstance = copyToken(token.tokenInstance);
                p->tokens[0].lineCharNum = copyToken(token.lineCharNum);
		token = Driver(inputFilePointer);
		p->child1 = A();
		p->child2 = NPrime();
		return p;
	}
	else if (!strcmp(token.tokenInstance, "+"))
	{
		p->tokens[0].tokenID = token.tokenID;
                p->tokens[0].tokenInstance = copyToken(token.tokenInstance);
                p->tokens[0].lineCharNum = copyToken(token.lineCharNum);
		token = Driver(inputFilePointer);
		p->child1 = A();
		//token = Driver(inputFilePointer);
		p->child2 = NPrime();
		return p;
	}
	else
	{
		return p;
	}
	return p;
}

//A bnf
struct node* A()
{
	struct node* p = createNode("A");
	p->child1 = M();
	//token = Driver(inputFilePointer);
	p->child2 = APrime();
	return p;
}

//APrime bnf
struct node* APrime()
{
	struct node* p = createNode("APrime");
	if (!strcmp(token.tokenInstance, "*"))
	{
		p->tokens[0].tokenID = token.tokenID;
                p->tokens[0].tokenInstance = copyToken(token.tokenInstance);
                p->tokens[0].lineCharNum = copyToken(token.lineCharNum);
		token = Driver(inputFilePointer);
		p->child1 = A();
		return p;
	}
	else
	{
		return p;
	}
	return p;
}

//M bnf
struct node* M()
{
	struct node* p = createNode("M");
	if (!strcmp(token.tokenInstance, "^"))
	{
		p->tokens[0].tokenID = token.tokenID;
                p->tokens[0].tokenInstance = copyToken(token.tokenInstance);
                p->tokens[0].lineCharNum = copyToken(token.lineCharNum);
		token = Driver(inputFilePointer);
		p->child1 = M();
		return p;
	}
	else
	{
		p->child1 = R();
		return p;
	}
	return p;
}

//R bnf
struct node* R()
{
	struct node* p = createNode("R");
	if (!strcmp(token.tokenInstance, "("))
	{
		//p->tokens[0].tokenID = token.tokenID;
                //p->tokens[0].tokenInstance = copyToken(token.tokenInstance);
                //p->tokens[0].lineCharNum = copyToken(token.lineCharNum);
		token = Driver(inputFilePointer);
		p->child1 = expr();
		if (!strcmp(token.tokenInstance, ")"))
		{
			//p->tokens[1].tokenID = token.tokenID;
                	//p->tokens[1].tokenInstance = copyToken(token.tokenInstance);
                	//p->tokens[1].lineCharNum = copyToken(token.lineCharNum);
			token = Driver(inputFilePointer);
			return p;
		}
		else
        	{
                	char err[100];
                	sprintf(err, "Error: expected ) but received %s on line.char %s", token.tokenInstance, token.lineCharNum);
                	error(err);
        	}
	}
	else if (token.tokenID == 1001)
	{
		p->tokens[0].tokenID = token.tokenID;
                p->tokens[0].tokenInstance = copyToken(token.tokenInstance);
                p->tokens[0].lineCharNum = copyToken(token.lineCharNum);
		token = Driver(inputFilePointer);
		return p;
	}
	else if (token.tokenID == 1002)
	{
		p->tokens[0].tokenID = token.tokenID;
                p->tokens[0].tokenInstance = copyToken(token.tokenInstance);
                p->tokens[0].lineCharNum = copyToken(token.lineCharNum);
		token = Driver(inputFilePointer);
		return p;
	}
	else
	{
		char err[100];
		sprintf(err, "Error: expected ( or a identifier or number but received%s on line.char %s", token.tokenInstance, token.lineCharNum);
		error(err);
	}
	return p;
}

//stats bnf
struct node* stats()
{
	struct node* p = createNode("stats");
	p->child1 = stat();
	p->child2 = mStat();
	return p;
}

//mStat bnf
struct node* mStat()
{
	struct node* p = createNode("mStat");
	if (!strcmp(token.tokenInstance, "cin"))
	{
		p->child1 = stat();
		p->child2 = mStat();
		return p;
	}
	else if (!strcmp(token.tokenInstance, "cout"))
	{
		p->child1 = stat();
		p->child2 = mStat();
		return p;
	}
	else if (!strcmp(token.tokenInstance, "{"))
      	{
                p->child1 = stat();
                p->child2 = mStat();
                return p;
        }
	else if (!strcmp(token.tokenInstance, "if"))
        {
                p->child1 = stat();
                p->child2 = mStat();
                return p;
        }
	else if (!strcmp(token.tokenInstance, "while"))
        {
                p->child1 = stat();
                p->child2 = mStat();
                return p;
        }
	else if (!strcmp(token.tokenInstance, "repeat"))
        {
                p->child1 = stat();
                p->child2 = mStat();
                return p;
        }
	else if (!strcmp(token.tokenInstance, "set"))
        {
                p->child1 = stat();
                p->child2 = mStat();
                return p;
        }
	else if (token.tokenID == 1001)
        {
                p->child1 = stat();
                p->child2 = mStat();
                return p;
        }
	else if (!strcmp(token.tokenInstance, "jump"))
        {
                p->child1 = stat();
                p->child2 = mStat();
                return p;
        }
	else if (!strcmp(token.tokenInstance, "label"))
        {
                p->child1 = stat();
                p->child2 = mStat();
                return p;
        }
	else if (!strcmp(token.tokenInstance, "pick"))
        {
                p->child1 = stat();
                p->child2 = mStat();
                return p;
        }
	else
	{
		return p;
	}
	return p;
}

//stat bnf
struct node* stat()
{
	struct node* p = createNode("stat");
	if (!strcmp(token.tokenInstance, "cin"))
        {
                p->child1 = in();
        }
        else if (!strcmp(token.tokenInstance, "cout"))
        {
                p->child1 = out();
        }
        else if (!strcmp(token.tokenInstance, "{"))
        {
                p->child1 = block();
		return p;
        }
        else if (!strcmp(token.tokenInstance, "if"))
        {
                p->child1 = If();
        }
        else if (!strcmp(token.tokenInstance, "while"))
        {
                p->child1 = loop1();
        }
        else if (!strcmp(token.tokenInstance, "repeat"))
        {
                p->child1 = loop2();
        }
        else if (!strcmp(token.tokenInstance, "set") || token.tokenID == 1001)
        {
                p->child1 = assign();
        }
        else if (!strcmp(token.tokenInstance, "jump"))
        {
                p->child1 = goTo();
        }
	else if (!strcmp(token.tokenInstance, "label"))
        {
                p->child1 = label();
        }
        else if (!strcmp(token.tokenInstance, "pick"))
        {
                p->child1 = pick();
        }
        else
        {
                char err[100];
                sprintf(err, "Error: expected a statement but received %s on line.char %s", token.tokenInstance, token.lineCharNum);
                error(err);
        }
	
	if (!strcmp(token.tokenInstance, ";"))
        {
                //p->tokens[1].tokenID = token.tokenID;
                //p->tokens[1].tokenInstance = copyToken(token.tokenInstance);
                //p->tokens[1].lineCharNum = copyToken(token.lineCharNum);
                token = Driver(inputFilePointer);
                return p;
        }
        else
        {
                char err[100];
                sprintf(err, "Error: expected ; but received %s on line.char %s", token.tokenInstance, token.lineCharNum);
                error(err);
        }
        return p;       
}

struct node* in()
{
	struct node* p = createNode("in");
	if (!strcmp(token.tokenInstance, "cin"))
        {
                //p->tokens[0].tokenID = token.tokenID;
                //p->tokens[0].tokenInstance = copyToken(token.tokenInstance);
                //p->tokens[0].lineCharNum = copyToken(token.lineCharNum);
                token = Driver(inputFilePointer);
                if (token.tokenID == 1001)
                {
                        p->tokens[0].tokenID = token.tokenID;
                        p->tokens[0].tokenInstance = copyToken(token.tokenInstance);
                        p->tokens[0].lineCharNum = copyToken(token.lineCharNum);
                        token = Driver(inputFilePointer);
			return p;
                }
                else
                {
                        char err[100];
                        sprintf(err, "Error: expected identifer but received %s on line.char %s", token.tokenInstance, token.lineCharNum);
                        error(err);
                }
        }
	return p;
}

struct node* out()
{
	struct node* p = createNode("out");
	if (!strcmp(token.tokenInstance, "cout"))
        {
                //p->tokens[0].tokenID = token.tokenID;
                //p->tokens[0].tokenInstance = copyToken(token.tokenInstance);
                //p->tokens[0].lineCharNum = copyToken(token.lineCharNum);
                token = Driver(inputFilePointer);
                p->child1 = expr();
                return p;
        }
	return p;
}

struct node* If()
{
	struct node* p = createNode("if");
	if (!strcmp(token.tokenInstance, "if"))
        {
                //p->tokens[0].tokenID = token.tokenID;
                //p->tokens[0].tokenInstance = copyToken(token.tokenInstance);
                //p->tokens[0].lineCharNum = copyToken(token.lineCharNum);
                token = Driver(inputFilePointer);
                if (!strcmp(token.tokenInstance, "["))
                {
                        //p->tokens[1].tokenID = token.tokenID;
                        //p->tokens[1].tokenInstance = copyToken(token.tokenInstance);
                        //p->tokens[1].lineCharNum = copyToken(token.lineCharNum);
                        token = Driver(inputFilePointer);
                        p->child1 = expr();
                        p->child2 = RO();
                        p->child3 = expr();
                        if (!strcmp(token.tokenInstance, "]"))
                        {
                                //p->tokens[2].tokenID = token.tokenID;
                                //p->tokens[2].tokenInstance = copyToken(token.tokenInstance);
                                //p->tokens[2].lineCharNum = copyToken(token.lineCharNum);
                                token = Driver(inputFilePointer);
                                if (!strcmp(token.tokenInstance, "then"))
                                {
                                        //p->tokens[3].tokenID = token.tokenID;
                                        //p->tokens[3].tokenInstance = copyToken(token.tokenInstance);
                                        //p->tokens[3].lineCharNum = copyToken(token.lineCharNum);
                                        token = Driver(inputFilePointer);
                                        p->child4 = stat();
                                        return p;
                                }
                                else
                                {
                                        char err[100];
                                        sprintf(err, "Error: expected then but received %s on line.char %s", token.tokenInstance, token.lineCharNum);
                                        error(err);
                                }
                        }
                        else
                        {
                                char err[100];
                                sprintf(err, "Error: expected ] but received %s on line.char %s", token.tokenInstance, token.lineCharNum);
                                error(err);
                        }
                }
                else
                {
                        char err[100];
                        sprintf(err, "Error: expected [ but received %s on line.char %s", token.tokenInstance, token.lineCharNum);
                        error(err);
                }
        }
	return p;
}

struct node* loop1()
{
	struct node* p = createNode("loop1");
	if (!strcmp(token.tokenInstance, "while"))
        {
                //p->tokens[0].tokenID = token.tokenID;
                //p->tokens[0].tokenInstance = copyToken(token.tokenInstance);
                //p->tokens[0].lineCharNum = copyToken(token.lineCharNum);
                token = Driver(inputFilePointer);
                if (!strcmp(token.tokenInstance, "["))
                {
                        //p->tokens[1].tokenID = token.tokenID;
                        //p->tokens[1].tokenInstance = copyToken(token.tokenInstance);
                        //p->tokens[1].lineCharNum = copyToken(token.lineCharNum);
                        token = Driver(inputFilePointer);
                        p->child1 = expr();
                        p->child2 = RO();
                        p->child3 = expr();
                        if (!strcmp(token.tokenInstance, "]"))
                        {
                                //p->tokens[2].tokenID = token.tokenID;
                                //p->tokens[2].tokenInstance = copyToken(token.tokenInstance);
                                //p->tokens[2].lineCharNum = copyToken(token.lineCharNum);
                                token = Driver(inputFilePointer);
                                p->child4 = stat();
                                return p;
                        }
                        else
                        {
                                char err[100];
                                sprintf(err, "Error: expected ] but received %s on line.char %s", token.tokenInstance, token.lineCharNum);
                                error(err);
                        }
                }
                else
                {
                        char err[100];
                        sprintf(err, "Error: expected [ but received %s on line.char %s", token.tokenInstance, token.lineCharNum);
                        error(err);
                }
        }
	return p;
}

struct node* loop2()
{
	struct node* p = createNode("loop2");
	if (!strcmp(token.tokenInstance, "repeat"))
        {
                //p->tokens[0].tokenID = token.tokenID;
                //p->tokens[0].tokenInstance = copyToken(token.tokenInstance);
                //p->tokens[0].lineCharNum = copyToken(token.lineCharNum);
                token = Driver(inputFilePointer);
                p->child1 = stat();
                if (!strcmp(token.tokenInstance, "until"))
                {
                        //p->tokens[1].tokenID = token.tokenID;
                        //p->tokens[1].tokenInstance = copyToken(token.tokenInstance);
                        //p->tokens[1].lineCharNum = copyToken(token.lineCharNum);
                        token = Driver(inputFilePointer);
                        if (!strcmp(token.tokenInstance, "["))
                        {
                                //p->tokens[2].tokenID = token.tokenID;
                                //p->tokens[2].tokenInstance = copyToken(token.tokenInstance);
                                //p->tokens[2].lineCharNum = copyToken(token.lineCharNum);
                                token = Driver(inputFilePointer);
                                p->child2 = expr();
                                p->child3 = RO();
                                p->child4 = expr();
                                if (!strcmp(token.tokenInstance, "]"))
                                {
                                        //p->tokens[3].tokenID = token.tokenID;
                                        //p->tokens[3].tokenInstance = copyToken(token.tokenInstance);
                                        //p->tokens[3].lineCharNum = copyToken(token.lineCharNum);
                                        token = Driver(inputFilePointer);
                                        return p;
                                }
                                else
                                {
                                        char err[100];
                                        sprintf(err, "Error: expected ] but received %s on line.char %s", token.tokenInstance, token.lineCharNum);
                                        error(err);
                                }
                        }
                        else
                        {
                                char err[100];
                                sprintf(err, "Error: expected [ but received %s on line.char %s", token.tokenInstance, token.lineCharNum);
                                error(err);
                        }
                }
                else
                {
                        char err[100];
                        sprintf(err, "Error: expected until but received %s on line.char %s", token.tokenInstance, token.lineCharNum);
                        error(err);
                }
        }
	return p;
}
struct node* assign()
{
	struct node* p = createNode("assign");
	if (!strcmp(token.tokenInstance, "set"))
        {
                //p->tokens[0].tokenID = token.tokenID;
                //p->tokens[0].tokenInstance = copyToken(token.tokenInstance);
                //p->tokens[0].lineCharNum = copyToken(token.lineCharNum);
                token = Driver(inputFilePointer);
                if (token.tokenID == 1001)
                {
                        p->tokens[0].tokenID = token.tokenID;
                        p->tokens[0].tokenInstance = copyToken(token.tokenInstance);
                        p->tokens[0].lineCharNum = copyToken(token.lineCharNum);
                        token = Driver(inputFilePointer);
                        if (!strcmp(token.tokenInstance, "="))
                        {
                                //p->tokens[2].tokenID = token.tokenID;
                                //p->tokens[2].tokenInstance = copyToken(token.tokenInstance);
                                //p->tokens[2].lineCharNum = copyToken(token.lineCharNum);
                                token = Driver(inputFilePointer);
                                p->child1 = expr();
                                return p;
                        }
                        else
                        {
                                char err[100];
                                sprintf(err, "Error: expected = but received %s on line.char %s", token.tokenInstance, token.lineCharNum);
                                error(err);
                        }
                }
                else
                {
                        char err[100];
                        sprintf(err, "Error: expected identifier but received %s on line.char %s", token.tokenInstance, token.lineCharNum);
                        error(err);
                }
        }
        else if (token.tokenID == 1001)
        {
                p->tokens[0].tokenID = token.tokenID;
                p->tokens[0].tokenInstance = copyToken(token.tokenInstance);
                p->tokens[0].lineCharNum = copyToken(token.lineCharNum);
                token = Driver(inputFilePointer);
                if (!strcmp(token.tokenInstance, "="))
                {
                        //p->tokens[1].tokenID = token.tokenID;
                        //p->tokens[1].tokenInstance = copyToken(token.tokenInstance);
                        //p->tokens[1].lineCharNum = copyToken(token.lineCharNum);
                        token = Driver(inputFilePointer);
                        p->child1 = expr();
                        return p;
                }
                else
                {
                        char err[100];
                        sprintf(err, "Error: expected = but received %s on line.char %s", token.tokenInstance, token.lineCharNum);
                        error(err);
                }
        }
	return p;
}
struct node* goTo()
{
	struct node* p = createNode("goto");
	if (!strcmp(token.tokenInstance, "jump"))
        {
                //p->tokens[0].tokenID = token.tokenID;
                //p->tokens[0].tokenInstance = copyToken(token.tokenInstance);
                //p->tokens[0].lineCharNum = copyToken(token.lineCharNum);
                token = Driver(inputFilePointer);
                if (token.tokenID == 1001)
                {
                        p->tokens[0].tokenID = token.tokenID;
                        p->tokens[0].tokenInstance = copyToken(token.tokenInstance);
                        p->tokens[0].lineCharNum = copyToken(token.lineCharNum);
                        token = Driver(inputFilePointer);
                        return p;
                }
                else
                {
                        char err[100];
                        sprintf(err, "Error: expected identifier but received %s on line.char %s", token.tokenInstance, token.lineCharNum);
                        error(err);
                }
        }
	return p;
}
struct node* label()
{
	struct node* p = createNode("label");
	if (!strcmp(token.tokenInstance, "label"))
        {
                //p->tokens[0].tokenID = token.tokenID;
                //p->tokens[0].tokenInstance = copyToken(token.tokenInstance);
                //p->tokens[0].lineCharNum = copyToken(token.lineCharNum);
                token = Driver(inputFilePointer);
                if (token.tokenID == 1001)
                {
                        p->tokens[0].tokenID = token.tokenID;
                        p->tokens[0].tokenInstance = copyToken(token.tokenInstance);
                        p->tokens[0].lineCharNum = copyToken(token.lineCharNum);
                        token = Driver(inputFilePointer);
                        return p;
                }
                else
                {
                        char err[100];
                        sprintf(err, "Error: expected identifier but received %s on line.char %s", token.tokenInstance, token.lineCharNum);
                        error(err);
                }
        }
	return p;
}
struct node* pick()
{
	struct node* p = createNode("pick");
	if (!strcmp(token.tokenInstance, "pick"))
        {
                //p->tokens[0].tokenID = token.tokenID;
                //p->tokens[0].tokenInstance = copyToken(token.tokenInstance);
                //p->tokens[0].lineCharNum = copyToken(token.lineCharNum);
                token = Driver(inputFilePointer);
                p->child1 = expr();
                p->child2 = pickBody();
                return p;
        }
	else
        {
                char err[100];
                sprintf(err, "Error: expected a statement but received %s on line.char %s", token.tokenInstance, token.lineCharNum);
                error(err);
        }
	return p;
}
//pickBody bnf
struct node* pickBody()
{
	struct node* p = createNode("pickBody");
	p->child1 = stat();
	if (!strcmp(token.tokenInstance, ":"))
	{
		//p->tokens[0].tokenID = token.tokenID;
                //p->tokens[0].tokenInstance = copyToken(token.tokenInstance);
                //p->tokens[0].lineCharNum = copyToken(token.lineCharNum);
		token = Driver(inputFilePointer);
		p->child2 = stat();
		return p;
	}
	else
	{
		char err[100];
                sprintf(err, "Error: expected : but received %s on line.char %s", token.tokenInstance, token.lineCharNum);
                error(err);
	}
	return p;
}

//RO bnf
struct node* RO()
{
	struct node* p = createNode("RO");
	if (!strcmp(token.tokenInstance, "<"))
	{
		p->tokens[0].tokenID = token.tokenID;
                p->tokens[0].tokenInstance = copyToken(token.tokenInstance);
                p->tokens[0].lineCharNum = copyToken(token.lineCharNum);
		token = Driver(inputFilePointer);
		return p;
	}
	else if (!strcmp(token.tokenInstance, ">"))
	{
		p->tokens[0].tokenID = token.tokenID;
                p->tokens[0].tokenInstance = copyToken(token.tokenInstance);
                p->tokens[0].lineCharNum = copyToken(token.lineCharNum);
		token = Driver(inputFilePointer);
		return p;
	}
	else if (!strcmp(token.tokenInstance, "=="))
	{
		p->tokens[0].tokenID = token.tokenID;
                p->tokens[0].tokenInstance = copyToken(token.tokenInstance);
                p->tokens[0].lineCharNum = copyToken(token.lineCharNum);
                token = Driver(inputFilePointer);
                return p;
	}
	else if (!strcmp(token.tokenInstance, "."))
	{
		p->tokens[0].tokenID = token.tokenID;
                p->tokens[0].tokenInstance = copyToken(token.tokenInstance);
                p->tokens[0].lineCharNum = copyToken(token.lineCharNum);
		token = Driver(inputFilePointer);
		if (!strcmp(token.tokenInstance, "."))
		{
			p->tokens[1].tokenID = token.tokenID;
                	p->tokens[1].tokenInstance = copyToken(token.tokenInstance);
                	p->tokens[1].lineCharNum = copyToken(token.lineCharNum);
			token = Driver(inputFilePointer);
			if (!strcmp(token.tokenInstance, "."))
			{
				p->tokens[2].tokenID = token.tokenID;
                		p->tokens[2].tokenInstance = copyToken(token.tokenInstance);
                		p->tokens[2].lineCharNum = copyToken(token.lineCharNum);
				token = Driver(inputFilePointer);
				return p;
			}
			else
        		{
                		char err[100];
                		sprintf(err, "Error: expected . but received %s on line.char %s", token.tokenInstance, token.lineCharNum);
                		error(err);
        		}
		}
		else
        	{
                	char err[100];
                	sprintf(err, "Error: expected . but received %s on line.char %s", token.tokenInstance, token.lineCharNum);
                	error(err);
        	}
	}
	else if (!strcmp(token.tokenInstance, "=!="))
	{
		p->tokens[0].tokenID = token.tokenID;
                p->tokens[0].tokenInstance = copyToken(token.tokenInstance);
                p->tokens[0].lineCharNum = copyToken(token.lineCharNum);
		token = Driver(inputFilePointer);
		return p;
	}
	else
	{
		char err[100];
                sprintf(err, "Error: expected < or > or . or =!= but received %s on line.char %s", token.tokenInstance, token.lineCharNum);
                error(err);
	}
	return p;
}
