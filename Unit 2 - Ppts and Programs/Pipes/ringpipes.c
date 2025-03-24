#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <stdlib.h>

#define MAXCHILD 20
#define NO_OF_CHILDREN 10
#define MAXBUF 100


void child(int readend, int writeend, int pid, int input[10]);

int main()
{
	int pipefd[MAXCHILD + 1][2];
	int noofchildren = 0;
	int nchild = NO_OF_CHILDREN;
	int ret;
	int pid;
	int i;
	char buf[MAXBUF];
	int input[30]={0,1,0,1,0,0,1,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,1,-1};
	int token;
	for(i = 0; i <= nchild; i++)
	{
    		if (pipe(pipefd[i]) < 0)
    		{
        		printf("Pipe Creation failed\n");
        		exit(1);
    		}
	}
	
	printf("Pipes created\n");

	for(i = 0; i < nchild; i++)
	{

    		if ((pid=fork())==0)
    		{
        		child(pipefd[i][0], pipefd[i + 1][1],i+1,input);
    		}
    		//These ends are no longer needed in parent.
    		close (pipefd[i][0]);
    		close(pipefd[i + 1][1]);
	}

	token=0;
	while(1) 
	{
    		//Write to the pipe which is connected to the first child.
		if (input[token]==1)
			printf("Process 0 is sending information\n");
		token=token+1;
		write(pipefd[0][1],&token, sizeof(int));    
		//Read from the pipe, which is attached to the last child created
		read(pipefd[nchild][0], &token, sizeof(int));
    		if (token == -1)
    		{
			write(pipefd[0][1],&token, sizeof(int));
        		break;
    		}
	} 

//By this time all other children, would have exited
//Let's wait to see this 

	while(1)
	{
    		ret = wait(NULL);
    		if (0 > ret) //Returns -ve when no more child is left.
    		{
        		break;
    		}
    		noofchildren++;
	}

	printf("My %d children terminated! Now I am exiting\n", noofchildren);
	exit(0);
}

void child(int readend, int writeend, int pid, int input[10])
{
	int token;
	char buf[MAXBUF];
	int ret,t;

//	dup2(readend, 0);
//	dup2(writeend,1);   

	while(1)
	{
    		ret = read(readend, &token, sizeof(token) );
		if (token >= 0 && input[token] == 1)
			printf("Process %d is sending information\n",pid); 
    		if (token == -1 || input[token]==-1)
    		{
			t= -1;
   			write(writeend,&t,sizeof(int));  
      			_exit(0);
    		}
		t=token+1; 
		write(writeend,&t,sizeof(int));
	}

}
