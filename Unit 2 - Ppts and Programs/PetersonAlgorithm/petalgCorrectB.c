#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/shm.h>

#define RUNTIMES 1

int main()
{
	pid_t pid;
	key_t key = 1234;
	int shmid;
	int *flag;
	sleep(1);
	shmid=shmget(key,sizeof(int)*3,IPC_CREAT | 0666);
	flag=(int *)shmat(shmid,0,0);
	for (int i=0;i<RUNTIMES;i++)
	{
		flag[1]=1;
		flag[2]=0;
		while(flag[0] && flag[2] == 0) /*do nothing*/ ;
		printf("Process 1 entering the CS\n");
		sleep(1);
		printf("Process 1 exiting the CS\n");
		flag[1]=0;
	}
	shmdt(flag);
    shmctl(shmid,IPC_RMID,NULL);
	return 0;
}
