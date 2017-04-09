#include <stdio.h>
#include <stdlib.h>

int main(int argc, char const *argv[]) {
  if (argc != 2) {
    fprintf(stderr, "Program requires one argument\n");
    return 1;
  }
  char *env;
  if ( (env =(getenv(argv[1]))) != NULL) {
    printf("%s\n", env);
  } else {
    fprintf(stderr, "No such environment variable: %s\n", argv[1]);
  }

  return 0;
}
