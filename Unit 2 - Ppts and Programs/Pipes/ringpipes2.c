#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <iostream>

using namespace std;

int main( int argc, char *argv[] )
{
	int fd[2];
	int childpid;
	int nprocs;
	int error;
	int i;
	int input[10]={1,0,1,0,0,1,-1};
	int token;

//process id of child
//total number of processes in the ring
//return value from dup2 call

	if ( argc < 2 ||  (nprocs = atoi( argv[1])) <= 0  ){
  		cout << "Usage: " << argv[0] << " nprocesses" << endl;
  		exit ( 1 );
	}
	
	if ( pipe ( fd ) == -1 ){
  		perror("Could not create pipe");
  		exit ( 1 );	
	}

	if ( dup2 ( fd[0], STDIN_FILENO ) == -1  || dup2 ( fd[1], STDOUT_FILENO ) == -1 ) {
  		perror ("Could not dup pipes");
  		exit ( 1 );
	}
	close ( fd[0] );
	close ( fd[1] );
	for ( i = 1; i < nprocs; i++ ) {

		if ( pipe( fd ) == -1 ) {
  			fprintf(stderr, "Could not create pipe %d: %s\n",i, strerror ( errno ) );
  			exit ( 1 );
		}
		if ( ( childpid = fork() ) == -1 ) {
  			fprintf(stderr, "Could not create child %d: %s\n",i, strerror ( errno ) );
			exit ( 1 ); 
		}
		if ( childpid > 0 )   //for parent, reassign stdout
   			error = dup2 ( fd[1], STDOUT_FILENO );
		else                  //for child, reassign stdin
   			error = dup2 ( fd[0], STDIN_FILENO );
		if ( error == -1 ) {
  			fprintf(stderr, "Could not dup pipes for iteration %d: %s\n",i, strerror( errno ) );
  			exit ( 1 );
  		}
  		close ( fd[0] );
  		close ( fd[1] );
  		if ( childpid )
			break; 
	}
	if (i==0)
	{
		token=0;
		write(1,&token, sizeof(token));
	}
	while(1)
	{
		fprintf(stderr,"Process %d is sending information\n",i);		
		read(0,&token, sizeof(token));
		fprintf(stderr,"Process %d is sending information\n",i);
		if (input[token] == -1)
		{
			write(1,&token, sizeof(token));
			break;
		}
		if (input[token] == 1)
			fprintf(stderr,"Process %d is sending information\n",i);
		token=token+1;
		write(1,&token, sizeof(token));
			
	
	}
	return 0; 
}
