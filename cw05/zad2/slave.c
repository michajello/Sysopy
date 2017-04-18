#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <unistd.h>
#include <fcntl.h>
#include <complex.h>
#include <time.h>
#include <math.h>


struct corner {
    double x;
    double y;
};
struct trio {
    double x;
    double y;
    int result;
} ;
typedef struct trio trio;
typedef struct corner corner;
corner left_corner;
corner right_corner;

int fifo, n, k;
char const * pathname;

bool is_integer(const char * arg) {
  int size = strlen(arg);
  for (int i = 0; i < size; i++) {
    if (arg[i]<48 || arg[i]>57) {
      return false;
    }
  }
  return true;
}

void parse(int argc,char const *argv[]) {
    if (argc != 4) {
        fprintf(stderr, "Incorrect amount arguments\n. Usage: [path_to_FIFO] [N] [K]" );
        exit(EXIT_FAILURE);
    }
    if (!is_integer(argv[2]) || !is_integer(argv[3])) {
        fprintf(stderr, "Second and third argument must be integer\n" );
        exit(EXIT_FAILURE);
    }
}


int get_seed() {
    int  random = open("/dev/urandom", O_RDONLY);
    if (random < 0) {
      printf("No such file /dev/urandom\n" );
      return(1);
    }
    int seed;
    read(random, &seed, sizeof(int)) ;
    close(random);
    return seed;
}

void open_fifo() {
    if ( (fifo=open(pathname, O_WRONLY )) == -1) {
        fprintf(stderr, "%s: ",pathname );
        perror("opening FIFO" );
        printf("I try to create FIFO ...\n" );
        if ((fifo = mkfifo(pathname, O_WRONLY |O_CREAT|  0666) ) == -1) {
            fprintf(stderr, "%s: ",pathname );
            perror("creating FIFO");
            exit(EXIT_FAILURE);
        }
        printf("FIFO: %s created\n",pathname );
        fifo=open(pathname, O_WRONLY );
    }

    printf("FIFO: %s opened\n",pathname );
}

void close_fifo() {
    printf("Closed fifo\n" );
    close(fifo);
}

int  iters(double point_x, double point_y){
    //x,y represent complex number
    double x,y,tmp;
    x=y=0;
    int i;
    for ( i = 0; i < k && (x*x + y*y)<4.0; i++) {
        tmp=x*x-y*y+point_x;
        y=2*x*y+point_y;
        x=tmp;
    }
    return i;
}

void generate_set() {
    trio pixel;

    //neceassary to receive different seeds when we run many processes at thee same time
    int seed =  get_seed();
    srand(time(NULL) * seed);
    // srand(time(0));

    open_fifo();
    printf("writing to FIFO\n" );
    for (int i = 0; i < n; i++) {
        pixel.x= (right_corner.x - left_corner.x ) * ((double)rand() / (double)RAND_MAX) + left_corner.x;
        pixel.y= (right_corner.y - left_corner.y ) * ((double)rand() / (double)RAND_MAX) + left_corner.y;
        pixel.result=iters(pixel.x,pixel.y);


        write(fifo, &pixel, sizeof(trio));
    }
    close_fifo();

}

int main(int argc, char const *argv[]) {
    parse(argc, argv);

    left_corner.x=-2;
    left_corner.y=-1;
    right_corner.x=1;
    right_corner.y=1;

    n=atoi(argv[2]);
    k=atoi(argv[3]);

    pathname=argv[1];
    generate_set();


    return 0;
}
