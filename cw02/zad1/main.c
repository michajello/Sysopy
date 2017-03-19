#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <time.h>




void error_arg(int i) {
  printf("Unappropriate argument %d\n",i );
}
void incorrect_amount_arguments() {
  printf("Incorrect amount of arguments\n" );
}
void print_not_int(int i){
  printf("Argument %d is not int\n",i );
}
bool is_integer(const char * arg) {
  int size = strlen(arg);
  for (int i = 0; i < size; i++) {
    if (arg[i]<48 || arg[i]>57) {
      return false;
    }
  }
  return true;
}

void print_introduction() {
  printf("Option arguments:\n" );
  printf("[sys,lib][sort]\n" );
  printf("[sys,lib][shuffle][records_number][record_size]\n" );
  printf("generate [file_name][records_number][record_size]\n" );

}



void generate_file(const char * name_file, int record_number, int record_size) {
  int generated_file = open(name_file, O_WRONLY|O_TRUNC| O_CREAT,0666  );
  unsigned int length = record_size + 1;   // adds '\n'
  unsigned char * buffor = ( unsigned char *) calloc(length +1, sizeof(char));
  int  random = open("/dev/urandom", O_RDONLY);
  if (random < 0) {
    printf("No such file /dev/urandom\n" );
    exit(1);
  }

  for (int i = 0; i < record_number; i++) {
    read(random, buffor, length) ;
    for (unsigned int  i = 0; i <length; i++) {
      buffor[i]=buffor[i] %25 +'A';
    }
    buffor[length-1]='\n';
    write(generated_file, buffor, length);
  }
  free(buffor);
  close(random);
  close(generated_file);
}

long  sys_get_record_size(int file) {
   char * buffor = (char *) calloc(64, sizeof(char));
  if (buffor == NULL) {
    return 0;
  }
  long size_of_record=0;
  char * ptr;
  while( read(file, buffor,64 ) > 0  ){

    for ( ptr=buffor; *ptr !='\0' && (*ptr)!='\n'; ptr++)size_of_record++;
    if ( *ptr !='\0' && *ptr == '\n') break;
  }

  size_of_record++;  //adds char '\n'
  lseek(file, 0, SEEK_SET);   //change location of read/write pointer to the beginning of file
  return size_of_record;

}
void sys_bubble_sort(int file,long  * record_size,long  records_number){
  bool flag = 1;
  char * record1 = (char *) calloc(*record_size, sizeof(char));
  char * record2 = (char *) calloc(*record_size, sizeof(char));

  while(flag){

    flag=false;
    lseek(file, 0  , SEEK_SET);
    read(file, record1, *record_size);

    for (long i= 1; i < records_number; i++) {
      read(file, record2, (*record_size));
      if (record1[0]>record2[0]) {

        lseek(file, -2*(*record_size), SEEK_CUR);
        write(file, record2, *record_size);
        write(file, record1, *record_size);
        flag=true;
      } else {
        char * tmp_swap = record1;
        record1 = record2;
        record2 = tmp_swap;
      }
    }
    records_number--;
  }
  free(record1);
  free(record2);
}

void sys_sort_file(const char * name_file) {

  int file = open(name_file, O_RDWR);
  if (file < 0) {
    printf("There is no such file %s\n",name_file );
    exit(1);
  }
  long record_size = sys_get_record_size(file);

  struct stat st;
  stat(name_file, &st);
  long long int size_of_file = st.st_size;

  if(size_of_file==0){
    printf("File is empty\n");
    return;
  }

  long records_number = size_of_file/record_size;
  sys_bubble_sort(file, &record_size, records_number);
  close(file);
}

long lib_get_record_size(FILE * file){
  char * buffor = (char *) calloc(64, sizeof(char));
  if (buffor == NULL) {
   return 0;
  }
  long size_of_record=0;
  char * ptr;
  while (  fread(buffor, sizeof(char), 64, file) >0) {
    for ( ptr=buffor; *ptr !='\0' && (*ptr)!='\n'; ptr++)size_of_record++;
    if ( *ptr !='\0' && *ptr == '\n') break;
  }
  size_of_record++;  //adds char '\n'
  fseek(file, 0, SEEK_SET);   //change location of read/write pointer to the beginning of file
  return size_of_record;
}

void lib_bubble_sort(FILE * file, long * record_size,long  records_number ) {
  bool flag = 1;
  char * record1 = (char *) calloc(*record_size, sizeof(char));
  char * record2 = (char *) calloc(*record_size, sizeof(char));

  while(flag){

    flag=false;
    fseek(file, 0  , SEEK_SET);
    fread(record1, sizeof(char), *record_size, file);

    for (long i= 1; i < records_number; i++) {
      fread(record2, sizeof(char), *record_size, file);
      if (record1[0]>record2[0]) {

        fseek(file, -2*(*record_size), SEEK_CUR);
        fwrite(record2, sizeof(char), *record_size, file);
        fwrite(record1, sizeof(char), *record_size, file);
        flag=true;
      } else {
        char * tmp_swap = record1;
        record1 = record2;
        record2 = tmp_swap;
      }
  }
  records_number--;
  }
  free(record1);
  free(record2);
}

void lib_sort_file(const char * name_file) {

  FILE * file = fopen(name_file, "r+");
  if (file == NULL) {
    printf("There is no such file %s\n",name_file );
    exit(1);
  }
  fseek(file, 0, SEEK_END);
	long int file_size = ftell(file);
  fseek(file, 0, SEEK_SET);

  long record_size = lib_get_record_size(file);
  if(file_size==0){
    printf("File is empty\n");
    return;
  }
  long records_number = file_size/record_size;

  lib_bubble_sort(file,&record_size,records_number);
  fclose(file);

}

