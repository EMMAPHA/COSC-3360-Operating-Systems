#include <pthread.h>
#include <iostream>
#include <cstring>
#include <unistd.h>


#define NTHREADS 5
#define FAMILYNAME "CASTRO"

static pthread_cond_t empty = PTHREAD_COND_INITIALIZER;
static pthread_mutex_t bsem;
static int members=0;

void *access_house(void *family_void_ptr)
{
	char fam[20];
	char *ptr = (char *) family_void_ptr;
	strcpy(fam,ptr);
	pthread_mutex_lock(&bsem);
    std::cout << fam << " member arrives to the house" << std::endl;
	if (strcmp(fam,FAMILYNAME)!=0)
		pthread_cond_wait(&empty, &bsem);
	members++;	
    std::cout << fam << " member inside the house" << std::endl;
	pthread_mutex_unlock(&bsem);
	sleep(5);	
 	pthread_mutex_lock(&bsem);
    std::cout << fam << " member leaving the house" << std::endl;
	members--;
	if (strcmp(fam,FAMILYNAME) == 0 && members == 0)
		pthread_cond_broadcast(&empty);
 	pthread_mutex_unlock(&bsem);
	return NULL;
}

int main()
{

 	pthread_t tid[NTHREADS];
	char family[NTHREADS][20];
   	pthread_mutex_init(&bsem, NULL); // Initialize access to 1

	for(int i=0;i<NTHREADS;i++)
	{
		if(i%2 == 0)
			strcpy((char *) &family[i],"RINCON");
		else
			strcpy((char *)&family[i],"CASTRO");
		if(pthread_create(&tid[i], NULL, access_house,(void *)&family[i])) 
		{
			fprintf(stderr, "Error creating thread\n");
			return 1;
		}
		
	}
	// Wait for the other threads to finish.
	for (int i = 0; i < NTHREADS; i++)
        	pthread_join(tid[i], NULL);
	return 0;
}
