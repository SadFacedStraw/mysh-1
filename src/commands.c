#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <sys/stat.h>

#include "commands.h"
#include "built_in.h"

void exe(char* command[], int background);
int path_resolution(char* name, char* re_path, size_t size);

static struct built_in_command built_in_commands[] = {
  { "cd", do_cd, validate_cd_argv },
  { "pwd", do_pwd, validate_pwd_argv },
  { "fg", do_fg, validate_fg_argv }
};

static int is_built_in_command(const char* command_name)
{
  static const int n_built_in_commands = sizeof(built_in_commands) / sizeof(built_in_commands[0]);

  for (int i = 0; i < n_built_in_commands; ++i) {
    if (strcmp(command_name, built_in_commands[i].command_name) == 0) {
      return i;
    }
  }

  return -1; // Not found
}

/*
 * Description: Currently this function only handles single built_in commands. You should modify this structure to launch process and offer pipeline functionality.
 */
int evaluate_command(int n_commands, struct single_command (*commands)[512])
{
  if (n_commands > 0) {
    struct single_command* com = (*commands);

    assert(com->argc != 0);

    int built_in_pos = is_built_in_command(com->argv[0]);
    if (built_in_pos != -1) {
      if (built_in_commands[built_in_pos].command_validate(com->argc, com->argv)) {
        if (built_in_commands[built_in_pos].command_do(com->argc, com->argv) != 0) {
          fprintf(stderr, "%s: Error occurs\n", com->argv[0]);
        }
      } else {
	  	
        fprintf(stderr, "%s: Invalid arguments\n", com->argv[0]);
        return -1;
      }
    } else if (strcmp(com->argv[0], "") == 0) {
      return 0;
    } else if (strcmp(com->argv[0], "exit") == 0) {
      return 1;
    } else {
	  int background = 0;
	  for(int i = 0; i < com->argc; i++){
        if(strcmp(com->argv[i], "&") == 0){
		  background = 1;
		  com->argv[i] = NULL;
		  strcpy(bgpath, com->argv[0]);
		  break;
		  }
	  }
	  exe(com->argv, background);
      return 0;
    }
  }

  return 0;
}

void free_commands(int n_commands, struct single_command (*commands)[512])
{
  for (int i = 0; i < n_commands; ++i) {
    struct single_command *com = (*commands) + i;
    int argc = com->argc;
    char** argv = com->argv;

    for (int j = 0; j < argc; ++j) {
      free(argv[j]);
    }

    free(argv);
  }

  memset((*commands), 0, sizeof(struct single_command) * n_commands);
}

void exe(char *command[], int background){
   int pid = fork();
   int status;

   if(pid == -1)
     printf("fork error is occured\n");
   else if(pid == 0){
     char re_path[256];
	 char *path = *command;

	 if(path_resolution(path, re_path, sizeof(re_path))){
	   path = re_path;
	   strcpy(*command, path);
	 }

     if(execv(*command, command) <0)
	 {
	 fprintf(stderr, "%s: command not found\n", *command);
	 exit(-1);
	 }
	 exit(0);
	}
   else{
     if(background == 0)
	   wait(&status);
	 else{
       bgid = pid;
	   printf("%d\n", bgid);
	   }
    }
}

int path_resolution(char* name, char* re_path, size_t size){
  
  char* env_path;
  char* dir;
  char* path;
  char* sys_env_path = getenv("PATH");
  struct stat sb;
  int re_check = 0;
  int sys_env_path_len = strlen(sys_env_path);

  if(name[0] == NULL)
    return 0;

  env_path = malloc(sys_env_path_len + 1);
  if(env_path == -1)
    return 0;

  strncpy(env_path, sys_env_path, sys_env_path_len);

  dir = strtok(env_path, ":");
  while(dir != NULL){
    int path_len = strlen(dir) + 1 + strlen(name) + 1;
	path = malloc(path_len);
	if(path == -1)
	  return 0;
	
	path[0] = '\0';
	strncat(path, dir, path_len);
	strncat(path, "/", path_len);
	strncat(path, name, path_len);

	memset(&sb, 0, sizeof(stat));
	if(stat(path, &sb) != -1 && S_ISREG(sb.st_mode) && !re_check){
      strncpy(re_path, path, size);
	  re_path[strlen(re_path)] = '\0';
	  re_check = 1;
  }

  free(path);
  dir = strtok(NULL, ":");
  }

  free(env_path);
  return (re_check == 1);
}
