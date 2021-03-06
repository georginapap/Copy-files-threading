# include <stdio.h>
# include <sys/types.h> /* sockets */
# include <sys/socket.h> /* sockets */
# include <netinet/in.h> /* internet sockets */
# include <unistd.h> /* read , write , close */
# include <netdb.h> /* gethostbyaddr */
# include <stdlib.h> /* exit */
# include <string.h> /* strlen */
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
#include <libgen.h>
#include <fcntl.h>
#include <errno.h>
#include <pthread.h>
#include"remoteClient.h"

char *ip;
int port=0;
char directory[256];

int main(int argc, char *argv[])
{
	int x, counter=0, found;
	int i=0;
	int length,l, k, m, sock,j;
	long size;
	char buf [256];
	char *path;
	 char * path2;

	struct sockaddr_in server ;
	struct sockaddr * serverptr = ( struct sockaddr*)&server ;
	struct hostent *rem ;
	struct in_addr ip_addr;

	while(argc)	//kratame ta orismata 
	{ 	
		if (strcmp(argv[i],"-i")== 0){	
			printf("argv[i]= %s\n",argv[i]);
			i++;
			ip=argv[i];
		}
		if (strcmp(argv[i],"-p")==0){
			i++;
			port=atoi(argv[i]);			
		}
		if (strcmp(argv[i],"-d")==0){
			i++;
			strcpy(directory,argv[i]);
			length=strlen(directory)+1 ;
		}
		i++;
		if(i==7)
			break;
	}
	printf ("Client's parameters are:\n");
	printf("	ServerIP : %s\n",ip);
	printf("	port : %d\n",port);
	printf("	path : ");
	for(k=0; k<length; k++)
	{
		printf("%c",directory[k]);
	}
	printf("\n");
	/* Create socket */
	if (( sock = socket ( PF_INET , SOCK_STREAM , 0)) < 0)
		perror_exit ("socket ");


	if ((rem = gethostbyname(ip)) == NULL) {	
	   herror("Cgethostbyname"); exit(1);
    	}

	server.sin_family = AF_INET ; /* Internet domain */
	memcpy(&server.sin_addr, rem->h_addr, rem->h_length );
	server.sin_port = htons(port); /* Server port */

	printf("Connecting to %s on port %d\n", rem->h_name, port);

	/* Initiate connection */
	if ( connect(sock , serverptr , sizeof(server)) < 0)
		perror_exit(" connect ");
	if ( write(sock , &length, sizeof(int)) < 0)
		perror_exit (" write ");
	
	if ( write(sock , directory, length) < 0)
		perror_exit (" write ");
	if (read(sock ,&found, sizeof(int))<0)	//diavazoume apo to socket to mhkos tou path (l)..
		perror_exit (" read ");

	for(j=0;j<found;j++) {	
		
		if (read(sock ,&l, sizeof(int))<0)	//diavazoume apo to socket to mhkos tou path (l)..
				perror_exit (" read ");
		path=malloc((l)*sizeof(char));
	 	if (path==NULL)
		{
			perror("Error in malloc");
		}
		i = 0;
		while ( i < l )				//...to path (path[l]...
			if ( i < ( i+= read (sock, &path[i], l - i)))
				printf("WRONG READING\n");
		
		if (read(sock ,&size, sizeof(long))<0)	//...kai to megethos tou arxeiou (size))
				perror_exit (" read ");
		
		path2 = malloc(sizeof(char) * l);
		strcpy(path2,path);
		split_function(path2, sock,size );
		 
	}
	char closeserver[]="exit";
	if ( write(sock , closeserver, strlen(closeserver)) < 0)
		perror_exit (" write ");
	close (sock); /* Close socket and exit */
	
}
int read_data(int fd, int sock, long size, char * tok)	//to arxeio einai anoixto kai pame na tou grayoume ta bytes symfwna me to pagesize

{
	int counter=0, bla=0;
	int bytes_read=0;
	int pgsize=4096;
	char buffer[pgsize];
	while(bla < size)
	{
		if ((counter = read(sock,buffer, pgsize))<0)
			perror_exit (" read ");
		//printf("COUNTER : %d\n", counter);
		bla=counter+bla;
		//printf("BUFFER : %d\n", bla);
		if (size < 4096)
		{
			if (write(fd, buffer,strlen(buffer))<0)	
				perror_exit (" read ");
		}
		else
		{
			if (write(fd, buffer,4096)<0)	
				perror_exit (" read ");
		}
		buffer[0]='\0';
	}
	printf("The file %s with size %ld just created!\n",tok , size);
	
}

//Gia thn ulopoihsh auths ths sunarthshs, vasisthka sto parakatw link
//http://stackoverflow.com/questions/9210528/split-string-with-delimiters-in-c

void split_function(char *path2, int sock, long size)	//sth sunarthsh auth spame to path kai dhmiourgoume enan enan kata vathos tous katalogous...
{
	int i, length, counter, parts = 1, fd;
	char* tokens, *tok, slash[2], telia[2]={'.','\0'}, *newpath;
	DIR *dir;
    	struct dirent *dp;
	struct stat s;
	FILE * thefile;

	slash[0]='/';
	slash[1]='\0';

	counter = 0;
	length = strlen(path2);
	newpath=malloc(sizeof(char)*(length+1));
	
	strcpy(newpath, telia);
	strcat(newpath, path2);
	length = strlen(newpath);

	for (i=0; i<length; i++)
	{
		if (newpath[i]=='/') 
		{
			counter++;
		}
	}
	tok = malloc(sizeof(char)* (strlen(newpath)+1));
	tokens = strtok(path2, "/");
	strcpy(tok, telia);
	while (tokens != NULL)
    	{//gia kathe dir tou path
		strcat(tok, slash);
 		strcat(tok, tokens);
		if (counter != parts)
		{
			if( stat(tok,&s) == 0 ){//uparxei to onoma
				if ( s.st_mode & S_IFDIR ){
					//printf("FOLDER EXISTS\n");
				}
			}
			else{
				//printf("NEW FOLDER\n");
				if(mkdir(tok, 0777) < 0)
					perror("mkdir");
			} 
		}
		else// ftasame sto onoma tou katalogou apo to path	//...otan vroume to onoma tou arxeiou pros dhmiourgia apo to path
		{
			if ( stat (tok, &s ) != 0 && errno == ENOENT ) //..kai dn uparxei ston katalogo to dhmiourgoume
			{
				if ((fd=open (tok, O_CREAT | O_WRONLY, S_IRWXU))== -1)
					printf("error in opening another file \n");
			}
			else						//... an uparxei to svhnume kai to 3ana dhmiourgoume
			{
				unlink(tok);
				
				if ((fd=open (tok, O_CREAT | O_WRONLY, S_IRWXU))== -1)
					printf("error in opening another file \n");
			} 
			read_data(fd, sock, size, tok);	//me afth th sunarthsh tha pame na grayoume to periexomeno tou kathe arxxeioupou dhmiourghsame 
			close(fd);
		}
		tokens = strtok (NULL, "/");
		parts++;
    	}
}

void perror_exit ( char * message )
{
	perror ( message );
	exit( EXIT_FAILURE);
}
//gcc -o client remoteClient.c
//remoteClient -i <server_ip> -p <server_port> -d <directory>
//./client -i 127.0.0.1 -p 2010 -d testdir
//./client -i 127.0.0.1 -p 2021 -d /home/user/Documents/testdir

