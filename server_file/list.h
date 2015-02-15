typedef struct nodelist *listptr;
typedef struct nodelist
{      
    char *path;
    int newsock;
    listptr next;
 
}nodelist;


int deletelist(listptr *ptrnode);
void insertlist(listptr *ptrnode, char* path, int newsock);
int sizelist(listptr n);
