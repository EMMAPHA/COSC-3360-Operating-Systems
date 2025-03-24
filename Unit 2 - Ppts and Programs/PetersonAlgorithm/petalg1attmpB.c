#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/shm.h>

#define RUNTIMES 5

int main()
{
	
	pid_t pid;
	key_t key = 1234;
	int shmid;
	int *turn;
	sleep(1);
	shmid=shmget(key,sizeof(int),IPC_CREAT | 0666);
	turn=(int *)shmat(shmid,0,0);
	for (int i=0;i<RUNTIMES;i++)
	{
		while(*turn!=1) /*do nothing*/ ;
                printf("Process 1 entering the CS\n");
                sleep(1);
                printf("Process 1 exiting the CS\n");
		*turn=0;
	}
	shmdt(turn);
    shmctl(shmid,IPC_RMID,NULL);
	return 0;
}
