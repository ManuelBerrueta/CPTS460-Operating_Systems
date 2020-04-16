#include "ucode.c"
#define printk printf

int console;

int main(int argc, char *argv[ ])
{
  int in, out;
  int pid, status;
  
  in  = open("/dev/tty0", O_RDONLY);
  out = open("/dev/tty0", O_WRONLY);

  pid = getpid();

  printf("KCW INIT %d: fork a login proc on console\n", pid);

  console = fork();
  if (console){ // parent
    while(1){
      pid = wait(&status);
      if (pid == console){ // login child died
	printf("KCW INIT fork another login on console\n");
	console = fork();
	if (console){
	  continue;
	}
	else{   // child
	  exec("login /dev/tty0");
	}

      }
      printf("KCW INIT burried an orphan proc %d\n", pid);	
    }
  }
}



