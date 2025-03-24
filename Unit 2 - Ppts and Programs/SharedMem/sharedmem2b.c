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
	printf("Printing the value of x = %d\n",*x);
	shmdt(x);
	shmctl(shmid,IPC_RMID,NULL);
	return 0;
}
