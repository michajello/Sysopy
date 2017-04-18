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
int range_X;
int range_Y;

int fifo;
unsigned int r;
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

void open_fifo() {

    if ( (fifo=open(pathname, O_RDONLY)) == -1) {
        fprintf(stderr, "%s: ",pathname );
        perror("opening FIFO" );
        printf("I try to create FIFO ...\n" );
        if ((fifo = mkfifo(pathname, O_RDONLY|O_CREAT|  0666) ) == -1) {
            fprintf(stderr, "%s: ",pathname );
            perror("creating FIFO");
            exit(EXIT_FAILURE);
        }
        printf("FIFO: %s created\n",pathname );
        fifo=open(pathname, O_RDONLY);
    }
    printf("FIFO: %s opened\n",pathname );
}

void close_fifo() {
    printf("Closed fifo\n" );
    close(fifo);
}


void parse(int argc,char const *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Incorrect amount arguments\n. Usage: [path_to_FIFO] [R]" );
        exit(EXIT_FAILURE);
    }
    if (!is_integer(argv[2]) ) {
        fprintf(stderr, "Second argument must be integer\n" );
        exit(EXIT_FAILURE);
    }
}
int ** alloc_tab(){
    int ** tab = (int **)calloc(r, sizeof(int *));
    for (size_t i = 0; i < r; i++) tab[i]=(int*)calloc(r, sizeof(int));
    return tab;
}

void free_tab(int ** tab) {
    for (size_t i = 0; i < r; i++) {
        free(tab[i]);
    }
    free(tab);
}
void compute_cordinates(int ** tab,trio  pixel) {
    pixel.x-=left_corner.x;
    pixel.y-=left_corner.y;

    int x = (int) (pixel.x/range_X * r);
    int y = (int) (pixel.y/range_Y * r);
    tab[x][y]=pixel.result;
}

int ** read_from_fifo() {
    trio pixel;
    int ** tab  = alloc_tab();

    open_fifo();
    printf("reading from FIFO\n" );
    while (read(fifo, &pixel, sizeof(pixel)))  {
        // printf("%f,%f,%d\n",pixel.x, pixel.y, pixel.result );
        compute_cordinates(tab,pixel);
    }

    close(fifo);
    return tab;

}

void save_to_file(int ** tab) {
    FILE * data = fopen("data", "w+");
    for (size_t i = 0; i < r; i++) {
        for (size_t j = 0; j < r; j++) {
            fprintf(data, "%lu %lu %u\n",i,j,tab[i][j] );
        }
    }
    free_tab(tab);
    fclose(data);
}

void set_display() {
    left_corner.x=-2;
    left_corner.y=-1;
    right_corner.x=1;
    right_corner.y=1;

    range_X= right_corner.x - left_corner.x;
    range_Y = right_corner.y - left_corner.y;
}


void print_plot() {
    FILE *gnuplot;
    printf("plotting\n" );

    if ((gnuplot=popen("gnuplot -p", "w")) == NULL) {
        perror("gnuplot");
        exit(EXIT_FAILURE);
    }

    int status=fprintf(gnuplot, "set view map\n set xrange [0:%d]\n set yrange [0:%d]\n plot 'data' with image\n",r,r );
    if (status < 0) {
        perror("stdin gnuplot");
    }
    // fflush(gnuplot);
    // getc(stdin);
    pclose(gnuplot);

}


int main(int argc, char const *argv[]) {
    parse(argc, argv);
    set_display();
    int ** tab;
    r=atoi(argv[2]);
    pathname=argv[1];

    tab=read_from_fifo();
    save_to_file(tab);
    print_plot();
    return 0;
}
