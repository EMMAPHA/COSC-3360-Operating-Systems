#include <pthread.h>
#include <iostream>
#include <unistd.h>

#define NTHREADS 5

void *threadFunction(void *arg_void_ptr)
{
    int arg = *((int *)arg_void_ptr);
    int *result = new int;
    *result = arg+10;
    return (void *) result;
}

int main()
{
    pthread_t tid[NTHREADS];
    int arg[NTHREADS];
    int *result[NTHREADS];
    for (int i = 0; i < NTHREADS; i++)
    {
        arg[i] =i;
        if (pthread_create(&tid[i], NULL, threadFunction, &arg[i]))
        {
            fprintf(stderr, "Error creating thread\n");
            return 1;
        }
    }
    // Wait for the other threads to finish.
    for (int i = 0; i < NTHREADS; i++)
        pthread_join(tid[i],(void **)&result[i]);

    for (int i = 0; i < NTHREADS; i++)
    {
        std::cout << *result[i] << std::endl;
        delete result[i];
    }
    return 0;
}
