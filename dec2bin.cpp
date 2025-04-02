#include <pthread.h>
#include <iostream>
#include <string>
#include <vector>

struct arguments
{
    int value;
    std::string binCode;
};

void *dec2bin(void *void_ptr)
{
    arguments *ptr = (arguments *)void_ptr;
    ptr->binCode = "";
    int value = ptr->value;
    do
    {
        ptr->binCode = std::to_string(value % 2) + ptr->binCode;
        value = (value - value % 2) / 2;
    } while (value > 0);

    return nullptr;
}

int main()
{
    std::vector<arguments> arg;
    arguments tempHolder;
    tempHolder.binCode = "";
    while (std::cin >> tempHolder.value)
    {
        arg.push_back(tempHolder);
    }
    int nValues = arg.size();
    pthread_t *tid = new pthread_t[nValues];

    for (int i = 0; i < nValues; i++)
    {
        // Call pthread_create here
        if (pthread_create(&tid[i], nullptr, dec2bin, &arg[i]) != 0)
        {
            std::cerr << "Error creating thread" << std::endl;
            return 1;
        }
    }

    // Wait for the other threads to finish.
    // Call pthread_join here

    for (int i = 0; i < nValues; i++)
        pthread_join(tid[i], nullptr);

    for (int i = 0; i < nValues; i++)
    {
        std::cout << arg[i].value << " base 10 is equal to " << arg[i].binCode << std::endl;
    }
    if (tid != nullptr)
        delete[] tid;
    return 0;
}