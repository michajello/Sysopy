#define _GNU_SOURCE
#define _XOPEN_SOURCE 700
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <errno.h>

#define PROGRAM_NUMBER 20
#define ARGUMENT_NUMBER 30

pid_t parent_pid;

struct program {
    char * command;
    char * arguments[ARGUMENT_NUMBER];
    int argument_number;
};

typedef struct program program;




void parse_program(char * buffor,program * prog) {
    prog->argument_number=0;
    char * ptr;
    ptr = strtok(buffor, " \t\n");
    prog->command=ptr;
    prog->arguments[prog->argument_number++]=ptr;

    ptr = strtok(NULL, " \t\n");
    while (ptr != NULL) {
      prog->arguments[prog->argument_number]=ptr;
      prog->argument_number++;
      ptr = strtok(NULL, " \t\n");
    }
    prog->arguments[prog->argument_number]=NULL;
}

void print_command(program * programs,int programs_number) {
    if (programs_number == 0) {
        return;
    }
    printf("Executing: ");
    for (int i = 0; i < programs_number; i++) {
        for (int j = 0; j < programs[i].argument_number; j++) {
            printf("%s ",programs[i].arguments[j] );
        }
        if (i<programs_number -1) {
            printf("| " );
        }
    }
    printf("\n" );
}

void execute_line(program * programs,int programs_number) {
    int fd[2];
    pid_t pid;

    int executed_command=0;
    int fd_in=0;
    int status;
    print_command(programs, programs_number);

    while (executed_command < programs_number) {
        pipe(fd);
        if ((pid= fork()) ==-1) {
            perror("fork");
            exit(EXIT_FAILURE);
        } else if (pid > 0) { //parent
            close(fd[1]);
            wait(&status);
            if (status != 0 ) {
                printf("Interpreter: incorrect command\n" );
                exit(EXIT_FAILURE);
            }
            fd_in=fd[0];
            executed_command++;
        } else {    //child
            dup2(fd_in, STDIN_FILENO);
            if (executed_command != programs_number -1) {
                dup2(fd[1], STDOUT_FILENO);
            }
            close(fd[0]);
            if((execvp(programs[executed_command].command, programs[executed_command].arguments)) == -1 ){
                perror(programs[executed_command].command);
                _Exit(EXIT_FAILURE);
            }
            _exit(EXIT_SUCCESS);
        }
    }
}


void interprete(char *path) {
  FILE *file;
  char *buffor = NULL;
  char *ptr;
  char * prog_string;

  int line_number = 0;
  size_t length;
  program programs[PROGRAM_NUMBER];
  file = fopen(path, "r");

  if (file == NULL) {
    fprintf(stderr, "No such file %s\n", path);
    exit(1);
  }


  while (getline(&buffor, &length, file) != -1) {
    line_number++;
    int program_number=0;
    printf("%s\n", buffor);

    ptr = strtok(buffor, "|\n");
    while (ptr != NULL) {
        prog_string = (char *)malloc((strlen(ptr)+1) * sizeof(char));
        strcpy(prog_string, ptr);
        parse_program(prog_string, &programs[program_number]);
        ptr=strtok(ptr+strlen(ptr)+1 , "|\n");
        program_number++;
    }
    execute_line(programs,program_number);
  }

  free(buffor);
  fclose(file);
}

int main(int argc, char *argv[]) {

  if (argc != 2) {
    fprintf(stderr, "Program requires only one argument\n");
    exit(1);
  }
  interprete(argv[1]);
  return 0;
}
