#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "list.h"

void insertlist(listptr *ptrnode,  char *path, int newsock)
{   
    listptr temp;
    int i=0;
    temp = *ptrnode;
    if (temp == NULL)
    { //an einai to prwto stoixeio
        *ptrnode = malloc(sizeof(nodelist)); /* Space for new node */
	(*ptrnode)->path = malloc(sizeof(char)*(strlen(path)+1));
	strcpy((*ptrnode)->path, path);
        (*ptrnode)->newsock = newsock;
        (*ptrnode)->next = temp;
    }
    else
    {// an dn einai to prwto stoixeio
        while (*ptrnode != NULL)/* Go to end of list */
        {   ptrnode = &((*ptrnode)->next);/* Prepare what we need to change */
        }      
        *ptrnode = malloc(sizeof(nodelist)); /* Space for new node */
       (*ptrnode)->path = malloc(sizeof(char)*(strlen(path)+1));
	strcpy((*ptrnode)->path, path);
        (*ptrnode)->newsock = newsock;
        (*ptrnode)->next = temp;
        (*ptrnode)->next = NULL;
    }
}

int sizelist(listptr n)
{  
    int node_count=0;
    while (n != NULL)
    {
        n = n->next;
        node_count++;
         
    }
    return (node_count);
}



int deletelist(listptr *ptrnode)
{
    listptr tempnode;
    while ((*ptrnode) != NULL) 
    {
            tempnode = *ptrnode;
            *ptrnode = (*ptrnode)->next;
	    free(tempnode->path);
            free(tempnode);
	    ptrnode = &((*ptrnode)->next);
    }
            
        return 0;
}




