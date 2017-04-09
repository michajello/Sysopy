#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <time.h>
#include <signal.h>
#include <stdbool.h>


int *children_pid;
int *children_status;

int m;
int n;
int received_request=0;
pid_t parent_pid;
enum proc_status {FROZEN, WAITING ,FINISHED} ;

bool is_integer(const char *arg) {
  int size = strlen(arg);
  for (int i = 0; i < size; i++) {
    if (arg[i] < 48 || arg[i] > 57) {
      return false;
    }
  }
  return true;
}


void sig_handler_int(int sig_num) {
    if (getpid() == parent_pid) {
        printf("Received SIGINT\n" );
        for (int i = 0; i < n; i++) {
                if (children_status[i] != FINISHED) {
                    printf("Killed child: %d, PID: %d\n",i ,children_pid[i] );
                    kill(children_pid[i],SIGKILL);
                }
        }
    }
    free(children_pid);
    exit(0);
}

void waiting() {
  int status;
  waitpid(getpid(), &status, WUNTRACED | WCONTINUED);
  while ( WTERMSIG(status) != SIGUSR2) {
    waitpid(getpid(), &status, WUNTRACED | WCONTINUED);
    usleep( ((rand() % 9500000) + 500 * 1000));
    kill(parent_pid,SIGUSR1);
    pause();
  }
}

void manage_signal_from_child(int sig_num, siginfo_t *sig_info, void *sig_context) {
    printf("PID:%d  requested\n", sig_info->si_pid);
    // children_pid[received_request++] = sig_info->si_pid;

    int i=0;
    for (i = 0; i < n && children_pid[i]!=sig_info->si_pid; i++) ;
    if (children_status[i]==FROZEN) {
        children_status[i]=WAITING;
        received_request++;
    }

    if (received_request == m) {
        printf("Received m requests\n" );
        for (int i = 0; i < m; ++i) {
               if (children_status[i] == WAITING) {
                   printf("sended to child\n" );
                   kill(children_pid[i], SIGUSR2);
                   children_status[i]=FINISHED;
               }
           }
    }
    else if (received_request > m) {
       kill(sig_info->si_pid, SIGUSR2);
       usleep(500);
       kill(sig_info->si_pid, SIGUSR2);

   }
   if (received_request==n) {
       for (size_t i = 0; i < n; i++) {
           kill(children_pid[i],SIGUSR2);
       }
   }

}



void manage_real_signal(int sig_num, siginfo_t *sig_info, void *sig_context) {
    printf("Signal %d from %d\n", sig_num, sig_info->si_pid);
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

int main(int argc, char const **argv) {

    parser(argc,argv);
    n = atoi(argv[1]);
    m = atoi(argv[2]);
    children_pid = (int *)calloc(n, sizeof(int));
    children_status = (int *)calloc(n, sizeof(int));
    printf("Parent PID: %d\n", getpid());


    struct sigaction action;

    action.sa_handler = &sig_handler_int;
    sigfillset(&action.sa_mask);
    sigaction(SIGINT, &action, NULL);
    action.sa_handler = NULL;

    action.sa_sigaction = &manage_signal_from_child;
    action.sa_flags = SA_SIGINFO | SA_RESTART;

    sigaction(SIGUSR1, &action, NULL);

    action.sa_sigaction = &manage_real_signal;

    for (int i = SIGRTMIN; i <= SIGRTMAX; ++i) {
        sigaction(i, &action, NULL);
    }

    parent_pid = getpid();
    for (int i = 0; i < n; ++i) {
        pid_t pid = fork();

        if(pid ==0) {
            srand(time(NULL)  * (getpid() ) & getpid());
            printf("Child %d, PID: %d\n", i , getpid());
            sleep(rand() % 9 + 2);
            time_t start = time(NULL);
            usleep(10);
            kill(parent_pid, SIGUSR1);
            children_status[i]=WAITING;
            pause();
            // waiting();
            kill(parent_pid, rand() % (SIGRTMAX - SIGRTMIN + 1) + SIGRTMIN);
            time_t activ_time = time(NULL) - start;
            printf("PID: %d, time: %f\n", getpid(), (double) (activ_time));
            free(children_pid);
            exit((int) activ_time);
        }
        else if (pid >0) {
            //  printf("%d\n",pid );
             children_pid[i]=pid;
             children_status[i]=FROZEN;
            usleep(10);
            }
        }

    while (wait(NULL) > 0) {}

    free(children_pid);
    printf("Finished\n" );
    sleep(2);
    return 0;
}
