#include <pthread.h>
#include <iostream>
#include <string>
#include <unistd.h>
#include <semaphore.h>
// Instructions: 
// You are given a positive integer x. Create a thread for each digit inside x (for example 4723 would have 4 threads for '4' '7' '2' '3') 
// Assign each child thread a counter starting from 0 and incrementing by 1 for each thread ('4' = thread 0, '7' = thread 1, '2' = thread 2, '3' = thread 3)
// If any of the thread's values are even, increment the number by 1 (so for 4723, turn it to 5733)
// Print out the threads in the order of their index (so print out value of thread 0, then thread 1, then thread 2, then thread 3)

// Input: 4723
// Output: 5733

// Input: 1234
// Output: 1335

// NOTE: The print section can not be inside a critical section

struct infoFromMain
{
    int threadID; //e.g. thread 0, thread 1....
    int digit; //e.g. 1234, first digit - 1, second digit - 2...
};


static pthread_mutex_t bsem;    // Mutex semaphore
static pthread_cond_t waitTurn = PTHREAD_COND_INITIALIZER;  // Condition variable to control the turn
static int turn = 0; // value to control which child thread enters the critical section
int output;

void *printDigit(void *void_ptr)
{
    // Write the implementation of the printDigit function here
    pthread_mutex_lock(&bsem);
        struct infoFromMain *info= (struct infoFromMain*)void_ptr;
        while(info->threadID!=turn){
            pthread_cond_wait(&waitTurn, &bsem);
        }

        if(info->digit%2==0){
            output= info->digit+1;
        }
        else{
            output= info->digit;
        }
        turn++;
    pthread_mutex_unlock(&bsem);

    pthread_mutex_lock(&bsem);
        pthread_cond_broadcast(&waitTurn);
    pthread_mutex_unlock(&bsem);

    std::cout << output ; // this way it is not inside the critical section
    
    return nullptr;
}

int main()
{
    std::string input;
    std::cin >> input;
    
    int nThreads = input.size();// initialize the number of threads variable;
    pthread_t *tid = new pthread_t[nThreads];
    infoFromMain *arg = new infoFromMain[nThreads];
    
    pthread_mutex_init(&bsem,nullptr);   // Initializing bsem
    
    for(int i=0;i<nThreads;i++)
    {
        arg[i].threadID = i;// initialize the threadID based on the question's specifications
        arg[i].digit = input[i]-'0';// assign the digit for each child thread based on the question's specifications
        // Call pthread_create here:
        if (pthread_create(&tid[i], nullptr, printDigit, (void*)&arg[i])!=0){
            std::cerr << "Error creating thread" << std::endl;
        }
        
    }
    for(int i=0;i<nThreads;i++)
        pthread_join(tid[i],nullptr);
   
    if (arg != nullptr)
        delete [] arg; 
    if (tid != nullptr)
        delete [] tid;
    return 0;
}