#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <time.h>


bool is_integer(const char * arg) {
  int size = strlen(arg);
  for (int i = 0; i < size; i++) {
    if (arg[i]<48 || arg[i]>57) {
      return false;
    }
  }
  return true;
}
void print_stat(struct stat * file_stat, char * name_file,char * directory_path) {

   printf("Information for: %s\n",name_file);
   printf("---------------------------\n");
   printf("Directory path: \t%s/\n", directory_path );
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
   printf("\n");
   printf("Last time modification: %s\n",  asctime(gmtime(&(file_stat->st_mtime))) );


}

char * create_path(char * path, char * dir_name) {
  char * dir_path = (char*) calloc(strlen(path)+strlen(dir_name)+1 ,sizeof(char));
  strcpy(dir_path, path);
  strcat(dir_path,"/");
  strcat(dir_path, dir_name);
  return dir_path;
}

void search_dir(char * path, int  * max_size) {
  char * directory_path;
  DIR * dirp = opendir(path);
  struct dirent * dp;

  struct stat * file_stat = (struct  stat *) malloc(sizeof(struct stat));
  if (dirp == NULL) {
    perror("dirp:");
    return;
  }

  while ((dp = readdir(dirp)) != NULL) {
      if (strcmp(dp->d_name,"..")!=0 && strcmp(dp->d_name,".")!=0) {
        directory_path = create_path(path,dp->d_name);
        lstat(directory_path,file_stat);

        if( S_ISDIR(file_stat->st_mode) ){
          search_dir(directory_path,max_size);
        }
        else if (S_ISREG(file_stat->st_mode) && file_stat->st_size < *max_size) {
          print_stat(file_stat,dp->d_name   ,directory_path);

        }
      }
    }

    closedir(dirp);

}

void print_introduction() {
  printf("[path_to_file][max_file_size]\n" );
}

int main(int argc, char const *argv[]) {

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
  int  max_size = atoi(argv[2]);
  search_dir((char*) argv[1],&max_size );

  return 0;
}
