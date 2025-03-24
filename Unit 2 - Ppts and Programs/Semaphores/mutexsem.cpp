#include <pthread.h>
#include <iostream>

#define NTHREADS 5000
#define COUNT 5000

struct arguments
{
	int *value;
	pthread_mutex_t *semB;
};

void *inc_x(void *x_void_ptr)
{
	arguments *x_ptr = (arguments *)x_void_ptr;
	pthread_mutex_lock(x_ptr->semB);
	for (int i = 0; i < COUNT; i++)
	{
		(*x_ptr->value)++;
		
	}
	pthread_mutex_unlock(x_ptr->semB);
	return NULL;
}

int main()
{
	static int x = 0;
	std::string temp;
	static pthread_mutex_t semB;
	pthread_t tid[NTHREADS];
	pthread_mutex_init(&semB, NULL);
	std::cout << "Initial value of x: " << x << std::endl;
	arguments arg[NTHREADS];
	
	for (int i = 0; i < NTHREADS; i++)
	{
		arg[i].semB = &semB;
		arg[i].value = &x;
		if (pthread_create(&tid[i], NULL, inc_x, &arg[i]))
		{
			std::cerr << "Error creating thread\n";
			return 1;
		}
	}
	// Wait for the other threads to finish.
	for (int i = 0; i < NTHREADS; i++)
		pthread_join(tid[i], NULL);
	std::cout << "Final value of x: " << x << std::endl;
	return 0;
}
