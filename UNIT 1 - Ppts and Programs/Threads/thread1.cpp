#include <pthread.h>
#include <iostream>
#include <unistd.h>



#define NTHREADS 5
#define COUNT 5000

struct info
{
	int value;
	char ch;
};

void * change_x(void *x_void_ptr)
{

	info *x_ptr = (info *)x_void_ptr;

	x_ptr->ch = 'C';
    x_ptr->value = 100;
	return NULL;
}

int main()
{
	static info x[NTHREADS];
	pthread_t tid[NTHREADS];
/*	int finalX =0;
	for(int i=0;i<NTHREADS;i++)
		finalX = finalX + x[i];
	printf("Initial value of x: %d\n", finalX);*/

	for (int i = 0; i < NTHREADS; i++)
	{

		if (pthread_create(&tid[i], NULL, change_x, (void *) &x[i]))
		{
			std::cerr << "Error creating thread" << std::endl;
			return 1;
		}
	//	pthread_join(tid[i], NULL);
		
	}
	for (int i = 0; i < NTHREADS; i++)
		pthread_join(tid[i], NULL);
	int finalX = 0;
	for(int i=0;i<NTHREADS;i++)
		std::cout << x[i].ch << " " << x[i].value << std::endl;
	return 0;
}
