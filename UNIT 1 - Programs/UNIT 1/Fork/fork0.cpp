#include <iostream>
#include <unistd.h>
#include <sys/wait.h>

int main()
{
  int pid;
  fork();
  fork();
  fork();
  std::cout << "I am a process" << std::endl;
  return 0;
}