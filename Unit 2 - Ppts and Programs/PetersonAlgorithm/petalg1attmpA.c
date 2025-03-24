#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/shm.h>

#define RUNTIMES 7

int main()
{
	pid_t pid;
	key_t key = 1234;
	int shmid;
	int *turn;
	shmid=shmget(key,sizeof(int),IPC_CREAT | 0666);
	turn=(int *) shmat(shmid,0,0);
	*turn=0;
	for (int i=0;i<RUNTIMES;i++)
	{
		while(*turn!=0) /*do nothing*/ ;
                printf("Process 0 entering the CS\n");
                sleep(1);
                printf("Process 0 exiting the CS\n");
		*turn=1;
	}
	shmdt(turn);
	return 0;
}
