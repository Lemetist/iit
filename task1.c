#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define N 750
#define K 10

int main(void) {
  const double a = 100.0;
  const double b = 110.0;
  const double step = (b - a) / K;
  double numbers[N];
  int counts[K] = {0};

  srand((unsigned int)time(NULL));

  /* Формирование массива и подсчёт частот */
  for (int i = 0; i < N; i++) {
    numbers[i] = a + (b - a) * ((double)rand() / RAND_MAX);
    int index = (int)((numbers[i] - a) / step);

    /* Число b относится к последнему интервалу */
    if (index >= K) {
      index = K - 1;
    }
    counts[index]++;
  }

  printf("Histogram (one * = one number):\n");
  for (int i = 0; i < K; i++) {
    double left = a + i * step;
    double right = a + (i + 1) * step;
    printf("[%.1f; %.1f%c ", left, right, i == K - 1 ? ']' : ')');
    for (int j = 0; j < counts[i]; j++) {
      putchar('*');
    }
    printf(" (%d)\n", counts[i]);
  }

  /* Оценка отклонений от ожидаемой частоты */
  double expected = (double)N / K;
  double max_deviation = 0.0;
  for (int i = 0; i < K; i++) {
    double deviation = counts[i] - expected;
    if (deviation < 0) {
      deviation = -deviation;
    }
    if (deviation > max_deviation) {
      max_deviation = deviation;
    }
  }

  printf("\nExpected count per interval: %.0f\n", expected);
  printf("Maximum deviation: %.0f (%.1f%%)\n", max_deviation,
         max_deviation / expected * 100.0);
  return 0;
}
