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
	shmid=shmget(key,sizeof(int)*2,IPC_CREAT | 0666);
	flag=(int *)shmat(shmid,0,0);
	flag[0]=0;
	flag[1]=0;
	for (int i=0;i<RUNTIMES;i++)
	{
		while(flag[1]) /*do nothing*/ ;
		flag[0]=1;
                printf("Process 0 entering the CS\n");
                sleep(1);
                printf("Process 0 exiting the CS\n");
		flag[0]=0;
	}
	shmdt(flag);
	return 0;
}
