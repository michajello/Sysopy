#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>

int i=1;

void reverse(int nr) {
    i=i*(-1);
}

void au(int sig_no) {
    fprintf(stdout,"Odebrano signal %d.\n", sig_no);
    exit(0);
}

int main(int argc, char const *argv[]) {
    int x = -1;
    int range = 'Z'- 'A'+1;
    signal(SIGTSTP, reverse);

    struct sigaction act;
    act.sa_handler = au;
    sigemptyset(&act.sa_mask);
    act.sa_flags = 0;
    sigaction(SIGINT, &act, NULL);

    while (1) {
        x+=i;
        fprintf(stdout, "\r%c ",'A'+x%range);
        fflush(stdout);
        sleep(1);
        if (x==0) x=26;
        
    }

    return 0;
}
