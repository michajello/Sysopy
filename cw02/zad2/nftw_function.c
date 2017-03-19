#define _XOPEN_SOURCE 500

#include <string.h>
#include <time.h>
#include <ftw.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>

bool is_integer(const char * arg) {
  int size = strlen(arg);
  for (int i = 0; i < size; i++) {
    if (arg[i]<48 || arg[i]>57) {
      return false;
    }
  }
  return true;
}

int max_size;

static int display_info(const char *fpath, const struct stat *file_stat,
             int tflag, struct FTW *ftwbuf){
    if (file_stat->st_size<max_size && tflag == FTW_F  ) {

        printf("Information for :\n%s\n",  fpath);
        printf("File Size: \t\t%ld bytes\n",file_stat->st_size);
        printf("File Permissions: \t");
        printf( (S_ISDIR(file_stat->st_mode)) ? "d" : "-");
        printf( (file_stat->st_mode & S_IRUSR) ? "r" : "-");
        printf( (file_stat->st_mode & S_IWUSR) ? "w" : "-");
        printf( (file_stat->st_mode & S_IXUSR) ? "x" : "-");
        printf( (file_stat->st_mode & S_IRGRP) ? "r" : "-");
        printf( (file_stat->st_mode & S_IWGRP) ? "w" : "-");
        printf( (file_stat->st_mode & S_IXGRP) ? "x" : "-");
        printf( (file_stat->st_mode & S_IROTH) ? "r" : "-");
        printf( (file_stat->st_mode & S_IWOTH) ? "w" : "-");
        printf( (file_stat->st_mode & S_IXOTH) ? "x" : "-");
        printf("\n" );
        printf("Last time modification: %s",  asctime(gmtime(&(file_stat->st_mtime))));
        printf("---------------------------\n");

    }
    return 0;
}
void print_introduction() {
  printf("[path_to_file][max_file_size]\n" );
}

int main(int argc, char const *argv[])
{
    int flags = 0;
    if (argc!=3) {
      printf("Incorrect number of arguments\n" );
      print_introduction();
      exit(1);
    }
    if (!(is_integer(argv[2]))) {
      printf("Second argument must be integer\n" );
      print_introduction();
      exit(1);
    }
    max_size = atoi(argv[2]);
    flags = FTW_DEPTH ;//| FTW_PHYS;

   if (nftw( argv[1], display_info, 20, flags)    == -1) {
        perror("nftw");
        exit(EXIT_FAILURE);
    }
    exit(EXIT_SUCCESS);
}
