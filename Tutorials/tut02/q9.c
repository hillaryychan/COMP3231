#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>

main()
{
  int r;
  r = chdir("..");
  if (r < 0) {
    perror("Eek!");
    exit(1);
  }
  
  r = execl("/bin/ls","/bin/ls",NULL);
  perror("Double eek!");
}

