#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define N 750

int main() {
  srand(time(0));

  int map[N];
  int max = 110;
  int min = 100;

  for (int i = 0; i < N; i++) {
    map[i] = min + rand() % (max - min + 1);
    printf("number %d\n", map[i]);
  }

  return 0;
}
