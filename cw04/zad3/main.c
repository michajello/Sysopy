#include <errno.h>
#include <signal.h>
#include <signal.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/resource.h>
#include <sys/time.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>
#include <wait.h>
#include <stdbool.h>
unsigned short int type;
unsigned int L;
pid_t child_pid;
pid_t parent_pid;

struct sigaction usr1;
struct sigaction sigrtmin2;
struct sigaction sigrtmin1;
struct sigaction usr2;
struct sigaction s_int;
int static sent_signal_by_parent = 0;
int static received_signal_by_parent = 0;
int static *received_signal_by_child = 0;

int SIGRTMIN1 ;
int SIGRTMIN2 ;

bool is_integer(const char *arg) {
  int size = strlen(arg);
  for (int i = 0; i < size; i++) {
    if (arg[i] < 48 || arg[i] > 57) {
      return false;
    }
  }
  return true;
}

void sig_handler_int(int signo  ) {

    if (getpid()==parent_pid) {
        kill(child_pid,SIGKILL);
        printf("Program interrupted by SIGINT\n" );
    }
    exit(1);
}

void sig_handler_usr1(int sig_no, siginfo_t *siginfo, void *context) {

  if (siginfo->si_pid == child_pid /*&& getpid() == parent_pid*/) {
    received_signal_by_parent++;
  } else if (siginfo->si_pid == parent_pid) {
    *received_signal_by_child += 1;
    kill(parent_pid, SIGUSR1);
  }
}

void sig_handler_usr2(int signo, siginfo_t *siginfo, void *context) {
  if (siginfo->si_pid == parent_pid) {
    _exit(0);
  }
}
void sig_handler_rtmin1(int signo, siginfo_t *siginfo, void *context) {
  if (siginfo->si_pid == child_pid /*&& getpid() == parent_pid*/) {
    received_signal_by_parent++;
  } else if (siginfo->si_pid == parent_pid) {
    *received_signal_by_child += 1;
    kill(parent_pid, SIGRTMIN1);
  }
}
void sig_handler_rtmin2(int signo, siginfo_t *siginfo, void *context) {
    if (siginfo->si_pid == parent_pid) {
      _exit(0);
    }
}

void waiting() {
  // int status;
  // waitpid(child_pid, &status, WUNTRACED | WCONTINUED);
  // while (!WIFEXITED(status)  || WTERMSIG(status) != SIGUSR2) {
  //   waitpid(child_pid, &status, WUNTRACED | WCONTINUED);
  //   if ( WTERMSIG(status) == SIGUSR1) {
  //       kill(parent_pid, SIGUSR1);
  //
  //   }
  // }
  while (1) {
      /* code */
  }
}

void send_signal_to_child_using_kill() {
    sleep(1);
  for (unsigned int i = 0; i < L; i++) {
    // usleep(1);
    kill(child_pid, SIGUSR1);
    sent_signal_by_parent++;
  }
  kill(child_pid, SIGUSR2);
   sleep(1);
}

void send_signal_to_child_using_sigqueue() {
  union sigval sval;
  sleep(1);
  sval.sival_int = 0;
  for (unsigned int i = 0; i < L; i++) {
    //  usleep(1);
    sigqueue(child_pid, SIGUSR1, sval);
    sent_signal_by_parent++;
  }
  sigqueue(child_pid, SIGUSR2, sval);

   sleep(1);
}

void send_using_real_signal() {
    sleep(1);
  for (unsigned int i = 0; i < L; i++) {
    // usleep(1);
    kill(child_pid, SIGRTMIN1);
    sent_signal_by_parent++;
  }
  kill(child_pid, SIGRTMIN2);
   sleep(1);
}

void send_signal_to_child() {
   sleep(1);
  if (type == 1) {
    send_signal_to_child_using_kill();
  } else if (type == 2) {
    send_signal_to_child_using_sigqueue();
  } else if (type == 3) {
    send_using_real_signal();
  } else {
    printf("Incorrect argument type\n");
    exit(1);
  }
}

void print_stat() {
  // if (getpid() == parent_pid) {
    printf("Sent signals from parent to child: %d\n", sent_signal_by_parent);
    printf("Received signal by parent: %d\n", received_signal_by_parent);
    printf("Received signal by child: %d\n", *received_signal_by_child);

}

void parser(int argc, char const *argv[]) {
  if (argc != 3) {
    fprintf(stderr, "Incorrect number of arguments\n");
    exit(1);
  }
  if (!is_integer(argv[1])) {
    fprintf(stderr, "First argument must be integer\n");
    exit(1);
  }
  if (!is_integer(argv[2])) {
    fprintf(stderr, "Second argument must be integer\n");
    exit(1);
  }
}

int main(int argc, char const *argv[]) {
  SIGRTMIN1 = SIGRTMIN + 1;
  SIGRTMIN2 = SIGRTMIN + 2;
  parent_pid = getpid();

  parser(argc, argv);

  L = atoi(argv[1]);
  type = atoi(argv[2]);

  signal(SIGINT, sig_handler_int);

  if (type == 3) {
    sigrtmin1.sa_sigaction = sig_handler_rtmin1;
    sigemptyset(&sigrtmin1.sa_mask);
    sigrtmin1.sa_flags = SA_SIGINFO | SA_RESTART;
    sigfillset(&sigrtmin1.sa_mask);
    if (sigaction(SIGRTMIN1, &sigrtmin1, NULL) == -1) {
      perror("Sigaction sigrtmin1");
      exit(1);
    }
    sigrtmin2.sa_sigaction = sig_handler_rtmin2;
    sigemptyset(&sigrtmin2.sa_mask);
    sigrtmin2.sa_flags = SA_SIGINFO | SA_RESTART;
    sigfillset(&sigrtmin2.sa_mask);

    if (sigaction(SIGRTMIN2, &sigrtmin2, NULL) == -1) {
      perror("Sigaction sigrtmin2");
      exit(1);
    }
  }

  received_signal_by_child =
      (int *)mmap(NULL, sizeof(*received_signal_by_child),
                  PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);

  usr2.sa_sigaction = sig_handler_usr2;
  sigemptyset(&usr2.sa_mask);
  usr2.sa_flags = SA_SIGINFO | SA_RESTART;

  if (sigaction(SIGUSR1, &usr2, NULL) == -1) {
    perror("Sigaction usr2");
    exit(1);
  }

  usr1.sa_sigaction = sig_handler_usr1;
  sigemptyset(&usr1.sa_mask);
  usr1.sa_flags = SA_SIGINFO | SA_RESTART;
  if (sigaction(SIGUSR1, &usr1, NULL) == -1) {
    perror("Sigaction usr1");
    exit(1);
  }


  pid_t pid = fork();

  if (pid == 0) {
    waiting();
  } else if (pid > 0) {
    // sleep(1);
    child_pid = pid;
    sleep(1);
    send_signal_to_child();
  } else {
    perror("fork");
    exit(1);
  }
  print_stat();

  return 0;
}
