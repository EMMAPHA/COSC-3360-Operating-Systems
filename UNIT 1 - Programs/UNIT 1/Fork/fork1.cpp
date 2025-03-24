#include <iostream>
#include <unistd.h>
#include <sys/wait.h>

int main()
{
  int pid;
  std::cout << "I am the parent process " << std::endl;
  for (int i = 0; i < 3; i++)
  {
    pid = fork();
    if (pid == 0)
    {
      std::cout << "I am child process " << i << std::endl;
      if (i == 1)
      {

        pid = fork();
        if (pid == 0)
        {
          std::cout << "I am a grandchild process from child process " << i << std::endl;
          _exit(0);
        }
        wait(nullptr);
      }
      _exit(0);
    }
    wait(nullptr);
  }
  //  for (int i = 0; i < 3; i++)
  //      wait(nullptr);
  return 0;
}