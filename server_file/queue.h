typedef struct node *nodeptr;
typedef struct node
{       int newsock_;
    	char *path_;
	pthread_mutex_t *mutx;
 	nodeptr next;
 
}node;

extern nodeptr queue;


int empty(nodeptr n);
void insert(nodeptr *ptrnode, char *thepath,  int newsock, pthread_mutex_t *mutx );
nodeptr delete();
void print();
int size(nodeptr n);

