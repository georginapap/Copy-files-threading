# include <stdio.h>
# include <sys/wait.h> /* sockets */
# include <sys/types.h> /* sockets */
# include <sys/socket.h> /* sockets */
# include <netinet/in.h> /* internet sockets */
# include <netdb.h> /* gethostbyaddr */
# include <unistd.h> /* fork */
# include <stdlib.h> /* exit */
# include <ctype.h> /* toupper */
# include <signal.h> /* signal */
#include <string.h>
#include <dirent.h>
#include <errno.h>
#include <pthread.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "queue.h"
#include "dataServer.h"
#include "list.h"
#define SIZEOFBUF 1024

int tp_size;
int port=0;
int queue_size=0;
nodeptr queue=NULL;
pthread_mutex_t mtx, mtx2;
pthread_cond_t cond_full_remove;
pthread_cond_t cond_empty_insert;

void path_finder(char *, int newsock, pthread_mutex_t*, listptr* lista);

int main(int argc, char *argv[])
{
	pthread_t t; 
	int i=0, l,  sock , newsock, rc;
	struct sockaddr_in server , client ;
	socklen_t clientlen ;
	struct sockaddr * serverptr =(struct sockaddr *)& server ;
	struct sockaddr * clientptr =(struct sockaddr *)& client ;
	struct hostent *rem ;
	while(argc)		//kratame ta orismata
	{
		if (strcmp(argv[i],"-s")== 0)
		{
			i++;
			tp_size =atoi(argv[i]);
		}
		if (strcmp(argv[i],"-p")== 0)
		{
			i++;
			port=atoi(argv[i]);
		}
		if (strcmp(argv[i],"-q")== 0)
		{
			i++;
			queue_size=atoi(argv[i]);
		}
		i++;
		if(i==7)
			break;
	}
	printf ("Servers parameters are:\n");
	printf("	port : %d\n",port);
	printf("	thread_pool_size : %d\n",tp_size);
	printf("	queue_size : %d\n",queue_size);
	pthread_t n[tp_size];// ta thread prwths grammhs
	
	/* Create socket */
	if (( sock = socket ( PF_INET , SOCK_STREAM , 0)) < 0)
		perror_exit ("socket ");
	server.sin_family = AF_INET ; /* Internet domain */
	server.sin_addr.s_addr = htonl(INADDR_ANY);
	server.sin_port = htons(port); /* The given port */
	printf("Server was succesfully initialized...\n");

	/* Bind socket to address */
	if ( bind(sock, serverptr, sizeof(server)) < 0)
		perror_exit ("bind");
	/* Listen for connections */
	if ( listen (sock , 5) < 0) 	//The call to the function ‘listen()’ with second argument as ’5′ specifies maximum number
		perror_exit (" listen ");		// of client connections that server will queue for this listening socket.
	printf("Listening for cnnections to port %d\n", port);	
	
	clientlen = sizeof(*clientptr);	
	
	 /* Initialize mutex and condition variable objects */
  	pthread_mutex_init(&mtx, NULL);			
 	pthread_cond_init(&cond_empty_insert, NULL);
	pthread_cond_init(&cond_full_remove, NULL);

	for(l=0; l<tp_size; l++)		//me thn sunarthsh ths pthread ousiastika tha vgazoume apo thn oura otan mas to epitrepei to mutex
	{
		rc=pthread_create(&n[l], NULL, threads_from_queue,(void *) &newsock);
		if (rc){
         		printf("ERROR in PTHREAD %d CREATION \n", rc);
         		exit(-1);
      		}
	}
	while (1) {				//gia kathe thread
		/* accept connection */
		if (( newsock = accept (sock , clientptr , &clientlen ))< 0) 
			perror_exit (" accept ");
		printf(" Accepted connection from localhost\n");
		/* Find client’s address */
		rc=pthread_create(&t, NULL, split_directory,(void *) &newsock);		//
		if (rc){
         		printf("ERROR in PTHREAD %d CREATION \n", rc);
         		exit(-1);
      		}
		
		//close (newsock ); /* parent closes socket to client */
	}
}