void sys_shuffle_file(const char * name_file, int record_number, int record_size) {
  record_size++ ; //ads '\n'
  int file = open(name_file, O_RDWR);
  if (file < 0) {
    printf("There is no such file %s\n",name_file );
    exit(1);
  }

  srand(time(NULL));
  if (file < 0) {
    printf("There is no such file %s\n",name_file );
    exit(1);
  }
  char * record1 = (char *) calloc(record_size, sizeof(char));
  char * record2 = (char *) calloc(record_size, sizeof(char));
  //algorithm fisher-yates
  int j=0;
  for (int i = 0; i < record_number-1; i++) {
    lseek(file, i*record_size, SEEK_SET);
    read(file, record1, record_size);

    j=rand() %(record_number-i)+i;
    lseek(file, j*record_size, SEEK_SET);
    read(file,record2,record_size);

    lseek(file, i*record_size, SEEK_SET);
    write(file, record2, record_size);

    lseek(file, j*record_size, SEEK_SET);
    write(file, record1, record_size);

  }
  free(record1);
  free(record2);
}

void lib_shuffle_file(const char * name_file, int record_number, int record_size) {
   record_size++ ; //ads '\n';
  FILE * file =fopen(name_file, "r+");
  if (file == NULL) {
    printf("There is no such file %s\n",name_file );
    exit(1);
  }
  char * record1 = (char *) calloc(record_size, sizeof(char));
  char * record2 = (char *) calloc(record_size, sizeof(char));
  //algorithm fisher-yates
  int j=0;
  for (int i = 0; i < record_number-1; i++) {
    fseek(file, i*record_size, SEEK_SET);
    fread(record1, sizeof(char), record_size, file);

    j=rand() %(record_number-i)+i;
    fseek(file, j*record_size, SEEK_SET);
    fread(record2, sizeof(char), record_size, file);


    fseek(file, i*record_size, SEEK_SET);
    fwrite(record2, sizeof(char), record_size, file);

    fseek(file, j*record_size, SEEK_SET);
    fwrite(record1, sizeof(char), record_size, file);

  }
  free(record1);
  free(record2);
}

void init_measurement(struct timeval * userTmp, struct timeval * sysTmp, struct rusage * usage) {
  getrusage (RUSAGE_SELF, usage);
  (*userTmp)=(*usage).ru_utime;
  (*sysTmp)=(*usage).ru_stime;
}

void finish_measurement(struct rusage * usage) {
  getrusage (RUSAGE_SELF, usage);
}

void print_result(struct timeval * userTmp, struct timeval * systemTmp, struct rusage * usage) {
  printf ("CPU time: sec user: %ld.%09ld ,sec system  %ld.%09ld\n",
      (*usage).ru_utime.tv_sec-(*userTmp).tv_sec, (*usage).ru_utime.tv_usec-(*userTmp).tv_usec,
      (*usage).ru_stime.tv_sec-(*systemTmp).tv_sec, (*usage).ru_stime.tv_usec-(*systemTmp).tv_usec);

}

void parse_and_perform_action(int argc, const char *argv[]) {
  if (argc<3 || argc>7) {
    return incorrect_amount_arguments();
  }
  struct rusage usage;
  struct timeval userTmp,systemTmp;
  int i=1;
  init_measurement(&userTmp, &systemTmp, &usage);
  if (strcmp(argv[i], "generate") == 0) {
    if (argc != 5) {
      printf("Incorrect number of argument.\n" );
      printf("./program.out record_number record_size.\n" );
      return;
    }
    i++;
    if (is_integer(argv[++i])) {
      if (is_integer(argv[++i])) {
          generate_file(argv[2],atoi(argv[3]),atoi(argv[4]));
      } else {
        error_arg(i);
        print_not_int(i);
      }
    } else {
      error_arg(i);
      print_not_int(i);
    }

  }
  else if (strcmp(argv[i], "sys") == 0 || strcmp(argv[i], "lib") == 0) {
      if (strcmp(argv[++i], "shuffle")==0) {
        if(argc != 6) {
          return incorrect_amount_arguments();
        }
        else {
          i++;
          i++;
          if (is_integer(argv[i])) {
            if (is_integer(argv[++i])) {
              if (strcmp(argv[1], "sys") == 0) {
                sys_shuffle_file(argv[3],atoi(argv[4]),atoi(argv[5]));
              }
              else {
                lib_shuffle_file(argv[3],atoi(argv[4]),atoi(argv[5]));
              }
            } else {
              error_arg(i);
              print_not_int(i);
            }
          } else{
            error_arg(i);
            print_not_int(i);
          }
        }
      }
      else if (strcmp(argv[i], "sort") == 0) {
        if (strcmp(argv[1], "sys") == 0) {
          sys_sort_file(argv[++i]);
        }
        else if (strcmp(argv[1], "lib") == 0) {
          lib_sort_file(argv[++i]);
        }
        else {
          error_arg(i);
        }
      }
  }
  else {
    error_arg(i);
    print_introduction();
    return;
  }
    finish_measurement(&usage);
    print_result(&userTmp, &systemTmp, &usage);
}


int main(int argc, char const *argv[]) {

  parse_and_perform_action(argc,argv);
  return 0;
}
