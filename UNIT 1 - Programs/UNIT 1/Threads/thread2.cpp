#include <pthread.h>
#include <iostream>
#include <string>

struct arg
{
    int value;
    std::string array;
};

void *inc_x(void *x_void_ptr)
{

	struct arg *x_ptr = (struct arg *)x_void_ptr;
    x_ptr->array = x_ptr->value+'0';
	return NULL;
}

int main()
{
	static struct arg x[5];
 	pthread_t tid[5];


	for(int i=0;i<5;i++)
	{
        x[i].value = i;
		if(pthread_create(&tid[i], NULL, inc_x, &x[i])) 
		{
			fprintf(stderr, "Error creating thread\n");
			return 1;

		}		

	}
	
	// Wait for the other threads to finish.
	for (int i = 0; i < 5; i++)
        	pthread_join(tid[i], NULL);
	for (int i = 0; i < 5; i++)
        std::cout << x[i].array << std::endl;
	return 0;
}
