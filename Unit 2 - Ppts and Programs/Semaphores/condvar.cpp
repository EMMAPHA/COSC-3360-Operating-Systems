#include <pthread.h>
#include <iostream>
#include <cstring>
#include <unistd.h>

#define NTHREADS 5
#define FAMILYNAME "CASTRO"

struct argFromMain
{
	char *famName;
	pthread_mutex_t * bsemPTR;
	pthread_cond_t *condVar;
	int *memb;
};

void *access_house(void *family_void_ptr)
{
	char fam[20];
	struct argFromMain *ptr = (struct argFromMain *) family_void_ptr;
	strcpy(fam,(char *) ptr->famName);
	printf("%s member arrives to the house\n", fam);
	pthread_mutex_lock(ptr->bsemPTR);
	std::cout << fam << " member arrives to the house" << std::endl;
	if (strcmp(fam,FAMILYNAME)!=0)
		pthread_cond_wait(ptr->condVar, ptr->bsemPTR);
	(*ptr->memb)++;	
	std::cout << fam << " member inside the house" << std::endl;
	pthread_mutex_unlock(ptr->bsemPTR);
	sleep(5);
 	pthread_mutex_lock(ptr->bsemPTR);
	std::cout << fam << " member leaving the house" << std::endl;
	(*ptr->memb)--;
	if (strcmp(fam,FAMILYNAME) == 0 && *ptr->memb == 0)
		pthread_cond_broadcast(ptr->condVar);
 	pthread_mutex_unlock(ptr->bsemPTR);
	return NULL;
}

int main()
{

 	pthread_t tid[NTHREADS];
	static pthread_mutex_t bsem;
	static int members=0;
	static pthread_cond_t empty = PTHREAD_COND_INITIALIZER;
	char family[NTHREADS][20];
	struct argFromMain arg[NTHREADS];
   	pthread_mutex_init(&bsem, NULL); // Initialize access to 1

	for(int i=0;i<NTHREADS;i++)
	{
		if(i%2 == 0)
			strcpy((char *) &family[i],"RINCON");
		else
			strcpy((char *)&family[i],"CASTRO");
		arg[i].famName = family[i];
		arg[i].bsemPTR = &bsem;
		arg[i].condVar = &empty;
		arg[i].memb = &members;
		if(pthread_create(&tid[i], NULL, access_house,(void *)&arg[i])) 
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
