#include <sys/wait.h>
#include <sys/types.h> 
#include <sys/ipc.h> 
#include <sys/shm.h> 
#include <stdio.h> 
#include <semaphore.h> 
#include <sys/stat.h> 
#include <unistd.h>
#include <fcntl.h>

#define SEM_S_INIT_VALUE 1
#define SEM_N_INIT_VALUE 0 
#define SEM_E_INIT_VALUE 3	// Size of buffer 
#define NPRODUCERS 5
#define NCONSUMERS 2
int main()
{
	sem_t *semE, *semS, *semN;
	pid_t pid;
	char name1[] = "bproducerS";
	char name2[] = "bproducerN";
	char name3[] = "bproducerE"; 
	sem_unlink(name1);
	sem_unlink(name2);
	sem_unlink(name3);
	semS = sem_open(name1, O_CREAT,0600, SEM_S_INIT_VALUE);
	semN = sem_open(name2, O_CREAT,0600, SEM_N_INIT_VALUE);
	semE = sem_open(name3, O_CREAT,0600, SEM_E_INIT_VALUE);
	pid=fork();
	if (pid == 0) // Consumer
	{
		sleep(15);
		for (int i = 0; i < NCONSUMERS; i++)
        	{
                	sem_wait(semN);
                	sem_wait(semS);
                	printf("Consumer Entering CS\n");
                	printf("Taking element from buffer\n");
                	sleep(1);
                	printf("Consumer Exiting CS\n");
                	sem_post(semS);
                	sem_post(semE);
                	printf("Consuming element\n");
                	sleep(2);
        	}
		_exit(0);
	}
	else	// Producer
	{
		for (int i = 0; i < NPRODUCERS; i++)
		{
			printf("Producing element\n");
			sem_wait(semE);
			sem_wait(semS);
			printf("Producer Entering CS\n");
			printf("Appending element to buffer\n");
			printf("Producer Exiting CS\n");
			sleep(1);
			sem_post(semS);
			sem_post(semN);
		}
	} 
	wait(0);	// Waiting child process
	sem_unlink(name1);
	sem_unlink(name2);
	sem_unlink(name3);
}
