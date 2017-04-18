#ifndef data_header
#define  data_header
#define MAX_PROCESSES_NUMBER 64

#define KEY_SERVER 123
#define MESSAGE_LENGTH 1024

#define COMMUNICATION_TYPES  5

// Communication's types
#define ECHO 1
#define TO_UPPER_CASE 2
#define TIME 3
#define EXIT 4
#define CREATED_QUEUE 5

typedef struct my_msgbuf my_msgbuf;
struct my_msgbuf{
    long mtype;
    int pid;
    char message [2048];
};

// typedef struct my_msgbuf_with_int my_msgbuf_with_int;
// struct my_msgbuf_with_int{
//     long mtype;
//     int qid;
//     int pid;
// };

typedef struct pair pair;
struct pair {
    int qid;
    int pid;
};

#endif
