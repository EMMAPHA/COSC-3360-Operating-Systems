#include <pthread.h>
#include <iostream>
#include <unistd.h>

#define NTHREADS 5

struct arguments
{
    int value;
    int result;
};

void *threadFunction(void *arg_void_ptr)
{
    arguments *arg = (arguments *)arg_void_ptr;
    arg->result = arg->value + 10;
    return nullptr;
}

int main()
{
    pthread_t tid[NTHREADS];
    arguments arg[NTHREADS];
    for (int i = 0; i < NTHREADS; i++)
    {
        arg[i].value =i;
        if (pthread_create(&tid[i], NULL, threadFunction, &arg[i]))
        {
            fprintf(stderr, "Error creating thread\n");
            return 1;
        }
    }
    // Wait for the other threads to finish.
    for (int i = 0; i < NTHREADS; i++)
        pthread_join(tid[i],nullptr);

    for (int i = 0; i < NTHREADS; i++)
    {
        std::cout << arg[i].result << std::endl;
    }
    return 0;
}
