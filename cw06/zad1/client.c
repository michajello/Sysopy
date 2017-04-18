#include <stdio.h>
#include <stdlib.h>
#include <sys/msg.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include <stdbool.h>
#include <errno.h>
#include "data.h"

int server_qid=0;
int self_qid;
int id_from_server;
char buffer [MESSAGE_LENGTH];
my_msgbuf msg;

void remove_queue(int qid) {
    if (msgctl(qid, IPC_RMID, NULL) == -1) {
        perror("Removing queue");
    }
    printf("removed_queue   \n" );
}

bool is_server_queue_allowed (){
    if (( server_qid = msgget(KEY_SERVER, IPC_CREAT|IPC_EXCL|0600)) == -1) {
        if (errno == EEXIST) {
            return true;
        }
        else {
        perror("Server queue");
        exit(EXIT_FAILURE);
        }
    }
    return false;
}

void open_server_queue() {
    if (is_server_queue_allowed()) {
        if ((server_qid = msgget(KEY_SERVER,0600)) == -1) {
            perror("Server queue");
            exit(EXIT_FAILURE);
        }
    }
    else {
        fprintf(stderr, "Cannot find server queue with id=%d\n",server_qid );
        remove_queue(server_qid);
        exit(EXIT_FAILURE);
    }
    printf("Found server queue wit id: %d\n", server_qid);
}
void get_queue_key(key_t * key) {
    if ((*key = ftok(".",getpid())) == -1) {
        perror("Key");
        exit(EXIT_FAILURE);
    }
}

void create_own_queue() {
    key_t key ;
    get_queue_key(&key);

    if ((self_qid = msgget(key, IPC_CREAT|IPC_EXCL|0600)) == -1) {
        perror("Queue");
        exit(EXIT_FAILURE);
    }
}

void int_to_char(char * buffer_arg,int  number) {
    sprintf(buffer_arg, "%d",number);
}

void send_prepared_message() {
    if (msgsnd(server_qid, &msg, sizeof(msg), 0) == -1) {
        perror("Sending message");
        // printf("%lu %lu\n",sizeof(msg),sizeof(&msg) );
        fprintf(stderr, "Problem with sending message \"%s\"\n",msg.message );
    }
}

void send_queue_id_to_server() {
    msg.mtype=CREATED_QUEUE;
    msg.pid=getpid();
    int_to_char(msg.message, self_qid);
    send_prepared_message();
}

void send_request_echo() {
    sprintf(msg.message, "Communication sended by PID %d by QID %d",getpid(),self_qid);
    msg.mtype=ECHO;
    send_prepared_message();
}

void send_request_to_upper_case() {
    sprintf(msg.message, "Communication sended by PID %d by QID %d",getpid(),self_qid);
    msg.mtype=TO_UPPER_CASE;
    send_prepared_message();
}

void send_request_time() {
    sprintf(msg.message, "Give time,please, PID %d, QID %d",getpid(),self_qid);
    msg.mtype=TIME;
    send_prepared_message();
}

void send_request_finish() {
    sprintf(msg.message, "Let's finish playing... , PID %d, QID %d",getpid(),self_qid);
    msg.mtype=EXIT;
    send_prepared_message();
}

void print_message() {
    printf("Received message from server: type=" );
    printf((msg.mtype == ECHO) ? "ECHO" : "");
    printf((msg.mtype == TO_UPPER_CASE) ? "TO_UPPER_CASE" : "");
    printf((msg.mtype == TIME) ? "TIME" : "");
    printf((msg.mtype == CREATED_QUEUE) ? "CREATED_QUEUE" : "");
    printf("\nMessage: \"%s\"\n\n",msg.message );
}

void receive_id_from_server() {
    while (msgrcv(self_qid, &msg, sizeof(msg), CREATED_QUEUE, 0) == -1) {
        perror("Cannot receive first message ");
         usleep(100);
    }
    id_from_server=atoi(msg.message);
    print_message();
}

void receive_message() {
    if (msgrcv(self_qid, &msg, sizeof(msg), 0, 0) == -1) {
        perror("Cannot receive message ");
    }
    print_message();
}

void send_messages() {
    msg.pid=getpid();
    send_request_echo();
    receive_message();
    send_request_to_upper_case();
    receive_message();
    send_request_time();
    receive_message();
    send_request_finish();
}

int main(int argc, char const *argv[]) {
    open_server_queue();
    create_own_queue();
    send_queue_id_to_server();
    receive_id_from_server();
    send_messages();
    remove_queue(self_qid);
    return 0;
}
