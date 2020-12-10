typedef struct node {
	char file[100];
	struct node *next;
}node;
typedef struct list {
	node *front;
	node *end;
}list;
void init (list *);
void add (list *, char[]);
char* del (list*);
int isempty (list *l);

