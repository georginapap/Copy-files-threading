/*
 
Gia to arxeio afto, h diaxeirish ths ouras vasisthke se sunarthseis apo to
mathhma ths Eisagwghs ston Programmatismo. Xrhsimopoihthhkan vash aytwn poy 
paradwthhkan sthn askhsh 2 toy mathhmatos
 
*/
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <string.h>
#include"queue.h"
 


int empty(nodeptr n)    //elegxos gia kenh oura
{
    if (n == NULL)
        return 0;
    else
        return 1; 
	
}

void insert(nodeptr *ptrnode, char *thepath,  int newsock, pthread_mutex_t *mutx) 
{   //eisagwgh sthn oura
    nodeptr temp;
    int thesize,i=0, k;
    temp = *ptrnode;
    thesize=strlen(thepath)+1;
    if (temp == NULL)
    { //an einai to prwto stoixeio
        *ptrnode = malloc(sizeof(node)); /* Space for new node */
        (*ptrnode)->newsock_ = newsock;
       
	(*ptrnode)->mutx = mutx;
	(*ptrnode)->path_ = malloc(sizeof(char) * (thesize));
	
	strcpy((*ptrnode)->path_, thepath);
	(*ptrnode)->path_[thesize] = '\0';
	printf("[ Thread : %d ] : ", (int)pthread_self());
	printf("Adding file %s to the queue... \n",(*ptrnode)->path_);
        (*ptrnode)->next = temp;
    }
    else
    {// an dn einai to prwto stoixeio
        while (*ptrnode != NULL)/* Go to end of list */
        {   ptrnode = &((*ptrnode)->next);/* Prepare what we need to change */
        }      
        *ptrnode = malloc(sizeof(node)); /* Space for new node */
         (*ptrnode)->newsock_ = newsock;
	(*ptrnode)->mutx = mutx;
	(*ptrnode)->path_ = malloc(sizeof(char) * (thesize));
	strcpy((*ptrnode)->path_, thepath);
	(*ptrnode)->path_[thesize] = '\0';
	printf("[ Thread : %d ] : ", (int)pthread_self());
	printf("Adding file %s to the queue... \n",(*ptrnode)->path_);
        (*ptrnode)->next = NULL;
    }printf("Adding file \n");
}
 
void print()
//print ta stoixeia ths ouras .
{
    while (queue != NULL)
    {
        printf("< %d >   < %s >\n",queue->newsock_, queue->path_);
        queue = queue->next;
    }
	if(queue==NULL)
		printf("KENH OURA\n");    
}
 
int size(nodeptr n)
{   //epistrefetai to megethos thw ouras
    int node_count=0;
    while (n != NULL)
    {
        n = n->next;
        node_count++;
         
    }
    return node_count ;
}



nodeptr delete()
//deletes elements from the queue
{	
    	nodeptr tempnode;
	int thesize;
    	if ((queue) != NULL) /* Visit list elements up to the end */
    	{    
            	tempnode = queue;
		queue = queue->next;
		free(tempnode->path_);
            	
            /* Bypass deleted element */
		
            	 /* Free memory for the corresponding node */
		free(tempnode);
            	return ;
        	
	}
	 
}

