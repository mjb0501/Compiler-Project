#include "scanner.h"

struct node
{
	char label[50];
	struct tokeType tokens[10];
	struct node *child1;
	struct node *child2;
	struct node *child3;
	struct node *child4;
};

struct node* parser(FILE *);
void print(struct node*, int);
void freeTree(struct node*);
