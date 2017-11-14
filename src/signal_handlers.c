#include "signal_handlers.h"
#include <signal.h>
#include <stdio.h>
#include <bits/sigaction.h>
#include <sys/wait.h>

#include "built_in.h"

void catch_sigint(int signalNo)
{
  signal(signalNo, SIG_IGN);
  signal(SIGINT, catch_sigint);
}

void catch_sigtstp(int signalNo)
{
  signal(signalNo, SIG_IGN);
  signal(SIGTSTP, catch_sigtstp);
}

void catch_sigchld(int signalNo){

  int status;
  int pid;
  while((pid = wait(&status)) != -1){
    if(WIFEXITED(status) && WEXITSTATUS(status) != (unsigned char)-1){
      if(pid == bgid){
		printf("%d done %s\n", bgid, bgpath);
        fflush(stdout);
	  }
	}
  }

}
