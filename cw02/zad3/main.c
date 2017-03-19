#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdbool.h>

void print_introduction() {
  printf("Opcje programu:\n" );
  printf("11 - ustawienie rygla do odczytu na wybrany znak pliku - wersja nieblokująca.\n");
  printf("12 - ustawienie rygla do odczytu na wybrany znak pliku - wersja blokująca.\n");
  printf("21 - ustawienie rygla do zapisu na wybrany znak pliku - wersja nieblokująca.\n");
  printf("22 - ustawienie rygla do zapisu na wybrany znak pliku - wersja blokująca.\n");
  printf("3  - wyświetlenie listy zaryglowanych znaków pliku (z informacją.\n");
  printf("numerze PID procesu będącego właścicielem rygla oraz jego typie - odczyt/zapis).\n");
  printf("4  - zwolnienie wybranego rygla.\n");
  printf("5  - odczyt (funkcją read) wybranego znaku pliku.\n");
  printf("6  - zmiana (funkcją write) wybranego znaku pliku.\n");
  printf("7  - zakonczenie programu.\n" );
}

struct  flock * create_flock(int lock_type,int char_number) {
  struct  flock * lock = (struct flock *) calloc(1, sizeof(lock));
  lock->l_type=lock_type;
  lock->l_whence=SEEK_SET;
  lock->l_start=char_number;
  lock->l_len=1;
  return lock;
}

void set_file_locking_instantly(int file,char * name_file,int char_number,int lock_type){
  struct flock * lock = create_flock(lock_type,char_number);
  if(fcntl(file,F_SETLK,lock ) == -1){
    free(lock);
    perror(name_file);
    return;
  }
  if (lock_type == F_UNLCK) {
    printf("Zwolnienie rygla - znak %d\n",char_number );
  }
  else {
    printf("Ustawienie rygla do %s znak: %d \n", (lock_type == F_RDLCK) ? "odczytu" : "zapisu" ,char_number );
  }
  free(lock);
}

void set_file_locking_waiting(int file,char * name_file,int char_number,int lock_type) {
  struct flock * lock = create_flock(lock_type,char_number);

  if(fcntl(file,F_SETLKW,lock )==-1){
    perror(name_file);
    free(lock);
    return;
  }

  printf("Ustawienie rygla do %s znak: %d \n", (lock_type == F_RDLCK) ? "odczytu" : "zapisu" ,char_number );
  free(lock);
}

void print_locked_char(int file){
  printf("Loki\n" );
  printf("-------\n" );

  long file_size=lseek(file,0,SEEK_END);
  long iterator = 0;
  lseek(file, 0, SEEK_SET);

  struct flock * lock = (struct flock *) malloc(sizeof(struct flock));
  do {

    lock->l_type   = F_WRLCK;
    lock->l_whence = SEEK_SET;
    lock->l_start  = iterator;
    lock->l_len    = 1;
    fcntl(file, F_GETLK,lock);
    if (lock->l_type == F_WRLCK) {
      printf("Lock do zapisu, znak: %ld\t PID:%d\n",iterator,lock->l_pid);
    }
    lock->l_type   = F_RDLCK;      /* read/write lock */
    lock->l_whence = SEEK_SET;     /* beginning of file */
    lock->l_start  = iterator;            /* offset from l_whence */
    lock->l_len    = 1;            /* length, 0 = to EOF */
    fcntl(file, F_GETLK,lock);
    if (lock->l_type == F_RDLCK) {
      printf("Lock do odczytu, znak: %ld\t PID:%d\n",iterator,lock->l_pid);
    }
    iterator++;
  } while(lseek(file, iterator, SEEK_SET) < file_size );

  free(lock);

}

void read_a_char(int file,int char_number) {
  char  c[2];
  lseek(file, char_number, SEEK_SET);
  read(file, &c, 1);
  printf("Odczytanie znaku na pozycji %d: %c\n",char_number, c[0]);


}

void write_a_char(int file,int char_number, char  c) {
  lseek(file, char_number, SEEK_SET);
  printf("Zapisanie znaku na pozycji %d: %c\n",char_number, c);
  write(file, &c, 1);
}

int main(int argc, char  *argv[]) {
  char * file_name = argv[1];
  int file = open(file_name,O_RDWR);
  if (file < 0) {
    perror(file_name);
    printf("No such file\n" );
    return(EXIT_FAILURE);
  }
  int  option ;
  char  arg;
  int char_number;
  print_introduction();


  while (true) {

    scanf("\n%d",&option);
    switch (option) {
      case 11:
        printf("Podaj bajt pliku do ryglowania\n" );
        scanf("\n%d",&char_number );
        set_file_locking_instantly(file,file_name,char_number,F_RDLCK);
      break;

      case 12:
        printf("Podaj bajt pliku do ryglowania\n" );
        scanf("\n%d",&char_number );
        set_file_locking_waiting(file,file_name,char_number,F_RDLCK)  ;
      break;

      case 21:
        printf("Podaj bajt pliku do ryglowania\n" );
        scanf("\n%d",&char_number );
        set_file_locking_instantly(file,file_name,char_number,F_WRLCK);
      break;
      case 22:
        printf("Podaj bajt pliku do ryglowania\n" );
        scanf("\n%d",&char_number );
        set_file_locking_waiting(file,file_name,char_number,F_WRLCK);
      break;

      case 3:
        print_locked_char(file);
      break;

      case 4:
        printf("Podaj bajt do odblokowania\n" );
        scanf("\n%d",&char_number );
        set_file_locking_instantly(file,file_name,char_number,F_UNLCK);
      break;

      case 5:
        printf("Podaj nr znaku do odczytania\n" );
        scanf("\n%d",&char_number );
        read_a_char(file,char_number);
      break;

      case 6:
        printf("Podaj nr znaku do zamiany oraz nowy znak\n" );
        scanf("\n%d %c", &char_number,&arg );
        write_a_char(file,char_number,arg);
      break;

      case 7:
        close(file);
        return (EXIT_SUCCESS);
      break;

      default:
        printf("Nieprawidlowy arguement - sprobuj ponownie\n");
      break;
    }
  }

}