void perror_exit ( char * message ) {
	perror ( message );
	exit( EXIT_FAILURE);
}


 void path_finder( char *arg, int newsock, pthread_mutex_t *mutx, listptr* lista)		//vriskoume anadromika ola ta arxeia apo to path pou zhthse o client
{
	char thepath[150]; 
    	DIR *dirp;
    	struct dirent *dp;
        struct stat s;
	//printf("PATH : %s\n", arg);
    	if ((dirp = opendir(arg)) == NULL) {// current folder apo syspro 3 kai meta
        	perror("couldn't open '.'");
        	return;
    	}
	do {			//oso uparxoun files
        	errno = 0;
       		if ((dp = readdir(dirp)) != NULL) //diavazei ena ena ta arxeia h tous fakelous
		{// diavazei ta arxeia epistrefei deikth se mia domh ginetai null otan dn exei alla arxeia
			if((strcmp(dp->d_name,".")==0)||(strcmp(dp->d_name,"..")==0))
				continue;
			//printf("%s    ", dp->d_name);

			strcpy(thepath,arg);
			strcat(thepath, "/");
			strcat(thepath, dp->d_name);
			if( stat(thepath,&s) == 0 )
			{
			    if( s.st_mode & S_IFREG )		
			    {				//it's a file
				//tha ta vazw sthn oura
				//insert(&queue,thepath,newsock, length); 
				//printf("It's a FILE and the path : %s\n", thepath);
				//threads_to_queue(newsock,thepath, mutx );		//an einai arxeio to vazoume sthn oura
				insertlist(lista, thepath, newsock);
			    }
			    else if ( s.st_mode & S_IFDIR )
			    {				//it's a directory
				//printf("It's a FOLDER and the path : %s\n", thepath);				
				path_finder(thepath, newsock, mutx, lista);			//anadromikh klhsh
			    }
			    else
				{}
			}
		} 
	} while (dp != NULL);
	//printf("The path : %s\n", thepath);	
	
	closedir(dirp);
    	if (errno != 0)
        	perror("error reading directory");
    	
    	return;
}

void * split_directory(void * newsock ) {	//sth sunarthsh auth diavazoume oti mas stelnei o client sto socket
	
	listptr lista;
	lista=NULL;
	int *news = (int *) newsock;	
	char buf [1];
	int length,  k, found, i;
	//char * dir;
	char * directory;	
	char path[SIZEOFBUF];
	pthread_mutex_t mutx;
	pthread_mutex_init(&mutx, NULL);
	if (read(*news ,&length, sizeof(int))<0)
		perror_exit ("read "); 
	directory=malloc((length)*sizeof(char));
	if (directory==NULL)
	{
		perror("Error in malloc");
	}
	if (read (*news , directory, length)<0)
		perror_exit (" read "); 

	for(k=0; k<length; k++)
	{
		printf("%c",directory[k]);
	}
	printf("\n");
	pthread_mutex_lock(&mtx2);			//kleidwnoume to mutex wspou 
	printf("[ Thread : %d ] : About to scan the path \n", (int)pthread_self());
	printf(" : %s\n", directory);
	path_finder(directory, *news, &mutx, &lista);		//vriskoume anadromika ola ta arxeia apo to path
	found=sizelist(lista);
	if (write(*news  ,&found ,sizeof(int) )<0)
			perror_exit (" write files");
	for(i=0;i<found;i++)
	{
		threads_to_queue(lista->newsock, lista->path, &mutx);
		lista=lista->next;
	}
	
	pthread_mutex_unlock(&mtx2); //3ekleidwnoume to mutex
	char closeserver[5];
	if (read (*news , closeserver, 5)<0)
		perror_exit (" read "); 	

	if (strcmp(closeserver, "exit") == 0)
	{
		close(*news);
		deletelist(&lista);
		pthread_detach(pthread_self());
	}
		
	printf ("Closing connection .\n");
	//print(queue);
}

