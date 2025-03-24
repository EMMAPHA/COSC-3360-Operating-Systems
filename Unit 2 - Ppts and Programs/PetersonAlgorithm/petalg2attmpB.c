#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/shm.h>

#define RUNTIMES 5

int main()
{
	pid_t pid;
	key_t key = 1236;
	int shmid;
	int *flag;
	//sleep(1);
	shmid=shmget(key,sizeof(int)*2,IPC_CREAT | 0666);
	flag=(int *)shmat(shmid,0,0);
	for (int i=0;i<RUNTIMES;i++)
	{
		while(flag[0]) /*do nothing*/ ;
		flag[1]=1;
		printf("Process 1 entering the CS\n");
		sleep(1);
		printf("Process 1 exiting the CS\n");
		flag[1]=0;
	}
	shmdt(flag);
    shmctl(shmid,IPC_RMID,NULL);
	return 0;
}
