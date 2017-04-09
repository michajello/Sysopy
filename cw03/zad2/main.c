#define _GNU_SOURCE
#define _XOPEN_SOURCE 700

#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/resource.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#define BUFFOR_SIZE 1024
#define MAX_ARGS_NUMBER 16

const int megabyte = 1024000;

void init_measurement(struct timeval *userTmp, struct timeval *sysTmp,
                      struct rusage *usage) {
  getrusage(RUSAGE_CHILDREN, usage);
  (*userTmp) = (*usage).ru_utime;
  (*sysTmp) = (*usage).ru_stime;
}

void finish_measurement(struct rusage *usage) {
  getrusage(RUSAGE_CHILDREN, usage);
}

void print_result(struct timeval *userTmp, struct timeval *systemTmp,
                  struct rusage *usage) {
  printf("CPU time: sec user: %ld.%06ld ,sec system  %ld.%06ld\n",
         (*usage).ru_utime.tv_sec - (*userTmp).tv_sec,
         (*usage).ru_utime.tv_usec - (*userTmp).tv_usec,
         (*usage).ru_stime.tv_sec - (*systemTmp).tv_sec,
         (*usage).ru_stime.tv_usec - (*systemTmp).tv_usec);
  printf("\n");
}

bool is_integer(const char *arg) {
  int size = strlen(arg);
  for (int i = 0; i < size; i++) {
    if (arg[i] < 48 || arg[i] > 57) {
      return false;
    }
  }
  return true;
}
void set_limit_procces(struct rlimit *limit, long process_limit) {
  limit->rlim_cur = process_limit;
  limit->rlim_max = process_limit;
}

void interprete(char *path, int cpu_time_limit, long memory_limit) {

  FILE *file;
  char *buffor = NULL;
  char *variable = NULL;
  char *value = NULL;

  char *ptr;
  char *command;
  char *arguments[MAX_ARGS_NUMBER];
  char *env;

  struct rusage usage;
  struct timeval userTmp, systemTmp;

  int line_number = 0;
  int args;
  size_t length;
  file = fopen(path, "r");

  if (file == NULL) {
    fprintf(stderr, "No such file %s\n", path);
    exit(1);
  }

  while (getline(&buffor, &length, file) != -1) {
    line_number++;

    if (buffor[0] == '#') {
      if (length == 1) {
        fprintf(stderr, "Line: %d, no name variable.\n", line_number);
        exit(1);
      }
      ptr = strtok(buffor, " #\t\n");
      variable = ptr;

      ptr = strtok(NULL, " #\t\n");
      value = ptr;

      ptr = strtok(NULL, " #\t\n");
      if (ptr != NULL) {
        fprintf(stderr, "Line: %d, too much values for one variable\n",
                line_number);
        exit(1);
      }

      if (value == NULL) {
        unsetenv(variable);
      } else {
        setenv(variable, value, 1); // overvrites if variable exists
        printf("Enviroment variable: |%s| was set with value: |%s|\n", variable,
               value);
      }

    } else {

      ptr = strtok(buffor, " \t\n");
      args = 0;

      if (ptr == NULL)
        continue;
      if (ptr[0] == '$' && (env = getenv(ptr + 1)) != NULL) {
        command = env;
      } else {
        command = ptr;
      }
      arguments[args] = command;
      args++;

      ptr = strtok(NULL, " \t\n");

      while (ptr != NULL) {
        if (ptr[0] == '$') {
          if ((env = getenv(ptr + 1)) != NULL) {
            arguments[args] = env;
            args++;
          } else {
            fprintf(stderr, "Line: %d no such enviroment variable %s\n",
                    line_number, env);
             exit(1);
          }
        } else {
          arguments[args] = ptr;
          args++;
        }
        ptr = strtok(NULL, " \t\n");
      }

      arguments[args] = NULL;
      int pid;
      init_measurement(&userTmp, &systemTmp, &usage);


      if ((pid = fork()) < 0) {
        fprintf(stderr, "Cannot create fork\n");
      } else if (pid == 0) {
        struct rlimit limit;

        set_limit_procces(&limit, memory_limit);
        if (setrlimit(RLIMIT_AS, &limit) == -1) {
          fprintf(stderr, "Cannot set memory limit\n");
          _exit(EXIT_FAILURE);
        }

        set_limit_procces(&limit, cpu_time_limit);
        if (setrlimit(RLIMIT_CPU, &limit) == -1) {
          fprintf(stderr, "Cannot set CPU limit\n");
          _exit(EXIT_FAILURE);
        }

        if (execvp(command, arguments) < 0) {
          fprintf(stderr, "Line: %d, cannot execute command\n", line_number);
          _exit(EXIT_FAILURE);
        }
        _exit(EXIT_SUCCESS);
      } else {
        int w, status;
        do {
          w = waitpid(pid, &status, WUNTRACED | WCONTINUED);
          if (w == -1) {
            perror("waitpid");
            exit(EXIT_FAILURE);
          }
          if (WIFEXITED(status)) {
            if (WEXITSTATUS(status) != 0) {
              fprintf(stderr, "Line: %d, ", line_number);
              fprintf(stderr, "command was exited with status: %d\n",
                      WEXITSTATUS(status));
              exit(EXIT_FAILURE);
            }
          } else if (WIFSIGNALED(status)) {
            printf("Line: %d, Child process was killed by signal %d\n",
                   line_number, WTERMSIG(status));
          } else if (WIFSTOPPED(status)) {
            printf("Line: %d, Child process was stopped by signal %d\n",
                   line_number, WSTOPSIG(status));
          } else if (WIFCONTINUED(status)) {

            printf("Line: %d, Child process is being continued\n", line_number);
          }
        } while (!WIFEXITED(status) && !WIFSIGNALED(status) &&
                 !WIFSTOPPED(status));
      }
      finish_measurement(&usage);
      for (int i = 0; i < args; i++)
        printf("%s ", arguments[i]);
      printf("\t");
      print_result(&userTmp, &systemTmp, &usage);
    }
  }
  free(buffor);
  fclose(file);
}

int main(int argc, char *argv[]) {

  if (argc != 4) {
    fprintf(stderr, "Program requires three arguments\n");
    exit(1);
  }
  if (!is_integer(argv[2]) || !is_integer(argv[3])) {
    fprintf(stderr, "Second and third arguent must be integer\n");
    exit(1);
  }

  interprete(argv[1], atoi(argv[2]), atoi(argv[3]) * megabyte);
  return 0;
}
