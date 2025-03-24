#include <pthread.h>
#include <iostream>
#include <unistd.h>

struct arguments
{
    int value;
    std::string **str;
};

void *threadFunction(void *arg_void_ptr)
{
    arguments *arg_ptr = (arguments *)arg_void_ptr;
    *arg_ptr->str = new std::string("I am child thread " + std::to_string(arg_ptr->value));
    return NULL;
}

int main()
{
    std::string *strArr[2];
    pthread_t tid[2];
    arguments arg[2];
    for (int i = 0; i < 2; i++)
    {
        arg[i].str = &strArr[i];
        arg[i].value = i+1;
        if (pthread_create(&tid[i], NULL, threadFunction, &arg[i]))
        {
            fprintf(stderr, "Error creating thread\n");
            return 1;
        }
    }
    // Wait for the other threads to finish.
    for (int i = 0; i < 2; i++)
        pthread_join(tid[i], NULL);

    for (int i = 0; i < 2; i++)
    {
        std::cout << *strArr[i] << std::endl;
    }
    return 0;
}
