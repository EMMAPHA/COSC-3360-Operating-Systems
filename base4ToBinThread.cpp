#include <pthread.h>
#include <iostream>
#include <unistd.h>
#include <string>

struct arguments
{
    char base4Dig;
    std::string *base4ToBin;
};

std::string base4digToBin(char dig)
{
    switch(dig)
    {
        case '0': return "00";
        case '1': return "01";
        case '2': return "10";
        case '3': return "11";
    }

    return "";
}

void *base4ToBin(void *void_ptr)
{
    struct arguments *arg_ptr = (arguments*) void_ptr; //cast void pointer to struct arguments pointer

    //Given the base 4 digit from main thread, store string with binary code (returned by base4ToBin)
    //to memory location provided in arguments
    *arg_ptr -> base4ToBin = base4digToBin(arg_ptr -> base4Dig);


    return nullptr;
}

int main()
{
    std::string base4Number = "103";
    //std::cin >> base4Number;
    int size = base4Number.size();
    std::string *binNumber = new std::string[size];
    pthread_t *tid = new pthread_t[size];
    struct arguments *arg = new arguments[size];

    for(int i = 0; i < size; i++)
    {
        arg[i].base4Dig = base4Number[i]; //assign base 4 digit (char) from string with base 4 value provided as input
        arg[i].base4ToBin = &binNumber[i]; //assign address from the string array where you want to store the binary code for the assigned base 4 digit

        if(pthread_create(&tid[i], NULL, base4ToBin, &arg[i]))/* Call pthread_create here */
        {
            fprintf(stderr, "Error creating thread\n");
            return 1;
        }
    }

    //call pthread_join here
    for(int i = 0; i < size; i++)
    {
        pthread_join(tid[i], NULL);
    }


    std::cout << "The base 4 number " << base4Number << " is equal to: ";
    for(int i = 0; i < size; i++)
    {
        std::cout << binNumber[i];
    }

    std::cout << std::endl;
    delete [] tid;
    delete [] arg;
    delete [] binNumber;


    return 0;
}