#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

#define FORK_DEPTH 3

int main()
{
  int i, r;
  pid_t my_pid;

  my_pid = getpid();
  
  for (i = 1; i <= FORK_DEPTH; i++) {
    
    r = fork();
    
    if (r > 0) {
      /* we're in the parent process after
         successfully forking a child */
      
      printf("Parent process %d forked child process %d\n",my_pid, r);  
      
    } else if (r == 0) {
      
      /* We're in the child process, so update my_pid */
      my_pid = getpid();
      
      /* run /bin/echo if we are at maximum depth, otherwise continue loop */
      if (i == FORK_DEPTH) { 
        r = execl("/bin/echo","/bin/echo","Hello World",NULL);
        
        /* we never expect to get here, just bail out */
        exit(1);
      }
    } else { /* r < 0 */
      /* Eek, not expecting to fail, just bail ungracefully */
      exit(1);
    }
  }
}

