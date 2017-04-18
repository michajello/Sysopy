#define _GNU_SOURCE
#define _XOPEN_SOURCE 700

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <signal.h>
#include "data.h"



pair tab_processes[MAX_CLIENT_NUMBER];
int clients_number=0;
void (*function_pointers[COMMUNICATION_TYPES + 1])(int * qid);
int removed_queue=0;

int server_qid;
my_msgbuf msg;
// msqid_ds queue_information;

void create_server_queue() {
    if ((server_qid = msgget(KEY_SERVER, IPC_CREAT|IPC_EXCL|0600)) == -1) {
        if (errno == EEXIST) {
            server_qid = msgget(KEY_SERVER,0600);
        }else {
        perror("Server queue");
        exit(EXIT_FAILURE);
        }
    }
}
void remove_server_queue() {
    if (msgctl(server_qid, IPC_RMID, NULL) == -1) {
        perror("Removing queue");
    }
    printf("Removed server queue   \n" );
}

void remove_server_queue_by_signal(int nr) {
    remove_server_queue();
    exit(0);
}

void send_prepared_message(int * qid) {
    if (msgsnd(*qid, &msg, sizeof(msg), 0) == -1) {
        perror("Sending message");
        // printf("%lu %lu\n",sizeof(msg),sizeof(&msg) );
        fprintf(stderr, "Problem with sending message \"%s\"\n",msg.message );
    }
}

void send_echo(int * qid) {
    send_prepared_message(qid);
}

void send_to_upper_case(int * qid) {
    for (size_t i = 0; i < strlen(msg.message); i++) {
        if (msg.message[i] >= 'a' && msg.message[i] <= 'z') {
            msg.message[i]-=32;
        }
    }
    send_prepared_message(qid);
}
void send_time(int * qid) {
    static time_t curtime;
    time(&curtime);
    sprintf(msg.message, "%s", ctime(&curtime));
    msg.message[strlen(msg.message) -1] = '\0'; //Removes \n
    send_prepared_message(qid);
}

void send_client_id(int * qid) {
    sprintf(msg.message, "%d", clients_number);
    send_prepared_message(qid);
}

void add_client_qid(int * qid /*this paramether is required only to have consistent function_pointers and print communicate*/ ) {
    if (clients_number == MAX_CLIENT_NUMBER -1) {
        printf("Cannot add more clients\n");
        return;
    }
    *qid=tab_processes[clients_number].qid = atoi(msg.message);;  //save client qid
    tab_processes[clients_number].pid=msg.pid;
    send_client_id(&tab_processes[clients_number].qid);
    clients_number++;
}

int search_client_info(int * qid) {
    int index;
    for (index = 0; index < clients_number && tab_processes[index].qid != (*qid) ; index++) ;
    return index;
}


void remove_client_qid(int * qid) {
    int index = search_client_info(qid);
    if (index == clients_number) {
        fprintf(stderr, "There is no client with such queue%d\n", *qid );
        return;
    }
    clients_number--;
    tab_processes[index]=tab_processes[clients_number];  //overrides data to remove

}

int get_client_qid(int pid){
    int index;
    for (index = 0; index < clients_number && tab_processes[index].pid != pid ; index++) ;
    return tab_processes[index].qid;
}

void receive_first_request() {
    if (msgrcv(server_qid, &msg, sizeof(msg), 0, 0) == -1) {
        perror("Cannot receive first message ");
    }
    int qid = atoi(msg.message);
    add_client_qid(&qid);
}

void print_message(int client_qid) {
    printf("Received message: type=" );
    printf((msg.mtype == ECHO) ? "ECHO" : "");
    printf((msg.mtype == TO_UPPER_CASE) ? "TO_UPPER_CASE" : "");
    printf((msg.mtype == TIME) ? "TIME" : "");
    printf((msg.mtype == EXIT) ? "EXIT" : "");
    printf((msg.mtype == CREATED_QUEUE) ? "CREATED_QUEUE" : "");
    printf("\nfrom PID %d, ", msg.pid );
    printf((client_qid == 0) ? "by server queue"  :"by qid %d",client_qid );
    printf("\nMessage: \"%s\"\n\n",msg.message );
}

void request_control() {
    int length;
    int client_qid;
    while (1) {
        if ( ((length=msgrcv(server_qid, &msg, sizeof(msg), 0, 0)) != -1)) {
            client_qid=get_client_qid(msg.pid);
            print_message(client_qid);
            (*function_pointers[msg.mtype])(&client_qid);
        }else if (length == -1) {
            perror("Server");
            exit(1);
        } else {
            sleep(1);
        }
    }
}

void override_signallNT() {
    signal(SIGINT,remove_server_queue_by_signal);
}

void run_server() {
    override_signallNT();
    request_control();
}



void set_function_pointers( ) {
    function_pointers[0]=NULL;
    function_pointers[ECHO]=send_echo;
    function_pointers[TO_UPPER_CASE]=send_to_upper_case;
    function_pointers[TIME]=send_time;
    function_pointers[EXIT]=remove_client_qid;
    function_pointers[CREATED_QUEUE]=add_client_qid;

}



int main(int argc, char const *argv[]) {
    set_function_pointers();
    create_server_queue();
    set_function_pointers();
    run_server();
    remove_server_queue();
    return 0;
}
