#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/shm.h>

int main()
{
	pid_t pid;
	key_t key = 1234;
	int shmid;
	int *x;
	shmid=shmget(key,sizeof(int),IPC_CREAT | 0666);
	x=(int *)shmat(shmid,0,0);
	printf("Enter the value of X: ");
	scanf("%d",x);
	shmdt(x);
	return 0;
}