//gia threads defterhs grammhw- vgazoume apo thn oura
void* threads_from_queue(void * newsock)
{ //sunarthsh pou elegxei pote uparxei kati sthn oura. 
	int *news = (int *) newsock;
	int thesize;
	nodeptr thedata;
	thedata = malloc(sizeof(node));	
	while(1)
	{
		pthread_mutex_lock(&mtx);
		while (empty(queue)==0)//kenh oura ara perimenei
		{	
			pthread_cond_wait(&cond_full_remove, &mtx);
		}
		thesize=strlen(queue->path_)+1;
		thedata->path_ = malloc(sizeof(char)*thesize);
		strcpy((thedata)->path_, queue->path_);
		thedata->newsock_= queue->newsock_;
		thedata->mutx= queue->mutx;
		delete();			//diagrafh apo thn oura
		pthread_mutex_unlock(&mtx);	//3ekleidwma tou mutex
		pthread_cond_signal(&cond_empty_insert);	//stelnoume shma otiadeiase kati apo thn oura gia na pane ta threads prwths grammhs na valoun pali 
		if (send_to_client(thedata, thesize)== 0)	//stelnoume ta data sto socket gia ton  client
		{
			printf("We sent to the client : thepath= %s	newsock=%d	mutex=%p\n	\n", thedata->path_, thedata->newsock_, thedata->mutx);	
		}
	}
}

//gia threads prwths grammhs -vazoume sthn oura

void threads_to_queue(int newsock, char *thepath, pthread_mutex_t *mutx )
{
		int m=9;
		m= size(queue);
		pthread_mutex_lock(&mtx);		//kleidwma mutex
		while(queue_size==size(queue))		//an einai gemath oura tote wait
		{	//printf("WHILE\n");
			pthread_cond_wait(&cond_empty_insert, &mtx);
		}
		insert(&queue,thepath,newsock, mutx); 	//insert sto queue
		pthread_mutex_unlock(&mtx);		//3ekleidwma mutex
		pthread_cond_signal(&cond_full_remove);	//stelnoume shma oti mphka kati sthn oura gia na pane ta worker thread na to paroun 
}


int send_to_client(nodeptr thedata, int thesize)//To ulopoihsa me vash to paradeigma apo tis diafaneies The ROCK PAPER SCISSORS game
{
	
	pthread_mutex_lock(thedata->mutx);
	printf("Thread : %d : LOCKED\n", (int)pthread_self());
	//printf("Here is what we keep thepath= %s	newsock=%d	mutex=%p\n	\n", thedata->path_, thedata->newsock_, thedata->mutx);	
	//printf("SEND : %p\n", thedata->mutx);
	
	struct stat st;//gia na vroume to megethos tou arxeiou
	long size;
	stat((thedata)->path_, &st);
	size = st.st_size;
	if (write((thedata)->newsock_  ,&(thesize), sizeof(int))<0)
			perror_exit (" write ");
	if (write((thedata)->newsock_  ,(thedata)->path_, thesize)<0)
			perror_exit (" write ");
	if (write((thedata)->newsock_  ,&size,sizeof(long) )<0)
			perror_exit (" write ");
	printf("Thread : %d : UNLOCKED\n", (int)pthread_self());//to anagnwristiko tou thread
	send_data((thedata)->path_ , (thedata)->newsock_);
	pthread_mutex_unlock(thedata->mutx);
	return 0;
}


void send_data(char *path_ , int newsock_)	//se afth th sunarthsh pername ana pagesize sto socket ta periexomena tou arxeiou

{	int fd, bytes;
	long pgsize=getpagesize();
	char buffer[pgsize];
	if ((fd=open (path_, O_RDONLY))== -1)
		printf("error in opening file \n");
	while(1)
	{
		if ((bytes=read(fd ,buffer, pgsize))<0)
				perror_exit (" read ");
		if (bytes>0)
		{	if (write(newsock_, buffer,pgsize)<0)	
				perror_exit (" read ");
		}
		else
			break;
		buffer[0]='\0';
	}
	close(fd);
}

//dataServer -p <port> -s <thread_pool_size> -q <queue_size>

//./server -p 2021 -s 7 -q 6
// gcc -o serv data.c queue.c -lpthread




