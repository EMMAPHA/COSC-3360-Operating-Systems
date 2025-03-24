#include <sys/types.h> 
#include <sys/ipc.h> 
#include <sys/shm.h> 
#include <stdio.h> 
#include <semaphore.h> 
#include <sys/stat.h> 
#include <fcntl.h>
#include <unistd.h>		// RWX
						// 110
int main()
{
	sem_t mysem;
	int value=10;
	unsigned int initial_value = 1;
	sem_init(&mysem, O_CREAT | 0600, initial_value);
	sem_wait(&mysem);
	sem_getvalue(&mysem,&value);	
	printf("Semaphore Value %d\n",value);
	sleep(2);
 	sem_post(&mysem);
	sem_destroy(&mysem);
}
